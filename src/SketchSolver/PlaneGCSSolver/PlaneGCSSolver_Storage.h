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

#ifndef PlaneGCSSolver_Storage_H_
#define PlaneGCSSolver_Storage_H_

#include <PlaneGCSSolver_Defs.h>
#include <SketchSolver_Storage.h>

class PlaneGCSSolver_EntityBuilder;

/** \class   PlaneGCSSolver_Storage
 *  \ingroup Plugins
 *  \brief   Contains all necessary data in PlaneGCS format to solve a single group of constraints
 */
class PlaneGCSSolver_Storage : public SketchSolver_Storage
{
public:
  PlaneGCSSolver_Storage(const SolverPtr& theSolver);

// =============   Inherited from SketchSolver_Storage   =============

  /// \brief Change mapping between constraint from SketchPlugin and
  ///        a constraint applicable for corresponding solver.
  /// \param theConstraint       [in]   original SketchPlugin constraint
  /// \param theSolverConstraint [in]   solver's constraint
  virtual void addConstraint(ConstraintPtr        theConstraint,
                             ConstraintWrapperPtr theSolverConstraint);

  /// \brief Add a movement constraint which will be destroyed
  ///        after the next solving of the set of constraints.
  /// \param theSolverConstraint [in]  solver's constraint
  virtual void addMovementConstraint(const ConstraintWrapperPtr& theSolverConstraint);


  /// \brief Convert feature to the form applicable for specific solver and map it
  /// \param theFeature [in]  feature to convert
  /// \param theForce   [in]  forced feature creation
  /// \return \c true if the feature has been created or updated
  virtual bool update(FeaturePtr theFeature, bool theForce = false);

  /// \brief Convert attribute to the form applicable for specific solver and map it
  /// \param theAttribute [in]  attribute to convert
  /// \param theForce     [in]  forced feature creation
  /// \return \c true if the attribute has been created or updated
  virtual bool update(AttributePtr theAttribute, bool theForce = false);

  /// \brief Make entity external
  virtual void makeExternal(const EntityWrapperPtr& theEntity);
  /// \brief Make entity non-external
  virtual void makeNonExternal(const EntityWrapperPtr& theEntity);

  /// \brief Removes constraint from the storage
  /// \return \c true if the constraint and all its parameters are removed successfully
  virtual bool removeConstraint(ConstraintPtr theConstraint);

  /// \brief Verify, the sketch contains degenerated geometry
  ///        after resolving the set of constraints
  /// \return STATUS_OK if the geometry is valid, STATUS_DEGENERATED otherwise.
  virtual PlaneGCSSolver_Solver::SolveStatus checkDegeneratedGeometry() const;

  /// \brief Update SketchPlugin features after resolving constraints
  virtual void refresh() const;

  /// \brief Initialize memory for new solver's parameter
  double* createParameter();
  /// \brief Release memory occupied by parameters
  void removeParameters(const GCS::SET_pD& theParams);

  /// \brief Remove all features became invalid
  virtual void removeInvalidEntities();

  /// \brief Check the storage has constraints
  virtual bool isEmpty() const
  { return SketchSolver_Storage::isEmpty() && myArcConstraintMap.empty(); }

  /// \brief Make parametrization of arcs consistent.
  ///        Forward arcs should have the last parameter greater than the first parameter.
  ///        Reversed arcs should have the last parameter lesser than the first parameter.
  virtual void adjustParametrizationOfArcs();

private:
  /// \brief Convert feature using specified builder.
  EntityWrapperPtr createFeature(const FeaturePtr&             theFeature,
                                 PlaneGCSSolver_EntityBuilder* theBuilder);

  /// \brief Convert attribute using specified builder.
  EntityWrapperPtr createAttribute(const AttributePtr&           theAttribute,
                                   PlaneGCSSolver_EntityBuilder* theBuilder);

  void createArcConstraints(const EntityWrapperPtr& theArc);
  void removeArcConstraints(const EntityWrapperPtr& theArc);

private:
  ConstraintID myConstraintLastID;   ///< identifier of last added constraint

  /// additional constraints for correct processing of the arcs
  std::map<EntityWrapperPtr, ConstraintWrapperPtr> myArcConstraintMap;

  /// list of removed constraints to notify solver
  std::list<GCSConstraintPtr> myRemovedConstraints;
};

#endif
