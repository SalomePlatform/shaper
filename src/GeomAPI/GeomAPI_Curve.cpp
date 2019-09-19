// Copyright (C) 2014-2019  CEA/DEN, EDF R&D
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
#include <BRep_Tool.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS.hxx>
#include <GeomAdaptor_Curve.hxx>

#define MY_CURVE (*(implPtr<Handle_Geom_Curve>()))

GeomAPI_Curve::GeomAPI_Curve()
    : GeomAPI_Interface(new Handle_Geom_Curve()), myStart(0), myEnd(1)
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

bool GeomAPI_Curve::isLine() const
{
  return !isNull() && MY_CURVE->DynamicType() == STANDARD_TYPE(Geom_Line);
}

bool GeomAPI_Curve::isCircle() const
{
  return !isNull() && MY_CURVE->DynamicType() == STANDARD_TYPE(Geom_Circle);
}

bool GeomAPI_Curve::isEllipse() const
{
  return !isNull() && MY_CURVE->DynamicType() == STANDARD_TYPE(Geom_Ellipse);
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

// unused in the unit tests for now
// LCOV_EXCL_START
GeomCurvePtr GeomAPI_Curve::basisCurve() const
{
  Handle(Geom_Curve) aCurve = MY_CURVE;
  if (aCurve->DynamicType() == STANDARD_TYPE(Geom_TrimmedCurve))
    aCurve = Handle(Geom_TrimmedCurve)::DownCast(aCurve)->BasisCurve();

  GeomCurvePtr aNewCurve(new GeomAPI_Curve);
  aNewCurve->setImpl(new Handle(Geom_Curve)(aCurve));
  return aNewCurve;
}
// LCOV_EXCL_STOP

// ================================================================================================

bool GeomAPI_Curve::Comparator::operator()(const GeomCurvePtr& theCurve1,
                                           const GeomCurvePtr& theCurve2) const
{
  const Handle(Geom_Curve)& aCurve1 = theCurve1->impl<Handle_Geom_Curve>();
  const Handle(Geom_Curve)& aCurve2 = theCurve2->impl<Handle_Geom_Curve>();
  return aCurve1.get() < aCurve2.get();
}
