// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:    PlaneGCSSolver_ConstraintWrapper.h
// Created: 14 Dec 2015
// Author:  Artem ZHIDKOV

#ifndef PlaneGCSSolver_ConstraintWrapper_H_
#define PlaneGCSSolver_ConstraintWrapper_H_

#include <PlaneGCSSolver_Defs.h>
#include <PlaneGCSSolver_Solver.h>
#include <SketchSolver_IConstraintWrapper.h>


/**
 *  Wrapper providing operations with SovleSpace constraints.
 */
class PlaneGCSSolver_ConstraintWrapper : public SketchSolver_IConstraintWrapper
{
public:
  PlaneGCSSolver_ConstraintWrapper(const ConstraintPtr& theOriginal,
                                   const GCSConstraintPtr& theConstraint,
                                   const SketchSolver_ConstraintType& theType);
  PlaneGCSSolver_ConstraintWrapper(const ConstraintPtr& theOriginal,
                                   const std::list<GCSConstraintPtr>& theConstraints,
                                   const SketchSolver_ConstraintType& theType);

  /// \brief Return list of constraints
  const std::list<GCSConstraintPtr>& constraints() const
  { return myGCSConstraints; }
  /// \brief Change list of constraints
  void setConstraints(const std::list<GCSConstraintPtr>& theConstraints)
  { myGCSConstraints = theConstraints; }

  /// \brief Return ID of current entity
  virtual ConstraintID id() const
  { return myID; }
  /// \brief Change constraint ID
  void setId( const ConstraintID& theID);

  /// \brief Change group for the constraint
  virtual void setGroup(const GroupID& theGroup);
  /// \brief Return identifier of the group the constraint belongs to
  virtual GroupID group() const
  { return myGroup; }

  /// \brief Return type of current entity
  virtual SketchSolver_ConstraintType type() const
  { return myType; }

  /// \brief Assign numeric parameter of constraint
  virtual void setValue(const double& theValue);

  /// \brief Change parameter representing the value of constraint
  void setValueParameter(const ParameterWrapperPtr& theValue);
  /// \brief Return parametric representation of constraint value
  const ParameterWrapperPtr& valueParameter() const
  { return myValueParam; }

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
  ConstraintID     myID;
  GroupID          myGroup;
  SketchSolver_ConstraintType myType;
  ParameterWrapperPtr myValueParam;
  std::list<GCSConstraintPtr> myGCSConstraints;
};

#endif
