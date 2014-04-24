// File:        GeomAPI_Pln.cpp
// Created:     23 Apr 2014
// Author:      Mikhail PONIKAROV

#include<GeomAPI_Pln.h>
#include <GeomAPI_Pnt.h>
#include <GeomAPI_Dir.h>

#include<gp_Pln.hxx>

using namespace std;

GeomAPI_Pln::GeomAPI_Pln(const boost::shared_ptr<GeomAPI_Pnt>& thePoint,
            const boost::shared_ptr<GeomAPI_Dir>& theNormal)
: GeomAPI_Interface(new gp_Pln(thePoint->impl<gp_Pnt>(),
                               theNormal->impl<gp_Dir>()))
{
}

GeomAPI_Pln::GeomAPI_Pln(
  const double theA, const double theB, const double theC, const double theD)
: GeomAPI_Interface(new gp_Pln(theA, theB, theC, theD))
{
}

boost::shared_ptr<GeomAPI_Pnt> GeomAPI_Pln::location()
{
  gp_Pnt aLoc = impl<gp_Pln>().Location();
  return boost::shared_ptr<GeomAPI_Pnt>(new GeomAPI_Pnt(aLoc.X(), aLoc.Y(), aLoc.Z()));
}

boost::shared_ptr<GeomAPI_Dir> GeomAPI_Pln::direction()
{
  const gp_Dir& aDir = impl<gp_Pln>().Axis().Direction();
  return boost::shared_ptr<GeomAPI_Dir>(new GeomAPI_Dir(aDir.X(), aDir.Y(), aDir.Z()));
}

void GeomAPI_Pln::coefficients(double& theA, double& theB, double& theC, double& theD)
{
  impl<gp_Pln>().Coefficients(theA, theB, theC, theD);
}
