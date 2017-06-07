// Copyright (C) 2014-2017  CEA/DEN, EDF R&D
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com<mailto:webmaster.salome@opencascade.com>
//

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
  if (theFeature == myBaseConstraint && myInSolver)
    return; // the constraint is already being updated

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
      myStorage->removeConstraint(myBaseConstraint);
      myInSolver = false;
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
