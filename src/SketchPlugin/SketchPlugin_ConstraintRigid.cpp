// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:    SketchPlugin_ConstraintRigid.cpp
// Created: 13 Oct 2014
// Author:  Artem ZHIDKOV

#include "SketchPlugin_ConstraintRigid.h"
#include "SketchPlugin_ConstraintParallel.h"

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
  data()->addAttribute(SketchPlugin_Constraint::ENTITY_A(), ModelAPI_AttributeRefAttr::type());
}

void SketchPlugin_ConstraintRigid::execute()
{
}

AISObjectPtr SketchPlugin_ConstraintRigid::getAISObject(AISObjectPtr thePrevious)
{
  if (!sketch())
    return thePrevious;

  AISObjectPtr anAIS = thePrevious;
  if (!anAIS) {
    anAIS = SketcherPrs_Factory::rigidConstraint(this, sketch()->coordinatePlane());
  }

  return anAIS;
}