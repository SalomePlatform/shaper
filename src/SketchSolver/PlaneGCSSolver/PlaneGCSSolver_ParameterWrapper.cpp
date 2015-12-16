// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:    PlaneGCSSolver_ParameterWrapper.cpp
// Created: 14 Dec 2015
// Author:  Artem ZHIDKOV

#include <PlaneGCSSolver_ParameterWrapper.h>

#include <math.h>

PlaneGCSSolver_ParameterWrapper::PlaneGCSSolver_ParameterWrapper(double *const theParam)
  : myValue(theParam),
    myProcessing(false)
{
}

PlaneGCSSolver_ParameterWrapper::~PlaneGCSSolver_ParameterWrapper()
{
  delete myValue;
}

void PlaneGCSSolver_ParameterWrapper::setValue(double theValue)
{
  *(myValue) = theValue;
}

double PlaneGCSSolver_ParameterWrapper::value() const
{
  return *(myValue);
}

bool PlaneGCSSolver_ParameterWrapper::isEqual(const ParameterWrapperPtr& theOther)
{
  return fabs(value() - theOther->value()) < tolerance;
}

bool PlaneGCSSolver_ParameterWrapper::update(const ParameterWrapperPtr& theOther)
{
  if (fabs(value() - theOther->value()) < tolerance)
    return false;
  setValue(theOther->value());
  myIsParametric = theOther->isParametric();
  return true;
}
