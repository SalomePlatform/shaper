// File:        Model_Object.cpp
// Created:     19 May 2014
// Author:      Mikhail PONIKAROV

#include "Model_Object.h"
#include <TCollection_AsciiString.hxx>

boost::shared_ptr<ModelAPI_Feature> Model_Object::featureRef()
{
  return myRef;
}

std::string Model_Object::getName()
{
  return TCollection_AsciiString(myName->Get()).ToCString();
}

void Model_Object::setName(std::string theName)
{
  myName->Set(theName.c_str());
}

Model_Object::Model_Object(boost::shared_ptr<ModelAPI_Feature> theRef,
                           Handle_TDataStd_Name theName)
                           : myRef(theRef), myName(theName)
{
}
