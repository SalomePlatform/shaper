// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:    SketchSolver_Storage.h
// Created: 30 Nov 2015
// Author:  Artem ZHIDKOV

#ifndef SketchSolver_Storage_H_
#define SketchSolver_Storage_H_

#include <SketchSolver.h>
#include <SketchSolver_IConstraintWrapper.h>
#include <SketchSolver_IEntityWrapper.h>
#include <SketchSolver_IParameterWrapper.h>
#include <SketchSolver_ISolver.h>

#include <ModelAPI_Attribute.h>
#include <ModelAPI_AttributeDouble.h>
#include <ModelAPI_Feature.h>
#include <SketchPlugin_Constraint.h>


typedef std::map<EntityWrapperPtr, std::set<EntityWrapperPtr> > CoincidentPointsMap;


/** \class   SketchSolver_Storage
 *  \ingroup Plugins
 *  \brief   Interface to map SketchPlugin features to the entities of corresponding solver.
 */
class SketchSolver_Storage
{
private:
  SketchSolver_Storage();
  SketchSolver_Storage(const SketchSolver_Storage&);
  SketchSolver_Storage& operator=(const SketchSolver_Storage&);

public:
  SketchSolver_Storage(const GroupID& theGroup)
    : myGroupID(theGroup),
      myNeedToResolve(false),
      myEventsBlocked(false)
  {}

  /// \brief Change mapping between constraint from SketchPlugin and
  ///        a constraint applicable for corresponding solver.
  /// \param theConstraint       [in]   original SketchPlugin constraint
  /// \param theSolverConstraint [in]   solver's constraints
  SKETCHSOLVER_EXPORT void addConstraint(ConstraintPtr        theConstraint,
                                         ConstraintWrapperPtr theSolverConstraints);
  /// \brief Change mapping between constraint from SketchPlugin and
  ///        the list of constraints applicable for corresponding solver.
  /// \param theConstraint        [in]   original SketchPlugin constraint
  /// \param theSolverConstraints [in]   list of solver's constraints
  SKETCHSOLVER_EXPORT
    void addConstraint(ConstraintPtr                   theConstraint,
                       std::list<ConstraintWrapperPtr> theSolverConstraints);

  /// \brief Convert feature to the form applicable for specific solver and map it
  /// \param theFeature [in]  feature to convert
  /// \param theGroup   [in]  id of the group where the feature should be placed
  /// \return \c true if the feature has been created or updated
  SKETCHSOLVER_EXPORT bool update(FeaturePtr theFeature, const GroupID& theGroup = GID_UNKNOWN);
  /// \brief Convert attribute to the form applicable for specific solver and map it
  /// \param theFeature [in]  feature to convert
  /// \return \c true if the attribute has been created or updated
  SKETCHSOLVER_EXPORT bool update(AttributePtr theAttribute, const GroupID& theGroup = GID_UNKNOWN);

  /// \brief Returns constraint related to corresponding constraint
  SKETCHSOLVER_EXPORT
    const std::list<ConstraintWrapperPtr>& constraint(const ConstraintPtr& theConstraint) const;

  /// \brief Returns entity related to corresponding feature
  SKETCHSOLVER_EXPORT const EntityWrapperPtr& entity(const FeaturePtr& theFeature) const;
  /// \brief Returns entity related to corresponding attribute
  SKETCHSOLVER_EXPORT const EntityWrapperPtr& entity(const AttributePtr& theAttribute) const;

  /// \brief Return parsed sketch entity
  const EntityWrapperPtr& sketch() const;
  /// \brief Set parsed sketch entity.
  /// Be careful, this method does not update fields of the storage specific for the solver.
  /// Does not update if the sketch already exists.
  void setSketch(const EntityWrapperPtr& theSketch);

  /// \brief Mark two points as coincident
  virtual void addCoincidentPoints(EntityWrapperPtr theMaster, EntityWrapperPtr theSlave) = 0;

  /// \brief Shows the storage has any constraint twice
  virtual bool hasDuplicatedConstraint() const = 0;

  /// \brief Removes constraint from the storage
  /// \return \c true if the constraint and all its parameters are removed successfully
  SKETCHSOLVER_EXPORT bool removeConstraint(ConstraintPtr theConstraint);
  /// \brief Removes feature from the storage
  /// \return \c true if the feature and its attributes are removed successfully;
  ///         \c false if the feature or any it attribute is used by remaining constraints.
  SKETCHSOLVER_EXPORT bool removeEntity(FeaturePtr theFeature);
  /// \brief Removes attribute from the storage
  /// \return \c true if the attribute is not used by remaining features and constraints
  SKETCHSOLVER_EXPORT bool removeEntity(AttributePtr theAttribute);

  /// \brief Remove all features became invalid
  SKETCHSOLVER_EXPORT void removeInvalidEntities();

////  /// \brief Mark specified constraint as temporary
////  virtual void setTemporary(ConstraintPtr theConstraint) = 0;
////  /// \brief Returns number of temporary constraints
////  virtual size_t nbTemporary() const = 0;
////  /// \brief Remove temporary constraints
////  /// \param theNbConstraints [in]  number of temporary constraints to be deleted
////  /// \return number of remaining temporary constraints
////  virtual size_t removeTemporary(size_t theNbConstraints = 1) = 0;

  /// \brief Check whether the feature or its attributes are used by this storage
  /// \param theFeature [in]  feature to be checked
  /// \return \c true if the feature interacts with the storage
  bool isInteract(const FeaturePtr& theFeature) const;
  /// \brief Check whether the attribute is used by this storage
  /// \param theAttribute [in]  attribute to be checked
  /// \return \c true if the attribute interacts with the storage
  bool isInteract(const AttributePtr& theAttribute) const;

  /// \brief Check the features is not removed
  bool isConsistent() const;

  /// \brief Check the entity is fixed.
  ///        If the point is under verification, all coincident points are checked too.
  SKETCHSOLVER_EXPORT bool isFixed(EntityWrapperPtr theEntity) const;

  /// \brief Shows the sketch should be resolved
  virtual bool isNeedToResolve()
  { return myNeedToResolve; }
  /// \brief Changes the flag of group to be resolved
  void setNeedToResolve(bool theFlag)
  { myNeedToResolve = theFlag; }

  /// \brief Initialize solver by constraints, entities and parameters
  virtual void initializeSolver(SolverPtr theSolver) = 0;

  /// \brief Update SketchPlugin features after resolving constraints
  /// \param theFixedOnly [in]  if \c true the fixed points will be updated only
  virtual void refresh(bool theFixedOnly = false) const = 0;

  /// \brief Check if some parameters or entities are returned
  ///        to the current group after removing temporary constraints
  virtual void verifyFixed() = 0;

  /// \brief Calculate point on theBase entity. Value theCoeff is in [0.0 .. 1.0] and
  ///        shows the distance from the start point.
  virtual EntityWrapperPtr calculateMiddlePoint(EntityWrapperPtr theBase,
                                                double theCoeff) = 0;

  /// \brief Block or unblock events when refreshing features
  SKETCHSOLVER_EXPORT void blockEvents(bool isBlocked);

protected:
  /// \brief Change mapping feature from SketchPlugin and
  ///        the entity applicable for corresponding solver.
  /// \param theFeature      [in]  original SketchPlugin feature
  /// \param theSolverEntity [in]  solver's entity, created outside
  SKETCHSOLVER_EXPORT
    void addEntity(FeaturePtr       theFeature,
                   EntityWrapperPtr theSolverEntity);

  /// \brief Change mapping attribute of a feature and the entity applicable for corresponding solver.
  /// \param theAttribute    [in]  original attribute
  /// \param theSolverEntity [in]  solver's entity, created outside
  SKETCHSOLVER_EXPORT
    void addEntity(AttributePtr     theAttribute,
                   EntityWrapperPtr theSolverEntity);

  /// \brief Update constraint's data
  /// \return \c true if any value is updated
  virtual bool update(ConstraintWrapperPtr theConstraint) = 0;
  /// \brief Update entity's data
  /// \return \c true if any value is updated
  virtual bool update(EntityWrapperPtr theEntity) = 0;
  /// \brief Update parameter's data
  /// \return \c true if the value of parameter is updated
  virtual bool update(ParameterWrapperPtr theParameter) = 0;

  /// \brief Remove constraint
  /// \return \c true if the constraint and all its parameters are removed successfully
  SKETCHSOLVER_EXPORT virtual bool remove(ConstraintWrapperPtr theConstraint);
  /// \brief Remove entity
  /// \return \c true if the entity and all its parameters are removed successfully
  SKETCHSOLVER_EXPORT virtual bool remove(EntityWrapperPtr theEntity);
  /// \brief Remove parameter
  /// \return \c true if the parameter has been removed
  virtual bool remove(ParameterWrapperPtr theParameter) = 0;

  /// \brief Update the group for the given entity, its sub-entities and parameters
  virtual void changeGroup(EntityWrapperPtr theEntity, const GroupID& theGroup) = 0;
  /// \brief Update the group for the given parameter
  virtual void changeGroup(ParameterWrapperPtr theParam, const GroupID& theGroup) = 0;

private:
  /// \brief Find the normal of the sketch
  EntityWrapperPtr getNormal() const;

  /// \brief Verify the feature or any its attribute is used by constraint
  bool isUsed(FeaturePtr theFeature) const;
  /// \brief Verify the attribute is used by constraint
  bool isUsed(AttributePtr theAttirubute) const;

protected:
  GroupID myGroupID;       ///< identifier of the group, this storage belongs to
  bool    myNeedToResolve; ///< parameters are changed and group needs to be resolved
  bool    myEventsBlocked; ///< indicates that features do not send events

  /// map SketchPlugin constraint to a list of solver's constraints
  std::map<ConstraintPtr, std::list<ConstraintWrapperPtr> > myConstraintMap;
  /// map SketchPlugin feature to solver's entity
  std::map<FeaturePtr, EntityWrapperPtr>                    myFeatureMap;
  /// map attribute to solver's entity
  std::map<AttributePtr, EntityWrapperPtr>                  myAttributeMap;

  CoincidentPointsMap myCoincidentPoints; ///< lists of coincident points (first is a master point, second is a set of slaves)
};

typedef std::shared_ptr<SketchSolver_Storage> StoragePtr;

#endif
