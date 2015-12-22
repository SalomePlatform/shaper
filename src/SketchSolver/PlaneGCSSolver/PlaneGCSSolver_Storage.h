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

////#include <GeomDataAPI_Point2D.h>
////#include <ModelAPI_Attribute.h>
////#include <ModelAPI_AttributeDouble.h>
////#include <ModelAPI_Feature.h>
////#include <SketchPlugin_Constraint.h>
////
////#include <Geo.h>
////#include <Util.h>
////
////#include <list>
////#include <memory>
////#include <set>
////#include <vector>

////#ifndef OBSOLETE
////#include <Constraints.h>
////
////typedef std::shared_ptr<GCS::Point> GCSPointPtr;
////typedef std::shared_ptr<GCS::Curve> GCSCurvePtr;
////typedef GCS::Constraint*            GCSConstraintPtr;
////#endif

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



#ifndef OBSOLETE

////  /** \brief Add or change constraint and corresponding GCS constraints to the storage
////   *  \param theConstraint     [in] base SketchPlugin constraint
////   *  \param theGCSConstraints [in] list of GCS constraints created by SketchSolver constraint
////   */
////  void changeConstraint(const ConstraintPtr& theConstraint,
////                        const std::vector<GCSConstraintPtr>& theGCSConstraints);

////  /** \brief Add GCS constraint not related to any SketchPlugin constraint.
////   *         For example, it may be related to the moved feature.
////   *  \param theConstraint [in] GCS constraint
////   */
////  void addConstraint(GCSConstraintPtr theConstraint);

////  /** \brief Convert SketchPlugin_Feature to the corresponding entity in FreeGCS
////   *  \param theEntity [in] feature to convert
////   *  \return ID of created or updated entity
////   */
////  EntityID changeEntity(const FeaturePtr& theEntity);
////  /** \brief Convert ModelAPI_Attribute to the corresponding entity in FreeGCS
////   *  \param theEntity [in] attribute to convert
////   *  \return ID of created or updated entity
////   */
////  EntityID changeEntity(const AttributePtr& theEntity);

////  /** \brief Remove constraint and corresponding GCS constraints
////   *  \param theConstraint [in] constraint to remove
////   *  \return \c true, if the constraint is fully removed,
////   *          \c false, if there are available entities used by other constraints
////   */
////  bool removeConstraint(const ConstraintPtr& theConstraint);
////  /** \brief Remove GCS constraint
////   *  \param theConstraint [in] constraint to remove
////   *  \return \c true, if the constraint is fully removed
////   */
////  bool removeConstraint(GCSConstraintPtr theConstraint);
////  /** \brief Remove feature and corresponding GCS entities
////   *  \param theFeature [in] feature to remove
////   *  \return \c true, if the feature is fully removed,
////   *          \c false, if there are available entities used by other constraints or features
////   */
////  bool removeEntity(const FeaturePtr& theFeature);
////  /** \brief Remove attribute and corresponding GCS entities
////   *  \param theAttribute [in] attribute to remove
////   *  \return \c true, if the attribute is removed,
////   *          \c false, if it is used by existent features or constraints
////   */
////  bool removeEntity(const AttributePtr& theAttribute);

  /// \brief Move parameters of the entity to the constants
  void makeConstant(const EntityWrapperPtr& theEntity);
  /// \brief Move parameters of the entity to the variables
  void makeVariable(const EntityWrapperPtr& theEntity);

////  /// \brief Return scalar by its ID or null pointer
////  double*      getScalar(const EntityID& theID) const;
////  /// \brief Return point by its ID or null pointer
////  GCS::Point*  getPoint(const EntityID& theID) const;
////  /// \brief Return line by its ID or null pointer
////  GCS::Line*   getLine(const EntityID& theID) const;
////  /// \brief Return circle by its ID or null pointer
////  GCS::Circle* getCircle(const EntityID& theID) const;
////  /// \brief Return arc by its ID or null pointer
////  GCS::Arc*    getArc(const EntityID& theID) const;
////
////  /// \brief Return list of GCS constraints corresponding to the given
////  const std::vector<GCSConstraintPtr>& getConstraint(const ConstraintPtr& theConstraint) const;

////  /// \brief Verifies, the attribute is available in current storage
////  bool isInteract(AttributePtr theAttribute) const;
////  /// \brief Verifies, the feature is available in current storage
////  bool isInteract(FeaturePtr theFeature) const;

private:
////  /** \brief Creates scalar parameter in the list of parameters
////   *  \param theScalar [in] value to be converted
////   *  \return pointer to new scalar
////   */
////  double* createScalar(const AttributeDoublePtr& theScalar = AttributeDoublePtr());
////  /** \brief Creates two parameters in the list of parameters to store point coordinates
////   *  \param thePoint [in] point to be converted
////   *  \return FreeGCS point
////   */
////  GCS::Point createPoint(const std::shared_ptr<GeomDataAPI_Point2D>& thePoint);
////  /** \brief Creates or updates a scalar value
////   *  \param theScalar [in] attribute to update scalar value
////   *  \param theID     [in] ID of scalar to be updated
////   */
////  void updateScalar(const AttributeDoublePtr& theScalar, EntityID& theID);
////  /** \brief Creates or updates an angular value
////   *  \param theAngle [in] attribute to update scalar value
////   *  \param theID    [in] ID of scalar to be updated
////   */
////  void updateAngle(const AttributeDoublePtr& theAngle, EntityID& theID);
////  /** \brief Creates or updates parameters of the point
////   *  \param thePoint [in] point used to update parameters
////   *  \param theID    [in] ID of FreeGCS entity to be updated
////   */
////  void updatePoint(const std::shared_ptr<GeomDataAPI_Point2D>& thePoint,
////                         EntityID&                             theID);
////  /** \brief Creates or updates parameters of the line
////   *  \param theLine [in]     line used to update parameters
////   *  \param theID   [in/out] ID of FreeGCS entity to be updated (cleared if problems)
////   */
////  void updateLine(const FeaturePtr& theLine, EntityID& theID);
////  /** \brief Creates or updates parameters of the circle
////   *  \param theLine [in]     circle used to update parameters
////   *  \param theID   [in/out] ID of FreeGCS entity to be updated (cleared if problems)
////   */
////  void updateCircle(const FeaturePtr& theCircle, EntityID& theID);
////  /** \brief Creates or updates parameters of the arc
////   *  \param theLine [in]     arc used to update parameters
////   *  \param theID   [in/out] ID of FreeGCS entity to be updated (cleared if problems)
////   */
////  void updateArc(const FeaturePtr& theArc, EntityID& theID);

////  /// \brief Check the feature is used by constraints
////  bool isUsed(const FeaturePtr& theFeature) const;
////  /// \brief Check the attribute is used by constraints or features
////  bool isUsed(const AttributePtr& theAttribute) const;

////  /// \brief Remove given parameter from the lists of parameters and constants
////  void removeParameters(double* theParam1, double* theParam2 = 0);

  /// \brief Move parameters of the entity from the list of variables to the list of constants
  ///        and vice versa
  /// \param theEntity [in]  entity to be changed
  /// \param theFrom   [out] source list
  /// \param theTo     [out] destination list
  void toggleEntity(const EntityWrapperPtr& theEntity, GCS::VEC_pD& theFrom, GCS::VEC_pD& theTo);

  /// \brief Create additional constraints for correct processing of arcs
  /// \param theArc [in]  updated arc
  void processArc(const EntityWrapperPtr& theArc);

private:
  GCS::VEC_pD                      myParameters;         ///< list of parameters
  GCS::VEC_pD                      myConst;              ///< list of constants
  EntityID                         myEntityLastID;       ///< identifier of last added entity

////  std::map<ConstraintPtr, std::vector<GCSConstraintPtr> >
////                                   myConstraintsMap;     ///< map SketchPlugin constraints to corresponding GCS constraints
////
////  std::map<AttributePtr, EntityID> myAttributeEntityMap; ///< map attributes to corresponding entities
////  std::map<FeaturePtr, EntityID>   myFeatureEntityMap;   ///< map features to corresponding entities
////
////  std::map<EntityID, double*>      myGCSScalars;         ///< list of scalar values and their IDs
////  std::map<EntityID, GCSPointPtr>  myGCSPoints;          ///< list of points and their IDs
////  std::map<EntityID, GCSCurvePtr>  myGCSEntities;        ///< list of lines, circles and arcs and their IDs

  std::map<EntityWrapperPtr, std::vector<GCSConstraintPtr> >
                                  myArcConstraintMap;    ///< additional constraints for correct processing of the arcs

////  std::list<double*>               myRemovedParameters;  ///< list of removed parameters
  std::list<GCSConstraintPtr>      myRemovedConstraints; ///< list of removed constraints to notify solver

#else

// ================================================================================================
// ===================     OBSOLETE     ===========================================================
public:
  PlaneGCSSolver_Storage();

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
  /** \brief Remove all entities, which are not used in constraints
   */
  void removeUnusedEntities();
  /// \brief Returns the entity by its ID
  const Slvs_Entity& getEntity(const Slvs_hEntity& theEntityID) const;
  /// \brief Makes a full copy of the given entity
  Slvs_hEntity copyEntity(const Slvs_hEntity& theCopied);
  /// \brief Copy one entity to another
  void copyEntity(const Slvs_hEntity& theFrom, const Slvs_hEntity& theTo);
  /// \brief Check the entity is used in constraints
  bool isUsedByConstraints(const Slvs_hEntity& theEntityID) const;

  /// \brief Verifies the current point or another coincident one is fixed
  /// \param[in]  thePointID  entity to be checked fixed
  /// \param[out] theFixed    ID of constraint
  /// \param[in]  theAccurate if \c true, the calculation will be made for all type of constraints,
  ///                         if \c false, only the point is verified
  /// \return \c true if the point is fixed
  bool isPointFixed(const Slvs_hEntity& thePointID, Slvs_hConstraint& theFixed, bool theAccurate = false) const;
  /// \brief Verifies the current entity is fully fixed (may not be changed by constraints)
  /// \param[in] theEntityID entity to be checked fixed
  /// \param[in] theAccurate if \c true, the calculation will be made for all type of constraints,
  ///                        if \c false, only points are verified
  /// \return \c true if the entity is fixed
  bool isEntityFixed(const Slvs_hEntity& theEntityID, bool theAccurate = false) const;

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
  /// \brief Returns list of constraints of specified type
  std::list<Slvs_Constraint> getConstraintsByType(int theConstraintType) const;

  /// \brief Attach constraint SLVS_C_WHERE_DRAGGED to this storage. It need to make precise calculations
  void addConstraintWhereDragged(const Slvs_hConstraint& theConstraintID);

  /// \brief Add transient constraint
  void addTemporaryConstraint(const Slvs_hConstraint& theConstraintID);
  /// \brief Remove all transient constraints
  void removeTemporaryConstraints();
  /// \brief Remove one temporary constraint. Preferable to remove the points under Point-on-Line constraint
  /// \return Number of remaining temporary constraints
  int deleteTemporaryConstraint();
  /// \brief Checks the constraint is temporary
  bool isTemporary(const Slvs_hConstraint& theConstraintID) const;
  /// \brief Number of temporary constraints
  int numberTemporary() const
  { return (int)myTemporaryConstraints.size(); }

  /// \brief Shows the sketch should be resolved
  bool isNeedToResolve();

  /// \brief Shows the storage has the same constraint twice
  bool hasDuplicatedConstraint() const
  { return myDuplicatedConstraint; }

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
  /// \brief Remove point-point coincidence
  void removeCoincidence(const Slvs_Constraint& theCoincidence);

public:
  /// \brief Check two points are coincident
  bool isCoincident(const Slvs_hEntity& thePoint1, const Slvs_hEntity& thePoint2) const;

  /// \brief Check two points are coincident or have same coordinates
  bool isEqual(const Slvs_hEntity& thePoint1, const Slvs_hEntity& thePoint2) const;

  /// \brief Check the entity is horizontal of vertical
  bool isAxisParallel(const Slvs_hEntity& theEntity) const;

  /// \brief Verifies the entity is used in any equal constraint
  /// \param[in]  theEntity entity to be found
  /// \param[out] theEqual  constraint, which uses the entity
  /// \return \c true, if the Equal constrait is found
  bool isUsedInEqual(const Slvs_hEntity& theEntity, Slvs_Constraint& theEqual) const;

  /// \brief Fixes specified entity
  /// \param theEntity  ID of the entity to be fixed
  /// \return List of created constraints
  std::vector<Slvs_hConstraint> fixEntity(const Slvs_hEntity& theEntity);

private:
  /// \brief Fixes specified point
  /// \param [in]  thePoint    point to be fixed
  /// \param [out] theCreated  list of the Fixed constraints created
  void fixPoint(const Slvs_Entity& thePoint, std::vector<Slvs_hConstraint>& theCreated);
  /// \brief Fixes specified line
  /// \param [in]  theLine     line to be fixed
  /// \param [out] theCreated  list of the Fixed constraints created
  void fixLine(const Slvs_Entity& theLine, std::vector<Slvs_hConstraint>& theCreated);
  /// \brief Fixes specified circle
  /// \param [in]  theCircle   circle to be fixed
  /// \param [out] theCreated  list of the Fixed constraints created
  void fixCircle(const Slvs_Entity& theCircle, std::vector<Slvs_hConstraint>& theCreated);
  /// \brief Fixes specified arc
  /// \param [in]  theArc      arc to be fixed
  /// \param [out] theCreated  list of the Fixed constraints created
  void fixArc(const Slvs_Entity& theArc, std::vector<Slvs_hConstraint>& theCreated);

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
  bool myDuplicatedConstraint; ///< shows the storage has same constraint twice

  std::set<Slvs_hConstraint> myTemporaryConstraints; ///< list of transient constraints
  std::set<Slvs_hParam> myRemovedParameters; ///< list of just removed parameters (cleared when returning to applicant)
  std::set<Slvs_hEntity> myRemovedEntities; ///< list of just removed entities (cleared when returning to applicant)
  std::set<Slvs_hConstraint> myRemovedConstraints; ///< list of just removed constraints (cleared when returning to applicant)
  std::set<Slvs_hParam> myUpdatedParameters; ///< list of just updated parameters (cleared when isNeedToResolve() called)
#endif
};

#endif
