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

protected:
  /// \brief Generate list of attributes of constraint in order useful for constraints
  /// \param[out] theValue      numerical characteristic of constraint (e.g. distance)
  /// \param[out] theAttributes list of attributes to be filled
  virtual void getAttributes(double& theValue, std::vector<EntityWrapperPtr>& theAttributes);
};

#endif
