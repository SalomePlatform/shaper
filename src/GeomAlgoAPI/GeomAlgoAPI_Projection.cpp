// Copyright (C) 2019  CEA/DEN, EDF R&D
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

#include <GeomAlgoAPI_Projection.h>

#include <GeomAPI_Curve.h>
#include <GeomAPI_Edge.h>
#include <GeomAPI_Pln.h>

#include <Geom_Curve.hxx>
#include <Geom_Plane.hxx>
#include <GeomProjLib.hxx>

#include <BRep_Tool.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>

GeomAlgoAPI_Projection::GeomAlgoAPI_Projection(const GeomPlanePtr& thePlane)
  : myPlane(thePlane)
{
}

GeomCurvePtr GeomAlgoAPI_Projection::project(const GeomCurvePtr& theCurve)
{
  Handle(Geom_Curve) aCurve = theCurve->impl<Handle_Geom_Curve>();
  Handle(Geom_Plane) aPlane = new Geom_Plane(myPlane->impl<gp_Ax3>());

  Handle(Geom_Curve) aProj = GeomProjLib::Project(aCurve, aPlane);

  GeomCurvePtr aProjCurve(new GeomAPI_Curve);
  aProjCurve->setImpl(new Handle_Geom_Curve(aProj));
  return aProjCurve;
}

GeomCurvePtr GeomAlgoAPI_Projection::project(const GeomEdgePtr& theEdge)
{
  GeomCurvePtr aCurve(new GeomAPI_Curve);

  const TopoDS_Shape& aShape = theEdge->impl<TopoDS_Shape>();
  TopoDS_Edge anEdge = TopoDS::Edge(aShape);
  if (!anEdge.IsNull()) {
    double aStart, aEnd;
    Handle(Geom_Curve) anEdgeCurve = BRep_Tool::Curve(anEdge, aStart, aEnd);
    if (!anEdgeCurve.IsNull() && !BRep_Tool::IsClosed(anEdge))
      anEdgeCurve = new Geom_TrimmedCurve(anEdgeCurve, aStart, aEnd);
    aCurve->setImpl(new Handle_Geom_Curve(anEdgeCurve));
  }

  return project(aCurve);
}
