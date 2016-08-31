// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:    PlaneGCSSolver_Solver.cpp
// Created: 14 Dec 2014
// Author:  Artem ZHIDKOV

#include "PlaneGCSSolver_Solver.h"
#include <Events_LongOp.h>

#include <cmath>


PlaneGCSSolver_Solver::PlaneGCSSolver_Solver()
  : myEquationSystem(new GCS::System),
  myConfCollected(false)
{
}

PlaneGCSSolver_Solver::~PlaneGCSSolver_Solver()
{
  clear();
}

void PlaneGCSSolver_Solver::clear()
{
  myEquationSystem->clear();
  myConstraints.clear();
  myParameters.clear();
}

void PlaneGCSSolver_Solver::addConstraint(GCSConstraintPtr theConstraint,
    const SketchSolver_ConstraintType theType)
{
  GCS::Constraint* aConstraint = theConstraint.get();
  if (myConstraints.find(aConstraint) != myConstraints.end())
    return; // constraint already exists, no need to add it again

  myEquationSystem->addConstraint(aConstraint);
  myConstraints[aConstraint] = theType;
}

void PlaneGCSSolver_Solver::removeConstraint(GCSConstraintPtr theConstraint)
{
  GCS::Constraint* aConstraint = theConstraint.get();
  removeConstraint(aConstraint);
}

void PlaneGCSSolver_Solver::removeConstraint(GCS::Constraint* theConstraint)
{
  if (myConstraints.find(theConstraint) == myConstraints.end())
    return; // no constraint, no need to remove it

  myEquationSystem->removeConstraint(theConstraint);
  myConstraints.erase(theConstraint);
}

SketchSolver_SolveStatus PlaneGCSSolver_Solver::solve()
{
  // clear list of conflicting constraints
  if (myConfCollected) {
    myConflictingIDs.clear();
    myConfCollected = false;
  }

  if (myConstraints.empty())
    return STATUS_EMPTYSET;
  if (myParameters.empty())
    return STATUS_INCONSISTENT;

  Events_LongOp::start(this);
  GCS::SolveStatus aResult = GCS::Success;
  // if there is a constraint with all attributes constant, set fail status
  GCS::SET_pD aParameters;
  aParameters.insert(myParameters.begin(), myParameters.end());
  ConstraintMap::const_iterator aConstrIt = myConstraints.begin();
  for (; aConstrIt != myConstraints.end(); ++aConstrIt) {
    GCS::VEC_pD aParams = aConstrIt->first->params();
    GCS::VEC_pD::const_iterator aPIt = aParams.begin();
    for (; aPIt != aParams.end(); ++aPIt)
      if (aParameters.find(*aPIt) != aParameters.end())
        break;
    if (aPIt == aParams.end() && aConstrIt->first->getTag() > 0) {
      myConflictingIDs.insert(aConstrIt->first->getTag());
      myConfCollected = true;
      aResult = GCS::Failed;
    }
  }
  // solve equations
  if (aResult == GCS::Success)
    aResult = (GCS::SolveStatus)myEquationSystem->solve(myParameters);

  GCS::VEC_I aRedundantID;

  // Workaround: the system with tangent constraint may fail if the tangent entities are connected smoothly.
  // Investigate this situation and move constraints to redundant list
  if (aResult == GCS::Failed && !myTangent.empty()) {
    GCS::VEC_I aConflictingID;
    myEquationSystem->getConflicting(aConflictingID);
    GCS::VEC_I::iterator aCIt = aConflictingID.begin();
    for (; aCIt != aConflictingID.end(); ++ aCIt) {
      if (myTangent.find(*aCIt) == myTangent.end())
        continue;
      if (isTangentTruth(*aCIt))
        aRedundantID.push_back(*aCIt);
    }

    if (!aRedundantID.empty())
      aResult = GCS::Success; // check redundant constraints
  }

  // Additionally check redundant constraints
  if (aResult == GCS::Success || aResult == GCS::Converged) {
    GCS::VEC_I aRedundantLocal;
    myEquationSystem->getRedundant(aRedundantLocal);
    aRedundantID.insert(aRedundantID.end(), aRedundantLocal.begin(), aRedundantLocal.end());
    // Workaround: remove all point-point coincidences from list of redundant
    if (!aRedundantID.empty()) {
      ConstraintMap::const_iterator aCIt = myConstraints.begin();
      for (; aCIt != myConstraints.end(); ++aCIt) {
        if (aCIt->second != CONSTRAINT_PT_PT_COINCIDENT)
          continue;
        GCS::VEC_I::iterator aRIt = aRedundantID.begin();
        for (; aRIt != aRedundantID.end(); ++aRIt)
          if (aCIt->first->getTag() == *aRIt) {
            aRedundantID.erase(aRIt);
            break;
          }
      }
    }
    // The system with tangent constraints may show redundant constraints if the entities are coupled smoothly.
    // Sometimes tangent constraints are fall to both conflicting and redundant constraints.
    // Need to check if there are redundant constraints without these tangencies.
    if (!aRedundantID.empty())
      aResult = myTangent.empty() ? GCS::Failed : solveWithoutTangent();
    else
      aResult = GCS::Success;
  }
  Events_LongOp::end(this);

  SketchSolver_SolveStatus aStatus;
  if (aResult == GCS::Success) {
    myEquationSystem->applySolution();
    aStatus = STATUS_OK;
  } else
    aStatus = STATUS_FAILED;

  return aStatus;
}

GCS::SolveStatus PlaneGCSSolver_Solver::solveWithoutTangent()
{
  std::shared_ptr<GCS::System> aSystemWithoutTangent(new GCS::System);

  // Remove tangency which leads to redundant or conflicting constraints
  GCS::VEC_I aConflicting, aRedundant;
  myEquationSystem->getRedundant(aRedundant);
  size_t aNbRemove = myTangent.size(); // number of tangent constraints which can be removed
  myEquationSystem->getConflicting(aConflicting);
  aRedundant.insert(aRedundant.end(), aConflicting.begin(), aConflicting.end());

  GCS::SET_I aTangentToRemove;
  GCS::VEC_I::iterator aCIt = aRedundant.begin();
  for (; aCIt != aRedundant.end() && aNbRemove > 0; ++aCIt)
    if (myTangent.find(*aCIt) != myTangent.end()) {
      aTangentToRemove.insert(*aCIt);
      --aNbRemove;
    }

  std::set<GCS::Constraint*> aRemovedTangent;
  ConstraintMap::const_iterator aConstrIt = myConstraints.begin();
  while (aConstrIt != myConstraints.end()) {
    GCS::Constraint* aConstraint = aConstrIt->first;
    int anID = aConstraint->getTag();
    ++aConstrIt;
    if (aTangentToRemove.find(anID) == aTangentToRemove.end())
      aSystemWithoutTangent->addConstraint(aConstraint);
    else
      aRemovedTangent.insert(aConstraint);
  }

  myTangent.clear();
  GCS::SolveStatus aResult = (GCS::SolveStatus)aSystemWithoutTangent->solve(myParameters);
  if (aResult == GCS::Success) {
    GCS::VEC_I aRedundant;
    aSystemWithoutTangent->getRedundant(aRedundant);
    if (!aRedundant.empty())
      aResult = GCS::Failed;
  }

  // additional check that removed constraints are still correct
  if (aResult == GCS::Success) {
    aSystemWithoutTangent->applySolution();
    std::set<GCS::Constraint*>::const_iterator aRemIt = aRemovedTangent.begin();
    for (; aRemIt != aRemovedTangent.end(); ++aRemIt)
      if (!isTangentTruth(*aRemIt))
        break;
    if (aRemIt != aRemovedTangent.end())
      aResult = GCS::Failed;
  }

  if (aResult == GCS::Success)
      myEquationSystem = aSystemWithoutTangent;
  else {
    // Add IDs of removed tangent to the list of conflicting constraints
    std::set<GCS::Constraint*>::const_iterator aRemIt = aRemovedTangent.begin();
    for (; aRemIt != aRemovedTangent.end(); ++aRemIt)
      myConflictingIDs.insert((*aRemIt)->getTag());
  }

  return aResult;
}

bool PlaneGCSSolver_Solver::isTangentTruth(GCS::Constraint* theTangent) const
{
  static const double aTol = 1e-5;
  double aTol2 = aTol *aTol;

  if (theTangent->getTypeId() == GCS::TangentCircumf) {
    GCS::VEC_pD aParams = theTangent->params();
    double dx = *(aParams[2]) - *(aParams[0]);
    double dy = *(aParams[3]) - *(aParams[1]);
    double aDist2 = dx * dx + dy * dy;
    double aRadSum  = *(aParams[4]) + *(aParams[5]);
    double aRadDiff = *(aParams[4]) - *(aParams[5]);
    aTol2 *= aDist2 > 1.0 ? aDist2 : 1.0;
    return fabs(aDist2 - aRadSum * aRadSum) <= aTol2 ||
           fabs(aDist2 - aRadDiff * aRadDiff) <= aTol2;
  }
  if (theTangent->getTypeId() == GCS::P2LDistance) {
    GCS::VEC_pD aParams = theTangent->params();
    double aDist2 = *(aParams[6]) * *(aParams[6]);
    // orthogonal line direction
    double aDirX = *(aParams[5]) - *(aParams[3]);
    double aDirY = *(aParams[2]) - *(aParams[4]);
    double aLen2 = aDirX * aDirX + aDirY * aDirY;
    // vector from line's start to point
    double aVecX = *(aParams[0]) - *(aParams[2]);
    double aVecY = *(aParams[1]) - *(aParams[3]);

    double aDot = aVecX * aDirX + aVecY * aDirY;
    return fabs(aDot * aDot - aDist2 * aLen2) <= aTol2 * aLen2;
  }
  return false;
}

bool PlaneGCSSolver_Solver::isTangentTruth(int theTagID) const
{
  ConstraintMap::const_iterator anIt = myConstraints.begin();
  for (; anIt != myConstraints.end(); ++anIt)
    if (anIt->first->getTag() == theTagID)
      return isTangentTruth(anIt->first);
  return false;
}

void PlaneGCSSolver_Solver::undo()
{
  myEquationSystem->undoSolution();
}

bool PlaneGCSSolver_Solver::isConflicting(const ConstraintID& theConstraint) const
{
  if (!myConfCollected)
    const_cast<PlaneGCSSolver_Solver*>(this)->collectConflicting();
  return myConflictingIDs.find((int)theConstraint) != myConflictingIDs.end();
}

void PlaneGCSSolver_Solver::collectConflicting()
{
  GCS::VEC_I aConflict;
  myEquationSystem->getConflicting(aConflict);
  myConflictingIDs.insert(aConflict.begin(), aConflict.end());

  myEquationSystem->getRedundant(aConflict);
  myConflictingIDs.insert(aConflict.begin(), aConflict.end());

  myConfCollected = true;
}

int PlaneGCSSolver_Solver::dof() const
{
  return const_cast<PlaneGCSSolver_Solver*>(this)->myEquationSystem->dofsNumber();
}
