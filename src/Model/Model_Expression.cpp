// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        Model_Expression.cpp
// Created:     5 Aug 2015
// Author:      Sergey POKHODENKO

#include "Model_Expression.h"

#include <TCollection_AsciiString.hxx>
#include <TCollection_ExtendedString.hxx>
#include <TDataStd_ListIteratorOfListOfExtendedString.hxx>
#include <TDataStd_UAttribute.hxx>

#include <TDataStd_RealArray.hxx>
#include <TDataStd_ExtStringArray.hxx>


static Standard_GUID kInvalidGUID("caee5ce4-34b1-4b29-abcb-685287d18096");


Model_Expression::Model_Expression(TDF_Label& theLabel)
{
  if (!theLabel.FindAttribute(TDataStd_Name::GetID(), myText)) {
    myText = TDataStd_Name::Set(theLabel, TCollection_ExtendedString());
  }
  if (!theLabel.FindAttribute(TDataStd_Comment::GetID(), myError)) {
    myError = TDataStd_Comment::Set(theLabel, TCollection_ExtendedString());
  }
  if (!theLabel.FindAttribute(TDataStd_ExtStringList::GetID(), myUsedParameters)) {
    myUsedParameters = TDataStd_ExtStringList::Set(theLabel);
  }
}

void Model_Expression::setText(const std::string& theValue)
{
  if (text() != theValue)
    myText->Set(TCollection_ExtendedString(theValue.c_str()));

  setError(text().empty() ? "" : "Not a double value.");
}

std::string Model_Expression::text() const 
{
  return TCollection_AsciiString(myText->Get()).ToCString();
}

void Model_Expression::setError(const std::string& theError)
{
  if (error() != theError)
    myError->Set(TCollection_ExtendedString(theError.c_str()));
}

std::string Model_Expression::error()
{
  return TCollection_AsciiString(myError->Get()).ToCString();
}

void Model_Expression::setUsedParameters(const std::set<std::string>& theUsedParameters)
{
  myUsedParameters->Clear();
  std::set<std::string>::const_iterator anIt = theUsedParameters.begin();
  for (; anIt != theUsedParameters.end(); ++anIt)
    myUsedParameters->Append(TCollection_ExtendedString(anIt->c_str()));
}

std::set<std::string> Model_Expression::usedParameters() const
{
  std::set<std::string> aResult;
  TDataStd_ListIteratorOfListOfExtendedString aIt;
  for (aIt.Initialize(myUsedParameters->List()); aIt.More(); aIt.Next())
    aResult.insert(TCollection_AsciiString(aIt.Value()).ToCString());
  return aResult;
}

Model_ExpressionDouble::Model_ExpressionDouble(TDF_Label& theLabel)
    : Model_Expression(theLabel)
{
  if (!theLabel.FindAttribute(TDataStd_Real::GetID(), myReal)) {
    myReal = TDataStd_Real::Set(theLabel, 0.);
    myIsInitialized = false;
    // MPV: temporarily to support the previously saved files (to check and resolve bugs), to be removed
    Handle(TDataStd_RealArray) anOldArray;
    if (theLabel.Father().FindAttribute(TDataStd_RealArray::GetID(), anOldArray) == Standard_True) {
      myReal->Set(anOldArray->Value(theLabel.Tag() - 1));
      myIsInitialized = true;
      Handle(TDataStd_ExtStringArray) anOldExp;
      if (theLabel.Father().FindAttribute(TDataStd_ExtStringArray::GetID(), anOldExp) == Standard_True) {
        myText->Set(anOldExp->Value(theLabel.Tag() - 1));
      }
    } else {
      Handle(TDataStd_Real) anOldReal;
      if (theLabel.Father().FindAttribute(TDataStd_Real::GetID(), anOldReal)) {
        myIsInitialized = true;
        myReal->Set(anOldReal->Get());
        Handle(TDataStd_Name) aText;
        if (theLabel.Father().FindAttribute(TDataStd_Name::GetID(), aText)) {
          myText->Set(aText->Get());
        }
      }
    }
  } else
    myIsInitialized = true;
}

void Model_ExpressionDouble::setValue(const double theValue)
{
  if (value() != theValue)
    myReal->Set(theValue);
}

double Model_ExpressionDouble::value()
{
  return myReal->Get();
}

void Model_ExpressionDouble::setInvalid(const bool theFlag)
{
  if (theFlag) {
    TDataStd_UAttribute::Set(myReal->Label(), kInvalidGUID);
  } else {
    myReal->Label().ForgetAttribute(kInvalidGUID);
  }
}

bool Model_ExpressionDouble::isInvalid()
{
  return myReal->Label().IsAttribute(kInvalidGUID) == Standard_True;
}


Model_ExpressionInteger::Model_ExpressionInteger(TDF_Label& theLabel)
    : Model_Expression(theLabel)
{
  if (!theLabel.FindAttribute(TDataStd_Integer::GetID(), myInteger)) {
    myInteger = TDataStd_Integer::Set(theLabel, 0);
    myIsInitialized = false;
  } else
    myIsInitialized = true;
}

void Model_ExpressionInteger::setValue(const int theValue)
{
  if (value() != theValue)
    myInteger->Set(theValue);
}

int Model_ExpressionInteger::value()
{
  return myInteger->Get();
}

void Model_ExpressionInteger::setInvalid(const bool theFlag)
{
  if (theFlag) {
    TDataStd_UAttribute::Set(myInteger->Label(), kInvalidGUID);
  } else {
    myInteger->Label().ForgetAttribute(kInvalidGUID);
  }
}

bool Model_ExpressionInteger::isInvalid()
{
  return myInteger->Label().IsAttribute(kInvalidGUID) == Standard_True;
}
