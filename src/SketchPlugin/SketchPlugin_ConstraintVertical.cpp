// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:    SketchPlugin_ConstraintVertical.cpp
// Created: 16 Mar 2015
// Author:  Artem ZHIDKOV

#include "SketchPlugin_ConstraintVertical.h"

#include <ModelAPI_AttributeDouble.h>
#include <ModelAPI_Data.h>
#include <ModelAPI_ResultConstruction.h>

#include <SketchPlugin_Line.h>
#include <SketchPlugin_Sketch.h>

#include <SketcherPrs_Factory.h>

#include <Config_PropManager.h>

SketchPlugin_ConstraintVertical::SketchPlugin_ConstraintVertical()
{
}

void SketchPlugin_ConstraintVertical::initAttributes()
{
  data()->addAttribute(SketchPlugin_Constraint::ENTITY_A(), ModelAPI_AttributeRefAttr::type());
}

void SketchPlugin_ConstraintVertical::execute()
{
}

AISObjectPtr SketchPlugin_ConstraintVertical::getAISObject(AISObjectPtr thePrevious)
{
  if (!sketch())
    return thePrevious;

  AISObjectPtr anAIS = thePrevious;
  /// TODO: Horizontal constraint presentation should be put here
  return anAIS;
}


