// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:    SketchPlugin_ConstraintFillet.cpp
// Created: 19 Mar 2015
// Author:  Artem ZHIDKOV

#include "SketchPlugin_ConstraintFillet.h"

#include <ModelAPI_AttributeDouble.h>
#include <ModelAPI_Data.h>
#include <ModelAPI_ResultConstruction.h>
#include <ModelAPI_AttributeRefAttr.h>
#include <ModelAPI_AttributeRefList.h>
#include <ModelAPI_Session.h>

#include <SketchPlugin_Arc.h>
#include <SketchPlugin_Sketch.h>

#include <SketcherPrs_Factory.h>

#include <Config_PropManager.h>

SketchPlugin_ConstraintFillet::SketchPlugin_ConstraintFillet()
{
}

void SketchPlugin_ConstraintFillet::initAttributes()
{
  data()->addAttribute(SketchPlugin_Constraint::VALUE(), ModelAPI_AttributeDouble::type());
  data()->addAttribute(SketchPlugin_Constraint::ENTITY_A(), ModelAPI_AttributeRefAttr::type());
  data()->addAttribute(SketchPlugin_Constraint::ENTITY_B(), ModelAPI_AttributeRefAttr::type());
  data()->addAttribute(SketchPlugin_Constraint::ENTITY_C(), ModelAPI_AttributeRefList::type());
  // initialize attribute not applicable for user
  data()->attribute(SketchPlugin_Constraint::ENTITY_C())->setInitialized();
}

void SketchPlugin_ConstraintFillet::execute()
{
  std::shared_ptr<ModelAPI_Data> aData = data();
  // Check the base objects are initialized
  AttributeRefAttrPtr aBaseA = std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(
      aData->attribute(SketchPlugin_Constraint::ENTITY_A()));
  AttributeRefAttrPtr aBaseB = std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(
      aData->attribute(SketchPlugin_Constraint::ENTITY_B()));
  if (!aBaseA->isInitialized() || !aBaseB->isInitialized() ||
      !aBaseA->isObject() || !aBaseB->isObject())
    return;
  // Check the fillet shapes is not initialized yet
  AttributeRefListPtr aRefListOfFillet = std::dynamic_pointer_cast<ModelAPI_AttributeRefList>(
      aData->attribute(SketchPlugin_Constraint::ENTITY_C()));
  if (aRefListOfFillet->size() > 0)
    return;
  // Obtain features for the base objects
  FeaturePtr aFeatureA, aFeatureB;
  ResultConstructionPtr aRC =
      std::dynamic_pointer_cast<ModelAPI_ResultConstruction>(aBaseA->object());
  if (aRC) aFeatureA = aRC->document()->feature(aRC);
  aRC = std::dynamic_pointer_cast<ModelAPI_ResultConstruction>(aBaseB->object());
  if (aRC) aFeatureB = aRC->document()->feature(aRC);
  if (!aFeatureA || !aFeatureB)
    return;

  // Create list of objects composing a fillet
  // copy aFeatureA
  FeaturePtr aNewFeature = sketch()->addFeature(aFeatureA->getKind());
  aFeatureA->data()->copyTo(aNewFeature->data());
  aRefListOfFillet->append(aNewFeature);
  // copy aFeatureB
  aNewFeature = sketch()->addFeature(aFeatureB->getKind());
  aFeatureB->data()->copyTo(aNewFeature->data());
  aRefListOfFillet->append(aNewFeature);
  // create filleting arc
  aNewFeature = sketch()->addFeature(SketchPlugin_Arc::ID());
  aNewFeature->attribute(SketchPlugin_Arc::CENTER_ID())->setInitialized();
  aNewFeature->attribute(SketchPlugin_Arc::START_ID())->setInitialized();
  aNewFeature->attribute(SketchPlugin_Arc::END_ID())->setInitialized();
  aRefListOfFillet->append(aNewFeature);
  aRefListOfFillet->setInitialized();
}

AISObjectPtr SketchPlugin_ConstraintFillet::getAISObject(AISObjectPtr thePrevious)
{
  if (!sketch())
    return thePrevious;

  AISObjectPtr anAIS = thePrevious;
  /// TODO: Equal constraint presentation should be put here
  return anAIS;
}


