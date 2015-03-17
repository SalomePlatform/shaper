// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:    SketchPlugin_ConstraintTangent.cpp
// Created: 16 Mar 2015
// Author:  Artem ZHIDKOV

#include "SketchPlugin_ConstraintTangent.h"

#include <ModelAPI_AttributeDouble.h>
#include <ModelAPI_Data.h>
#include <ModelAPI_ResultConstruction.h>

#include <SketchPlugin_Line.h>
#include <SketchPlugin_Sketch.h>

#include <SketcherPrs_Factory.h>

#include <Config_PropManager.h>

SketchPlugin_ConstraintTangent::SketchPlugin_ConstraintTangent()
{
}

void SketchPlugin_ConstraintTangent::initAttributes()
{
  data()->addAttribute(SketchPlugin_Constraint::ENTITY_A(), ModelAPI_AttributeRefAttr::type());
  data()->addAttribute(SketchPlugin_Constraint::ENTITY_B(), ModelAPI_AttributeRefAttr::type());
}

void SketchPlugin_ConstraintTangent::execute()
{
}

AISObjectPtr SketchPlugin_ConstraintTangent::getAISObject(AISObjectPtr thePrevious)
{
  if (!sketch())
    return thePrevious;

  AISObjectPtr anAIS = thePrevious;
  if (!anAIS) {
    anAIS = SketcherPrs_Factory::tangentConstraint(this, sketch()->coordinatePlane());
  }
  return anAIS;
}


