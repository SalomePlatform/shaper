// Copyright (C) 2017-20xx CEA/DEN, EDF R&D

// File:    PlaneGCSSolver_EntityDestroyer.h
// Created: 10 Feb 2017
// Author:  Artem ZHIDKOV

#ifndef PlaneGCSSolver_EntityDestroyer_H_
#define PlaneGCSSolver_EntityDestroyer_H_

#include <PlaneGCSSolver_Defs.h>
#include <PlaneGCSSolver_Storage.h>

/** \class   PlaneGCSSolver_EntityDestroyer
 *  \ingroup Plugins
 *  \brief   Collect entities of PlaneGCS solver which should be removed
 */
class PlaneGCSSolver_EntityDestroyer
{
public:
  PlaneGCSSolver_EntityDestroyer() {}

  /// \brief Add entity to remove. Its parameters are stored for further remove from the storage.
  void remove(const EntityWrapperPtr& theEntity);

  /// \brief Return parameters to remove
  const GCS::SET_pD& parametersToRemove()
  { return myParams; }

private:
  GCS::SET_pD myParams; ///< set of parameters to be removed
  GCS::SET_pD myParamsOutOfStorage; ///< set of parameters not kept in the storage
};

#endif
