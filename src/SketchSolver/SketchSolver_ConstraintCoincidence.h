// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:    SketchSolver_ConstraintCoincidence.h
// Created: 29 Mar 2015
// Author:  Artem ZHIDKOV

#ifndef SketchSolver_ConstraintCoincidence_H_
#define SketchSolver_ConstraintCoincidence_H_

#include <SketchSolver_Constraint.h>

/** \class   SketchSolver_ConstraintCoincidence
 *  \ingroup Plugins
 *  \brief   Convert coincidence constraint to SolveSpace structure
 */
class SketchSolver_ConstraintCoincidence : public SketchSolver_Constraint
{
public:
  /// Constructor based on SketchPlugin constraint
  SketchSolver_ConstraintCoincidence(ConstraintPtr theConstraint) :
      SketchSolver_Constraint(theConstraint),
      myInSolver(false)
  {}

  /// \brief Notify this object about the feature is changed somewhere
  virtual void notify(const FeaturePtr&      theFeature,
                      PlaneGCSSolver_Update* theUpdater);

  /// \brief Remove constraint
  virtual bool remove();

protected:
  /// \brief Converts SketchPlugin constraint to a list of solver constraints
  virtual void process();

  /// \brief Generate list of attributes of constraint in order useful for constraints
  /// \param[out] theValue      numerical characteristic of constraint (e.g. distance)
  /// \param[out] theAttributes list of attributes to be filled
  virtual void getAttributes(EntityWrapperPtr&              theValue,
                             std::vector<EntityWrapperPtr>& theAttributes);

protected:
  bool myInSolver; ///< shows the constraint is added to the solver
};

#endif
