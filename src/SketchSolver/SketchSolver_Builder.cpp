// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:    SketchSolver_Builder.cpp
// Created: 25 Mar 2015
// Author:  Artem ZHIDKOV

#include "SketchSolver_Builder.h"
#include <SketchSolver_ConstraintCoincidence.h>
#include <SketchSolver_ConstraintDistance.h>
#include <SketchSolver_ConstraintEqual.h>
#include <SketchSolver_ConstraintFillet.h>
#include <SketchSolver_ConstraintLength.h>
#include <SketchSolver_ConstraintMirror.h>
#include <SketchSolver_ConstraintRigid.h>
#include <SketchSolver_ConstraintTangent.h>
#include <SketchSolver_ConstraintMultiRotation.h>
#include <SketchSolver_ConstraintMultiTranslation.h>
#include <SketchSolver_ConstraintMovement.h>
#include <SketchSolver_Error.h>

#include <GeomAPI_Edge.h>
#include <GeomDataAPI_Dir.h>
#include <GeomDataAPI_Point.h>
#include <GeomDataAPI_Point2D.h>
#include <Events_Error.h>
#include <ModelAPI_Attribute.h>
#include <ModelAPI_AttributeDouble.h>
#include <ModelAPI_AttributeRefAttr.h>
#include <ModelAPI_AttributeRefList.h>
#include <ModelAPI_ResultConstruction.h>

#include <SketchPlugin_Arc.h>
#include <SketchPlugin_Circle.h>
#include <SketchPlugin_Line.h>
#include <SketchPlugin_Point.h>
#include <SketchPlugin_ConstraintCoincidence.h>
#include <SketchPlugin_ConstraintDistance.h>
#include <SketchPlugin_ConstraintEqual.h>
#include <SketchPlugin_ConstraintFillet.h>
#include <SketchPlugin_ConstraintHorizontal.h>
#include <SketchPlugin_ConstraintLength.h>
#include <SketchPlugin_ConstraintMirror.h>
#include <SketchPlugin_ConstraintParallel.h>
#include <SketchPlugin_ConstraintPerpendicular.h>
#include <SketchPlugin_ConstraintRadius.h>
#include <SketchPlugin_ConstraintRigid.h>
#include <SketchPlugin_ConstraintTangent.h>
#include <SketchPlugin_ConstraintVertical.h>
#include <SketchPlugin_MultiRotation.h>
#include <SketchPlugin_MultiTranslation.h>

#include <math.h>

// Initialization of constraint builder self pointer
SketchSolver_Builder* SketchSolver_Builder::mySelf = 0;

SketchSolver_Builder* SketchSolver_Builder::getInstance()
{
  if (!mySelf)
    mySelf = new SketchSolver_Builder();
  return mySelf;
}

SolverConstraintPtr SketchSolver_Builder::createConstraint(ConstraintPtr theConstraint)
{
  SolverConstraintPtr aResult;
  DataPtr aData = theConstraint->data();
  if (!aData || !aData->isValid())
    return aResult;

#ifdef _DEBUG
  // Verify attributes of constraint and generate errors
  std::list<AttributePtr> anAttrList = aData->attributes(std::string());
  std::list<AttributePtr>::iterator anIter = anAttrList.begin();
  for (; anIter != anAttrList.end(); anIter++) {
    AttributeRefAttrPtr aRefAttr = std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(*anIter);
    if (aRefAttr) {
      if (aRefAttr->isObject() && aRefAttr->object()) {
        ResultConstructionPtr aRC =
            std::dynamic_pointer_cast<ModelAPI_ResultConstruction>(aRefAttr->object());
        if (!aRC)
          Events_Error::send(SketchSolver_Error::NEED_OBJECT_NOT_FEATURE(), this);
      }
      continue;
    }
    AttributeRefListPtr aRefList = std::dynamic_pointer_cast<ModelAPI_AttributeRefList>(*anIter);
    if (aRefList) {
      std::list<ObjectPtr> aList = aRefList->list();
      std::list<ObjectPtr>::iterator aListIter = aList.begin();
      for (; aListIter != aList.end(); aListIter++) {
        ResultConstructionPtr aRC =
            std::dynamic_pointer_cast<ModelAPI_ResultConstruction>(*aListIter);
        if (*aListIter && !aRC)
          Events_Error::send(SketchSolver_Error::NEED_OBJECT_NOT_FEATURE(), this);
      }
    }
  }
#endif

  if (theConstraint->getKind() == SketchPlugin_ConstraintCoincidence::ID()) {
    return SolverConstraintPtr(new SketchSolver_ConstraintCoincidence(theConstraint));
  } else if (theConstraint->getKind() == SketchPlugin_ConstraintDistance::ID()) {
    return SolverConstraintPtr(new SketchSolver_ConstraintDistance(theConstraint));
  } else if (theConstraint->getKind() == SketchPlugin_ConstraintEqual::ID()) {
    return SolverConstraintPtr(new SketchSolver_ConstraintEqual(theConstraint));
  } else if (theConstraint->getKind() == SketchPlugin_ConstraintFillet::ID()) {
    return SolverConstraintPtr(new SketchSolver_ConstraintFillet(theConstraint));
  } else if (theConstraint->getKind() == SketchPlugin_ConstraintHorizontal::ID()) {
    return SolverConstraintPtr(new SketchSolver_ConstraintHorizontal(theConstraint));
  } else if (theConstraint->getKind() == SketchPlugin_ConstraintLength::ID()) {
    return SolverConstraintPtr(new SketchSolver_ConstraintLength(theConstraint));
  } else if (theConstraint->getKind() == SketchPlugin_ConstraintMirror::ID()) {
    return SolverConstraintPtr(new SketchSolver_ConstraintMirror(theConstraint));
  } else if (theConstraint->getKind() == SketchPlugin_ConstraintParallel::ID()) {
    return SolverConstraintPtr(new SketchSolver_ConstraintParallel(theConstraint));
  } else if (theConstraint->getKind() == SketchPlugin_ConstraintPerpendicular::ID()) {
    return SolverConstraintPtr(new SketchSolver_ConstraintPerpendicular(theConstraint));
  } else if (theConstraint->getKind() == SketchPlugin_ConstraintRadius::ID()) {
    return SolverConstraintPtr(new SketchSolver_ConstraintRadius(theConstraint));
  } else if (theConstraint->getKind() == SketchPlugin_ConstraintTangent::ID()) {
    return SolverConstraintPtr(new SketchSolver_ConstraintTangent(theConstraint));
  } else if (theConstraint->getKind() == SketchPlugin_ConstraintVertical::ID()) {
    return SolverConstraintPtr(new SketchSolver_ConstraintVertical(theConstraint));
  } else if (theConstraint->getKind() == SketchPlugin_ConstraintRigid::ID()) {
    return SolverConstraintPtr(new SketchSolver_ConstraintRigid(theConstraint));
  } else if (theConstraint->getKind() == SketchPlugin_MultiTranslation::ID()) {
    return SolverConstraintPtr(new SketchSolver_ConstraintMultiTranslation(theConstraint));
  } else if (theConstraint->getKind() == SketchPlugin_MultiRotation::ID()) {
    return SolverConstraintPtr(new SketchSolver_ConstraintMultiRotation(theConstraint));
  }
  return aResult;
}

SolverConstraintPtr SketchSolver_Builder::createRigidConstraint(FeaturePtr theFixedFeature)
{
  DataPtr aData = theFixedFeature->data();
  if (!aData || !aData->isValid())
    return SolverConstraintPtr();
  return SolverConstraintPtr(new SketchSolver_ConstraintRigid(theFixedFeature));
}

SolverConstraintPtr SketchSolver_Builder::createMovementConstraint(FeaturePtr theFixedFeature)
{
  DataPtr aData = theFixedFeature->data();
  if (!aData || !aData->isValid())
    return SolverConstraintPtr();
  return SolverConstraintPtr(new SketchSolver_ConstraintMovement(theFixedFeature));
}



bool SketchSolver_Builder::createWorkplane(
    CompositeFeaturePtr theSketch,
    std::vector<Slvs_Entity>& theEntities,
    std::vector<Slvs_Param>& theParameters)
{
  DataPtr aSketchData = theSketch->data();
  if (!aSketchData || !aSketchData->isValid())
    return false; // the sketch is incorrect

  // Get parameters of workplane
  std::shared_ptr<ModelAPI_Attribute> aDirX = aSketchData->attribute(
      SketchPlugin_Sketch::DIRX_ID());
  std::shared_ptr<ModelAPI_Attribute> aNorm = aSketchData->attribute(
      SketchPlugin_Sketch::NORM_ID());
  std::shared_ptr<ModelAPI_Attribute> anOrigin = aSketchData->attribute(
      SketchPlugin_Sketch::ORIGIN_ID());
  // Create SolveSpace entity corresponding to the sketch origin
  if (!createEntity(anOrigin, theEntities, theParameters))
    return false;
  Slvs_hEntity anOriginID = theEntities.back().h;
  // Create SolveSpace entity corresponding the the sketch normal
  if (!createNormal(aNorm, aDirX, theEntities, theParameters))
    return false;
  Slvs_hEntity aNormalID = theEntities.back().h;

  // Create workplane
  Slvs_hEntity aWorkplaneID = theEntities.back().h + 1;
  Slvs_Entity aWorkplane = Slvs_MakeWorkplane(aWorkplaneID, SLVS_G_UNKNOWN, anOriginID, aNormalID);
  theEntities.push_back(aWorkplane);
  return true;
}

bool SketchSolver_Builder::createEntity(
    AttributePtr theAttribute,
    std::vector<Slvs_Entity>& theEntities,
    std::vector<Slvs_Param>& theParameters)
{
  Slvs_hEntity anEntID = theEntities.empty() ? 0 : theEntities.back().h;
  Slvs_hParam aParamID = theParameters.empty() ? 0 : theParameters.back().h;

  // Point in 3D
  std::shared_ptr<GeomDataAPI_Point> aPoint =
      std::dynamic_pointer_cast<GeomDataAPI_Point>(theAttribute);
  if (aPoint) {
    theParameters.push_back(Slvs_MakeParam(++aParamID, SLVS_G_UNKNOWN, aPoint->x()));
    theParameters.push_back(Slvs_MakeParam(++aParamID, SLVS_G_UNKNOWN, aPoint->y()));
    theParameters.push_back(Slvs_MakeParam(++aParamID, SLVS_G_UNKNOWN, aPoint->z()));
    theEntities.push_back(Slvs_MakePoint3d(++anEntID, SLVS_G_UNKNOWN,
        aParamID-2, aParamID-1, aParamID));
    return true;
  }
  // Point in 2D
  std::shared_ptr<GeomDataAPI_Point2D> aPoint2D =
    std::dynamic_pointer_cast<GeomDataAPI_Point2D>(theAttribute);
  if (aPoint2D) {
    theParameters.push_back(Slvs_MakeParam(++aParamID, SLVS_G_UNKNOWN, aPoint2D->x()));
    theParameters.push_back(Slvs_MakeParam(++aParamID, SLVS_G_UNKNOWN, aPoint2D->y()));
    theEntities.push_back(Slvs_MakePoint2d(++anEntID, SLVS_G_UNKNOWN, SLVS_E_UNKNOWN,
      aParamID-1, aParamID));
    return true;
  }
  // Scalar value (used for the distance entities)
  AttributeDoublePtr aScalar = std::dynamic_pointer_cast<ModelAPI_AttributeDouble>(theAttribute);
  if (aScalar) {
    theParameters.push_back(Slvs_MakeParam(++aParamID, SLVS_G_UNKNOWN, aScalar->value()));
    theEntities.push_back(Slvs_MakeDistance(++anEntID, SLVS_G_UNKNOWN,
      SLVS_E_UNKNOWN, aParamID));
    return true;
  }
  // unknown attribute type
  return false;
}

bool SketchSolver_Builder::createEntity(
    FeaturePtr theFeature,
    std::vector<Slvs_Entity>& theEntities,
    std::vector<Slvs_Param>& theParameters)
{
  if (!theFeature->data()->isValid())
    return false;

  // SketchPlugin features
  std::shared_ptr<SketchPlugin_Feature> aFeature = std::dynamic_pointer_cast<
      SketchPlugin_Feature>(theFeature);
  if (!aFeature)
    return false;

  // Verify the feature by its kind
  const std::string& aFeatureKind = aFeature->getKind();
  DataPtr aData = aFeature->data();
  // Line
  if (aFeatureKind == SketchPlugin_Line::ID()) {
    AttributePtr aStart = aData->attribute(SketchPlugin_Line::START_ID());
    AttributePtr aEnd = aData->attribute(SketchPlugin_Line::END_ID());
    if (!aStart->isInitialized() || !aEnd->isInitialized())
      return false;
    if (!createEntity(aStart, theEntities, theParameters) ||
        !createEntity(aEnd, theEntities, theParameters))
      return false;
    Slvs_hEntity aLineID = theEntities.back().h + 1;
    theEntities.push_back(Slvs_MakeLineSegment(aLineID, SLVS_G_UNKNOWN, SLVS_E_UNKNOWN,
        aLineID-2, aLineID-1));
  }
  // Circle
  else if (aFeatureKind == SketchPlugin_Circle::ID()) {
    AttributePtr aCenter = aData->attribute(SketchPlugin_Circle::CENTER_ID());
    AttributePtr aRadius = aData->attribute(SketchPlugin_Circle::RADIUS_ID());
    if (!aCenter->isInitialized() || !aRadius->isInitialized())
      return false;
    if (!createEntity(aCenter, theEntities, theParameters) ||
        !createEntity(aRadius, theEntities, theParameters))
      return false;
    Slvs_hEntity aCircID = theEntities.back().h;
    theEntities.push_back(Slvs_MakeCircle(aCircID, SLVS_G_UNKNOWN, SLVS_E_UNKNOWN, aCircID-2,
      SLVS_E_UNKNOWN, aCircID-1));
  }
  // Arc
  else if (aFeatureKind == SketchPlugin_Arc::ID()) {
    AttributePtr aCenter = aData->attribute(SketchPlugin_Arc::CENTER_ID());
    AttributePtr aStart = aData->attribute(SketchPlugin_Arc::START_ID());
    AttributePtr aEnd = aData->attribute(SketchPlugin_Arc::END_ID());
    if (!aCenter->isInitialized() || !aStart->isInitialized() || !aEnd->isInitialized())
      return false;
    if (!createEntity(aCenter, theEntities, theParameters) ||
        !createEntity(aStart, theEntities, theParameters) ||
        !createEntity(aEnd, theEntities, theParameters))
      return false;
    Slvs_hEntity anArcID = theEntities.back().h;
    theEntities.push_back(Slvs_MakeArcOfCircle(anArcID, SLVS_G_UNKNOWN, SLVS_E_UNKNOWN,
        SLVS_E_UNKNOWN, anArcID-3, anArcID-2, anArcID-1));
  }
  // Point (it has low probability to be an attribute of constraint, so it is checked at the end)
  else if (aFeatureKind == SketchPlugin_Point::ID()) {
    AttributePtr aPoint = aData->attribute(SketchPlugin_Point::COORD_ID());
    if (!aPoint->isInitialized() ||
        !createEntity(aPoint, theEntities, theParameters))
      return false;
    // Both the sketch point and its attribute (coordinates) link to the same SolveSpace point identifier.
    // No need to add another entity.
  }
  return true;
}

bool SketchSolver_Builder::createNormal(
    AttributePtr theNormal,
    AttributePtr theDirX,
    std::vector<Slvs_Entity>& theEntities,
    std::vector<Slvs_Param>& theParameters)
{
  std::shared_ptr<GeomDataAPI_Dir> aNorm = std::dynamic_pointer_cast<GeomDataAPI_Dir>(theNormal);
  std::shared_ptr<GeomDataAPI_Dir> aDirX = std::dynamic_pointer_cast<GeomDataAPI_Dir>(theDirX);
  if (!aDirX || (fabs(aDirX->x()) + fabs(aDirX->y()) + fabs(aDirX->z()) < tolerance) || 
      !aNorm->isInitialized())
    return false;
  // calculate Y direction
  std::shared_ptr<GeomAPI_Dir> aDirY(new GeomAPI_Dir(aNorm->dir()->cross(aDirX->dir())));

  // quaternion parameters of normal vector
  double qw, qx, qy, qz;
  Slvs_MakeQuaternion(aDirX->x(), aDirX->y(), aDirX->z(), aDirY->x(), aDirY->y(), aDirY->z(), &qw,
                      &qx, &qy, &qz);
  double aNormCoord[4] = { qw, qx, qy, qz };

  // Create parameters of the normal
  Slvs_hParam aCurParam = theParameters.back().h;
  for (int i = 0; i < 4; i++)
    theParameters.push_back(Slvs_MakeParam(++aCurParam, SLVS_G_UNKNOWN, aNormCoord[i]));

  // Create a normal
  Slvs_hEntity aCurEntity = theEntities.back().h + 1;
  Slvs_Entity aNormal = Slvs_MakeNormal3d(aCurEntity, SLVS_G_UNKNOWN,
      aCurParam-3, aCurParam-2, aCurParam-1, aCurParam);
  theEntities.push_back(aNormal);
  return true;
}
