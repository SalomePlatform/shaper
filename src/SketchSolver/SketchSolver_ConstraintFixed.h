// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:    SketchSolver_ConstraintFixed.h
// Created: 30 Mar 2015
// Author:  Artem ZHIDKOV

#ifndef SketchSolver_ConstraintFixed_H_
#define SketchSolver_ConstraintFixed_H_

#include <SketchSolver_Constraint.h>

/** \class   SketchSolver_ConstraintFixed
 *  \ingroup Plugins
 *  \brief   Stores data of the Fixed constraint
 */
class SketchSolver_ConstraintFixed : public SketchSolver_Constraint
{
public:
  /// Creates constraint to manage the given constraint from plugin
  SketchSolver_ConstraintFixed(ConstraintPtr theConstraint);

  /// \brief Block or unblock events from this constraint
  virtual void blockEvents(bool isBlocked);

protected:
  /// \brief Converts SketchPlugin constraint to a list of SolveSpace constraints
  virtual void process();

  /// \brief Generate list of attributes of constraint in order useful for constraints
  /// \param[out] theValue      numerical characteristic of constraint (e.g. distance)
  /// \param[out] theAttributes list of attributes to be filled
  virtual void getAttributes(EntityWrapperPtr& , std::vector<EntityWrapperPtr>& )
  {}

  /// \brief Obtain entity to be fixed
  virtual EntityWrapperPtr entityToFix();

  /// \brief Create Fixed constraint for the feature basing on its type
  /// \param theFeature [in]  feature, converted to solver specific format
  /// \return Fixed constraint
  virtual ConstraintWrapperPtr fixFeature(EntityWrapperPtr theFeature);

  /// \brief Get list of parameters of current entity
  static GCS::VEC_pD toParameters(const EntityWrapperPtr& theEntity);

protected:
  std::vector<double> myFixedValues;
};

#endif
