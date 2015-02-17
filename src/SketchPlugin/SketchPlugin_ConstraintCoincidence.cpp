// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:    SketchPlugin_ConstraintCoincidence.cpp
// Created: 08 May 2014
// Author:  Artem ZHIDKOV

#include "SketchPlugin_ConstraintCoincidence.h"

#include <ModelAPI_AttributeDouble.h>
#include <ModelAPI_Data.h>
#include <SketchPlugin_Point.h>

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

