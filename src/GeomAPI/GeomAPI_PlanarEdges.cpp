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

bool GeomAPI_PlanarEdges::hasPlane() const {
  return myOrigin && myNorm && myDirX && myDirY;
}

bool GeomAPI_PlanarEdges::isVertex() const {
  return false;
}

bool GeomAPI_PlanarEdges::isEdge() const {
  return false;
}

void GeomAPI_PlanarEdges::setOrigin(const boost::shared_ptr<GeomAPI_Pnt>& theOrigin)
{
  myOrigin = theOrigin;
}
boost::shared_ptr<GeomAPI_Pnt> GeomAPI_PlanarEdges::origin() const {
  return myOrigin;
}
void GeomAPI_PlanarEdges::setDirX(const boost::shared_ptr<GeomAPI_Dir>& theDirX) {
  myDirX = theDirX;
}
boost::shared_ptr<GeomAPI_Dir> GeomAPI_PlanarEdges::dirX() const {
  return myDirX;
}
void GeomAPI_PlanarEdges::setDirY(const boost::shared_ptr<GeomAPI_Dir>& theDirY) {
  myDirY = theDirY;
}
boost::shared_ptr<GeomAPI_Dir> GeomAPI_PlanarEdges::dirY() const {
  return myDirY;
}
void GeomAPI_PlanarEdges::setNorm(const boost::shared_ptr<GeomAPI_Dir>& theNorm) {
  myNorm = theNorm;
}
boost::shared_ptr<GeomAPI_Dir> GeomAPI_PlanarEdges::norm() const {
  return myNorm;
}
