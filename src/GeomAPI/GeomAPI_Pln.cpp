// Copyright (C) 2014-2017  CEA/DEN, EDF R&D
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
//
// See http://www.salome-platform.org/ or
// email : webmaster.salome@opencascade.com<mailto:webmaster.salome@opencascade.com>
//

#include <GeomAPI_Pln.h>
#include <GeomAPI_Ax3.h>
#include <GeomAPI_Pnt.h>
#include <GeomAPI_Dir.h>
#include <GeomAPI_Lin.h>
#include <GeomAPI_XYZ.h>

#include <gp_Pln.hxx>

#include <IntAna_QuadQuadGeo.hxx>

GeomAPI_Pln::GeomAPI_Pln(const std::shared_ptr<GeomAPI_Ax3>& theAxis)
: GeomAPI_Interface(new gp_Ax3(theAxis->impl<gp_Ax3>()))
{
}

GeomAPI_Pln::GeomAPI_Pln(const std::shared_ptr<GeomAPI_Pnt>& thePoint,
                         const std::shared_ptr<GeomAPI_Dir>& theNormal)
    : GeomAPI_Interface(new gp_Pln(thePoint->impl<gp_Pnt>(), theNormal->impl<gp_Dir>()))
{
}

GeomAPI_Pln::GeomAPI_Pln(const double theA, const double theB, const double theC, const double theD)
    : GeomAPI_Interface(new gp_Pln(theA, theB, theC, theD))
{
}

std::shared_ptr<GeomAPI_Pnt> GeomAPI_Pln::location() const
{
  gp_Pnt aLoc = impl<gp_Pln>().Location();
  return std::shared_ptr<GeomAPI_Pnt>(new GeomAPI_Pnt(aLoc.X(), aLoc.Y(), aLoc.Z()));
}

std::shared_ptr<GeomAPI_Dir> GeomAPI_Pln::direction() const
{
  const gp_Dir& aDir = impl<gp_Pln>().Axis().Direction();
  return std::shared_ptr<GeomAPI_Dir>(new GeomAPI_Dir(aDir.X(), aDir.Y(), aDir.Z()));
}

std::shared_ptr<GeomAPI_Dir> GeomAPI_Pln::xDirection() const
{
  const gp_Dir& aDir = impl<gp_Pln>().XAxis().Direction();
  return std::shared_ptr<GeomAPI_Dir>(new GeomAPI_Dir(aDir.X(), aDir.Y(), aDir.Z()));
}

void GeomAPI_Pln::coefficients(double& theA, double& theB, double& theC, double& theD)
{
  impl<gp_Pln>().Coefficients(theA, theB, theC, theD);
}

bool GeomAPI_Pln::isCoincident(const std::shared_ptr<GeomAPI_Pln> thePlane,
                               const double theTolerance)
{
  if(!thePlane.get()) {
    return false;
  }

  const gp_Pln& aMyPln = impl<gp_Pln>();
  const gp_Pln& anOtherPln = thePlane->impl<gp_Pln>();
  return (aMyPln.Contains(anOtherPln.Location(), theTolerance) &&
    aMyPln.Axis().IsParallel(anOtherPln.Axis(), theTolerance));
}

bool GeomAPI_Pln::isParallel(const std::shared_ptr<GeomAPI_Lin> theLine)
{
  std::shared_ptr<GeomAPI_XYZ> aLineDir = theLine->direction()->xyz();
  std::shared_ptr<GeomAPI_XYZ> aLineLoc = theLine->location()->xyz();

  std::shared_ptr<GeomAPI_XYZ> aNormal = direction()->xyz();
  std::shared_ptr<GeomAPI_XYZ> aLocation = location()->xyz();

  double aDot = aNormal->dot(aLineDir);
  return Abs(aDot) < Precision::SquareConfusion();
}

std::shared_ptr<GeomAPI_Pnt>
  GeomAPI_Pln::intersect(const std::shared_ptr<GeomAPI_Lin>& theLine) const
{
  std::shared_ptr<GeomAPI_XYZ> aLineDir = theLine->direction()->xyz();
  std::shared_ptr<GeomAPI_XYZ> aLineLoc = theLine->location()->xyz();

  std::shared_ptr<GeomAPI_XYZ> aNormal = direction()->xyz();
  std::shared_ptr<GeomAPI_XYZ> aLocation = location()->xyz();

  double aDot = aNormal->dot(aLineDir);
  if (Abs(aDot) < Precision::SquareConfusion())
    return std::shared_ptr<GeomAPI_Pnt>();

  double aParam = aNormal->dot(aLocation->decreased(aLineLoc)) / aDot;
  return std::shared_ptr<GeomAPI_Pnt>(
    new GeomAPI_Pnt(aLineLoc->added(aLineDir->multiplied(aParam))));
}

std::shared_ptr<GeomAPI_Pnt>
  GeomAPI_Pln::project(const std::shared_ptr<GeomAPI_Pnt>& thePoint) const
{
  std::shared_ptr<GeomAPI_XYZ> aNormal = direction()->xyz();
  std::shared_ptr<GeomAPI_XYZ> aLocation = location()->xyz();

  std::shared_ptr<GeomAPI_XYZ> aVec = thePoint->xyz()->decreased(aLocation);
  double aDot = aNormal->dot(aVec);
  std::shared_ptr<GeomAPI_XYZ> aProjection =
      aLocation->added(aVec->decreased(aNormal->multiplied(aDot)));
  return std::shared_ptr<GeomAPI_Pnt>(new GeomAPI_Pnt(aProjection));
}

double GeomAPI_Pln::distance(const std::shared_ptr<GeomAPI_Pln> thePlane) const
{
  const gp_Pln& aMyPln = impl<gp_Pln>();
  const gp_Pln& anOtherPln = thePlane->impl<gp_Pln>();

  return aMyPln.Distance(anOtherPln);
}

void GeomAPI_Pln::translate(const std::shared_ptr<GeomAPI_Dir> theDir, double theDist)
{
  gp_Vec aVec(theDir->impl<gp_Dir>());
  aVec.Normalize();
  aVec.Multiply(theDist);
  implPtr<gp_Pln>()->Translate(aVec);
}

std::shared_ptr<GeomAPI_Lin>
  GeomAPI_Pln::intersect(const std::shared_ptr<GeomAPI_Pln> thePlane) const
{
  std::shared_ptr<GeomAPI_Lin> aRes;

  if(!thePlane.get()) {
    return aRes;
  }

  const gp_Pln& aMyPln = impl<gp_Pln>();
  const gp_Pln& anOtherPln = thePlane->impl<gp_Pln>();

  IntAna_QuadQuadGeo aQuad(aMyPln, anOtherPln, Precision::Confusion(), Precision::Confusion());

  if(aQuad.IsDone() != Standard_True) {
    return aRes;
  }

  if(aQuad.NbSolutions() != 1) {
    return aRes;
  }

  gp_Lin aLin = aQuad.Line(1);
  gp_Pnt aLoc = aLin.Location();
  gp_Dir aDir = aLin.Direction();

  std::shared_ptr<GeomAPI_Pnt> aGeomLoc(new GeomAPI_Pnt(aLoc.X(), aLoc.Y(), aLoc.Z()));
  std::shared_ptr<GeomAPI_Dir> aGeomDir(new GeomAPI_Dir(aDir.X(), aDir.Y(), aDir.Z()));

  aRes.reset(new GeomAPI_Lin(aGeomLoc, aGeomDir));

  return aRes;
}
