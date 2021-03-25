// Copyright (C) 2014-2021  CEA/DEN, EDF R&D
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

#include <SketchSolver_Storage.h>
#include <PlaneGCSSolver_UpdateCoincidence.h>
#include <PlaneGCSSolver_UpdateFeature.h>

#include <ModelAPI_AttributeRefAttr.h>
#include <ModelAPI_AttributeRefList.h>
#include <SketchPlugin_Arc.h>
#include <SketchPlugin_Circle.h>
#include <SketchPlugin_Line.h>
#include <SketchPlugin_Point.h>
#include <SketchPlugin_IntersectionPoint.h>
#include <SketchPlugin_ConstraintCoincidence.h>
#include <SketchPlugin_ConstraintMirror.h>
#include <SketchPlugin_ConstraintRigid.h>


SketchSolver_Storage::SketchSolver_Storage(SolverPtr theSolver)
  : mySketchSolver(theSolver),
    myNeedToResolve(false),
    myEventsBlocked(false)
{
  // create updaters (constraints processed at first)
  UpdaterPtr aFeatureUpdater(new PlaneGCSSolver_UpdateFeature);
  myUpdaters = UpdaterPtr(new PlaneGCSSolver_UpdateCoincidence(aFeatureUpdater));
}

void SketchSolver_Storage::addConstraint(ConstraintPtr        theConstraint,
                                         ConstraintWrapperPtr theSolverConstraint)
{
  std::map<ConstraintPtr, ConstraintWrapperPtr>::const_iterator
      aFound = myConstraintMap.find(theConstraint);
  if (aFound == myConstraintMap.end() || aFound->second != theSolverConstraint)
    setNeedToResolve(true);

////  if (theSolverConstraints.empty()) {
////    // constraint links to the empty list, add its attributes linked to the empty entities
////    std::list<AttributePtr> aRefAttrs =
////        theConstraint->data()->attributes(ModelAPI_AttributeRefAttr::typeId());
////    std::list<AttributePtr>::const_iterator anAttrIt = aRefAttrs.begin();
////    for (; anAttrIt != aRefAttrs.end(); ++anAttrIt) {
////      AttributeRefAttrPtr aRef =
////          std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(*anAttrIt);
////      if (aRef->isObject()) {
////        FeaturePtr aFeature = ModelAPI_Feature::feature(aRef->object());
////        if (aFeature) addEntity(aFeature, EntityWrapperPtr());
////      } else
////        addEntity(aRef->attr(), EntityWrapperPtr());
////    }
////    std::list<AttributePtr> aRefLists =
////        theConstraint->data()->attributes(ModelAPI_AttributeRefList::typeId());
////    for (anAttrIt = aRefLists.begin(); anAttrIt != aRefLists.end(); ++anAttrIt) {
////      AttributeRefListPtr aRef =
////          std::dynamic_pointer_cast<ModelAPI_AttributeRefList>(*anAttrIt);
////      std::list<ObjectPtr> anObj = aRef->list();
////      std::list<ObjectPtr>::iterator anIt = anObj.begin();
////      for (; anIt != anObj.end(); ++anIt) {
////        FeaturePtr aFeature = ModelAPI_Feature::feature(*anIt);
////        if (aFeature) addEntity(aFeature, EntityWrapperPtr());
////      }
////    }
////  }
////  else if (theSolverConstraints.front()->type() != CONSTRAINT_PT_PT_COINCIDENT) {
////    // Do not add point-point coincidence, because it is already made by setting
////    // the same parameters for both points
////    std::list<ConstraintWrapperPtr>::iterator aCIt = theSolverConstraints.begin();
////    for (; aCIt != theSolverConstraints.end(); ++aCIt)
////      update(*aCIt);
////  }

  if (!theSolverConstraint || aFound == myConstraintMap.end())
    myConstraintMap[theConstraint] = theSolverConstraint;
  // block events if necessary
  if (myEventsBlocked && theConstraint && theConstraint->data() && theConstraint->data()->isValid())
    theConstraint->data()->blockSendAttributeUpdated(myEventsBlocked);
}

void SketchSolver_Storage::addEntity(FeaturePtr       theFeature,
                                     EntityWrapperPtr theSolverEntity)
{
  if (theSolverEntity) {
    myFeatureMap[theFeature] = theSolverEntity;
    setNeedToResolve(true);
  }

  // block events if necessary
  if (myEventsBlocked && theFeature->data() && theFeature->data()->isValid())
    theFeature->data()->blockSendAttributeUpdated(myEventsBlocked);
}

void SketchSolver_Storage::addEntity(AttributePtr     theAttribute,
                                     EntityWrapperPtr theSolverEntity)
{
  if (theSolverEntity) {
    myAttributeMap[theAttribute] = theSolverEntity;
    setNeedToResolve(true);
  }

  // block events if necessary
  if (myEventsBlocked && theAttribute->owner() &&
      theAttribute->owner()->data() && theAttribute->owner()->data()->isValid())
    theAttribute->owner()->data()->blockSendAttributeUpdated(myEventsBlocked);
}



const ConstraintWrapperPtr& SketchSolver_Storage::constraint(
    const ConstraintPtr& theConstraint) const
{
  static ConstraintWrapperPtr aDummy;

  std::map<ConstraintPtr, ConstraintWrapperPtr>::const_iterator
      aFound = myConstraintMap.find(theConstraint);
  if (aFound != myConstraintMap.end())
    return aFound->second;
  return aDummy;
}

const EntityWrapperPtr& SketchSolver_Storage::entity(const FeaturePtr& theFeature) const
{
  std::map<FeaturePtr, EntityWrapperPtr>::const_iterator aFound = myFeatureMap.find(theFeature);
  if (aFound != myFeatureMap.end())
    return aFound->second;

  static EntityWrapperPtr aDummy;
  return aDummy;
}

const EntityWrapperPtr& SketchSolver_Storage::entity(const AttributePtr& theAttribute) const
{
  std::map<AttributePtr, EntityWrapperPtr>::const_iterator
      aFound = myAttributeMap.find(theAttribute);
  if (aFound != myAttributeMap.end())
    return aFound->second;

  AttributeRefAttrPtr aRefAttr =
      std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(theAttribute);
  if (aRefAttr) {
    AttributePtr anAttribute;

    if (aRefAttr->isObject()) {
      /// TODO: Check resultToFeatureOrAttribute() precisely. Create additional unit-test
      FeaturePtr aFeature;
      resultToFeatureOrAttribute(aRefAttr->object(), aFeature, anAttribute);
      if (aFeature)
        return entity(aFeature);
    } else
      anAttribute = aRefAttr->attr();

    return entity(anAttribute);
  }

  static EntityWrapperPtr aDummy;
  return aDummy;
}


void SketchSolver_Storage::removeFeature(FeaturePtr theFeature)
{
  myFeatureMap.erase(theFeature);
  myNeedToResolve = true;
}

void SketchSolver_Storage::removeAttribute(AttributePtr theAttribute)
{
  myAttributeMap.erase(theAttribute);
  myNeedToResolve = true;
}


bool SketchSolver_Storage::areFeaturesValid() const
{
  // Check the features are valid
  std::map<FeaturePtr, EntityWrapperPtr>::const_iterator aFIter = myFeatureMap.begin();
  for (; aFIter != myFeatureMap.end(); aFIter++)
    if (!aFIter->first->data() || !aFIter->first->data()->isValid())
      return false;
  return true;
}

void SketchSolver_Storage::blockEvents(bool isBlocked)
{
  std::map<FeaturePtr, EntityWrapperPtr>::const_iterator aFIter = myFeatureMap.begin();
  for (; aFIter != myFeatureMap.end(); aFIter++)
    if (aFIter->first->data() && aFIter->first->data()->isValid())
      aFIter->first->data()->blockSendAttributeUpdated(isBlocked);

  std::map<AttributePtr, EntityWrapperPtr>::const_iterator anAtIter = myAttributeMap.begin();
  for (; anAtIter != myAttributeMap.end(); anAtIter++)
    if (anAtIter->first->owner() && anAtIter->first->owner()->data() &&
        anAtIter->first->owner()->data()->isValid())
      anAtIter->first->owner()->data()->blockSendAttributeUpdated(isBlocked);

  myEventsBlocked = isBlocked;
}

std::set<ObjectPtr> SketchSolver_Storage::getConflictingConstraints(SolverPtr theSolver) const
{
  std::set<ObjectPtr> aConflicting;
  std::map<ConstraintPtr, ConstraintWrapperPtr>::const_iterator
      aConstrIt = myConstraintMap.begin();
  for (; aConstrIt != myConstraintMap.end(); ++aConstrIt) {
    if (theSolver->isConflicting(aConstrIt->second->id()))
      aConflicting.insert(aConstrIt->first);
  }
  return aConflicting;
}

void SketchSolver_Storage::subscribeUpdates(
    SketchSolver_Constraint* theSubscriber, const std::string& theGroup) const
{
  myUpdaters->attach(theSubscriber, theGroup);
}

void SketchSolver_Storage::unsubscribeUpdates(SketchSolver_Constraint* theSubscriber) const
{
  myUpdaters->detach(theSubscriber);
}

void SketchSolver_Storage::notify(const FeaturePtr & theFeature) const
{
  myUpdaters->update(theFeature);
}

void SketchSolver_Storage::resultToFeatureOrAttribute(const ObjectPtr& theResult,
    FeaturePtr& theFeature, AttributePtr& theAttribute)
{
  FeaturePtr aFeature = ModelAPI_Feature::feature(theResult);
  if (!aFeature)
    return;

  // if the feature has several results, we choose which one is referred
  const std::list<ResultPtr>& aResults = aFeature->results();
  if (aResults.size() > 1 && theResult != aFeature->lastResult()) {
    // actually, the attribute refers to center of arc or circle,
    // but not the edge, get correct attributes
    std::string anAttrName;
    if (aFeature->getKind() == SketchPlugin_Arc::ID())
      anAttrName = SketchPlugin_Arc::CENTER_ID();
    else if (aFeature->getKind() == SketchPlugin_Circle::ID())
      anAttrName = SketchPlugin_Circle::CENTER_ID();
    if (!anAttrName.empty()) {
      theAttribute = aFeature->attribute(anAttrName);
      aFeature = FeaturePtr();
    }
  }
  theFeature = aFeature;
}
