// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

#include "SketchPlugin_SketchEntity.h"

#include <ModelAPI_Session.h>
#include <ModelAPI_Validator.h>
#include <ModelAPI_AttributeIntArray.h>

SketchPlugin_SketchEntity::SketchPlugin_SketchEntity()
: SketchPlugin_Feature()
{
}

void SketchPlugin_SketchEntity::initAttributes()
{
  data()->addAttribute(AUXILIARY_ID(), ModelAPI_AttributeBoolean::typeId());
  data()->addAttribute(COPY_ID(), ModelAPI_AttributeBoolean::typeId());
  ModelAPI_Session::get()->validators()->registerNotObligatory(getKind(), AUXILIARY_ID());
  ModelAPI_Session::get()->validators()->registerNotObligatory(getKind(), COPY_ID());
}
