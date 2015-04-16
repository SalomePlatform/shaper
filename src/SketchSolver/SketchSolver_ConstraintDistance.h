// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:    SketchSolver_ConstraintDistance.h
// Created: 31 Mar 2015
// Author:  Artem ZHIDKOV

#ifndef SketchSolver_ConstraintDistance_H_
#define SketchSolver_ConstraintDistance_H_

#include "SketchSolver.h"
#include <SketchSolver_Constraint.h>

/** \class   SketchSolver_ConstraintDistance
 *  \ingroup Plugins
 *  \brief   Convert distance constraint to SolveSpace structure
 */
class SketchSolver_ConstraintDistance : public SketchSolver_Constraint
{
public:
  SketchSolver_ConstraintDistance(ConstraintPtr theConstraint) :
      SketchSolver_Constraint(theConstraint), myType(SLVS_C_UNKNOWN)
  {}

  virtual int getType() const
  {return myType; }

protected:
  /// \brief Converts SketchPlugin constraint to a list of SolveSpace constraints
  virtual void process();

private:
  int myType; ///< type of constraint (applicable: SLVS_C_PT_PT_DISTANCE, SLVS_C_PT_LINE_DISTANCE)
};

#endif
