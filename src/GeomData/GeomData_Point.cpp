// File:        GeomData_Point.cxx
// Created:     24 Apr 2014
// Author:      Mikhail PONIKAROV

#include "GeomData_Point.h"
#include <GeomAPI_Pnt.h>
#include <ModelAPI_Feature.h>
#include <ModelAPI_Data.h>

using namespace std;

void GeomData_Point::setValue(const double theX, const double theY, const double theZ)
{
  if (!myIsInitialized || myCoords->Value(0) != theX || myCoords->Value(1) != theY || 
       myCoords->Value(2) != theZ) {
    myCoords->SetValue(0, theX);
    myCoords->SetValue(1, theY);
    myCoords->SetValue(2, theZ);
    owner()->data()->sendAttributeUpdated(this);
  }
}

void GeomData_Point::setValue(const boost::shared_ptr<GeomAPI_Pnt>& thePoint)
{
  setValue(thePoint->x(), thePoint->y(), thePoint->z());
  owner()->data()->sendAttributeUpdated(this);
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

boost::shared_ptr<GeomAPI_Pnt> GeomData_Point::pnt()
{
  boost::shared_ptr<GeomAPI_Pnt> aResult(new GeomAPI_Pnt(
    myCoords->Value(0), myCoords->Value(1), myCoords->Value(2)));
  return aResult;
}

GeomData_Point::GeomData_Point(TDF_Label& theLabel)
{
  myIsInitialized = theLabel.FindAttribute(TDataStd_RealArray::GetID(), myCoords) == Standard_True;
  if (!myIsInitialized) {
    // create attribute: not initialized by value yet, just zero
    myCoords = TDataStd_RealArray::Set(theLabel, 0, 2);
  }
}
