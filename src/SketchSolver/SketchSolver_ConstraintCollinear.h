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
  SketchSolver_ConstraintCollinear(ConstraintPtr theConstraint)
    : SketchSolver_Constraint(theConstraint)
  {
    for (int i = 0; i < 4; ++i)
      myIsConstraintApplied[i] = false;
  }

  /// \brief Notify this object about the feature is changed somewhere
  virtual void notify(const FeaturePtr&      theFeature,
                      PlaneGCSSolver_Update* theUpdater);

protected:
  /// \brief Converts SketchPlugin constraint to a list of solver constraints
  virtual void process();

private:
  EntityWrapperPtr myPoints[4];  ///< extremities on collinear lines
  bool myIsConstraintApplied[4]; ///< set \c true if point on opposite line
};

#endif
