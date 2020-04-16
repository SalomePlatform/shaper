// Copyright (C) 2014-2020  CEA/DEN, EDF R&D
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

#include<GeomAPI_Curve.h>
#include<GeomAPI_Pnt.h>

#include <TopoDS_Shape.hxx>
#include <Geom_Circle.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Ellipse.hxx>
#include <Geom_Line.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <GeomAPI_ProjectPointOnCurve.hxx>
#include <BRep_Tool.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS.hxx>
#include <GeomAdaptor_Curve.hxx>

#define MY_CURVE (*(implPtr<Handle_Geom_Curve>()))

GeomAPI_Curve::GeomAPI_Curve()
    : GeomAPI_Interface(new Handle_Geom_Curve()),
      myStart(-Precision::Infinite()),
      myEnd(Precision::Infinite())
{
}

GeomAPI_Curve::GeomAPI_Curve(const std::shared_ptr<GeomAPI_Shape>& theShape)
  : GeomAPI_Interface(new Handle_Geom_Curve()) // initially it is null
{
  const TopoDS_Shape& aShape = theShape->impl<TopoDS_Shape>();
  TopoDS_Edge anEdge = TopoDS::Edge(aShape);
  if (!anEdge.IsNull()) {
    Handle(Geom_Curve) aCurve = BRep_Tool::Curve(anEdge, myStart, myEnd);
    if (!aCurve.IsNull()) {
      setImpl(new Handle(Geom_Curve)(aCurve));
    }
  }
}

bool GeomAPI_Curve::isNull() const
{
  return MY_CURVE.IsNull() == Standard_True;
}

static bool isCurveType(const Handle(Geom_Curve)& theCurve, const Handle(Standard_Type)& theType)
{
  if (theCurve.IsNull())
    return false;
  Handle(Geom_Curve) aCurve = theCurve;
  if (aCurve->DynamicType() == STANDARD_TYPE(Geom_TrimmedCurve))
    aCurve = Handle(Geom_TrimmedCurve)::DownCast(aCurve)->BasisCurve();
  return aCurve->DynamicType() == theType;
}

bool GeomAPI_Curve::isLine() const
{
  return isCurveType(MY_CURVE, STANDARD_TYPE(Geom_Line));
}

bool GeomAPI_Curve::isCircle() const
{
  return isCurveType(MY_CURVE, STANDARD_TYPE(Geom_Circle));
}

bool GeomAPI_Curve::isEllipse() const
{
  return isCurveType(MY_CURVE, STANDARD_TYPE(Geom_Ellipse));
}

double GeomAPI_Curve::startParam()
{
  if (Precision::IsInfinite(myStart)) {
    if (isTrimmed()) {
      myStart = Handle(Geom_TrimmedCurve)::DownCast(MY_CURVE)->FirstParameter();
    }
    else if (MY_CURVE->IsClosed() && MY_CURVE->IsPeriodic())
      myStart = 0.0;
  }
  return myStart;
}

double GeomAPI_Curve::endParam()
{
  if (Precision::IsInfinite(myEnd)) {
    if (isTrimmed()) {
      myEnd = Handle(Geom_TrimmedCurve)::DownCast(MY_CURVE)->LastParameter();
    }
    else if (MY_CURVE->IsClosed() && MY_CURVE->IsPeriodic())
      myEnd = MY_CURVE->Period();
  }
  return myEnd;
}

std::shared_ptr<GeomAPI_Pnt> GeomAPI_Curve::getPoint(double theParam)
{
  GeomAdaptor_Curve aAdaptor(MY_CURVE, myStart, myEnd);
  gp_Pnt aPnt = aAdaptor.Value(theParam);
  return std::shared_ptr<GeomAPI_Pnt>(new GeomAPI_Pnt(aPnt.X(), aPnt.Y(), aPnt.Z()));
}

bool GeomAPI_Curve::isEqual(const std::shared_ptr<GeomAPI_Curve>& theOther) const
{
  return MY_CURVE == theOther->impl<Handle_Geom_Curve>();
}

bool GeomAPI_Curve::isTrimmed() const
{
  return !isNull() && MY_CURVE->DynamicType() == STANDARD_TYPE(Geom_TrimmedCurve);
}

GeomCurvePtr GeomAPI_Curve::basisCurve() const
{
  Handle(Geom_Curve) aCurve = MY_CURVE;
  if (aCurve->DynamicType() == STANDARD_TYPE(Geom_TrimmedCurve))
    aCurve = Handle(Geom_TrimmedCurve)::DownCast(aCurve)->BasisCurve();

  GeomCurvePtr aNewCurve(new GeomAPI_Curve);
  aNewCurve->setImpl(new Handle(Geom_Curve)(aCurve));
  return aNewCurve;
}


const std::shared_ptr<GeomAPI_Pnt> GeomAPI_Curve::project(
    const std::shared_ptr<GeomAPI_Pnt>& thePoint) const
{
  std::shared_ptr<GeomAPI_Pnt> aResult;
  if (MY_CURVE.IsNull())
    return aResult;

  const gp_Pnt& aPoint = thePoint->impl<gp_Pnt>();

  GeomAPI_ProjectPointOnCurve aProj(aPoint, MY_CURVE);
  Standard_Integer aNbPoint = aProj.NbPoints();
  if (aNbPoint > 0) {
    gp_Pnt aNearest = aProj.NearestPoint();
    aResult = GeomPointPtr(new GeomAPI_Pnt(aNearest.X(), aNearest.Y(), aNearest.Z()));
  }
  return aResult;
}

// ================================================================================================

bool GeomAPI_Curve::Comparator::operator()(const GeomCurvePtr& theCurve1,
                                           const GeomCurvePtr& theCurve2) const
{
  const Handle(Geom_Curve)& aCurve1 = theCurve1->impl<Handle_Geom_Curve>();
  const Handle(Geom_Curve)& aCurve2 = theCurve2->impl<Handle_Geom_Curve>();
  return aCurve1.get() < aCurve2.get();
}
