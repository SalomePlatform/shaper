// File:        ModelAPI_AttributeDouble.cxx
// Created:     2 Apr 2014
// Author:      Mikhail PONIKAROV

#include "GeomData_Point.h"

using namespace std;

void GeomData_Point::setValue(const double theX, const double theY, const double theZ)
{
  myCoords->SetValue(0, theX);
  myCoords->SetValue(1, theY);
  myCoords->SetValue(2, theZ);
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

GeomData_Point::GeomData_Point(TDF_Label& theLabel)
{
  // check the attribute could be already presented in this doc (after load document)
  if (!theLabel.FindAttribute(TDataStd_RealArray::GetID(), myCoords)) {
    // create attribute: not initialized by value yet, just zero
    myCoords = TDataStd_RealArray::Set(theLabel, 0, 2);
  }
}
