// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:    PlaneGCSSolver_ScalarWrapper.h
// Created: 16 Dec 2015
// Author:  Artem ZHIDKOV

#ifndef PlaneGCSSolver_ScalarWrapper_H_
#define PlaneGCSSolver_ScalarWrapper_H_

#include <PlaneGCSSolver_Defs.h>
#include <PlaneGCSSolver_EntityWrapper.h>

/**
 *  Wrapper providing operations with PlaneGCS scalars.
 */
class PlaneGCSSolver_ScalarWrapper : public PlaneGCSSolver_EntityWrapper
{
public:
  PlaneGCSSolver_ScalarWrapper(double *const theParam);

  /// \brief Return PlaneGCS parameter
  double* scalar() const
  { return myValue; }

  /// \brief Change value of parameter
  virtual void setValue(double theValue);
  /// \brief Return value of parameter
  virtual double value() const;

  /// \brief Return type of current entity
  virtual SketchSolver_EntityType type() const
  { return ENTITY_SCALAR; }

protected:
  double* myValue; ///< pointer to value provided by the storage
};

typedef std::shared_ptr<PlaneGCSSolver_ScalarWrapper> ScalarWrapperPtr;

#endif
