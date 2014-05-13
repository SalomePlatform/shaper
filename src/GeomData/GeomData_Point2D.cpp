// File:        GeomData_Point2D.cxx
// Created:     24 Apr 2014
// Author:      Mikhail PONIKAROV

#include "GeomData_Point2D.h"
#include "Model_Events.h"
#include <Events_Loop.h>

using namespace std;

void GeomData_Point2D::setValue(const double theX, const double theY)
{
  if (myCoords->Value(0) != theX || myCoords->Value(1) != theY) {
    myCoords->SetValue(0, theX);
    myCoords->SetValue(1, theY);
    static Events_ID anEvent = Events_Loop::eventByName(EVENT_FEATURE_UPDATED);
    Model_FeatureUpdatedMessage aMsg(owner(), anEvent);
    Events_Loop::loop()->send(aMsg);
  }
}

double GeomData_Point2D::x() const
{
  return myCoords->Value(0);
}

double GeomData_Point2D::y() const
{
  return myCoords->Value(1);
}

GeomData_Point2D::GeomData_Point2D(TDF_Label& theLabel)
{
  // check the attribute could be already presented in this doc (after load document)
  if (!theLabel.FindAttribute(TDataStd_RealArray::GetID(), myCoords)) {
    // create attribute: not initialized by value yet, just zero
    myCoords = TDataStd_RealArray::Set(theLabel, 0, 1);
  }
}
