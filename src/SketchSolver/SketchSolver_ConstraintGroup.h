// File:    SketchSolver_ConstraintGroup.h
// Created: 27 May 2014
// Author:  Artem ZHIDKOV

#ifndef SketchSolver_ConstraintGroup_H_
#define SketchSolver_ConstraintGroup_H_

#include "SketchSolver.h"
#include <SketchSolver_Solver.h>

#include <SketchPlugin_Constraint.h>
#include <ModelAPI_Data.h>

#include <list>
#include <map>
#include <vector>
#include <set>

/** \class   SketchSolver_ConstraintGroup
 *  \ingroup DataModel
 *  \brief   Keeps the group of constraints which based on the same entities
 */
class SketchSolver_ConstraintGroup
{
 public:
  /** \brief New group based on specified workplane.
   *         Throws an exception if theWorkplane is not an object of SketchPlugin_Sketch type
   *  \remark Type of theSketch is not verified inside
   */
  SketchSolver_ConstraintGroup(boost::shared_ptr<SketchPlugin_Feature> theWorkplane);

  ~SketchSolver_ConstraintGroup();

  /// \brief Returns group's unique identifier
  inline const Slvs_hGroup& getId() const
  {
    return myID;
  }

  /// \brief Returns true if the group has no constraints yet
  inline bool isEmpty() const
  {
    return myConstraints.empty();
  }

  /// \brief Check for valid sketch data
  inline bool isWorkplaneValid() const
  {
    return mySketch->data() && mySketch->data()->isValid();
  }

  /** \brief Adds or updates a constraint in the group
   *  \param[in] theConstraint constraint to be changed
   *  \return \c true if the constraint added or updated successfully
   */
  bool changeConstraint(boost::shared_ptr<SketchPlugin_Constraint> theConstraint);

  /** \brief Verifies the constraint uses the objects from this group
   *  \param[in] theConstraint constraint for verification of interaction
   *  \return \c true if the constrained objects are used in current group
   */
  bool isInteract(boost::shared_ptr<SketchPlugin_Constraint> theConstraint) const;

  /** \brief Verifies the specified feature is equal to the base workplane for this group
   *  \param[in] theWorkplane the feature to be compared with base workplane
   *  \return \c true if workplanes are the same
   */
  bool isBaseWorkplane(boost::shared_ptr<SketchPlugin_Feature> theWorkplane) const;

  boost::shared_ptr<SketchPlugin_Feature> getWorkplane() const
  {
    return mySketch;
  }

  /** \brief Update parameters of workplane. Should be called when Update event is coming.
   *  \return \c true if workplane updated successfully, \c false if workplane parameters are not consistent
   */
  bool updateWorkplane();

  /** \brief If the entity is in this group it will updated
   *  \param[in] theEntity attribute, which values should update SolveSpace entity
   */
  void updateEntityIfPossible(boost::shared_ptr<ModelAPI_Attribute> theEntity);

  /** \brief Searches invalid features and constraints in the group and avoids them
   *  \return \c true if the group several constraints were removed
   */
  bool updateGroup();

  /** \brief Add specified group to this one
   *  \param[in] theGroup group of constraint to be added
   */
  void mergeGroups(const SketchSolver_ConstraintGroup& theGroup);

  /** \brief Cut from the group several subgroups, which are not connected to the current one by any constraint
   *  \param[out] theCuts enlarge this list by newly created groups
   */
  void splitGroup(std::vector<SketchSolver_ConstraintGroup*>& theCuts);

  /** \brief Start solution procedure if necessary and update attributes of features
   *  \return \c false when no need to solve constraints
   */
  bool resolveConstraints();

  /** \brief Searches the constraints built on the entity and emit the signal to update them
   *  \param[in] theEntity attribute of the constraint
   */
  void updateRelatedConstraints(boost::shared_ptr<ModelAPI_Attribute> theEntity) const;
  void updateRelatedConstraints(boost::shared_ptr<ModelAPI_Feature> theFeature) const;

 protected:
  /** \brief Adds or updates an entity in the group
   *
   *  The parameters of entity will be parsed and added to the list of SolveSpace parameters.
   *  Parameters of certain entity will be placed sequentially in the list.
   *
   *  \param[in] theEntity the object of constraint
   *  \return identifier of changed entity or 0 if entity could not be changed
   */
  Slvs_hEntity changeEntity(boost::shared_ptr<ModelAPI_Attribute> theEntity);
  Slvs_hEntity changeEntity(FeaturePtr theEntity);

  /** \brief Adds or updates a normal in the group
   *
   *  Normal is a special entity in SolveSpace, which defines a direction in 3D and
   *  a rotation about this direction. So, SolveSpace represents normals as unit quaternions.
   *
   *  To define a normal there should be specified two coordinate axis
   *  on the plane transversed to created normal.
   *
   *  \param[in] theDirX first coordinate axis of the plane
   *  \param[in] theDirY second coordinate axis of the plane
   *  \param[in] theNorm attribute for the normal (used to identify newly created entity)
   *  \return identifier of created or updated normal
   */
  Slvs_hEntity changeNormal(boost::shared_ptr<ModelAPI_Attribute> theDirX,
                            boost::shared_ptr<ModelAPI_Attribute> theDirY,
                            boost::shared_ptr<ModelAPI_Attribute> theNorm);

  /** \brief Adds or updates a parameter in the group
   *  \param[in] theParam   the value of parameter
   *  \param[in] thePrmIter the cell in the list of parameters which should be changed
   *                        (the iterator will be increased if it does not reach the end of the list)
   *  \return identifier of changed parameter; when the parameter cannot be created, returned ID is 0
   */
  Slvs_hParam changeParameter(const double& theParam,
                              std::vector<Slvs_Param>::const_iterator& thePrmIter);

  /** \brief Removes constraints from the group
   *  \param[in] theConstraint constraint to be removed
   */
  void removeConstraint(boost::shared_ptr<SketchPlugin_Constraint> theConstraint);

  /** \brief Change values of attribute by parameters received from SolveSpace solver
   *  \param[in,out] theAttribute pointer to the attribute to be changed
   *  \param[in]     theEntityID  identifier of SolveSpace entity, which contains updated data
   *  \return \c true if the attribute's value has changed
   */
  bool updateAttribute(boost::shared_ptr<ModelAPI_Attribute> theAttribute,
                       const Slvs_hEntity& theEntityID);

  /** \brief Adds a constraint for a point which should not be changed during computations
   *  \param[in] theEntity the base for the constraint
   */
  void addTemporaryConstraintWhereDragged(boost::shared_ptr<ModelAPI_Attribute> theEntity);

  /** \brief Remove all temporary constraint after computation finished
   */
  void removeTemporaryConstraints();

 private:
  /** \brief Creates a workplane from the sketch parameters
   *  \param[in] theSketch parameters of workplane are the attributes of this sketch
   *  \return \c true if success, \c false if workplane parameters are not consistent
   */
  bool addWorkplane(boost::shared_ptr<SketchPlugin_Feature> theSketch);

  /** \brief Add the entities of constraint for points coincidence into the appropriate list
   *  \param[in] thePoint1 identifier of the first point
   *  \param[in] thePoint2 identifier of the second point
   *  \return \c true if the points are added successfully, and 
   *          \c false if the constraint is the extra one (should not be created in SolveSpace)
   */
  bool addCoincidentPoints(const Slvs_hEntity& thePoint1, const Slvs_hEntity& thePoint2);

  /** \brief Verifies and changes parameters of constriant, 
   *         e.g. sign of the distance between line and point
   *  \param[in,out] theConstraint SolveSpace constraint to be verified
   */
  void checkConstraintConsistence(Slvs_Constraint& theConstraint);

 private:
  // SolveSpace entities
  Slvs_hGroup myID;            ///< the index of the group
  Slvs_Entity myWorkplane;     ///< Workplane for the current group
  std::vector<Slvs_Param> myParams;        ///< List of parameters of the constraints
  Slvs_hParam myParamMaxID;    ///< Actual maximal ID of parameters (not equal to myParams size)
  std::vector<Slvs_Entity> myEntities;      ///< List of entities of the constaints
  Slvs_hEntity myEntityMaxID;   ///< Actual maximal ID of entities (not equal to myEntities size)
  std::vector<Slvs_Constraint> myConstraints;   ///< List of constraints in SolveSpace format
  Slvs_hConstraint myConstrMaxID;  ///< Actual maximal ID of constraints (not equal to myConstraints size)
  bool myNeedToSolve;  ///< Indicator that something changed in the group and constraint system need to be rebuilt

  SketchSolver_Solver myConstrSolver;  ///< Solver for set of equations obtained by constraints

  std::vector<Slvs_hParam> myTempPointWhereDragged;  ///< Parameters of one of the points which is moved by user
  Slvs_hEntity myTempPointWDrgdID;      ///< Identifier of such point
  std::list<Slvs_hConstraint> myTempConstraints;  ///< The list of identifiers of temporary constraints (SLVS_C_WHERE_DRAGGED) applied for all other points moved by user

  // SketchPlugin entities
  boost::shared_ptr<SketchPlugin_Feature> mySketch;        ///< Equivalent to workplane
  std::map<boost::shared_ptr<SketchPlugin_Constraint>, Slvs_hConstraint> myConstraintMap;  ///< The map between SketchPlugin and SolveSpace constraints
  std::map<boost::shared_ptr<ModelAPI_Attribute>, Slvs_hEntity> myEntityAttrMap;  ///< The map between "attribute" parameters of constraints and their equivalent SolveSpace entities
  std::map<FeaturePtr, Slvs_hEntity> myEntityFeatMap;  ///< The map between "feature" parameters of constraints and their equivalent SolveSpace entities

  // Conincident items
  std::vector<std::set<Slvs_hEntity> > myCoincidentPoints;  ///< Stores the lists of identifiers of coincident points (to avoid unnecessary coincidence constraints)
  std::set<boost::shared_ptr<SketchPlugin_Constraint> > myExtraCoincidence;  ///< Additional coincidence constraints which are not necessary (coincidence between points already done
                                                                             ///< by other constraints) but created by GUI tools. Useful when some coincidence constraints were removed
};

#endif
