// File:    SketchSolver_Solver.cpp
// Created: 07 May 2014
// Author:  Artem ZHIDKOV

#include "SketchSolver_Solver.h"

SketchSolver_Solver::SketchSolver_Solver()
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

  // If the set of constraints is inconsistent,
  // the failed field will contain wrong constraints
  myEquationsSystem.calculateFaileds = 1;
}

SketchSolver_Solver::~SketchSolver_Solver()
{
  if (myEquationsSystem.param)
    delete [] myEquationsSystem.param;
  if (myEquationsSystem.entity)
    delete [] myEquationsSystem.entity;
  if (myEquationsSystem.constraint)
    delete [] myEquationsSystem.constraint;
  if (myEquationsSystem.failed)
    delete [] myEquationsSystem.failed;
}

void SketchSolver_Solver::setParameters(const std::vector<Slvs_Param>& theParameters)
{
  if (theParameters.size() != myEquationsSystem.params) // number of parameters was changed => reallocate the memory
  {
    if (myEquationsSystem.param)
      delete [] myEquationsSystem.param;
    myEquationsSystem.params = theParameters.size();
    myEquationsSystem.param = new Slvs_Param[theParameters.size()];
  }

  // Copy data
  std::vector<Slvs_Param>::const_iterator aParamIter = theParameters.begin();
  for (int i = 0; i < myEquationsSystem.params; i++, aParamIter++)
    myEquationsSystem.param[i] = *aParamIter;
}

void SketchSolver_Solver::setEntities(const std::vector<Slvs_Entity>& theEntities)
{
  if (theEntities.size() != myEquationsSystem.entities) // number of entities was changed => reallocate the memory
  {
    if (myEquationsSystem.entity)
      delete [] myEquationsSystem.entity;
    myEquationsSystem.entities = theEntities.size();
    myEquationsSystem.entity = new Slvs_Entity[theEntities.size()];
  }

  // Copy data
  std::vector<Slvs_Entity>::const_iterator aEntIter = theEntities.begin();
  for (int i = 0; i < myEquationsSystem.entities; i++, aEntIter++)
    myEquationsSystem.entity[i] = *aEntIter;
}

void SketchSolver_Solver::setConstraints(const std::vector<Slvs_Constraint>& theConstraints)
{
  if (theConstraints.size() != myEquationsSystem.constraints) // number of constraints was changed => reallocate the memory
  {
    if (myEquationsSystem.constraint)
      delete [] myEquationsSystem.constraint;
    myEquationsSystem.constraints = theConstraints.size();
    myEquationsSystem.constraint = new Slvs_Constraint[theConstraints.size()];
  }

  // Copy data
  std::vector<Slvs_Constraint>::const_iterator aConstrIter = theConstraints.begin();
  for (int i = 0; i < myEquationsSystem.constraints; i++, aConstrIter++)
    myEquationsSystem.constraint[i] = *aConstrIter;
}

int SketchSolver_Solver::solve()
{
  if (myEquationsSystem.constraints <= 0)
    return SLVS_RESULT_EMPTY_SET;

  Slvs_Solve(&myEquationsSystem, myGroupID);

  return myEquationsSystem.result;
}

bool SketchSolver_Solver::getResult(std::vector<Slvs_Param>& theParameters)
{
  if (myEquationsSystem.result != SLVS_RESULT_OKAY)
    return false;

  if (theParameters.size() != myEquationsSystem.params)
    return false; // number of parameters is not the same

  std::vector<Slvs_Param>::iterator aParamIter = theParameters.begin();
  for (int i = 0; i < myEquationsSystem.params; i++, aParamIter++)
  {
    if (myEquationsSystem.param[i].h != aParamIter->h)
      return false; // sequence of parameters was changed
    aParamIter->val = myEquationsSystem.param[i].val;
  }

  return true;
}
