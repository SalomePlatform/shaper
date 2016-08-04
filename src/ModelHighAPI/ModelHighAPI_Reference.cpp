// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:        FeaturesAPI_Reference.cpp
// Created:     04 August 2016
// Author:      Mikhail Ponikarov

//--------------------------------------------------------------------------------------
#include "ModelHighAPI_Reference.h"

#include <ModelAPI_AttributeReference.h>
#include <ModelAPI_AttributeRefList.h>
#include <ModelAPI_Feature.h>
#include <ModelAPI_Result.h>
#include "ModelHighAPI_Interface.h"
//--------------------------------------------------------------------------------------
ModelHighAPI_Reference::ModelHighAPI_Reference()
{}

ModelHighAPI_Reference::ModelHighAPI_Reference(
    const std::shared_ptr<ModelAPI_Object> & theValue)
: myObject(theValue)
{}

ModelHighAPI_Reference::ModelHighAPI_Reference(
    const std::shared_ptr<ModelHighAPI_Interface> & theValue)
: myObject(std::shared_ptr<ModelAPI_Object>(theValue->defaultResult()))
{
}

ModelHighAPI_Reference::~ModelHighAPI_Reference()
{
}

//--------------------------------------------------------------------------------------
void ModelHighAPI_Reference::fillAttribute(
    const std::shared_ptr<ModelAPI_AttributeReference> & theAttribute) const
{
  theAttribute->setValue(myObject);
}

//--------------------------------------------------------------------------------------
void ModelHighAPI_Reference::appendToList(
    const std::shared_ptr<ModelAPI_AttributeRefList> & theAttribute) const
{
  theAttribute->append(myObject);
}
