// File:        SketchPlugin_Validators.cpp
// Created:     01 Aug 2014
// Author:      Vitaly SMETANNIKOV

#include "SketchPlugin_Validators.h"
#include "SketchPlugin_Constraint.h"
#include "SketchPlugin_Sketch.h"
#include "SketchPlugin_Point.h"
#include "SketchPlugin_Line.h"
#include "SketchPlugin_Circle.h"
#include "SketchPlugin_Arc.h"
#include <ModelAPI_Data.h>


bool isValidType(const std::string& theType)
{
  return (theType == SketchPlugin_Point::ID()) || 
         (theType == SketchPlugin_Circle::ID()) ||
         (theType == SketchPlugin_Arc::ID());
}

bool SketchPlugin_DistanceFeatureValidator::isValid(const FeaturePtr theFeature) const
{
  if (!theFeature)
    return false;
  if (!theFeature->data() || !theFeature->data()->isValid())
    return false;
  boost::shared_ptr<ModelAPI_Data> aData = theFeature->data();

  boost::shared_ptr<ModelAPI_AttributeRefAttr> aRefA =
          boost::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(
          aData->attribute(SketchPlugin_Constraint::ENTITY_A()));

  boost::shared_ptr<ModelAPI_AttributeRefAttr> aRefB =
          boost::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(
          aData->attribute(SketchPlugin_Constraint::ENTITY_B()));

  if (!aRefA || !aRefB)
    return false;

  return true;
/*  FeaturePtr aFetureA = SketchPlugin_Sketch::getFeature(aRefA->object());
  FeaturePtr aFetureB = SketchPlugin_Sketch::getFeature(aRefB->object());
  if (!aFetureA || !aFetureB)
    return false;

  std::string aTypeA = aFetureA->getKind();
  std::string aTypeB = aFetureB->getKind();

  if (aTypeA == SketchPlugin_Line::ID()) {
    return isValidType(aTypeB);
  } else if (aTypeB == SketchPlugin_Line::ID()) {
    return isValidType(aTypeA);
  } else
    return isValidType(aTypeA) && isValidType(aTypeB);
  return false;*/
}
