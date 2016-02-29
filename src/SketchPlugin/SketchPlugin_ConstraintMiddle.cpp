// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:    SketchPlugin_ConstraintMiddle.cpp
// Created: 26 May 2014
// Author:  Artem ZHIDKOV

#include "SketchPlugin_ConstraintMiddle.h"

#include "SketcherPrs_Factory.h"

SketchPlugin_ConstraintMiddle::SketchPlugin_ConstraintMiddle()
{
}

void SketchPlugin_ConstraintMiddle::initAttributes()
{
  data()->addAttribute(SketchPlugin_Constraint::ENTITY_A(), ModelAPI_AttributeRefAttr::typeId());
  data()->addAttribute(SketchPlugin_Constraint::ENTITY_B(), ModelAPI_AttributeRefAttr::typeId());
}

void SketchPlugin_ConstraintMiddle::execute()
{
}

AISObjectPtr SketchPlugin_ConstraintMiddle::getAISObject(AISObjectPtr thePrevious)
{
  if (!sketch())
    return thePrevious;

  AISObjectPtr anAIS = SketcherPrs_Factory::middleConstraint(this, sketch()->coordinatePlane(),
                                                             thePrevious);
  return anAIS;
}


