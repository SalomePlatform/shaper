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
  std::set<GCS::Constraint*>::const_iterator anIt = myConstraints.begin();
  for (; anIt != myConstraints.end(); ++anIt)
    myEquationSystem.removeConstraint(*anIt);
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

SketchSolver_SolveStatus PlaneGCSSolver_Solver::solve()
{
  if (myConstraints.empty())
    return STATUS_EMPTYSET;
  if (myParameters.empty())
    return STATUS_INCONSISTENT;

////  myEquationSystem.calculateFaileds = myFindFaileds ? 1 : 0;

  Events_LongOp::start(this);
  GCS::SolveStatus aResult = (GCS::SolveStatus)myEquationSystem.solve(myParameters);
  Events_LongOp::end(this);

  SketchSolver_SolveStatus aStatus;
  if (aResult == GCS::Success) {
    myEquationSystem.applySolution();
    aStatus = STATUS_OK;
  } else {
    myEquationSystem.undoSolution();
    aStatus = STATUS_FAILED;
  }
  return aStatus;
}
