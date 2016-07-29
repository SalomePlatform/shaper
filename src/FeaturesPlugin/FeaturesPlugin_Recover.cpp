// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:        FeaturesPlugin_Recover.cpp
// Created:     29 Jul 2016
// Author:      Natalia ERMOLAEVA

#include "FeaturesPlugin_Recover.h"

#include <ModelAPI_Data.h>
#include <ModelAPI_Document.h>
#include <ModelAPI_AttributeReference.h>
#include <ModelAPI_AttributeRefList.h>
#include <ModelAPI_Session.h>
#include <ModelAPI_Validator.h>

using namespace std;

FeaturesPlugin_Recover::FeaturesPlugin_Recover()
{
}

void FeaturesPlugin_Recover::initAttributes()
{
  data()->addAttribute(BASE_SHAPE_ID(), ModelAPI_AttributeReference::typeId());
  data()->addAttribute(RECOVERED_ENTITIES(), ModelAPI_AttributeRefList::typeId());

  /// temporary modification for empty list
  ModelAPI_Session::get()->validators()->registerNotObligatory(getKind(), RECOVERED_ENTITIES());
}

void FeaturesPlugin_Recover::execute()
{
}
