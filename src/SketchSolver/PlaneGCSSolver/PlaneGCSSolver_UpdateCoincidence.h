// Copyright (C) 2017-20xx CEA/DEN, EDF R&D

// File:    PlaneGCSSolver_UpdateCoincidence.h
// Created: 17 Feb 2017
// Author:  Artem ZHIDKOV

#ifndef PlaneGCSSolver_UpdateCoincidence_H_
#define PlaneGCSSolver_UpdateCoincidence_H_

#include <PlaneGCSSolver_Update.h>
#include <PlaneGCSSolver_EntityWrapper.h>

#include <GCS.h>

#include <map>

/** \class   PlaneGCSSolver_UpdateCoincidence
 *  \ingroup Plugins
 *  \brief   Send events to listeners about changing a constraint
 */
class PlaneGCSSolver_UpdateCoincidence : public PlaneGCSSolver_Update
{
public:
  PlaneGCSSolver_UpdateCoincidence(UpdaterPtr theNext = UpdaterPtr())
    : PlaneGCSSolver_Update(theNext)
  {}

  virtual ~PlaneGCSSolver_UpdateCoincidence() {}

  /// \brief Group of entities, processed by this kind of updater
  static const std::string& GROUP()
  {
    static const std::string TYPE("Coincidence");
    return TYPE;
  }

  /// \brief Attach listener
  /// \param theObserver [in]  object which want to receive notifications
  /// \param theType     [in]  receive notifications about changing objects
  ///                          of theType and their derivatives
  virtual void attach(SketchSolver_Constraint* theObserver, const std::string& theType);

  /// \brief Send notification about update of the feature to all interested
  virtual void update(const FeaturePtr& theFeature);

  /// \brief Set coincidence between two given entities
  /// \return \c true if the entities does not coincident yet
  bool addCoincidence(const EntityWrapperPtr& theEntity1, const EntityWrapperPtr& theEntity2);

  /// \brief Verifies the point is coincident to the feature
  /// \return \c true if the point is on the feature
  bool isPointOnEntity(const EntityWrapperPtr& thePoint, const EntityWrapperPtr& theEntity);

private:
  /// \brief Container for collecting and operating coincident entities
  class CoincidentEntities
  {
  public:
    CoincidentEntities(const EntityWrapperPtr& theEntity1,
                       const EntityWrapperPtr& theEntity2);

    /// Verify the entity is already in the list
    bool isExist(const EntityWrapperPtr& theEntity) const;
    /// Verify the point is already in the list
    bool isExist(const GCS::Point& thePoint) const;

    /// Add entity to group
    bool add(const EntityWrapperPtr& theEntity);

    /// Remove entity from group
    void remove(const EntityWrapperPtr& theEntity);

    /// Merge two groups
    void merge(const CoincidentEntities& theOther);

  private:
    std::set<EntityWrapperPtr> myPoints; ///< coincident points
    std::set<EntityWrapperPtr> myExternalPoints; //< external points coincident to other points
    std::set<EntityWrapperPtr> myFeatures; ///< other entities containing points
  };

  /// \brief Search the group of coincidences containing given entity.
  ///        Searches points only.
  std::list<CoincidentEntities>::iterator findGroupOfCoincidence(const EntityWrapperPtr& theEntity);

  /// \brief Add entity to group of coincidences
  /// \reutrn \c true if the entity is added, thus the coincidence is new
  bool addToGroupOfCoincidence(CoincidentEntities& theGroup, const EntityWrapperPtr& theEntity);

private:
  std::list<CoincidentEntities> myCoincident; ///< list of coincidences
};

#endif
