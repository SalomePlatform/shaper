// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:    PlaneGCSSolver_Storage.h
// Created: 14 Dec 2015
// Author:  Artem ZHIDKOV

#ifndef PlaneGCSSolver_Storage_H_
#define PlaneGCSSolver_Storage_H_

#include <PlaneGCSSolver_Defs.h>

#include <SketchSolver.h>
#include <SketchSolver_Storage.h>
#include <SketchSolver_ISolver.h>

/** \class   PlaneGCSSolver_Storage
 *  \ingroup Plugins
 *  \brief   Contains all necessary data in PlaneGCS format to solve a single group of constraints
 */
class PlaneGCSSolver_Storage : public SketchSolver_Storage
{
public:
  PlaneGCSSolver_Storage(const GroupID& theGroup);

// =============   Inherited from SketchSolver_Storage   =============

  /// \brief Change mapping between constraint from SketchPlugin and
  ///        the list of constraints applicable for corresponding solver.
  ///        Derived here to update point-point coincidence.
  /// \param theConstraint        [in]   original SketchPlugin constraint
  /// \param theSolverConstraints [in]   list of solver's constraints
  virtual void addConstraint(ConstraintPtr                   theConstraint,
                             std::list<ConstraintWrapperPtr> theSolverConstraints);

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

  /// \brief Shows the storage has the same constraint twice
  virtual bool hasDuplicatedConstraint() const
  { return false; }

  /// \brief Calculate point on theBase entity. Value theCoeff is in [0.0 .. 1.0] and
  ///        shows the distance from the start point.
  virtual EntityWrapperPtr calculateMiddlePoint(EntityWrapperPtr theBase, double theCoeff);

  /// \brief Initialize solver by constraints, entities and parameters
  virtual void initializeSolver(SolverPtr theSolver);

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

  /// \brief Update the group for the given entity, its sub-entities and parameters
  virtual void changeGroup(EntityWrapperPtr theEntity, const GroupID& theGroup);
  /// \brief Update the group for the given parameter
  virtual void changeGroup(ParameterWrapperPtr theParam, const GroupID& theGroup);


// =============   Own methods   =============

  /// \brief Move parameters of the entity to the constants
  void makeConstant(const EntityWrapperPtr& theEntity);
  /// \brief Move parameters of the entity to the variables
  void makeVariable(const EntityWrapperPtr& theEntity);

private:
  /// \brief Move parameters of the entity from the list of variables to the list of constants
  ///        and vice versa
  /// \param theEntity [in]  entity to be changed
  /// \param theFrom   [out] source list
  /// \param theTo     [out] destination list
  void toggleEntity(const EntityWrapperPtr& theEntity, GCS::VEC_pD& theFrom, GCS::VEC_pD& theTo);

  /// \brief Create additional constraints for correct processing of arcs
  /// \param theArc [in]  updated arc
  void processArc(const EntityWrapperPtr& theArc);

  /// \brief Adjust parameters of points coincident with the given
  void updateCoincident(const EntityWrapperPtr& thePoint);

private:
  GCS::VEC_pD                      myParameters;         ///< list of parameters
  GCS::VEC_pD                      myConst;              ///< list of constants
  EntityID                         myEntityLastID;       ///< identifier of last added entity
  ConstraintID                     myConstraintLastID;   ///< identifier of last added constraint

  std::map<EntityWrapperPtr, std::vector<GCSConstraintPtr> >
                                  myArcConstraintMap;    ///< additional constraints for correct processing of the arcs

  std::list<GCSConstraintPtr>      myRemovedConstraints; ///< list of removed constraints to notify solver
};

#endif
