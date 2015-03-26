// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:        ParametersPlugin_Parameter.cpp
// Created:     23 MArch 2015
// Author:      sbh

#include "ParametersPlugin_Parameter.h"
#include <ModelAPI_AttributeString.h>

ParametersPlugin_Parameter::ParametersPlugin_Parameter()
{
}

void ParametersPlugin_Parameter::initAttributes()
{
  data()->addAttribute(ParametersPlugin_Parameter::VARIABLE_ID(), ModelAPI_AttributeString::typeId());
  data()->addAttribute(ParametersPlugin_Parameter::EXPRESSION_ID(), ModelAPI_AttributeString::typeId());
}

void ParametersPlugin_Parameter::execute()
{
}
