// File:    SketchSolver_Constraint.cpp
// Created: 27 May 2014
// Author:  Artem ZHIDKOV

#include "SketchSolver_Constraint.h"
#include <SketchSolver_Solver.h>

#include <SketchPlugin_Line.h>
#include <SketchPlugin_Point.h>
#include <SketchPlugin_Circle.h>
#include <SketchPlugin_Arc.h>
#include <SketchPlugin_ConstraintDistance.h>
#include <SketchPlugin_ConstraintLength.h>
#include <SketchPlugin_ConstraintParallel.h>
#include <SketchPlugin_ConstraintPerpendicular.h>
#include <SketchPlugin_ConstraintRadius.h>

#include <ModelAPI_AttributeRefAttr.h>
#include <ModelAPI_Data.h>

#include <GeomDataAPI_Point.h>
#include <GeomDataAPI_Point2D.h>


SketchSolver_Constraint::SketchSolver_Constraint()
  : myConstraint(boost::shared_ptr<SketchPlugin_Constraint>()),
    myType(SLVS_C_UNKNOWN),
    myAttributesList()
{
}

SketchSolver_Constraint::SketchSolver_Constraint(
                boost::shared_ptr<SketchPlugin_Constraint> theConstraint)
  : myConstraint(theConstraint),
    myAttributesList()
{
  myType = getType(myConstraint);
}

const int& SketchSolver_Constraint::getType(boost::shared_ptr<SketchPlugin_Constraint> theConstraint)
{
  myType = SLVS_C_UNKNOWN;
  if (!theConstraint)
    return getType();

  // Assign empty names of attributes
  myAttributesList.clear();
  for (int i = 0; i < CONSTRAINT_ATTR_SIZE; i++)
    myAttributesList.push_back(std::string());

  const std::string& aConstraintKind = theConstraint->getKind();
  // Constraint for coincidence of two points
  if (aConstraintKind.compare("SketchConstraintCoincidence") == 0)
  {
    int anAttrPos = 0;
    // Verify the constraint has only two attributes and they are points
    int aPt2d = 0; // bit-mapped field, each bit indicates whether the attribute is 2D point
    int aPt3d = 0; // bit-mapped field, the same information for 3D points
    for (unsigned int indAttr = 0; indAttr < CONSTRAINT_ATTR_SIZE; indAttr++)
    {
      boost::shared_ptr<ModelAPI_AttributeRefAttr> anAttr =
        boost::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(
          theConstraint->data()->attribute(CONSTRAINT_ATTRIBUTES[indAttr])
        );
      if (!anAttr) continue;
      // Verify the attribute is a 2D point
      boost::shared_ptr<GeomDataAPI_Point2D> aPoint2D =
        boost::dynamic_pointer_cast<GeomDataAPI_Point2D>(anAttr->attr());
      if (aPoint2D)
      {
        aPt2d |= (1 << indAttr);
        myAttributesList[anAttrPos++] = CONSTRAINT_ATTRIBUTES[indAttr];
        continue;
      }
      // Verify the attribute is a 3D point
      boost::shared_ptr<GeomDataAPI_Point> aPoint3D =
        boost::dynamic_pointer_cast<GeomDataAPI_Point>(anAttr->attr());
      if (aPoint3D)
      {
        aPt3d |= (1 << indAttr);
        myAttributesList[anAttrPos++] = CONSTRAINT_ATTRIBUTES[indAttr];
        continue;
      }
      // Attribute neither 2D nor 3D point is not supported by this type of constraint
      return getType();
    }
    // The constrained points should be in first and second positions,
    // so the expected value of aPt2d or aPt3d is 3
    if ((aPt2d == 3 && aPt3d == 0) || (aPt2d == 0 && aPt3d == 3))
      myType = SLVS_C_POINTS_COINCIDENT;
    // Constraint parameters are wrong
    return getType();
  }

  // Constraint for distance between point and another entity
  if (aConstraintKind.compare(SKETCH_CONSTRAINT_DISTANCE_KIND) == 0)
  {
    int aNbPoints = 0;
    int aNbEntities = 0;
    for (unsigned int indAttr = 0; indAttr < CONSTRAINT_ATTR_SIZE; indAttr++)
    {
      boost::shared_ptr<ModelAPI_AttributeRefAttr> anAttr =
        boost::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(
          theConstraint->data()->attribute(CONSTRAINT_ATTRIBUTES[indAttr])
        );
      if (!anAttr) continue;
      if (anAttr->isFeature() && anAttr->feature())
      { // verify posiible entities
        const std::string& aKind = anAttr->feature()->getKind();
        if (aKind.compare(SKETCH_POINT_KIND) == 0)
        {
          myAttributesList[aNbPoints++] = CONSTRAINT_ATTRIBUTES[indAttr];
          continue;
        }
        else if(aKind.compare(SKETCH_LINE_KIND) == 0)
        {
          // entities are placed starting from CONSTRAINT_ATTR_ENTITY_C attribute
          myAttributesList[2 + aNbEntities++] = CONSTRAINT_ATTRIBUTES[indAttr];
          myType = SLVS_C_PT_LINE_DISTANCE;
          continue;
        }
      }
      else
      { // verify points
        // Verify the attribute is a 2D point
        boost::shared_ptr<GeomDataAPI_Point2D> aPoint2D =
          boost::dynamic_pointer_cast<GeomDataAPI_Point2D>(anAttr->attr());
        if (aPoint2D)
        {
          myAttributesList[aNbPoints++] = CONSTRAINT_ATTRIBUTES[indAttr];
          continue;
        }
        // Verify the attribute is a 3D point
        boost::shared_ptr<GeomDataAPI_Point> aPoint3D =
          boost::dynamic_pointer_cast<GeomDataAPI_Point>(anAttr->attr());
        if (aPoint3D)
        {
          myAttributesList[aNbPoints++] = CONSTRAINT_ATTRIBUTES[indAttr];
          continue;
        }
      }
    }
    // Verify the correctness of constraint arguments
    if (aNbPoints == 2 && aNbEntities ==0)
      myType = SLVS_C_PT_PT_DISTANCE;
    else if (aNbPoints == 1 && aNbEntities == 1)
      myType = SLVS_C_UNKNOWN;
    return getType();
  }

  // Constraint for the given length of a line
  if (aConstraintKind.compare(SKETCH_CONSTRAINT_LENGTH_KIND) == 0)
  {
    int aNbLines = 0;
    for (unsigned int indAttr = 0; indAttr < CONSTRAINT_ATTR_SIZE; indAttr++)
    {
      boost::shared_ptr<ModelAPI_AttributeRefAttr> anAttr =
        boost::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(
          theConstraint->data()->attribute(CONSTRAINT_ATTRIBUTES[indAttr])
        );
      if (!anAttr) continue;
      if (anAttr->isFeature() && anAttr->feature() &&
          anAttr->feature()->getKind().compare(SKETCH_LINE_KIND) == 0)
      {
        myAttributesList[aNbLines++] = CONSTRAINT_ATTRIBUTES[indAttr];
        break;
      }
    }
    if (aNbLines == 1)
      myType = SLVS_C_PT_PT_DISTANCE;
    return getType();
  }

  // Constraint for two parallel/perpendicular lines
  bool isParallel = (aConstraintKind.compare(SKETCH_CONSTRAINT_PARALLEL_KIND) == 0);
  bool isPerpendicular = (aConstraintKind.compare(SKETCH_CONSTRAINT_PERPENDICULAR_KIND) == 0);
  if (isParallel || isPerpendicular)
  {
    int aNbEntities = 2; // lines in SolveSpace constraints should started from CONSTRAINT_ATTR_ENTITY_C attribute
    for (unsigned int indAttr = 0; indAttr < CONSTRAINT_ATTR_SIZE; indAttr++)
    {
      boost::shared_ptr<ModelAPI_AttributeRefAttr> anAttr =
        boost::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(
          theConstraint->data()->attribute(CONSTRAINT_ATTRIBUTES[indAttr])
        );
      if (!anAttr || !anAttr->isFeature()) continue;
      const std::string& aKind = anAttr->feature()->getKind();
      if (aKind.compare(SKETCH_LINE_KIND) == 0)
      {
        myAttributesList[aNbEntities++] = CONSTRAINT_ATTRIBUTES[indAttr];
        continue;
      }
    }
    if (aNbEntities == 4)
      myType = isParallel ? SLVS_C_PARALLEL : SLVS_C_PERPENDICULAR;
    return getType();
  }

  // Constraint for radius of a circle or an arc of circle
  if (aConstraintKind.compare(SKETCH_CONSTRAINT_RADIUS_KIND) == 0)
  {
    int aNbEntities = 2; // lines in SolveSpace constraints should started from CONSTRAINT_ATTR_ENTITY_C attribute
    for (unsigned int indAttr = 0; indAttr < CONSTRAINT_ATTR_SIZE; indAttr++)
    {
      boost::shared_ptr<ModelAPI_AttributeRefAttr> anAttr =
        boost::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(
          theConstraint->data()->attribute(CONSTRAINT_ATTRIBUTES[indAttr])
        );
      if (!anAttr || !anAttr->isFeature()) continue;
      const std::string& aKind = anAttr->feature()->getKind();
      if (aKind.compare(SKETCH_CIRCLE_KIND) == 0 || aKind.compare(SKETCH_ARC_KIND) == 0)
      {
        myAttributesList[aNbEntities++] = CONSTRAINT_ATTRIBUTES[indAttr];
        continue;
      }
    }
    if (aNbEntities == 3)
      myType = SLVS_C_DIAMETER;
    return getType();
  }

  /// \todo Implement other kind of constrtaints

  return getType();
}
