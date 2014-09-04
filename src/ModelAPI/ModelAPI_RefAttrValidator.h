// File:        ModelAPI_RefAttrValidator.h
// Created:     5 Aug 2014
// Author:      Vitaly SMETANNIKOV

#ifndef ModelAPI_RefAttrValidator_H
#define ModelAPI_RefAttrValidator_H

#include <ModelAPI_Feature.h>
#include <ModelAPI_Object.h>
#include <ModelAPI_AttributeValidator.h>

/*
 * Used for filtering out the object that can be used for reference attribute value
 */
class ModelAPI_RefAttrValidator : public ModelAPI_AttributeValidator
{
public:
  //! Returns true if object is good for the feature attribute
  virtual bool isValid(const FeaturePtr& theFeature, const std::list<std::string>& theArguments,
                       const ObjectPtr& theObject) const = 0;
};

#endif
