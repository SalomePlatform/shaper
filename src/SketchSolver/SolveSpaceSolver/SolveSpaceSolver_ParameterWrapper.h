// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:    SolveSpaceSolver_ParameterWrapper.h
// Created: 2 Dec 2015
// Author:  Artem ZHIDKOV

#ifndef SolveSpaceSolver_ParameterWrapper_H_
#define SolveSpaceSolver_ParameterWrapper_H_

#include <SketchSolver_IParameterWrapper.h>

#include <slvs.h>

/**
 *  Wrapper providing operations with parameters in SolveSpace.
 */
class SolveSpaceSolver_ParameterWrapper : public SketchSolver_IParameterWrapper
{
public:
  SolveSpaceSolver_ParameterWrapper(const Slvs_Param& theParam);

  /// \brief Return SolveSpace parameter
  const Slvs_Param& parameter() const
  { return myParameter; }
  /// \brief Return SolveSpace parameter to change
  Slvs_Param& changeParameter()
  { return myParameter; }

  /// \brief Return ID of current parameter
  virtual ParameterID id() const;

  /// \brief Change group for the parameter
  virtual void setGroup(const GroupID& theGroup)
  { myParameter.group = (Slvs_hGroup)theGroup; }

  /// \brief Return identifier of the group the parameter belongs to
  virtual const GroupID& group() const
  { return (GroupID)myParameter.group; }

  /// \brief Change value of parameter
  virtual void setValue(double theValue);
  /// \brief Return value of parameter
  virtual double value() const;

  /// \brief Compare current parameter with other
  virtual bool isEqual(const ParameterWrapperPtr& theOther);

  /// \brief Update value of parameter by the given one
  /// \return \c true if the value of parameter is changed
  virtual bool update(const std::shared_ptr<SketchSolver_IParameterWrapper>& theOther);

private:
  Slvs_Param myParameter;
};

#endif
