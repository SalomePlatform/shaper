// Copyright (C) 2014-20xx CEA/DEN, EDF R&D
// Name   : ModelHighAPI_Integer.cpp
// Purpose: 
//
// History:
// 29/03/16 - Sergey POKHODENKO - Creation of the file

//--------------------------------------------------------------------------------------
#include "ModelHighAPI_Integer.h"

#include <ModelAPI_AttributeInteger.h>
//--------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------
ModelHighAPI_Integer::ModelHighAPI_Integer(int theValue)
: myVariantType(VT_INT)
, myInt(theValue)
{
}

ModelHighAPI_Integer::ModelHighAPI_Integer(const std::string & theValue)
: myVariantType(VT_STRING)
, myString(theValue)
{
}

ModelHighAPI_Integer::ModelHighAPI_Integer(const char * theValue)
: myVariantType(VT_STRING)
, myString(theValue)
{
}

ModelHighAPI_Integer::~ModelHighAPI_Integer()
{
}

//--------------------------------------------------------------------------------------
void ModelHighAPI_Integer::fillAttribute(
    const std::shared_ptr<ModelAPI_AttributeInteger> & theAttribute) const
{
  switch(myVariantType) {
    case VT_INT: theAttribute->setValue(myInt); return;
    case VT_STRING: theAttribute->setText(myString); return;
  }
}
