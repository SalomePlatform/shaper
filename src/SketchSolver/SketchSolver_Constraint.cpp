// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:    SketchSolver_Constraint.cpp
// Created: 27 May 2014
// Author:  Artem ZHIDKOV

#include "SketchSolver_Constraint.h"
#include <SketchSolver_Solver.h>

#include <SketchPlugin_Line.h>
#include <SketchPlugin_Point.h>
#include <SketchPlugin_Circle.h>
#include <SketchPlugin_Arc.h>
#include <SketchPlugin_ConstraintCoincidence.h>
#include <SketchPlugin_ConstraintDistance.h>
#include <SketchPlugin_ConstraintEqual.h>
#include <SketchPlugin_ConstraintHorizontal.h>
#include <SketchPlugin_ConstraintLength.h>
#include <SketchPlugin_ConstraintMirror.h>
#include <SketchPlugin_ConstraintParallel.h>
#include <SketchPlugin_ConstraintPerpendicular.h>
#include <SketchPlugin_ConstraintRadius.h>
#include <SketchPlugin_ConstraintRigid.h>
#include <SketchPlugin_ConstraintTangent.h>
#include <SketchPlugin_ConstraintVertical.h>

#include <ModelAPI_AttributeRefAttr.h>
#include <ModelAPI_AttributeRefList.h>
#include <ModelAPI_Data.h>
#include <ModelAPI_Document.h>
#include <ModelAPI_Object.h>
#include <ModelAPI_ResultConstruction.h>

#include <GeomDataAPI_Point.h>
#include <GeomDataAPI_Point2D.h>
#include <GeomAPI_Edge.h>
#include <GeomAPI_Shape.h>

/// Possible types of attributes (used to determine constraint type)
enum AttrType
{
  UNKNOWN,  // Something wrong during type determination
  POINT2D,
  POINT3D,
  LINE,
  CIRCLE,
  ARC
};

/// Calculate type of the attribute
static AttrType typeOfAttribute(std::shared_ptr<ModelAPI_Attribute> theAttribute);

SketchSolver_Constraint::SketchSolver_Constraint()
    : myConstraint(std::shared_ptr<SketchPlugin_Constraint>()),
      myType(SLVS_C_UNKNOWN),
      myAttributesList()
{
}

SketchSolver_Constraint::SketchSolver_Constraint(
    std::shared_ptr<SketchPlugin_Constraint> theConstraint)
    : myConstraint(theConstraint),
      myAttributesList()
{
  myType = getType(myConstraint);
}

const int& SketchSolver_Constraint::getType(
    std::shared_ptr<SketchPlugin_Constraint> theConstraint)
{
  myType = SLVS_C_UNKNOWN;
  if (!theConstraint)
    return getType();

  DataPtr aConstrData = theConstraint->data();
  if (!aConstrData || !aConstrData->isValid())
    return getType();

  // Assign empty names of attributes
  myAttributesList.clear();
  for (int i = 0; i < CONSTRAINT_ATTR_SIZE; i++)
    myAttributesList.push_back(std::string());

  const std::string& aConstraintKind = theConstraint->getKind();
  // Constraint for coincidence of two points
  if (aConstraintKind.compare(SketchPlugin_ConstraintCoincidence::ID()) == 0) {
    int anAttrPos = 0;
    // Verify the constraint has only two attributes and they are points
    int aPt2d = 0;  // bit-mapped field, each bit indicates whether the attribute is 2D point
    int aPt3d = 0;  // bit-mapped field, the same information for 3D points
    for (unsigned int indAttr = 0; indAttr < CONSTRAINT_ATTR_SIZE; indAttr++) {
      std::shared_ptr<ModelAPI_Attribute> anAttr = 
          aConstrData->attribute(SketchPlugin_Constraint::ATTRIBUTE(indAttr));
      if (!anAttr)
        continue;
      switch (typeOfAttribute(anAttr)) {
        case POINT2D:  // the attribute is a 2D point
          aPt2d |= (1 << indAttr);
          myAttributesList[anAttrPos++] = SketchPlugin_Constraint::ATTRIBUTE(indAttr);
          break;
        case POINT3D:  // the attribute is a 3D point
          aPt3d |= (1 << indAttr);
          myAttributesList[anAttrPos++] = SketchPlugin_Constraint::ATTRIBUTE(indAttr);
          break;
        default:
          // Attribute neither 2D nor 3D point is not supported by this type of constraint
          return getType();
      }
    }
    // The constrained points should be in first and second positions,
    // so the expected value of aPt2d or aPt3d is 3
    if ((aPt2d == 3 && aPt3d == 0) || (aPt2d == 0 && aPt3d == 3))
      myType = SLVS_C_POINTS_COINCIDENT;
    // Constraint parameters are wrong
    return getType();
  }

  // Constraint for distance between point and another entity
  if (aConstraintKind.compare(SketchPlugin_ConstraintDistance::ID()) == 0) {
    int aNbPoints = 0;
    int aNbEntities = 0;
    for (unsigned int indAttr = 0; indAttr < CONSTRAINT_ATTR_SIZE; indAttr++) {
      std::shared_ptr<ModelAPI_Attribute> anAttr = 
          aConstrData->attribute(SketchPlugin_Constraint::ATTRIBUTE(indAttr));
      switch (typeOfAttribute(anAttr)) {
        case POINT2D:
        case POINT3D:
          myAttributesList[aNbPoints++] = SketchPlugin_Constraint::ATTRIBUTE(indAttr);
          break;
        case LINE:
          // entities are placed starting from SketchPlugin_Constraint::ENTITY_C() attribute
          myAttributesList[2 + aNbEntities++] = SketchPlugin_Constraint::ATTRIBUTE(indAttr);
          myType = SLVS_C_PT_LINE_DISTANCE;
          break;
      }
    }
    // Verify the correctness of constraint arguments
    if (aNbPoints == 2 && aNbEntities == 0)
      myType = SLVS_C_PT_PT_DISTANCE;
    else if (aNbPoints != 1 || aNbEntities != 1)
      myType = SLVS_C_UNKNOWN;
    return getType();
  }

  // Constraint for the given length of a line
  if (aConstraintKind.compare(SketchPlugin_ConstraintLength::ID()) == 0) {
    int aNbLines = 0;
    for (unsigned int indAttr = 0; indAttr < CONSTRAINT_ATTR_SIZE; indAttr++) {
      std::shared_ptr<ModelAPI_Attribute> anAttr = 
          aConstrData->attribute(SketchPlugin_Constraint::ATTRIBUTE(indAttr));
      if (typeOfAttribute(anAttr) == LINE)
        myAttributesList[aNbLines++] = SketchPlugin_Constraint::ATTRIBUTE(indAttr);
    }
    if (aNbLines == 1)
      myType = SLVS_C_PT_PT_DISTANCE;
    return getType();
  }

  // Constraint for two parallel/perpendicular lines
  bool isParallel = (aConstraintKind.compare(SketchPlugin_ConstraintParallel::ID()) == 0);
  bool isPerpendicular = (aConstraintKind.compare(SketchPlugin_ConstraintPerpendicular::ID()) == 0);
  if (isParallel || isPerpendicular) {
    int aNbEntities = 2;  // lines in SolveSpace constraints should start from SketchPlugin_Constraint::ENTITY_C() attribute
    for (unsigned int indAttr = 0; indAttr < CONSTRAINT_ATTR_SIZE; indAttr++) {
      std::shared_ptr<ModelAPI_Attribute> anAttr = 
          aConstrData->attribute(SketchPlugin_Constraint::ATTRIBUTE(indAttr));
      if (typeOfAttribute(anAttr) == LINE)
        myAttributesList[aNbEntities++] = SketchPlugin_Constraint::ATTRIBUTE(indAttr);
    }
    if (aNbEntities == 4)
      myType = isParallel ? SLVS_C_PARALLEL : SLVS_C_PERPENDICULAR;
    return getType();
  }

  // Constraint for radius of a circle or an arc of circle
  if (aConstraintKind.compare(SketchPlugin_ConstraintRadius::ID()) == 0) {
    int aNbEntities = 2;  // lines in SolveSpace constraints should started from SketchPlugin_Constraint::ENTITY_C() attribute
    for (unsigned int indAttr = 0; indAttr < CONSTRAINT_ATTR_SIZE; indAttr++) {
      std::shared_ptr<ModelAPI_Attribute> anAttr = 
          aConstrData->attribute(SketchPlugin_Constraint::ATTRIBUTE(indAttr));
      AttrType aType = typeOfAttribute(anAttr);
      if (aType == CIRCLE || aType == ARC)
        myAttributesList[aNbEntities++] = SketchPlugin_Constraint::ATTRIBUTE(indAttr);
    }
    if (aNbEntities == 3)
      myType = SLVS_C_DIAMETER;
    return getType();
  }

  // Constraint for fixed entity
  if (aConstraintKind.compare(SketchPlugin_ConstraintRigid::ID()) == 0) {
    // Verify that only one entity is filled
    int aNbAttrs = 0;
    for (unsigned int indAttr = 0; indAttr < CONSTRAINT_ATTR_SIZE; indAttr++) {
      std::shared_ptr<ModelAPI_Attribute> anAttr = 
          aConstrData->attribute(SketchPlugin_Constraint::ATTRIBUTE(indAttr));
      AttrType aType = typeOfAttribute(anAttr);
      if (aType != UNKNOWN)
        myAttributesList[aNbAttrs++] = SketchPlugin_Constraint::ATTRIBUTE(indAttr);
    }
    if (aNbAttrs == 1)
      myType = SLVS_C_WHERE_DRAGGED;
    return getType();
  }

  // Constraint for horizontal/vertical line
  bool isHorizontal = (aConstraintKind.compare(SketchPlugin_ConstraintHorizontal::ID()) == 0);
  bool isVertical = (aConstraintKind.compare(SketchPlugin_ConstraintVertical::ID()) == 0);
  if (isHorizontal || isVertical) {
    int aNbEntities = 2;  // lines in SolveSpace constraints should start from SketchPlugin_Constraint::ENTITY_C() attribute
    for (unsigned int indAttr = 0; indAttr < CONSTRAINT_ATTR_SIZE; indAttr++) {
      std::shared_ptr<ModelAPI_Attribute> anAttr = 
          aConstrData->attribute(SketchPlugin_Constraint::ATTRIBUTE(indAttr));
      if (typeOfAttribute(anAttr) == LINE)
        myAttributesList[aNbEntities++] = SketchPlugin_Constraint::ATTRIBUTE(indAttr);
    }
    if (aNbEntities == 3)
      myType = isHorizontal ? SLVS_C_HORIZONTAL : SLVS_C_VERTICAL;
    return getType();
  }

  if (aConstraintKind.compare(SketchPlugin_ConstraintEqual::ID()) == 0) {
    static const int aConstrType[3] = {
        SLVS_C_EQUAL_RADIUS,
        SLVS_C_EQUAL_LINE_ARC_LEN,
        SLVS_C_EQUAL_LENGTH_LINES
    };
    int aNbLines = 0;
    int aNbEntities = 2;  // lines and circles in SolveSpace constraints should start from SketchPlugin_Constraint::ENTITY_C() attribute
    for (unsigned int indAttr = 0; indAttr < CONSTRAINT_ATTR_SIZE; indAttr++) {
      std::shared_ptr<ModelAPI_Attribute> anAttr = 
          aConstrData->attribute(SketchPlugin_Constraint::ATTRIBUTE(indAttr));
      AttrType aType = typeOfAttribute(anAttr);
      if (aType == LINE) {
        myAttributesList[aNbEntities++] = SketchPlugin_Constraint::ATTRIBUTE(indAttr);
        aNbLines++;
      }
      else if (aType == CIRCLE || aType == ARC)
        myAttributesList[aNbEntities++] = SketchPlugin_Constraint::ATTRIBUTE(indAttr);
    }
    if (aNbEntities == 4)
      myType = aConstrType[aNbLines];
    return getType();
  }

  if (aConstraintKind.compare(SketchPlugin_ConstraintTangent::ID()) == 0) {
    static const int anArcPosDefault = 2;
    static const int aLinePosDefault = 3;
    int anArcPos = anArcPosDefault; // arc in tangency constraint should be placed before line
    int aLinePos = aLinePosDefault;
    for (unsigned int indAttr = 0; indAttr < CONSTRAINT_ATTR_SIZE; indAttr++) {
      std::shared_ptr<ModelAPI_Attribute> anAttr = 
          aConstrData->attribute(SketchPlugin_Constraint::ATTRIBUTE(indAttr));
      AttrType aType = typeOfAttribute(anAttr);
      if (aType == LINE && aLinePos < CONSTRAINT_ATTR_SIZE)
        myAttributesList[aLinePos++] = SketchPlugin_Constraint::ATTRIBUTE(indAttr);
      else if (aType == ARC)
        myAttributesList[anArcPos++] = SketchPlugin_Constraint::ATTRIBUTE(indAttr);
    }
    if (anArcPos - anArcPosDefault + aLinePos - aLinePosDefault == 2)
      myType = aLinePos > 3 ? SLVS_C_ARC_LINE_TANGENT : SLVS_C_CURVE_CURVE_TANGENT;
    return getType();
  }

  if (aConstraintKind.compare(SketchPlugin_ConstraintMirror::ID()) == 0) {
    int aNbAttrs = 0;
    bool hasMirrorLine = false;
    for (unsigned int indAttr = 0; indAttr < CONSTRAINT_ATTR_SIZE; indAttr++) {
      AttributeRefListPtr anAttrRefList = std::dynamic_pointer_cast<ModelAPI_AttributeRefList>(
          aConstrData->attribute(SketchPlugin_Constraint::ATTRIBUTE(indAttr)));
      if (anAttrRefList) {
        aNbAttrs++;
        myAttributesList[aNbAttrs] = SketchPlugin_Constraint::ATTRIBUTE(indAttr);
      }
      else {
        std::shared_ptr<ModelAPI_Attribute> anAttr = 
            aConstrData->attribute(SketchPlugin_Constraint::ATTRIBUTE(indAttr));
        if (typeOfAttribute(anAttr) == LINE) {
          hasMirrorLine = !hasMirrorLine;
          myAttributesList[0] = SketchPlugin_Constraint::ATTRIBUTE(indAttr);
        }
      }
    }
    if (aNbAttrs == 2 && hasMirrorLine)
      myType = SLVS_C_SYMMETRIC_LINE;
    return getType();
  }

  /// \todo Implement other kind of constraints

  return getType();
}

// ================= Auxiliary functions ==============================
AttrType typeOfAttribute(std::shared_ptr<ModelAPI_Attribute> theAttribute)
{
  std::shared_ptr<ModelAPI_AttributeRefAttr> anAttrRef = std::dynamic_pointer_cast<
      ModelAPI_AttributeRefAttr>(theAttribute);
  if (!anAttrRef)
    return UNKNOWN;

  if (anAttrRef->isObject()) {
    ResultConstructionPtr aRC = std::dynamic_pointer_cast<ModelAPI_ResultConstruction>(
        anAttrRef->object());
    if (!aRC || !aRC->shape())
      return UNKNOWN;

    if (aRC->shape()->isVertex())
      return POINT3D;
    else if (aRC->shape()->isEdge()) {
      std::shared_ptr<GeomAPI_Edge> anEdge = std::dynamic_pointer_cast<GeomAPI_Edge>(
          aRC->shape());
      if (anEdge->isLine())
        return LINE;
      else if (anEdge->isCircle())
        return CIRCLE;
      else if (anEdge->isArc())
        return ARC;
    }
  } else {
    if (anAttrRef->attr().get() != NULL) {
      const std::string aType = anAttrRef->attr()->attributeType();
      if (aType == GeomDataAPI_Point2D::type())
        return POINT2D;
      if (aType == GeomDataAPI_Point2D::type())
        return POINT2D;
    }
  }

  return UNKNOWN;
}

