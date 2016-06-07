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
  if (myConstraints.find(aConstraint) == myConstraints.end())
    return; // no constraint, no need to remove it

  myEquationSystem.removeConstraint(aConstraint);
  myConstraints.erase(aConstraint);
}

static void removeTangent(GCS::VEC_I& theRedundant, const GCS::SET_I& theTangent)
{
  int i = 0;
  while (i < theRedundant.size()) {
    if (theTangent.find(theRedundant[i]) == theTangent.end())
      ++i;
    else
      theRedundant.erase(theRedundant.begin() + i);
  }
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
    if (aPIt == aParams.end()) {
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
    // remove redundant constraints relative to tangency
    removeTangent(aRedundantID, myTangent);
    if (!aRedundantID.empty())
      aResult = GCS::Failed;
  }
  Events_LongOp::end(this);

  SketchSolver_SolveStatus aStatus;
  if (aResult == GCS::Success) {
    myEquationSystem.applySolution();
    aStatus = STATUS_OK;
  } else {
    undo();
    aStatus = STATUS_FAILED;
  }

  return aStatus;
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
  myEquationSystem.getConflicting(myConflictingIDs);

  GCS::VEC_I aRedundantID;
  myEquationSystem.getRedundant(aRedundantID);
  myConflictingIDs.insert(myConflictingIDs.end(), aRedundantID.begin(), aRedundantID.end());

  myConfCollected = true;
}

int PlaneGCSSolver_Solver::dof() const
{
  return const_cast<PlaneGCSSolver_Solver*>(this)->myEquationSystem.dofsNumber();
}
