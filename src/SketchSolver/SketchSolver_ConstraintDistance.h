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
  /// Constructor based on SketchPlugin constraint
  SketchSolver_ConstraintDistance(ConstraintPtr theConstraint) :
      SketchSolver_Constraint(theConstraint),
      myType(SLVS_C_UNKNOWN),
      myIsNegative(false)
  {}

  /// \brief Update constraint
  virtual void update(ConstraintPtr theConstraint = ConstraintPtr());

  virtual int getType() const
  {return myType; }

protected:
  /// \brief Converts SketchPlugin constraint to a list of SolveSpace constraints
  virtual void process();

  /// \brief This method is used in derived objects to check consistence of constraint.
  ///        E.g. the distance between line and point may be signed.
  virtual void adjustConstraint();

private:
  int myType; ///< type of constraint (applicable: SLVS_C_PT_PT_DISTANCE, SLVS_C_PT_LINE_DISTANCE)
  double myPrevValue; ///< previous value of distance (for correct calculation of a distance sign)
  bool myIsNegative;  ///< \c true, if the point if placed rightside of line direction (SLVS_C_PT_LINE_DISTANCE only)
};

#endif
