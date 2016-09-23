// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:    SketchSolver_Manager.h
// Created: 08 May 2014
// Author:  Artem ZHIDKOV

#ifndef SketchSolver_Manager_H_
#define SketchSolver_Manager_H_

#include "SketchSolver.h"
#include <SketchSolver_Group.h>
#include <SketchSolver_Builder.h>

#include <Events_Listener.h>
#include <SketchPlugin_Constraint.h>

#include <list>
#include <set>

/** \class   SketchSolver_Manager
 *  \ingroup Plugins
 *  \brief   Listens the changes of SketchPlugin features and transforms the Constraint
 *           feature into the format understandable by SolveSpace library.
 *
 *  Constraints created for SolveSpace library are divided into the groups.
 *  The division order based on connectedness of the features by the constraints.
 *  The groups may be fused or separated according to the new constraints.
 *
 *  \remark This is a singleton.
 */
class SketchSolver_Manager : public Events_Listener
{
public:
  /** \brief Main method to create constraint manager
   *  \return pointer to the singleton
   */
  SKETCHSOLVER_EXPORT static SketchSolver_Manager* instance();

  /** \brief Implementation of Event Listener method
   *  \param[in] theMessage the data of the event
   */
  virtual void processEvent(const std::shared_ptr<Events_Message>& theMessage);

  /**
   * The solver needs all the updated objects are transfered in one group, not one by one.
   * This iscreases performance and avoids problems in resolve of only part of the made updates.
   */
  virtual bool groupMessages();

  /// \brief Initialize builder for solver's data structure entities
  /// \param theBuilder [in]  solver's specific builder
  SKETCHSOLVER_EXPORT void setBuilder(BuilderPtr theBuilder);
  /// \brief Returns the builder specific for the solver
  BuilderPtr builder();

protected:
  SketchSolver_Manager();
  ~SketchSolver_Manager();

  /** \brief Adds or updates a constraint or an entity in the suitable group
   *  \param[in] theFeature sketch feature to be changed
   *  \return \c true if the feature changed successfully
   */
  bool changeFeature(std::shared_ptr<SketchPlugin_Feature> theFeature);

  /** \brief Removes a constraint from the manager
   *  \param[in] theConstraint constraint to be removed
   *  \return \c true if the constraint removed successfully
   */
  bool removeConstraint(std::shared_ptr<SketchPlugin_Constraint> theConstraint);

  /** \brief Adds or updates a workplane in the manager
   *  \param[in] theSketch the feature to create or update workplane
   *  \return \c true if the workplane changed successfully
   *  \remark Type of theSketch is not verified inside
   */
  bool changeWorkplane(CompositeFeaturePtr theSketch);

  /** \brief Removes a workplane from the manager.
   *         All groups based on such workplane will be removed too.
   *  \param[in] theSketch the feature to be removed
   *  \return \c true if the workplane removed successfully
   */
  bool removeWorkplane(std::shared_ptr<SketchPlugin_Sketch> theSketch);

  /** \brief Updates entity which is moved in GUI
   *  \param[in] theFeature entity to be updated
   *  \return \c true, if the entity has been moved
   */
  bool moveEntity(std::shared_ptr<SketchPlugin_Feature> theFeature);

  /** \brief Goes through the list of groups and solve the constraints
   *  \param[in] theGroups  list of groups to be resolved (if empty list, all groups are resolved)
   *  \return \c true, if groups are resolved, and features should be updated (send the Update event)
   */
  bool resolveConstraints(const std::list<SketchSolver_Group*>& theGroups = std::list<SketchSolver_Group*>());

private:
  /** \brief Searches list of groups which interact with specified feature
   *  \param[in]  theFeature  object to be found
   *  \param[out] theGroups   list of group indexes interacted with the feature
   */
  void findGroups(std::shared_ptr<SketchPlugin_Feature> theFeature,
                  std::set<GroupID>& theGroupIDs) const;

  /** \brief Searches in the list of groups the workplane which contains specified feature
   *  \param[in] theFeature object to be found
   *  \return workplane containing the feature
   */
  std::shared_ptr<ModelAPI_CompositeFeature> findWorkplane(
      std::shared_ptr<SketchPlugin_Feature> theFeature) const;

  /// \brief Stop sending the Update event until all features updated
  /// \return \c true, if the last flushed event is Update
  bool stopSendUpdate() const;
  /// \brief Allow to send the Update event
  void allowSendUpdate() const;

  /// \brief If the message shows that any group is repaired after conflicting,
  ///        find other groups on the same sketch, which have conflicts.
  void checkConflictingConstraints(const std::shared_ptr<Events_Message>& theMessage);

  /// \brief Calculate DoF for each sketch and send messages if changed
  void degreesOfFreedom();

private:
  static SketchSolver_Manager*     mySelf;    ///< Self pointer to implement singleton functionality
  std::list<SketchSolver_Group*>   myGroups;  ///< Groups of constraints
  BuilderPtr                       myBuilder; ///< Builder for solver's entities
  /// true if computation is performed and all "updates" are generated by this algo
  /// and needs no recomputation
  bool myIsComputed;

  std::map<CompositeFeaturePtr, int> myDoF; ///< Degree of freedom for corresponding sketch
};

#endif
