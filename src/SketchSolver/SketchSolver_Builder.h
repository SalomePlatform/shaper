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
  virtual StoragePtr createStorage(const GroupID& theGroup) const = 0;
  /// \brief Creates specific solver
  virtual SolverPtr createSolver() const = 0;

  /// \brief Creates a solver's constraint using given SketchPlugin constraint
  ///        or returns empty pointer if not all attributes are correct
  SolverConstraintPtr createConstraint(ConstraintPtr theConstraint) const;

  /// \brief Creates temporary constraint to fix the placement of the feature
  SolverConstraintPtr createFixedConstraint(FeaturePtr theFixedFeature) const;

  /// \brief Creates temporary constraint to fix the feature after movement
  SolverConstraintPtr createMovementConstraint(FeaturePtr theFixedFeature) const;

  /// \brief Creates new constraint(s) using given parameters
  /// \param theConstraint [in]  original constraint
  /// \param theGroupID    [in]  group the constraint belongs to
  /// \param theSketchID   [in]  sketch the constraint belongs to
  /// \param theType       [in]  type of constraint
  /// \param theValue      [in]  numeric characteristic of constraint (e.g. distance or radius) if applicable
  /// \param theEntity1    [in]  first attribute of constraint
  /// \param theEntity2    [in]  second attribute of constraint
  /// \param theEntity3    [in]  third attribute of constraint
  /// \param theEntity4    [in]  fourth attribute of constraint
  /// \return Created list of wrappers of constraints applicable for specific solver.
  ///         Most of constraint types lead to single constraint, but there are some kind of
  ///         constraints (e.g. mirror), which may produce couple of constraints.
  virtual std::list<ConstraintWrapperPtr>
    createConstraint(ConstraintPtr theConstraint,
                     const GroupID& theGroup,
                     const EntityID& theSketchID,
                     const SketchSolver_ConstraintType& theType,
                     const double& theValue,
                     const EntityWrapperPtr& theEntity1,
                     const EntityWrapperPtr& theEntity2 = EntityWrapperPtr(),
                     const EntityWrapperPtr& theEntity3 = EntityWrapperPtr(),
                     const EntityWrapperPtr& theEntity4 = EntityWrapperPtr()) const = 0;

  /// \brief Creates new multi-translation or multi-rotation constraint
  /// \param theConstraint [in]  original constraint
  /// \param theGroupID    [in]  group the constraint belongs to
  /// \param theSketchID   [in]  sketch the constraint belongs to
  /// \param theType       [in]  type of constraint
  /// \param theValue      [in]  numeric characteristic of constraint (angle for multi-rotation) if applicable
  /// \param thePoint1     [in]  center for multi-rotation or start point for multi-translation
  /// \param thePoint2     [in]  end point for multi-translation (empty for multi-rotation)
  /// \param theTrsfEnt    [in]  list of transformed entities
  virtual std::list<ConstraintWrapperPtr>
    createConstraint(ConstraintPtr theConstraint,
                     const GroupID& theGroup,
                     const EntityID& theSketchID,
                     const SketchSolver_ConstraintType& theType,
                     const double& theValue,
                     const bool& theFullValue,
                     const EntityWrapperPtr& thePoint1,
                     const EntityWrapperPtr& thePoint2,
                     const std::list<EntityWrapperPtr>& theTrsfEnt) const = 0;

  /// \brief Update flags for several kinds of constraints
  virtual void adjustConstraint(ConstraintWrapperPtr theConstraint) const = 0;

  /// \brief Creates a feature using list of already created attributes
  /// \param theFeature    [in]  feature to create
  /// \param theAttributes [in]  attributes of the feature
  /// \param theGroupID    [in]  group the feature belongs to
  /// \param theSketchID   [in]  sketch the feature belongs to
  /// \return Created wrapper of the feature applicable for specific solver
  virtual EntityWrapperPtr createFeature(FeaturePtr theFeature,
                                         const std::list<EntityWrapperPtr>& theAttributes,
                                         const GroupID& theGroupID,
                                         const EntityID& theSketchID = EID_UNKNOWN) const = 0;

  /// \brief Creates an attribute
  /// \param theAttribute [in]  attribute to create
  /// \param theGroup     [in]  group the attribute belongs to
  /// \param theSketchID  [in]  sketch the attribute belongs to
  /// \return Created wrapper of the attribute applicable for specific solver
  virtual EntityWrapperPtr createAttribute(AttributePtr theAttribute,
                                           const GroupID& theGroup,
                                           const EntityID& theSketchID = EID_UNKNOWN) const = 0;


  /// \brief Convert entity to point
  /// \return empty pointer if the entity is not a point
  SKETCHSOLVER_EXPORT std::shared_ptr<GeomAPI_Pnt2d> point(EntityWrapperPtr theEntity) const;
  /// \brief Convert entity to line
  /// \return empty pointer if the entity is not a line
  SKETCHSOLVER_EXPORT std::shared_ptr<GeomAPI_Lin2d> line(EntityWrapperPtr theEntity) const;
};

typedef std::shared_ptr<SketchSolver_Builder> BuilderPtr;

#endif
