// File:        GeomData_Point.cxx
// Created:     24 Apr 2014
// Author:      Mikhail PONIKAROV

#include "GeomData_Point.h"
#include "Model_Events.h"
#include <Events_Loop.h>

using namespace std;

void GeomData_Point::setValue(const double theX, const double theY, const double theZ)
{
  if (myCoords->Value(0) != theX || myCoords->Value(1) != theY || myCoords->Value(2) != theZ) {
    myCoords->SetValue(0, theX);
    myCoords->SetValue(1, theY);
    myCoords->SetValue(2, theZ);
    static Events_ID anEvent = Events_Loop::eventByName(EVENT_FEATURE_UPDATED);
    Model_FeatureUpdatedMessage aMsg(feature(), anEvent);
    Events_Loop::loop()->send(aMsg);
  }
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
