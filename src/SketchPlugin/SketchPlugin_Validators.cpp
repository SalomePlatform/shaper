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
#include "SketchPlugin_Circle.h"
#include "SketchPlugin_Point.h"

#include "SketcherPrs_Tools.h"

#include <ModelAPI_Data.h>
#include <ModelAPI_Validator.h>
#include <ModelAPI_AttributeDouble.h>
#include <ModelAPI_AttributeRefAttr.h>
#include <ModelAPI_AttributeRefList.h>
#include <ModelAPI_AttributeSelectionList.h>
#include <ModelAPI_Session.h>

#include <GeomValidators_ShapeType.h>

#include <GeomDataAPI_Point2D.h>


bool SketchPlugin_DistanceAttrValidator::isValid(const AttributePtr& theAttribute, 
                                                 const std::list<std::string>& theArguments,
                                                 std::string& theError) const
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

    const ModelAPI_AttributeValidator* aShapeValidator = 
      dynamic_cast<const GeomValidators_ShapeType*>(aFactory->validator("GeomValidators_ShapeType"));
    std::list<std::string> anArguments;
    anArguments.push_back("circle");
    std::string aCircleError;
    bool aShapeValid = aShapeValidator->isValid(aRefAttr, anArguments, aCircleError);
    // the circle line is not a valid case
    if (aShapeValid)
      return false;
      
    anArguments.clear();
    anArguments.push_back("line");
    std::string aLineError;
    aShapeValid = aShapeValidator->isValid(aRefAttr, anArguments, aLineError);
    // if the attribute value is not a line, that means it is a vertex. A vertex is always valid
    if (!aShapeValid)
      return true;

    FeaturePtr aFeature = std::dynamic_pointer_cast<ModelAPI_Feature>(theAttribute->owner());
    // If it is a line then we have to check that first attribute id not a line
    std::shared_ptr<SketchPlugin_Feature> aSFeature =
                            std::dynamic_pointer_cast<SketchPlugin_Feature>(theAttribute->owner());
    SketchPlugin_Sketch* aSketch = aSFeature->sketch();
    std::shared_ptr<GeomAPI_Ax3> aPlane = SketchPlugin_Sketch::plane(aSketch);
    std::shared_ptr<GeomDataAPI_Point2D> aPoint = SketcherPrs_Tools::getFeaturePoint(
                                                               aFeature->data(), aParamA, aPlane);
    if (aPoint)
      return true;
  }
  return false;
}

bool SketchPlugin_TangentAttrValidator::isValid(const AttributePtr& theAttribute, 
                                                const std::list<std::string>& theArguments,
                                                std::string& theError) const
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
      if (aOtherFea->getKind() != SketchPlugin_Line::ID() &&
          aOtherFea->getKind() != SketchPlugin_Arc::ID())
        return false;
    } else
      return false;

    return true;
  }
  return false;
}

bool SketchPlugin_NotFixedValidator::isValid(const AttributePtr& theAttribute, 
                                             const std::list<std::string>& theArguments,
                                             std::string& theError) const
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

bool SketchPlugin_EqualAttrValidator::isValid(const AttributePtr& theAttribute, 
                                              const std::list<std::string>& theArguments,
                                              std::string& theError) const
{
  std::string aParamA = theArguments.front();
  FeaturePtr aFeature = std::dynamic_pointer_cast<ModelAPI_Feature>(theAttribute->owner());
  AttributeRefAttrPtr aRefAttr[2];
  aRefAttr[0] = std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(theAttribute);
  if (!aRefAttr)
    return false;
  aRefAttr[1] = aFeature->data()->refattr(aParamA);

  if (!aRefAttr[0]->isObject() || !aRefAttr[1]->isObject())
    return false;

  int aType[2] = {0, 0}; // types of attributes: 0 - incorrect, 1 - line, 2 - circle, 3 - arc
  std::list<std::string> anArguments;
  for (int i = 0; i < 2; i++) {
    ObjectPtr anObject = aRefAttr[i]->object();
    aFeature = ModelAPI_Feature::feature(anObject);
    if (!aFeature)
      return false;

    if (aFeature->getKind() == SketchPlugin_Line::ID()) {
      aType[i] = 1;
      continue;
    }
    if (aFeature->getKind() == SketchPlugin_Circle::ID()) {
      aType[i] = 2;
      continue;
    }
    if (aFeature->getKind() == SketchPlugin_Arc::ID()) {
      aType[i] = 3;
      continue;
    }
    // wrong type of attribute
    return false;
  }

  if ((aType[0] == 1 && aType[1] == 2) ||
      (aType[0] == 2 && aType[1] == 1))
    return false;
  return true;
}

bool SketchPlugin_MirrorAttrValidator::isValid(const AttributePtr& theAttribute, 
                                               const std::list<std::string>& theArguments,
                                               std::string& theError) const
{
  FeaturePtr aFeature = std::dynamic_pointer_cast<ModelAPI_Feature>(theAttribute->owner());
  AttributeSelectionListPtr aSelAttr = 
    std::dynamic_pointer_cast<ModelAPI_AttributeSelectionList>(theAttribute);
  if (!aSelAttr)
    return false;

  AttributeRefListPtr aRefListOfMirrored = std::dynamic_pointer_cast<ModelAPI_AttributeRefList>(
      aFeature->attribute(SketchPlugin_Constraint::ENTITY_C()));
  std::list<ObjectPtr> aMirroredObjects = aRefListOfMirrored->list();

  for(int anInd = 0; anInd < aSelAttr->size(); anInd++) {
    std::shared_ptr<ModelAPI_AttributeSelection> aSelect = aSelAttr->value(anInd);
    std::list<ObjectPtr>::iterator aMirIter = aMirroredObjects.begin();
    for (; aMirIter != aMirroredObjects.end(); aMirIter++)
      if (aSelect->context() == *aMirIter)
        return false;
  }
  return true;
}


bool SketchPlugin_CoincidenceAttrValidator::isValid(const AttributePtr& theAttribute, 
                                                    const std::list<std::string>& theArguments,
                                                    std::string& theError) const
{
  // there is a check whether the feature contains a point and a linear edge or two point values
  std::string aParamA = theArguments.front();
  SessionPtr aMgr = ModelAPI_Session::get();
  ModelAPI_ValidatorsFactory* aFactory = aMgr->validators();

  FeaturePtr aConstraint = std::dynamic_pointer_cast<ModelAPI_Feature>(theAttribute->owner());
  AttributeRefAttrPtr aRefAttrA = aConstraint->data()->refattr(aParamA);
  if (!aRefAttrA)
    return false;

  AttributeRefAttrPtr aRefAttrB = std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(theAttribute);
  if (!aRefAttrB)
    return false;

  // first attribute is a point, it may coincide with any object
  if (!aRefAttrA->isObject())
    return true;
  else {
    FeaturePtr aFeature = ModelAPI_Feature::feature(aRefAttrA->object());
    if (!aFeature)
      return false;
    if (aFeature->getKind() == SketchPlugin_Point::ID())
      return true;
  }

  // second attribute is a point, it may coincide with any object
  if (!aRefAttrB->isObject())
    return true;
  else {
    FeaturePtr aFeature = ModelAPI_Feature::feature(aRefAttrB->object());
    if (!aFeature)
      return false;
    if (aFeature->getKind() == SketchPlugin_Point::ID())
      return true;
  }

  return false;
}


bool SketchPlugin_CopyValidator::isValid(const AttributePtr& theAttribute, 
                                         const std::list<std::string>& theArguments,
                                         std::string& theError) const
{
  FeaturePtr aFeature = std::dynamic_pointer_cast<ModelAPI_Feature>(theAttribute->owner());
  AttributeSelectionListPtr aSelAttr = 
    std::dynamic_pointer_cast<ModelAPI_AttributeSelectionList>(theAttribute);
  if (!aSelAttr)
    return false;

  AttributeRefListPtr aRefListOfInitial = std::dynamic_pointer_cast<ModelAPI_AttributeRefList>(
      aFeature->attribute(SketchPlugin_Constraint::ENTITY_A()));
  AttributeRefListPtr aRefListOfCopied = std::dynamic_pointer_cast<ModelAPI_AttributeRefList>(
      aFeature->attribute(SketchPlugin_Constraint::ENTITY_B()));
  std::list<ObjectPtr> anInitialObjects = aRefListOfInitial->list();
  std::list<ObjectPtr> aCopiedObjects = aRefListOfCopied->list();

  std::list<ObjectPtr>::iterator anObjIter;
  for(int anInd = 0; anInd < aSelAttr->size(); anInd++) {
    std::shared_ptr<ModelAPI_AttributeSelection> aSelect = aSelAttr->value(anInd);
    anObjIter = anInitialObjects.begin();
    for (; anObjIter != anInitialObjects.end(); anObjIter++)
      if (aSelect->context() == *anObjIter)
        break;
    if (anObjIter != anInitialObjects.end())
      continue;
    anObjIter = aCopiedObjects.begin();
    for (; anObjIter != aCopiedObjects.end(); anObjIter++)
      if (aSelect->context() == *anObjIter)
        return false;
  }
  return true;
}

