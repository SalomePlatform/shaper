// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:        SketchPlugin_Validators.cpp
// Created:     01 Aug 2014
// Author:      Vitaly SMETANNIKOV

#include "SketchPlugin_Validators.h"
#include "SketchPlugin_ConstraintDistance.h"
#include <ModelAPI_Data.h>
#include <ModelAPI_Validator.h>
#include <ModelAPI_ResultValidator.h>
#include <ModelAPI_AttributeDouble.h>
#include <ModelAPI_AttributeRefAttr.h>
#include <ModelAPI_Session.h>
#include <GeomDataAPI_Point2D.h>

bool SketchPlugin_DistanceAttrValidator::isValid(const FeaturePtr& theFeature,
                                                 const std::list<std::string>& theArguments,
                                                 const ObjectPtr& theObject) const
{
  std::string aParamA = theArguments.front();
  SessionPtr aMgr = ModelAPI_Session::get();
  ModelAPI_ValidatorsFactory* aFactory = aMgr->validators();

  // If the object is not a line then it is accepted
  const ModelAPI_ResultValidator* aLineValidator =
      dynamic_cast<const ModelAPI_ResultValidator*>(aFactory->validator("SketchPlugin_ResultLine"));
  bool aLineValid = aLineValidator->isValid(theObject);

  const ModelAPI_ResultValidator* anArcValidator =
      dynamic_cast<const ModelAPI_ResultValidator*>(aFactory->validator("SketchPlugin_ResultArc"));
  bool anArcValid = anArcValidator->isValid(theObject);

  if (!aLineValid && !anArcValid)
    return true;

  // If it is a line then we have to check that first attribute id not a line
  std::shared_ptr<GeomDataAPI_Point2D> aPoint = getFeaturePoint(theFeature->data(), aParamA);
  if (aPoint)
    return true;
  return false;
}

bool SketchPlugin_DistanceAttrValidator::isValid(
  const AttributePtr& theAttribute, const std::list<std::string>& theArguments ) const
{
  // any point attribute is acceptable for the distance operation
  return true;
}

bool SketchPlugin_DistanceAttrValidator::isValid(const FeaturePtr& theFeature,
                                                 const std::list<std::string>& theArguments,
                                                 const AttributePtr& theAttribute) const
{
  return isValid(theAttribute, theArguments);
}

bool SketchPlugin_DifferentObjectsValidator::isValid(const FeaturePtr& theFeature,
                                                 const std::list<std::string>& theArguments,
                                                 const ObjectPtr& theObject) const
{
  std::list<std::shared_ptr<ModelAPI_Attribute> > anAttrs = 
    theFeature->data()->attributes(ModelAPI_AttributeRefAttr::type());
  std::list<std::shared_ptr<ModelAPI_Attribute> >::iterator anAttr = anAttrs.begin();
  for(; anAttr != anAttrs.end(); anAttr++) {
    if (*anAttr) {
      std::shared_ptr<ModelAPI_AttributeRefAttr> aRef = 
        std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(*anAttr);
      // check the object is already presented
      if (aRef->isObject() && aRef->object() == theObject)
        return false;
    }
  }
  return true;
}

bool SketchPlugin_DifferentObjectsValidator::isValid(
  const AttributePtr& theAttribute, const std::list<std::string>& theArguments ) const
{
  std::shared_ptr<ModelAPI_AttributeRefAttr> anOrigAttr = 
    std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(theAttribute);
  if (anOrigAttr && anOrigAttr->isObject()) {
    const ObjectPtr& anObj = theAttribute->owner();
    const FeaturePtr aFeature = std::dynamic_pointer_cast<ModelAPI_Feature>(anObj);

    std::list<std::shared_ptr<ModelAPI_Attribute> > anAttrs = 
      aFeature->data()->attributes(ModelAPI_AttributeRefAttr::type());
    std::list<std::shared_ptr<ModelAPI_Attribute> >::iterator anAttr = anAttrs.begin();
    for(; anAttr != anAttrs.end(); anAttr++) {
      if (*anAttr && *anAttr != theAttribute) {
        std::shared_ptr<ModelAPI_AttributeRefAttr> aRef = 
          std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(*anAttr);
        // check the object is already presented
        if (aRef->isObject() && aRef->object() == anOrigAttr->object())
          return false;
      }
    }
  }
  return true;
}

bool SketchPlugin_DifferentObjectsValidator::isValid(const FeaturePtr& theFeature,
  const std::list<std::string>& theArguments, const AttributePtr& theAttribute) const
{
  std::list<std::shared_ptr<ModelAPI_Attribute> > anAttrs = 
    theFeature->data()->attributes(ModelAPI_AttributeRefAttr::type());
  std::list<std::shared_ptr<ModelAPI_Attribute> >::iterator anAttr = anAttrs.begin();
  for(; anAttr != anAttrs.end(); anAttr++) {
    if (*anAttr) {
      std::shared_ptr<ModelAPI_AttributeRefAttr> aRef = 
        std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(*anAttr);
      // check the object is already presented
      if (!aRef->isObject() && aRef->attr() == theAttribute)
        return false;
    }
  }
  return true;
}
