// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        ModelAPI_ResultParameter.h
// Created:     07 Jul 2014
// Author:      Vitaly SMETANNIKOV

#include "ModelAPI_ResultParameter.h"
#include <ModelAPI_AttributeDouble.h>

void ModelAPI_ResultParameter::initAttributes()
{
  data()->addAttribute(ModelAPI_ResultParameter::VALUE(),
                       ModelAPI_AttributeDouble::typeId());
}
