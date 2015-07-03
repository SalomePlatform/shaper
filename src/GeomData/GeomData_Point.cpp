// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        GeomData_Point.cxx
// Created:     24 Apr 2014
// Author:      Mikhail PONIKAROV

#include "GeomData_Point.h"
#include <GeomAPI_Pnt.h>
#include <ModelAPI_Feature.h>
#include <ModelAPI_Data.h>
#include <ModelAPI_Events.h>

using namespace std;

void GeomData_Point::setCalculatedValue(const double theX, const double theY, const double theZ)
{
  if (!myIsInitialized || myCoords->Value(0) != theX || myCoords->Value(1) != theY
      || myCoords->Value(2) != theZ) {
    myCoords->SetValue(0, theX);
    myCoords->SetValue(1, theY);
    myCoords->SetValue(2, theZ);
    owner()->data()->sendAttributeUpdated(this);
  }
}

void GeomData_Point::setValue(const double theX, const double theY, const double theZ)
{
  setCalculatedValue(textX().empty() ? theX : x(),
                     textY().empty() ? theY : y(),
                     textZ().empty() ? theZ : z());
}

void GeomData_Point::setValue(const std::shared_ptr<GeomAPI_Pnt>& thePoint)
{
  setValue(thePoint->x(), thePoint->y(), thePoint->z());
}

double GeomData_Point::x() const
{
  return myCoords->Value(0);
}

double GeomData_Point::y() const
{
  return myCoords->Value(1);
}

double GeomData_Point::z() const
{
  return myCoords->Value(2);
}

std::shared_ptr<GeomAPI_Pnt> GeomData_Point::pnt()
{
  std::shared_ptr<GeomAPI_Pnt> aResult(
      new GeomAPI_Pnt(myCoords->Value(0), myCoords->Value(1), myCoords->Value(2)));
  return aResult;
}

void GeomData_Point::setText(const std::string& theX,
                             const std::string& theY,
                             const std::string& theZ)
{
  TCollection_ExtendedString aX(theX.c_str());
  TCollection_ExtendedString aY(theY.c_str());
  TCollection_ExtendedString aZ(theZ.c_str());

  if (!myIsInitialized ||
      myTextArray->Value(0) != aX ||
      myTextArray->Value(1) != aY ||
      myTextArray->Value(2) != aZ) {
    myTextArray->SetValue(0, aX);
    myTextArray->SetValue(1, aY);
    myTextArray->SetValue(2, aZ);
    // Send it to evaluator to convert into the double and store in the attribute
    ModelAPI_AttributeEvalMessage::send(owner()->data()->attribute(id()), this);
    owner()->data()->sendAttributeUpdated(this);
  }
}

std::string GeomData_Point::textX()
{
  return TCollection_AsciiString(myTextArray->Value(0)).ToCString();;
}
std::string GeomData_Point::textY()
{
  return TCollection_AsciiString(myTextArray->Value(1)).ToCString();;
}
std::string GeomData_Point::textZ()
{
  return TCollection_AsciiString(myTextArray->Value(2)).ToCString();;
}

void GeomData_Point::setExpressionInvalid(int theComponent, bool theFlag)
{
  if (!myIsInitialized || myExpressionInvalidArray->Value(theComponent) != theFlag) {
    myExpressionInvalidArray->SetValue(theComponent, theFlag);
  }
}

bool GeomData_Point::expressionInvalid(int theComponent)
{
  return myExpressionInvalidArray->Value(theComponent);
}

GeomData_Point::GeomData_Point(TDF_Label& theLabel)
{
  myIsInitialized = true;

  if (theLabel.FindAttribute(TDataStd_RealArray::GetID(), myCoords) != Standard_True) {
    // create attribute: not initialized by value yet, just zero
    myCoords = TDataStd_RealArray::Set(theLabel, 0, 2);
    myIsInitialized = false;
  }
  if (theLabel.FindAttribute(TDataStd_ExtStringArray::GetID(), myTextArray) != Standard_True) {
    // create attribute: not initialized by value yet, just zero
    myTextArray = TDataStd_ExtStringArray::Set(theLabel, 0, 2);
    myIsInitialized = false;
  }
  if (theLabel.FindAttribute(TDataStd_BooleanArray::GetID(), myExpressionInvalidArray) != Standard_True) {
    // create attribute: not initialized by value yet, just zero
    myExpressionInvalidArray = TDataStd_BooleanArray::Set(theLabel, 0, 2);
    myIsInitialized = false;
  }
}
