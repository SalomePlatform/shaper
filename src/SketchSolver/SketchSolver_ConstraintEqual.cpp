// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

#include <SketchSolver_ConstraintEqual.h>
#include <SketchSolver_Error.h>
#include <PlaneGCSSolver_AttributeBuilder.h>

#include <ModelAPI_AttributeRefAttr.h>
#include <SketchPlugin_Line.h>

void SketchSolver_ConstraintEqual::getAttributes(
    EntityWrapperPtr& theValue,
    std::vector<EntityWrapperPtr>& theAttributes)
{
  SketchSolver_Constraint::getAttributes(theValue, theAttributes);
  if (!myErrorMsg.empty() || !theAttributes[2] || !theAttributes[3]) {
    theAttributes.clear();
    return;
  }

  // Check the quantity of entities of each type
  int aNbLines = 0;
  int aNbArcs = 0;
  int aNbCircs = 0;
  bool isArcFirst = false; // in line-arc equivalence, the line should be first
  std::vector<EntityWrapperPtr>::iterator anAttrIt = theAttributes.begin() + 2;
  for (; anAttrIt != theAttributes.end(); ++anAttrIt) {
    SketchSolver_EntityType aType = (*anAttrIt)->type();
    if (aType == ENTITY_LINE)
      ++aNbLines;
    else if (aType == ENTITY_CIRCLE)
      ++aNbCircs;
    else if (aType == ENTITY_ARC) {
      ++aNbArcs;
      isArcFirst = (aNbLines == 0);
    }
  }

  if (aNbLines + aNbArcs + aNbCircs != 2 ||
     (aNbLines == aNbCircs && aNbArcs == 0)) {
    myErrorMsg = SketchSolver_Error::INCORRECT_ATTRIBUTE();
    return;
  }

  switch (aNbLines) {
  case 0:
    myType = CONSTRAINT_EQUAL_RADIUS;
    break;
  case 1:
    myType = CONSTRAINT_EQUAL_LINE_ARC;
    if (isArcFirst) { // change the order of arc and line
      EntityWrapperPtr aTmp = theAttributes[2];
      theAttributes[2] = theAttributes[3];
      theAttributes[3] = aTmp;
    }
    break;
  default:
    myType = CONSTRAINT_EQUAL_LINES;

    AttributeRefAttrPtr aRefLine1 =
        myBaseConstraint->refattr(SketchPlugin_Constraint::ENTITY_A());
    FeaturePtr aLine1 = ModelAPI_Feature::feature(aRefLine1->object());
    if (aLine1) {
      PlaneGCSSolver_AttributeBuilder aBuilder(myStorage);
      // store length of first line as a value for constraint
      // (will be used to make equal lengths of lines)
      theValue = aBuilder.createAttribute(aLine1->attribute(SketchPlugin_Line::LENGTH_ID()));
    }
    break;
  }
}
