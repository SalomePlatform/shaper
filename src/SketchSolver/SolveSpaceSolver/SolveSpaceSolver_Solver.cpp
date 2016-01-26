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

  myParamsCopy = 0;
}

SolveSpaceSolver_Solver::~SolveSpaceSolver_Solver()
{
  if (myEquationsSystem.constraint)
    delete[] myEquationsSystem.constraint;
  myEquationsSystem.constraint = 0;
  if (myEquationsSystem.failed)
    delete[] myEquationsSystem.failed;
  myEquationsSystem.failed = 0;
  if (myParamsCopy)
    delete [] myParamsCopy;
  myParamsCopy = 0;
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
  //if (myEquationsSystem.constraints <= 0)
  //  return STATUS_EMPTYSET;

  myEquationsSystem.calculateFaileds = 0;
  if (myFindFaileds) {
    myEquationsSystem.calculateFaileds = 1;
    myEquationsSystem.faileds = myEquationsSystem.constraints;
  }

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

  if (aStatus == STATUS_OK) {
    // additional verification of arcs to be non-degenerated
    if (hasDegeneratedArcs()) {
      undo();
      aStatus = STATUS_INCONSISTENT;
    }
  }

  return aStatus;
}

void SolveSpaceSolver_Solver::prepare()
{
  // make a copy of parameters to be able to make undo
  if (myParamsCopy)
    delete [] myParamsCopy;
  myParamsCopy = new Slvs_Param[myEquationsSystem.params];
  memcpy(myParamsCopy, myEquationsSystem.param, myEquationsSystem.params * sizeof(Slvs_Param));
}

void SolveSpaceSolver_Solver::undo()
{
  if (myParamsCopy) {
    memcpy(myEquationsSystem.param, myParamsCopy, myEquationsSystem.params * sizeof(Slvs_Param));
    delete [] myParamsCopy;
  }
  myParamsCopy = 0;
}


bool SolveSpaceSolver_Solver::hasDegeneratedArcs() const
{
  const double aTol2 = tolerance * tolerance;
  double anArcPoints[3][2];

  for (int anEnt = 0; anEnt < myEquationsSystem.entities; ++anEnt) {
    const Slvs_Entity& anEntity = myEquationsSystem.entity[anEnt];
    if (anEntity.type != SLVS_E_ARC_OF_CIRCLE)
      continue;

    for (int aPnt = 0; aPnt < 3; ++aPnt) {
      // search point of arc
      const int aShift = anEntity.point[aPnt] - anEntity.h;
      int aPntInd = anEnt + aShift;
      int aStep = 1;
      if (aPntInd < 0)
        aPntInd = 0;
      else if (aPntInd >= myEquationsSystem.entities)
        aPntInd = myEquationsSystem.entities - 1;
      if (myEquationsSystem.entity[aPntInd].h > anEntity.point[aPnt])
        aStep = -1;
      for (; aPntInd >=0 && aPntInd < myEquationsSystem.entities; aPntInd += aStep)
        if (myEquationsSystem.entity[aPntInd].h == anEntity.point[aPnt])
          break;

      // search coordinates of the point
      int aParamInd = myEquationsSystem.entity[aPntInd].param[0];
      if (aParamInd >= myEquationsSystem.params) {
        aParamInd = myEquationsSystem.params - 1;
        aStep = -1;
      }
      else if ((int)myEquationsSystem.param[aParamInd].h > aParamInd)
        aStep = -1;
      else aStep = 1;

      for (; aParamInd >=0 && aParamInd < myEquationsSystem.params; aParamInd += aStep)
        if (myEquationsSystem.param[aParamInd].h == myEquationsSystem.entity[aPntInd].param[0])
          break;
      anArcPoints[aPnt][0] = myEquationsSystem.param[aParamInd].val;
      anArcPoints[aPnt][1] = myEquationsSystem.param[aParamInd+1].val;
    }

    // check radius of arc
    anArcPoints[1][0] -= anArcPoints[0][0];
    anArcPoints[1][1] -= anArcPoints[0][1];
    anArcPoints[2][0] -= anArcPoints[0][0];
    anArcPoints[2][1] -= anArcPoints[0][1];
    if (anArcPoints[1][0] * anArcPoints[1][0] + anArcPoints[1][1] * anArcPoints[1][1] < aTol2 ||
        anArcPoints[2][0] * anArcPoints[2][0] + anArcPoints[2][1] * anArcPoints[2][1] < aTol2)
      return true;
  }
  return false;
}

bool SolveSpaceSolver_Solver::isConflicting(const ConstraintID& theConstraint) const
{
  for (int i = 0; i < myEquationsSystem.faileds; ++i)
    if (myEquationsSystem.failed[i] == (Slvs_hConstraint)theConstraint)
      return true;
  return false;
}
