// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:    SketchSolver_Builder.h
// Created: 25 Mar 2015
// Author:  Artem ZHIDKOV

#ifndef SketchSolver_Builder_H_
#define SketchSolver_Builder_H_

#include "SketchSolver.h"
#include <SketchSolver_Constraint.h>

#include <SketchPlugin_Constraint.h>

#include <ModelAPI_CompositeFeature.h>

/** \class   SketchSolver_Builder
 *  \ingroup Plugins
 *  \brief   Create bridges between SketchPlugin constraints and SolveSpace constraints
 */
class SketchSolver_Builder
{
private:
  /// Default constructor
  SketchSolver_Builder() {}

public:
  /// \brief Returns single instance of builder
  static SketchSolver_Builder* getInstance();

  /// \brief Creates a solver's constraint using given SketchPlugin constraint
  ///        or returns empty pointer if not all attributes are correct
  SolverConstraintPtr createConstraint(ConstraintPtr theConstraint);

  /// \brief Creates temporary constraint to fix the placement of the feature
  SolverConstraintPtr createRigidConstraint(FeaturePtr theFixedFeature);

  /// \brief Creates temporary constraint to fix the feature after movement
  SolverConstraintPtr createMovementConstraint(FeaturePtr theFixedFeature);

  /// \brief Creates constraint for parametrically given attribute
  SolverConstraintPtr createParametricConstraint(AttributePtr theAttribute);

  /// \brief Converts sketch parameters to the list of SolveSpace entities.
  ///        Identifiers of entities and parameters are local. They should be changed while adding into storage.
  ///        The sketch entity goes last.
  /// \param[in]  theSketch     the element to be converted
  /// \param[out] theEntities   created list of entities
  /// \param[out] theParameters created list of parameters of the entities
  /// \return \c true if workplane created
  bool createWorkplane(CompositeFeaturePtr theSketch,
                       std::vector<Slvs_Entity>& theEntities,
                       std::vector<Slvs_Param>& theParameters);

  /// \brief Converts attribute to the list of entities.
  ///        Identifiers are local (see createWorkplane).
  ///        The main entity goes last.
  bool createEntity(AttributePtr theAttribute,
                    std::vector<Slvs_Entity>& theEntities,
                    std::vector<Slvs_Param>& theParameters);
  /// \brief Converts feature to the list of entities.
  ///        Identifiers are local (see createWorkplane).
  ///        The main entity goes last.
  bool createEntity(FeaturePtr theFeature,
                    std::vector<Slvs_Entity>& theEntities,
                    std::vector<Slvs_Param>& theParameters);

  /// \brief Converts normal and OX direction to the list of entities representing a normal in SolveSpace.
  ///        Identifiers are local (see createWorkplane).
  ///        The main entity goes last.
  bool createNormal(AttributePtr theNormal,
                    AttributePtr theDirX,
                    std::vector<Slvs_Entity>& theEntities,
                    std::vector<Slvs_Param>& theParameters);

private:
  static SketchSolver_Builder* mySelf;
};

#endif
