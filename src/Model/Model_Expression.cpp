// Copyright (C) 2014-2017  CEA/DEN, EDF R&D
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
//
// See http://www.salome-platform.org/ or
// email : webmaster.salome@opencascade.com<mailto:webmaster.salome@opencascade.com>
//

#include "Model_Expression.h"

#include <TCollection_AsciiString.hxx>
#include <TCollection_ExtendedString.hxx>
#include <TDataStd_ListIteratorOfListOfExtendedString.hxx>
#include <TDataStd_UAttribute.hxx>

#include <TDataStd_RealArray.hxx>
#include <TDataStd_ExtStringArray.hxx>

#include <limits>

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
  if (text() != theValue) {
    myText->Set(TCollection_ExtendedString(theValue.c_str()));
    myIsInitialized = true; // the value will be set very soon
  }

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

///////////////// Model_ExpressionDouble /////////////
Model_ExpressionDouble::Model_ExpressionDouble(TDF_Label& theLabel)
    : Model_Expression(theLabel)
{
  myLab = theLabel;
  reinit();
}

void Model_ExpressionDouble::reinit()
{
  if (!myLab.FindAttribute(TDataStd_Real::GetID(), myReal)) {
    myIsInitialized = false;
  } else {
    myIsInitialized = true;
  }
}

void Model_ExpressionDouble::setValue(const double theValue)
{
  if (!myIsInitialized || myReal.IsNull()) {
    myReal = TDataStd_Real::Set(myText->Label(), theValue);
    myIsInitialized = true;
  } else if (value() != theValue) {
    myReal->Set(theValue);
  }
}

double Model_ExpressionDouble::value()
{
  if (myIsInitialized)
    return myReal->Get();
  return std::numeric_limits<double>::max(); // error
}

void Model_ExpressionDouble::setInvalid(const bool theFlag)
{
  if (theFlag) {
    TDataStd_UAttribute::Set(myText->Label(), kInvalidGUID);
  } else {
    myText->Label().ForgetAttribute(kInvalidGUID);
  }
}

bool Model_ExpressionDouble::isInvalid()
{
  return myText->Label().IsAttribute(kInvalidGUID) == Standard_True;
}

///////////////// Model_ExpressionInteger /////////////
Model_ExpressionInteger::Model_ExpressionInteger(TDF_Label& theLabel)
    : Model_Expression(theLabel)
{
  myLab = theLabel;
  reinit();
}

void Model_ExpressionInteger::reinit()
{
  if (!myLab.FindAttribute(TDataStd_Integer::GetID(), myInteger)) {
    myIsInitialized = false;
  } else {
    myIsInitialized = true;
  }
}

void Model_ExpressionInteger::setValue(const int theValue)
{
  if (!myIsInitialized || myInteger.IsNull()) {
    myInteger = TDataStd_Integer::Set(myText->Label(), theValue);
    myIsInitialized = true;
  } else if (value() != theValue) {
    myInteger->Set(theValue);
  }
}

int Model_ExpressionInteger::value()
{
  if (myIsInitialized)
    return myInteger->Get();
  return std::numeric_limits<int>::max(); // error
}

void Model_ExpressionInteger::setInvalid(const bool theFlag)
{
  if (theFlag) {
    TDataStd_UAttribute::Set(myText->Label(), kInvalidGUID);
  } else {
    myText->Label().ForgetAttribute(kInvalidGUID);
  }
}

bool Model_ExpressionInteger::isInvalid()
{
  return myText->Label().IsAttribute(kInvalidGUID) == Standard_True;
}
