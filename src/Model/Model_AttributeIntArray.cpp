// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        Model_AttributeIntArray.cpp
// Created:     6 Mar 2015
// Author:      Natalia ERMOLAEVA

#include <Model_AttributeIntArray.h>
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

int Model_AttributeIntArray::size()
{
  if (myArray.IsNull() || !myArray->IsValid()) {
    // this could be on undo and then redo creation of the attribute
    // in result creation it may be uninitialized
    myIsInitialized = myLab.FindAttribute(TDataStd_IntegerArray::GetID(), myArray) == Standard_True;
  }
  // checking the validity because attribute (as a field) may be presented,
  // but without label: it is undoed
  return (myArray.IsNull() || !myArray->IsValid()) ? 0 : myArray->Length();
}

void Model_AttributeIntArray::setSize(const int theSize)
{
  if (myArray.IsNull() || !myArray->IsValid()) { // create array if it is not done yet
    if (theSize != 0) { // if size is zero, nothing to do (null array means there is no array)
      myArray = TDataStd_IntegerArray::Set(myLab, 0, theSize);
      owner()->data()->sendAttributeUpdated(this);
    }
  } else { // reset the old array
    if (theSize) {
      if (theSize != myArray->Length()) { // old data is not keept, a new array is created
        Handle(TColStd_HArray1OfInteger) aNewArray = new TColStd_HArray1OfInteger(0, theSize - 1);
        myArray->ChangeArray(aNewArray);
        owner()->data()->sendAttributeUpdated(this);
      }
    } else { // size is zero => array must be erased
      if (!myArray.IsNull()) {
        myArray.Nullify();
        myLab.ForgetAttribute(TDataStd_IntegerArray::GetID());
        owner()->data()->sendAttributeUpdated(this);
      }
    }
  }
}

void Model_AttributeIntArray::setValue(const int theIndex,
                                       const int theValue)
{
  if (myArray->Value(theIndex) != theValue) {
    myArray->SetValue(theIndex, theValue);
    owner()->data()->sendAttributeUpdated(this);
  }
}

int Model_AttributeIntArray::value(const int theIndex)
{
  return myArray->Value(theIndex);
}

Model_AttributeIntArray::Model_AttributeIntArray(TDF_Label& theLabel)
{
  myLab = theLabel;
  // check the attribute could be already presented in this doc (after load document)
  myIsInitialized = 
    myLab.FindAttribute(TDataStd_IntegerArray::GetID(), myArray) == Standard_True;
}
