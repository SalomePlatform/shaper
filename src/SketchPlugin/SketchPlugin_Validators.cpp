// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:        SketchPlugin_Validators.cpp
// Created:     01 Aug 2014
// Author:      Vitaly SMETANNIKOV

#include "SketchPlugin_Validators.h"
#include "SketchPlugin_ConstraintDistance.h"
#include <ModelAPI_Data.h>
#include <ModelAPI_Validator.h>
#include <ModelAPI_AttributeDouble.h>
#include <ModelAPI_AttributeRefAttr.h>
#include <ModelAPI_Session.h>

#include <GeomValidators_Edge.h>

#include <GeomDataAPI_Point2D.h>

bool SketchPlugin_DistanceAttrValidator::isValid(
  const AttributePtr& theAttribute, const std::list<std::string>& theArguments ) const
{
  // there is a check whether the feature contains a point and a linear edge or two point values
  std::string aParamA = theArguments.front();
  SessionPtr aMgr = ModelAPI_Session::get();
  ModelAPI_ValidatorsFactory* aFactory = aMgr->validators();

  AttributeRefAttrPtr aRefAttr = std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(theAttribute);
  if (!aRefAttr)
    return false;

  bool isObject = aRefAttr->isObject();
  if (!isObject) {
    // an attribute is a point. A point value is valid always for the distance
    return true;
  } else {
    // 1. check whether the references object is a linear
    ObjectPtr anObject = aRefAttr->object();

    const ModelAPI_AttributeValidator* anEdgeValidator = 
      dynamic_cast<const GeomValidators_Edge*>(aFactory->validator("GeomValidators_Edge"));
    std::list<std::string> anArguments;
    anArguments.push_back("circle");
    bool anEdgeValid = anEdgeValidator->isValid(aRefAttr, anArguments);
    // the circle line is not a valid case
    if (anEdgeValid)
      return false;
      
    anArguments.clear();
    anArguments.push_back("line");
    anEdgeValid = anEdgeValidator->isValid(aRefAttr, anArguments);
    // if the attribute value is not a line, that means it is a vertex. A vertex is always valid
    if (!anEdgeValid)
      return true;

    FeaturePtr aFeature = std::dynamic_pointer_cast<ModelAPI_Feature>(theAttribute->owner());
    // If it is a line then we have to check that first attribute id not a line
    std::shared_ptr<GeomDataAPI_Point2D> aPoint = getFeaturePoint(aFeature->data(), aParamA);
    if (aPoint)
      return true;
  }
  return false;
}
