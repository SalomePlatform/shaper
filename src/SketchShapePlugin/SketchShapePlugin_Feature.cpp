// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:        SketchShapePlugin_Feature.cpp
// Created:     25 Nov 2015
// Author:      Natalia ERMOLAEVA

#include "SketchShapePlugin_Feature.h"

#include <ModelAPI_AttributeSelectionList.h>
#include <ModelAPI_AttributeBoolean.h>

SketchShapePlugin_Feature::SketchShapePlugin_Feature()
: ModelAPI_Feature()
{
}

void SketchShapePlugin_Feature::initAttributes()
{
  data()->addAttribute(VERTEX_LIST_ID(), ModelAPI_AttributeSelectionList::typeId());
  data()->addAttribute(VERTEX_CHOICE_ID(), ModelAPI_AttributeBoolean::typeId());
}

void SketchShapePlugin_Feature::execute()
{
}

void SketchShapePlugin_Feature::attributeChanged(const std::string& theID)
{
}

