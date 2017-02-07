// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:    SketchSolver_ConstraintMirror.h
// Created: 1 Apr 2015
// Author:  Artem ZHIDKOV

#ifndef SketchSolver_ConstraintMirror_H_
#define SketchSolver_ConstraintMirror_H_

#include "SketchSolver.h"
#include <SketchSolver_Constraint.h>

/** \class   SketchSolver_ConstraintMirror
 *  \ingroup Plugins
 *  \brief   Convert fillet constraint to SolveSpace structure
 */
class SketchSolver_ConstraintMirror : public SketchSolver_Constraint
{
public:
  /// Constructor based on SketchPlugin constraint
  SketchSolver_ConstraintMirror(ConstraintPtr theConstraint) :
      SketchSolver_Constraint(theConstraint),
      myNumberOfObjects(0)
  {}

  /// \brief Update constraint
  virtual void update() override;

  /// \brief Notify this object about the feature is changed somewhere
  virtual void notify(const FeaturePtr& theFeature, PlaneGCSSolver_Update*) override;

  /// \brief Block or unblock events from this constraint
  virtual void blockEvents(bool isBlocked) override;

protected:
  /// \brief Converts SketchPlugin constraint to a list of SolveSpace constraints
  virtual void process() override;

  /// \brief Generate list of entities of mirror constraint
  virtual void getAttributes(EntityWrapperPtr&, std::vector<EntityWrapperPtr>&) override;

  /// \brief This method is used in derived objects to check consistence of constraint.
  ///        E.g. the distance between line and point may be signed.
  virtual void adjustConstraint() override;

private:
  size_t myNumberOfObjects;  ///< number of previously mirrored objects
  std::set<FeaturePtr> myFeatures; ///< mirror line, original and mirrored features
};

#endif
