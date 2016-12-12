// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        Model_AttributeTables.cpp
// Created:     14 Nov 2016
// Author:      Mikhail Ponikarov

#include "Model_AttributeTables.h"

#include <ModelAPI_Data.h>
#include <ModelAPI_Object.h>

#include <TColStd_ListIteratorOfListOfInteger.hxx>
#include <TDataStd_RealArray.hxx>
#include <TDataStd_BooleanArray.hxx>
#include <TDataStd_IntegerArray.hxx>
#include <TDataStd_ExtStringArray.hxx>

#include <TColStd_HArray1OfReal.hxx>
#include <TColStd_HArray1OfByte.hxx>
#include <TColStd_HArray1OfInteger.hxx>
#include <TColStd_HArray1OfExtendedString.hxx>

#include <string>

// returns the array attribute GUID for the given type
#define MY_ARRAY_ID(type) \
((type) == ModelAPI_AttributeTables::DOUBLE ? TDataStd_RealArray::GetID(): \
((type) == ModelAPI_AttributeTables::BOOLEAN ? TDataStd_BooleanArray::GetID(): \
((type) == ModelAPI_AttributeTables::INTEGER? TDataStd_IntegerArray::GetID(): \
TDataStd_ExtStringArray::GetID())))

int Model_AttributeTables::rows()
{
  return myRows;
}

int Model_AttributeTables::columns()
{
  return myCols;
}

int Model_AttributeTables::tables()
{
  return myTables;
}

void Model_AttributeTables::setSize(const int theRows, const int theColumns, const int theTables)
{
  if (theRows != myRows || theColumns != myCols || theTables != myTables) {
    owner()->data()->sendAttributeUpdated(this);
    int aSize = myRows * myCols * myTables;
    int aNewSize = theRows * theColumns * theTables;
    if (aSize == 0 && aNewSize == 0) {
      // nothing to do
    } else if (aNewSize == 0) {
      // remove old array
      myLab.ForgetAttribute(MY_ARRAY_ID(myType));
    } else {
      // prepare new arrays
      Handle(TColStd_HArray1OfReal) anOldDouble, aNewDouble =
        (myType == ModelAPI_AttributeTables::DOUBLE) ?
        new TColStd_HArray1OfReal(0, aNewSize - 1) : Handle(TColStd_HArray1OfReal)();
      Handle(TColStd_HArray1OfByte) anOldBool, aNewBool =
        (myType == ModelAPI_AttributeTables::BOOLEAN) ?
        new TColStd_HArray1OfByte(0, aNewSize - 1) : Handle(TColStd_HArray1OfByte)();
      Handle(TColStd_HArray1OfInteger) anOldInt, aNewInt =
        (myType == ModelAPI_AttributeTables::INTEGER) ?
        new TColStd_HArray1OfInteger(0, aNewSize - 1) : Handle(TColStd_HArray1OfInteger)();
      Handle(TColStd_HArray1OfExtendedString) anOldStr, aNewStr =
        (myType == ModelAPI_AttributeTables::STRING) ?
        new TColStd_HArray1OfExtendedString(0, aNewSize - 1) :
        Handle(TColStd_HArray1OfExtendedString)();
      if (aSize != 0) { // copy the previous values into new positions, otherwise default values
        Handle(TDF_Attribute) anArray;
        myLab.FindAttribute(MY_ARRAY_ID(myType), anArray);
        switch(myType) {
        case ModelAPI_AttributeTables::DOUBLE:
          anOldDouble = Handle(TDataStd_RealArray)::DownCast(anArray)->Array();
          break;
        case ModelAPI_AttributeTables::BOOLEAN:
          anOldBool = Handle(TDataStd_BooleanArray)::DownCast(anArray)->InternalArray();
          break;
        case ModelAPI_AttributeTables::INTEGER:
          anOldInt = Handle(TDataStd_IntegerArray)::DownCast(anArray)->Array();
          break;
        case ModelAPI_AttributeTables::STRING:
          anOldStr = Handle(TDataStd_ExtStringArray)::DownCast(anArray)->Array();
          break;
        }
      }
      for(int aTable = 0; aTable < theTables; aTable++) {
        for(int aColumn = 0; aColumn < theColumns; aColumn++) {
          for(int aRow = 0; aRow < theRows; aRow++) {
            int anOldIndex, anIndex = aTable * theRows * theColumns + aRow * theColumns + aColumn;
            bool aRestore = aTable < myTables && aColumn < myCols && aRow < myRows;
            if (aRestore)
              anOldIndex = aTable * myRows * myCols + aRow * myCols + aColumn;
            switch(myType) {
            case ModelAPI_AttributeTables::DOUBLE: {
              aNewDouble->SetValue(anIndex, aRestore ? anOldDouble->Value(anOldIndex) : 0.);
              break;
            }
            case ModelAPI_AttributeTables::BOOLEAN: {
              aNewBool->SetValue(anIndex, aRestore ? anOldBool->Value(anOldIndex) : Standard_False);
              break;
            }
            case ModelAPI_AttributeTables::INTEGER: {
              aNewInt->SetValue(anIndex, aRestore ? anOldInt->Value(anOldIndex) : 0);
              break;
            }
            case ModelAPI_AttributeTables::STRING: {
              aNewStr->SetValue(anIndex, aRestore ? anOldStr->Value(anOldIndex) : "");
              break;
            }
            }
          }
        }
      }
      // store the new array
      switch(myType) {
      case ModelAPI_AttributeTables::DOUBLE:
        TDataStd_RealArray::Set(myLab, 0, aNewSize - 1)->ChangeArray(aNewDouble);
        break;
      case ModelAPI_AttributeTables::BOOLEAN:
        TDataStd_BooleanArray::Set(myLab, 0, aNewSize - 1)->SetInternalArray(aNewBool);
        break;
      case ModelAPI_AttributeTables::INTEGER:
        TDataStd_IntegerArray::Set(myLab, 0, aNewSize - 1)->ChangeArray(aNewInt);
        break;
      case ModelAPI_AttributeTables::STRING:
        TDataStd_ExtStringArray::Set(myLab, 0, aNewSize - 1)->ChangeArray(aNewStr);
        break;
      }
    }
    // store the new sizes
    myRows = theRows;
    myCols = theColumns;
    myTables = theTables;
    myProp->Clear();
    myProp->Append(int(myType)); // default
    myProp->Append(myTables);
    myProp->Append(myRows);
    myProp->Append(myCols);
    owner()->data()->sendAttributeUpdated(this);
  }
}

void Model_AttributeTables::setType(ModelAPI_AttributeTables::ValueType theType)
{
  if (myType != theType) {
    // remove the old attr
    int aSize = myRows * myCols * myTables;
    if (aSize != 0) {
      myLab.ForgetAttribute(MY_ARRAY_ID(myType));
      myType = theType;
      int aTables = myTables;
      myTables = 0; // to let setSize know that there is no old array
      setSize(myRows, myCols, aTables);
    } else {
      myType = theType;
      owner()->data()->sendAttributeUpdated(this);
    }
  }
}

const ModelAPI_AttributeTables::ValueType& Model_AttributeTables::type() const
{
  return myType;
}

void Model_AttributeTables::setValue(const ModelAPI_AttributeTables::Value theValue,
  const int theRow, const int theColumn, const int theTable)
{
  int anIndex = theTable * myRows * myCols + theRow * myCols + theColumn;
  Handle(TDF_Attribute) anArray;
  myLab.FindAttribute(MY_ARRAY_ID(myType), anArray);
  switch(myType) {
  case ModelAPI_AttributeTables::DOUBLE: {
    Handle(TDataStd_RealArray)::DownCast(anArray)->SetValue(anIndex, theValue.myDouble);
    break;
  }
  case ModelAPI_AttributeTables::BOOLEAN: {
    Handle(TDataStd_BooleanArray)::DownCast(anArray)->SetValue(anIndex, theValue.myBool);
    break;
  }
  case ModelAPI_AttributeTables::INTEGER: {
    Handle(TDataStd_IntegerArray)::DownCast(anArray)->SetValue(anIndex, theValue.myInt);
    break;
  }
  case ModelAPI_AttributeTables::STRING: {
    Handle(TDataStd_ExtStringArray)::DownCast(anArray)->SetValue(anIndex, theValue.myStr.c_str());
    break;
  }
  }
  owner()->data()->sendAttributeUpdated(this);
}

ModelAPI_AttributeTables::Value Model_AttributeTables::value(
  const int theRow, const int theColumn, const int theTable)
{
  ModelAPI_AttributeTables::Value aResult;
  int anIndex = theTable * myRows * myCols + theRow * myCols + theColumn;
  Handle(TDF_Attribute) anArray;
  myLab.FindAttribute(MY_ARRAY_ID(myType), anArray);
  switch(myType) {
  case ModelAPI_AttributeTables::DOUBLE: {
    aResult.myDouble = Handle(TDataStd_RealArray)::DownCast(anArray)->Value(anIndex);
    break;
  }
  case ModelAPI_AttributeTables::BOOLEAN: {
    aResult.myBool = Handle(TDataStd_BooleanArray)::DownCast(anArray)->Value(anIndex) ==
      Standard_True;
    break;
  }
  case ModelAPI_AttributeTables::INTEGER: {
    aResult.myInt = Handle(TDataStd_IntegerArray)::DownCast(anArray)->Value(anIndex);
    break;
  }
  case ModelAPI_AttributeTables::STRING: {
    aResult.myStr = TCollection_AsciiString(Handle(TDataStd_ExtStringArray)::DownCast(anArray)->
      Value(anIndex)).ToCString();
    break;
  }
  }
  return aResult;
}

std::string Model_AttributeTables::valueStr(
    const int theRow, const int theColumn, const int theTable)
{
  std::ostringstream aStr;
  switch(myType) {
  case ModelAPI_AttributeTables::DOUBLE:
    aStr<<value(theRow, theColumn, theTable).myDouble;
    break;
  case ModelAPI_AttributeTables::BOOLEAN:
    aStr<<(value(theRow, theColumn, theTable).myBool ? "True" :"False");
    break;
  case ModelAPI_AttributeTables::INTEGER:
    aStr<<value(theRow, theColumn, theTable).myInt;
    break;
  case ModelAPI_AttributeTables::STRING:
    aStr<<value(theRow, theColumn, theTable).myStr;
    break;
  }
  return aStr.str();
}

//==================================================================================================
Model_AttributeTables::Model_AttributeTables(TDF_Label& theLabel)
{
  myLab = theLabel;
  reinit();
}

void Model_AttributeTables::reinit()
{
  // check the attribute could be already presented in this doc (after load document)
  myIsInitialized =
    myLab.FindAttribute(TDataStd_IntegerList::GetID(), myProp) == Standard_True;
  if (!myIsInitialized) {
    myProp = TDataStd_IntegerList::Set(myLab);
    myProp->Append(int(Model_AttributeTables::DOUBLE)); // default
    myProp->Append(0);
    myProp->Append(0);
    myProp->Append(0);
  }
  TColStd_ListIteratorOfListOfInteger aListIter(myProp->List());
  myType = ModelAPI_AttributeTables::ValueType(aListIter.Value());
  aListIter.Next();
  myTables = aListIter.Value();
  aListIter.Next();
  myRows = aListIter.Value();
  aListIter.Next();
  myCols = aListIter.Value();
}
