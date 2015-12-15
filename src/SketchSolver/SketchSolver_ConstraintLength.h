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
  /// Constructor based on SketchPlugin constraint
  SketchSolver_ConstraintLength(ConstraintPtr theConstraint) :
      SketchSolver_Constraint(theConstraint)
  {}

protected:
  /// \brief Generate list of attributes of constraint in order useful for constraints
  /// \param[out] theValue      numerical characteristic of constraint (e.g. distance)
  /// \param[out] theAttributes list of attributes to be filled
  virtual void getAttributes(double& theValue, std::vector<EntityWrapperPtr>& theAttributes);
};

#endif
