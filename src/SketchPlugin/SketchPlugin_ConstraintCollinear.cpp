// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:    SketchPlugin_ConstraintCollinear.cpp
// Created: 26 May 2014
// Author:  Artem ZHIDKOV

#include "SketchPlugin_ConstraintCollinear.h"

SketchPlugin_ConstraintCollinear::SketchPlugin_ConstraintCollinear()
{
}

void SketchPlugin_ConstraintCollinear::initAttributes()
{
  data()->addAttribute(SketchPlugin_Constraint::ENTITY_A(), ModelAPI_AttributeRefAttr::typeId());
  data()->addAttribute(SketchPlugin_Constraint::ENTITY_B(), ModelAPI_AttributeRefAttr::typeId());
}

void SketchPlugin_ConstraintCollinear::execute()
{
}

AISObjectPtr SketchPlugin_ConstraintCollinear::getAISObject(AISObjectPtr thePrevious)
{
  if (!sketch())
    return thePrevious;

  AISObjectPtr anAIS = thePrevious;
  if (!anAIS) {
    // TODO
    //anAIS = SketcherPrs_Factory::collinearConstraint(this, sketch()->coordinatePlane());
  }
  return anAIS;
}


