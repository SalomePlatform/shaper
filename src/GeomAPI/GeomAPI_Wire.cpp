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

#include "GeomAPI_Wire.h"
#include "GeomAPI_Pnt.h"

#include <BRep_Builder.hxx>
#include <BRepTools_WireExplorer.hxx>
#include <Geom_Line.hxx>
#include <Precision.hxx>
#include <Standard_Type.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Wire.hxx>

//==================================================================================================
GeomAPI_Wire::GeomAPI_Wire()
{
  TopoDS_Wire* aWire = new TopoDS_Wire();

  BRep_Builder aBuilder;
  aBuilder.MakeWire(*aWire);

  this->setImpl(aWire);
}

//==================================================================================================
GeomAPI_Wire::GeomAPI_Wire(const std::shared_ptr<GeomAPI_Shape>& theShape)
{
  if (!theShape->isNull() && theShape->isWire()) {
    setImpl(new TopoDS_Shape(theShape->impl<TopoDS_Shape>()));
  }
}

//==================================================================================================
bool GeomAPI_Wire::isClosed() const
{
  return BRep_Tool::IsClosed(impl<TopoDS_Shape>());
}

//==================================================================================================
bool GeomAPI_Wire::isPolygon(std::list<GeomPointPtr>& thePoints) const
{
  const TopoDS_Wire& aWire = TopoDS::Wire(impl<TopoDS_Shape>());

  bool isPolygon = true;
  const Handle(Standard_Type)& aLineType = STANDARD_TYPE(Geom_Line);
  for (BRepTools_WireExplorer anExp(aWire); anExp.More() && isPolygon; anExp.Next()) {
    const TopoDS_Edge& anEdge = anExp.Current();
    double aT1, aT2;
    Handle(Geom_Curve) aC3D = BRep_Tool::Curve(anEdge, aT1, aT2);
    if (!aC3D.IsNull() && aC3D->IsKind(aLineType)) {
      gp_Pnt aCorner = BRep_Tool::Pnt(anExp.CurrentVertex());
      thePoints.push_back(GeomPointPtr(new GeomAPI_Pnt(aCorner.X(), aCorner.Y(), aCorner.Z())));
    }
    else
      isPolygon = false;
  }

  if (!isPolygon)
    thePoints.clear();
  return isPolygon;
}

//==================================================================================================
bool GeomAPI_Wire::isRectangle(std::list<GeomPointPtr>& thePoints) const
{
  const TopoDS_Wire& aWire = TopoDS::Wire(impl<TopoDS_Shape>());
  const Handle(Standard_Type)& aLineType = STANDARD_TYPE(Geom_Line);

  gp_XYZ aPrevDir(0, 0, 0);

  for (BRepTools_WireExplorer anExp(aWire); anExp.More(); anExp.Next()) {
    const TopoDS_Edge& anEdge = anExp.Current();
    double aT1, aT2;
    Handle(Geom_Curve) aC3D = BRep_Tool::Curve(anEdge, aT1, aT2);
    if (!aC3D.IsNull() && aC3D->IsKind(aLineType)) {
      gp_Pnt aCorner = BRep_Tool::Pnt(anExp.CurrentVertex());
      thePoints.push_back(GeomPointPtr(new GeomAPI_Pnt(aCorner.X(), aCorner.Y(), aCorner.Z())));
    }
    else
      return false;

    if (thePoints.size() > 4)
      return false;

    // collect length of the edge
    gp_Pnt aStart = aC3D->Value(aT1);
    gp_Pnt aEnd = aC3D->Value(aT2);

    // check the edge is orthogonal to the previous
    gp_XYZ aCurDir = (aEnd.XYZ() - aStart.XYZ()).Normalized();
    if (aPrevDir.Dot(aCurDir) < Precision::Confusion())
      aPrevDir = aCurDir;
    else
      return false;
  }
  return true;
}
