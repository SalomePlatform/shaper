// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:    PlaneGCSSolver_ScalarWrapper.cpp
// Created: 14 Dec 2015
// Author:  Artem ZHIDKOV

#include <PlaneGCSSolver_ScalarWrapper.h>

#include <cmath>

PlaneGCSSolver_ScalarWrapper::PlaneGCSSolver_ScalarWrapper(double *const theParam)
  : myValue(theParam)
{
}

void PlaneGCSSolver_ScalarWrapper::setValue(double theValue)
{
  *myValue = theValue;
}

double PlaneGCSSolver_ScalarWrapper::value() const
{
  return *myValue;
}
