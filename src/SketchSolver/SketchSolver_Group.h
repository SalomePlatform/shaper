// Copyright (C) 2014-2019  CEA/DEN, EDF R&D
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

#ifndef SketchSolver_Group_H_
#define SketchSolver_Group_H_

#include <SketchSolver_Constraint.h>
#include <SketchSolver_Storage.h>

#include <PlaneGCSSolver_Solver.h>

#include <SketchPlugin_Constraint.h>

#include <memory>
#include <map>

class GeomAPI_Dir;
class GeomAPI_Pnt;
class GeomAPI_Pnt2d;

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

  /** \brief Updates the sketch feature
   */
  bool updateSketch(CompositeFeaturePtr theSketch);

  /** \brief Updates the data corresponding the specified feature
   *  \param[in] theFeature the feature to be updated
   */
  bool updateFeature(FeaturePtr theFeature);

  /** \brief Updates the data corresponding the specified feature moved in GUI.
   *         Special kind of Fixed constraints is created.
   *  \param[in] theFeature the feature to be updated
   *  \param[in] theFrom    start point of the movement
   *  \param[in] theTo      final point of the movement
   *  \return \c true, if the feature is really moved
   */
  bool moveFeature(FeaturePtr theFeature,
                   const std::shared_ptr<GeomAPI_Pnt2d>& theFrom,
                   const std::shared_ptr<GeomAPI_Pnt2d>& theTo);
  /** \brief Updates the data corresponding the specified point moved in GUI.
   *         Special kind of Fixed constraints is created.
   *  \param[in] thePoint the attribute to be updated
   *  \param[in] theFrom  start point of the movement
   *  \param[in] theTo    final point of the movement
   *  \return \c true, if the attribute is really moved
   */
  bool movePoint(AttributePtr thePoint,
                 const std::shared_ptr<GeomAPI_Pnt2d>& theFrom,
                 const std::shared_ptr<GeomAPI_Pnt2d>& theTo);

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
  std::shared_ptr<GeomAPI_Pnt> mySketchOrigin;
  std::shared_ptr<GeomAPI_Dir> mySketchNormal;
  std::shared_ptr<GeomAPI_Dir> mySketchXDir;

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
