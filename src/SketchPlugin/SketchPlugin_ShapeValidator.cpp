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

bool SketchPlugin_ShapeValidator::isValid(const FeaturePtr& theFeature,
                                                  const std::list<std::string>& theArguments,
                                                  const ObjectPtr& theObject,
                                                  const AttributePtr& theAttribute,
                                                  const GeomShapePtr& theShape) const
{
  ResultPtr aResult = std::dynamic_pointer_cast<ModelAPI_Result>(theObject);
  std::shared_ptr<GeomAPI_Shape> aShape = ModelAPI_Tools::shape(aResult);

  // if the shapes are equal, that means that the given shape is a result.
  // if the result is selected, the 
  if (aShape->isEqual(theShape))
    return true;

  // found a non-external argument on the feature
  std::list<std::string>::const_iterator anIt = theArguments.begin(), aLast = theArguments.end();
  bool aHasNullParam = false;
  bool aHasNonExternalParams = false;
  for (; anIt != aLast; anIt++) {
    std::string aParamA = *anIt;
    std::shared_ptr<ModelAPI_AttributeRefAttr> anAttr = std::dynamic_pointer_cast<
          ModelAPI_AttributeRefAttr>(theFeature->data()->attribute(aParamA));
    if (anAttr) {
      FeaturePtr anOtherFeature = ModelAPI_Feature::feature(anAttr->object());
      if (anOtherFeature.get() != NULL) {
        std::shared_ptr<SketchPlugin_Feature> aSketchFeature = 
          std::dynamic_pointer_cast<SketchPlugin_Feature>(anOtherFeature);
        if (aSketchFeature) {
          aHasNonExternalParams = !aSketchFeature->isExternal();
        }
      }
      else
        aHasNullParam = true;
    }
  }
  if (aHasNullParam || aHasNonExternalParams)
    return true;
  return false;
}
