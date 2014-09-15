// File:    SketchSolver_ConstraintManager.h
// Created: 08 May 2014
// Author:  Artem ZHIDKOV

#ifndef SketchSolver_ConstraintManager_H_
#define SketchSolver_ConstraintManager_H_

#include "SketchSolver.h"
#include <SketchSolver_Solver.h>
#include <SketchSolver_ConstraintGroup.h>

#include <Events_Listener.h>
#include <SketchPlugin_Constraint.h>

#include <string.h>
#include <slvs.h>

#include <list>
#include <map>
#include <vector>
#include <set>

/** \class   SketchSolver_ConstraintManager
 *  \ingroup DataModel
 *  \brief   Listens the changes of SketchPlugin features and transforms the Constraint
 *           feature into the format understandable by SolveSpace library.
 *
 *  Constraints created for SolveSpace library are divided into the groups.
 *  The division order based on connectedness of the features by the constraints.
 *  The groups may be fused or separated according to the new constraints.
 *
 *  \remark This is a singleton.
 */
class SketchSolver_ConstraintManager : public Events_Listener
{
 public:
  /** \brief Main method to create constraint manager
   *  \return pointer to the singleton
   */
  static SketchSolver_ConstraintManager* Instance();

  /** \brief Implementation of Event Listener method
   *  \param[in] theMessage the data of the event
   */
  virtual void processEvent(const boost::shared_ptr<Events_Message>& theMessage);

 protected:
  SketchSolver_ConstraintManager();
  ~SketchSolver_ConstraintManager();

  /** \brief Adds or updates a constraint in the suitable group
   *  \param[in] theConstraint constraint to be changed
   *  \return \c true if the constraint changed successfully
   */
  bool changeConstraint(boost::shared_ptr<SketchPlugin_Constraint> theConstraint);

  /** \brief Removes a constraint from the manager
   *  \param[in] theConstraint constraint to be removed
   *  \return \c true if the constraint removed successfully
   */
  bool removeConstraint(boost::shared_ptr<SketchPlugin_Constraint> theConstraint);

  /** \brief Adds or updates a workplane in the manager
   *  \param[in] theSketch the feature to create or update workplane
   *  \return \c true if the workplane changed successfully
   *  \remark Type of theSketch is not verified inside
   */
  bool changeWorkplane(boost::shared_ptr<SketchPlugin_Feature> theSketch);

  /** \brief Removes a workplane from the manager.
   *         All groups based on such workplane will be removed too.
   *  \param[in] theSketch the feature to be removed
   *  \return \c true if the workplane removed successfully
   */
  bool removeWorkplane(boost::shared_ptr<SketchPlugin_Sketch> theSketch);

  /** \brief Updates entity which is neither workplane nor constraint
   *  \param[in] theFeature entity to be updated
   */
  void updateEntity(boost::shared_ptr<SketchPlugin_Feature> theFeature);

  /** \brief Goes through the list of groups and solve the constraints
   */
  void resolveConstraints();

 private:
  /** \brief Searches list of groups which interact with specified constraint
   *  \param[in]  theConstraint constraint to be found
   *  \param[out] theGroups     list of group indexes interacted with constraint
   */
  void findGroups(boost::shared_ptr<SketchPlugin_Constraint> theConstraint,
                  std::set<Slvs_hGroup>& theGroupIDs) const;

  /** \brief Searches in the list of groups the workplane which constains specified constraint
   *  \param[in] theConstraint constraint to be found
   *  \return workplane containing the constraint
   */
  boost::shared_ptr<SketchPlugin_Feature> findWorkplaneForConstraint(
      boost::shared_ptr<SketchPlugin_Constraint> theConstraint) const;

 private:
  static SketchSolver_ConstraintManager* _self;  ///< Self pointer to implement singleton functionality
  std::vector<SketchSolver_ConstraintGroup*> myGroups;  ///< Groups of constraints
};

#endif
