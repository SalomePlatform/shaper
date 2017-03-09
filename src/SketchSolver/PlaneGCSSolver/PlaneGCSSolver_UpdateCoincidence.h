// Copyright (C) 2017-20xx CEA/DEN, EDF R&D

// File:    PlaneGCSSolver_UpdateCoincidence.h
// Created: 17 Feb 2017
// Author:  Artem ZHIDKOV

#ifndef PlaneGCSSolver_UpdateCoincidence_H_
#define PlaneGCSSolver_UpdateCoincidence_H_

#include <PlaneGCSSolver_Update.h>
#include <SketchSolver_IEntityWrapper.h>

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

  /// \brief Verifies the entities are not coincident yet
  /// \return \c true if the entities does not coincident
  bool checkCoincidence(const EntityWrapperPtr& theEntity1, const EntityWrapperPtr& theEntity2);

private:
  /// \brief Container for collecting and operating coincident entities
  class CoincidentEntities
  {
  public:
    CoincidentEntities(const EntityWrapperPtr& theEntity1,
                       const EntityWrapperPtr& theEntity2);

    /// Verify the entity is already in the list
    bool isExist(const EntityWrapperPtr& theEntity) const;
    /// Check the coincidence is not in list yet
    bool isNewCoincidence(const EntityWrapperPtr& theEntityExist,
                          const EntityWrapperPtr& theOtherEntity);
    bool isNewCoincidence(const EntityWrapperPtr& theEntityExist,
                          const CoincidentEntities& theOtherGroup,
                          const EntityWrapperPtr& theEntityInOtherGroup);

  private:
    bool hasExternal() const;

  private:
    /// external entity and set of entities connected to it
    std::map<EntityWrapperPtr, std::set<EntityWrapperPtr> > myExternalAndConnected;
  };

  std::list<CoincidentEntities> myCoincident; ///< list of coincidences
};

#endif
