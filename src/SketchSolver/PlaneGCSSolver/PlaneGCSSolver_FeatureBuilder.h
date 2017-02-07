// Copyright (C) 2017-20xx CEA/DEN, EDF R&D

// File:    PlaneGCSSolver_FeatureBuilder.h
// Created: 10 Feb 2017
// Author:  Artem ZHIDKOV

#ifndef PlaneGCSSolver_FeatureBuilder_H_
#define PlaneGCSSolver_FeatureBuilder_H_

#include <PlaneGCSSolver_AttributeBuilder.h>

typedef std::map<AttributePtr, EntityWrapperPtr> AttributeEntityMap;

/** \class   PlaneGCSSolver_FeatureBuilder
 *  \ingroup Plugins
 *  \brief   Converts SketchPlugin_Feature to the entity applicable for PlaneGCS solver
 */
class PlaneGCSSolver_FeatureBuilder : public PlaneGCSSolver_AttributeBuilder
{
public:
  PlaneGCSSolver_FeatureBuilder(PlaneGCSSolver_Storage* theStorage = 0);
  PlaneGCSSolver_FeatureBuilder(const StoragePtr& theStorage);

  /// \brief Converts an attribute to the solver's entity and stores it for further processing.
  ///        Double attributes and 2D points are supported only.
  /// \param theAttribute [in]  attribute to create
  /// \return Created wrapper of the attribute applicable for specific solver
  virtual EntityWrapperPtr createAttribute(AttributePtr theAttribute) override;

  /// \brief Converts SketchPlugin's feature to the solver's entity.
  ///        Result if based on the list of already converted attributes.
  /// \param theFeature [in]  feature to create
  virtual EntityWrapperPtr createFeature(FeaturePtr theFeature) override;

  /// \brief Return list of constraints necessary to fix feature's extra DoF
  virtual const std::list<GCSConstraintPtr>& constraints() const override
  { return myFeatureConstraints; }

private:
  /// list of converted attributes (will be cleared when the feature is created)
  AttributeEntityMap myAttributes;

  /// constraints for the feature
  /// (primarily used for constrain arcs, which have 9 parameters but 5 DoF)
  std::list<GCSConstraintPtr> myFeatureConstraints;
};

#endif
