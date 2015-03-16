// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:    SketchPlugin_ConstraintHorizontal.cpp
// Created: 16 Mar 2015
// Author:  Artem ZHIDKOV

#include "SketchPlugin_ConstraintHorizontal.h"

#include <ModelAPI_AttributeDouble.h>
#include <ModelAPI_Data.h>
#include <ModelAPI_ResultConstruction.h>

#include <SketchPlugin_Line.h>
#include <SketchPlugin_Sketch.h>

#include <SketcherPrs_Factory.h>

#include <Config_PropManager.h>

SketchPlugin_ConstraintHorizontal::SketchPlugin_ConstraintHorizontal()
{
}

void SketchPlugin_ConstraintHorizontal::initAttributes()
{
  data()->addAttribute(SketchPlugin_Constraint::ENTITY_A(), ModelAPI_AttributeRefAttr::type());
}

void SketchPlugin_ConstraintHorizontal::execute()
{
}

AISObjectPtr SketchPlugin_ConstraintHorizontal::getAISObject(AISObjectPtr thePrevious)
{
  if (!sketch())
    return thePrevious;

  AISObjectPtr anAIS = thePrevious;
  if (!anAIS) {
    anAIS = SketcherPrs_Factory::horisontalConstraint(this, sketch()->coordinatePlane());
  }
  return anAIS;
}


