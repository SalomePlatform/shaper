// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

#include <SketchSolver_ConstraintDistance.h>
#include <SketchSolver_Error.h>
#include <SketchSolver_Manager.h>

#include <SketchPlugin_ConstraintDistanceHorizontal.h>
#include <SketchPlugin_ConstraintDistanceVertical.h>

#include <GeomAPI_Dir2d.h>
#include <GeomAPI_Lin2d.h>
#include <GeomAPI_Pnt2d.h>
#include <GeomAPI_XY.h>

#include <math.h>


void SketchSolver_ConstraintDistance::getAttributes(
    EntityWrapperPtr& theValue,
    std::vector<EntityWrapperPtr>& theAttributes)
{
  SketchSolver_Constraint::getAttributes(theValue, theAttributes);
  if (!myErrorMsg.empty() || !theAttributes[0]) {
    theAttributes.clear();
    return;
  }

  if (theAttributes[1]) {
    if (myBaseConstraint->getKind() == SketchPlugin_ConstraintDistanceHorizontal::ID())
      myType = CONSTRAINT_HORIZONTAL_DISTANCE;
    else if (myBaseConstraint->getKind() == SketchPlugin_ConstraintDistanceVertical::ID())
      myType = CONSTRAINT_VERTICAL_DISTANCE;
    else
      myType = CONSTRAINT_PT_PT_DISTANCE;
  } else if (theAttributes[2] && theAttributes[2]->type() == ENTITY_LINE)
    myType = CONSTRAINT_PT_LINE_DISTANCE;
  else
    theAttributes.clear();

  myPrevValue = 0.0;
}

void SketchSolver_ConstraintDistance::adjustConstraint()
{
  ConstraintWrapperPtr aConstraint = myStorage->constraint(myBaseConstraint);

  // Adjust point-point distance if the points are equal
  if (getType() == CONSTRAINT_PT_PT_DISTANCE) {
////    AttributePtr aPt1 = myBaseConstraint->attribute(SketchPlugin_Constraint::ENTITY_A());
////    AttributePtr aPt2 = myBaseConstraint->attribute(SketchPlugin_Constraint::ENTITY_B());
////
////    BuilderPtr aBuilder = SketchSolver_Manager::instance()->builder();
////    std::shared_ptr<GeomAPI_Pnt2d> aPoint1 = aBuilder->point(myStorage->entity(aPt1));
////    EntityWrapperPtr anEntity2 = myStorage->entity(aPt2);
////    std::shared_ptr<GeomAPI_Pnt2d> aPoint2 = aBuilder->point(anEntity2);
////
////////    if (aPoint1->distance(aPoint2) < tolerance) {
////////      // Change X coordinate of second point to eliminate coincidence
////////      ParameterWrapperPtr aX = aSubs.back()->parameters().front();
////////      aX->setValue(aX->value() + 1.0);
////////      myStorage->update(aX);
////////    }
    return;
  }

  // Adjust point-line distance
  if (fabs(myPrevValue) == fabs(aConstraint->value())) {
    // sign of distance is not changed
////    aConstraint->setValue(myPrevValue);
////    myStorage->addConstraint(myBaseConstraint, aConstraint);
    return;
  }

////  // Adjust the sign of constraint value
////  BuilderPtr aBuilder = SketchSolver_Manager::instance()->builder();
////
////  std::shared_ptr<GeomAPI_Lin2d> aLine;
////  std::shared_ptr<GeomAPI_Pnt2d> aPoint;
////  for (int i = 0; i < 2; ++i) {
////    AttributePtr anAttr = myBaseConstraint->attribute(SketchPlugin_Constraint::ATTRIBUTE(i));
////    EntityWrapperPtr anEntity = myStorage->entity(anAttr);
////    if (anEntity->type() == ENTITY_POINT)
////      aPoint = aBuilder->point(anEntity);
////    else if (anEntity->type() == ENTITY_LINE)
////      aLine = aBuilder->line(anEntity);
////  }
////
////  std::shared_ptr<GeomAPI_XY> aLineVec = aLine->direction()->xy();
////  std::shared_ptr<GeomAPI_XY> aPtLineVec = aPoint->xy()->decreased(aLine->location()->xy());
////  if (aLineVec->cross(aPtLineVec) * aConstraint->value() < 0.0)
////    aConstraint->setValue(aConstraint->value() * (-1.0));
}

void SketchSolver_ConstraintDistance::update()
{
  ConstraintWrapperPtr aConstraint = myStorage->constraint(myBaseConstraint);
  myPrevValue = aConstraint->value();

  SketchSolver_Constraint::update();
}
