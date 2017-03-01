// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

#include <SketchSolver_ConstraintCoincidence.h>
#include <SketchSolver_Error.h>
#include <PlaneGCSSolver_Tools.h>
#include <PlaneGCSSolver_UpdateCoincidence.h>

void SketchSolver_ConstraintCoincidence::process()
{
  cleanErrorMsg();
  if (!myBaseConstraint || !myStorage) {
    // Not enough parameters are assigned
    return;
  }

  EntityWrapperPtr aValue;
  std::vector<EntityWrapperPtr> anAttributes;
  getAttributes(aValue, anAttributes);
  if (!myErrorMsg.empty())
    return;
  if (anAttributes.empty()) {
    myErrorMsg = SketchSolver_Error::INCORRECT_ATTRIBUTE();
    return;
  }

  mySolverConstraint = PlaneGCSSolver_Tools::createConstraint(
      myBaseConstraint, getType(),
      aValue, anAttributes[0], anAttributes[1], anAttributes[2], anAttributes[3]);

  myStorage->subscribeUpdates(this, PlaneGCSSolver_UpdateCoincidence::GROUP());
  myStorage->notify(myBaseConstraint);
}

bool SketchSolver_ConstraintCoincidence::remove()
{
  myInSolver = false;
  return SketchSolver_Constraint::remove();
}

void SketchSolver_ConstraintCoincidence::getAttributes(
    EntityWrapperPtr& theValue,
    std::vector<EntityWrapperPtr>& theAttributes)
{
  SketchSolver_Constraint::getAttributes(theValue, theAttributes);
  if (!myErrorMsg.empty() || !theAttributes[0]) {
    theAttributes.clear();
    return;
  }

  if (theAttributes[1])
    myType = CONSTRAINT_PT_PT_COINCIDENT;
  else if (theAttributes[2]) {
    // check the type of entity (line or circle)
    SketchSolver_EntityType anEntType = theAttributes[2]->type();
    if (anEntType == ENTITY_LINE)
      myType = CONSTRAINT_PT_ON_LINE;
    else if (anEntType == ENTITY_CIRCLE || anEntType == ENTITY_ARC)
      myType = CONSTRAINT_PT_ON_CIRCLE;
    else
      myErrorMsg = SketchSolver_Error::INCORRECT_ATTRIBUTE();
  } else
    myErrorMsg = SketchSolver_Error::INCORRECT_ATTRIBUTE();
}

void SketchSolver_ConstraintCoincidence::notify(const FeaturePtr&      theFeature,
                                                PlaneGCSSolver_Update* theUpdater)
{
  PlaneGCSSolver_UpdateCoincidence* anUpdater =
      static_cast<PlaneGCSSolver_UpdateCoincidence*>(theUpdater);
  bool isAccepted = anUpdater->checkCoincidence(myAttributes.front(), myAttributes.back());
  if (isAccepted) {
    if (!myInSolver) {
      myInSolver = true;
      myStorage->addConstraint(myBaseConstraint, mySolverConstraint);
    }
  } else {
    if (myInSolver) {
      myInSolver = false;
      myStorage->removeConstraint(myBaseConstraint);
    }
  }
}
