// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:    SketchSolver_ConstraintFixedArcRadiusArcRadius.h
// Created: 22 Jan 2016
// Author:  Artem ZHIDKOV

#ifndef SketchSolver_ConstraintFixedArcRadius_H_
#define SketchSolver_ConstraintFixedArcRadius_H_

#include "SketchSolver.h"
#include <SketchSolver_ConstraintFixed.h>

/** \class   SketchSolver_ConstraintFixedArcRadius
 *  \ingroup Plugins
 *  \brief   Fix radius of the arc. Temporary constraint. Based on an arc, sharp!
 */
class SketchSolver_ConstraintFixedArcRadius : public SketchSolver_ConstraintFixed
{
public:
  /// Creates temporary constraint based on feature
  SketchSolver_ConstraintFixedArcRadius(FeaturePtr theFeature);

  /// \brief Tries to remove constraint
  /// \return \c false, if current constraint contains another SketchPlugin constraints (like for multiple coincidence)
  virtual bool remove();

protected:
  /// \brief Converts SketchPlugin constraint to a list of SolveSpace constraints
  virtual void process();

  /// \brief Fix radius of arc
  /// \param theFeature [in]  feature, converted to solver specific format
  virtual void fixFeature(EntityWrapperPtr theFeature);

private:
  ConstraintWrapperPtr myRadiusConstraint;
};

#endif
