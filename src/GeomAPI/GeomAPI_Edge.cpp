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

#include<GeomAPI_Edge.h>
#include<GeomAPI_Pln.h>
#include<GeomAPI_Pnt.h>
#include<GeomAPI_Circ.h>
#include<GeomAPI_Dir.h>
#include<GeomAPI_Lin.h>
#include<GeomAPI_Ax2.h>
#include<GeomAPI_Ellipse.h>
#include<GeomAPI_Vertex.h>

#include <BRepAdaptor_Curve.hxx>

#include <TopoDS_Shape.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS.hxx>
#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <ElCLib.hxx>
#include <GCPnts_UniformAbscissa.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Line.hxx>
#include <Geom_Circle.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <Geom_Ellipse.hxx>
#include <Geom_Plane.hxx>
#include <GeomAPI_ExtremaCurveCurve.hxx>
#include <GeomAPI_ExtremaCurveSurface.hxx>
#include <GeomAPI_IntCS.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <gp_Ax1.hxx>
#include <gp_Pln.hxx>
#include <gp_Elips.hxx>
#include <TopExp.hxx>

#include <GCPnts_AbscissaPoint.hxx>

GeomAPI_Edge::GeomAPI_Edge()
{
  TopoDS_Edge* anEdge = new TopoDS_Edge;

  BRep_Builder aBuilder;
  aBuilder.MakeEdge(*anEdge);

  setImpl(anEdge);
}

GeomAPI_Edge::GeomAPI_Edge(const std::shared_ptr<GeomAPI_Shape>& theShape)
{
  if (!theShape->isNull() && theShape->isEdge()) {
    setImpl(new TopoDS_Shape(theShape->impl<TopoDS_Shape>()));
  }
}

void GeomAPI_Edge::vertices(std::shared_ptr<GeomAPI_Vertex>& theStartVertex,
                            std::shared_ptr<GeomAPI_Vertex>& theEndVertex) const
{
  const TopoDS_Edge& anEdge = impl<TopoDS_Edge>();
  TopoDS_Vertex aStart, aEnd;
  TopExp::Vertices(anEdge, aStart, aEnd);
  theStartVertex.reset(new GeomAPI_Vertex);
  theStartVertex->setImpl(new TopoDS_Vertex(aStart));
  theEndVertex.reset(new GeomAPI_Vertex);
  theEndVertex->setImpl(new TopoDS_Vertex(aEnd));
}

static Handle(Geom_Curve) baseCurve(const TopoDS_Edge& theEdge)
{
  double aFirst, aLast;
  Handle(Geom_Curve) aCurve = BRep_Tool::Curve(theEdge, aFirst, aLast);
  while (aCurve->IsKind(STANDARD_TYPE(Geom_TrimmedCurve))) {
    Handle(Geom_TrimmedCurve) tc = Handle(Geom_TrimmedCurve)::DownCast(aCurve);
    aCurve = tc->BasisCurve();
  }
  return aCurve;
}

bool GeomAPI_Edge::isSameGeometry(const std::shared_ptr<GeomAPI_Shape> theShape) const
{
  if (!theShape->isEdge())
    return false;
  if (isSame(theShape))
    return true;

  TopoDS_Edge anOwnEdge = TopoDS::Edge(impl<TopoDS_Shape>());
  TopoDS_Edge anOtherEdge = TopoDS::Edge(theShape->impl<TopoDS_Shape>());

  Handle(Geom_Curve) anOwnCurve = baseCurve(anOwnEdge);
  Handle(Geom_Curve) anOtherCurve = baseCurve(anOtherEdge);
  GeomAPI_ExtremaCurveCurve anExtrema(anOwnCurve, anOtherCurve);

  bool isSame = anExtrema.Extrema().IsParallel() &&
                anExtrema.TotalLowerDistance() < Precision::Confusion();
  return isSame;
}

bool GeomAPI_Edge::isLine() const
{
  const TopoDS_Shape& aShape = const_cast<GeomAPI_Edge*>(this)->impl<TopoDS_Shape>();
  double aFirst, aLast;
  Handle(Geom_Curve) aCurve = BRep_Tool::Curve((const TopoDS_Edge&)aShape, aFirst, aLast);
  if (aCurve.IsNull()) // degenerative edge
    return false;
  if (aCurve->IsKind(STANDARD_TYPE(Geom_Line)))
    return true;
  return false;
}

/// extracts a circle curve from the arbitrary curve, returns null is it is different type
static Handle(Geom_Circle) circ(const Handle(Geom_Curve) theCurve)
{
  Handle(Geom_Circle) aResult = Handle(Geom_Circle)::DownCast(theCurve);
  if (!aResult.IsNull())
    return aResult;
  // check this may be a trimmed curve that contains circle inside
  Handle(Geom_TrimmedCurve) aTrimmed = Handle(Geom_TrimmedCurve)::DownCast(theCurve);
  while(!aTrimmed.IsNull()) {
    aResult = Handle(Geom_Circle)::DownCast(aTrimmed->BasisCurve());
    if (!aResult.IsNull())
      return aResult;
    aTrimmed = Handle(Geom_TrimmedCurve)::DownCast(aTrimmed->BasisCurve());
  }
  return aResult; // null, not circle
}

bool GeomAPI_Edge::isCircle() const
{
  const TopoDS_Shape& aShape = const_cast<GeomAPI_Edge*>(this)->impl<TopoDS_Shape>();
  double aFirst, aLast;
  Handle(Geom_Curve) aCurve = BRep_Tool::Curve((const TopoDS_Edge&)aShape, aFirst, aLast);
  if (!circ(aCurve).IsNull()) {
    // Check the difference of first and last parameters to be equal to the curve period
    if (Abs(aLast - aFirst - aCurve->Period()) < Precision::PConfusion())
      return true;
  }
  return false;
}

bool GeomAPI_Edge::isArc() const
{
  const TopoDS_Shape& aShape = const_cast<GeomAPI_Edge*>(this)->impl<TopoDS_Shape>();
  double aFirst, aLast;
  Handle(Geom_Curve) aCurve = BRep_Tool::Curve((const TopoDS_Edge&)aShape, aFirst, aLast);
  if (!circ(aCurve).IsNull()) {
    // Check the difference of first and last parameters is not equal the curve period
    if (Abs(aLast - aFirst - aCurve->Period()) >= Precision::PConfusion())
      return true;
  }
  return false;
}

bool GeomAPI_Edge::isEllipse() const
{
  const TopoDS_Shape& aShape = const_cast<GeomAPI_Edge*>(this)->impl<TopoDS_Shape>();
  double aFirst, aLast;
  Handle(Geom_Curve) aCurve = BRep_Tool::Curve((const TopoDS_Edge&)aShape, aFirst, aLast);
  if (aCurve.IsNull()) // degenerative edge
    return false;
  while (aCurve->IsKind(STANDARD_TYPE(Geom_TrimmedCurve)))
    aCurve = Handle(Geom_TrimmedCurve)::DownCast(aCurve)->BasisCurve();
  return aCurve->IsKind(STANDARD_TYPE(Geom_Ellipse));
}

bool GeomAPI_Edge::isBSpline() const
{
  const TopoDS_Shape& aShape = const_cast<GeomAPI_Edge*>(this)->impl<TopoDS_Shape>();
  double aFirst, aLast;
  Handle(Geom_Curve) aCurve = BRep_Tool::Curve((const TopoDS_Edge&)aShape, aFirst, aLast);
  if (aCurve.IsNull()) // degenerative edge
    return false;
  while (aCurve->IsKind(STANDARD_TYPE(Geom_TrimmedCurve)))
    aCurve = Handle(Geom_TrimmedCurve)::DownCast(aCurve)->BasisCurve();
  return aCurve->IsKind(STANDARD_TYPE(Geom_BSplineCurve));
}

std::shared_ptr<GeomAPI_Pnt> GeomAPI_Edge::firstPoint()
{
  const TopoDS_Shape& aShape = const_cast<GeomAPI_Edge*>(this)->impl<TopoDS_Shape>();
  double aFirst, aLast;
  Handle(Geom_Curve) aCurve = BRep_Tool::Curve((const TopoDS_Edge&)aShape, aFirst, aLast);
  gp_Pnt aPoint;
  aCurve->D0(aFirst, aPoint);
  return std::shared_ptr<GeomAPI_Pnt>(new GeomAPI_Pnt(aPoint.X(), aPoint.Y(), aPoint.Z()));
}

std::shared_ptr<GeomAPI_Pnt> GeomAPI_Edge::lastPoint()
{
  const TopoDS_Shape& aShape = const_cast<GeomAPI_Edge*>(this)->impl<TopoDS_Shape>();
  double aFirst, aLast;
  Handle(Geom_Curve) aCurve = BRep_Tool::Curve((const TopoDS_Edge&)aShape, aFirst, aLast);
  gp_Pnt aPoint;
  aCurve->D0(aLast, aPoint);
  return std::shared_ptr<GeomAPI_Pnt>(new GeomAPI_Pnt(aPoint.X(), aPoint.Y(), aPoint.Z()));
}

std::shared_ptr<GeomAPI_Circ> GeomAPI_Edge::circle() const
{
  const TopoDS_Shape& aShape = const_cast<GeomAPI_Edge*>(this)->impl<TopoDS_Shape>();
  double aFirst, aLast;
  Handle(Geom_Curve) aCurve = BRep_Tool::Curve((const TopoDS_Edge&)aShape, aFirst, aLast);
  Handle(Geom_Circle) aCirc = circ(aCurve);
  if (!aCirc.IsNull()) {
    gp_Pnt aLoc = aCirc->Location();
    std::shared_ptr<GeomAPI_Pnt> aCenter(new GeomAPI_Pnt(aLoc.X(), aLoc.Y(), aLoc.Z()));
    gp_Dir anAxis = aCirc->Axis().Direction();
    std::shared_ptr<GeomAPI_Dir> aDir(new GeomAPI_Dir(anAxis.X(), anAxis.Y(), anAxis.Z()));
    return std::shared_ptr<GeomAPI_Circ>(new GeomAPI_Circ(aCenter, aDir, aCirc->Radius()));
  }
  return std::shared_ptr<GeomAPI_Circ>(); // not circle
}

std::shared_ptr<GeomAPI_Ellipse> GeomAPI_Edge::ellipse() const
{
  const TopoDS_Shape& aShape = const_cast<GeomAPI_Edge*>(this)->impl<TopoDS_Shape>();
  double aFirst, aLast;
  Handle(Geom_Curve) aCurve = BRep_Tool::Curve((const TopoDS_Edge&)aShape, aFirst, aLast);
  if (!aCurve.IsNull()) {
    while (aCurve->IsKind(STANDARD_TYPE(Geom_TrimmedCurve)))
      aCurve = Handle(Geom_TrimmedCurve)::DownCast(aCurve)->BasisCurve();
    Handle(Geom_Ellipse) aElips = Handle(Geom_Ellipse)::DownCast(aCurve);
    if (!aElips.IsNull()) {
      gp_Elips aGpElips = aElips->Elips();
      std::shared_ptr<GeomAPI_Ellipse> aEllipse(new GeomAPI_Ellipse());
      aEllipse->setImpl(new gp_Elips(aGpElips));
      return aEllipse;
    }
  }
  return std::shared_ptr<GeomAPI_Ellipse>(); // not ellipse
}

std::shared_ptr<GeomAPI_Lin> GeomAPI_Edge::line() const
{
  const TopoDS_Shape& aShape = const_cast<GeomAPI_Edge*>(this)->impl<TopoDS_Shape>();
  double aFirst, aLast;
  Handle(Geom_Curve) aCurve = BRep_Tool::Curve((const TopoDS_Edge&)aShape, aFirst, aLast);
  if (aCurve) {
    Handle(Geom_Line) aLine = Handle(Geom_Line)::DownCast(aCurve);
    if (aLine) {
      gp_Pnt aStartPnt = aLine->Value(aFirst);
      std::shared_ptr<GeomAPI_Pnt> aStart(
          new GeomAPI_Pnt(aStartPnt.X(), aStartPnt.Y(), aStartPnt.Z()));
      gp_Pnt aEndPnt = aLine->Value(aLast);
      std::shared_ptr<GeomAPI_Pnt> aEnd(
          new GeomAPI_Pnt(aEndPnt.X(), aEndPnt.Y(), aEndPnt.Z()));
      return std::shared_ptr<GeomAPI_Lin>(new GeomAPI_Lin(aStart, aEnd));
    }
  }
  return std::shared_ptr<GeomAPI_Lin>(); // not circle
}


bool GeomAPI_Edge::isEqual(const std::shared_ptr<GeomAPI_Shape> theEdge) const
{
  if (!theEdge.get() || ! theEdge->isEdge())
    return false;
  const TopoDS_Shape& aMyShape = const_cast<GeomAPI_Edge*>(this)->impl<TopoDS_Shape>();
  const TopoDS_Shape& aInShape = theEdge->impl<TopoDS_Shape>();

  if (aMyShape.IsNull() || aInShape.IsNull())
    return false;

  if (aMyShape.ShapeType() != aInShape.ShapeType())
    return false;

  double aMyStart, aMyEnd;
  Handle(Geom_Curve) aMyCurve = BRep_Tool::Curve(TopoDS::Edge(aMyShape), aMyStart, aMyEnd);
  double aInStart, aInEnd;
  Handle(Geom_Curve) aInCurve = BRep_Tool::Curve(TopoDS::Edge(aInShape), aInStart, aInEnd);

  // Check that end point parameters are the same
  if ((aMyStart != aInStart) || (aMyEnd != aInEnd))
    return false;

  // Check that curves a the same type
  GeomAdaptor_Curve aMyAdaptor(aMyCurve);
  GeomAdaptor_Curve aInAdaptor(aInCurve);
  if (aMyAdaptor.GetType() != aInAdaptor.GetType())
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

void GeomAPI_Edge::setRange(const double& theFirst, const double& theLast)
{
  TopoDS_Edge anEdge = impl<TopoDS_Edge>();
  double aFirst, aLast;
  Handle(Geom_Curve) aCurve = BRep_Tool::Curve(anEdge, aFirst, aLast);
  double aTolerance = BRep_Tool::Tolerance(anEdge);
  if (aCurve->DynamicType() == STANDARD_TYPE(Geom_TrimmedCurve)) {
    aCurve = Handle(Geom_TrimmedCurve)::DownCast(aCurve)->BasisCurve();
    BRep_Builder().UpdateEdge(anEdge, aCurve, aTolerance);
  }
  BRep_Builder().Range(anEdge, theFirst, theLast);
}

void GeomAPI_Edge::getRange(double& theFirst, double& theLast) const
{
  const TopoDS_Shape& aShape = const_cast<GeomAPI_Edge*>(this)->impl<TopoDS_Shape>();
  Handle(Geom_Curve) aCurve = BRep_Tool::Curve((const TopoDS_Edge&)aShape, theFirst, theLast);
}

// LCOV_EXCL_START
bool GeomAPI_Edge::isInPlane(std::shared_ptr<GeomAPI_Pln> thePlane) const
{
  double aFirst, aLast;
  const TopoDS_Shape& aShape = const_cast<GeomAPI_Edge*>(this)->impl<TopoDS_Shape>();
  Handle(Geom_Curve) aCurve = BRep_Tool::Curve((const TopoDS_Edge&)aShape, aFirst, aLast);
  if (aCurve.IsNull())
    return false;

  double A, B, C, D;
  thePlane->coefficients(A, B, C, D);
  gp_Pln aPlane(A, B, C, D);

  bool inPlane = false;
  if (aCurve->IsKind(STANDARD_TYPE(Geom_Line))) {
    // check start and end points on the plane
    gp_Pnt aFirstPnt = aCurve->Value(aFirst);
    gp_Pnt aLastPnt = aCurve->Value(aLast);
    inPlane = aPlane.SquareDistance(aFirstPnt) < Precision::SquareConfusion() &&
              aPlane.SquareDistance(aLastPnt) < Precision::SquareConfusion();
  } else {
    Handle(Geom_Circle) aCirc = circ(aCurve);
    if (!aCirc.IsNull()) {
      gp_Pnt aCenter = aCirc->Location();
      Standard_Real aDot = aPlane.Axis().Direction().Dot(aCirc->Axis().Direction());
      inPlane = aPlane.SquareDistance(aCenter) < Precision::SquareConfusion() &&
                Abs(Abs(aDot) - 1.0) < Precision::Confusion();
    } else {
      // three points checking
      gp_Pnt aFirstPnt = aCurve->Value(aFirst);
      gp_Pnt aMidPnt = aCurve->Value((aFirst + aLast) / 2.);
      gp_Pnt aLastPnt = aCurve->Value(aLast);
      inPlane = aPlane.SquareDistance(aFirstPnt) < Precision::SquareConfusion() &&
                aPlane.SquareDistance(aMidPnt) < Precision::SquareConfusion() &&
                aPlane.SquareDistance(aLastPnt) < Precision::SquareConfusion();
    }
  }
  return inPlane;
}
// LCOV_EXCL_STOP

void GeomAPI_Edge::intersectWithPlane(const std::shared_ptr<GeomAPI_Pln> thePlane,
                                      std::list<std::shared_ptr<GeomAPI_Pnt>>& theResult) const
{
  double aFirst, aLast;
  const TopoDS_Shape& aShape = const_cast<GeomAPI_Edge*>(this)->impl<TopoDS_Shape>();
  Handle(Geom_Curve) aCurve = BRep_Tool::Curve((const TopoDS_Edge&)aShape, aFirst, aLast);
  if (!aCurve.IsNull()) {
    double A, B, C, D;
    thePlane->coefficients(A, B, C, D);
    gp_Pln aPln(A, B, C, D);
    Handle(Geom_Plane) aPlane = new Geom_Plane(aPln);

    // intersect the plane with the curve
    GeomAPI_IntCS aIntersect;
    aIntersect.Perform(aCurve, aPlane);
    if (aIntersect.IsDone() && (aIntersect.NbPoints() > 0)) {
      gp_Pnt aPnt;
      for (int i = 1; i <= aIntersect.NbPoints(); i++) {
        // check the parameter of intersection in the edge range
        aIntersect.Parameters(i, A, B, C);
        if (aCurve->IsPeriodic())
          C = ElCLib::InPeriod(C, aFirst, aFirst + aCurve->Period());
        if (C < aFirst - Precision::PConfusion() || C > aLast + Precision::PConfusion())
          continue;

        // obtain intersection point
        aPnt = aIntersect.Point(i);
        std::shared_ptr<GeomAPI_Pnt> aPntPtr(new GeomAPI_Pnt(aPnt.X(), aPnt.Y(), aPnt.Z()));
        theResult.push_back(aPntPtr);
      }
    }
    else {
      // find minimal distance between the plane and the curve
      GeomAPI_ExtremaCurveSurface anExtrema(aCurve, aPlane);
      double aTolerance = BRep_Tool::Tolerance(TopoDS::Edge(aShape));
      if (anExtrema.NbExtrema() > 0 &&
          !anExtrema.Extrema().IsParallel() &&
          anExtrema.LowerDistance() < aTolerance) {
        // distance is lower than tolerance => tangent case
        gp_Pnt aPntC, aPntS;
        anExtrema.NearestPoints(aPntC, aPntS);
        std::shared_ptr<GeomAPI_Pnt> aPntPtr(new GeomAPI_Pnt(aPntS.X(), aPntS.Y(), aPntS.Z()));
        theResult.push_back(aPntPtr);
      }
    }
  }
}

double GeomAPI_Edge::length() const
{
  const TopoDS_Edge& anEdge = TopoDS::Edge(impl<TopoDS_Shape>());
  BRepAdaptor_Curve aBRepAdaptor = BRepAdaptor_Curve(anEdge);
  Adaptor3d_Curve* anAdaptor3d = &aBRepAdaptor;
  return GCPnts_AbscissaPoint::Length(*anAdaptor3d);
}

bool GeomAPI_Edge::isClosed() const
{
  const TopoDS_Shape& aShape = const_cast<GeomAPI_Edge*>(this)->impl<TopoDS_Shape>();
  if (aShape.IsNull())
    return false;
  double aFirst, aLast;
  Handle(Geom_Curve) aCurve = BRep_Tool::Curve((const TopoDS_Edge&)aShape, aFirst, aLast);
  if (aCurve.IsNull() || !aCurve->IsPeriodic())
    return false;
  aLast += aLast > aFirst ? -aCurve->Period() : aCurve->Period();;

  return fabs(aFirst - aLast) < 1.e-9;
}

bool GeomAPI_Edge::isDegenerated() const
{
  const TopoDS_Shape& aShape = const_cast<GeomAPI_Edge*>(this)->impl<TopoDS_Shape>();
  if (aShape.IsNull() || aShape.ShapeType() != TopAbs_EDGE)
    return false;
  return BRep_Tool::Degenerated(TopoDS::Edge(aShape));
}

void GeomAPI_Edge::setFirstPointTolerance(const double theTolerance)
{
  TopoDS_Edge anEdge = impl<TopoDS_Edge>();
  TopoDS_Vertex aVFirst, aVLast;
  TopExp::Vertices(anEdge, aVFirst, aVLast);
  BRep_Builder().UpdateVertex(aVFirst, theTolerance);
}

void GeomAPI_Edge::setLastPointTolerance(const double theTolerance)
{
  TopoDS_Edge anEdge = impl<TopoDS_Edge>();
  TopoDS_Vertex aVFirst, aVLast;
  TopExp::Vertices(anEdge, aVFirst, aVLast);
  BRep_Builder().UpdateVertex(aVLast, theTolerance);
}

double GeomAPI_Edge::firstPointTolerance() const
{
  TopoDS_Edge anEdge = impl<TopoDS_Edge>();
  TopoDS_Vertex aVFirst, aVLast;
  TopExp::Vertices(anEdge, aVFirst, aVLast);
  return BRep_Tool::Tolerance(aVFirst);
}

double GeomAPI_Edge::lastPointTolerance() const
{
  TopoDS_Edge anEdge = impl<TopoDS_Edge>();
  TopoDS_Vertex aVFirst, aVLast;
  TopExp::Vertices(anEdge, aVFirst, aVLast);
  return BRep_Tool::Tolerance(aVLast);
}

GeomPointPtr GeomAPI_Edge::middlePoint() const
{
  GeomPointPtr aMiddlePoint;

  const TopoDS_Edge& anEdge = impl<TopoDS_Edge>();
  if (anEdge.IsNull())
    return aMiddlePoint;
  double aFirst, aLast;
  Handle(Geom_Curve) aCurve = BRep_Tool::Curve(anEdge, aFirst, aLast);
  if (aCurve.IsNull())
    return aMiddlePoint;

  static const int NB_POINTS = 3;
  GeomAdaptor_Curve aCurveAdaptor(aCurve, aFirst, aLast);
  GCPnts_UniformAbscissa anAlgo(aCurveAdaptor, NB_POINTS);
  if (anAlgo.IsDone()) {
    gp_Pnt aPnt = aCurveAdaptor.Value(anAlgo.Parameter(2));
    aMiddlePoint = GeomPointPtr(new GeomAPI_Pnt(aPnt.X(), aPnt.Y(), aPnt.Z()));
  }
  return aMiddlePoint;
}
