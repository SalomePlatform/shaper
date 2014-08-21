// File:        SketchPlugin_Validators.cpp
// Created:     01 Aug 2014
// Author:      Vitaly SMETANNIKOV

#include "SketchPlugin_Validators.h"
#include "SketchPlugin_ConstraintDistance.h"
#include <ModelAPI_Data.h>
#include <ModelAPI_Validator.h>
#include <ModelAPI_ResultValidator.h>
#include <GeomDataAPI_Point2D.h>

bool SketchPlugin_DistanceAttrValidator::isValid(const FeaturePtr& theFeature,
                                                 const std::list<std::string>& theArguments,
                                                 const ObjectPtr& theObject) const
{
  std::string aParamA = theArguments.front();
  PluginManagerPtr aMgr = ModelAPI_PluginManager::get();
  ModelAPI_ValidatorsFactory* aFactory = aMgr->validators();

  // If the object is not a line then it is accepted
  const ModelAPI_ResultValidator* aLineValidator =
      dynamic_cast<const ModelAPI_ResultValidator*>(aFactory->validator("Model_ResultLineValidator"));
  if (!aLineValidator->isValid(theObject))
    return true;

  // If it is a line then we have to check that first attribute id not a line
  boost::shared_ptr<GeomDataAPI_Point2D> aPoint = getFeaturePoint(theFeature->data(), aParamA);
  if (aPoint)
    return true;
  return false;
}

