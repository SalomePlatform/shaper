// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:    SketchSolver_ConstraintTangent.h
// Created: 1 Apr 2015
// Author:  Artem ZHIDKOV

#ifndef SketchSolver_ConstraintTangent_H_
#define SketchSolver_ConstraintTangent_H_

#include <SketchSolver_Constraint.h>

/** \class   SketchSolver_ConstraintTangent
 *  \ingroup Plugins
 *  \brief   Convert tangency constraint to SolveSpace structure
 */
class SketchSolver_ConstraintTangent : public SketchSolver_Constraint
{
public:
  /// Constructor based on SketchPlugin constraint
  SketchSolver_ConstraintTangent(ConstraintPtr theConstraint) :
      SketchSolver_Constraint(theConstraint),
      isArcArcInternal(false),
      myCurveCurveAngle(0.0)
  {}

  /// \brief Notify this object about the feature is changed somewhere
  virtual void notify(const FeaturePtr&      theFeature,
                      PlaneGCSSolver_Update* theUpdater);

protected:
  /// \brief Converts SketchPlugin constraint to a list of solver constraints
  virtual void process();

  /// \brief Remove current constraint from the storage and build is again
  void rebuild();

  /// \brief This method is used in derived objects to check consistency of constraint.
  ///        E.g. the distance between line and point may be signed.
  virtual void adjustConstraint();

private:
  bool isArcArcInternal;
  double myCurveCurveAngle;
  AttributePtr mySharedPoint;
};

#endif
