// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:    SketchSolver_ConstraintFillet.h
// Created: 1 Apr 2015
// Author:  Artem ZHIDKOV

#ifndef SketchSolver_ConstraintFillet_H_
#define SketchSolver_ConstraintFillet_H_

#include "SketchSolver.h"
#include <SketchSolver_Constraint.h>

/** \class   SketchSolver_ConstraintFillet
 *  \ingroup Plugins
 *  \brief   Convert fillet constraint to SolveSpace structure
 */
class SketchSolver_ConstraintFillet : public SketchSolver_Constraint
{
public:
  SketchSolver_ConstraintFillet(ConstraintPtr theConstraint) :
      SketchSolver_Constraint(theConstraint)
  {}

  virtual int getType() const
  { return SLVS_C_FILLET; }

  /// \brief Tries to remove constraint
  /// \return \c false, if current constraint contains another SketchPlugin constraints (like for multiple coincidence)
  virtual bool remove(ConstraintPtr theConstraint = ConstraintPtr());

protected:
  /// \brief Converts SketchPlugin constraint to a list of SolveSpace constraints
  virtual void process();

  /// \brief Generate list of attributes of constraint in order useful for SolveSpace constraints
  /// \param[out] theValue      numerical characteristic of constraint (e.g. distance)
  /// \param[out] theAttributes list of attributes to be filled
  ///
  /// Parameter theAttributes contains 5 elements:
  /// Two first entities in theAttributes list correspond to base features of fillet,
  /// the next two entities represent trimmed base features and the last one is a fillet arc.
  virtual void getAttributes(double& theValue, std::vector<Slvs_hEntity>& theAttributes);
};

#endif
