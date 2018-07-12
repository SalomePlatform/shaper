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

#include "GeomAlgoAPI_WireBuilder.h"

#include <GeomAPI_Edge.h>
#include <GeomAPI_Pnt.h>
#include <GeomAPI_Vertex.h>
#include <GeomAPI_ShapeExplorer.h>

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

//=================================================================================================
bool GeomAlgoAPI_WireBuilder::isSelfIntersected(const std::shared_ptr<GeomAPI_Shape>& theWire)
{
  // Collect edges.
  ListOfShape anEdges;

  for (GeomAPI_ShapeExplorer anExp(theWire, GeomAPI_Shape::EDGE); anExp.more(); anExp.next()) {
    GeomShapePtr anEdge = anExp.current();
    anEdges.push_back(anEdge);
  }

  // Check intersections between edges pair-wise
  int aNbEdges = (int)anEdges.size();
  std::list<std::shared_ptr<GeomAPI_Shape> >::const_iterator anEdgesIt = anEdges.begin();
  for (int i = 0; anEdgesIt != anEdges.end(); ++anEdgesIt, i++) {
    std::shared_ptr<GeomAPI_Edge> anEdge1(new GeomAPI_Edge(*anEdgesIt));

    std::list<std::shared_ptr<GeomAPI_Shape> >::const_iterator anOtherEdgesIt = std::next(anEdgesIt);
    for (int j = i + 1; anOtherEdgesIt != anEdges.end(); ++anOtherEdgesIt, j++) {
      std::shared_ptr<GeomAPI_Edge> anEdge2(new GeomAPI_Edge(*anOtherEdgesIt));
      GeomShapePtr anInter = anEdge1->intersect(anEdge2);
      if (!anInter.get()) {
        continue;
      }

      bool isOk = false;

      if (anInter->isVertex()) {
        std::shared_ptr<GeomAPI_Vertex> aVertex(new GeomAPI_Vertex(anInter));
        std::shared_ptr<GeomAPI_Pnt> aPnt = aVertex->point();

        GeomPointPtr aFirstPnt1 = anEdge1->orientation() == GeomAPI_Shape::FORWARD ?
                                  anEdge1->firstPoint() : anEdge1->lastPoint();
        GeomPointPtr aLastPnt1 = anEdge1->orientation() == GeomAPI_Shape::FORWARD ?
                                 anEdge1->lastPoint() : anEdge1->firstPoint();
        GeomPointPtr aFirstPnt2 = anEdge2->orientation() == GeomAPI_Shape::FORWARD ?
                                  anEdge2->firstPoint() : anEdge2->lastPoint();
        GeomPointPtr aLastPnt2 = anEdge2->orientation() == GeomAPI_Shape::FORWARD ?
                                 anEdge2->lastPoint() : anEdge2->firstPoint();

        std::shared_ptr<GeomAPI_Pnt> aCommonEndPnt;
        if (aFirstPnt1->isEqual(aLastPnt2)) {
          aCommonEndPnt = aFirstPnt1;
        } else if(aLastPnt1->isEqual(aFirstPnt2)) {
          aCommonEndPnt = aLastPnt1;
        }

        isOk = aCommonEndPnt && aPnt->isEqual(aCommonEndPnt);
      }

      if (!isOk) {
        return true;
      }
    }
  }

  return false;
}