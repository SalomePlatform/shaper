// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:        SketchPlugin_Validators.h
// Created:     01 Aug 2014
// Author:      Vitaly SMETANNIKOV

#ifndef SketchPlugin_Validators_H
#define SketchPlugin_Validators_H

#include "SketchPlugin.h"
#include <ModelAPI_RefAttrValidator.h>

class SketchPlugin_DistanceAttrValidator : public ModelAPI_RefAttrValidator
{
 public:
  //! returns true if attribute is valid
  //! \param theAttribute the checked attribute
  //! \param theArguments arguments of the attribute
  virtual bool isValid(
    const AttributePtr& theAttribute, const std::list<std::string>& theArguments) const;
  //! Returns true if object is good for the feature attribute
  virtual bool isValid(const FeaturePtr& theFeature, const std::list<std::string>& theArguments,
                       const ObjectPtr& theObject) const;

  //! Returns true if the attribute is good for the feature attribute
  virtual bool isValid(const FeaturePtr& theFeature, const std::list<std::string>& theArguments,
                       const AttributePtr& theAttribute) const;
};

/**
 * Check that there is no same object was already selected in the feature.
 * For an example: to avoid perpendicularity on line and the same line.
 */
class SketchPlugin_DifferentObjectsValidator : public ModelAPI_RefAttrValidator
{
 public:
  //! returns true if attribute is valid
  //! \param theAttribute the checked attribute
  //! \param theArguments arguments of the attribute
  virtual bool isValid(
    const AttributePtr& theAttribute, const std::list<std::string>& theArguments) const;
  //! Returns true if object is good for the feature attribute
  virtual bool isValid(const FeaturePtr& theFeature, const std::list<std::string>& theArguments,
                       const ObjectPtr& theObject) const;
  //! Returns true if the attribute is good for the feature attribute
  virtual bool isValid(const FeaturePtr& theFeature, const std::list<std::string>& theArguments,
                       const AttributePtr& theAttribute) const;
};

#endif
