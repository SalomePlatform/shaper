// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:    SketchSolver_ConstraintLength.h
// Created: 31 Mar 2015
// Author:  Artem ZHIDKOV

#ifndef SketchSolver_ConstraintLength_H_
#define SketchSolver_ConstraintLength_H_

#include "SketchSolver.h"
#include <SketchSolver_Constraint.h>

/** \class   SketchSolver_ConstraintLength
 *  \ingroup Plugins
 *  \brief   Convert length constraint to SolveSpace structure
 */
class SketchSolver_ConstraintLength : public SketchSolver_Constraint
{
public:
  SketchSolver_ConstraintLength(ConstraintPtr theConstraint) :
      SketchSolver_Constraint(theConstraint)
  {}

  virtual int getType() const
  { return SLVS_C_PT_PT_DISTANCE; }

protected:
  /// \brief Converts SketchPlugin constraint to a list of SolveSpace constraints
  virtual void process();
};

#endif
