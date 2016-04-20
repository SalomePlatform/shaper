// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:    SketchSolver_Manager.cpp
// Created: 08 May 2014
// Author:  Artem ZHIDKOV

#include "SketchSolver_Manager.h"
#include "SketchSolver_Error.h"

#include <Events_Loop.h>
#include <ModelAPI_AttributeDouble.h>
#include <ModelAPI_AttributeRefList.h>
#include <ModelAPI_Data.h>
#include <ModelAPI_Events.h>
#include <ModelAPI_Object.h>
#include <ModelAPI_ResultConstruction.h>
#include <ModelAPI_Attribute.h>
#include <ModelAPI_AttributeInteger.h>
#include <ModelAPI_AttributeString.h>

#include <SketchPlugin_Constraint.h>
#include <SketchPlugin_ConstraintAngle.h>
#include <SketchPlugin_ConstraintCoincidence.h>
#include <SketchPlugin_ConstraintCollinear.h>
#include <SketchPlugin_ConstraintDistance.h>
#include <SketchPlugin_ConstraintEqual.h>
#include <SketchPlugin_ConstraintHorizontal.h>
#include <SketchPlugin_ConstraintLength.h>
#include <SketchPlugin_ConstraintMiddle.h>
#include <SketchPlugin_ConstraintMirror.h>
#include <SketchPlugin_ConstraintParallel.h>
#include <SketchPlugin_ConstraintPerpendicular.h>
#include <SketchPlugin_ConstraintRadius.h>
#include <SketchPlugin_ConstraintRigid.h>
#include <SketchPlugin_ConstraintTangent.h>
#include <SketchPlugin_ConstraintVertical.h>
#include <SketchPlugin_MultiRotation.h>
#include <SketchPlugin_MultiTranslation.h>

#include <SketchPlugin_Arc.h>
#include <SketchPlugin_Circle.h>
#include <SketchPlugin_Line.h>
#include <SketchPlugin_Point.h>
#include <SketchPlugin_Sketch.h>
#include <SketchPlugin_Feature.h>

#include <assert.h>
#include <list>
#include <set>
#include <memory>
#include <sstream>

static const Events_ID anUpdateEvent = Events_Loop::eventByName(EVENT_OBJECT_UPDATED);

// Initialization of constraint manager self pointer
SketchSolver_Manager* SketchSolver_Manager::mySelf = 0;

/// Global constraint manager object
SketchSolver_Manager* myManager = SketchSolver_Manager::instance();


// ========================================================
// ========= SketchSolver_Manager ===============
// ========================================================
SketchSolver_Manager* SketchSolver_Manager::instance()
{
  if (!mySelf)
    mySelf = new SketchSolver_Manager();
  return mySelf;
}

SketchSolver_Manager::SketchSolver_Manager()
{
  myGroups.clear();
  myIsComputed = false;

  // Register in event loop
  Events_Loop::loop()->registerListener(this, Events_Loop::eventByName(EVENT_OBJECT_CREATED));
  Events_Loop::loop()->registerListener(this, Events_Loop::eventByName(EVENT_OBJECT_UPDATED));
  Events_Loop::loop()->registerListener(this, Events_Loop::eventByName(EVENT_OBJECT_DELETED));
  Events_Loop::loop()->registerListener(this, Events_Loop::eventByName(EVENT_OBJECT_MOVED));

  Events_Loop::loop()->registerListener(this, Events_Loop::eventByName(EVENT_SOLVER_FAILED));
  Events_Loop::loop()->registerListener(this, Events_Loop::eventByName(EVENT_SOLVER_REPAIRED));
}

SketchSolver_Manager::~SketchSolver_Manager()
{
  myGroups.clear();
}

void SketchSolver_Manager::setBuilder(BuilderPtr theBuilder)
{
  myBuilder = theBuilder;
}

BuilderPtr SketchSolver_Manager::builder()
{
  return myBuilder;
}

// ============================================================================
//  Function: processEvent
//  Purpose:  listen the event loop and process the message
// ============================================================================
void SketchSolver_Manager::processEvent(
  const std::shared_ptr<Events_Message>& theMessage)
{
  checkConflictingConstraints(theMessage);
  if (myIsComputed)
    return;
  myIsComputed = true;
  if (theMessage->eventID() == Events_Loop::loop()->eventByName(EVENT_OBJECT_CREATED)
      || theMessage->eventID() == Events_Loop::loop()->eventByName(EVENT_OBJECT_UPDATED)
      || theMessage->eventID() == Events_Loop::loop()->eventByName(EVENT_OBJECT_MOVED)) {
    std::shared_ptr<ModelAPI_ObjectUpdatedMessage> anUpdateMsg =
        std::dynamic_pointer_cast<ModelAPI_ObjectUpdatedMessage>(theMessage);
    std::set<ObjectPtr> aFeatures = anUpdateMsg->objects();

    bool isUpdateFlushed = stopSendUpdate();
    // Shows the message has at least one feature applicable for solver
    bool hasProperFeature = false;

    bool isMovedEvt = theMessage->eventID()
          == Events_Loop::loop()->eventByName(EVENT_OBJECT_MOVED);
    if (isMovedEvt) {
      std::set<ObjectPtr>::iterator aFeatIter;
      for (aFeatIter = aFeatures.begin(); aFeatIter != aFeatures.end(); aFeatIter++) {
        std::shared_ptr<SketchPlugin_Feature> aSFeature = 
            std::dynamic_pointer_cast<SketchPlugin_Feature>(*aFeatIter);
        if (aSFeature) {
          moveEntity(aSFeature);
          hasProperFeature = true;
        }
      }
    } else {
      std::list<FeaturePtr> aSketchFeatures = SketchSolver_Group::selectApplicableFeatures(aFeatures);
      std::list<FeaturePtr>::iterator aFeatIter = aSketchFeatures.begin();
      for (; aFeatIter != aSketchFeatures.end(); ++aFeatIter) {
        if ((*aFeatIter)->getKind() == SketchPlugin_Sketch::ID()) {
          std::shared_ptr<ModelAPI_CompositeFeature> aSketch = 
              std::dynamic_pointer_cast<ModelAPI_CompositeFeature>(*aFeatIter);
          hasProperFeature = changeWorkplane(aSketch) || hasProperFeature;
          continue;
        }
        std::shared_ptr<SketchPlugin_Feature> aFeature = 
            std::dynamic_pointer_cast<SketchPlugin_Feature>(*aFeatIter);
        if (!aFeature)
          continue;
        hasProperFeature = changeFeature(aFeature) || hasProperFeature;
      }
    }

    bool needToUpdate = false;
    // Solve the set of constraints
    if (hasProperFeature)
      needToUpdate = resolveConstraints();

    // Features may be updated => now send events, but for all changed at once
    if (isUpdateFlushed)
      allowSendUpdate();
    // send update for movement in any case
    if (needToUpdate || isMovedEvt)
      Events_Loop::loop()->flush(anUpdateEvent);

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
      std::list<SketchSolver_Group*> aGroupsToResolve;
      std::list<SketchSolver_Group*>::iterator aGroupIter = myGroups.begin();
      std::list<SketchSolver_Group*> aSeparatedGroups;
      while (aGroupIter != myGroups.end()) {
        if (!(*aGroupIter)->isWorkplaneValid()) {  // the group should be removed
          delete *aGroupIter;
          std::list<SketchSolver_Group*>::iterator aRemoveIt = aGroupIter++;
          myGroups.erase(aRemoveIt);
          continue;
        }
        if (!(*aGroupIter)->isConsistent()) {  // some constraints were removed, try to split the group
          (*aGroupIter)->splitGroup(aSeparatedGroups);
          //if (!(*aGroupIter)->getWorkplane()->string(
          //    SketchPlugin_Sketch::SOLVER_ERROR())->value().empty())
            aGroupsToResolve.push_back(*aGroupIter);
        }
        aGroupIter++;
      }
      if (aSeparatedGroups.size() > 0) {
        myGroups.insert(myGroups.end(), aSeparatedGroups.begin(), aSeparatedGroups.end());
        aGroupsToResolve.insert(aGroupsToResolve.end(),
            aSeparatedGroups.begin(), aSeparatedGroups.end());
      }

      if (!aGroupsToResolve.empty())
        resolveConstraints(aGroupsToResolve);
    }
  }
  degreesOfFreedom();
  myIsComputed = false;
}

void SketchSolver_Manager::checkConflictingConstraints(const std::shared_ptr<Events_Message>& theMessage)
{
  if (theMessage->eventID() == Events_Loop::loop()->eventByName(EVENT_SOLVER_REPAIRED)) {
    std::shared_ptr<ModelAPI_SolverFailedMessage> aMessage =
        std::dynamic_pointer_cast<ModelAPI_SolverFailedMessage>(theMessage);
    std::set<ObjectPtr> aSentObjs = aMessage->objects();
    if (!aSentObjs.empty()) {
      // Obtain sketch where the constraints are placed.
      // It is enough to check only one constraint.
      CompositeFeaturePtr aSketch;
      FeaturePtr aConstraint = ModelAPI_Feature::feature(*aSentObjs.begin());
      std::list<SketchSolver_Group*>::const_iterator aGrIt = myGroups.begin();
      for (; aGrIt != myGroups.end(); ++aGrIt)
        if ((*aGrIt)->isInteract(aConstraint)) {
          aSketch = (*aGrIt)->getWorkplane();
          break;
        }

      // Search failed groups built on the same sketch
      if (aSketch) {
        for (aGrIt = myGroups.begin(); aGrIt != myGroups.end(); ++aGrIt) {
          SketchSolver_Group* aGroup = *aGrIt;
          if (aGroup->isBaseWorkplane(aSketch) && aGroup->isFailed() &&
              !aGroup->isInteract(aConstraint)) {
            // reset error message on the sketch
            aGroup->getWorkplane()->string(SketchPlugin_Sketch::SOLVER_ERROR())->setValue(
                SketchSolver_Error::CONSTRAINTS());
            Events_Loop::loop()->flush(Events_Loop::eventByName(EVENT_OBJECT_UPDATED));
            break;
          }
        }
      }
    }
  }
}

// ============================================================================
//  Function: changeWorkplane
//  Purpose:  update workplane by given parameters of the sketch
// ============================================================================
bool SketchSolver_Manager::changeWorkplane(CompositeFeaturePtr theSketch)
{
  bool aResult = true;  // changed when a workplane wrongly updated
  bool isUpdated = false;
  // Try to update specified workplane in all groups
  std::list<SketchSolver_Group*>::iterator aGroupIter;
  for (aGroupIter = myGroups.begin(); aGroupIter != myGroups.end(); aGroupIter++)
    if ((*aGroupIter)->isBaseWorkplane(theSketch)) {
      isUpdated = true;
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
//  Purpose:  create/update the constraint or the feature and place it into appropriate group
// ============================================================================
bool SketchSolver_Manager::changeFeature(std::shared_ptr<SketchPlugin_Feature> theFeature)
{
  // Search the groups which this feature touches
  std::set<GroupID> aGroups;
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
    GroupID aGroupId = *(aGroups.begin());
    std::list<SketchSolver_Group*>::iterator aGroupIter;
    for (aGroupIter = myGroups.begin(); aGroupIter != myGroups.end(); aGroupIter++)
      if ((*aGroupIter)->getId() == aGroupId) {
        // If the group is empty, the feature is not added (the constraint only)
        if (!aConstraint && !(*aGroupIter)->isEmpty())
          return (*aGroupIter)->updateFeature(theFeature);
        return (*aGroupIter)->changeConstraint(aConstraint);
      }
  } else if (aGroups.size() > 1) {  // Several groups applicable for this feature => need to merge them
    std::set<GroupID>::const_iterator aGroupsIter = aGroups.begin();

    // Search first group
    std::list<SketchSolver_Group*>::iterator aFirstGroupIter;
    for (aFirstGroupIter = myGroups.begin(); aFirstGroupIter != myGroups.end(); aFirstGroupIter++)
      if ((*aFirstGroupIter)->getId() == *aGroupsIter)
        break;
    if (aFirstGroupIter == myGroups.end())
      return false;

    // Append other groups to the first one
    std::list<SketchSolver_Group*>::iterator anOtherGroupIter = aFirstGroupIter;
    ++anOtherGroupIter;
    for (aGroupsIter++; aGroupsIter != aGroups.end(); aGroupsIter++) {
      for (; anOtherGroupIter != myGroups.end(); anOtherGroupIter++)
        if ((*anOtherGroupIter)->getId() == *aGroupsIter)
          break;
      if (anOtherGroupIter == myGroups.end()) {  // Group disappears
        anOtherGroupIter = aFirstGroupIter;
        ++anOtherGroupIter;
        continue;
      }

      (*aFirstGroupIter)->mergeGroups(**anOtherGroupIter);
      std::list<SketchSolver_Group*>::iterator aRemoveIt = anOtherGroupIter++;
      delete *aRemoveIt;
      myGroups.erase(aRemoveIt);
    }

    if (aConstraint)
      (*aFirstGroupIter)->changeConstraint(aConstraint);
    else
      (*aFirstGroupIter)->updateFeature(theFeature);
    // groups are merged => need to resolve them
    return true;
  }

  // Something goes wrong
  return false;
}

// ============================================================================
//  Function: moveEntity
//  Purpose:  update element moved on the sketch, which is used by constraints
// ============================================================================
void SketchSolver_Manager::moveEntity(std::shared_ptr<SketchPlugin_Feature> theFeature)
{
  bool isMoved = false;
  std::list<SketchSolver_Group*>::iterator aGroupIt = myGroups.begin();
  for (; aGroupIt != myGroups.end(); aGroupIt++)
    if (!(*aGroupIt)->isEmpty() && (*aGroupIt)->isInteract(theFeature)) {
      (*aGroupIt)->moveFeature(theFeature);
      isMoved = true;
    }

  if (!isMoved && theFeature->getKind() == SketchPlugin_Arc::ID()) {
    // Workaround to move arc.
    // If the arc has not been constrained, we will push it into empty group and apply movement.
    for (aGroupIt = myGroups.begin(); aGroupIt != myGroups.end(); aGroupIt++)
      if ((*aGroupIt)->isEmpty())
        (*aGroupIt)->moveFeature(theFeature);
  }
}

// ============================================================================
//  Function: findGroups
//  Purpose:  search groups of entities interacting with given feature
// ============================================================================
void SketchSolver_Manager::findGroups(
    std::shared_ptr<SketchPlugin_Feature> theFeature,
    std::set<GroupID>& theGroupIDs) const
{
  std::shared_ptr<ModelAPI_CompositeFeature> aWP = findWorkplane(theFeature);

  SketchSolver_Group* anEmptyGroup = 0;  // appropriate empty group for specified constraint
  std::list<SketchSolver_Group*>::const_iterator aGroupIter;
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
//  Purpose:  search workplane containing given feature
// ============================================================================
std::shared_ptr<ModelAPI_CompositeFeature> SketchSolver_Manager
::findWorkplane(std::shared_ptr<SketchPlugin_Feature> theFeature) const
{
  // Already verified workplanes
  std::set<std::shared_ptr<ModelAPI_CompositeFeature> > aVerified;

  std::list<SketchSolver_Group*>::const_iterator aGroupIter;
  for (aGroupIter = myGroups.begin(); aGroupIter != myGroups.end(); aGroupIter++) {
    std::shared_ptr<ModelAPI_CompositeFeature> aWP = (*aGroupIter)->getWorkplane();
    if (aVerified.find(aWP) != aVerified.end())
      continue;

    DataPtr aData = aWP->data();
    if (aData->isValid()) {
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
//  Purpose:  change entities according to available constraints
// ============================================================================
bool SketchSolver_Manager::resolveConstraints(const std::list<SketchSolver_Group*>& theGroups)
{
  bool needToUpdate = false;
  const std::list<SketchSolver_Group*>& aGroupsToResolve = theGroups.empty() ? myGroups : theGroups;
  std::list<SketchSolver_Group*>::const_iterator aGroupIter = aGroupsToResolve.begin();
  for (; aGroupIter != aGroupsToResolve.end(); aGroupIter++)
    if ((*aGroupIter)->resolveConstraints())
      needToUpdate = true;
  return needToUpdate;
}

// ============================================================================
//  Function: degreesOfFreedom
//  Purpose:  calculate DoFs for each sketch
// ============================================================================
void SketchSolver_Manager::degreesOfFreedom()
{
  static std::map<std::string, int> aDoFDelta; // indicates how many DoF adds or decreases a feature
  static bool isNeedInit = true;
  if (isNeedInit) {
    aDoFDelta[SketchPlugin_Point::ID()] = 2;
    aDoFDelta[SketchPlugin_Line::ID()] = 4;
    aDoFDelta[SketchPlugin_Circle::ID()] = 3;
    aDoFDelta[SketchPlugin_Arc::ID()] = 5;

    aDoFDelta[SketchPlugin_ConstraintAngle::ID()] = -1;
    aDoFDelta[SketchPlugin_ConstraintCollinear::ID()] = -1;
    aDoFDelta[SketchPlugin_ConstraintDistance::ID()] = -1;
    aDoFDelta[SketchPlugin_ConstraintEqual::ID()] = -1;
    aDoFDelta[SketchPlugin_ConstraintHorizontal::ID()] = -1;
    aDoFDelta[SketchPlugin_ConstraintLength::ID()] = -1;
    aDoFDelta[SketchPlugin_ConstraintMiddle::ID()] = -1;
    aDoFDelta[SketchPlugin_ConstraintParallel::ID()] = -1;
    aDoFDelta[SketchPlugin_ConstraintPerpendicular::ID()] = -1;
    aDoFDelta[SketchPlugin_ConstraintRadius::ID()] = -1;
    aDoFDelta[SketchPlugin_ConstraintTangent::ID()] = -1;
    aDoFDelta[SketchPlugin_ConstraintVertical::ID()] = -1;

    isNeedInit = false;
  }

  std::map<CompositeFeaturePtr, int> aSketchDoF;

  std::list<SketchSolver_Group*>::const_iterator aGroupIt = myGroups.begin();
  for (; aGroupIt != myGroups.end(); ++aGroupIt) {
    CompositeFeaturePtr aSketch = (*aGroupIt)->getWorkplane();
    if (!aSketch->data()->isValid()) {
      myDoF.erase(aSketch);
      continue;
    }

    // check conflicting constraints in the group
    if ((*aGroupIt)->isFailed())
      aSketchDoF[aSketch] = -1;
    // check the sketch is already processed
    if (aSketchDoF.find(aSketch) != aSketchDoF.end() || aSketchDoF[aSketch] < 0)
      continue;

    int aDoF = 0;
    int aNbSubs = aSketch->numberOfSubs();
    for (int i = 0; i < aNbSubs; ++i) {
      FeaturePtr aFeature = aSketch->subFeature(i);
      // check DoF delta for invariant types
      std::map<std::string, int>::const_iterator aFound = aDoFDelta.find(aFeature->getKind());
      if (aFound != aDoFDelta.end()) {
        aDoF += aFound->second;
        continue;
      }

      // DoF delta in specific cases
      if (aFeature->getKind() == SketchPlugin_ConstraintCoincidence::ID()) {
        for (int j = 0; j < 2; ++j) {
          AttributeRefAttrPtr aRefAttr = std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(
              aFeature->attribute(SketchPlugin_Constraint::ATTRIBUTE(j)));
          if (!aRefAttr)
            continue;
          bool isPoint = !aRefAttr->isObject();
          if (!isPoint) {
            FeaturePtr anAttr = ModelAPI_Feature::feature(aRefAttr->object());
            isPoint = anAttr && anAttr->getKind() == SketchPlugin_Point::ID();
          }
          if (isPoint)
            aDoF -= 1;
        }
      }
      else if (aFeature->getKind() == SketchPlugin_ConstraintRigid::ID()) {
        AttributeRefAttrPtr aRefAttr = std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(
            aFeature->attribute(SketchPlugin_Constraint::ENTITY_A()));
        assert(aRefAttr);
        if (!aRefAttr->isObject())
          aDoF -= 2; // attribute is a point
        else {
          FeaturePtr anAttr = ModelAPI_Feature::feature(aRefAttr->object());
          assert(anAttr);
          aDoF -= aDoFDelta[anAttr->getKind()];
        }
      }
      else if (aFeature->getKind() == SketchPlugin_ConstraintMirror::ID() ||
               aFeature->getKind() == SketchPlugin_MultiRotation::ID() ||
               aFeature->getKind() == SketchPlugin_MultiTranslation::ID()) {
        int aNbCopies = 1;
        std::string anAttrName;
        if (aFeature->getKind() == SketchPlugin_ConstraintMirror::ID())
          anAttrName = SketchPlugin_Constraint::ENTITY_B();
        else {
          if (aFeature->getKind() == SketchPlugin_MultiRotation::ID())
            aNbCopies = aFeature->integer(SketchPlugin_MultiRotation::NUMBER_OF_OBJECTS_ID())->value() - 1;
          else if (aFeature->getKind() == SketchPlugin_MultiTranslation::ID())
            aNbCopies = aFeature->integer(SketchPlugin_MultiTranslation::NUMBER_OF_OBJECTS_ID())->value() - 1;
          anAttrName = SketchPlugin_Constraint::ENTITY_A();
        }

        AttributeRefListPtr aRefListOfShapes = std::dynamic_pointer_cast<ModelAPI_AttributeRefList>(
            aFeature->attribute(anAttrName));
        std::list<ObjectPtr> anObjList = aRefListOfShapes->list();
        std::list<ObjectPtr>::const_iterator anObjIt = anObjList.begin();
        for (; anObjIt != anObjList.end(); ++anObjIt) {
          FeaturePtr aSub = ModelAPI_Feature::feature(*anObjIt);
          aDoF -= aDoFDelta[aSub->getKind()] * aNbCopies;
        }
      }
    }

    aSketchDoF[aSketch] = aDoF;
  }

  // Check the degrees of freedom are changed
  std::map<CompositeFeaturePtr, int>::const_iterator aDoFIt = aSketchDoF.begin();
  std::map<CompositeFeaturePtr, int>::iterator aFound;
  for (; aDoFIt != aSketchDoF.end(); ++aDoFIt) {
    if (aDoFIt->second < 0)
      continue; // conflicting constraints on the current sketch
    aFound = myDoF.find(aDoFIt->first);
    if (aFound != myDoF.end() && aFound->second == aDoFIt->second)
      continue; // nothing is changed
    myDoF[aDoFIt->first] = aDoFIt->second;
    // change attribute value
    std::ostringstream aStream;
    std::string aValue = "DOF(degree of freedom) = ";
    aStream << aDoFIt->second;
    aDoFIt->first->data()->string(SketchPlugin_Sketch::SOLVER_DOF())->setValue(aValue);

    Events_Loop::loop()->flush(Events_Loop::eventByName(EVENT_OBJECT_UPDATED));
  }
}

bool SketchSolver_Manager::stopSendUpdate() const
{
  // to avoid redisplay of each segment on update by solver one by one in the viewer
  bool isUpdateFlushed = Events_Loop::loop()->isFlushed(anUpdateEvent);
  if (isUpdateFlushed) {
    Events_Loop::loop()->setFlushed(anUpdateEvent, false);
  }
  return isUpdateFlushed;
}

void SketchSolver_Manager::allowSendUpdate() const
{
  Events_Loop::loop()->setFlushed(anUpdateEvent, true);
}
