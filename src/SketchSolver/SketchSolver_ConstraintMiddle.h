// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:    SketchSolver_ConstraintMiddle.h
// Created: 03 March 2016
// Author:  Artem ZHIDKOV

#ifndef SketchSolver_ConstraintMiddle_H_
#define SketchSolver_ConstraintMiddle_H_

#include <SketchSolver_Constraint.h>

/** \class   SketchSolver_ConstraintMiddle
 *  \ingroup Plugins
 *  \brief   Converts middle-point constraint to the constraint applicable for solver
 */
class SketchSolver_ConstraintMiddle : public SketchSolver_Constraint
{
public:
  /// Constructor based on SketchPlugin constraint
  SKETCHSOLVER_EXPORT SketchSolver_ConstraintMiddle(ConstraintPtr theConstraint);

  virtual ~SketchSolver_ConstraintMiddle() {}

  /// \brief Notify constraint, that coincidence appears or removed
  virtual void notifyCoincidenceChanged(EntityWrapperPtr theCoincAttr1, EntityWrapperPtr theCoincAttr2);
};

#endif
