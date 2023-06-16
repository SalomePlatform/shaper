// Copyright (C) 2014-2023  CEA/DEN, EDF R&D
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

#ifndef SolveSpaceSolver_Builder_H_
#define SolveSpaceSolver_Builder_H_

#include <SketchSolver_Builder.h>
#include <SketchSolver_Constraint.h>

#include <SketchPlugin_Constraint.h>

#include <ModelAPI_CompositeFeature.h>

/** \class   SolveSpaceSolver_Builder
 *  \ingroup Plugins
 *  \brief   Create bridges between SketchPlugin constraints and SolveSpace constraints
 */
class SolveSpaceSolver_Builder : public SketchSolver_Builder
{
private:
  /// Default constructor
  SolveSpaceSolver_Builder() {}

public:
  /// \brief Returns single instance of builder
  static BuilderPtr getInstance();

  /// \brief Creates a storage specific for used solver
  virtual StoragePtr createStorage(const GroupID& theGroup) const;
  /// \brief Creates specific solver
  virtual SolverPtr createSolver() const;

  /// \brief Creates new constraint(s) using given parameters
  /// \param theConstraint [in]  original constraint
  /// \param theGroupID    [in]  group the constraint belongs to
  /// \param theSketchID   [in]  sketch the constraint belongs to
  /// \param theType       [in]  type of constraint
  /// \param theValue      [in]  numeric characteristic of constraint
  ///                            (e.g. distance or radius) if applicable
  /// \param theEntity1    [in]  first attribute of constraint
  /// \param theEntity2    [in]  second attribute of constraint
  /// \param theEntity3    [in]  third attribute of constraint
  /// \param theEntity4    [in]  fourth attribute of constraint
  /// \return Created list of wrappers of constraints applicable for specific solver.
  ///         Most of constraint types lead to single constraint, but there are some kind of
  ///         constraints (e.g. mirror), which may produce couple of constraints.
  virtual std::list<ConstraintWrapperPtr>
    createConstraint(ConstraintPtr theConstraint,
                     const GroupID& theGroupID,
                     const EntityID& theSketchID,
                     const SketchSolver_ConstraintType& theType,
                     const double& theValue,
                     const EntityWrapperPtr& theEntity1,
                     const EntityWrapperPtr& theEntity2 = EntityWrapperPtr(),
                     const EntityWrapperPtr& theEntity3 = EntityWrapperPtr(),
                     const EntityWrapperPtr& theEntity4 = EntityWrapperPtr()) const;

  /// \brief Creates new multi-translation or multi-rotation constraint
  /// \param theConstraint [in]  original constraint
  /// \param theGroupID    [in]  group the constraint belongs to
  /// \param theSketchID   [in]  sketch the constraint belongs to
  /// \param theType       [in]  type of constraint
  /// \param theValue      [in]  numeric characteristic of constraint
  ///                            (angle for multi-rotation) if applicable
  /// \param theFullValue  [in]  indicates theValue shows full translation
  ///                            delta/rotation angle or delta/angle between neighbor entities
  /// \param thePoint1     [in]  center for multi-rotation or start point for multi-translation
  /// \param thePoint2     [in]  end point for multi-translation (empty for multi-rotation)
  /// \param theTrsfEnt    [in]  list of transformed entities
  virtual std::list<ConstraintWrapperPtr>
    createConstraint(ConstraintPtr theConstraint,
                     const GroupID& theGroupID,
                     const EntityID& theSketchID,
                     const SketchSolver_ConstraintType& theType,
                     const double& theValue,
                     const bool theFullValue,
                     const EntityWrapperPtr& thePoint1,
                     const EntityWrapperPtr& thePoint2,
                     const std::list<EntityWrapperPtr>& theTrsfEnt) const;

  /// \brief Update flags for several kinds of constraints
  virtual void adjustConstraint(ConstraintWrapperPtr theConstraint) const;

  /// \brief Creates a feature using list of already created attributes
  /// \param theFeature    [in]  feature to create
  /// \param theAttributes [in]  attributes of the feature
  /// \param theGroupID    [in]  group the feature belongs to
  /// \param theSketchID   [in]  sketch the feature belongs to
  /// \return Created wrapper of the feature applicable for specific solver
  virtual EntityWrapperPtr createFeature(FeaturePtr theFeature,
                                         const std::list<EntityWrapperPtr>& theAttributes,
                                         const GroupID& theGroupID,
                                         const EntityID& theSketchID = EID_UNKNOWN) const;

  /// \brief Creates an attribute
  /// \param theAttribute [in]  attribute to create
  /// \param theGroup     [in]  group the attribute belongs to
  /// \param theSketchID  [in]  sketch the attribute belongs to
  /// \return Created wrapper of the attribute applicable for specific solver
  virtual EntityWrapperPtr createAttribute(AttributePtr theAttribute,
                                           const GroupID& theGroup,
                                           const EntityID& theSketchID = EID_UNKNOWN) const;

private:
  /// \brief Create necessary constraints to make two object symmetric relatively a given line
  std::list<ConstraintWrapperPtr> createMirror(ConstraintPtr theConstraint,
                                               const GroupID& theGroupID,
                                               const EntityID& theSketchID,
                                               const EntityWrapperPtr& theEntity1,
                                               const EntityWrapperPtr& theEntity2,
                                               const EntityWrapperPtr& theMirrorLine) const;

  /// \brief Converts sketch parameters to the entity applicable for the solver.
  /// \param theSketch  [in]  the element to be converted
  /// \param theGroupID [in]  group where the sketch should be created
  /// \return Entity respective the sketch or empty pointer, it the sketch has incorrect attributes
  EntityWrapperPtr createSketchEntity(CompositeFeaturePtr theSketch,
                                      const GroupID& theGroupID) const;

  /// \brief Converts two axes of sketch's trihedron to the normal entity
  /// \param theNormal  [in]  direction of the normal of the sketch
  /// \param theDirX    [in]  direction of the X axis of the sketch
  /// \param theGroupID [in]  group, the normal belongs to
  /// \return Created entity or empty pointer, if there are incorrect attributes
  EntityWrapperPtr createNormal(AttributePtr theNormal,
                                AttributePtr theDirX,
                                const GroupID& theGroupID) const;

  /// \brief Converts a value to SolveSpace parameter
  /// \param theGroup [in]  group to store parameter
  /// \param theValue [in]  value of parameter
  /// \param theExpr  [in]  shows the parameter is given by expression
  /// \return Created parameter's wrapper
  ParameterWrapperPtr createParameter(const GroupID& theGroup,
                                      const double theValue = 0.0,
                                      const bool theExpr = false) const;

private:
  static BuilderPtr mySelf;
};

#endif
