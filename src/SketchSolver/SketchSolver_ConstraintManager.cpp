// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:    SketchSolver_ConstraintManager.cpp
// Created: 08 May 2014
// Author:  Artem ZHIDKOV

#include "SketchSolver_ConstraintManager.h"

#include <Events_Loop.h>
#include <ModelAPI_AttributeDouble.h>
#include <ModelAPI_AttributeRefList.h>
#include <ModelAPI_Data.h>
#include <ModelAPI_Events.h>
#include <ModelAPI_Object.h>
#include <ModelAPI_ResultConstruction.h>
#include <ModelAPI_Attribute.h>

#include <SketchPlugin_Constraint.h>
#include <SketchPlugin_ConstraintCoincidence.h>

#include <SketchPlugin_Arc.h>
#include <SketchPlugin_Circle.h>
#include <SketchPlugin_Line.h>
#include <SketchPlugin_Point.h>
#include <SketchPlugin_Sketch.h>
#include <SketchPlugin_Feature.h>

#include <list>
#include <set>
#include <memory>

// Initialization of constraint manager self pointer
SketchSolver_ConstraintManager* SketchSolver_ConstraintManager::_self = 0;

/// Global constraint manager object
SketchSolver_ConstraintManager* myManager = SketchSolver_ConstraintManager::Instance();

// ========================================================
// ========= SketchSolver_ConstraintManager ===============
// ========================================================
SketchSolver_ConstraintManager* SketchSolver_ConstraintManager::Instance()
{
  if (!_self)
    _self = new SketchSolver_ConstraintManager();
  return _self;
}

SketchSolver_ConstraintManager::SketchSolver_ConstraintManager()
{
  myGroups.clear();
  myIsComputed = false;

  // Register in event loop
  Events_Loop::loop()->registerListener(this, Events_Loop::eventByName(EVENT_OBJECT_CREATED));
  Events_Loop::loop()->registerListener(this, Events_Loop::eventByName(EVENT_OBJECT_UPDATED));
  Events_Loop::loop()->registerListener(this, Events_Loop::eventByName(EVENT_OBJECT_DELETED));
  Events_Loop::loop()->registerListener(this, Events_Loop::eventByName(EVENT_OBJECT_MOVED));
}

SketchSolver_ConstraintManager::~SketchSolver_ConstraintManager()
{
  myGroups.clear();
}

// ============================================================================
//  Function: processEvent
//  Class:    SketchSolver_Session
//  Purpose:  listen the event loop and process the message
// ============================================================================
void SketchSolver_ConstraintManager::processEvent(
  const std::shared_ptr<Events_Message>& theMessage)
{
  if (myIsComputed)
    return;
  if (theMessage->eventID() == Events_Loop::loop()->eventByName(EVENT_OBJECT_CREATED)
      || theMessage->eventID() == Events_Loop::loop()->eventByName(EVENT_OBJECT_UPDATED)
      || theMessage->eventID() == Events_Loop::loop()->eventByName(EVENT_OBJECT_MOVED)) {
    std::shared_ptr<ModelAPI_ObjectUpdatedMessage> anUpdateMsg =
        std::dynamic_pointer_cast<ModelAPI_ObjectUpdatedMessage>(theMessage);
    std::set<ObjectPtr> aFeatures = anUpdateMsg->objects();

    bool isMovedEvt = theMessage->eventID()
        == Events_Loop::loop()->eventByName(EVENT_OBJECT_MOVED);
    if (isMovedEvt) {
      std::set<ObjectPtr>::iterator aFeatIter;
      for (aFeatIter = aFeatures.begin(); aFeatIter != aFeatures.end(); aFeatIter++) {
        std::shared_ptr<SketchPlugin_Feature> aSFeature = 
            std::dynamic_pointer_cast<SketchPlugin_Feature>(*aFeatIter);
        if (aSFeature)
          moveEntity(aSFeature);
      }
    } else {
      std::set<ObjectPtr>::iterator aFeatIter;
      // iterate sketchers fisrt to create all sketches before (on load may exist several sketches)
      for (aFeatIter = aFeatures.begin(); aFeatIter != aFeatures.end(); aFeatIter++) {
        FeaturePtr aFeature = std::dynamic_pointer_cast<ModelAPI_Feature>(*aFeatIter);
        if (!aFeature)
          continue;
        const std::string& aFeatureKind = aFeature->getKind();
        if (aFeatureKind.compare(SketchPlugin_Sketch::ID()) == 0) {
          std::shared_ptr<ModelAPI_CompositeFeature> aSketch = std::dynamic_pointer_cast<
              ModelAPI_CompositeFeature>(aFeature);
          changeWorkplane(aSketch);
        }
      }
      // then get anything but not the sketch
      // at first, add coincidence constraints, because they may be used by other constraints
      for (aFeatIter = aFeatures.begin(); aFeatIter != aFeatures.end(); aFeatIter++) {
        std::shared_ptr<SketchPlugin_Feature> aFeature = 
          std::dynamic_pointer_cast<SketchPlugin_Feature>(*aFeatIter);
        if (!aFeature || aFeature->getKind() != SketchPlugin_ConstraintCoincidence::ID())
          continue;
        changeConstraintOrEntity(aFeature);
      }
      // after that, add all features except coincidence
      for (aFeatIter = aFeatures.begin(); aFeatIter != aFeatures.end(); aFeatIter++) {
        std::shared_ptr<SketchPlugin_Feature> aFeature = 
          std::dynamic_pointer_cast<SketchPlugin_Feature>(*aFeatIter);
        if (!aFeature /*|| aFeature->getKind() == SketchPlugin_ConstraintCoincidence::ID()*/)
          continue;
        changeConstraintOrEntity(aFeature);
      }
    }

    // Solve the set of constraints
    resolveConstraints(isMovedEvt); // send update for movement in any case
  } else if (theMessage->eventID() == Events_Loop::loop()->eventByName(EVENT_OBJECT_DELETED)) {
    std::shared_ptr<ModelAPI_ObjectDeletedMessage> aDeleteMsg =
        std::dynamic_pointer_cast<ModelAPI_ObjectDeletedMessage>(theMessage);
    const std::set<std::string>& aFeatureGroups = aDeleteMsg->groups();

    // Find SketchPlugin_Sketch::ID() in groups. The constraint groups should be updated when an object removed from Sketch
    std::set<std::string>::const_iterator aFGrIter;
    for (aFGrIter = aFeatureGroups.begin(); aFGrIter != aFeatureGroups.end(); aFGrIter++)
      if (aFGrIter->compare(ModelAPI_ResultConstruction::group()) == 0 ||
          aFGrIter->compare(ModelAPI_Feature::group()) == 0)
        break;

    if (aFGrIter != aFeatureGroups.end()) {
      std::vector<SketchSolver_Group*>::iterator aGroupIter = myGroups.begin();
      std::vector<SketchSolver_Group*> aSeparatedGroups;
      while (aGroupIter != myGroups.end()) {
        if (!(*aGroupIter)->isWorkplaneValid()) {  // the group should be removed
          delete *aGroupIter;
          int aShift = aGroupIter - myGroups.begin();
          myGroups.erase(aGroupIter);
          aGroupIter = myGroups.begin() + aShift;
          continue;
        }
        if (!(*aGroupIter)->isConsistent()) {  // some constraints were removed, try to split the group
          (*aGroupIter)->splitGroup(aSeparatedGroups);
        }
        aGroupIter++;
      }
      if (aSeparatedGroups.size() > 0)
        myGroups.insert(myGroups.end(), aSeparatedGroups.begin(), aSeparatedGroups.end());
    }
  }
}

// ============================================================================
//  Function: changeWorkplane
//  Class:    SketchSolver_Session
//  Purpose:  update workplane by given parameters of the sketch
// ============================================================================
bool SketchSolver_ConstraintManager::changeWorkplane(CompositeFeaturePtr theSketch)
{
  bool aResult = true;  // changed when a workplane wrongly updated
  bool isUpdated = false;
  // Try to update specified workplane in all groups
  std::vector<SketchSolver_Group*>::iterator aGroupIter;
  for (aGroupIter = myGroups.begin(); aGroupIter != myGroups.end(); aGroupIter++)
    if ((*aGroupIter)->isBaseWorkplane(theSketch)) {
      isUpdated = true;
      if (!(*aGroupIter)->updateWorkplane())
        aResult = false;
    }
  // If the workplane is not updated, so this is a new workplane
  if (!isUpdated) {
    SketchSolver_Group* aNewGroup = new SketchSolver_Group(theSketch);
    // Verify that the group is created successfully
    if (!aNewGroup->isBaseWorkplane(theSketch) || !aNewGroup->isWorkplaneValid()) {
      delete aNewGroup;
      return false;
    }
    myGroups.push_back(aNewGroup);
  }
  return aResult;
}

// ============================================================================
//  Function: changeConstraintOrEntity
//  Class:    SketchSolver_Session
//  Purpose:  create/update the constraint or the feature and place it into appropriate group
// ============================================================================
bool SketchSolver_ConstraintManager::changeConstraintOrEntity(
    std::shared_ptr<SketchPlugin_Feature> theFeature)
{
  // Search the groups which this feature touches
  std::set<Slvs_hGroup> aGroups;
  findGroups(theFeature, aGroups);

  std::shared_ptr<SketchPlugin_Constraint> aConstraint = 
      std::dynamic_pointer_cast<SketchPlugin_Constraint>(theFeature);

  // Process the groups list
  if (aGroups.size() == 0) {
    // There are no groups applicable for this constraint => create new one
    // The group will be created only for constraints, not for features
    if (!aConstraint) return false;
    std::shared_ptr<ModelAPI_CompositeFeature> aWP = findWorkplane(aConstraint);
    if (!aWP)
      return false;
    SketchSolver_Group* aGroup = new SketchSolver_Group(aWP);
    if (!aGroup->changeConstraint(aConstraint)) {
      delete aGroup;
      return false;
    }
    myGroups.push_back(aGroup);
    return true;
  } else if (aGroups.size() == 1) {  // Only one group => add feature into it
    Slvs_hGroup aGroupId = *(aGroups.begin());
    std::vector<SketchSolver_Group*>::iterator aGroupIter;
    for (aGroupIter = myGroups.begin(); aGroupIter != myGroups.end(); aGroupIter++)
      if ((*aGroupIter)->getId() == aGroupId) {
        // If the group is empty, the feature is not added (the constraint only)
        if (!aConstraint && !(*aGroupIter)->isEmpty())
          return (*aGroupIter)->updateFeature(theFeature);
////          return (*aGroupIter)->changeEntityFeature(theFeature) != SLVS_E_UNKNOWN;
        return (*aGroupIter)->changeConstraint(aConstraint);
      }
  } else if (aGroups.size() > 1) {  // Several groups applicable for this feature => need to merge them
    std::set<Slvs_hGroup>::const_iterator aGroupsIter = aGroups.begin();

    // Search first group
    std::vector<SketchSolver_Group*>::iterator aFirstGroupIter;
    for (aFirstGroupIter = myGroups.begin(); aFirstGroupIter != myGroups.end(); aFirstGroupIter++)
      if ((*aFirstGroupIter)->getId() == *aGroupsIter)
        break;
    if (aFirstGroupIter == myGroups.end())
      return false;

    // Append other groups to the first one
    std::vector<SketchSolver_Group*>::iterator anOtherGroupIter = aFirstGroupIter + 1;
    for (aGroupsIter++; aGroupsIter != aGroups.end(); aGroupsIter++) {
      for (; anOtherGroupIter != myGroups.end(); anOtherGroupIter++)
        if ((*anOtherGroupIter)->getId() == *aGroupsIter)
          break;
      if (anOtherGroupIter == myGroups.end()) {  // Group disappears
        anOtherGroupIter = aFirstGroupIter + 1;
        continue;
      }

      (*aFirstGroupIter)->mergeGroups(**anOtherGroupIter);
      int aShiftFirst = aFirstGroupIter - myGroups.begin();
      int aShiftOther = anOtherGroupIter - myGroups.begin();
      delete *anOtherGroupIter;
      myGroups.erase(anOtherGroupIter);
      aFirstGroupIter = myGroups.begin() + aShiftFirst;
      anOtherGroupIter = myGroups.begin() + aShiftOther;
    }

    if (aConstraint)
      return (*aFirstGroupIter)->changeConstraint(aConstraint);
    return (*aFirstGroupIter)->updateFeature(theFeature);
////    return (*aFirstGroupIter)->changeEntityFeature(theFeature) != SLVS_E_UNKNOWN;
  }

  // Something goes wrong
  return false;
}

// ============================================================================
//  Function: moveEntity
//  Class:    SketchSolver_Session
//  Purpose:  update element moved on the sketch, which is used by constraints
// ============================================================================
void SketchSolver_ConstraintManager::moveEntity(
    std::shared_ptr<SketchPlugin_Feature> theFeature)
{
  std::vector<SketchSolver_Group*>::iterator aGroupIt = myGroups.begin();
  for (; aGroupIt != myGroups.end(); aGroupIt++)
    if (!(*aGroupIt)->isEmpty() && (*aGroupIt)->isInteract(theFeature))
      (*aGroupIt)->moveFeature(theFeature);

////  // Create list of attributes depending on type of the feature
////  std::vector<std::string> anAttrList;
////  const std::string& aFeatureKind = theFeature->getKind();
////  // Point
////  if (aFeatureKind.compare(SketchPlugin_Point::ID()) == 0)
////    anAttrList.push_back(SketchPlugin_Point::COORD_ID());
////  // Line
////  else if (aFeatureKind.compare(SketchPlugin_Line::ID()) == 0) {
////    anAttrList.push_back(SketchPlugin_Line::START_ID());
////    anAttrList.push_back(SketchPlugin_Line::END_ID());
////  }
////  // Circle
////  else if (aFeatureKind.compare(SketchPlugin_Circle::ID()) == 0) {
////    anAttrList.push_back(SketchPlugin_Circle::CENTER_ID());
////    anAttrList.push_back(SketchPlugin_Circle::RADIUS_ID());
////  }
////  // Arc
////  else if (aFeatureKind.compare(SketchPlugin_Arc::ID()) == 0) {
////    anAttrList.push_back(SketchPlugin_Arc::CENTER_ID());
////    anAttrList.push_back(SketchPlugin_Arc::START_ID());
////    anAttrList.push_back(SketchPlugin_Arc::END_ID());
////  }
////  /// \todo Other types of features should be implemented
////
////  // Check changing of feature's attributes (go through the groups and search usage of the attributes)
////  std::vector<std::string>::const_iterator anAttrIter;
////  for (anAttrIter = anAttrList.begin(); anAttrIter != anAttrList.end(); anAttrIter++) {
////    std::vector<SketchSolver_Group*>::iterator aGroupIter;
////    for (aGroupIter = myGroups.begin(); aGroupIter != myGroups.end(); aGroupIter++) {
////      if ((*aGroupIter)->isEmpty())
////        continue;
////      std::shared_ptr<ModelAPI_Attribute> anAttribute = std::dynamic_pointer_cast<
////          ModelAPI_Attribute>(theFeature->data()->attribute(*anAttrIter));
////      (*aGroupIter)->updateEntityIfPossible(anAttribute);
////    }
////  }
////
////  std::vector<SketchSolver_Group*>::iterator aGroupIter;
////  for (aGroupIter = myGroups.begin(); aGroupIter != myGroups.end(); aGroupIter++)
////    if (!(*aGroupIter)->isEmpty())
////      (*aGroupIter)->updateRelatedConstraintsFeature(theFeature);
}

// ============================================================================
//  Function: findGroups
//  Class:    SketchSolver_Session
//  Purpose:  search groups of entities interacting with given feature
// ============================================================================
void SketchSolver_ConstraintManager::findGroups(
    std::shared_ptr<SketchPlugin_Feature> theFeature,
    std::set<Slvs_hGroup>& theGroupIDs) const
{
  std::shared_ptr<ModelAPI_CompositeFeature> aWP = findWorkplane(theFeature);

  SketchSolver_Group* anEmptyGroup = 0;  // appropriate empty group for specified constraint
  std::vector<SketchSolver_Group*>::const_iterator aGroupIter;
  for (aGroupIter = myGroups.begin(); aGroupIter != myGroups.end(); aGroupIter++)
    if (aWP == (*aGroupIter)->getWorkplane() && (*aGroupIter)->isInteract(theFeature)) {
      if (!(*aGroupIter)->isEmpty())
        theGroupIDs.insert((*aGroupIter)->getId());
      else if (!anEmptyGroup)
        anEmptyGroup = *aGroupIter;
    }

  // When only empty group is found, use it
  if (anEmptyGroup && theGroupIDs.empty())
    theGroupIDs.insert(anEmptyGroup->getId());
}

// ============================================================================
//  Function: findWorkplane
//  Class:    SketchSolver_Session
//  Purpose:  search workplane containing given feature
// ============================================================================
std::shared_ptr<ModelAPI_CompositeFeature> SketchSolver_ConstraintManager
::findWorkplane(std::shared_ptr<SketchPlugin_Feature> theFeature) const
{
  // Already verified workplanes
  std::set<std::shared_ptr<ModelAPI_CompositeFeature> > aVerified;

  std::vector<SketchSolver_Group*>::const_iterator aGroupIter;
  for (aGroupIter = myGroups.begin(); aGroupIter != myGroups.end(); aGroupIter++) {
    std::shared_ptr<ModelAPI_CompositeFeature> aWP = (*aGroupIter)->getWorkplane();
    if (aVerified.find(aWP) != aVerified.end())
      continue;

    DataPtr aData = aWP->data();
    if (aData) {
      std::shared_ptr<ModelAPI_AttributeRefList> aWPFeatures = std::dynamic_pointer_cast<
          ModelAPI_AttributeRefList>(aData->attribute(SketchPlugin_Sketch::FEATURES_ID()));
      std::list<ObjectPtr> aFeaturesList = aWPFeatures->list();
      std::list<ObjectPtr>::const_iterator anIter;
      for (anIter = aFeaturesList.begin(); anIter != aFeaturesList.end(); anIter++)
        if (*anIter == theFeature)
          return aWP;  // workplane is found
    }
    aVerified.insert(aWP);
  }

  return std::shared_ptr<ModelAPI_CompositeFeature>();
}

// ============================================================================
//  Function: resolveConstraints
//  Class:    SketchSolver_Session
//  Purpose:  change entities according to available constraints
// ============================================================================
void SketchSolver_ConstraintManager::resolveConstraints(const bool theForceUpdate)
{
  myIsComputed = true;
  bool needToUpdate = false;
  static Events_ID anUpdateEvent = Events_Loop::eventByName(EVENT_OBJECT_UPDATED);
  // to avoid redisplay of each segment on update by solver one by one in the viewer
  bool isUpdateFlushed = Events_Loop::loop()->isFlushed(anUpdateEvent);
  if (isUpdateFlushed) {
    Events_Loop::loop()->setFlushed(anUpdateEvent, false);
  }

  std::vector<SketchSolver_Group*>::iterator aGroupIter;
  for (aGroupIter = myGroups.begin(); aGroupIter != myGroups.end(); aGroupIter++)
    if ((*aGroupIter)->resolveConstraints())
      needToUpdate = true;

  // Features may be updated => now send events, but for all changed at once
  if (isUpdateFlushed) {
    Events_Loop::loop()->setFlushed(anUpdateEvent, true);
  }
  // Must be before flush because on "Updated" flush the results may be produced
  // and the creation event is appeared with many new objects. If myIsComputed these
  // events are missed in processEvents and some elements are not added.
  myIsComputed = false;
  if (needToUpdate || theForceUpdate)
    Events_Loop::loop()->flush(anUpdateEvent);
}
