// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:    SketchSolver_IParameterWrapper.h
// Created: 1 Dec 2015
// Author:  Artem ZHIDKOV

#ifndef SketchSolver_IParameterWrapper_H_
#define SketchSolver_IParameterWrapper_H_

#include <SketchSolver.h>

#include <memory>

/**
 *  Wrapper providing operations with parameters regardless the solver.
 */
class SketchSolver_IParameterWrapper
{
public:
  virtual ~SketchSolver_IParameterWrapper() {}

  /// \brief Return ID of current parameter
  virtual ParameterID id() const = 0;

  /// \brief Change group for the parameter
  virtual void setGroup(const GroupID& theGroup) = 0;
  /// \brief Return identifier of the group the parameter belongs to
  virtual GroupID group() const = 0;

  /// \brief Change value of parameter
  virtual void setValue(double theValue) = 0;
  /// \brief Return value of parameter
  virtual double value() const = 0;

  /// \brief Set or unset flag the parameter is given by expression
  void setIsParametric(bool isParametric)
  { myIsParametric = isParametric; }
  /// \brief Show the parameter is an expression
  bool isParametric() const
  { return myIsParametric; }

  /// \brief Compare current parameter with other
  virtual bool isEqual(const std::shared_ptr<SketchSolver_IParameterWrapper>& theOther) = 0;

  /// \brief Update value of parameter by the given one
  /// \return \c true if the value of parameter is changed
  virtual bool update(const std::shared_ptr<SketchSolver_IParameterWrapper>& theOther) = 0;

protected:
  bool myIsParametric; ///< indicate the parameter is given by parametric expression
};

typedef std::shared_ptr<SketchSolver_IParameterWrapper> ParameterWrapperPtr;

#endif
