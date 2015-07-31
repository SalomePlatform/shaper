// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        ModelAPI_ResultBody.cpp
// Created:     07 Jul 2014
// Author:      Mikhail PONIKAROV

#include "ModelAPI_BodyBuilder.h"
#include "ModelAPI_Object.h"

ModelAPI_BodyBuilder::ModelAPI_BodyBuilder(ModelAPI_Object* theOwner)
: myOwner(theOwner)
{
}

std::shared_ptr<ModelAPI_Data> ModelAPI_BodyBuilder::data() const
{
  return myOwner->data();
}

std::shared_ptr<ModelAPI_Document> ModelAPI_BodyBuilder::document() const
{
  return myOwner->document();
}
