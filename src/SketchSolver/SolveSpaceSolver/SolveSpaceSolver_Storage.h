// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:    SolveSpaceSolver_Storage.h
// Created: 18 Mar 2015
// Author:  Artem ZHIDKOV

#ifndef SolveSpaceSolver_Storage_H_
#define SolveSpaceSolver_Storage_H_

#include <SketchSolver_Storage.h>
#include <SolveSpaceSolver_Solver.h>

#include <list>
#include <memory>
#include <set>
#include <vector>

typedef std::list< std::set<ConstraintWrapperPtr> >             SameConstraintMap;

/** \class   SolveSpaceSolver_Storage
 *  \ingroup Plugins
 *  \brief   Contains all necessary data in SolveSpace format to solve a single group of constraints
 */
class SolveSpaceSolver_Storage : public SketchSolver_Storage
{
public:
  SolveSpaceSolver_Storage(const GroupID& theGroup);

// =============   Inherited from SketchSolver_Storage   =============

  /// \brief Update constraint's data
  /// \return \c true if any value is updated
  virtual bool update(ConstraintWrapperPtr theConstraint);
  /// \brief Update entity's data
  /// \return \c true if any value is updated
  virtual bool update(EntityWrapperPtr theEntity);
  /// \brief Update parameter's data
  /// \return \c true if the value of parameter is updated
  virtual bool update(ParameterWrapperPtr theParameter);

  /// \brief Update SketchPlugin features after resolving constraints
  /// \param theFixedOnly [in]  if \c true the fixed points will be updated only
  virtual void refresh(bool theFixedOnly = false) const;

  /// \brief Check if some parameters or entities are returned
  ///        to the current group after removing temporary constraints
  virtual void verifyFixed();

  /// \brief Mark two points as coincident
  virtual void addCoincidentPoints(EntityWrapperPtr theMaster, EntityWrapperPtr theSlave);

  /// \brief Calculate point on theBase entity. Value theCoeff is in [0.0 .. 1.0] and
  ///        shows the distance from the start point.
  virtual EntityWrapperPtr calculateMiddlePoint(EntityWrapperPtr theBase, double theCoeff);

protected:
  /// \brief Remove constraint
  /// \return \c true if the constraint and all its parameters are removed successfully
  virtual bool remove(ConstraintWrapperPtr theConstraint);
  /// \brief Remove entity
  /// \return \c true if the entity and all its parameters are removed successfully
  virtual bool remove(EntityWrapperPtr theEntity);
  /// \brief Remove parameter
  /// \return \c true if the parameter has been removed
  virtual bool remove(ParameterWrapperPtr theParameter);

  /// \brief Remove point-point coincidence
  bool removeCoincidence(ConstraintWrapperPtr theConstraint);

  /// \brief Update the group for the given entity, its sub-entities and parameters
  virtual void changeGroup(EntityWrapperPtr theEntity, const GroupID& theGroup);
  /// \brief Update the group for the given parameter
  virtual void changeGroup(ParameterWrapperPtr theParam, const GroupID& theGroup);


// =============   Own methods   =============
public:
  /// \brief Obtain and store identifier of sketch
  void storeWorkplane(EntityWrapperPtr theSketch);

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
  /// \brief Returns maximal ID of entities in this storage
  const Slvs_hEntity& entityMaxID() const
  { return myEntityMaxID; }

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
  /// \brief Returns quantity of constraints in this storage
  size_t nbConstraints() const
  { return myConstraints.size(); }

  /// \brief Shows the storage has the same constraint twice
  virtual bool hasDuplicatedConstraint() const
  { return myDuplicatedConstraint; }

  /// \brief Initialize constraint solver by the entities collected by current storage
  virtual void initializeSolver(SolverPtr theSolver);

private:
  /// \brief Update arc points to be on circle sharp.
  void adjustArc(const Slvs_Entity& theArc);

  /// \brief Replace sub-entity theSource in all features by theDest
  void replaceInFeatures(EntityWrapperPtr theSource, EntityWrapperPtr theDest);
  /// \brief Replace constrained entity theSource by theDest in all constraints;
  void replaceInConstraints(EntityWrapperPtr theSource, EntityWrapperPtr theDest);

  /// \brief Add pair of constraints which have same representation in SolveSpace notation.
  ///
  ///        These constraints may be different and become the same after the substitution
  ///        of point coincidence.
  void addSameConstraints(ConstraintWrapperPtr theConstraint1, ConstraintWrapperPtr theConstraint2);

  /// \brief Search constraint equal to the given in terms of SolveSpace notation
  bool findSameConstraint(ConstraintWrapperPtr theConstraint);

private:
  Slvs_hEntity myWorkplaneID; ///< identifier of workplane

  Slvs_hParam myParamMaxID; ///< current parameter index (may differs with the number of parameters)
  std::vector<Slvs_Param> myParameters; ///< list of parameters used in the current group of constraints (sorted by the identifier)
  Slvs_hEntity myEntityMaxID; ///< current entity index (may differs with the number of entities)
  std::vector<Slvs_Entity> myEntities; ///< list of entities used in the current group of constraints (sorted by the identifier)
  Slvs_hConstraint myConstrMaxID; ///< current constraint index (may differs with the number of constraints)
  std::vector<Slvs_Constraint> myConstraints; ///< list of constraints used in the current group (sorted by the identifier)

  bool myDuplicatedConstraint; ///< shows the storage has same constraint twice

  SameConstraintMap myEqualConstraints; ///< list of groups of equal constraints
};

#endif
