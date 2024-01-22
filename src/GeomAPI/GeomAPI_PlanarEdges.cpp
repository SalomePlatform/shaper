// Copyright (C) 2014-2024  CEA, EDF
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

#include <GeomAPI_Interface.h>
#include <GeomAPI_PlanarEdges.h>

#include <Standard_TypeDef.hxx>
#include <TopAbs_ShapeEnum.hxx>

#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Wire.hxx>
#include <BRep_Builder.hxx>
#include <BRepTools_WireExplorer.hxx>
#include <TopExp_Explorer.hxx>

#include <list>

GeomAPI_PlanarEdges::GeomAPI_PlanarEdges() : GeomAPI_Shape()
{
  TopoDS_Compound aBigWireImpl;
  BRep_Builder aBuilder;
  aBuilder.MakeCompound(aBigWireImpl);
  this->setImpl(new TopoDS_Shape(aBigWireImpl));
}

void GeomAPI_PlanarEdges::addEdge(std::shared_ptr<GeomAPI_Shape> theEdge)
{
  const TopoDS_Edge& anEdge = theEdge->impl<TopoDS_Edge>();
  if (anEdge.ShapeType() != TopAbs_EDGE)
    return;
  TopoDS_Shape& aWire = const_cast<TopoDS_Shape&>(impl<TopoDS_Shape>());
  BRep_Builder aBuilder;
  aBuilder.Add(aWire, anEdge);
}

std::list<std::shared_ptr<GeomAPI_Shape> > GeomAPI_PlanarEdges::getEdges()
{
  TopoDS_Shape& aShape = const_cast<TopoDS_Shape&>(impl<TopoDS_Shape>());
  TopExp_Explorer aWireExp(aShape, TopAbs_EDGE);
  std::list<std::shared_ptr<GeomAPI_Shape> > aResult;
  for (; aWireExp.More(); aWireExp.Next()) {
    std::shared_ptr<GeomAPI_Shape> anEdge(new GeomAPI_Shape);
    anEdge->setImpl(new TopoDS_Shape(aWireExp.Current()));
    aResult.push_back(anEdge);
  }
  return aResult;
}

bool GeomAPI_PlanarEdges::hasPlane() const {
  return myPlane.get() != NULL;
}

bool GeomAPI_PlanarEdges::isVertex() const {
  return false;
}

bool GeomAPI_PlanarEdges::isEdge() const {
  return false;
}

std::shared_ptr<GeomAPI_Pnt> GeomAPI_PlanarEdges::origin() const
{
  if (hasPlane())
    return myPlane->origin();
  return std::shared_ptr<GeomAPI_Pnt>();
}

std::shared_ptr<GeomAPI_Dir> GeomAPI_PlanarEdges::dirX() const
{
  if (hasPlane())
    return myPlane->dirX();
  return std::shared_ptr<GeomAPI_Dir>();
}

std::shared_ptr<GeomAPI_Dir> GeomAPI_PlanarEdges::dirY() const
{
  if (hasPlane())
    return myPlane->dirY();
  return std::shared_ptr<GeomAPI_Dir>();
}

std::shared_ptr<GeomAPI_Dir> GeomAPI_PlanarEdges::norm() const
{
  if (hasPlane())
    return myPlane->normal();
  return std::shared_ptr<GeomAPI_Dir>();
}

bool GeomAPI_PlanarEdges::isPlanar() const
{
  return true;
}

void GeomAPI_PlanarEdges::setPlane(const std::shared_ptr<GeomAPI_Pnt>& theOrigin,
                                   const std::shared_ptr<GeomAPI_Dir>& theDirX,
                                   const std::shared_ptr<GeomAPI_Dir>& theNorm)
{
  myPlane = std::shared_ptr<GeomAPI_Ax3>(new GeomAPI_Ax3(theOrigin, theDirX, theNorm));
}

bool GeomAPI_PlanarEdges::isEqual(const std::shared_ptr<GeomAPI_Shape> theShape) const
{
  if (!theShape.get())
    return false;
  TopoDS_Shape& aMyShape = const_cast<TopoDS_Shape&>(impl<TopoDS_Shape>());
  TopoDS_Shape aTheShape = theShape->impl<TopoDS_Shape>();
  if (aMyShape.ShapeType() != aTheShape.ShapeType()) // to don't confuse by the face of same edges
    return false;
  TopExp_Explorer aMyExp(aMyShape, TopAbs_EDGE);
  TopExp_Explorer aTheExp(aTheShape, TopAbs_EDGE);
  for (; aMyExp.More() && aTheExp.More(); aMyExp.Next(), aTheExp.Next()) {
    // check that edge by edge all geometrically matches
    std::shared_ptr<GeomAPI_Edge> aMyEdge(new GeomAPI_Edge);
    aMyEdge->setImpl<TopoDS_Shape>(new TopoDS_Shape(aMyExp.Current()));
    std::shared_ptr<GeomAPI_Edge> aTheEdge(new GeomAPI_Edge);
    aTheEdge->setImpl<TopoDS_Shape>(new TopoDS_Shape(aTheExp.Current()));
    if (!aMyEdge->isEqual(aTheEdge))
      return false;
  }
  return !(aMyExp.More() || aTheExp.More());
}
