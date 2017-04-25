// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:    PlaneGCSSolver_Solver.cpp
// Created: 14 Dec 2014
// Author:  Artem ZHIDKOV

#include <PlaneGCSSolver_Solver.h>
#include <Events_LongOp.h>


PlaneGCSSolver_Solver::PlaneGCSSolver_Solver()
  : myEquationSystem(new GCS::System),
    myDiagnoseBeforeSolve(false),
    myInitilized(false),
    myConfCollected(false),
    myDOF(0)
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
  myConstraints.clear();
  myConflictingIDs.clear();
  myDOF = 0;
}

void PlaneGCSSolver_Solver::addConstraint(GCSConstraintPtr theConstraint)
{
  myEquationSystem->addConstraint(theConstraint.get());
  myConstraints[theConstraint->getTag()].insert(theConstraint);
  myDOF = -1;
}

void PlaneGCSSolver_Solver::removeConstraint(ConstraintID theID)
{
  myConstraints.erase(theID);
  if (myConstraints.empty()) {
    myEquationSystem->clear();
    myDOF = (int)myParameters.size();
  } else {
    myEquationSystem->clearByTag(theID);
    myDOF = -1;
  }
}

double* PlaneGCSSolver_Solver::createParameter()
{
  double* aResult = new double(0);
  myParameters.push_back(aResult);
  if (myConstraints.empty() && myDOF >= 0)
    ++myDOF; // calculate DoF by hand if and only if there is no constraints yet
  else
    myDiagnoseBeforeSolve = true;
  return aResult;
}

void PlaneGCSSolver_Solver::removeParameters(const GCS::SET_pD& theParams)
{
  for (int i = (int)myParameters.size() - 1; i >= 0; --i)
    if (theParams.find(myParameters[i]) != theParams.end()) {
      myParameters.erase(myParameters.begin() + i);
      --myDOF;
    }
}

void PlaneGCSSolver_Solver::initialize()
{
  Events_LongOp::start(this);
  if (myDiagnoseBeforeSolve)
    diagnose();
  myEquationSystem->declareUnknowns(myParameters);
  myEquationSystem->initSolution();
  Events_LongOp::end(this);

  myInitilized = true;
}

PlaneGCSSolver_Solver::SolveStatus PlaneGCSSolver_Solver::solve()
{
  // clear list of conflicting constraints
  if (myConfCollected) {
    myConflictingIDs.clear();
    myConfCollected = false;
  }

  if (myParameters.empty())
    return STATUS_INCONSISTENT;

  GCS::SolveStatus aResult = GCS::Success;
  Events_LongOp::start(this);
  if (myInitilized) {
    aResult = (GCS::SolveStatus)myEquationSystem->solve();
  } else {
    if (myDiagnoseBeforeSolve)
      diagnose();
    aResult = (GCS::SolveStatus)myEquationSystem->solve(myParameters);
  }
  Events_LongOp::end(this);

  // collect information about conflicting constraints every time,
  // sometimes solver reports about succeeded recalculation but has conflicting constraints
  // (for example, apply horizontal constraint for a copied feature)
  collectConflicting();
  if (!myConflictingIDs.empty())
    aResult = GCS::Failed;

  SolveStatus aStatus;
  if (aResult == GCS::Failed)
    aStatus = STATUS_FAILED;
  else {
    myEquationSystem->applySolution();
    if (myDOF < 0)
      myDOF = myEquationSystem->dofsNumber();
    aStatus = STATUS_OK;
  }

  myInitilized = false;
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

int PlaneGCSSolver_Solver::dof()
{
  if (myDOF < 0 && !myConstraints.empty())
    diagnose();
  return myDOF;
}

void PlaneGCSSolver_Solver::diagnose()
{
  myEquationSystem->declareUnknowns(myParameters);
  myDOF = myEquationSystem->diagnose();
  myDiagnoseBeforeSolve = false;
}
