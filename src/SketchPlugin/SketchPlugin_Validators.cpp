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
  if (aLineValidator && (!aLineValidator->isValid(theObject)))
    return true;

  // If it is a line then we have to check that first attribute id not a line
  boost::shared_ptr<GeomDataAPI_Point2D> aPoint = getFeaturePoint(theFeature->data(), aParamA);
  if (aPoint)
    return true;
  return false;
}

bool SketchPlugin_DistanceAttrValidator::isValid(
  const AttributePtr& theAttribute, const std::list<std::string>& theArguments ) const
{
  boost::shared_ptr<ModelAPI_AttributeRefAttr> anAttr = 
    boost::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(theAttribute);
  if (anAttr) {
    const ObjectPtr& anObj = theAttribute->owner();
    const FeaturePtr aFeature = boost::dynamic_pointer_cast<ModelAPI_Feature>(anObj);
    return isValid(aFeature, theArguments, anAttr->object());
  }
  return true; // it may be not reference attribute, in this case, it is OK
}

bool SketchPlugin_DifferentObjectsValidator::isValid(const FeaturePtr& theFeature,
                                                 const std::list<std::string>& theArguments,
                                                 const ObjectPtr& theObject) const
{
  std::list<boost::shared_ptr<ModelAPI_Attribute> > anAttrs = 
    theFeature->data()->attributes(ModelAPI_AttributeRefAttr::type());
  std::list<boost::shared_ptr<ModelAPI_Attribute> >::iterator anAttr = anAttrs.begin();
  for(; anAttr != anAttrs.end(); anAttr++) {
    if (*anAttr) {
      boost::shared_ptr<ModelAPI_AttributeRefAttr> aRef = 
        boost::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(*anAttr);
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
  boost::shared_ptr<ModelAPI_AttributeRefAttr> anOrigAttr = 
    boost::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(theAttribute);
  if (anOrigAttr && anOrigAttr->isObject()) {
    const ObjectPtr& anObj = theAttribute->owner();
    const FeaturePtr aFeature = boost::dynamic_pointer_cast<ModelAPI_Feature>(anObj);

    std::list<boost::shared_ptr<ModelAPI_Attribute> > anAttrs = 
      aFeature->data()->attributes(ModelAPI_AttributeRefAttr::type());
    std::list<boost::shared_ptr<ModelAPI_Attribute> >::iterator anAttr = anAttrs.begin();
    for(; anAttr != anAttrs.end(); anAttr++) {
      if (*anAttr && *anAttr != theAttribute) {
        boost::shared_ptr<ModelAPI_AttributeRefAttr> aRef = 
          boost::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(*anAttr);
        // check the object is already presented
        if (aRef->isObject() && aRef->object() == anOrigAttr->object())
          return false;
      }
    }
  }
  return true;
}
