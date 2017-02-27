// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

#include <SketchSolver_ConstraintTangent.h>
#include <SketchSolver_Error.h>
#include <SketchSolver_Manager.h>

#include <GeomAPI_Pnt2d.h>
#include <SketchPlugin_Circle.h>
#include <SketchPlugin_ConstraintCoincidence.h>

#include <cmath>


/// \brief Check whether the entities has only one shared point or less
static bool hasSingleCoincidence(FeaturePtr theFeature1, FeaturePtr theFeature2)
{
  const std::set<AttributePtr>& aRefs1 = theFeature1->data()->refsToMe();
  const std::set<AttributePtr>& aRefs2 = theFeature2->data()->refsToMe();

  // collect all shared coincidendes
  std::set<FeaturePtr> aCoincidences;
  std::set<AttributePtr>::const_iterator anIt;
  for (anIt = aRefs1.begin(); anIt != aRefs1.end(); ++anIt) {
    FeaturePtr aRef = ModelAPI_Feature::feature((*anIt)->owner());
    if (aRef && aRef->getKind() == SketchPlugin_ConstraintCoincidence::ID())
      aCoincidences.insert(aRef);
  }
  for (anIt = aRefs2.begin(); anIt != aRefs2.end(); ++anIt) {
    FeaturePtr aRef = ModelAPI_Feature::feature((*anIt)->owner());
    if (aRef)
      aCoincidences.erase(aRef);
  }

  return aCoincidences.size() <= 1;
}

/// \brief Check if two connected arcs have centers
///        in same direction relatively to connection point
static bool isInternalTangency(EntityWrapperPtr theEntity1, EntityWrapperPtr theEntity2)
{
  BuilderPtr aBuilder = SketchSolver_Manager::instance()->builder();
  return aBuilder->isArcArcTangencyInternal(theEntity1, theEntity2);
}


void SketchSolver_ConstraintTangent::getAttributes(
    EntityWrapperPtr& theValue,
    std::vector<EntityWrapperPtr>& theAttributes)
{
  SketchSolver_Constraint::getAttributes(theValue, theAttributes);
  if (!myErrorMsg.empty() || !theAttributes[2] || !theAttributes[3]) {
    theAttributes.clear();
    return;
  }

  // Check the quantity of entities of each type and their order (arcs first)
  int aNbLines = 0;
  int aNbArcs = 0;
  int aNbCircles = 0;
  bool isSwap = false; // whether need to swap arguments (arc goes before line)
  std::vector<EntityWrapperPtr>::iterator anEntIt = theAttributes.begin() + 2;
  for (; anEntIt != theAttributes.end(); ++anEntIt) {
    if ((*anEntIt)->type() == ENTITY_LINE)
      ++aNbLines;
    else if ((*anEntIt)->type() == ENTITY_ARC) {
      ++aNbArcs;
      isSwap = aNbLines > 0;
    }
    else if ((*anEntIt)->type() == ENTITY_CIRCLE) {
      ++aNbCircles;
      isSwap = aNbLines > 0;
    }
  }

  if (aNbArcs < 1 && aNbCircles < 1) {
    myErrorMsg = SketchSolver_Error::INCORRECT_TANGENCY_ATTRIBUTE();
    return;
  }
  if (aNbLines == 1) {
    if (aNbArcs == 1)
      myType = CONSTRAINT_TANGENT_ARC_LINE;
    else if (aNbCircles == 1)
      myType = CONSTRAINT_TANGENT_CIRCLE_LINE;
  }
  else if (aNbArcs == 2) {
    myType = CONSTRAINT_TANGENT_ARC_ARC;
    isArcArcInternal = isInternalTangency(theAttributes[2], theAttributes[3]);
  }
  else {
    myErrorMsg = SketchSolver_Error::INCORRECT_ATTRIBUTE();
    return;
  }

  if (myType == CONSTRAINT_TANGENT_ARC_LINE) {
    AttributeRefAttrPtr aRefAttr = myBaseConstraint->refattr(SketchPlugin_Constraint::ENTITY_A());
    FeaturePtr aFeature1 = ModelAPI_Feature::feature(aRefAttr->object());
    aRefAttr = myBaseConstraint->refattr(SketchPlugin_Constraint::ENTITY_B());
    FeaturePtr aFeature2 = ModelAPI_Feature::feature(aRefAttr->object());

    if (!hasSingleCoincidence(aFeature1, aFeature2))
      myErrorMsg = SketchSolver_Error::TANGENCY_FAILED();
  }

  if (isSwap) {
    EntityWrapperPtr aTemp = theAttributes[2];
    theAttributes[2] = theAttributes[3];
    theAttributes[3] = aTemp;
  }
}

void SketchSolver_ConstraintTangent::adjustConstraint()
{
  if (myType == CONSTRAINT_TANGENT_ARC_ARC) {
    EntityWrapperPtr anEntity1 =
        myStorage->entity(myBaseConstraint->attribute(SketchPlugin_Constraint::ENTITY_A()));
    EntityWrapperPtr anEntity2 =
        myStorage->entity(myBaseConstraint->attribute(SketchPlugin_Constraint::ENTITY_B()));

    if (isArcArcInternal != isInternalTangency(anEntity1, anEntity2)) {
      // fully rebuld constraint, because it is unable to access attributes of PlaneGCS constraint
      remove();
      process();
    }
  }
}
