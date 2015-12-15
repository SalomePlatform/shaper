// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:    SketchSolver_ConstraintTangent.h
// Created: 1 Apr 2015
// Author:  Artem ZHIDKOV

#ifndef SketchSolver_ConstraintTangent_H_
#define SketchSolver_ConstraintTangent_H_

#include "SketchSolver.h"
#include <SketchSolver_Constraint.h>

/** \class   SketchSolver_ConstraintTangent
 *  \ingroup Plugins
 *  \brief   Convert tangency constraint to SolveSpace structure
 */
class SketchSolver_ConstraintTangent : public SketchSolver_Constraint
{
public:
  /// Constructor based on SketchPlugin constraint
  SketchSolver_ConstraintTangent(ConstraintPtr theConstraint) :
      SketchSolver_Constraint(theConstraint)
  {}

  virtual int getType() const
  { return myType; }

protected:
  /// \brief Converts SketchPlugin constraint to a list of SolveSpace constraints
  virtual void process();

private:
  int myType; ///< type of constraint (applicable: SLVS_C_ARC_LINE_TANGENT, SLVS_C_CURVE_CURVE_TANGENT)
};

#endif
