// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        GeomAPI_Ellipse.cpp
// Created:     25 April 2017
// Author:      Vitaly Smetannikov

#include "GeomAPI_Ellipse.h"
#include "GeomAPI_Ax2.h"
#include "GeomAPI_Pnt.h"

#include <gp_Elips.hxx>

#define MY_ELIPS implPtr<gp_Elips>()

GeomAPI_Ellipse::GeomAPI_Ellipse(const std::shared_ptr<GeomAPI_Ax2>& theAx2,
                                 double theMajorRadius, double theMinorRadius)
: GeomAPI_Interface(new gp_Elips(theAx2->impl<gp_Ax2>(), theMajorRadius, theMinorRadius))
{
}

GeomPointPtr GeomAPI_Ellipse::firstFocus() const
{
  const gp_Pnt& aFirst = MY_ELIPS->Focus1();
  return std::shared_ptr<GeomAPI_Pnt>(new GeomAPI_Pnt(aFirst.X(), aFirst.Y(), aFirst.Z()));
}

GeomPointPtr GeomAPI_Ellipse::secondFocus() const
{
  const gp_Pnt& aSecond = MY_ELIPS->Focus2();
  return std::shared_ptr<GeomAPI_Pnt>(new GeomAPI_Pnt(aSecond.X(), aSecond.Y(), aSecond.Z()));
}

double GeomAPI_Ellipse::minorRadius() const
{
  return MY_ELIPS->MinorRadius();
}

double GeomAPI_Ellipse::majorRadius() const
{
  return MY_ELIPS->MajorRadius();
}
