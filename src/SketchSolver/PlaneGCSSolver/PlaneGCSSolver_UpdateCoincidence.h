// Copyright (C) 2017-20xx CEA/DEN, EDF R&D

// File:    PlaneGCSSolver_UpdateCoincidence.h
// Created: 17 Feb 2017
// Author:  Artem ZHIDKOV

#ifndef PlaneGCSSolver_UpdateCoincidence_H_
#define PlaneGCSSolver_UpdateCoincidence_H_

#include <PlaneGCSSolver_Update.h>
#include <SketchSolver_IEntityWrapper.h>

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
  std::list<std::set<EntityWrapperPtr> > myCoincident; ///< list of coincidences
};

#endif
