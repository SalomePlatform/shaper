// Copyright (C) 2014-2025  CEA, EDF
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

#include <GeomAlgoAPI_STLImport.h>

#include "GeomAlgoAPI_Tools.h"

#include <Basics_OCCTVersion.hxx>

#include <TopoDS.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Shell.hxx>
#include <BRepBuilderAPI_MakeSolid.hxx>
#include <BRepBuilderAPI_Sewing.hxx>
#include <StlAPI_Reader.hxx>

std::shared_ptr<GeomAPI_Shape> STLImport(const std::string& theFileName,
                                         std::string& theError)
{
  TopoDS_Shape aResShape;
  StlAPI_Reader aReader;

  try
  {
    if(!aReader.Read(aResShape, theFileName.c_str()))
    {
      theError = "Can't import file.";
      aResShape.Nullify();
    }
#if OCC_VERSION_LARGE >= 0x07070000
    BRepBuilderAPI_Sewing aSewingTool;
    aSewingTool.Init(1.0e-06, Standard_True);
    aSewingTool.Load(aResShape);
    aSewingTool.Perform();
    TopoDS_Shape aSewedShape = aSewingTool.SewedShape();
    if (!aSewedShape.IsNull())
      aResShape = aSewedShape;
#endif
    if(aResShape.ShapeType() == TopAbs_SHELL) {
      if (aResShape.Closed()) {
        BRepBuilderAPI_MakeSolid soliMaker(TopoDS::Shell(aResShape));
        soliMaker.Build();
        if(soliMaker.IsDone())
          aResShape = soliMaker.Shape();
      }
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
