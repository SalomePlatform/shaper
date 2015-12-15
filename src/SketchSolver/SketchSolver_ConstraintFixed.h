// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:    SketchSolver_ConstraintFixed.h
// Created: 30 Mar 2015
// Author:  Artem ZHIDKOV

#ifndef SketchSolver_ConstraintFixed_H_
#define SketchSolver_ConstraintFixed_H_

#include "SketchSolver.h"
#include <SketchSolver_Constraint.h>

/** \class   SketchSolver_ConstraintFixed
 *  \ingroup Plugins
 *  \brief   Stores data of the Fixed constraint
 *
 *  Fixed constraint may have NULL basic SketchPlugin constraint,
 *  because the Fixed constraint may be temporary for correct moving of objects.
 *
 *  Fixed constraint does not create a constraint, but builds the entities in separate group,
 *  so they will not be moved while resolving the set of constraints.
 */
class SketchSolver_ConstraintFixed : public SketchSolver_Constraint
{
public:
  /// Creates constraint to manage the given constraint from plugin
  SketchSolver_ConstraintFixed(ConstraintPtr theConstraint);
  /// Creates temporary constraint based on feature
  SketchSolver_ConstraintFixed(FeaturePtr theFeature);

  /// \brief Tries to remove constraint
  /// \return \c false, if current constraint contains another SketchPlugin constraints (like for multiple coincidence)
  virtual bool remove();

protected:
  /// \brief Converts SketchPlugin constraint to a list of SolveSpace constraints
  virtual void process();

  /// \brief Generate list of attributes of constraint in order useful for constraints
  /// \param[out] theValue      numerical characteristic of constraint (e.g. distance)
  /// \param[out] theAttributes list of attributes to be filled
  virtual void getAttributes(ParameterWrapperPtr& theValue, std::vector<EntityWrapperPtr>& theAttributes);

  /// \brief Fixed feature basing on its type
  /// \param theFeature [in]  feature, converted to solver specific format
  virtual void fixFeature(EntityWrapperPtr theFeature);

protected:
  FeaturePtr myBaseFeature; ///< fixed feature (when it is set, myBaseConstraint should be NULL)

private:
  AttributeRefAttrPtr myFixedAttribute; ///< attribute of a fixed constraint (for correct remove)
};

#endif
