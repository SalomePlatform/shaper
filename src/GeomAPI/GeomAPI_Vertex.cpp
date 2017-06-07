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

#include<GeomAPI_Vertex.h>
#include<GeomAPI_Pnt.h>

#include <TopoDS_Shape.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Vertex.hxx>
#include <BRep_Tool.hxx>
#include <BRep_Builder.hxx>
#include <gp_Pnt.hxx>
#include <Precision.hxx>

GeomAPI_Vertex::GeomAPI_Vertex()
  : GeomAPI_Shape()
{
}

GeomAPI_Vertex::GeomAPI_Vertex(const std::shared_ptr<GeomAPI_Shape>& theShape)
{
  if (!theShape->isNull() && theShape->isVertex()) {
    setImpl(new TopoDS_Shape(theShape->impl<TopoDS_Shape>()));
  }
}

GeomAPI_Vertex::GeomAPI_Vertex(double theX, double theY, double theZ)
{
  TopoDS_Vertex aVertex;
  BRep_Builder aBuilder;
  aBuilder.MakeVertex(aVertex, gp_Pnt(theX, theY, theZ), Precision::Confusion());
  setImpl(new TopoDS_Shape(aVertex));
}

std::shared_ptr<GeomAPI_Pnt> GeomAPI_Vertex::point()
{
  const TopoDS_Shape& aShape = const_cast<GeomAPI_Vertex*>(this)->impl<TopoDS_Shape>();
  TopoDS_Vertex aVertex = TopoDS::Vertex(aShape);
  gp_Pnt aPoint = BRep_Tool::Pnt(aVertex);
  return std::shared_ptr<GeomAPI_Pnt>(new GeomAPI_Pnt(aPoint.X(), aPoint.Y(), aPoint.Z()));
}

bool GeomAPI_Vertex::isEqual(const std::shared_ptr<GeomAPI_Shape> theVert) const
{
  if (!theVert.get() || ! theVert->isVertex())
    return false;
  const TopoDS_Shape& aMyShape = const_cast<GeomAPI_Vertex*>(this)->impl<TopoDS_Shape>();
  const TopoDS_Shape& aInShape = theVert->impl<TopoDS_Shape>();

  if (aMyShape.ShapeType() != aInShape.ShapeType())
    return false;

  TopoDS_Vertex aVertex1 = TopoDS::Vertex(aMyShape);
  gp_Pnt aPoint1 = BRep_Tool::Pnt(aVertex1);

  TopoDS_Vertex aVertex2 = TopoDS::Vertex(aInShape);
  gp_Pnt aPoint2 = BRep_Tool::Pnt(aVertex2);

  return aPoint1.IsEqual(aPoint2, Precision::Confusion()) == Standard_True;
}
