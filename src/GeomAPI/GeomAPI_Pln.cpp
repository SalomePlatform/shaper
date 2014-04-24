// File:        GeomAPI_Pln.cpp
// Created:     23 Apr 2014
// Author:      Mikhail PONIKAROV

#include<GeomAPI_Pln.h>
#include <GeomAPI_Pnt.h>
#include <GeomAPI_Dir.h>

#include<gp_Pln.hxx>

using namespace std;

GeomAPI_Pln::GeomAPI_Pln(const shared_ptr<GeomAPI_Pnt>& thePoint,
            const shared_ptr<GeomAPI_Dir>& theNormal)
: GeomAPI_Interface(new gp_Pln(thePoint->impl<gp_Pnt>(),
                               theNormal->impl<gp_Dir>()))
{
}

GeomAPI_Pln::GeomAPI_Pln(
  const double theA, const double theB, const double theC, const double theD)
: GeomAPI_Interface(new gp_Pln(theA, theB, theC, theD))
{
}

shared_ptr<GeomAPI_Pnt> GeomAPI_Pln::location()
{
  gp_Pnt aLoc = impl<gp_Pln>().Location();
  return shared_ptr<GeomAPI_Pnt>(new GeomAPI_Pnt(aLoc.X(), aLoc.Y(), aLoc.Z()));
}

shared_ptr<GeomAPI_Dir> GeomAPI_Pln::direction()
{
  const gp_Dir& aDir = impl<gp_Pln>().Axis().Direction();
  return shared_ptr<GeomAPI_Dir>(new GeomAPI_Dir(aDir.X(), aDir.Y(), aDir.Z()));
}
