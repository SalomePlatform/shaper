// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:    SketchPlugin_ConstraintCoincidence.cpp
// Created: 08 May 2014
// Author:  Artem ZHIDKOV

#include "SketchPlugin_ConstraintCoincidence.h"

#include <SketcherPrs_Factory.h>

#include <ModelAPI_AttributeDouble.h>
#include <ModelAPI_Data.h>
#include <SketchPlugin_Point.h>
#include <GeomDataAPI_Dir.h>
#include <GeomDataAPI_Point.h>

SketchPlugin_ConstraintCoincidence::SketchPlugin_ConstraintCoincidence()
{
}

void SketchPlugin_ConstraintCoincidence::initAttributes()
{
  data()->addAttribute(SketchPlugin_Constraint::ENTITY_A(), ModelAPI_AttributeRefAttr::typeId());
  data()->addAttribute(SketchPlugin_Constraint::ENTITY_B(), ModelAPI_AttributeRefAttr::typeId());
}

void SketchPlugin_ConstraintCoincidence::execute()
{
}

AISObjectPtr SketchPlugin_ConstraintCoincidence::getAISObject(AISObjectPtr thePrevious)
{
  if (!sketch())
    return thePrevious;

  AISObjectPtr anAIS = thePrevious;
  if (!anAIS) {
    anAIS = SketcherPrs_Factory::coincidentConstraint(this, sketch()->coordinatePlane());
  //std::vector<int> aRGB = Config_PropManager::color("Visualization", "sketch_constraint_color",
  //                                                  SKETCH_DIMENSION_COLOR);
  //anAIS->setColor(aRGB[0], aRGB[1], aRGB[2]);
  }
  return anAIS;
}
