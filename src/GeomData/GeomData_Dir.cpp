// File:        GeomData_Dir.cxx
// Created:     2 Apr 2014
// Author:      Mikhail PONIKAROV

#include "GeomData_Dir.h"
#include "GeomAPI_Dir.h"
#include <gp_Dir.hxx>
#include <ModelAPI_Feature.h>
#include <ModelAPI_Data.h>

using namespace std;

void GeomData_Dir::setValue(const double theX, const double theY, const double theZ)
{
  if (!myIsInitialized || myCoords->Value(0) != theX || myCoords->Value(1) != theY || 
       myCoords->Value(2) != theZ) {
    myCoords->SetValue(0, theX);
    myCoords->SetValue(1, theY);
    myCoords->SetValue(2, theZ);
    owner()->data()->sendAttributeUpdated(this);
  }
}

void GeomData_Dir::setValue(const boost::shared_ptr<GeomAPI_Dir>& theDir)
{
  setValue(theDir->x(), theDir->y(), theDir->z());
  owner()->data()->sendAttributeUpdated(this);
}

double GeomData_Dir::x() const
{
  return myCoords->Value(0);
}

double GeomData_Dir::y() const
{
  return myCoords->Value(1);
}

double GeomData_Dir::z() const
{
  return myCoords->Value(2);
}

boost::shared_ptr<GeomAPI_Dir> GeomData_Dir::dir()
{
  return boost::shared_ptr<GeomAPI_Dir>(new GeomAPI_Dir(
    myCoords->Value(0), myCoords->Value(1), myCoords->Value(2)));
}

GeomData_Dir::GeomData_Dir(TDF_Label& theLabel)
{
  myIsInitialized = theLabel.FindAttribute(TDataStd_RealArray::GetID(), myCoords) == Standard_True;
  if (!myIsInitialized) {
    // create attribute: not initialized by value yet, just zero
    myCoords = TDataStd_RealArray::Set(theLabel, 0, 2);
  }
}
