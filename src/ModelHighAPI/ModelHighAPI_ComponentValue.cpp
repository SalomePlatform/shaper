// Copyright (C) 2014-20xx CEA/DEN, EDF R&D
// Name   : ModelHighAPI_ComponentValue.cpp
// Purpose:
//
// History:
// 29/03/16 - Sergey POKHODENKO - Creation of the file

//--------------------------------------------------------------------------------------
#include "ModelHighAPI_ComponentValue.h"

#include <ModelAPI_AttributeInteger.h>

#include <sstream>

//--------------------------------------------------------------------------------------
ModelHighAPI_ComponentValue::ModelHighAPI_ComponentValue(const bool theValue)
  : myType(ModelAPI_AttributeTables::BOOLEAN)
{
  // initialize everything since in python there may be problem with correct typification
  myValue.myBool = theValue;
  myValue.myInt = theValue ? 1 : 0;
  myValue.myDouble = theValue ? 1. : 0.;
  myValue.myStr = theValue ? "True" : "False";
}
//--------------------------------------------------------------------------------------
ModelHighAPI_ComponentValue::ModelHighAPI_ComponentValue(const int theValue)
  : myType(ModelAPI_AttributeTables::INTEGER)
{
  // initialize everything since in python there may be problem with correct typification
  myValue.myBool = theValue == 0 ? false : true;
  myValue.myInt = theValue;
  myValue.myDouble = theValue;
  std::ostringstream s;
  s << theValue;
  myValue.myStr = s.str();
}
//--------------------------------------------------------------------------------------
ModelHighAPI_ComponentValue::ModelHighAPI_ComponentValue(const double theValue)
  : myType(ModelAPI_AttributeTables::DOUBLE)
{
  // initialize everything since in python there may be problem with correct typification
  myValue.myBool = theValue == 0. ? false : true;
  myValue.myInt = int(theValue);
  myValue.myDouble = theValue;
  std::ostringstream s;
  s << theValue;
  myValue.myStr = s.str();
}
//--------------------------------------------------------------------------------------
ModelHighAPI_ComponentValue::ModelHighAPI_ComponentValue(const std::string& theValue)
  : myType(ModelAPI_AttributeTables::STRING)
{
  myValue.myBool = (theValue.empty() || theValue == "False" || theValue == "0") ? false : true;

  std::stringstream stream1(theValue);
  myValue.myInt = 0;
  stream1 >> myValue.myInt;
  std::stringstream stream2(theValue);
  myValue.myDouble = 0.;
  stream2 >> myValue.myDouble;

  myValue.myStr = theValue;
}

//--------------------------------------------------------------------------------------
ModelHighAPI_ComponentValue::~ModelHighAPI_ComponentValue()
{
}

//--------------------------------------------------------------------------------------
void ModelHighAPI_ComponentValue::fill(const std::shared_ptr<ModelAPI_AttributeTables>& theAttr,
    const int theTable, const int theColumn, const int theRow) const
{
  theAttr->setValue(myValue, theRow, theColumn, theTable);
}
