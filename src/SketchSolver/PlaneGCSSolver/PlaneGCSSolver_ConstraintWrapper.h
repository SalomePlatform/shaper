// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:    PlaneGCSSolver_ConstraintWrapper.h
// Created: 14 Dec 2015
// Author:  Artem ZHIDKOV

#ifndef PlaneGCSSolver_ConstraintWrapper_H_
#define PlaneGCSSolver_ConstraintWrapper_H_

#include <PlaneGCSSolver_Defs.h>
#include <PlaneGCSSolver_ScalarWrapper.h>

/**
 *  Wrapper providing operations with PlaneGCS constraints.
 */
class PlaneGCSSolver_ConstraintWrapper
{
public:
  PlaneGCSSolver_ConstraintWrapper(const GCSConstraintPtr&            theConstraint,
                                   const SketchSolver_ConstraintType& theType);
  PlaneGCSSolver_ConstraintWrapper(const std::list<GCSConstraintPtr>& theConstraints,
                                   const SketchSolver_ConstraintType& theType);

  /// \brief Return list of constraints
  const std::list<GCSConstraintPtr>& constraints() const
  { return myGCSConstraints; }
  /// \brief Change list of constraints
  void setConstraints(const std::list<GCSConstraintPtr>& theConstraints)
  { myGCSConstraints = theConstraints; }

  /// \brief Return ID of current constraint
  const ConstraintID& id() const
  { return myID; }
  /// \brief Change constraint ID
  virtual void setId(const ConstraintID& theID);

  /// \brief Return type of current entity
  virtual SketchSolver_ConstraintType type() const
  { return myType; }

  /// \brief Assign numeric parameter of constraint
  virtual void setValue(const double& theValue);
  /// \brief Return numeric parameter of constraint
  virtual double value() const;

  /// \brief Change parameter representing the value of constraint
  void setValueParameter(const ScalarWrapperPtr& theValue);
  /// \brief Return parametric representation of constraint value
  const ScalarWrapperPtr& valueParameter() const
  { return myValueParam; }

private:
  ConstraintID                myID;
  SketchSolver_ConstraintType myType;
  ScalarWrapperPtr            myValueParam;
  std::list<GCSConstraintPtr> myGCSConstraints;
};

typedef std::shared_ptr<PlaneGCSSolver_ConstraintWrapper> ConstraintWrapperPtr;

#endif
