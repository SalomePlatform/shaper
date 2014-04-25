// File:        GeomData_Dir.cxx
// Created:     2 Apr 2014
// Author:      Mikhail PONIKAROV

#include "GeomData_Dir.h"
#include "GeomAPI_Dir.h"
#include <gp_Dir.hxx>

using namespace std;

void GeomData_Dir::setValue(const double theX, const double theY, const double theZ)
{
  myCoords->SetValue(0, theX);
  myCoords->SetValue(1, theY);
  myCoords->SetValue(2, theZ);
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
  // check the attribute could be already presented in this doc (after load document)
  if (!theLabel.FindAttribute(TDataStd_RealArray::GetID(), myCoords)) {
    // create attribute: not initialized by value yet, just zero
    myCoords = TDataStd_RealArray::Set(theLabel, 0, 2);
  }
}
