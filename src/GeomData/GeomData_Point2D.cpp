// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        GeomData_Point2D.cxx
// Created:     24 Apr 2014
// Author:      Mikhail PONIKAROV

#include "GeomData_Point2D.h"
#include <GeomAPI_Pnt2d.h>
#include <ModelAPI_Feature.h>
#include <ModelAPI_Data.h>

using namespace std;

void GeomData_Point2D::setValue(const double theX, const double theY)
{
  if (!myIsInitialized || myCoords->Value(0) != theX || myCoords->Value(1) != theY) {
    myCoords->SetValue(0, theX);
    myCoords->SetValue(1, theY);
    owner()->data()->sendAttributeUpdated(this);
  }
}

void GeomData_Point2D::setValue(const std::shared_ptr<GeomAPI_Pnt2d>& thePoint)
{
  setValue(thePoint->x(), thePoint->y());
}

double GeomData_Point2D::x() const
{
  return myCoords->Value(0);
}

double GeomData_Point2D::y() const
{
  return myCoords->Value(1);
}

std::shared_ptr<GeomAPI_Pnt2d> GeomData_Point2D::pnt()
{
  std::shared_ptr<GeomAPI_Pnt2d> aResult(
      new GeomAPI_Pnt2d(myCoords->Value(0), myCoords->Value(1)));
  return aResult;
}

void GeomData_Point2D::setText(const std::string& theX,
                               const std::string& theY)
{
  TCollection_ExtendedString aX(theX.c_str());
  TCollection_ExtendedString aY(theY.c_str());

  if (!myIsInitialized ||
      myTextArray->Value(0) != aX ||
      myTextArray->Value(1) != aY) {
    myTextArray->SetValue(0, aX);
    myTextArray->SetValue(1, aY);
    owner()->data()->sendAttributeUpdated(this);
  }
}

std::string GeomData_Point2D::textX()
{
  return TCollection_AsciiString(myTextArray->Value(0)).ToCString();;
}
std::string GeomData_Point2D::textY()
{
  return TCollection_AsciiString(myTextArray->Value(1)).ToCString();;
}

void GeomData_Point2D::setExpressionInvalid(int theComponent, bool theFlag)
{
  if (!myIsInitialized || myExpressionInvalidArray->Value(theComponent) != theFlag) {
    myExpressionInvalidArray->SetValue(theComponent, theFlag);
  }
}

bool GeomData_Point2D::expressionInvalid(int theComponent)
{
  return myExpressionInvalidArray->Value(theComponent);
}

GeomData_Point2D::GeomData_Point2D(TDF_Label& theLabel)
{
  myIsInitialized = true;

  if (theLabel.FindAttribute(TDataStd_RealArray::GetID(), myCoords) != Standard_True) {
    // create attribute: not initialized by value yet, just zero
    myCoords = TDataStd_RealArray::Set(theLabel, 0, 1);
    myIsInitialized = false;
  }
  if (theLabel.FindAttribute(TDataStd_ExtStringArray::GetID(), myTextArray) != Standard_True) {
    // create attribute: not initialized by value yet, just zero
    myTextArray = TDataStd_ExtStringArray::Set(theLabel, 0, 1);
    myIsInitialized = false;
  }
  if (theLabel.FindAttribute(TDataStd_BooleanArray::GetID(), myExpressionInvalidArray) != Standard_True) {
    // create attribute: not initialized by value yet, just zero
    myExpressionInvalidArray = TDataStd_BooleanArray::Set(theLabel, 0, 1);
    myIsInitialized = false;
  }
}
