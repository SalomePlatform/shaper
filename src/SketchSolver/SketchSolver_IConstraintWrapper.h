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
  CONSTRAINT_TANGENT_ARC_LINE,
  CONSTRAINT_TANGENT_CIRCLE_LINE,
  CONSTRAINT_TANGENT_ARC_ARC,
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

  /// \brief Return base feature
  const ConstraintPtr& baseConstraint() const
  { return myBaseConstraint; }

  /// \brief Return ID of current entity
  virtual ConstraintID id() const = 0;

  /// \brief Change group for the constraint
  virtual void setGroup(const GroupID& theGroup) = 0;
  /// \brief Return identifier of the group the constraint belongs to
  virtual GroupID group() const = 0;

  /// \brief Return type of current entity
  virtual SketchSolver_ConstraintType type() const = 0;

  /// \brief Assign list of constrained objects
  void setEntities(const std::list<EntityWrapperPtr>& theEntities)
  { myConstrained = theEntities; }
  /// \brief Return list of constrained objects
  const std::list<EntityWrapperPtr>& entities() const
  { return myConstrained; }

  /// \brief Assign numeric parameter of constraint
  virtual void setValue(const double& theValue)
  { myValue = theValue; }
  /// \brief Return numeric parameter of constraint
  const double& value() const
  { return myValue; }

  /// \brief Store a boolean flag for full value using
  void setIsFullValue(const bool& theFullValue)
  { myIsFullValue = theFullValue; }
  /// \brief Return a flag of a full value using
  const bool& isFullValue() const
  { return myIsFullValue; }

  /// \brief Verify the feature is used in the constraint
  virtual bool isUsed(FeaturePtr theFeature) const = 0;
  /// \brief Verify the attribute is used in the constraint
  virtual bool isUsed(AttributePtr theAttribute) const = 0;

  /// \brief Compare current constraint with other
  virtual bool isEqual(const std::shared_ptr<SketchSolver_IConstraintWrapper>& theOther) = 0;

  /// \brief Update values of parameters of this constraint by the parameters of given one
  /// \return \c true if some parameters change their values
  virtual bool update(const std::shared_ptr<SketchSolver_IConstraintWrapper>& theOther) = 0;

protected:
  ConstraintPtr               myBaseConstraint;
  std::list<EntityWrapperPtr> myConstrained;
  double                      myValue;
  bool                        myIsFullValue;
};

typedef std::shared_ptr<SketchSolver_IConstraintWrapper> ConstraintWrapperPtr;

#endif
