// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:        SamplePanelPlugin_Feature.cpp
// Created:     29 Mar 2015
// Author:      Natalia ERMOLAEVA

#include "SamplePanelPlugin_Feature.h"

#include "ModelAPI_AttributeInteger.h"

SamplePanelPlugin_Feature::SamplePanelPlugin_Feature()
: ModelAPI_Feature()
{
}

void SamplePanelPlugin_Feature::initAttributes()
{
  data()->addAttribute(VALUE_ID(), ModelAPI_AttributeInteger::typeId());
}
