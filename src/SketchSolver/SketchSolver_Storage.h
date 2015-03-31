// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:    SketchSolver_Storage.h
// Created: 18 Mar 2015
// Author:  Artem ZHIDKOV

#ifndef SketchSolver_Storage_H_
#define SketchSolver_Storage_H_

#include "SketchSolver.h"
#include <SketchSolver_Solver.h>

#include <memory>
#include <set>
#include <vector>

/** \class   SketchSolver_Storage
 *  \ingroup Plugins
 *  \brief   Contains all necessary data in SolveSpace format to solve a single group of constraints
 */
class SketchSolver_Storage
{
public:
  SketchSolver_Storage();

  /** \brief Add new parameter to the current group
   *  \param[in] theParam  SolveSpace parameter
   *  \return the ID of added parameter
   */
  Slvs_hParam addParameter(const Slvs_Param& theParam);
  /** \brief Updates parameter in the current group. If the ID of parameter is zero, the new item will be added
   *  \param[in] theParam  SolveSpace parameter
   *  \return the ID of updated/added parameter
   */
  Slvs_hParam updateParameter(const Slvs_Param& theParam);
  /** \brief Removes the parameter by its ID
   *  \param[in] theParamID  index of parameter to be removed
   *  \return \c true if the parameter was successfully removed
   */
  bool removeParameter(const Slvs_hParam& theParamID);
  /// \brief Returns the parameter by its ID
  const Slvs_Param& getParameter(const Slvs_hParam& theParamID) const;

  /** \brief Add new entity to the current group
   *  \param[in] theEntity  SolveSpace entity
   *  \return the ID of added entity
   */
  Slvs_hEntity addEntity(const Slvs_Entity& theEntity);
  /** \brief Updates entity in the current group. If the ID of entity is zero, the new item will be added
   *  \param[in] theEntity  SolveSpace entity
   *  \return the ID of updated/added entity
   */
  Slvs_hEntity updateEntity(const Slvs_Entity& theEntity);
  /** \brief Removes the entity by its ID. All parameters used in this entity,
   *         and not used in other constraints, will be removed too.
   *  \param[in] theEntityID  index of entity to be removed
   *  \return \c true if the entity was successfully removed
   */
  bool removeEntity(const Slvs_hEntity& theEntityID);
  /// \brief Returns the entity by its ID
  const Slvs_Entity& getEntity(const Slvs_hEntity& theEntityID) const;

  /// \brief Verifies the current point or another coincident one is fixed
  /// \return the ID of the Fixed constraint or SLVS_E_UNKNOWN
  Slvs_hConstraint isPointFixed(const Slvs_hEntity& thePointID) const;

  /** \brief Add new constraint to the current group
   *  \param[in] theConstraint   SolveSpace's constraint
   *  \return the ID of added constraint
   */
  Slvs_hConstraint addConstraint(const Slvs_Constraint& theConstraint);
  /** \brief Updates constraint in the current group.
   *         If the ID of constraint is zero, the new item will be added
   *  \param[in] theConstraint  SolveSpace constraint
   *  \return the ID of updated/added constraint
   */
  Slvs_hConstraint updateConstraint(const Slvs_Constraint& theConstraint);
  /** \brief Removes the constraint by its ID. All entities and parameters depending on this
   *         constraint, which are not used in other constraints, will be removed too.
   *  \param[in] theConstraintID  index of constraint to be removed
   *  \return \c true if the constraint was successfully removed
   */
  bool removeConstraint(const Slvs_hConstraint& theConstraintID);
  /// \brief Returns the constraint by its ID
  const Slvs_Constraint& getConstraint(const Slvs_hConstraint& theConstraintID) const;

  /// \brief Attach temporary constraint to this storage. It need to make precise calculations
  void addTemporaryConstraint(const Slvs_hConstraint& theConstraintID);

  /// \brief Shows the sketch should be resolved
  bool isNeedToResolve() const
  { return myNeedToResolve; }

  /// \brief Changes the flag of group to be resolved
  void setNeedToResolve(bool theFlag)
  { myNeedToResolve = theFlag; }

  /// \brief Returns lists of removed elements
  void getRemoved(std::set<Slvs_hParam>& theParameters,
                  std::set<Slvs_hEntity>& theEntities,
                  std::set<Slvs_hConstraint>& theConstraints);

  /// \brief Initialize constraint solver by the entities collected by current storage
  void initializeSolver(SketchSolver_Solver& theSolver);

private:
  /// \brief Store coincident points
  void addCoincidentPoints(const Slvs_hEntity& thePoint1, const Slvs_hEntity& thePoint2);
  /// \brief Remove point from lists of coincidence
  void removeCoincidentPoint(const Slvs_hEntity& thePoint);

public:
  /// \brief Check two points are coincident
  bool isCoincident(const Slvs_hEntity& thePoint1, const Slvs_hEntity& thePoint2) const;

private:
  Slvs_hParam myParamMaxID; ///< current parameter index (may differs with the number of parameters)
  std::vector<Slvs_Param> myParameters; ///< list of parameters used in the current group of constraints (sorted by the identifier)
  Slvs_hEntity myEntityMaxID; ///< current entity index (may differs with the number of entities)
  std::vector<Slvs_Entity> myEntities; ///< list of entities used in the current group of constraints (sorted by the identifier)
  Slvs_hConstraint myConstrMaxID; ///< current constraint index (may differs with the number of constraints)
  std::vector<Slvs_Constraint> myConstraints; ///< list of constraints used in the current group (sorted by the identifier)

  std::vector< std::set<Slvs_hEntity> > myCoincidentPoints; ///< lists of coincident points
  Slvs_hConstraint myFixed; ///< identifier of one of temporary constraints to fix separate point

  bool myNeedToResolve; ///< parameters are changed and group needs to be resolved

  std::set<Slvs_hParam> myRemovedParameters; ///< list of just removed parameters (cleared when returning to applicant)
  std::set<Slvs_hEntity> myRemovedEntities; ///< list of just removed entities (cleared when returning to applicant)
  std::set<Slvs_hConstraint> myRemovedConstraints; ///< list of just removed constraints (cleared when returning to applicant)
};

typedef std::shared_ptr<SketchSolver_Storage> StoragePtr;

#endif
