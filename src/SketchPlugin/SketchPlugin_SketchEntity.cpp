// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

#include "SketchPlugin_SketchEntity.h"

#include <ModelAPI_Session.h>
#include <ModelAPI_Validator.h>

SketchPlugin_SketchEntity::SketchPlugin_SketchEntity()
: SketchPlugin_Feature()
{
}

void SketchPlugin_SketchEntity::initAttributes()
{
  data()->addAttribute(CONSTRUCTION_ID(), ModelAPI_AttributeBoolean::type());
  ModelAPI_Session::get()->validators()->registerNotObligatory(getKind(), CONSTRUCTION_ID());
}
