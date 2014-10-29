// File:        GeomAPI_PlanarEdges.cpp
// Created:     06 Oct 2014
// Author:      Sergey BELASH

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

void GeomAPI_PlanarEdges::addEdge(boost::shared_ptr<GeomAPI_Shape> theEdge)
{
  const TopoDS_Edge& anEdge = theEdge->impl<TopoDS_Edge>();
  if (anEdge.ShapeType() != TopAbs_EDGE)
    return;
  TopoDS_Shape& aWire = const_cast<TopoDS_Shape&>(impl<TopoDS_Shape>());
  BRep_Builder aBuilder;
  aBuilder.Add(aWire, anEdge);
}

std::list<boost::shared_ptr<GeomAPI_Shape> > GeomAPI_PlanarEdges::getEdges()
{
  TopoDS_Shape& aShape = const_cast<TopoDS_Shape&>(impl<TopoDS_Shape>());
  //BRepTools_WireExplorer aWireExp(TopoDS::Wire(aShape));
  TopExp_Explorer aWireExp(aShape, TopAbs_EDGE);
  std::list<boost::shared_ptr<GeomAPI_Shape> > aResult;
  for (; aWireExp.More(); aWireExp.Next()) {
    boost::shared_ptr<GeomAPI_Shape> anEdge(new GeomAPI_Shape);
    anEdge->setImpl(new TopoDS_Shape(aWireExp.Current()));
    aResult.push_back(anEdge);
  }
  return aResult;
}
