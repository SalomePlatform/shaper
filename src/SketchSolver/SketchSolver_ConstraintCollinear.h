// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:    SketchSolver_ConstraintCollinear.h
// Created: 27 May 2014
// Author:  Artem ZHIDKOV

#ifndef SketchSolver_ConstraintCollinear_H_
#define SketchSolver_ConstraintCollinear_H_

#include <SketchSolver_Constraint.h>

/** \class   SketchSolver_ConstraintCollinear
 *  \ingroup Plugins
 *  \brief   Converts collinear constraint to the constraint applicable for solver
 */
class SketchSolver_ConstraintCollinear : public SketchSolver_Constraint
{
public:
  /// Constructor based on SketchPlugin constraint
  SKETCHSOLVER_EXPORT SketchSolver_ConstraintCollinear(ConstraintPtr theConstraint);

  virtual ~SketchSolver_ConstraintCollinear() {}

  /// \brief Notify constraint, that coincidence appears or removed
  virtual void notifyCoincidenceChanged(EntityWrapperPtr theCoincAttr1, EntityWrapperPtr theCoincAttr2);
};

#endif
