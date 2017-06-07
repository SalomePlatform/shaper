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
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com<mailto:webmaster.salome@opencascade.com>
//

#include "GeomAlgoAPI_WireBuilder.h"

#include <BRepBuilderAPI_MakeWire.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Wire.hxx>
#include <TopExp_Explorer.hxx>

//=================================================================================================
std::shared_ptr<GeomAPI_Shape> GeomAlgoAPI_WireBuilder::wire(const ListOfShape& theShapes)
{
  TopTools_ListOfShape aListOfEdges;

  for(ListOfShape::const_iterator anIt = theShapes.cbegin(); anIt != theShapes.cend(); ++anIt) {
    const TopoDS_Shape& aShape = (*anIt)->impl<TopoDS_Shape>();
    switch(aShape.ShapeType()) {
      case TopAbs_EDGE: {
        aListOfEdges.Append(aShape);
        break;
      }
      case TopAbs_WIRE: {
        for(TopExp_Explorer anExp(aShape, TopAbs_EDGE); anExp.More(); anExp.Next()) {
          aListOfEdges.Append(anExp.Current());
        }
        break;
      }
      default: {
        return GeomShapePtr();
      }
    }
  }

  BRepBuilderAPI_MakeWire aWireBuilder;
  aWireBuilder.Add(aListOfEdges);
  if(aWireBuilder.Error() != BRepBuilderAPI_WireDone) {
    return GeomShapePtr();
  }

  GeomShapePtr aResultShape(new GeomAPI_Shape());
  aResultShape->setImpl(new TopoDS_Shape(aWireBuilder.Wire()));
  return aResultShape;
}
