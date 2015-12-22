// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:    PlaneGCSSolver_AngleWrapper.h
// Created: 18 Dec 2015
// Author:  Artem ZHIDKOV

#ifndef PlaneGCSSolver_AngleWrapper_H_
#define PlaneGCSSolver_AngleWrapper_H_

#include <PlaneGCSSolver_ParameterWrapper.h>

/**
 *  Wrapper providing operations with angular parameters in PlaneGCS.
 */
class PlaneGCSSolver_AngleWrapper : public PlaneGCSSolver_ParameterWrapper
{
public:
  PlaneGCSSolver_AngleWrapper(double *const theParam);
  ~PlaneGCSSolver_AngleWrapper() {}

  /// \brief Change value of parameter
  virtual void setValue(double theValue);
  /// \brief Return value of parameter
  virtual double value() const;
};

#endif
