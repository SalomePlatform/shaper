// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:    SketchSolver_Builder.h
// Created: 25 Mar 2015
// Author:  Artem ZHIDKOV

#ifndef SketchSolver_Builder_H_
#define SketchSolver_Builder_H_

#include <SketchSolver_Constraint.h>
#include <SketchPlugin_Constraint.h>

#include <GeomAPI_Lin2d.h>
#include <GeomAPI_Pnt2d.h>

/** \class   SketchSolver_Builder
 *  \ingroup Plugins
 *  \brief   Abstract class for builders of solver's entities
 */
class SketchSolver_Builder
{
public:
  /// \brief Creates a storage specific for used solver
  virtual StoragePtr createStorage(const SolverPtr&) const = 0;
  /// \brief Creates specific solver
  virtual SolverPtr createSolver() const = 0;

  /// \brief Creates a solver's constraint using given SketchPlugin constraint
  ///        or returns empty pointer if not all attributes are correct
  SolverConstraintPtr createConstraint(ConstraintPtr theConstraint) const;

  /// \brief Creates temporary constraint to fix the feature after movement
  SolverConstraintPtr createMovementConstraint(FeaturePtr theMovedFeature) const;

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
  virtual ConstraintWrapperPtr
    createConstraint(ConstraintPtr theConstraint,
                     const SketchSolver_ConstraintType& theType,
                     const EntityWrapperPtr& theValue,
                     const EntityWrapperPtr& theEntity1,
                     const EntityWrapperPtr& theEntity2 = EntityWrapperPtr(),
                     const EntityWrapperPtr& theEntity3 = EntityWrapperPtr(),
                     const EntityWrapperPtr& theEntity4 = EntityWrapperPtr()) const = 0;

  /// \brief Creates new multi-translation or multi-rotation constraint
  /// \param theConstraint [in]  original constraint
  /// \param theType       [in]  type of constraint
  /// \param theValue      [in]  numeric characteristic of constraint (angle for multi-rotation)
  ///                            if applicable
  /// \param theFullValue  [in]  indicates theValue shows full translation delta/rotation angle or
  ///                            delta/angle between neighbor entities
  /// \param thePoint1     [in]  center for multi-rotation or start point for multi-translation
  /// \param thePoint2     [in]  end point for multi-translation (empty for multi-rotation)
  /// \param theTrsfEnt    [in]  list of transformed entities
  virtual ConstraintWrapperPtr
    createConstraint(ConstraintPtr theConstraint,
                     const SketchSolver_ConstraintType& theType,
                     const EntityWrapperPtr& theValue,
                     const bool theFullValue,
                     const EntityWrapperPtr& thePoint1,
                     const EntityWrapperPtr& thePoint2,
                     const std::list<EntityWrapperPtr>& theTrsfEnt) const = 0;


  /// \brief Convert entity to point
  /// \return empty pointer if the entity is not a point
  virtual std::shared_ptr<GeomAPI_Pnt2d> point(EntityWrapperPtr theEntity) const = 0;
  /// \brief Convert entity to line
  /// \return empty pointer if the entity is not a line
  virtual std::shared_ptr<GeomAPI_Lin2d> line(EntityWrapperPtr theEntity) const = 0;

  /// \brief Convert entity to line
  /// \return empty pointer if the entity is not a line
  virtual std::shared_ptr<GeomAPI_Lin2d> line(FeaturePtr theFeature) const = 0;

  /// \brief Check if two connected arcs have centers
  ///        in same direction relatively to connection point
  virtual bool isArcArcTangencyInternal(EntityWrapperPtr theArc1,
                                        EntityWrapperPtr theArc2) const = 0;
};

typedef std::shared_ptr<SketchSolver_Builder> BuilderPtr;

#endif
