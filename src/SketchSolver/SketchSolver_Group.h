// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:    SketchSolver_Group.h
// Created: 27 May 2014
// Author:  Artem ZHIDKOV

#ifndef SketchSolver_Group_H_
#define SketchSolver_Group_H_

#include <SketchSolver_Constraint.h>
#include <SketchSolver_Storage.h>

#include <PlaneGCSSolver_Solver.h>

#include <SketchPlugin_Constraint.h>

#include <memory>
#include <map>

typedef std::map<ConstraintPtr, SolverConstraintPtr> ConstraintConstraintMap;

/** \class   SketchSolver_Group
 *  \ingroup Plugins
 *  \brief   Keeps the group of constraints which placed in the same sketch
 */
class SketchSolver_Group
{
 public:
  /** \brief New group based on specified workplane.
   *         Throws an exception if theWorkplane is not an object of SketchPlugin_Sketch type
   *  \remark Type of theSketch is not verified inside
   */
  SketchSolver_Group(const CompositeFeaturePtr& theWorkplane);

  virtual ~SketchSolver_Group();

  /// \brief Returns true if the group has no constraints yet
  inline bool isEmpty() const
  {
    return myConstraints.empty() && myTempConstraints.empty();
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
  bool updateFeature(FeaturePtr theFeature);

  /** \brief Updates the data corresponding the specified feature moved in GUI.
   *         Additional Fixed constraints are created.
   *  \param[in] theFeature the feature to be updated
   *  \return \c true, if the feature is moved
   */
  bool moveFeature(FeaturePtr theFeature);

  /// Returns the current workplane
  inline const CompositeFeaturePtr& getWorkplane() const
  {
    return mySketch;
  }

  /** \brief Searches invalid features and constraints in the group and removes them
   *  \return \c false if the group several constraints were removed
   */
  void repairConsistency();

  /** \brief Start solution procedure if necessary and update attributes of features
   *  \return \c false when no need to solve constraints
   */
  bool resolveConstraints();

  /// \brief Block or unblock events sent by features in this group
  void blockEvents(bool isBlocked);

private:
  /// \biref Verify constraints have not been removed
  bool areConstraintsValid() const;

  /** \brief Removes constraints from the group
   *  \param[in] theConstraint constraint to be removed
   */
  void removeConstraint(ConstraintPtr theConstraint);

  /// \brief Remove all temporary constraints after the computation finished
  void removeTemporaryConstraints();

  /// \brief Append given constraint to the group of temporary constraints
  void setTemporary(SolverConstraintPtr theConstraint);

  /// \brief Compute DoF of the sketch and set corresponding field
  void computeDoF();

private:
  CompositeFeaturePtr mySketch; ///< Sketch for this group
  ConstraintConstraintMap myConstraints; ///< List of constraints
  std::set<SolverConstraintPtr> myTempConstraints; ///< List of temporary constraints

  StoragePtr myStorage; ///< Container for the set of SolveSpace constraints and their entities
  SolverPtr mySketchSolver;  ///< Solver for set of equations obtained by constraints

  /// Result of previous solution of the set of constraints
  PlaneGCSSolver_Solver::SolveStatus myPrevResult;
  std::set<ObjectPtr>      myConflictingConstraints; ///< List of conflicting constraints

  int  myDOF; ///< degrees of freedom of the current sketch

  bool myIsEventsBlocked; ///< shows the events are blocked for this group

  int myMultiConstraintUpdateStack; ///< depth of the stack updating "Multi" constraints
};

typedef std::shared_ptr<SketchSolver_Group> SketchGroupPtr;

#endif
