// Copyright (C) 2014-2022  CEA/DEN, EDF R&D
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

#include "ModuleBase_ArrowPrs.h"

#include <Prs3d_ArrowAspect.hxx>

#include <Events_Loop.h>
#include <ModelAPI_Events.h>

#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <GCPnts_AbscissaPoint.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <Prs3d_Arrow.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopExp.hxx>


IMPLEMENT_STANDARD_RTTIEXT(ModuleBase_ArrowPrs, AIS_InteractiveContext)


ModuleBase_ArrowPrs::ModuleBase_ArrowPrs(const Handle(V3d_Viewer)& theViewer,
                                         const GeomEdgePtr& theEdge)
  : AIS_InteractiveContext(theViewer),
    myEdge(theEdge)
{
}

//********************************************************************
void ModuleBase_ArrowPrs::DrawArrow(const Handle(Prs3d_Presentation)& thePrs,
                                    Quantity_Color theColor)
{
  Handle(Prs3d_Drawer) aDrawer = myDefaultDrawer;
  Handle(Prs3d_ArrowAspect) anArrowAspect = aDrawer->ArrowAspect();

  Handle(Graphic3d_AspectLine3d) PtA = anArrowAspect->Aspect();
  PtA->SetColor(theColor);

  Handle(Graphic3d_Group) TheGroup = thePrs->CurrentGroup();
  TheGroup->SetPrimitivesAspect(PtA);

  TopoDS_Vertex aV1, aV2;
  TopoDS_Edge anEdgeE = myEdge->impl<TopoDS_Edge>();
  anEdgeE.Orientation(TopAbs_FORWARD);
  if (anEdgeE.IsNull()) return;

  TopExp::Vertices(anEdgeE, aV1, aV2);
  gp_Pnt aP1 = BRep_Tool::Pnt(aV1);
  gp_Pnt aP2 = BRep_Tool::Pnt(aV2);

  double fp, lp;
  gp_Vec aDirVec;
  Handle(Geom_Curve) C = BRep_Tool::Curve(anEdgeE, fp, lp);

  if (C.IsNull()) return;

  if (anEdgeE.Orientation() == TopAbs_FORWARD)
    C->D1(lp, aP2, aDirVec);
  else {
    C->D1(fp, aP1, aDirVec);
    aP2 = aP1;
  }

  GeomAdaptor_Curve aAdC;
  aAdC.Load(C, fp, lp);
  Standard_Real aDist = GCPnts_AbscissaPoint::Length(aAdC, fp, lp);

  if (aDist > gp::Resolution()) {
    gp_Dir aDir;
    if (anEdgeE.Orientation() == TopAbs_FORWARD)
      aDir = aDirVec;
    else
      aDir = -aDirVec;

    TopoDS_Vertex aVertex;
    BRep_Builder aB;
    aB.MakeVertex(aVertex, aP2, Precision::Confusion());
    Prs3d_Arrow::Draw(TheGroup, aP2, aDir, M_PI / 180. * 5., aDist / 10.);
  }
}

//********************************************************************
bool ModuleBase_ArrowPrs::Comparator::operator()(const std::shared_ptr<GeomAPI_Edge>& theEdge1,
                                                 const std::shared_ptr<GeomAPI_Edge>& theEdge2) const
{
  const TopoDS_Edge& aShape1 = theEdge1->impl<TopoDS_Edge>();
  const TopoDS_Edge& aShape2 = theEdge2->impl<TopoDS_Edge>();
  bool isLess = aShape1.TShape() < aShape2.TShape();
  if (aShape1.TShape() == aShape2.TShape()) {
    Standard_Integer aHash1 = aShape1.Location().HashCode(IntegerLast());
    Standard_Integer aHash2 = aShape2.Location().HashCode(IntegerLast());
    isLess = aHash1 < aHash2;
  }
  return isLess;
}
