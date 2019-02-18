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

#include "SketchSolver_Group.h"
#include <SketchSolver_Error.h>
#include <SketchSolver_Manager.h>

#include <PlaneGCSSolver_Solver.h>
#include <PlaneGCSSolver_Storage.h>
#include <PlaneGCSSolver_Tools.h>

#include <Events_InfoMessage.h>
#include <ModelAPI_AttributeString.h>
#include <ModelAPI_Events.h>
#include <SketchPlugin_ConstraintMirror.h>
#include <SketchPlugin_ConstraintRigid.h>
#include <SketchPlugin_MultiRotation.h>
#include <SketchPlugin_MultiTranslation.h>


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

static void sendMessage(const char* theMessageName,
                        const CompositeFeaturePtr& theSketch,
                        const int theDOF)
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
// =========       SketchSolver_Group       ===============
// ========================================================

SketchSolver_Group::SketchSolver_Group(const CompositeFeaturePtr& theWorkplane)
  : mySketch(theWorkplane),
    myPrevResult(PlaneGCSSolver_Solver::STATUS_UNKNOWN),
    myDOF(-1),
    myIsEventsBlocked(false),
    myMultiConstraintUpdateStack(0)
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

  // constraint is created/updated => reset stack of "multi" constraints updates
  myMultiConstraintUpdateStack = 0;
  return true;
}

bool SketchSolver_Group::updateFeature(FeaturePtr theFeature)
{
  return myStorage->update(theFeature);
}

template <class Type>
static SolverConstraintPtr move(StoragePtr theStorage,
                                SolverPtr theSketchSolver,
                                int theSketchDOF,
                                bool theEventsBlocked,
                                Type theFeatureOrPoint,
                                const std::shared_ptr<GeomAPI_Pnt2d>& theFrom,
                                const std::shared_ptr<GeomAPI_Pnt2d>& theTo)
{
  bool isEntityExists = (theStorage->entity(theFeatureOrPoint).get() != 0);
  if (theSketchDOF == 0 && isEntityExists) {
    // avoid moving elements of fully constrained sketch
    theStorage->refresh();
    return SolverConstraintPtr();
  }

  // Create temporary Fixed constraint
  std::shared_ptr<SketchSolver_ConstraintMovement> aConstraint =
      PlaneGCSSolver_Tools::createMovementConstraint(theFeatureOrPoint);
  if (aConstraint) {
    SolverConstraintPtr(aConstraint)->process(theStorage, theEventsBlocked);
    if (aConstraint->error().empty()) {
      aConstraint->startPoint(theFrom);
      theSketchSolver->initialize();
      aConstraint->moveTo(theTo);
      theStorage->setNeedToResolve(true);
    } else
      theStorage->notify(aConstraint->movedFeature());
  }

  return aConstraint;
}

bool SketchSolver_Group::moveFeature(FeaturePtr theFeature,
                                     const std::shared_ptr<GeomAPI_Pnt2d>& theFrom,
                                     const std::shared_ptr<GeomAPI_Pnt2d>& theTo)
{
  SolverConstraintPtr aConstraint =
      move(myStorage, mySketchSolver, myDOF, myIsEventsBlocked, theFeature, theFrom, theTo);
  setTemporary(aConstraint);
  return true;
}

bool SketchSolver_Group::movePoint(AttributePtr theAttribute,
                                   const std::shared_ptr<GeomAPI_Pnt2d>& theFrom,
                                   const std::shared_ptr<GeomAPI_Pnt2d>& theTo)
{
  SolverConstraintPtr aConstraint =
      move(myStorage, mySketchSolver, myDOF, myIsEventsBlocked, theAttribute, theFrom, theTo);
  setTemporary(aConstraint);
  return true;
}

// ============================================================================
//  Function: resolveConstraints
//  Class:    SketchSolver_Group
//  Purpose:  solve the set of constraints for the current group
// ============================================================================
bool SketchSolver_Group::resolveConstraints()
{
  static const int MAX_STACK_SIZE = 5;
  // check the "Multi" constraints do not drop sketch into infinite loop
  if (myMultiConstraintUpdateStack > MAX_STACK_SIZE) {
    myMultiConstraintUpdateStack = 0;
    myPrevResult = PlaneGCSSolver_Solver::STATUS_FAILED;
    // generate error message due to loop update of the sketch
    getWorkplane()->string(SketchPlugin_Sketch::SOLVER_ERROR())
      ->setValue(SketchSolver_Error::INFINITE_LOOP());
    sendMessage(EVENT_SOLVER_FAILED, myConflictingConstraints);
    return false;
  }

  bool aResolved = false;
  bool isGroupEmpty = isEmpty() && myStorage->isEmpty();
  if (myStorage->isNeedToResolve() &&
      (!isGroupEmpty || !myConflictingConstraints.empty() ||
        myPrevResult == PlaneGCSSolver_Solver::STATUS_FAILED)) {

    PlaneGCSSolver_Solver::SolveStatus aResult = PlaneGCSSolver_Solver::STATUS_OK;
    try {
      if (!isGroupEmpty)
        aResult = mySketchSolver->solve();
      if (aResult == PlaneGCSSolver_Solver::STATUS_FAILED &&
          !myTempConstraints.empty()) {
        mySketchSolver->undo();
        removeTemporaryConstraints();
        aResult = mySketchSolver->solve();
      }
      // check degenerated geometry after constraints resolving
      if (aResult == PlaneGCSSolver_Solver::STATUS_OK)
        aResult = myStorage->checkDegeneratedGeometry();
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

      // additional check that copied entities used in Mirror and other "Multi" constraints
      // is not connected with their originals by constraints.
      myMultiConstraintUpdateStack += 1;
      aResolved = true;
      if (myStorage->isNeedToResolve())
        aResolved = resolveConstraints();

      if (aResolved) {
        myMultiConstraintUpdateStack -= 1;

        if (myPrevResult != PlaneGCSSolver_Solver::STATUS_OK ||
            myPrevResult == PlaneGCSSolver_Solver::STATUS_UNKNOWN) {
          getWorkplane()->string(SketchPlugin_Sketch::SOLVER_ERROR())->setValue("");
          std::set<ObjectPtr> aConflicting = myConflictingConstraints;
          myConflictingConstraints.clear();
          myPrevResult = PlaneGCSSolver_Solver::STATUS_OK;
          // the error message should be changed before sending the message
          sendMessage(EVENT_SOLVER_REPAIRED, aConflicting);
        }
      }

      // show degrees of freedom
      computeDoF();
    } else {
      mySketchSolver->undo();
      if (!myConstraints.empty()) {
        // the error message should be changed before sending the message
        const std::string& aErrorMsg = aResult == PlaneGCSSolver_Solver::STATUS_DEGENERATED ?
                                       SketchSolver_Error::DEGENERATED_GEOMETRY() :
                                       SketchSolver_Error::CONSTRAINTS();
        getWorkplane()->string(SketchPlugin_Sketch::SOLVER_ERROR())->setValue(aErrorMsg);
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

      // show degrees of freedom only if the degenerated geometry appears
      if (aResult == PlaneGCSSolver_Solver::STATUS_DEGENERATED)
        computeDoF();
    }

  }
  else if (isGroupEmpty && isWorkplaneValid()) {
    // clear error related to previously degenerated entities
    if (myPrevResult == PlaneGCSSolver_Solver::STATUS_DEGENERATED) {
      getWorkplane()->string(SketchPlugin_Sketch::SOLVER_ERROR())->setValue("");
      myPrevResult = PlaneGCSSolver_Solver::STATUS_OK;
      // the error message should be changed before sending the message
      myConflictingConstraints.clear();
      sendMessage(EVENT_SOLVER_REPAIRED, myConflictingConstraints);
    }

    computeDoF();
  }
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
  /// "DoF = 0" content of string value is used in PartSet by Sketch edit
  /// If it is changed, it should be corrected also there
  if (aDoF == 0)
    aDoFMsg << "Sketch is fully fixed (DoF = 0)";
  else
    aDoFMsg << "DoF (degrees of freedom) = " << aDoF;
  mySketch->string(SketchPlugin_Sketch::SOLVER_DOF())->setValue(aDoFMsg.str());

  if (aDoF > 0 && myDOF <= 0)
    sendMessage(EVENT_SKETCH_UNDER_CONSTRAINED, mySketch, aDoF);
  else if (aDoF == 0 && myDOF != 0)
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
  if (!areConstraintsValid() || !myStorage->areFeaturesValid()) {
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

      // constraint is removed => reset stack of "multi" constraints updates
      myMultiConstraintUpdateStack = 0;
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
  if (theConstraint)
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

bool SketchSolver_Group::areConstraintsValid() const
{
  // Check the constraints are valid
  ConstraintConstraintMap::const_iterator aCIter = myConstraints.begin();
  for (; aCIter != myConstraints.end(); ++aCIter)
    if (!aCIter->first->data() || !aCIter->first->data()->isValid())
      return false;
  return true;
}
