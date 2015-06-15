// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:    SketchSolver_ConstraintMovement.h
// Created: 15 Jun 2015
// Author:  Artem ZHIDKOV

#ifndef SketchSolver_ConstraintMovement_H_
#define SketchSolver_ConstraintMovement_H_

#include "SketchSolver.h"
#include <SketchSolver_ConstraintRigid.h>

/** \class   SketchSolver_ConstraintMovement
 *  \ingroup Plugins
 *  \brief   Stores data of Rigid (Fixed) constraint for the moved feature only
 */
class SketchSolver_ConstraintMovement : public SketchSolver_ConstraintRigid
{
private:
  /// Creates constraint to manage the given constraint from plugin
  SketchSolver_ConstraintMovement(ConstraintPtr theConstraint)
    : SketchSolver_ConstraintRigid(theConstraint)
  {}

public:
  /// Creates temporary constraint based on feature
  SketchSolver_ConstraintMovement(FeaturePtr theFeature);

protected:
  /// \brief Converts SketchPlugin constraint to a list of SolveSpace constraints
  virtual void process();

  /// \brief Generate list of attributes of constraint in order useful for SolveSpace constraints
  /// \param[out] theValue        numerical characteristic of constraint (e.g. distance)
  /// \param[out] theAttributes   list of attributes to be filled
  /// \param[out] theIsFullyMoved shows that the feature is moved, in other case only one point of the feature is shifted
  virtual void getAttributes(double& theValue, std::vector<Slvs_hEntity>& theAttributes, bool& theIsFullyMoved);
};

#endif
