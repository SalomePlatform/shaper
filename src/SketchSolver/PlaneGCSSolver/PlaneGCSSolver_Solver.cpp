// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:    PlaneGCSSolver_Solver.cpp
// Created: 14 Dec 2014
// Author:  Artem ZHIDKOV

#include <PlaneGCSSolver_Solver.h>
#include <Events_LongOp.h>


PlaneGCSSolver_Solver::PlaneGCSSolver_Solver()
  : myEquationSystem(new GCS::System),
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

void PlaneGCSSolver_Solver::addConstraint(GCSConstraintPtr theConstraint,
                                          const SketchSolver_ConstraintType theType)
{
  myEquationSystem->addConstraint(theConstraint.get());
  myConstraints[theConstraint->getTag()].insert(theConstraint);
  myDOF = -1;

  // Workaround: avoid tangent constraint in the list of redundant
  if (theType == CONSTRAINT_TANGENT_CIRCLE_LINE ||
      theType == CONSTRAINT_TANGENT_CIRCLE_CIRCLE ||
      theType == CONSTRAINT_PT_PT_COINCIDENT ||
      theType == CONSTRAINT_PT_ON_CIRCLE ||
      theType == CONSTRAINT_PT_ON_LINE)
    myConstraintIDsNotRedundant.insert(theConstraint->getTag());
}

void PlaneGCSSolver_Solver::removeConstraint(ConstraintID theID)
{
  myConstraints.erase(theID);
  myConstraintIDsNotRedundant.erase(theID);
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
  if (myDOF >= 0)
    ++myDOF;
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

PlaneGCSSolver_Solver::SolveStatus PlaneGCSSolver_Solver::solve()
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

  GCS::VEC_I aRedundant;
  myEquationSystem->getRedundant(aRedundant);
  if (!aRedundant.empty()) {
    collectConflicting();
    if (!myConflictingIDs.empty())
      aResult = GCS::Failed;
  }

  SolveStatus aStatus;
  if (aResult == GCS::Success) {
    myEquationSystem->applySolution();
    if (myDOF < 0)
      myDOF = myEquationSystem->dofsNumber();
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
  // Workaround: avoid conflicting tangent constraints
  GCS::VEC_I aTemp = aConflict;
  aConflict.clear();
  for (GCS::VEC_I::iterator anIt = aTemp.begin(); anIt != aTemp.end(); ++anIt)
    if (myConstraintIDsNotRedundant.find(*anIt) == myConstraintIDsNotRedundant.end())
      aConflict.push_back(*anIt);
  myConflictingIDs.insert(aConflict.begin(), aConflict.end());

  myConfCollected = true;
}

int PlaneGCSSolver_Solver::dof()
{
  if (myDOF < 0 && !myConstraints.empty())
    solve();
  return myDOF;
}
