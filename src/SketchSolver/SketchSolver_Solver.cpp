// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:    SketchSolver_Solver.cpp
// Created: 07 May 2014
// Author:  Artem ZHIDKOV

#include "SketchSolver_Solver.h"
#include <Events_LongOp.h>

SketchSolver_Solver::SketchSolver_Solver()
{
  myGroupID = 0;
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
  myEquationsSystem.calculateFaileds = 1;
}

SketchSolver_Solver::~SketchSolver_Solver()
{
  if (myEquationsSystem.constraint)
    delete[] myEquationsSystem.constraint;
  myEquationsSystem.constraint = 0;
  if (myEquationsSystem.failed)
    delete[] myEquationsSystem.failed;
  myEquationsSystem.failed = 0;
}

void SketchSolver_Solver::setParameters(Slvs_Param* theParameters, int theSize)
{
  myEquationsSystem.param = theParameters;
  myEquationsSystem.params = theSize;
}


void SketchSolver_Solver::setDraggedParameters(const Slvs_hParam* theDragged)
{
  for (unsigned int i = 0; i < 4; i++)
    myEquationsSystem.dragged[i] = theDragged[i];
}

void SketchSolver_Solver::setEntities(Slvs_Entity* theEntities, int theSize)
{
  myEquationsSystem.entity = theEntities;
  myEquationsSystem.entities = theSize;
}

void SketchSolver_Solver::setConstraints(Slvs_Constraint* theConstraints, int theSize)
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


int SketchSolver_Solver::solve()
{
  if (myEquationsSystem.constraints <= 0)
    return SLVS_RESULT_EMPTY_SET;

  Events_LongOp::start(this);
  Slvs_Solve(&myEquationsSystem, myGroupID);
  Events_LongOp::end(this);

  return myEquationsSystem.result;
}

bool SketchSolver_Solver::getResult(std::vector<Slvs_Param>& theParameters)
{
  if (myEquationsSystem.result != SLVS_RESULT_OKAY)
    return false;

  if (theParameters.size() != myEquationsSystem.params)
    return false;  // number of parameters is not the same

  std::vector<Slvs_Param>::iterator aParamIter = theParameters.begin();
  for (int i = 0; i < myEquationsSystem.params; i++, aParamIter++) {
    if (myEquationsSystem.param[i].h != aParamIter->h)
      return false;  // sequence of parameters was changed
    aParamIter->val = myEquationsSystem.param[i].val;
  }

  return true;
}
