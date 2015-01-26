// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        ModelAPI_RefAttrValidator.h
// Created:     5 Aug 2014
// Author:      Vitaly SMETANNIKOV

#ifndef ModelAPI_RefAttrValidator_H
#define ModelAPI_RefAttrValidator_H

#include <ModelAPI_Feature.h>
#include <ModelAPI_Object.h>
#include <ModelAPI_Attribute.h>
#include <ModelAPI_AttributeValidator.h>

/**\class ModelAPI_RefAttrValidator
 * \ingroup DataModel
 * Used for filtering out the object that can be used for reference attribute value
 */
class ModelAPI_RefAttrValidator : public ModelAPI_AttributeValidator
{
public:
  //! Returns true if object is good for the feature attribute
  virtual bool isValid(const FeaturePtr& theFeature, const std::list<std::string>& theArguments,
                       const ObjectPtr& theObject) const = 0;

  //! Returns true if the attribute is good for the feature attribute
  virtual bool isValid(const FeaturePtr& theFeature, const std::list<std::string>& theArguments,
                       const AttributePtr& theAttribute) const = 0;

};

#endif
