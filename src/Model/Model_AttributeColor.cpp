// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        Model_AttributeColor.cpp
// Created:     6 Mar 2015
// Author:      Natalia ERMOLAEVA

#include <Model_AttributeColor.h>
#include <ModelAPI_AttributeString.h>
#include <ModelAPI_Attribute.h>
#include <ModelAPI_Data.h>
#include <ModelAPI_Object.h>

#include <Standard_TypeDef.hxx>
#include <TCollection_AsciiString.hxx>
#include <TCollection_ExtendedString.hxx>
#include <TDataStd_Integer.hxx>
#include <TDataStd_Name.hxx>

#include <string>

void Model_AttributeColor::setValues(const int theRed,
                                     const int theGreen,
                                     const int theBlue)
{
  if (!myIsInitialized || myRed->Get() != theRed ||
      myGreen->Get() != theGreen || myBlue->Get() != theBlue) {
    myRed->Set(theRed);
    myGreen->Set(theGreen);
    myBlue->Set(theBlue);

    owner()->data()->sendAttributeUpdated(this);
  }
}

void Model_AttributeColor::setValuesRandom()
{
  setValues(300, 150, 40);
}

void Model_AttributeColor::values(int& theRed, int& theGreen, int& theBlue)
{
  theRed = myRed->Get();
  theGreen = myGreen->Get();
  theBlue = myBlue->Get();
}

Model_AttributeColor::Model_AttributeColor(TDF_Label& theLabel)
{
  // check the attribute could be already presented in this doc (after load document)
  myIsInitialized = theLabel.FindAttribute(TDataStd_Integer::GetID(), myRed) == Standard_True;
  if (!myIsInitialized) {
    // create attribute: not initialized by value yet, just zero
    myRed = TDataStd_Integer::Set(theLabel, 0);
    myGreen = TDataStd_Integer::Set(theLabel, 0);
    myBlue = TDataStd_Integer::Set(theLabel, 0);
  }
}
