// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:    PlaneGCSSolver_Solver.cpp
// Created: 14 Dec 2014
// Author:  Artem ZHIDKOV

#include "PlaneGCSSolver_Solver.h"
#include <Events_LongOp.h>


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
  myParameters.clear();
  myEqualConstraints.clear();
  myEqualParameters.clear();
}

void PlaneGCSSolver_Solver::addConstraint(GCSConstraintPtr theConstraint)
{
  myEquationSystem->addConstraint(theConstraint.get());
}

void PlaneGCSSolver_Solver::removeConstraint(ConstraintID theID)
{
  myEquationSystem->clearByTag(theID);
}

SketchSolver_SolveStatus PlaneGCSSolver_Solver::solve()
{
  // clear list of conflicting constraints
  if (myConfCollected) {
    myConflictingIDs.clear();
    myConfCollected = false;
  }

  if (myParameters.empty())
    return STATUS_INCONSISTENT;

  Events_LongOp::start(this);
  // solve equations
  GCS::SolveStatus aResult = (GCS::SolveStatus)myEquationSystem->solve(myParameters);
  Events_LongOp::end(this);

  SketchSolver_SolveStatus aStatus;
  if (aResult == GCS::Success) {
    myEquationSystem->applySolution();
    aStatus = STATUS_OK;
  } else
    aStatus = STATUS_FAILED;

  return aStatus;
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
