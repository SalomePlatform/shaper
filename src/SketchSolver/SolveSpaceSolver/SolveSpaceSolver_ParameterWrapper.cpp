// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:    SolveSpaceSolver_ParameterWrapper.cpp
// Created: 2 Dec 2015
// Author:  Artem ZHIDKOV

#include <SolveSpaceSolver_ParameterWrapper.h>

#include <math.h>

SolveSpaceSolver_ParameterWrapper::SolveSpaceSolver_ParameterWrapper(const Slvs_Param& theParam)
  : myParameter(theParam)
{
}

ParameterID SolveSpaceSolver_ParameterWrapper::id() const
{
  return (ParameterID)myParameter.h;
}

void SolveSpaceSolver_ParameterWrapper::setValue(double theValue)
{
  myParameter.val = theValue;
}

double SolveSpaceSolver_ParameterWrapper::value() const
{
  return myParameter.val;
}

bool SolveSpaceSolver_ParameterWrapper::isEqual(const ParameterWrapperPtr& theOther)
{
  std::shared_ptr<SolveSpaceSolver_ParameterWrapper> anOtherParam = 
      std::dynamic_pointer_cast<SolveSpaceSolver_ParameterWrapper>(theOther);
  return anOtherParam && fabs(value() - anOtherParam->value()) < tolerance;
}

bool SolveSpaceSolver_ParameterWrapper::update(const ParameterWrapperPtr& theOther)
{
  std::shared_ptr<SolveSpaceSolver_ParameterWrapper> anOther =
      std::dynamic_pointer_cast<SolveSpaceSolver_ParameterWrapper>(theOther);
  if (fabs(value() - anOther->value()) < tolerance)
    return false;
  myParameter.val = anOther->value();
  myIsParametric = theOther->isParametric();
  return true;
}
