// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

#include <SketchSolver_ConstraintAngle.h>

#include <ModelAPI_AttributeInteger.h>
#include <SketchPlugin_ConstraintAngle.h>

void SketchSolver_ConstraintAngle::getAttributes(
    EntityWrapperPtr& theValue, std::vector<EntityWrapperPtr>& theAttributes)
{
  SketchSolver_Constraint::getAttributes(theValue, theAttributes);
  myType = myBaseConstraint->integer(SketchPlugin_ConstraintAngle::TYPE_ID())->value();
}


void SketchSolver_ConstraintAngle::adjustConstraint()
{
  int aType = myBaseConstraint->integer(SketchPlugin_ConstraintAngle::TYPE_ID())->value();
  if (aType != myType) {
    myType = aType;
    myStorage->setNeedToResolve(true);
  }
}
