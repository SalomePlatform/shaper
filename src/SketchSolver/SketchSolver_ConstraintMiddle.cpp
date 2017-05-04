// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

#include <SketchSolver_ConstraintMiddle.h>
#include <PlaneGCSSolver_ConstraintWrapper.h>
#include <PlaneGCSSolver_UpdateCoincidence.h>

void SketchSolver_ConstraintMiddle::getAttributes(
    EntityWrapperPtr& theValue,
    std::vector<EntityWrapperPtr>& theAttributes)
{
  SketchSolver_Constraint::getAttributes(theValue, theAttributes);
}

void SketchSolver_ConstraintMiddle::notify(const FeaturePtr&      theFeature,
                                           PlaneGCSSolver_Update* theUpdater)
{
  PlaneGCSSolver_UpdateCoincidence* anUpdater =
      static_cast<PlaneGCSSolver_UpdateCoincidence*>(theUpdater);
  bool isAccepted = anUpdater->addCoincidence(myAttributes.front(), myAttributes.back());
  if (isAccepted) {
    if (!myInSolver) {
      myInSolver = true;

      if (myMiddle) {
        // remove previously adde constraint
        myStorage->removeConstraint(myBaseConstraint);
        // merge divided constraints into single object
        std::list<GCSConstraintPtr> aGCSConstraints;
        std::shared_ptr<PlaneGCSSolver_ConstraintWrapper> aConstraint =
            std::dynamic_pointer_cast<PlaneGCSSolver_ConstraintWrapper>(myMiddle);
        aGCSConstraints.push_back(aConstraint->constraints().front());
        aConstraint =
            std::dynamic_pointer_cast<PlaneGCSSolver_ConstraintWrapper>(mySolverConstraint);
        aGCSConstraints.push_back(aConstraint->constraints().front());

        myMiddle = ConstraintWrapperPtr();
        mySolverConstraint = ConstraintWrapperPtr(
            new PlaneGCSSolver_ConstraintWrapper(aGCSConstraints, CONSTRAINT_MIDDLE_POINT));
      }

      myStorage->addConstraint(myBaseConstraint, mySolverConstraint);
    }
  } else {
    if (myInSolver) {
      myInSolver = false;
      myStorage->removeConstraint(myBaseConstraint);
    }

    if (!myMiddle) {
      // divide solver constraints to the middle point and point-line coincidence
      std::shared_ptr<PlaneGCSSolver_ConstraintWrapper> aConstraint =
          std::dynamic_pointer_cast<PlaneGCSSolver_ConstraintWrapper>(mySolverConstraint);
      std::list<GCSConstraintPtr> aGCSConstraints = aConstraint->constraints();

      myMiddle = ConstraintWrapperPtr(
          new PlaneGCSSolver_ConstraintWrapper(aGCSConstraints.front(), CONSTRAINT_MIDDLE_POINT));
      mySolverConstraint = ConstraintWrapperPtr(
          new PlaneGCSSolver_ConstraintWrapper(aGCSConstraints.back(), CONSTRAINT_MIDDLE_POINT));

      // send middle constraint only
      myStorage->addConstraint(myBaseConstraint, myMiddle);
    }
  }
}
