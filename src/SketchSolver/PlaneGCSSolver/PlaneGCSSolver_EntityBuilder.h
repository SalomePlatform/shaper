// Copyright (C) 2017-20xx CEA/DEN, EDF R&D

// File:    PlaneGCSSolver_EntityBuilder.h
// Created: 14 Feb 2017
// Author:  Artem ZHIDKOV

#ifndef PlaneGCSSolver_EntityBuilder_H_
#define PlaneGCSSolver_EntityBuilder_H_

#include <PlaneGCSSolver_Defs.h>
#include <PlaneGCSSolver_Storage.h>

/** \class   PlaneGCSSolver_EntityBuilder
 *  \ingroup Plugins
 *  \brief   API to convert data model entity to the entity applicable for PlaneGCS solver
 */
class PlaneGCSSolver_EntityBuilder
{
public:
  /// \brief Create entity in the given storage.
  ///        If the storage is empty, the entity should not be changed
  ///         while constraint solving. So, it is created out of the storage.
  PlaneGCSSolver_EntityBuilder(PlaneGCSSolver_Storage* theStorage = 0)
    : myStorage(theStorage)
  {}

  virtual ~PlaneGCSSolver_EntityBuilder() {}

  /// \brief Converts an attribute to the solver's entity.
  ///        Double attributes and 2D points are supported only.
  /// \param theAttribute [in]  attribute to create
  /// \return Created wrapper of the attribute applicable for specific solver
  virtual EntityWrapperPtr createAttribute(AttributePtr theAttribute) = 0;

  /// \brief Converts SketchPlugin's feature to the solver's entity.
  ///        Result if based on the list of already converted attributes.
  /// \param theFeature [in]  feature to create
  virtual EntityWrapperPtr createFeature(FeaturePtr) = 0;

protected:
  PlaneGCSSolver_Storage* myStorage;
};

#endif
