// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:    SketchSolver_ConstraintMovement.h
// Created: 15 Jun 2015
// Author:  Artem ZHIDKOV

#ifndef SketchSolver_ConstraintMovement_H_
#define SketchSolver_ConstraintMovement_H_

#include "SketchSolver.h"
#include <SketchSolver_ConstraintFixed.h>

/** \class   SketchSolver_ConstraintMovement
 *  \ingroup Plugins
 *  \brief   Stores data to the Fixed constraint for the moved feature only
 */
class SketchSolver_ConstraintMovement : public SketchSolver_ConstraintFixed
{
private:
  /// Creates constraint to manage the given constraint from plugin
  SketchSolver_ConstraintMovement(ConstraintPtr theConstraint)
    : SketchSolver_ConstraintFixed(theConstraint)
  {}

public:
  /// Creates temporary constraint based on feature
  SketchSolver_ConstraintMovement(FeaturePtr theFeature)
    : SketchSolver_ConstraintFixed(theFeature)
  {}

protected:
  /// \brief Converts SketchPlugin constraint to a list of SolveSpace constraints
  virtual void process();

  /// \brief Generate list of attributes of constraint in order useful for SolveSpace constraints
  /// \param[out] theValue        numerical characteristic of constraint (e.g. distance)
  /// \param[out] theAttributes   list of attributes to be filled (list of moved entities or attributes)
  virtual void getAttributes(ParameterWrapperPtr& theValue, std::vector<EntityWrapperPtr>& theAttributes);

private:
  std::vector<EntityWrapperPtr> myMovedEntities; ///< list of entities that are moved
};

#endif
