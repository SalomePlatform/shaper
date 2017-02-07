// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:    PlaneGCSSolver_Builder.h
// Created: 14 Dec 2015
// Author:  Artem ZHIDKOV

#ifndef PlaneGCSSolver_Builder_H_
#define PlaneGCSSolver_Builder_H_

#include <PlaneGCSSolver_Defs.h>

#include <SketchSolver_Builder.h>
#include <SketchSolver_Constraint.h>

#include <SketchPlugin_Constraint.h>

#include <ModelAPI_CompositeFeature.h>

/** \class   PlaneGCSSolver_Builder
 *  \ingroup Plugins
 *  \brief   Create bridges between SketchPlugin constraints and PlaneGCS constraints
 */
class PlaneGCSSolver_Builder : public SketchSolver_Builder
{
  /// Default constructor
  PlaneGCSSolver_Builder() {}

public:
  /// \brief Returns single instance of builder
  static BuilderPtr getInstance();

  /// \brief Creates a storage specific for used solver
  virtual StoragePtr createStorage(const SolverPtr& theSolver) const override;
  /// \brief Creates specific solver
  virtual SolverPtr createSolver() const override;

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
                     const EntityWrapperPtr& theEntity4 = EntityWrapperPtr()) const override;

  /// \brief Creates new multi-translation or multi-rotation constraint
  /// \param theConstraint [in]  original constraint
  /// \param theType       [in]  type of constraint
  /// \param theValue      [in]  numeric characteristic of constraint (angle for multi-rotation)
  ///                            if applicable
  /// \param theFullValue  [in]  indicates theValue shows full translation delta/rotation
  ///                            angle or delta/angle between neighbor entities
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
                     const std::list<EntityWrapperPtr>& theTrsfEnt) const override;

  /// \brief Convert entity to point
  /// \return empty pointer if the entity is not a point
  virtual std::shared_ptr<GeomAPI_Pnt2d> point(EntityWrapperPtr theEntity) const override;
  /// \brief Convert entity to line
  /// \return empty pointer if the entity is not a line
  virtual std::shared_ptr<GeomAPI_Lin2d> line(EntityWrapperPtr theEntity) const override;

  /// \brief Convert entity to line
  /// \return empty pointer if the entity is not a line
  virtual std::shared_ptr<GeomAPI_Lin2d> line(FeaturePtr theFeature) const override;

  /// \brief Check if two connected arcs have centers
  ///        in same direction relatively to connection point
  virtual bool isArcArcTangencyInternal(EntityWrapperPtr theArc1,
                                        EntityWrapperPtr theArc2) const override;
};

#endif
