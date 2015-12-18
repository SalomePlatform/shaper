// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:    SketchSolver_Group.cpp
// Created: 27 May 2014
// Author:  Artem ZHIDKOV

#include "SketchSolver_Group.h"

#include <SketchSolver_Constraint.h>
#include <SketchSolver_ConstraintCoincidence.h>
#include <SketchSolver_ConstraintMulti.h>
#include <SketchSolver_Error.h>
#include <SketchSolver_Manager.h>

#include <Events_Error.h>
#include <Events_Loop.h>
#include <ModelAPI_AttributeString.h>
#include <ModelAPI_Events.h>
#include <ModelAPI_Session.h>
#include <ModelAPI_Validator.h>

#include <SketchPlugin_ConstraintAngle.h>
#include <SketchPlugin_ConstraintCoincidence.h>
#include <SketchPlugin_ConstraintDistance.h>
#include <SketchPlugin_ConstraintEqual.h>
#include <SketchPlugin_ConstraintHorizontal.h>
#include <SketchPlugin_ConstraintLength.h>
#include <SketchPlugin_ConstraintFillet.h>
#include <SketchPlugin_ConstraintMirror.h>
#include <SketchPlugin_ConstraintParallel.h>
#include <SketchPlugin_ConstraintPerpendicular.h>
#include <SketchPlugin_ConstraintRadius.h>
#include <SketchPlugin_ConstraintRigid.h>
#include <SketchPlugin_ConstraintTangent.h>
#include <SketchPlugin_ConstraintVertical.h>
#include <SketchPlugin_MultiRotation.h>
#include <SketchPlugin_MultiTranslation.h>

#include <math.h>
#include <assert.h>


/// \brief This class is used to give unique index to the groups
class GroupIndexer
{
public:
  /// \brief Return vacant index
  static GroupID NEW_GROUP() { return ++myGroupIndex; }
  /// \brief Removes the index
  static void REMOVE_GROUP(const GroupID& theIndex) {
    if (myGroupIndex == theIndex)
      myGroupIndex--;
  }

private:
  GroupIndexer() {};

  static GroupID myGroupIndex; ///< index of the group
};

GroupID GroupIndexer::myGroupIndex = GID_OUTOFGROUP;


static void sendMessage(const char* theMessageName)
{
  std::shared_ptr<Events_Message> aMessage = std::shared_ptr<Events_Message>(
      new Events_Message(Events_Loop::eventByName(theMessageName)));
  Events_Loop::loop()->send(aMessage);
}



// ========================================================
// =========  SketchSolver_Group  ===============
// ========================================================

SketchSolver_Group::SketchSolver_Group(
    std::shared_ptr<ModelAPI_CompositeFeature> theWorkplane)
    : myID(GroupIndexer::NEW_GROUP()),
      myPrevSolved(true)
{
  // Initialize workplane
  myWorkplaneID = EID_UNKNOWN;
  addWorkplane(theWorkplane);
}

SketchSolver_Group::~SketchSolver_Group()
{
  myConstraints.clear();
  GroupIndexer::REMOVE_GROUP(myID);
}

// ============================================================================
//  Function: isBaseWorkplane
//  Class:    SketchSolver_Group
//  Purpose:  verify the group is based on the given workplane
// ============================================================================
bool SketchSolver_Group::isBaseWorkplane(CompositeFeaturePtr theWorkplane) const
{
  return theWorkplane == mySketch;
}

// ============================================================================
//  Function: isInteract
//  Class:    SketchSolver_Group
//  Purpose:  verify are there any entities in the group used by given constraint
// ============================================================================
bool SketchSolver_Group::isInteract(FeaturePtr theFeature) const
{
  // Empty group interacts with everything
  if (isEmpty())
    return true;
  // Check interaction with the storage
  return myStorage->isInteract(theFeature);
}

// ============================================================================
//  Function: changeConstraint
//  Class:    SketchSolver_Group
//  Purpose:  create/update the constraint in the group
// ============================================================================
bool SketchSolver_Group::changeConstraint(
    std::shared_ptr<SketchPlugin_Constraint> theConstraint)
{
  // There is no workplane yet, something wrong
  if (myWorkplaneID == EID_UNKNOWN)
    return false;

  if (!theConstraint || !theConstraint->data())
    return false;

  if (!checkFeatureValidity(theConstraint))
    return false;

  BuilderPtr aBuilder = SketchSolver_Manager::instance()->builder();
  myStorage->blockEvents(true);

  bool isNewConstraint = myConstraints.find(theConstraint) == myConstraints.end();
  if (isNewConstraint) {
    // Add constraint to the current group
    SolverConstraintPtr aConstraint = aBuilder->createConstraint(theConstraint);
    if (!aConstraint)
      return false;
    aConstraint->process(myStorage, getId(), getWorkplaneId());
    if (!aConstraint->error().empty()) {
      if (aConstraint->error() == SketchSolver_Error::NOT_INITIALIZED())
        return false; // some attribute are not initialized yet, don't show message
      Events_Error::send(aConstraint->error(), this);
    }
    myConstraints[theConstraint] = aConstraint;
  }
  else
    myConstraints[theConstraint]->update();

  // Fix mirror line
  if (theConstraint->getKind() == SketchPlugin_ConstraintMirror::ID()) {
    AttributeRefAttrPtr aRefAttr = std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(
        theConstraint->attribute(SketchPlugin_ConstraintMirror::ENTITY_A()));
    if (aRefAttr && aRefAttr->isObject()) {
      std::shared_ptr<SketchPlugin_Feature> aFeature =
          std::dynamic_pointer_cast<SketchPlugin_Feature>(
          ModelAPI_Feature::feature(aRefAttr->object()));
      if (aFeature) {
        SolverConstraintPtr aConstraint = aBuilder->createFixedConstraint(aFeature);
        if (aConstraint) {
          aConstraint->process(myStorage, getId(), getWorkplaneId());
          setTemporary(aConstraint);
        }
      }
    }
  }
  return true;
}


void SketchSolver_Group::updateConstraints()
{
  std::set<SolverConstraintPtr> aPostponed; // postponed constraints Multi-Rotation and Multi-Translation

  ConstraintConstraintMap::iterator anIt = myConstraints.begin();
  for (; anIt != myConstraints.end(); ++anIt) {
    if (myChangedConstraints.find(anIt->first) == myChangedConstraints.end())
      continue;
    if (anIt->first->getKind() == SketchPlugin_MultiRotation::ID() ||
        anIt->first->getKind() == SketchPlugin_MultiTranslation::ID())
      aPostponed.insert(anIt->second);
    else
      anIt->second->update();
  }

  // Update postponed constraints
  std::set<SolverConstraintPtr>::iterator aSCIter = aPostponed.begin();
  for (; aSCIter != aPostponed.end(); ++aSCIter)
    (*aSCIter)->update();

  myChangedConstraints.clear();
}

bool SketchSolver_Group::updateFeature(FeaturePtr theFeature)
{
  if (!checkFeatureValidity(theFeature))
    return false;

  myStorage->blockEvents(true);
  myStorage->refresh(true);
  return myStorage->update(theFeature);
}

void SketchSolver_Group::moveFeature(FeaturePtr theFeature)
{
  BuilderPtr aBuilder = SketchSolver_Manager::instance()->builder();

  // Firstly, revert changes in the fixed entities
  myStorage->blockEvents(true);
  myStorage->refresh(true);

  // Secondly, search attributes of the feature in the list of the Multi constraints and update them
  ConstraintConstraintMap::iterator aCIt = myConstraints.begin();
  for (; aCIt != myConstraints.end(); ++aCIt) {
    if ((aCIt->second->getType() == CONSTRAINT_MULTI_ROTATION ||
         aCIt->second->getType() == CONSTRAINT_MULTI_TRANSLATION)
        && aCIt->second->isUsed(theFeature))
      std::dynamic_pointer_cast<SketchSolver_ConstraintMulti>(aCIt->second)->update(true);
  }

  // Then, create temporary rigid constraint
  SolverConstraintPtr aConstraint = aBuilder->createMovementConstraint(theFeature);
  if (!aConstraint)
    return;
  aConstraint->process(myStorage, getId(), getWorkplaneId());
  if (aConstraint->error().empty())
    setTemporary(aConstraint);
}

// ============================================================================
//  Function: addWorkplane
//  Class:    SketchSolver_Group
//  Purpose:  create workplane for the group
// ============================================================================
bool SketchSolver_Group::addWorkplane(CompositeFeaturePtr theSketch)
{
  if (myWorkplaneID != EID_UNKNOWN || theSketch->getKind() != SketchPlugin_Sketch::ID())
    return false;  // the workplane already exists or the function parameter is not Sketch

  mySketch = theSketch;
  if (!updateWorkplane()) {
    mySketch = CompositeFeaturePtr();
    return false;
  }
  return true;
}

// ============================================================================
//  Function: updateWorkplane
//  Class:    SketchSolver_Group
//  Purpose:  update parameters of workplane
// ============================================================================
bool SketchSolver_Group::updateWorkplane()
{
  BuilderPtr aBuilder = SketchSolver_Manager::instance()->builder();
  if (!myStorage) // Create storage if not exists
    myStorage = aBuilder->createStorage(getId());

  // sketch should be unchanged, set it out of current group
  bool isUpdated = myStorage->update(FeaturePtr(mySketch), GID_OUTOFGROUP);
  if (isUpdated) {
    EntityWrapperPtr anEntity = myStorage->entity(FeaturePtr(mySketch));
    myWorkplaneID = anEntity->id();
  }
  return isUpdated;
}

// ============================================================================
//  Function: resolveConstraints
//  Class:    SketchSolver_Group
//  Purpose:  solve the set of constraints for the current group
// ============================================================================
bool SketchSolver_Group::resolveConstraints()
{
  if (!myChangedConstraints.empty())
    updateConstraints();

  bool aResolved = false;
  bool isGroupEmpty = isEmpty();
  if (myStorage->isNeedToResolve() && !isGroupEmpty) {
    if (!mySketchSolver)
      mySketchSolver = SketchSolver_Manager::instance()->builder()->createSolver();

    mySketchSolver->setGroup(myID);
    mySketchSolver->calculateFailedConstraints(false);
    myStorage->initializeSolver(mySketchSolver);

    SketchSolver_SolveStatus aResult = STATUS_OK;
    try {
      if (myStorage->hasDuplicatedConstraint())
        aResult = STATUS_INCONSISTENT;
      else {
        // To avoid overconstraint situation, we will remove temporary constraints one-by-one
        // and try to find the case without overconstraint
        bool isLastChance = false;
        while (true) {
          aResult = mySketchSolver->solve();
          if (aResult == STATUS_OK || aResult == STATUS_EMPTYSET || isLastChance)
            break;
          // try to update parameters and resolve once again
          ConstraintConstraintMap::iterator aConstrIt = myConstraints.begin();
          for (; aConstrIt != myConstraints.end(); ++aConstrIt)
            aConstrIt->second->update();
          isLastChance = true;

          removeTemporaryConstraints();
          mySketchSolver->calculateFailedConstraints(true); // something failed => need to find it
          myStorage->initializeSolver(mySketchSolver);
        }
      }
    } catch (...) {
//      Events_Error::send(SketchSolver_Error::SOLVESPACE_CRASH(), this);
      getWorkplane()->string(SketchPlugin_Sketch::SOLVER_ERROR())->setValue(SketchSolver_Error::SOLVESPACE_CRASH());
      if (myPrevSolved) {
        // the error message should be changed before sending the message
        sendMessage(EVENT_SOLVER_FAILED);
        myPrevSolved = false;
      }
      return false;
    }
    if (aResult == STATUS_OK || aResult == STATUS_EMPTYSET) {  // solution succeeded, store results into correspondent attributes
      myStorage->refresh();
      if (!myPrevSolved) {
        getWorkplane()->string(SketchPlugin_Sketch::SOLVER_ERROR())->setValue("");
        // the error message should be changed before sending the message
        sendMessage(EVENT_SOLVER_REPAIRED);
        myPrevSolved = true;
      }
    } else if (!myConstraints.empty()) {
//      Events_Error::send(SketchSolver_Error::CONSTRAINTS(), this);
      getWorkplane()->string(SketchPlugin_Sketch::SOLVER_ERROR())->setValue(SketchSolver_Error::CONSTRAINTS());
      if (myPrevSolved) {
        // the error message should be changed before sending the message
        sendMessage(EVENT_SOLVER_FAILED);
        myPrevSolved = false;
      }
    }

    aResolved = true;
  } else if (!isGroupEmpty) {
    // Check there are constraints Fixed. If they exist, update parameters by stored values
    ConstraintConstraintMap::iterator aCIt = myConstraints.begin();
    for (; aCIt != myConstraints.end(); ++aCIt)
      if (aCIt->first->getKind() == SketchPlugin_ConstraintRigid::ID()) {
        aResolved = true;
        break;
      }
    if (aCIt != myConstraints.end())
      myStorage->refresh();
  }
  removeTemporaryConstraints();
  myStorage->blockEvents(false);
  myStorage->setNeedToResolve(false);
  return aResolved;
}

// ============================================================================
//  Function: mergeGroups
//  Class:    SketchSolver_Group
//  Purpose:  append specified group to the current group
// ============================================================================
void SketchSolver_Group::mergeGroups(const SketchSolver_Group& theGroup)
{
  // If specified group is empty, no need to merge
  if (theGroup.isEmpty())
    return;

  std::set<ObjectPtr> aConstraints;
  ConstraintConstraintMap::const_iterator aConstrIter = theGroup.myConstraints.begin();
  for (; aConstrIter != theGroup.myConstraints.end(); aConstrIter++)
    aConstraints.insert(aConstrIter->first);

  std::list<FeaturePtr> aSortedConstraints = selectApplicableFeatures(aConstraints);
  std::list<FeaturePtr>::iterator aSCIter = aSortedConstraints.begin();
  for (; aSCIter != aSortedConstraints.end(); ++aSCIter) {
    ConstraintPtr aConstr = std::dynamic_pointer_cast<SketchPlugin_Constraint>(*aSCIter);
    if (!aConstr)
      continue;
    changeConstraint(aConstr);
  }
}

// ============================================================================
//  Function: splitGroup
//  Class:    SketchSolver_Group
//  Purpose:  divide the group into several subgroups
// ============================================================================
void SketchSolver_Group::splitGroup(std::list<SketchSolver_Group*>& theCuts)
{
  // New storage will be used in trimmed way to store the list of constraint interacted together.
  StoragePtr aNewStorage = SketchSolver_Manager::instance()->builder()->createStorage(getId());
  std::list<ConstraintWrapperPtr> aDummyVec; // empty vector to avoid creation of solver's constraints

  // Obtain constraints, which should be separated
  std::list<ConstraintPtr> anUnusedConstraints;
  ConstraintConstraintMap::iterator aCIter = myConstraints.begin();
  for ( ; aCIter != myConstraints.end(); aCIter++) {
    if (aNewStorage->isInteract(FeaturePtr(aCIter->first)))
      aNewStorage->addConstraint(aCIter->first, aDummyVec);
    else
      anUnusedConstraints.push_back(aCIter->first);
  }

  // Check the unused constraints once again, because they may become interacted with new storage since adding constraints
  std::list<ConstraintPtr>::iterator aUnuseIt = anUnusedConstraints.begin();
  while (aUnuseIt != anUnusedConstraints.end()) {
    if (aNewStorage->isInteract(FeaturePtr(*aUnuseIt))) {
      aNewStorage->addConstraint(*aUnuseIt, aDummyVec);
      anUnusedConstraints.erase(aUnuseIt);
      aUnuseIt = anUnusedConstraints.begin();
      continue;
    }
    aUnuseIt++;
  }

  std::list<SketchSolver_Group*>::iterator aCutsIter;
  aUnuseIt = anUnusedConstraints.begin();
  for ( ; aUnuseIt != anUnusedConstraints.end(); ++aUnuseIt) {
    // Remove unused constraints
    removeConstraint(*aUnuseIt);
    // Try to append constraint to already existent group
    for (aCutsIter = theCuts.begin(); aCutsIter != theCuts.end(); ++aCutsIter)
      if ((*aCutsIter)->isInteract(*aUnuseIt)) {
        (*aCutsIter)->changeConstraint(*aUnuseIt);
        break;
      }
    if (aCutsIter == theCuts.end()) {
      // Add new group
      SketchSolver_Group* aGroup = new SketchSolver_Group(mySketch);
      aGroup->changeConstraint(*aUnuseIt);
      theCuts.push_back(aGroup);
    } else {
      // Find other groups interacting with constraint
      std::list<SketchSolver_Group*>::iterator aBaseGroupIt = aCutsIter;
      for (++aCutsIter; aCutsIter != theCuts.end(); ++aCutsIter)
        if ((*aCutsIter)->isInteract(*aUnuseIt)) {
          (*aBaseGroupIt)->mergeGroups(**aCutsIter);
          std::list<SketchSolver_Group*>::iterator aRemoveIt = aCutsIter--;
          theCuts.erase(aRemoveIt);
        }
    }
  }
}

// ============================================================================
//  Function: isConsistent
//  Class:    SketchSolver_Group
//  Purpose:  search removed entities and constraints
// ============================================================================
bool SketchSolver_Group::isConsistent()
{
  if (isEmpty()) // no one constraint is initialized yet
    return true;

  // Check the features and constraint is the storage are valid
  bool aResult = myStorage->isConsistent();
  if (aResult) {
    // additional check of consistency of the Fixed constraint,
    // because they are not added to the storage
    ConstraintConstraintMap::iterator aCIter = myConstraints.begin();
    for (; aCIter != myConstraints.end(); ++aCIter)
      if (aCIter->first->getKind() == SketchPlugin_ConstraintRigid::ID() &&
         (!aCIter->first->data() || !aCIter->first->data()->isValid())) {
        aResult = false;
        break;
      }
  }
  if (!aResult) {
    // remove invalid constraints
    std::set<ConstraintPtr> anInvalidConstraints;
    ConstraintConstraintMap::iterator aCIter = myConstraints.begin();
    for (; aCIter != myConstraints.end(); ++aCIter) {
      if (!aCIter->first->data() || !aCIter->first->data()->isValid())
        anInvalidConstraints.insert(aCIter->first);
    }
    std::set<ConstraintPtr>::const_iterator aRemoveIt = anInvalidConstraints.begin();
    for (; aRemoveIt != anInvalidConstraints.end(); ++aRemoveIt)
      removeConstraint(*aRemoveIt);
    // remove invalid features
    myStorage->removeInvalidEntities();
  }
  return aResult;
}

// ============================================================================
//  Function: removeTemporaryConstraints
//  Class:    SketchSolver_Group
//  Purpose:  remove all transient SLVS_C_WHERE_DRAGGED constraints after
//            resolving the set of constraints
// ============================================================================
void SketchSolver_Group::removeTemporaryConstraints()
{
  std::set<SolverConstraintPtr>::iterator aTmpIt = myTempConstraints.begin();
  for (; aTmpIt != myTempConstraints.end(); ++aTmpIt)
    (*aTmpIt)->remove();

  if (!myTempConstraints.empty())
    myStorage->verifyFixed();
  myStorage->setNeedToResolve(false);
  myTempConstraints.clear();
}

// ============================================================================
//  Function: removeConstraint
//  Class:    SketchSolver_Group
//  Purpose:  remove constraint and all unused entities
// ============================================================================
void SketchSolver_Group::removeConstraint(ConstraintPtr theConstraint)
{
  bool isFullyRemoved = true;
  ConstraintConstraintMap::iterator aCIter = myConstraints.begin();
  for (; aCIter != myConstraints.end(); aCIter++)
    if (aCIter->first == theConstraint) {
      if (!aCIter->second->remove()) // the constraint is not fully removed
        isFullyRemoved = false;
      break;
    }
  if (aCIter == myConstraints.end())
    return;

  if (isFullyRemoved)
    myConstraints.erase(aCIter);
  else if (aCIter != myConstraints.end() &&
           aCIter->first->getKind() == SketchPlugin_ConstraintCoincidence::ID()) {
    // Update multicoincidence
    std::list<ConstraintPtr> aMultiCoinc;
    SolverConstraintPtr aCoincidence = aCIter->second;
    while (aCIter != myConstraints.end()) {
      if (aCIter->second != aCoincidence) {
        ++aCIter;
        continue;
      }
      if (aCIter->first != theConstraint)
        aMultiCoinc.push_back(aCIter->first);
      aCIter->second->remove();
      ConstraintConstraintMap::iterator aRemoveIt = aCIter++;
      myConstraints.erase(aRemoveIt);
    }

    std::list<ConstraintPtr>::iterator anIt = aMultiCoinc.begin();
    for (; anIt != aMultiCoinc.end(); ++anIt)
      changeConstraint(*anIt);
  }
}

// ============================================================================
//  Function: isComplexConstraint
//  Class:    SketchSolver_Group
//  Purpose:  verifies the constraint is complex, i.e. it needs another constraints to be created before
// ============================================================================
bool SketchSolver_Group::isComplexConstraint(FeaturePtr theConstraint)
{
  return theConstraint->getKind() == SketchPlugin_ConstraintFillet::ID() ||
         theConstraint->getKind() == SketchPlugin_ConstraintMirror::ID() ||
         theConstraint->getKind() == SketchPlugin_ConstraintTangent::ID();
}

// ============================================================================
//  Function: setTemporary
//  Class:    SketchSolver_Group
//  Purpose:  append given constraint to the group of temporary constraints
// ============================================================================
void SketchSolver_Group::setTemporary(SolverConstraintPtr theConstraint)
{
  myTempConstraints.insert(theConstraint);
}


// ============================================================================
//  Function: checkFeatureValidity
//  Class:    SketchSolver_Group
//  Purpose:  verifies is the feature valid
// ============================================================================
bool SketchSolver_Group::checkFeatureValidity(FeaturePtr theFeature)
{
  if (!theFeature || !theFeature->data()->isValid())
    return true;

  SessionPtr aMgr = ModelAPI_Session::get();
  ModelAPI_ValidatorsFactory* aFactory = aMgr->validators();
  return aFactory->validate(theFeature);
}




// ===========   Auxiliary functions   ========================================
static double featureToVal(FeaturePtr theFeature)
{
  if (theFeature->getKind() == SketchPlugin_Sketch::ID())
    return 0.0; // sketch
  ConstraintPtr aConstraint = std::dynamic_pointer_cast<SketchPlugin_Constraint>(theFeature);
  if (!aConstraint)
    return 1.0; // features (arc, circle, line, point)

  const std::string& anID = aConstraint->getKind();
  if (anID == SketchPlugin_ConstraintCoincidence::ID()) {
    AttributeRefAttrPtr anAttrA = std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(
        aConstraint->attribute(SketchPlugin_Constraint::ENTITY_A()));
    AttributeRefAttrPtr anAttrB = std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(
        aConstraint->attribute(SketchPlugin_Constraint::ENTITY_B()));
    if (anAttrA && anAttrB && (anAttrA->isObject() || anAttrB->isObject()))
      return 2.0; // point-on-line and point-on-circle should go before points coincidence constraint
    return 2.5;
  }
  if (anID == SketchPlugin_ConstraintDistance::ID() ||
      anID == SketchPlugin_ConstraintLength::ID() ||
      anID == SketchPlugin_ConstraintRadius::ID())
    return 3.0;
  if (anID == SketchPlugin_ConstraintAngle::ID())
    return 3.5;
  if (anID == SketchPlugin_ConstraintHorizontal::ID() ||
      anID == SketchPlugin_ConstraintVertical::ID() ||
      anID == SketchPlugin_ConstraintParallel::ID() ||
      anID == SketchPlugin_ConstraintPerpendicular::ID())
    return 4.0;
  if (anID == SketchPlugin_ConstraintEqual::ID())
    return 5.0;
  if (anID == SketchPlugin_ConstraintTangent::ID() ||
      anID == SketchPlugin_ConstraintMirror::ID())
    return 6.0;
  if (anID == SketchPlugin_ConstraintRigid::ID())
    return 7.0;
  if (anID == SketchPlugin_MultiRotation::ID() ||
      anID == SketchPlugin_MultiTranslation::ID())
    return 8.0;

  // all other constraints are placed between Equal and Tangent constraints
  return 5.5;
}

static bool isLess(FeaturePtr theFeature1, FeaturePtr theFeature2)
{
  return featureToVal(theFeature1) < featureToVal(theFeature2);
}

std::list<FeaturePtr> SketchSolver_Group::selectApplicableFeatures(const std::set<ObjectPtr>& theObjects)
{
  std::list<FeaturePtr> aResult;
  std::list<FeaturePtr>::iterator aResIt;

  std::set<ObjectPtr>::const_iterator anObjIter = theObjects.begin();
  for (; anObjIter != theObjects.end(); ++anObjIter) {
    // Operate sketch itself and SketchPlugin features only.
    // Also, the Fillet need to be skipped, because there are several separated constraints composing it.
    FeaturePtr aFeature = std::dynamic_pointer_cast<ModelAPI_Feature>(*anObjIter);
    if (!aFeature)
      continue;
    std::shared_ptr<SketchPlugin_Feature> aSketchFeature = 
        std::dynamic_pointer_cast<SketchPlugin_Feature>(aFeature);
    if ((aFeature->getKind() != SketchPlugin_Sketch::ID() && !aSketchFeature) ||
        aFeature->getKind() == SketchPlugin_ConstraintFillet::ID())
      continue;

    // Find the place where to insert a feature
    for (aResIt = aResult.begin(); aResIt != aResult.end(); ++aResIt)
      if (isLess(aFeature, *aResIt))
        break;
    aResult.insert(aResIt, aFeature);
  }

  return aResult;
}

