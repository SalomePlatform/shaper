// Copyright (C) 2014-20xx CEA/DEN, EDF R&D
// Name   : ModelHighAPI_Double.cpp
// Purpose: 
//
// History:
// 29/03/16 - Sergey POKHODENKO - Creation of the file

//--------------------------------------------------------------------------------------
#include "ModelHighAPI_Double.h"

#include <ModelAPI_AttributeDouble.h>
//--------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------
ModelHighAPI_Double::ModelHighAPI_Double(double theValue)
: myVariantType(VT_DOUBLE)
, myDouble(theValue)
{
}

ModelHighAPI_Double::ModelHighAPI_Double(const std::string & theValue)
: myVariantType(VT_STRING)
, myString(theValue)
{
}

ModelHighAPI_Double::ModelHighAPI_Double(const char * theValue)
: myVariantType(VT_STRING)
, myString(theValue)
{
}

ModelHighAPI_Double::~ModelHighAPI_Double()
{
}

//--------------------------------------------------------------------------------------
void ModelHighAPI_Double::fillAttribute(
    const std::shared_ptr<ModelAPI_AttributeDouble> & theAttribute) const
{
  switch(myVariantType) {
    case VT_DOUBLE: theAttribute->setValue(myDouble); return;
    case VT_STRING: theAttribute->setText(myString); return;
  }
}
