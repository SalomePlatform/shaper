// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:    SketchSolver_ConstraintMiddle.h
// Created: 03 March 2016
// Author:  Artem ZHIDKOV

#ifndef SketchSolver_ConstraintMiddle_H_
#define SketchSolver_ConstraintMiddle_H_

#include <SketchSolver_ConstraintCoincidence.h>

/** \class   SketchSolver_ConstraintMiddle
 *  \ingroup Plugins
 *  \brief   Converts middle-point constraint to the constraint applicable for solver
 */
class SketchSolver_ConstraintMiddle : public SketchSolver_ConstraintCoincidence
{
public:
  /// Constructor based on SketchPlugin constraint
  SketchSolver_ConstraintMiddle(ConstraintPtr theConstraint)
    : SketchSolver_ConstraintCoincidence(theConstraint)
  {}

  /// \brief Notify this object about the feature is changed somewhere
  virtual void notify(const FeaturePtr&      theFeature,
                      PlaneGCSSolver_Update* theUpdater) override;

protected:
  /// \brief Generate list of attributes of constraint in order useful for constraints
  /// \param[out] theValue      numerical characteristic of constraint (e.g. distance)
  /// \param[out] theAttributes list of attributes to be filled
  virtual void getAttributes(EntityWrapperPtr&              theValue,
                             std::vector<EntityWrapperPtr>& theAttributes) override;

private:
  ConstraintWrapperPtr myMiddle;
};

#endif
