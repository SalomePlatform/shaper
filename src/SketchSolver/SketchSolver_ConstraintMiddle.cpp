// Copyright (C) 2014-2019  CEA/DEN, EDF R&D
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
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

#include <SketchSolver_ConstraintMiddle.h>
#include <PlaneGCSSolver_ConstraintWrapper.h>
#include <PlaneGCSSolver_EdgeWrapper.h>
#include <PlaneGCSSolver_PointWrapper.h>
#include <PlaneGCSSolver_Storage.h>
#include <PlaneGCSSolver_Tools.h>
#include <PlaneGCSSolver_UpdateCoincidence.h>

static bool isArc(EntityWrapperPtr theEntity)
{
  return theEntity->type() == ENTITY_ARC || theEntity->type() == ENTITY_ELLIPTIC_ARC;
}

void SketchSolver_ConstraintMiddle::getAttributes(
    EntityWrapperPtr& theValue,
    std::vector<EntityWrapperPtr>& theAttributes)
{
  SketchSolver_Constraint::getAttributes(theValue, theAttributes);

  // create auxiliary point if middle point on arc is specified
  if (isArc(theAttributes[2])) {
    std::shared_ptr<PlaneGCSSolver_Storage> aStorage =
        std::dynamic_pointer_cast<PlaneGCSSolver_Storage>(myStorage);

    myOddPoint = GCSPointPtr(new GCS::Point);
    myOddPoint->x = aStorage->createParameter();
    myOddPoint->y = aStorage->createParameter();
    theAttributes[1] = PointWrapperPtr(new PlaneGCSSolver_PointWrapper(myOddPoint));
  }
}

bool SketchSolver_ConstraintMiddle::remove()
{
  if (myOddPoint) {
    std::shared_ptr<PlaneGCSSolver_Storage> aStorage =
        std::dynamic_pointer_cast<PlaneGCSSolver_Storage>(myStorage);

    GCS::SET_pD aParams;
    aParams.insert(myOddPoint->x);
    aParams.insert(myOddPoint->y);
    aStorage->removeParameters(aParams);
  }
  return SketchSolver_ConstraintCoincidence::remove();
}

void SketchSolver_ConstraintMiddle::notify(const FeaturePtr&      theFeature,
                                           PlaneGCSSolver_Update* theUpdater)
{
  if (theFeature == myBaseConstraint && myInSolver) {
    // the constraint is already being updated,
    // update the middle point parameter if the constraint is "point-on-arc".
    if (myOddPoint) {
      EntityWrapperPtr anArcEntity =
          isArc(myAttributes.front()) ? myAttributes.front() : myAttributes.back();
      EdgeWrapperPtr anArcEdge =
          std::dynamic_pointer_cast<PlaneGCSSolver_EdgeWrapper>(anArcEntity);
      if (anArcEdge) {
        std::shared_ptr<GCS::Arc> anArc = std::dynamic_pointer_cast<GCS::Arc>(anArcEdge->entity());
        if (anArc) {
          // recalculate parameters of middle point according to arc
          *myOddPoint->x = (*anArc->startAngle + *anArc->endAngle) * 0.5;
          *myOddPoint->y = (*anArc->endAngle - *anArc->startAngle) * 0.5;
        }
        else {
          std::shared_ptr<GCS::ArcOfEllipse> aEllArc =
              std::dynamic_pointer_cast<GCS::ArcOfEllipse>(anArcEdge->entity());
          if (aEllArc) {
            // recalculate parameters of middle point according to arc
            *myOddPoint->x = (*aEllArc->startAngle + *aEllArc->endAngle) * 0.5;
            *myOddPoint->y = (*aEllArc->endAngle - *aEllArc->startAngle) * 0.5;
          }
        }
      }
    }
    return;
  }

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
        std::list<GCSConstraintPtr> aGCSConstraints = myMiddle->constraints();
        aGCSConstraints.push_front(mySolverConstraint->constraints().front());

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

      mySolverConstraint = ConstraintWrapperPtr(
        new PlaneGCSSolver_ConstraintWrapper(aGCSConstraints.front(), CONSTRAINT_MIDDLE_POINT));
      aGCSConstraints.pop_front();
      myMiddle = ConstraintWrapperPtr(
          new PlaneGCSSolver_ConstraintWrapper(aGCSConstraints, CONSTRAINT_MIDDLE_POINT));

      // send middle constraint only
      myStorage->addConstraint(myBaseConstraint, myMiddle);
    }
  }
}
