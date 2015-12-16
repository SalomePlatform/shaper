// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:    PlaneGCSSolver_ParameterWrapper.h
// Created: 14 Dec 2015
// Author:  Artem ZHIDKOV

#ifndef PlaneGCSSolver_ParameterWrapper_H_
#define PlaneGCSSolver_ParameterWrapper_H_

#include <PlaneGCSSolver_Defs.h>
#include <SketchSolver_IParameterWrapper.h>

/**
 *  Wrapper providing operations with parameters in PlaneGCS.
 */
class PlaneGCSSolver_ParameterWrapper : public SketchSolver_IParameterWrapper
{
public:
  PlaneGCSSolver_ParameterWrapper(double *const theParam);
  virtual ~PlaneGCSSolver_ParameterWrapper();

  /// \brief Return ID of current parameter
  virtual ParameterID id() const
  { return myID; }

  double* parameter() const
  { return myValue; }

  /// \brief Change group for the parameter
  virtual void setGroup(const GroupID& theGroup)
  { myGroup = theGroup; }

  /// \brief Return identifier of the group the parameter belongs to
  virtual GroupID group() const
  { return myGroup; }

  /// \brief Change value of parameter
  virtual void setValue(double theValue);
  /// \brief Return value of parameter
  virtual double value() const;

  /// \brief Compare current parameter with other
  virtual bool isEqual(const ParameterWrapperPtr& theOther);

  /// \brief Update value of parameter by the given one
  /// \return \c true if the value of parameter is changed
  virtual bool update(const ParameterWrapperPtr& theOther);

  /// \brief Shows the parameter is added to full list of parameters
  bool isProcessed() const
  { return myProcessing; }
  /// \brief Set the flag that parameter is under processing
  void setProcessed(bool isProc)
  { myProcessing = isProc; }

protected:
  ParameterID myID;
  GroupID     myGroup;
  double*     myValue;
  bool        myProcessing; ///< identify that the parameter is already in the list of parameters
};

#endif
