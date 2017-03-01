// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:    SketchSolver_IConstraintWrapper.h
// Created: 30 Nov 2015
// Author:  Artem ZHIDKOV

#ifndef SketchSolver_IConstraintWrapper_H_
#define SketchSolver_IConstraintWrapper_H_

#include <SketchSolver_IEntityWrapper.h>

#include <SketchPlugin_Constraint.h>

#include <list>
#include <memory>

/// Types of constraints
enum SketchSolver_ConstraintType {
  CONSTRAINT_UNKNOWN = 0,
  CONSTRAINT_COINCIDENCE,      // base coincidence if we don't know exact type yet
  CONSTRAINT_PT_PT_COINCIDENT,
  CONSTRAINT_PT_ON_LINE,
  CONSTRAINT_PT_ON_CIRCLE,
  CONSTRAINT_MIDDLE_POINT,
  CONSTRAINT_DISTANCE,         // base distance if we don't know the measured objects yet
  CONSTRAINT_PT_PT_DISTANCE,
  CONSTRAINT_PT_LINE_DISTANCE,
  CONSTRAINT_RADIUS,
  CONSTRAINT_ANGLE,
  CONSTRAINT_FIXED,
  CONSTRAINT_HORIZONTAL,
  CONSTRAINT_VERTICAL,
  CONSTRAINT_PARALLEL,
  CONSTRAINT_PERPENDICULAR,
  CONSTRAINT_SYMMETRIC,
  CONSTRAINT_EQUAL,           // base equality if we don't know the measured objects yet
  CONSTRAINT_EQUAL_LINES,
  CONSTRAINT_EQUAL_LINE_ARC,
  CONSTRAINT_EQUAL_RADIUS,
  CONSTRAINT_TANGENT,         // base tangency if we don't know the measured objects yet
  CONSTRAINT_TANGENT_CIRCLE_LINE,
  CONSTRAINT_TANGENT_CIRCLE_CIRCLE,
  CONSTRAINT_COLLINEAR,
  CONSTRAINT_MULTI_TRANSLATION,
  CONSTRAINT_MULTI_ROTATION
};

/**
 *  Wrapper providing operations with constraints regardless the solver.
 */
class SketchSolver_IConstraintWrapper
{
public:
  virtual ~SketchSolver_IConstraintWrapper() {}

  /// \brief Return ID of current constraint
  const ConstraintID& id() const
  { return myID; }
  /// \brief Change constraint ID
  virtual void setId( const ConstraintID& theID) = 0;

  /// \brief Return type of current entity
  virtual SketchSolver_ConstraintType type() const = 0;

  /// \brief Assign numeric parameter of constraint
  virtual void setValue(const double& theValue) = 0;
  /// \brief Return numeric parameter of constraint
  virtual double value() const = 0;

protected:
  ConstraintID myID;
};

typedef std::shared_ptr<SketchSolver_IConstraintWrapper> ConstraintWrapperPtr;

#endif
