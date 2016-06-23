// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:    PlaneGCSSolver_Solver.cpp
// Created: 14 Dec 2014
// Author:  Artem ZHIDKOV

#include "PlaneGCSSolver_Solver.h"
#include <Events_LongOp.h>


PlaneGCSSolver_Solver::~PlaneGCSSolver_Solver()
{
  clear();
}

void PlaneGCSSolver_Solver::clear()
{
  myEquationSystem.clear();
  myConstraints.clear();
  myParameters.clear();
}

void PlaneGCSSolver_Solver::addConstraint(GCSConstraintPtr theConstraint)
{
  GCS::Constraint* aConstraint = theConstraint.get();
  if (myConstraints.find(aConstraint) != myConstraints.end())
    return; // constraint already exists, no need to add it again

  myEquationSystem.addConstraint(aConstraint);
  myConstraints.insert(aConstraint);
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

  myEquationSystem.removeConstraint(theConstraint);
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
  std::set<GCS::Constraint*>::const_iterator aConstrIt = myConstraints.begin();
  for (; aConstrIt != myConstraints.end(); ++aConstrIt) {
    GCS::VEC_pD aParams = (*aConstrIt)->params();
    GCS::VEC_pD::const_iterator aPIt = aParams.begin();
    for (; aPIt != aParams.end(); ++aPIt)
      if (aParameters.find(*aPIt) != aParameters.end())
        break;
    if (aPIt == aParams.end() && (*aConstrIt)->getTag() > 0) {
      myConflictingIDs.push_back((*aConstrIt)->getTag());
      myConfCollected = true;
      aResult = GCS::Failed;
    }
  }
  // solve equations
  if (aResult == GCS::Success)
    aResult = (GCS::SolveStatus)myEquationSystem.solve(myParameters);
  if (aResult == GCS::Success) {
    // additionally check redundant constraints
    GCS::VEC_I aRedundantID;
    myEquationSystem.getRedundant(aRedundantID);
    // Workaround: remove all constraints "Equal"
    if (!aRedundantID.empty()) {
      std::set<GCS::Constraint*>::const_iterator aCIt = myConstraints.begin();
      for (; aCIt != myConstraints.end(); ++aCIt) {
        if ((*aCIt)->getTypeId() != GCS::Equal)
          continue;
        GCS::VEC_I::iterator aRIt = aRedundantID.begin();
        for (; aRIt != aRedundantID.end(); ++aRIt)
          if ((*aCIt)->getTag() == *aRIt) {
            aRedundantID.erase(aRIt);
            break;
          }
      }
    }
    // The system with tangent constraints may show redundant constraints if the entities are coupled smoothly.
    // Sometimes tangent constraints are fall to both conflicting and redundant constraints.
    // Need to check if there are redundant constraints without these tangencies.
    if (!aRedundantID.empty())
      aResult = myTangent.empty() ? GCS::Failed : (GCS::SolveStatus)solveWithoutTangent();
  }
  Events_LongOp::end(this);

  SketchSolver_SolveStatus aStatus;
  if (aResult == GCS::Success) {
    myEquationSystem.applySolution();
    aStatus = STATUS_OK;
  } else
    aStatus = STATUS_FAILED;

  return aStatus;
}

SketchSolver_SolveStatus PlaneGCSSolver_Solver::solveWithoutTangent()
{
  // Remove tangency which leads to redundant or conflicting constraints
  GCS::VEC_I aConflicting, aRedundant;
  myEquationSystem.getRedundant(aRedundant);
  size_t aNbRemove = myTangent.size(); // number of tangent constraints which can be removed
  myEquationSystem.getConflicting(aConflicting);
  aRedundant.insert(aRedundant.end(), aConflicting.begin(), aConflicting.end());

  GCS::SET_I aTangentToRemove;
  GCS::VEC_I::iterator aCIt = aRedundant.begin();
  for (; aCIt != aRedundant.end() && aNbRemove > 0; ++aCIt)
    if (myTangent.find(*aCIt) != myTangent.end()) {
      aTangentToRemove.insert(*aCIt);
      --aNbRemove;
    }

  std::set<GCS::Constraint*>::const_iterator aConstrIt = myConstraints.begin();
  while (aConstrIt != myConstraints.end()) {
    GCS::Constraint* aConstraint = *aConstrIt;
    int anID = aConstraint->getTag();
    ++aConstrIt;
    if (aTangentToRemove.find(anID) != aTangentToRemove.end())
      removeConstraint(aConstraint);
  }

  myTangent.clear();
  return solve();
}

void PlaneGCSSolver_Solver::undo()
{
  myEquationSystem.undoSolution();
}

bool PlaneGCSSolver_Solver::isConflicting(const ConstraintID& theConstraint) const
{
  if (!myConfCollected)
    const_cast<PlaneGCSSolver_Solver*>(this)->collectConflicting();

  GCS::VEC_I::const_iterator anIt = myConflictingIDs.begin();
  for (; anIt != myConflictingIDs.end(); ++anIt)
    if (*anIt == (int)theConstraint)
      return true;
  return false;
}

void PlaneGCSSolver_Solver::collectConflicting()
{
  GCS::VEC_I aConflict;
  myEquationSystem.getConflicting(myConflictingIDs);
  myConflictingIDs.insert(myConflictingIDs.end(), aConflict.begin(), aConflict.end());

  myEquationSystem.getRedundant(aConflict);
  myConflictingIDs.insert(myConflictingIDs.end(), aConflict.begin(), aConflict.end());

  myConfCollected = true;
}

int PlaneGCSSolver_Solver::dof() const
{
  return const_cast<PlaneGCSSolver_Solver*>(this)->myEquationSystem.dofsNumber();
}
