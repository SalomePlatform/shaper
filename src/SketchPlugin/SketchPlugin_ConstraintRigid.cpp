// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:    SketchPlugin_ConstraintRigid.cpp
// Created: 13 Oct 2014
// Author:  Artem ZHIDKOV

#include "SketchPlugin_ConstraintRigid.h"
#include "SketchPlugin_ConstraintParallel.h"
#include "SketchPlugin_Feature.h"

#include <SketcherPrs_Factory.h>

#include <ModelAPI_ResultConstruction.h>
#include <Config_PropManager.h>

#include <GeomDataAPI_Point2D.h>
#include <GeomAlgoAPI_PointBuilder.h>

SketchPlugin_ConstraintRigid::SketchPlugin_ConstraintRigid()
{
}

void SketchPlugin_ConstraintRigid::initAttributes()
{
  data()->addAttribute(SketchPlugin_Constraint::ENTITY_A(), ModelAPI_AttributeRefAttr::typeId());
}

void SketchPlugin_ConstraintRigid::execute()
{
}

AISObjectPtr SketchPlugin_ConstraintRigid::getAISObject(AISObjectPtr thePrevious)
{
  if (!sketch())
    return thePrevious;

  AISObjectPtr anAIS = thePrevious;

  bool isValidRigid = false;
  std::shared_ptr<ModelAPI_Data> aData = data();
  AttributeRefAttrPtr anAttr = aData->refattr(SketchPlugin_Constraint::ENTITY_A());
  ObjectPtr aObj = anAttr->object();
  if (aObj.get() != NULL) {
    FeaturePtr aFeature = ModelAPI_Feature::feature(aObj);
    std::shared_ptr<SketchPlugin_Feature> aSkFea = 
      std::dynamic_pointer_cast<SketchPlugin_Feature>(aFeature);
    if (!aSkFea->isExternal()) 
      isValidRigid = true;
  }

  if (isValidRigid)
    anAIS = SketcherPrs_Factory::rigidConstraint(this, sketch()->coordinatePlane(), thePrevious);
  else
    anAIS = AISObjectPtr();

  return anAIS;
}