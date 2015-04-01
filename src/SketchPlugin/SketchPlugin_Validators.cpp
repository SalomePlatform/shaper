// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:        SketchPlugin_Validators.cpp
// Created:     01 Aug 2014
// Author:      Vitaly SMETANNIKOV

#include "SketchPlugin_Validators.h"
#include "SketchPlugin_ConstraintDistance.h"
#include "SketchPlugin_ConstraintCoincidence.h"
#include "SketchPlugin_ConstraintRigid.h"
#include "SketchPlugin_Line.h"
#include "SketchPlugin_Arc.h"

#include "SketcherPrs_Tools.h"

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
    std::shared_ptr<GeomDataAPI_Point2D> aPoint = SketcherPrs_Tools::getFeaturePoint(aFeature->data(), aParamA);
    if (aPoint)
      return true;
  }
  return false;
}
bool SketchPlugin_TangentAttrValidator::isValid(
  const AttributePtr& theAttribute, const std::list<std::string>& theArguments ) const
{
  // there is a check whether the feature contains a point and a linear edge or two point values
  std::string aParamA = theArguments.front();
  SessionPtr aMgr = ModelAPI_Session::get();
  ModelAPI_ValidatorsFactory* aFactory = aMgr->validators();

  FeaturePtr aFeature = std::dynamic_pointer_cast<ModelAPI_Feature>(theAttribute->owner());
  AttributeRefAttrPtr aRefAttr = std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(theAttribute);
  if (!aRefAttr)
    return false;

  bool isObject = aRefAttr->isObject();
  ObjectPtr anObject = aRefAttr->object();
  if (isObject && anObject) {
    FeaturePtr aRefFea = ModelAPI_Feature::feature(anObject);

    AttributeRefAttrPtr aOtherAttr = aFeature->data()->refattr(aParamA);
    ObjectPtr aOtherObject = aOtherAttr->object();
    FeaturePtr aOtherFea = ModelAPI_Feature::feature(aOtherObject);

    if (aRefFea->getKind() == SketchPlugin_Line::ID()) {
      if (aOtherFea->getKind() != SketchPlugin_Arc::ID())
        return false;
    } else if (aRefFea->getKind() == SketchPlugin_Arc::ID()) {
      if (aOtherFea->getKind() != SketchPlugin_Line::ID())
        return false;
    } else
      return false;

    // check that both have coincidence
    FeaturePtr aConstrFeature;
    std::set<FeaturePtr> aCoinList;
    const std::set<std::shared_ptr<ModelAPI_Attribute>>& aRefsList = aRefFea->data()->refsToMe();
    std::set<std::shared_ptr<ModelAPI_Attribute>>::const_iterator aIt;
    for (aIt = aRefsList.cbegin(); aIt != aRefsList.cend(); ++aIt) {
      std::shared_ptr<ModelAPI_Attribute> aAttr = (*aIt);
      aConstrFeature = std::dynamic_pointer_cast<ModelAPI_Feature>(aAttr->owner());
      if (aConstrFeature->getKind() == SketchPlugin_ConstraintCoincidence::ID()) {
        AttributeRefAttrPtr aRAttr = std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(aAttr);
        AttributePtr aAR = aRAttr->attr();
        if (aAR->id() != SketchPlugin_Arc::CENTER_ID()) // ignore constraint to center of arc
          aCoinList.insert(aConstrFeature);
      }
    }
    // if there is no coincidence then it is not valid
    if (aCoinList.size() == 0)
      return false;

    // find that coincedence is the same
    const std::set<std::shared_ptr<ModelAPI_Attribute>>& aOtherList = aOtherFea->data()->refsToMe();
    std::set<FeaturePtr>::const_iterator aCoinsIt;
    for (aIt = aOtherList.cbegin(); aIt != aOtherList.cend(); ++aIt) {
      std::shared_ptr<ModelAPI_Attribute> aAttr = (*aIt);
      aConstrFeature = std::dynamic_pointer_cast<ModelAPI_Feature>(aAttr->owner());
      aCoinsIt = aCoinList.find(aConstrFeature);
      if (aCoinsIt != aCoinList.end()) {
        AttributeRefAttrPtr aRAttr = std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(aAttr);
        AttributePtr aAR = aRAttr->attr();
        if (aAR->id() != SketchPlugin_Arc::CENTER_ID())
          return true;
      }
    }
  }
  return false;
}

bool SketchPlugin_NotFixedValidator::isValid(
    const AttributePtr& theAttribute, const std::list<std::string>& theArguments) const
{
  std::shared_ptr<SketchPlugin_Feature> aFeature =
      std::dynamic_pointer_cast<SketchPlugin_Feature>(theAttribute->owner());
  if (!aFeature)
    return true;

  AttributeRefAttrPtr aRefAttr = std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(theAttribute);
  if (!aRefAttr)
    return false;

  SketchPlugin_Sketch* aSketch = aFeature->sketch();
  int aNbFeatures = aSketch->numberOfSubs();
  for (int anInd = 0; anInd < aNbFeatures; anInd++) {
    FeaturePtr aSubFeature = aSketch->subFeature(anInd);
    if (aSubFeature->getKind() != SketchPlugin_ConstraintRigid::ID() || aSubFeature == aFeature)
      continue;
    AttributeRefAttrPtr aRAttr = std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(
        aSubFeature->attribute(SketchPlugin_ConstraintRigid::ENTITY_A()));
    if (aRefAttr->isObject()) {
      if (aRefAttr->object() == aRAttr->object())
        return false;
    } else if (aRefAttr->attr() == aRAttr->attr())
      return false;
  }
  return true;
}

