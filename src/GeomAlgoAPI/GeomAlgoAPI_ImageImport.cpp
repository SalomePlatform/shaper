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

#include <GeomAlgoAPI_ImageImport.h>

#include <BRep_Builder.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepBuilderAPI_MakePolygon.hxx>
#include <BRepBuilderAPI_MakeVertex.hxx>
#include <BRepBuilderAPI_Sewing.hxx>
#include <gp_Pnt.hxx>
#include <OSD_Path.hxx>
#include <RWStl.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Shell.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Wire.hxx>

std::shared_ptr<GeomAPI_Shape> ImageImport(const int width,
                                           const int height,
                                           std::string& theError)
{
  TopoDS_Shape aResShape;
  try
  {
    TopoDS_Vertex aTriVertexes[4];
    gp_Pnt aPnt1( -0.5*width, -0.5*height, 0);
    gp_Pnt aPnt2( 0.5*width, -0.5*height, 0);
    gp_Pnt aPnt3(  0.5*width,  0.5*height, 0 );;
    gp_Pnt aPnt4( -0.5*width,  0.5*height, 0 );
    TopoDS_Face aFace;
    TopoDS_Wire aWire;

    aTriVertexes[0] = BRepBuilderAPI_MakeVertex (aPnt1);
    aTriVertexes[1] = BRepBuilderAPI_MakeVertex (aPnt2);
    aTriVertexes[2] = BRepBuilderAPI_MakeVertex (aPnt3);
    aTriVertexes[3] = BRepBuilderAPI_MakeVertex (aPnt4);

    aWire = BRepBuilderAPI_MakePolygon (aTriVertexes[0], aTriVertexes[1], aTriVertexes[2],
        aTriVertexes[3], Standard_True);

    BRepBuilderAPI_Sewing aSewingTool;
    aSewingTool.Init (1.0e-06, Standard_True);

    TopoDS_Compound aComp;
    BRep_Builder BuildTool;
    BuildTool.MakeCompound(aComp);

    if(!aWire.IsNull())
    {
      aFace = BRepBuilderAPI_MakeFace (aWire);
      if (!aFace.IsNull())
      {
        BuildTool.Add (aComp, aFace);
      }
    }

    aSewingTool.Load (aComp);
    aSewingTool.Perform();
    aResShape = aSewingTool.SewedShape();
    if (aResShape.IsNull())
    {
      aResShape = aComp;
    }
  }
  catch (Standard_Failure const& anException) {
    theError = anException.GetMessageString();
    aResShape.Nullify();
  }

  std::shared_ptr<GeomAPI_Shape> aGeomShape(new GeomAPI_Shape);
  aGeomShape->setImpl(new TopoDS_Shape(aResShape));

  return aGeomShape;
}
