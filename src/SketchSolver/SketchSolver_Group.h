// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:    SketchSolver_Group.h
// Created: 27 May 2014
// Author:  Artem ZHIDKOV

#ifndef SketchSolver_Group_H_
#define SketchSolver_Group_H_

#include "SketchSolver.h"
#include <SketchSolver_Constraint.h>
#include <SketchSolver_Storage.h>
#include <SketchSolver_FeatureStorage.h>
#include <SketchSolver_Solver.h>

#include <SketchPlugin_Constraint.h>
#include <ModelAPI_Data.h>
#include <ModelAPI_Feature.h>
#include <ModelAPI_AttributeRefList.h>

#include <memory>
#include <list>
#include <map>
#include <vector>
#include <set>

typedef std::map<ConstraintPtr, SolverConstraintPtr> ConstraintConstraintMap;

/** \class   SketchSolver_Group
 *  \ingroup Plugins
 *  \brief   Keeps the group of constraints which based on the same entities
 */
class SketchSolver_Group
{
 public:
  /** \brief New group based on specified workplane.
   *         Throws an exception if theWorkplane is not an object of SketchPlugin_Sketch type
   *  \remark Type of theSketch is not verified inside
   */
  SketchSolver_Group(std::shared_ptr<ModelAPI_CompositeFeature> theWorkplane);

  ~SketchSolver_Group();

  /// \brief Returns group's unique identifier
  inline const Slvs_hGroup& getId() const
  {
    return myID;
  }

  /// \brief Returns identifier of the workplane
  inline const Slvs_hEntity& getWorkplaneId() const
  {
    return myWorkplaneID;
  }

  /// \brief Find the identifier of the feature, if it already exists in the group
  Slvs_hEntity getFeatureId(FeaturePtr theFeature) const;
  /// \brief Find the identifier of the attribute, if it already exists in the group
  Slvs_hEntity getAttributeId(AttributePtr theAttribute) const;

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
  bool changeConstraint(std::shared_ptr<SketchPlugin_Constraint> theConstraint);

  /** \brief Updates the data corresponding the specified feature
   *  \param[in] theFeature the feature to be updated
   */
  bool updateFeature(std::shared_ptr<SketchPlugin_Feature> theFeature);

  /** \brief Updates the data corresponding the specified feature moved in GUI.
   *         Additional Fixed constraints are created.
   *  \param[in] theFeature the feature to be updated
   */
  void moveFeature(std::shared_ptr<SketchPlugin_Feature> theFeature);

  /** \brief Verifies the feature attributes are used in this group
   *  \param[in] theFeature constraint or any other object for verification of interaction
   *  \return \c true if some of attributes are used in current group
   */
  bool isInteract(std::shared_ptr<SketchPlugin_Feature> theFeature) const;

  /** \brief Verifies the specified feature is equal to the base workplane for this group
   *  \param[in] theWorkplane the feature to be compared with base workplane
   *  \return \c true if workplanes are the same
   */
  bool isBaseWorkplane(CompositeFeaturePtr theWorkplane) const;

  /// Returns the current workplane
  std::shared_ptr<ModelAPI_CompositeFeature> getWorkplane() const
  {
    return mySketch;
  }

  /** \brief Update parameters of workplane. Should be called when Update event is coming.
   *  \return \c true if workplane updated successfully, \c false if workplane parameters are not consistent
   */
  bool updateWorkplane();

  /** \brief Searches invalid features and constraints in the group and removes them
   *  \return \c false if the group several constraints were removed
   */
  bool isConsistent();

  /** \brief Add specified group to this one
   *  \param[in] theGroup group of constraint to be added
   */
  void mergeGroups(const SketchSolver_Group& theGroup);

  /** \brief Cut from the group several subgroups, which are not connected to the current one by any constraint
   *  \param[out] theCuts enlarge this list by newly created groups
   */
  void splitGroup(std::vector<SketchSolver_Group*>& theCuts);

  /** \brief Start solution procedure if necessary and update attributes of features
   *  \return \c false when no need to solve constraints
   */
  bool resolveConstraints();

protected:
  /** \brief Removes constraints from the group
   *  \param[in] theConstraint constraint to be removed
   */
  void removeConstraint(ConstraintPtr theConstraint);

  /** \brief Remove all temporary constraint after computation finished
   *  \param[in] theRemoved  indexes of constraints to be removed. If empty, all temporary constraints should be deleted
   */
  void removeTemporaryConstraints();

private:
  /** \brief Creates a workplane from the sketch parameters
   *  \param[in] theSketch parameters of workplane are the attributes of this sketch
   *  \return \c true if success, \c false if workplane parameters are not consistent
   */
  bool addWorkplane(CompositeFeaturePtr theSketch);

  /// \brief Apply temporary rigid constraints for the list of features
  void fixFeaturesList(AttributeRefListPtr theList);

private:
  Slvs_hGroup myID; ///< Index of the group
  Slvs_hEntity myWorkplaneID; ///< Index of workplane, the group is based on
  CompositeFeaturePtr mySketch; ///< Sketch is equivalent to workplane
  ConstraintConstraintMap myConstraints; ///< List of constraints
  std::set<SolverConstraintPtr> myTempConstraints; ///< List of temporary constraints

  StoragePtr myStorage; ///< Container for the set of SolveSpace constraints and their entities
  FeatureStoragePtr myFeatureStorage; ///< Container for the set of SketchPlugin features and their dependencies

  SketchSolver_Solver myConstrSolver;  ///< Solver for set of equations obtained by constraints
};

#endif
