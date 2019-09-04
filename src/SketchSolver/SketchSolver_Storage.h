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

#ifndef SketchSolver_Storage_H_
#define SketchSolver_Storage_H_

#include <PlaneGCSSolver_ConstraintWrapper.h>
#include <PlaneGCSSolver_EntityWrapper.h>

#include <PlaneGCSSolver_Solver.h>
#include <PlaneGCSSolver_UpdateFeature.h>

#include <ModelAPI_Attribute.h>
#include <ModelAPI_AttributeDouble.h>
#include <ModelAPI_Feature.h>
#include <SketchPlugin_Constraint.h>

class SketchSolver_ConstraintDistance;
class SketchSolver_ConstraintFixedArcRadius;
typedef std::map<EntityWrapperPtr, std::set<EntityWrapperPtr> > CoincidentPointsMap;


/** \class   SketchSolver_Storage
 *  \ingroup Plugins
 *  \brief   Interface to map SketchPlugin features to the entities of corresponding solver.
 */
class SketchSolver_Storage
{
private:
  SketchSolver_Storage(const SketchSolver_Storage&);
  SketchSolver_Storage& operator=(const SketchSolver_Storage&);

public:
  SketchSolver_Storage(SolverPtr theSolver);

  /// \brief Change mapping between constraint from SketchPlugin and
  ///        a constraint applicable for corresponding solver.
  /// \param theConstraint       [in]   original SketchPlugin constraint
  /// \param theSolverConstraint [in]   solver's constraint
  virtual void addConstraint(ConstraintPtr        theConstraint,
                             ConstraintWrapperPtr theSolverConstraint);

  /// \brief Add a movement constraint which will be destroyed
  ///        after the next solving of the set of constraints.
  /// \param theSolverConstraint [in]  solver's constraint
  virtual void addMovementConstraint(const ConstraintWrapperPtr& theSolverConstraint) = 0;

  /// \brief Change mapping feature from SketchPlugin and
  ///        the entity applicable for corresponding solver.
  /// \param theFeature      [in]  original SketchPlugin feature
  /// \param theSolverEntity [in]  solver's entity, created outside
  void addEntity(FeaturePtr       theFeature,
                 EntityWrapperPtr theSolverEntity);

  /// \brief Change mapping attribute of a feature and the entity
  /// applicable for corresponding solver.
  /// \param theAttribute    [in]  original attribute
  /// \param theSolverEntity [in]  solver's entity, created outside
  void addEntity(AttributePtr     theAttribute,
                 EntityWrapperPtr theSolverEntity);

  /// \brief Convert feature to the form applicable for specific solver and map it
  /// \param theFeature [in]  feature to convert
  /// \param theForce   [in]  forced feature creation
  /// \return \c true if the feature has been created or updated
  virtual bool update(FeaturePtr theFeature, bool theForce = false) = 0;

  /// \brief Convert attribute to the form applicable for specific solver and map it
  /// \param theAttribute [in]  attribute to convert
  /// \param theForce     [in]  forced feature creation
  /// \return \c true if the attribute has been created or updated
  virtual bool update(AttributePtr theAttribute, bool theForce = false) = 0;

  /// \brief Returns constraint related to corresponding constraint
  const ConstraintWrapperPtr& constraint(const ConstraintPtr& theConstraint) const;

  /// \brief Returns entity related to corresponding feature
  const EntityWrapperPtr& entity(const FeaturePtr& theFeature) const;
  /// \brief Returns entity related to corresponding attribute
  const EntityWrapperPtr& entity(const AttributePtr& theAttribute) const;

  /// \brief Make entity external
  virtual void makeExternal(const EntityWrapperPtr& theEntity) = 0;
  /// \brief Make entity non-external
  virtual void makeNonExternal(const EntityWrapperPtr& theEntity) = 0;

  /// \brief Removes constraint from the storage
  /// \return \c true if the constraint and all its parameters are removed successfully
  virtual bool removeConstraint(ConstraintPtr theConstraint) = 0;
  /// \brief Removes feature from the storage
  void removeFeature(FeaturePtr theFeature);
  /// \brief Removes attribute from the storage
  void removeAttribute(AttributePtr theAttribute);

  /// \brief Remove all features became invalid
  virtual void removeInvalidEntities() = 0;

  /// \brief Check the features have not been removed
  bool areFeaturesValid() const;

  /// \brief Check the storage has constraints
  virtual bool isEmpty() const
  { return myConstraintMap.empty(); }

  /// \brief Shows the sketch should be resolved
  virtual bool isNeedToResolve()
  { return myNeedToResolve; }
  /// \brief Changes the flag of group to be resolved
  void setNeedToResolve(bool theFlag)
  { myNeedToResolve = theFlag; }

  /// \brief Return list of conflicting constraints
  std::set<ObjectPtr> getConflictingConstraints(SolverPtr theSolver) const;

  /// \brief Verify, the sketch contains degenerated geometry
  ///        after resolving the set of constraints
  /// \return STATUS_OK if the geometry is valid, STATUS_DEGENERATED otherwise.
  virtual PlaneGCSSolver_Solver::SolveStatus checkDegeneratedGeometry() const = 0;

  /// \brief Update SketchPlugin features after resolving constraints
  virtual void refresh() const = 0;

  /// \brief Block or unblock events when refreshing features
  void blockEvents(bool isBlocked);

  /// \brief Subscribe for updates of features
  /// \param theSubscriber [in]  object which wants to revceive notifications
  /// \param theGroup      [in]  group of updates features to be send
  void subscribeUpdates(SketchSolver_Constraint* theSubscriber, const std::string& theGroup) const;
  /// \brief Unsubscribe for updates of features
  /// \param theSubscriber [in]  object which does not want to revceive notifications anymore
  void unsubscribeUpdates(SketchSolver_Constraint* theSubscriber) const;

  /// \brief Notify all subscribers about update of the feature
  void notify(const FeaturePtr& theFeature) const;

  /// \brief Make parametrization of arcs consistent.
  ///        Forward arcs should have the last parameter greater than the first parameter.
  ///        Reversed arcs should have the last parameter lesser than the first parameter.
  virtual void adjustParametrizationOfArcs() = 0;

protected:
  /// \brief Convert result to feature or attribute if theResult is linked to center of circle/arc
  static void resultToFeatureOrAttribute(const ObjectPtr& theResult,
                                         FeaturePtr&      theFeature,
                                         AttributePtr&    theAttribute);

protected:
  SolverPtr mySketchSolver; ///< Sketch solver, prepared in corresponding group
  bool    myNeedToResolve; ///< parameters are changed and group needs to be resolved
  bool    myEventsBlocked; ///< indicates that features do not send events

  /// map SketchPlugin constraint to a list of solver's constraints
  std::map<ConstraintPtr, ConstraintWrapperPtr> myConstraintMap;
  /// map SketchPlugin feature to solver's entity
  std::map<FeaturePtr, EntityWrapperPtr>        myFeatureMap;
  /// map attribute to solver's entity
  std::map<AttributePtr, EntityWrapperPtr>      myAttributeMap;

  UpdaterPtr myUpdaters;
};

typedef std::shared_ptr<SketchSolver_Storage> StoragePtr;

#endif
