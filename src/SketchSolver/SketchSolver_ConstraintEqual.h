// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:    SketchSolver_ConstraintEqual.h
// Created: 1 Apr 2015
// Author:  Artem ZHIDKOV

#ifndef SketchSolver_ConstraintEqual_H_
#define SketchSolver_ConstraintEqual_H_

#include "SketchSolver.h"
#include <SketchSolver_Constraint.h>

/** \class   SketchSolver_ConstraintEqual
 *  \ingroup Plugins
 *  \brief   Convert equality constraint to SolveSpace structure
 */
class SketchSolver_ConstraintEqual : public SketchSolver_Constraint
{
public:
  /// Constructor based on SketchPlugin constraint
  SketchSolver_ConstraintEqual(ConstraintPtr theConstraint) :
      SketchSolver_Constraint(theConstraint)
  {}

  virtual int getType() const
  { return myType; }

protected:
  /// \brief Converts SketchPlugin constraint to a list of SolveSpace constraints
  virtual void process();

private:
  int myType; ///< type of constraint (applicable: SLVS_C_EQUAL_LENGTH_LINES, SLVS_C_EQUAL_RADIUS, SLVS_C_EQUAL_LINE_ARC_LEN)
};

#endif
