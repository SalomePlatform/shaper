// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

#include <SketchSolver_ConstraintLength.h>
#include <SketchSolver_Error.h>

#include <SketchPlugin_Line.h>


void SketchSolver_ConstraintLength::getAttributes(
    EntityWrapperPtr& theValue,
    std::vector<EntityWrapperPtr>& theAttributes)
{
  SketchSolver_Constraint::getAttributes(theValue, theAttributes);
  if (!myErrorMsg.empty() || !theAttributes[2] ||
      theAttributes[2]->type() != ENTITY_LINE) {
    theAttributes.clear();
    return;
  }

  AttributeRefAttrPtr aRefAttr = myBaseConstraint->refattr(SketchPlugin_Constraint::ENTITY_A());
  FeaturePtr aLine = ModelAPI_Feature::feature(aRefAttr->object());

  theAttributes[0] = myStorage->entity(aLine->attribute(SketchPlugin_Line::START_ID()));
  theAttributes[1] = myStorage->entity(aLine->attribute(SketchPlugin_Line::END_ID()));

  myType = CONSTRAINT_PT_PT_DISTANCE;
}

