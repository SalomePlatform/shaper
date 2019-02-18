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

#include <SketchSolver_ConstraintCoincidence.h>
#include <SketchSolver_Error.h>
#include <PlaneGCSSolver_Tools.h>
#include <PlaneGCSSolver_UpdateCoincidence.h>

#include <SketchPlugin_Arc.h>
#include <SketchPlugin_Line.h>

static void getCoincidentFeatureExtremities(const ConstraintPtr& theConstraint,
                                            const StoragePtr& theStorage,
                                            EntityWrapperPtr theExtremities[2])
{
  for (int i = 0; i < CONSTRAINT_ATTR_SIZE; ++i) {
    AttributeRefAttrPtr aRefAttr = theConstraint->refattr(SketchPlugin_Constraint::ATTRIBUTE(i));
    if (!aRefAttr || !aRefAttr->isObject())
      continue;

    FeaturePtr aFeature = ModelAPI_Feature::feature(aRefAttr->object());
    if (!aFeature)
      continue;

    if (aFeature->getKind() == SketchPlugin_Line::ID()) {
      theExtremities[0] = theStorage->entity(aFeature->attribute(SketchPlugin_Line::START_ID()));
      theExtremities[1] = theStorage->entity(aFeature->attribute(SketchPlugin_Line::END_ID()));
    } else if (aFeature->getKind() == SketchPlugin_Arc::ID()) {
      theExtremities[0] = theStorage->entity(aFeature->attribute(SketchPlugin_Arc::START_ID()));
      theExtremities[1] = theStorage->entity(aFeature->attribute(SketchPlugin_Arc::END_ID()));
    }
  }
}


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
  myFeatureExtremities[0] = EntityWrapperPtr();
  myFeatureExtremities[1] = EntityWrapperPtr();
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

    // obtain extremity points of the coincident feature for further checking of multi-coincidence
    getCoincidentFeatureExtremities(myBaseConstraint, myStorage, myFeatureExtremities);
  } else
    myErrorMsg = SketchSolver_Error::INCORRECT_ATTRIBUTE();
}

void SketchSolver_ConstraintCoincidence::notify(const FeaturePtr&      theFeature,
                                                PlaneGCSSolver_Update* theUpdater)
{
  PlaneGCSSolver_UpdateCoincidence* anUpdater =
      static_cast<PlaneGCSSolver_UpdateCoincidence*>(theUpdater);
  bool isAccepted = anUpdater->addCoincidence(myAttributes.front(), myAttributes.back());

  // additionally check the point is coincident to extremity of coincident feature
  if (myFeatureExtremities[0] && myFeatureExtremities[1]) {
    EntityWrapperPtr aPoint =
        myAttributes.front()->type() == ENTITY_POINT ? myAttributes.front() : myAttributes.back();

    for (int i = 0; i < 2; ++i)
      isAccepted = isAccepted && !anUpdater->isPointOnEntity(aPoint, myFeatureExtremities[i]);
  }

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
