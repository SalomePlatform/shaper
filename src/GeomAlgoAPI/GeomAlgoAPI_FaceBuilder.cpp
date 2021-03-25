// Copyright (C) 2014-2021  CEA/DEN, EDF R&D
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

#include "GeomAlgoAPI_FaceBuilder.h"

#include <GeomAPI_Face.h>
#include <GeomAPI_Dir.h>
#include <GeomAPI_Pln.h>
#include <GeomAPI_Pnt.h>
#include <GeomAPI_Shape.h>
#include <GeomAPI_Vertex.h>

#include <BRep_Tool.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <GC_MakePlane.hxx>
#include <Geom_Plane.hxx>
#include <gp_Pln.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Face.hxx>

//==================================================================================================
std::shared_ptr<GeomAPI_Face> GeomAlgoAPI_FaceBuilder::squareFace(
  const std::shared_ptr<GeomAPI_Pnt> theCenter,
  const std::shared_ptr<GeomAPI_Dir> theNormal,
  const double theSize)
{
  const gp_Pnt& aCenter = theCenter->impl<gp_Pnt>();
  const gp_Dir& aDir = theNormal->impl<gp_Dir>();
  gp_Pln aPlane(aCenter, aDir);
  // half of the size in each direction from the center
  BRepBuilderAPI_MakeFace aFaceBuilder(aPlane, -theSize / 2., theSize / 2., -theSize / 2.,
                                       theSize / 2.);
  std::shared_ptr<GeomAPI_Face> aRes(new GeomAPI_Face());
  aRes->setImpl(new TopoDS_Face(aFaceBuilder.Face()));
  return aRes;
}

//==================================================================================================
std::shared_ptr<GeomAPI_Face> GeomAlgoAPI_FaceBuilder::squareFace(
  const std::shared_ptr<GeomAPI_Pln> thePlane,
  const double theSize)
{
  // half of the size in each direction from the center
  BRepBuilderAPI_MakeFace aFaceBuilder(thePlane->impl<gp_Pln>(),
                                       -theSize / 2., theSize / 2.,
                                       -theSize / 2., theSize / 2.);
  std::shared_ptr<GeomAPI_Face> aRes(new GeomAPI_Face());
  const TopoDS_Face& aFace = aFaceBuilder.Face();
  aRes->setImpl(new TopoDS_Face(aFace));
  return aRes;
}

//==================================================================================================
std::shared_ptr<GeomAPI_Face> GeomAlgoAPI_FaceBuilder::planarFace(
  const std::shared_ptr<GeomAPI_Pnt> theCenter,
  const std::shared_ptr<GeomAPI_Dir> theNormal)
{
  const gp_Pnt& aCenter = theCenter->impl<gp_Pnt>();
  const gp_Dir& aDir = theNormal->impl<gp_Dir>();
  gp_Pln aPlane(aCenter, aDir);
  BRepBuilderAPI_MakeFace aFaceBuilder(aPlane);
  std::shared_ptr<GeomAPI_Face> aRes(new GeomAPI_Face());
  aRes->setImpl(new TopoDS_Face(aFaceBuilder.Face()));
  return aRes;
}

//==================================================================================================
std::shared_ptr<GeomAPI_Face> GeomAlgoAPI_FaceBuilder::planarFace(
  const std::shared_ptr<GeomAPI_Pln> thePlane,
  const double theX, const double theY,
  const double theWidth, const double theHeight)
{
  double aA, aB, aC, aD;
  thePlane->coefficients(aA, aB, aC, aD);
  gp_Pln aPlane(aA, aB, aC, aD);

  // half of the size in each direction from the center
  BRepBuilderAPI_MakeFace aFaceBuilder(aPlane, theX, theX + theWidth,
                                       theY, theY + theHeight);
  std::shared_ptr<GeomAPI_Face> aRes(new GeomAPI_Face());
  aRes->setImpl(new TopoDS_Face(aFaceBuilder.Face()));
  return aRes;
}

//==================================================================================================
std::shared_ptr<GeomAPI_Face> GeomAlgoAPI_FaceBuilder::planarFaceByThreeVertices(
    const std::shared_ptr<GeomAPI_Vertex> theVertex1,
    const std::shared_ptr<GeomAPI_Vertex> theVertex2,
    const std::shared_ptr<GeomAPI_Vertex> theVertex3)
{
  gp_Pnt aPnt1 = theVertex1->point()->impl<gp_Pnt>();
  gp_Pnt aPnt2 = theVertex2->point()->impl<gp_Pnt>();
  gp_Pnt aPnt3 = theVertex3->point()->impl<gp_Pnt>();

  std::shared_ptr<GeomAPI_Face> aFace;
  GC_MakePlane aMakePlane(aPnt1, aPnt2, aPnt3);
  if(!aMakePlane.IsDone()) {
    return aFace;
  }

  BRepBuilderAPI_MakeFace aMakeFace(aMakePlane.Value()->Pln());
  aFace.reset(new GeomAPI_Face());
  aFace->setImpl(new TopoDS_Face(aMakeFace.Face()));
  return aFace;
}
