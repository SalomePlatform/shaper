// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:    PlaneGCSSolver_Tools.h
// Created: 01 Mar 2017
// Author:  Artem ZHIDKOV

#ifndef PlaneGCSSolver_Tools_H_
#define PlaneGCSSolver_Tools_H_

#include <SketchSolver_Constraint.h>
#include <SketchSolver_ConstraintMovement.h>
#include <SketchPlugin_Constraint.h>

#include <GeomAPI_Lin2d.h>
#include <GeomAPI_Pnt2d.h>

/** \namespace  PlaneGCSSolver_Tools
 *  \ingroup    Plugins
 *  \brief      Converter tools
 */
namespace PlaneGCSSolver_Tools
{
  /// \brief Creates a solver's constraint using given SketchPlugin constraint
  ///        or returns empty pointer if not all attributes are correct
  SolverConstraintPtr createConstraint(ConstraintPtr theConstraint);

#ifdef SUPPORT_NEW_MOVE
  /// \brief Creates temporary constraint to fix the feature after movement
  std::shared_ptr<SketchSolver_ConstraintMovement>
      createMovementConstraint(FeaturePtr theMovedFeature);
  /// \brief Creates temporary constraint to fix the attribute after movement
  std::shared_ptr<SketchSolver_ConstraintMovement>
      createMovementConstraint(AttributePtr theMovedAttribute);
#else
  /// \brief Creates temporary constraint to fix the feature after movement
  std::shared_ptr<SketchSolver_ConstraintFixed>
      createMovementConstraint(FeaturePtr theMovedFeature);
#endif

  /// \brief Creates new constraint using given parameters
  /// \param theConstraint [in]  original constraint
  /// \param theType       [in]  type of constraint
  /// \param theValue      [in]  numeric characteristic of constraint
  ///                            (e.g. distance or radius) if applicable
  /// \param theEntity1    [in]  first attribute of constraint
  /// \param theEntity2    [in]  second attribute of constraint
  /// \param theEntity3    [in]  third attribute of constraint
  /// \param theEntity4    [in]  fourth attribute of constraint
  /// \return Created wrapper of constraints applicable for specific solver.
  ConstraintWrapperPtr createConstraint(ConstraintPtr theConstraint,
                                        const SketchSolver_ConstraintType& theType,
                                        const EntityWrapperPtr& theValue,
                                        const EntityWrapperPtr& theEntity1,
                                        const EntityWrapperPtr& theEntity2 = EntityWrapperPtr(),
                                        const EntityWrapperPtr& theEntity3 = EntityWrapperPtr(),
                                        const EntityWrapperPtr& theEntity4 = EntityWrapperPtr());

  /// \brief Convert entity to point
  /// \return empty pointer if the entity is not a point
  std::shared_ptr<GeomAPI_Pnt2d> point(EntityWrapperPtr theEntity);
  /// \brief Convert entity to line
  /// \return empty pointer if the entity is not a line
  std::shared_ptr<GeomAPI_Lin2d> line(EntityWrapperPtr theEntity);

  /// \brief Convert entity to line
  /// \return empty pointer if the entity is not a line
  std::shared_ptr<GeomAPI_Lin2d> line(FeaturePtr theFeature);
};

#endif
