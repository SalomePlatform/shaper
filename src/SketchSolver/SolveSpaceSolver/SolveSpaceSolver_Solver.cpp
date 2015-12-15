// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:    SolveSpaceSolver_Solver.cpp
// Created: 07 May 2014
// Author:  Artem ZHIDKOV

#include "SolveSpaceSolver_Solver.h"
#include <Events_LongOp.h>

SolveSpaceSolver_Solver::SolveSpaceSolver_Solver()
{
  // Nullify all elements of the set of equations
  myEquationsSystem.param = 0;
  myEquationsSystem.params = 0;
  myEquationsSystem.entity = 0;
  myEquationsSystem.entities = 0;
  myEquationsSystem.constraint = 0;
  myEquationsSystem.constraints = 0;
  myEquationsSystem.failed = 0;
  myEquationsSystem.faileds = 0;

  myEquationsSystem.dragged[0] = 0;
  myEquationsSystem.dragged[1] = 0;
  myEquationsSystem.dragged[2] = 0;
  myEquationsSystem.dragged[3] = 0;

  // If the set of constraints is inconsistent,
  // the failed field will contain wrong constraints
  myEquationsSystem.calculateFaileds = 0;
}

SolveSpaceSolver_Solver::~SolveSpaceSolver_Solver()
{
  if (myEquationsSystem.constraint)
    delete[] myEquationsSystem.constraint;
  myEquationsSystem.constraint = 0;
  if (myEquationsSystem.failed)
    delete[] myEquationsSystem.failed;
  myEquationsSystem.failed = 0;
}

void SolveSpaceSolver_Solver::setParameters(Slvs_Param* theParameters, int theSize)
{
  myEquationsSystem.param = theParameters;
  myEquationsSystem.params = theSize;
}


void SolveSpaceSolver_Solver::setDraggedParameters(const Slvs_hParam* theDragged)
{
  for (unsigned int i = 0; i < 4; i++)
    myEquationsSystem.dragged[i] = theDragged[i];
}

void SolveSpaceSolver_Solver::setEntities(Slvs_Entity* theEntities, int theSize)
{
  myEquationsSystem.entity = theEntities;
  myEquationsSystem.entities = theSize;
}

void SolveSpaceSolver_Solver::setConstraints(Slvs_Constraint* theConstraints, int theSize)
{
  if (!myEquationsSystem.constraint) {
    myEquationsSystem.constraint = new Slvs_Constraint[theSize];
    myEquationsSystem.constraints = theSize;
    myEquationsSystem.failed = new Slvs_hConstraint[theSize];
  }
  else if (myEquationsSystem.constraints != theSize) {
    if (theSize > myEquationsSystem.constraints) {
      delete[] myEquationsSystem.constraint;
      myEquationsSystem.constraint = new Slvs_Constraint[theSize];
      if (myEquationsSystem.failed)
        delete[] myEquationsSystem.failed;
      myEquationsSystem.failed = new Slvs_hConstraint[theSize];
    }
    myEquationsSystem.constraints = theSize;
  }
  memcpy(myEquationsSystem.constraint, theConstraints, theSize * sizeof(Slvs_Constraint));
  memset(myEquationsSystem.failed, SLVS_C_UNKNOWN, theSize * sizeof(Slvs_hConstraint));
}


SketchSolver_SolveStatus SolveSpaceSolver_Solver::solve()
{
  if (myEquationsSystem.constraints <= 0)
    return STATUS_EMPTYSET;

  myEquationsSystem.calculateFaileds = myFindFaileds ? 1 : 0;

  Events_LongOp::start(this);
  Slvs_Solve(&myEquationsSystem, myGroup);
  Events_LongOp::end(this);

  SketchSolver_SolveStatus aStatus;
  switch (myEquationsSystem.result) {
  case SLVS_RESULT_OKAY:
    aStatus = STATUS_OK;
    break;
  case SLVS_RESULT_INCONSISTENT:
  case SLVS_RESULT_DIDNT_CONVERGE:
  case SLVS_RESULT_TOO_MANY_UNKNOWNS:
    aStatus = STATUS_INCONSISTENT;
    break;
  default:
    aStatus = STATUS_FAILED;
  }
  return aStatus;
}
