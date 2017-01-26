// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        Model_AttributeBoolean.cpp
// Created:     2 june 2014
// Author:      Vitaly Smetannikov

#include "Model_AttributeBoolean.h"
#include <ModelAPI_Feature.h>
#include <ModelAPI_Data.h>

void Model_AttributeBoolean::setValue(bool theValue)
{
  Standard_Integer aValue = theValue ? 1 : 0;
  if (!myIsInitialized || myBool->Get() != aValue) {
    if (myBool.IsNull())
      myBool = TDataStd_Integer::Set(myLab, 0);
    myBool->Set(aValue);
    owner()->data()->sendAttributeUpdated(this);
  }
}

bool Model_AttributeBoolean::value()
{
  return myIsInitialized && myBool->Get() == 1;
}

Model_AttributeBoolean::Model_AttributeBoolean(TDF_Label& theLabel)
{
  myLab = theLabel;
  reinit();
}

void Model_AttributeBoolean::reinit()
{
  // check the attribute could be already presented in this doc (after load document)
  myIsInitialized = myLab.FindAttribute(TDataStd_Integer::GetID(), myBool) == Standard_True;
}
