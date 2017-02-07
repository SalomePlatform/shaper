// Copyright (C) 2017-20xx CEA/DEN, EDF R&D

// File:    PlaneGCSSolver_UpdateFeature.h
// Created: 17 Feb 2017
// Author:  Artem ZHIDKOV

#ifndef PlaneGCSSolver_UpdateFeature_H_
#define PlaneGCSSolver_UpdateFeature_H_

#include <PlaneGCSSolver_Update.h>

/** \class   PlaneGCSSolver_UpdateFeature
 *  \ingroup Plugins
 *  \brief   Send events to listeners about changing a feature
 */
class PlaneGCSSolver_UpdateFeature : public PlaneGCSSolver_Update
{
public:
  PlaneGCSSolver_UpdateFeature(UpdaterPtr theNext = UpdaterPtr())
    : PlaneGCSSolver_Update(theNext)
  {}

  /// \brief Group of entities, processed by this kind of updater
  static const std::string& GROUP()
  {
    static const std::string TYPE("Feature");
    return TYPE;
  }

  /// \brief Attach listener
  /// \param theObserver [in]  object which want to receive notifications
  /// \param theType     [in]  receive notifications about changing objects
  ///                          of theType and their derivatives
  virtual void attach(SketchSolver_Constraint* theObserver, const std::string& theType) override;

  /// \brief Send notification about update of the feature to all interested
  virtual void update(const FeaturePtr& theFeature) override;
};

#endif
