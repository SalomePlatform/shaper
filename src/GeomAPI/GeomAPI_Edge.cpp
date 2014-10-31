// File:        GeomAPI_Edge.cpp
// Created:     24 Jul 2014
// Author:      Artem ZHIDKOV

#include<GeomAPI_Edge.h>
#include<GeomAPI_Pnt.h>
#include<GeomAPI_Circ.h>
#include<GeomAPI_Dir.h>

#include <TopoDS_Shape.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS.hxx>
#include <BRep_Tool.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Line.hxx>
#include <Geom_Circle.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <gp_Ax1.hxx>

GeomAPI_Edge::GeomAPI_Edge()
  : GeomAPI_Shape()
{
}

GeomAPI_Edge::GeomAPI_Edge(const boost::shared_ptr<GeomAPI_Shape>& theShape)
{
  if (!theShape->isNull() && theShape->isEdge()) {
    setImpl(new TopoDS_Shape(theShape->impl<TopoDS_Shape>()));
  }
}

bool GeomAPI_Edge::isLine() const
{
  const TopoDS_Shape& aShape = const_cast<GeomAPI_Edge*>(this)->impl<TopoDS_Shape>();
  double aFirst, aLast;
  Handle(Geom_Curve) aCurve = BRep_Tool::Curve((const TopoDS_Edge&)aShape, aFirst, aLast);
  if (aCurve->IsKind(STANDARD_TYPE(Geom_Line)))
    return true;
  return false;
}

bool GeomAPI_Edge::isCircle() const
{
  const TopoDS_Shape& aShape = const_cast<GeomAPI_Edge*>(this)->impl<TopoDS_Shape>();
  double aFirst, aLast;
  Handle(Geom_Curve) aCurve = BRep_Tool::Curve((const TopoDS_Edge&)aShape, aFirst, aLast);
  if (aCurve->IsKind(STANDARD_TYPE(Geom_Circle)) && aCurve->IsClosed())
    return true;
  return false;
}

bool GeomAPI_Edge::isArc() const
{
  const TopoDS_Shape& aShape = const_cast<GeomAPI_Edge*>(this)->impl<TopoDS_Shape>();
  double aFirst, aLast;
  Handle(Geom_Curve) aCurve = BRep_Tool::Curve((const TopoDS_Edge&)aShape, aFirst, aLast);
  if (aCurve->IsKind(STANDARD_TYPE(Geom_Circle)) && !aCurve->IsClosed())
    return true;
  return false;
}

boost::shared_ptr<GeomAPI_Pnt> GeomAPI_Edge::firstPoint()
{
  const TopoDS_Shape& aShape = const_cast<GeomAPI_Edge*>(this)->impl<TopoDS_Shape>();
  double aFirst, aLast;
  Handle(Geom_Curve) aCurve = BRep_Tool::Curve((const TopoDS_Edge&)aShape, aFirst, aLast);
  gp_Pnt aPoint;
  aCurve->D0(aFirst, aPoint);
  return boost::shared_ptr<GeomAPI_Pnt>(new GeomAPI_Pnt(aPoint.X(), aPoint.Y(), aPoint.Z()));
}

boost::shared_ptr<GeomAPI_Pnt> GeomAPI_Edge::lastPoint()
{
  const TopoDS_Shape& aShape = const_cast<GeomAPI_Edge*>(this)->impl<TopoDS_Shape>();
  double aFirst, aLast;
  Handle(Geom_Curve) aCurve = BRep_Tool::Curve((const TopoDS_Edge&)aShape, aFirst, aLast);
  gp_Pnt aPoint;
  aCurve->D0(aLast, aPoint);
  return boost::shared_ptr<GeomAPI_Pnt>(new GeomAPI_Pnt(aPoint.X(), aPoint.Y(), aPoint.Z()));
}

boost::shared_ptr<GeomAPI_Circ> GeomAPI_Edge::circle()
{
  const TopoDS_Shape& aShape = const_cast<GeomAPI_Edge*>(this)->impl<TopoDS_Shape>();
  double aFirst, aLast;
  Handle(Geom_Curve) aCurve = BRep_Tool::Curve((const TopoDS_Edge&)aShape, aFirst, aLast);
  if (aCurve) {
    Handle(Geom_Circle) aCirc = Handle(Geom_Circle)::DownCast(aCurve);
    if (aCirc) {
      gp_Pnt aLoc = aCirc->Location();
      boost::shared_ptr<GeomAPI_Pnt> aCenter(new GeomAPI_Pnt(aLoc.X(), aLoc.Y(), aLoc.Z()));
      gp_Dir anAxis = aCirc->Axis().Direction();
      boost::shared_ptr<GeomAPI_Dir> aDir(new GeomAPI_Dir(anAxis.X(), anAxis.Y(), anAxis.Z()));
      return boost::shared_ptr<GeomAPI_Circ>(new GeomAPI_Circ(aCenter, aDir, aCirc->Radius()));
    }
  }
  return boost::shared_ptr<GeomAPI_Circ>(); // not circle
}


bool GeomAPI_Edge::isEqual(boost::shared_ptr<GeomAPI_Shape> theEdge)
{
  const TopoDS_Shape& aMyShape = const_cast<GeomAPI_Edge*>(this)->impl<TopoDS_Shape>();
  const TopoDS_Shape& aInShape = theEdge->impl<TopoDS_Shape>();
  
  double aMyStart, aMyEnd;
  Handle(Geom_Curve) aMyCurve = BRep_Tool::Curve(TopoDS::Edge(aMyShape), aMyStart, aMyEnd);
  double aInStart, aInEnd;
  Handle(Geom_Curve) aInCurve = BRep_Tool::Curve(TopoDS::Edge(aInShape), aInStart, aInEnd);

  // Check that curves a the same type
  GeomAdaptor_Curve aMyAdaptor(aMyCurve);
  GeomAdaptor_Curve aInAdaptor(aInCurve);
  if (aMyAdaptor.GetType() != aInAdaptor.GetType())
    return false;

  // Check that end point parameters are the same
  if ((aMyStart != aInStart) || (aMyEnd != aInEnd))
    return false;

  // Check that end points are equal
  gp_Pnt aMyPnt1 = aMyAdaptor.Value(aMyStart);
  gp_Pnt aMyPnt2 = aMyAdaptor.Value(aMyEnd);
  gp_Pnt aInPnt1 = aInAdaptor.Value(aInStart);
  gp_Pnt aInPnt2 = aInAdaptor.Value(aInEnd);

  if ((!aMyPnt1.IsEqual(aInPnt1, Precision::Confusion())) || 
    (!aMyPnt2.IsEqual(aInPnt2, Precision::Confusion())))
    return false;

  return true;
}