// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:    SketchSolver_Manager.cpp
// Created: 08 May 2014
// Author:  Artem ZHIDKOV

#include "SketchSolver_Manager.h"
#include "SketchSolver_Error.h"

#include <Events_Loop.h>

#include <GeomDataAPI_Point2D.h>

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
  Events_Loop::loop()->registerListener(this, anUpdateEvent);
  Events_Loop::loop()->registerListener(this, Events_Loop::eventByName(EVENT_OBJECT_DELETED));
  Events_Loop::loop()->registerListener(this, Events_Loop::eventByName(EVENT_OBJECT_MOVED));

  Events_Loop::loop()->registerListener(this, Events_Loop::eventByName(EVENT_SOLVER_FAILED));
  Events_Loop::loop()->registerListener(this, Events_Loop::eventByName(EVENT_SOLVER_REPAIRED));
  Events_Loop::loop()->registerListener(this, Events_Loop::eventByName(EVENT_SKETCH_PREPARED));
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

bool SketchSolver_Manager::groupMessages()
{
  return true;
}

// ============================================================================
//  Function: processEvent
//  Purpose:  listen the event loop and process the message
// ============================================================================
void SketchSolver_Manager::processEvent(
  const std::shared_ptr<Events_Message>& theMessage)
{
  static const Events_ID aSketchPreparedEvent = Events_Loop::eventByName(EVENT_SKETCH_PREPARED);
  // sketch is prepared for resolve: all the needed events
  // are collected and must be processed by the solver
  if (theMessage->eventID() == aSketchPreparedEvent) {
    flushGrouped(anUpdateEvent);
    return;
  }

  checkConflictingConstraints(theMessage);

  if (myIsComputed)
    return;
  myIsComputed = true;

  // Shows that the message has at least one feature applicable for solver
  bool hasProperFeature = false;

  if (theMessage->eventID() == Events_Loop::loop()->eventByName(EVENT_OBJECT_CREATED)
      || theMessage->eventID() == anUpdateEvent
      || theMessage->eventID() == Events_Loop::loop()->eventByName(EVENT_OBJECT_MOVED)) {
    std::shared_ptr<ModelAPI_ObjectUpdatedMessage> anUpdateMsg =
        std::dynamic_pointer_cast<ModelAPI_ObjectUpdatedMessage>(theMessage);
    std::set<ObjectPtr> aFeatures = anUpdateMsg->objects();

    bool isUpdateFlushed = stopSendUpdate();

    bool isMovedEvt = theMessage->eventID()
          == Events_Loop::loop()->eventByName(EVENT_OBJECT_MOVED);
    if (isMovedEvt) {
      std::set<ObjectPtr>::iterator aFeatIter;
      for (aFeatIter = aFeatures.begin(); aFeatIter != aFeatures.end(); aFeatIter++) {
        std::shared_ptr<SketchPlugin_Feature> aSFeature =
            std::dynamic_pointer_cast<SketchPlugin_Feature>(*aFeatIter);
        if (aSFeature && moveEntity(aSFeature)) {
          // Want to avoid recalculation of DoF too frequently.
          // So, set the flag when the feature is really moved.
          hasProperFeature = true;
        }
      }
      if (!hasProperFeature)
        // in this iteration it will compute nothing, so, no problem with recursion
        // it is important that solver flushes signal updated after processing move signal as there
        // is optimization that relies on this update, might be found by key "optimization"
        myIsComputed = false;
    } else {
      std::list<FeaturePtr> aSketchFeatures =
        SketchSolver_Group::selectApplicableFeatures(aFeatures);
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

    myIsComputed = false;

    // send update for movement in any case
    if (needToUpdate || isMovedEvt)
      Events_Loop::loop()->flush(anUpdateEvent);

  } else if (theMessage->eventID() == Events_Loop::loop()->eventByName(EVENT_OBJECT_DELETED)) {
    std::shared_ptr<ModelAPI_ObjectDeletedMessage> aDeleteMsg =
      std::dynamic_pointer_cast<ModelAPI_ObjectDeletedMessage>(theMessage);
    const std::set<std::string>& aFeatureGroups = aDeleteMsg->groups();

    // Find SketchPlugin_Sketch::ID() in groups.
    // The constraint groups should be updated when an object removed from Sketch
    std::set<std::string>::const_iterator aFGrIter;
    for (aFGrIter = aFeatureGroups.begin(); aFGrIter != aFeatureGroups.end(); aFGrIter++)
      if (aFGrIter->compare(ModelAPI_ResultConstruction::group()) == 0 ||
        aFGrIter->compare(ModelAPI_Feature::group()) == 0)
        break;

    if (aFGrIter != aFeatureGroups.end()) {
      hasProperFeature = true;
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
        if (!(*aGroupIter)->isConsistent()) {
          // some constraints were removed, try to split the group
          (*aGroupIter)->splitGroup(aSeparatedGroups);
          if (!(*aGroupIter)->getWorkplane()->string(
              SketchPlugin_Sketch::SOLVER_ERROR())->value().empty() ||
              (*aGroupIter)->isFailed())
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
    myIsComputed = false;
  }

  if (hasProperFeature)
    degreesOfFreedom();
}

void SketchSolver_Manager::
  checkConflictingConstraints(const std::shared_ptr<Events_Message>& theMessage)
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
  return aResult || isUpdated;
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
  } else if (aGroups.size() > 1) {
    // Several groups applicable for this feature => need to merge them
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
bool SketchSolver_Manager::moveEntity(std::shared_ptr<SketchPlugin_Feature> theFeature)
{
  bool isMoved = false;
  std::list<SketchSolver_Group*>::iterator aGroupIt = myGroups.begin();
  for (; aGroupIt != myGroups.end(); aGroupIt++)
    if (!(*aGroupIt)->isEmpty() && (*aGroupIt)->isInteract(theFeature))
      isMoved = (*aGroupIt)->moveFeature(theFeature) || isMoved;

  if (!isMoved && theFeature->getKind() == SketchPlugin_Arc::ID()) {
    // Workaround to move arc.
    // If the arc has not been constrained, we will push it into empty group and apply movement.
    bool hasEmptyGroup = false;
    for (aGroupIt = myGroups.begin(); aGroupIt != myGroups.end(); aGroupIt++)
      if ((*aGroupIt)->isEmpty()) {
        isMoved = (*aGroupIt)->moveFeature(theFeature) || isMoved;
        hasEmptyGroup = true;
      }
    // There is no empty group, create it explicitly
    if (!hasEmptyGroup) {
      // find sketch containing the arc
      CompositeFeaturePtr aWP;
      const std::set<AttributePtr>& aRefs = theFeature->data()->refsToMe();
      std::set<AttributePtr>::const_iterator aRefIt = aRefs.begin();
      for (; aRefIt != aRefs.end(); ++aRefIt) {
        FeaturePtr anOwner = ModelAPI_Feature::feature((*aRefIt)->owner());
        if (anOwner && anOwner->getKind() == SketchPlugin_Sketch::ID()) {
          aWP = std::dynamic_pointer_cast<ModelAPI_CompositeFeature>(anOwner);
          break;
        }
      }
      if (aWP) {
        SketchSolver_Group* aGroup = new SketchSolver_Group(aWP);
        isMoved = aGroup->moveFeature(theFeature) || isMoved;
        myGroups.push_back(aGroup);
      }
    }
  }
  return isMoved;
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
  const std::list<SketchSolver_Group*>& aGroupsToResolve = theGroups.empty() ?
                                                           myGroups : theGroups;
  std::list<SketchSolver_Group*>::const_iterator aGroupIter = aGroupsToResolve.begin();
  for (; aGroupIter != aGroupsToResolve.end(); aGroupIter++)
    if ((*aGroupIter)->resolveConstraints())
      needToUpdate = true;
  return needToUpdate;
}


// Obtain points and their copies for Mirror, Multi-Rotation and Multi-Translation constraints
static void collectPointsAndCopies(FeaturePtr theConstraint,
                                   std::list<std::set<AttributePtr> >& thePoints)
{
  typedef std::list<std::string> strlist;
  static strlist aPointAttributes(1, SketchPlugin_Point::COORD_ID());
  static strlist aLineAttributes;
  if (aLineAttributes.empty()) {
    aLineAttributes.push_back(SketchPlugin_Line::START_ID());
    aLineAttributes.push_back(SketchPlugin_Line::END_ID());
  };
  static strlist aCircleAttributes(1, SketchPlugin_Circle::CENTER_ID());
  static strlist anArcAttributes;
  if (anArcAttributes.empty()) {
    anArcAttributes.push_back(SketchPlugin_Arc::CENTER_ID());
    anArcAttributes.push_back(SketchPlugin_Arc::START_ID());
    anArcAttributes.push_back(SketchPlugin_Arc::END_ID());
  };

  static std::map<std::string, strlist> aFeatureAttributes;
  if (aFeatureAttributes.empty()) {
    aFeatureAttributes[SketchPlugin_Point::ID()] = aPointAttributes;
    aFeatureAttributes[SketchPlugin_Line::ID()] = aLineAttributes;
    aFeatureAttributes[SketchPlugin_Circle::ID()] = aCircleAttributes;
    aFeatureAttributes[SketchPlugin_Arc::ID()] = anArcAttributes;
  }


  std::set<AttributePtr> aPoints;
  if (theConstraint->getKind() == SketchPlugin_ConstraintMirror::ID()) {
    AttributeRefListPtr aBaseRefList = theConstraint->reflist(SketchPlugin_Constraint::ENTITY_B());
    AttributeRefListPtr aMirrRefList = theConstraint->reflist(SketchPlugin_Constraint::ENTITY_C());

    std::list<ObjectPtr> aBaseList = aBaseRefList->list();
    std::list<ObjectPtr> aMirrList = aMirrRefList->list();
    std::list<ObjectPtr>::const_iterator aBIt, aMIt;
    for (aBIt = aBaseList.begin(), aMIt = aMirrList.begin();
         aBIt != aBaseList.end() && aMIt != aMirrList.end(); ++aBIt, ++aMIt) {
      FeaturePtr aBaseFeature = ModelAPI_Feature::feature(*aBIt);
      FeaturePtr aMirrFeature = ModelAPI_Feature::feature(*aMIt);

      strlist anAttrList = aFeatureAttributes[aBaseFeature->getKind()];
      strlist::iterator anIt = anAttrList.begin();
      for (; anIt != anAttrList.end(); ++anIt) {
        aPoints.clear();
        aPoints.insert(aBaseFeature->attribute(*anIt));
        aPoints.insert(aMirrFeature->attribute(*anIt));
        thePoints.push_back(aPoints);
      }
    }
  }
  else { // the "Multi" constraints
    std::string aNbObjName;
    if (theConstraint->getKind() == SketchPlugin_MultiRotation::ID())
      aNbObjName = SketchPlugin_MultiRotation::NUMBER_OF_OBJECTS_ID();
    else
      aNbObjName = SketchPlugin_MultiTranslation::NUMBER_OF_OBJECTS_ID();
    int aNbCopies = theConstraint->integer(aNbObjName)->value();

    AttributeRefListPtr aRefList = theConstraint->reflist(SketchPlugin_Constraint::ENTITY_B());
    std::list<ObjectPtr> aFullList = aRefList->list();
    std::list<ObjectPtr>::const_iterator anObjIt = aFullList.begin();
    std::list<ObjectPtr>::const_iterator aCopyIt;
    while (anObjIt != aFullList.end()) {
      FeaturePtr aBaseFeature = ModelAPI_Feature::feature(*anObjIt);
      strlist anAttrList = aFeatureAttributes[aBaseFeature->getKind()];
      strlist::iterator anIt = anAttrList.begin();
      for (; anIt != anAttrList.end(); ++anIt) {
        aPoints.clear();
        aCopyIt = anObjIt;
        for (int i = 0; i < aNbCopies && aCopyIt != aFullList.end(); ++i, ++aCopyIt) {
          FeaturePtr aFeature = ModelAPI_Feature::feature(*aCopyIt);
          aPoints.insert(aFeature->attribute(*anIt));
        }
        thePoints.push_back(aPoints);
      }
      anObjIt = aCopyIt;
    }
  }
}


// returns true if the feature is external
static bool isExternal(const FeaturePtr& theFeature)
{
  AttributeSelectionPtr anAttr = theFeature->selection(SketchPlugin_SketchEntity::EXTERNAL_ID());
  return anAttr && anAttr->context() && !anAttr->isInvalid();
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
    bool isSketchValid = aSketch->data() && aSketch->data()->isValid();

    if (isSketchValid) {
      std::shared_ptr<GeomDataAPI_Dir> aNormal =
          std::dynamic_pointer_cast<GeomDataAPI_Dir>(
          aSketch->data()->attribute(SketchPlugin_Sketch::NORM_ID()));
      isSketchValid = aNormal && aNormal->isInitialized();
    }

    if (!isSketchValid) {
      myDoF.erase(aSketch);
      continue;
    }

    // check conflicting constraints in the group
    if ((*aGroupIt)->isFailed())
      aSketchDoF[aSketch] = -1;
    // check the sketch is already processed
    if (aSketchDoF.find(aSketch) != aSketchDoF.end() || aSketchDoF[aSketch] < 0)
      continue;

    std::set<AttributePtr> aCoincidentPoints;
    std::set<AttributePtr> aFixedPoints;
    std::map<AttributePtr, std::set<FeaturePtr> > aPointOnLine;
    std::list<std::set<AttributePtr> > aPointsInMultiConstraints;
    int aDoF = 0;
    int aNbSubs = aSketch->numberOfSubs();
    for (int i = 0; i < aNbSubs; ++i) {
      FeaturePtr aFeature = aSketch->subFeature(i);
      // do not change DoF for external feature
      if (isExternal(aFeature))
        continue;
      // check DoF delta for invariant types
      std::map<std::string, int>::const_iterator aFound = aDoFDelta.find(aFeature->getKind());
      if (aFound != aDoFDelta.end()) {
        aDoF += aFound->second;
        continue;
      }

      // DoF delta in specific cases
      if (aFeature->getKind() == SketchPlugin_ConstraintCoincidence::ID()) {
        AttributePtr aCoincPoint[2] = {AttributePtr(), AttributePtr()};
        FeaturePtr aCoincLine;
        for (int j = 0; j < 2; ++j) {
          AttributeRefAttrPtr aRefAttr = std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(
              aFeature->attribute(SketchPlugin_Constraint::ATTRIBUTE(j)));
          if (!aRefAttr)
            continue;
          if (!aRefAttr->isObject())
            aCoincPoint[j] = aRefAttr->attr();
          else {
            FeaturePtr anAttr = ModelAPI_Feature::feature(aRefAttr->object());
            if (!anAttr)
              continue;
            if (anAttr->getKind() == SketchPlugin_Point::ID())
              aCoincPoint[j] = anAttr->attribute(SketchPlugin_Point::COORD_ID());
            else if (anAttr->getKind() == SketchPlugin_Line::ID())
              aCoincLine = anAttr;
          }
        }
        if (aCoincPoint[0] && aCoincPoint[1]) {
          bool isDoFDecreased = false;
          // point-point coincidence
          if (aCoincidentPoints.find(aCoincPoint[0]) == aCoincidentPoints.end() ||
              aCoincidentPoints.find(aCoincPoint[1]) == aCoincidentPoints.end()) {
            aDoF -= 2;
            isDoFDecreased = true;
          }
          // check the coincident point is used in "multi" constraints
          std::list<std::set<AttributePtr> >::const_iterator
              aPtIt = aPointsInMultiConstraints.begin();
          bool isFound[2] = {false, false};
          for (; aPtIt != aPointsInMultiConstraints.end(); ++aPtIt) {
            if ((!isFound[0] && (isFound[0] = (aPtIt->find(aCoincPoint[0]) != aPtIt->end())))
             || (!isFound[1] && (isFound[1] = (aPtIt->find(aCoincPoint[1]) != aPtIt->end()))))
              aCoincidentPoints.insert(aPtIt->begin(), aPtIt->end());
            if (isFound[0] && isFound[1])
              break;
          }
          // check both points are fixed => not need to decrease DoF
          bool isFixed[2] = { aFixedPoints.find(aCoincPoint[0]) != aFixedPoints.end(),
                              aFixedPoints.find(aCoincPoint[1]) != aFixedPoints.end() };
          if (isFixed[0] && isFixed[1] && isDoFDecreased)
            aDoF += 2; // revert decrease of DoF
          else if (isFixed[0] && !isFixed[1])
            aFixedPoints.insert(aCoincPoint[1]);
          else if (!isFixed[0] && isFixed[1])
            aFixedPoints.insert(aCoincPoint[0]);
        } else {
          aDoF -= 1;
          if (aCoincPoint[0] && aCoincLine) {
            // if the point is already coincident to a line
            // (by middle point constraint), do not decrease DoF
            std::map<AttributePtr, std::set<FeaturePtr> >::iterator
                aPtFound = aPointOnLine.find(aCoincPoint[0]);
            if (aPtFound != aPointOnLine.end() &&
                aPtFound->second.find(aCoincLine) != aPtFound->second.end())
              aDoF += 1; // restore value decreased above
            else
              aPointOnLine[aCoincPoint[0]].insert(aCoincLine);
          }
        }
        for (int j = 0; j < 2; ++j)
          if (aCoincPoint[j])
            aCoincidentPoints.insert(aCoincPoint[j]);
      }
      else if (aFeature->getKind() == SketchPlugin_ConstraintMiddle::ID()) {
        AttributePtr aPoint;
        FeaturePtr aLine;
        for (int j = 0; j < 2; ++j) {
          AttributeRefAttrPtr aRefAttr = std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(
              aFeature->attribute(SketchPlugin_Constraint::ATTRIBUTE(j)));
          if (!aRefAttr)
            continue;
          if (aRefAttr->isObject())
            aLine = ModelAPI_Feature::feature(aRefAttr->object());
          else
            aPoint = aRefAttr->attr();
        }
        if (aPoint && aLine) {
          // if the point is already on the line, decrease 1 DoF, instead decrease 2 DoF
          std::map<AttributePtr, std::set<FeaturePtr> >::iterator
              aPtFound = aPointOnLine.find(aPoint);
          if (aPtFound != aPointOnLine.end() &&
              aPtFound->second.find(aLine) != aPtFound->second.end())
            aDoF -= 1;
          else {
            aDoF -= 2;
            aPointOnLine[aPoint].insert(aLine);
          }
        }
      }
      else if (aFeature->getKind() == SketchPlugin_ConstraintRigid::ID()) {
        AttributeRefAttrPtr aRefAttr = std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(
            aFeature->attribute(SketchPlugin_Constraint::ENTITY_A()));
        assert(aRefAttr);
        std::set<AttributePtr> aPoints;
        if (!aRefAttr->isObject()) {
          aDoF -= 2; // attribute is a point
          aPoints.insert(aRefAttr->attr());
        } else {
          FeaturePtr anAttr = ModelAPI_Feature::feature(aRefAttr->object());
          if (anAttr) {
            if (isExternal(anAttr))
              continue; // feature is already fixed since it is external
            aDoF -= aDoFDelta[anAttr->getKind()];
            std::list<AttributePtr> aPtAttrs =
              anAttr->data()->attributes(GeomDataAPI_Point2D::typeId());
            aPoints.insert(aPtAttrs.begin(), aPtAttrs.end());
          }
        }

        // Check whether feature's points are already coincident with fixed points.
        // In this case we need to revert decrease of DoF for these points.
        // If the coordinates of fixed points are different, it will be processed by solver.
        for (int k = 0; k < i; ++k) {
          FeaturePtr aFeature = aSketch->subFeature(k);
          if (aFeature->getKind() != SketchPlugin_ConstraintCoincidence::ID())
            continue;
          AttributePtr aCoincPoint[2] = {AttributePtr(), AttributePtr()};
          for (int j = 0; j < 2; ++j) {
            AttributeRefAttrPtr aRefAttr = std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(
                aFeature->attribute(SketchPlugin_Constraint::ATTRIBUTE(j)));
            if (!aRefAttr)
              continue;
            if (!aRefAttr->isObject())
              aCoincPoint[j] = aRefAttr->attr();
            else {
              FeaturePtr anAttr = ModelAPI_Feature::feature(aRefAttr->object());
              if (anAttr && anAttr->getKind() == SketchPlugin_Point::ID())
                aCoincPoint[j] = anAttr->attribute(SketchPlugin_Point::COORD_ID());
            }
          }
          if (aCoincPoint[0] && aCoincPoint[1]) {
            if ((aFixedPoints.find(aCoincPoint[0]) != aFixedPoints.end() &&
                 aPoints.find(aCoincPoint[1]) != aPoints.end()) ||
                (aFixedPoints.find(aCoincPoint[1]) != aFixedPoints.end() &&
                 aPoints.find(aCoincPoint[0]) != aPoints.end()))
              aDoF += 2; // point already fixed
          }
        }
        // store fixed points
        aFixedPoints.insert(aPoints.begin(), aPoints.end());
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
            aNbCopies =
            aFeature->integer(SketchPlugin_MultiRotation::NUMBER_OF_OBJECTS_ID())->value() - 1;
          else if (aFeature->getKind() == SketchPlugin_MultiTranslation::ID())
            aNbCopies =
            aFeature->integer(SketchPlugin_MultiTranslation::NUMBER_OF_OBJECTS_ID())->value() - 1;
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
        // collect points and their copies for correct calculation of DoF for coincident points
        collectPointsAndCopies(aFeature, aPointsInMultiConstraints);
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
    if (aDoFIt->second == 0)
      aStream << "Sketch fully fixed (DOF = " << aDoFIt->second << ")";
    else
      aStream << "DOF (degree of freedom) = " << aDoFIt->second;
    aDoFIt->first->data()->string(SketchPlugin_Sketch::SOLVER_DOF())->setValue(aStream.str());
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
