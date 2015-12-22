// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:    PlaneGCSSolver_AngleWrapper.cpp
// Created: 18 Dec 2015
// Author:  Artem ZHIDKOV

#include <PlaneGCSSolver_AngleWrapper.h>

#include <math.h>

static double deg2rad(double theDegrees)
{
  return theDegrees * M_PI / 180.0;
}

static double rad2deg(double theRadians)
{
  return theRadians * 180.0 / M_PI;
}

PlaneGCSSolver_AngleWrapper::PlaneGCSSolver_AngleWrapper(double *const theParam)
  : PlaneGCSSolver_ParameterWrapper(theParam)
{
  setValue(*myValue);
}

void PlaneGCSSolver_AngleWrapper::setValue(double theValue)
{
  *(myValue) = deg2rad(theValue);
}

double PlaneGCSSolver_AngleWrapper::value() const
{
  return rad2deg(*myValue);
}
