// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:    SolveSpaceSolver_ConstraintWrapper.h
// Created: 2 Dec 2015
// Author:  Artem ZHIDKOV

#ifndef SolveSpaceSolver_ConstraintWrapper_H_
#define SolveSpaceSolver_ConstraintWrapper_H_

#include <SketchSolver_IConstraintWrapper.h>
#include <slvs.h>


/**
 *  Wrapper providing operations with SovleSpace constraints.
 */
class SolveSpaceSolver_ConstraintWrapper : public SketchSolver_IConstraintWrapper
{
public:
  SolveSpaceSolver_ConstraintWrapper(const ConstraintPtr&   theOriginal,
                                     const Slvs_Constraint& theConstraint);

  /// \brief Return SolveSpace constraint
  const Slvs_Constraint& constraint() const
  { return mySlvsConstraint; }
  /// \brief Return SolveSpace constraint to change
  Slvs_Constraint& changeConstraint()
  { return mySlvsConstraint; }

  /// \brief Return ID of current entity
  virtual ConstraintID id() const;

  /// \brief Change group for the constraint
  virtual void setGroup(const GroupID& theGroup);
  /// \brief Return identifier of the group the constraint belongs to
  virtual const GroupID& group() const
  { return (GroupID)mySlvsConstraint.group; }

  /// \brief Return type of current entity
  virtual SketchSolver_ConstraintType type() const;

  /// \brief Assign numeric parameter of constraint
  virtual void setValue(const double& theValue);

  /// \brief Verify the feature is used in the constraint
  virtual bool isUsed(FeaturePtr theFeature) const;
  /// \brief Verify the attribute is used in the constraint
  virtual bool isUsed(AttributePtr theAttribute) const;

  /// \brief Compare current constraint with other
  virtual bool isEqual(const ConstraintWrapperPtr& theOther);

  /// \brief Update values of parameters of this constraint by the parameters of given one
  /// \return \c true if some parameters change their values
  virtual bool update(const std::shared_ptr<SketchSolver_IConstraintWrapper>& theOther);

private:
  Slvs_Constraint mySlvsConstraint;
};

#endif
