// Copyright (C) 2018-2020  CEA/DEN, EDF R&D
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
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

#include <GeomAPI_Angle.h>

#include <GeomAPI_Edge.h>
#include <GeomAPI_Pnt.h>

#include <BRep_Tool.hxx>
#include <ElCLib.hxx>
#include <Geom_Curve.hxx>
#include <GeomAPI_ProjectPointOnCurve.hxx>
#include <Precision.hxx>
#include <TopoDS_Edge.hxx>

/// \struct AngleDirections
/// \brief Used to store info about angle.
struct AngleDirections {
  gp_Vec myDir1;
  gp_Vec myDir2;
};

#define MY_ANGLE implPtr<AngleDirections>()
#define PI 3.1415926535897932


GeomAPI_Angle::GeomAPI_Angle(const std::shared_ptr<GeomAPI_Edge>& theEdge1,
                             const std::shared_ptr<GeomAPI_Edge>& theEdge2,
                             const std::shared_ptr<GeomAPI_Pnt>&  thePoint)
{
  gp_Pnt aPoint = thePoint->impl<gp_Pnt>();
  const TopoDS_Edge& anEdge1 = theEdge1->impl<TopoDS_Edge>();
  const TopoDS_Edge& anEdge2 = theEdge2->impl<TopoDS_Edge>();

  double aF1, aL1;
  Handle(Geom_Curve) aCurve1 = BRep_Tool::Curve(anEdge1, aF1, aL1);
  double aF2, aL2;
  Handle(Geom_Curve) aCurve2 = BRep_Tool::Curve(anEdge2, aF2, aL2);

  AngleDirections* anAngle = new AngleDirections;
  gp_Pnt aP;

  GeomAPI_ProjectPointOnCurve aProj1(aPoint, aCurve1);
  if (aProj1.NbPoints() > 0) {
    aCurve1->D1(aProj1.LowerDistanceParameter(), aP, anAngle->myDir1);
    if (aCurve1->Value(aF1).SquareDistance(aPoint) < aCurve1->Value(aL1).SquareDistance(aPoint))
      anAngle->myDir1.Reverse();
  }

  GeomAPI_ProjectPointOnCurve aProj2(aPoint, aCurve2);
  if (aProj2.NbPoints() > 0) {
    aCurve2->D1(aProj2.LowerDistanceParameter(), aP, anAngle->myDir2);
    if (aCurve2->Value(aF2).SquareDistance(aPoint) < aCurve2->Value(aL2).SquareDistance(aPoint))
      anAngle->myDir2.Reverse();
  }

  setImpl(anAngle);
}

GeomAPI_Angle::GeomAPI_Angle(const std::shared_ptr<GeomAPI_Pnt>& thePoint1,
                             const std::shared_ptr<GeomAPI_Pnt>& thePoint2,
                             const std::shared_ptr<GeomAPI_Pnt>& thePoint3)
{
  gp_Pnt aPoint1 = thePoint1->impl<gp_Pnt>();
  gp_Pnt aPoint2 = thePoint2->impl<gp_Pnt>();
  gp_Pnt aPoint3 = thePoint3->impl<gp_Pnt>();

  AngleDirections* anAngle = new AngleDirections;
  anAngle->myDir1.SetXYZ(aPoint1.XYZ() - aPoint2.XYZ());
  anAngle->myDir2.SetXYZ(aPoint3.XYZ() - aPoint2.XYZ());
  setImpl(anAngle);
}

double GeomAPI_Angle::angleDegree()
{
  return angleRadian() * 180.0 / PI;
}

double GeomAPI_Angle::angleRadian()
{
  AngleDirections* anAngle = MY_ANGLE;
  if (anAngle->myDir1.SquareMagnitude() < Precision::SquareConfusion() ||
      anAngle->myDir2.SquareMagnitude() < Precision::SquareConfusion())
    return 0.0;

  gp_Dir aDir1(anAngle->myDir1);
  gp_Dir aDir2(anAngle->myDir2);
  double aRes = aDir1.Angle(aDir2);
  aRes = ElCLib::InPeriod(aRes, 0.0, 2.0 * PI);
  if (Abs(aRes) < 1.e-12)
    aRes = 0.0;
  return aRes;
}
