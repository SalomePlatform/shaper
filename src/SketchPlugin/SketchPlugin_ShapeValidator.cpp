// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:        Model_ResultValidators.cpp
// Created:     27 Feb 2015
// Author:      Natalia ERMOLAEVA

#include "SketchPlugin_ShapeValidator.h"
#include "SketchPlugin_Feature.h"

#include <ModelAPI_Session.h>
#include <ModelAPI_Result.h>
#include <ModelAPI_Tools.h>
#include <ModelAPI_AttributeRefAttr.h>
#include <ModelAPI_ResultValidator.h>

bool SketchPlugin_ShapeValidator::isValid(const AttributePtr& theAttribute,
                                          const std::list<std::string>& theArguments) const
{
  if (theArguments.size() != 1)
    return true;

  // ask whether the feature of the attribute is external
  bool isAttributeExternal = isExternalAttribute(theAttribute);

  // ask whether the feature of the attribute by parameter identifier is external
  std::string aFrontArgument = theArguments.front();
  FeaturePtr aFeature = std::dynamic_pointer_cast<ModelAPI_Feature>(theAttribute->owner());
  bool isParameterExternal = isExternalAttribute(aFeature->attribute(aFrontArgument));

  // it is not possible that both features, attribute and attribute in parameter, are external
  if (isAttributeExternal && isParameterExternal)
    return false;
  return true;
}

bool SketchPlugin_ShapeValidator::isExternalAttribute(const AttributePtr& theAttribute) const
{
  bool isExternal = false;
  AttributeRefAttrPtr anAttribute = std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(theAttribute);

  if (anAttribute.get() != NULL) {
    FeaturePtr anArgumentFeature = ModelAPI_Feature::feature(anAttribute->object());
    if (anArgumentFeature.get() != NULL) {
      std::shared_ptr<SketchPlugin_Feature> aSketchFeature =
                                    std::dynamic_pointer_cast<SketchPlugin_Feature>(anArgumentFeature);
      if (aSketchFeature.get() != NULL) {
        isExternal = aSketchFeature->isExternal();
      }
    }
  }
  return isExternal;
}
