// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:    SketchSolver_Group.cpp
// Created: 27 May 2014
// Author:  Artem ZHIDKOV

#include "SketchSolver_Group.h"
#include <SketchSolver_Error.h>
#include <SketchSolver_Manager.h>

#include <PlaneGCSSolver_Solver.h>
#include <PlaneGCSSolver_Storage.h>
#include <PlaneGCSSolver_Tools.h>

#include <Events_InfoMessage.h>
#include <ModelAPI_AttributeString.h>
#include <ModelAPI_Events.h>
#include <SketchPlugin_ConstraintRigid.h>


static void sendMessage(const char* theMessageName)
{
  std::shared_ptr<Events_Message> aMessage = std::shared_ptr<Events_Message>(
      new Events_Message(Events_Loop::eventByName(theMessageName)));
  Events_Loop::loop()->send(aMessage);
}

static void sendMessage(const char* theMessageName, const std::set<ObjectPtr>& theConflicting)
{
  std::shared_ptr<ModelAPI_SolverFailedMessage> aMessage =
      std::shared_ptr<ModelAPI_SolverFailedMessage>(
      new ModelAPI_SolverFailedMessage(Events_Loop::eventByName(theMessageName)));
  aMessage->setObjects(theConflicting);
  Events_Loop::loop()->send(aMessage);
}

static void sendMessage(const char* theMessageName, const CompositeFeaturePtr& theSketch, const int theDOF)
{
  std::shared_ptr<ModelAPI_SolverFailedMessage> aMessage =
      std::shared_ptr<ModelAPI_SolverFailedMessage>(
      new ModelAPI_SolverFailedMessage(Events_Loop::eventByName(theMessageName)));

  std::set<ObjectPtr> anObjects;
  anObjects.insert(theSketch);
  aMessage->setObjects(anObjects);
  aMessage->dof(theDOF);

  Events_Loop::loop()->send(aMessage);
}



// ========================================================
// =========  SketchSolver_Group  ===============
// ========================================================

SketchSolver_Group::SketchSolver_Group(const CompositeFeaturePtr& theWorkplane)
  : mySketch(theWorkplane),
    myPrevResult(PlaneGCSSolver_Solver::STATUS_UNKNOWN),
    myDOF(0),
    myIsEventsBlocked(false)
{
  mySketchSolver = SolverPtr(new PlaneGCSSolver_Solver);
  myStorage = StoragePtr(new PlaneGCSSolver_Storage(mySketchSolver));
}

SketchSolver_Group::~SketchSolver_Group()
{
  myConstraints.clear();
  // send the message that there is no more conflicting constraints
  if (!myConflictingConstraints.empty()) {
    sendMessage(EVENT_SOLVER_REPAIRED, myConflictingConstraints);
    myConflictingConstraints.clear();
  }
}

// ============================================================================
//  Function: changeConstraint
//  Class:    SketchSolver_Group
//  Purpose:  create/update the constraint in the group
// ============================================================================
bool SketchSolver_Group::changeConstraint(
    std::shared_ptr<SketchPlugin_Constraint> theConstraint)
{
  bool isNewConstraint = myConstraints.find(theConstraint) == myConstraints.end();
  if (isNewConstraint) {
    // Add constraint to the current group
    SolverConstraintPtr aConstraint = PlaneGCSSolver_Tools::createConstraint(theConstraint);
    if (!aConstraint)
      return false;
    aConstraint->process(myStorage, myIsEventsBlocked);
    if (!aConstraint->error().empty()) {
      if (aConstraint->error() == SketchSolver_Error::NOT_INITIALIZED())
        return false; // some attribute are not initialized yet, don't show message
      Events_InfoMessage("SketchSolver_Group", aConstraint->error(), this).send();
    }
    myConstraints[theConstraint] = aConstraint;
  }
  else
    myConstraints[theConstraint]->update();
  return true;
}

bool SketchSolver_Group::updateFeature(FeaturePtr theFeature)
{
  return myStorage->update(theFeature);
}

bool SketchSolver_Group::moveFeature(FeaturePtr theFeature)
{
  if (myDOF == 0) {
    // avoid moving elements of fully constrained sketch
    myStorage->refresh();
    return true;
  }

  // Create temporary Fixed constraint
  SolverConstraintPtr aConstraint = PlaneGCSSolver_Tools::createMovementConstraint(theFeature);
  if (!aConstraint)
    return false;
  aConstraint->process(myStorage, myIsEventsBlocked);
  if (aConstraint->error().empty())
    setTemporary(aConstraint);
  else
    myStorage->notify(theFeature);

  return true;
}

// ============================================================================
//  Function: resolveConstraints
//  Class:    SketchSolver_Group
//  Purpose:  solve the set of constraints for the current group
// ============================================================================
bool SketchSolver_Group::resolveConstraints()
{
  bool aResolved = false;
  bool isGroupEmpty = isEmpty() && myStorage->isEmpty();
  if (myStorage->isNeedToResolve() &&
      (!isGroupEmpty || !myConflictingConstraints.empty() ||
        myPrevResult == PlaneGCSSolver_Solver::STATUS_FAILED)) {

    PlaneGCSSolver_Solver::SolveStatus aResult = PlaneGCSSolver_Solver::STATUS_OK;
    try {
      if (!isGroupEmpty)
        aResult = mySketchSolver->solve();
    } catch (...) {
      getWorkplane()->string(SketchPlugin_Sketch::SOLVER_ERROR())
        ->setValue(SketchSolver_Error::SOLVESPACE_CRASH());
      if (myPrevResult == PlaneGCSSolver_Solver::STATUS_OK ||
          myPrevResult == PlaneGCSSolver_Solver::STATUS_UNKNOWN) {
        // the error message should be changed before sending the message
        sendMessage(EVENT_SOLVER_FAILED);
        myPrevResult = PlaneGCSSolver_Solver::STATUS_FAILED;
      }
      mySketchSolver->undo();
      return false;
    }
    // solution succeeded, store results into correspondent attributes
    if (aResult == PlaneGCSSolver_Solver::STATUS_OK ||
        aResult == PlaneGCSSolver_Solver::STATUS_EMPTYSET) {
      myStorage->setNeedToResolve(false);
      myStorage->refresh();
////      updateMultiConstraints(myConstraints);
////      // multi-constraints updated some parameters, need to store them
////      if (myStorage->isNeedToResolve())
////        resolveConstraints();

      if (myPrevResult != PlaneGCSSolver_Solver::STATUS_OK ||
          myPrevResult == PlaneGCSSolver_Solver::STATUS_UNKNOWN) {
        getWorkplane()->string(SketchPlugin_Sketch::SOLVER_ERROR())->setValue("");
        std::set<ObjectPtr> aConflicting = myConflictingConstraints;
        myConflictingConstraints.clear();
        myPrevResult = PlaneGCSSolver_Solver::STATUS_OK;
        // the error message should be changed before sending the message
        sendMessage(EVENT_SOLVER_REPAIRED, aConflicting);
      }

      // show degrees of freedom
      computeDoF();
    } else {
      mySketchSolver->undo();
      if (!myConstraints.empty()) {
        // the error message should be changed before sending the message
        getWorkplane()->string(SketchPlugin_Sketch::SOLVER_ERROR())
          ->setValue(SketchSolver_Error::CONSTRAINTS());
        if (myPrevResult != aResult ||
            myPrevResult == PlaneGCSSolver_Solver::STATUS_UNKNOWN ||
            myPrevResult == PlaneGCSSolver_Solver::STATUS_FAILED) {
          // Obtain list of conflicting constraints
          std::set<ObjectPtr> aConflicting = myStorage->getConflictingConstraints(mySketchSolver);

          if (!myConflictingConstraints.empty()) {
            std::set<ObjectPtr>::iterator anIt = aConflicting.begin();
            for (; anIt != aConflicting.end(); ++anIt)
              myConflictingConstraints.erase(*anIt);
            if (!myConflictingConstraints.empty()) {
              // some constraints does not conflict, send corresponding message
              sendMessage(EVENT_SOLVER_REPAIRED, myConflictingConstraints);
            }
          }
          myConflictingConstraints = aConflicting;
          if (!myConflictingConstraints.empty())
            sendMessage(EVENT_SOLVER_FAILED, myConflictingConstraints);
          myPrevResult = aResult;
        }
      }
    }

    aResolved = true;
  } else if (!isGroupEmpty) {
    // Check if the group contains only constraints Fixed, update parameters by stored values
    aResolved = true;
    ConstraintConstraintMap::iterator aCIt = myConstraints.begin();
    for (; aCIt != myConstraints.end(); ++aCIt)
      if (aCIt->first->getKind() != SketchPlugin_ConstraintRigid::ID()) {
        aResolved = false;
        break;
      }
    if (aCIt == myConstraints.end())
      myStorage->refresh();
  } else if (isGroupEmpty && isWorkplaneValid())
    computeDoF();
  removeTemporaryConstraints();
  myStorage->setNeedToResolve(false);
  return aResolved;
}

// ============================================================================
//  Function: computeDoF
//  Class:    SketchSolver_Group
//  Purpose:  compute DoF of the sketch and set corresponding field
// ============================================================================
void SketchSolver_Group::computeDoF()
{
  std::ostringstream aDoFMsg;
  int aDoF = mySketchSolver->dof();
  if (aDoF == 0)
    aDoFMsg << "Sketch is fully fixed (DoF = 0)";
  else
    aDoFMsg << "DoF (degrees of freedom) = " << aDoF;
  mySketch->string(SketchPlugin_Sketch::SOLVER_DOF())->setValue(aDoFMsg.str());

  if (aDoF > 0 && myDOF == 0)
    sendMessage(EVENT_SKETCH_UNDER_CONSTRAINED, mySketch, aDoF);
  else if (aDoF == 0 && myDOF > 0)
    sendMessage(EVENT_SKETCH_FULLY_CONSTRAINED, mySketch, aDoF);
  else if (aDoF < 0)
    sendMessage(EVENT_SKETCH_OVER_CONSTRAINED, mySketch, aDoF);

  myDOF = aDoF;
}

// ============================================================================
//  Function: repairConsistency
//  Class:    SketchSolver_Group
//  Purpose:  search removed entities and constraints
// ============================================================================
void SketchSolver_Group::repairConsistency()
{
  if (!myStorage->isConsistent()) {
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

    // show DoF
    computeDoF();
  }
}

// ============================================================================
//  Function: removeTemporaryConstraints
//  Class:    SketchSolver_Group
//  Purpose:  remove all transient SLVS_C_WHERE_DRAGGED constraints after
//            resolving the set of constraints
// ============================================================================
void SketchSolver_Group::removeTemporaryConstraints()
{
  if (!myTempConstraints.empty()) {
    mySketchSolver->removeConstraint(CID_MOVEMENT);

    std::set<SolverConstraintPtr>::iterator aTmpIt = myTempConstraints.begin();
    for (; aTmpIt != myTempConstraints.end(); ++aTmpIt)
      (*aTmpIt)->remove();

    myTempConstraints.clear();
  }

  myStorage->setNeedToResolve(false);
}

// ============================================================================
//  Function: removeConstraint
//  Class:    SketchSolver_Group
//  Purpose:  remove constraint and all unused entities
// ============================================================================
void SketchSolver_Group::removeConstraint(ConstraintPtr theConstraint)
{
  ConstraintConstraintMap::iterator aCIter = myConstraints.begin();
  for (; aCIter != myConstraints.end(); aCIter++)
    if (aCIter->first == theConstraint) {
      aCIter->second->remove(); // the constraint is not fully removed
      break;
    }
  if (aCIter != myConstraints.end())
    myConstraints.erase(aCIter);
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
//  Function: blockEvents
//  Class:    SketchSolver_Group
//  Purpose:  block or unblock events from features in this group
// ============================================================================
void SketchSolver_Group::blockEvents(bool isBlocked)
{
  if (myIsEventsBlocked == isBlocked)
    return;

  // block/unblock events from the features in the storage
  myStorage->blockEvents(isBlocked);

  // block/unblock events from constraints
  ConstraintConstraintMap::iterator aCIt = myConstraints.begin();
  for (; aCIt != myConstraints.end(); ++aCIt)
    aCIt->second->blockEvents(isBlocked);

  myIsEventsBlocked = isBlocked;
}
