// Copyright (C) 2014-2023  CEA, EDF
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

#include <GeomAlgoAPI_EdgeBuilder.h>

#include <GeomAPI_Ax2.h>
#include <GeomAPI_Ax3.h>
#include <GeomAPI_BSpline2d.h>
#include <GeomAPI_Ellipse.h>
#include <GeomAPI_Pnt2d.h>

#include <gp_Pln.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS.hxx>
#include <BRep_Tool.hxx>
#include <Geom2d_BSplineCurve.hxx>
#include <Geom_Plane.hxx>
#include <Geom_CylindricalSurface.hxx>
#include <Geom_RectangularTrimmedSurface.hxx>
#include <GeomLib.hxx>

#include <gp_Ax2.hxx>
#include <gp_Circ.hxx>
#include <gp_Elips.hxx>
#include <Bnd_Box.hxx>
#include <BRepBndLib.hxx>

static GeomEdgePtr createLine(const gp_Pnt& theStart, const gp_Pnt& theEnd)
{
  GeomEdgePtr aRes;
  if (!theStart.IsEqual(theEnd, Precision::Confusion()) &&
    Abs(theStart.SquareDistance(theEnd)) < 1.e+100) {
    BRepBuilderAPI_MakeEdge anEdgeBuilder(theStart, theEnd);
    TopoDS_Edge anEdge = anEdgeBuilder.Edge();
    aRes = GeomEdgePtr(new GeomAPI_Edge);
    aRes->setImpl(new TopoDS_Shape(anEdge));
  }
  return aRes;
}

std::shared_ptr<GeomAPI_Edge> GeomAlgoAPI_EdgeBuilder::line(
    std::shared_ptr<GeomAPI_Pnt> theStart, std::shared_ptr<GeomAPI_Pnt> theEnd)
{
  const gp_Pnt& aStart = theStart->impl<gp_Pnt>();
  const gp_Pnt& anEnd = theEnd->impl<gp_Pnt>();
  return createLine(aStart, anEnd);
}
std::shared_ptr<GeomAPI_Edge> GeomAlgoAPI_EdgeBuilder::line(
    double theDX, double theDY, double theDZ)
{
  const gp_Pnt& aStart = gp_Pnt(0, 0, 0);
  const gp_Pnt& anEnd = gp_Pnt(theDX, theDY, theDZ);
  return createLine(aStart, anEnd);
}

std::shared_ptr<GeomAPI_Edge> GeomAlgoAPI_EdgeBuilder::line(
    const std::shared_ptr<GeomAPI_Lin> theLin)
{
  GeomEdgePtr aRes;
  if (theLin.get()) {
    const gp_Lin& aLin = theLin->impl<gp_Lin>();
    BRepBuilderAPI_MakeEdge anEdgeBuilder(aLin);
    TopoDS_Edge anEdge = anEdgeBuilder.Edge();
    aRes = GeomEdgePtr(new GeomAPI_Edge);
    aRes->setImpl(new TopoDS_Shape(anEdge));
  }
  return aRes;
}

std::shared_ptr<GeomAPI_Edge> GeomAlgoAPI_EdgeBuilder::cylinderAxis(
    std::shared_ptr<GeomAPI_Shape> theCylindricalFace)
{
  std::shared_ptr<GeomAPI_Edge> aResult;
  const TopoDS_Shape& aShape = theCylindricalFace->impl<TopoDS_Shape>();
  if (aShape.IsNull())
    return aResult;
  TopoDS_Face aFace = TopoDS::Face(aShape);
  if (aFace.IsNull())
    return aResult;
  TopLoc_Location aLoc;
  Handle(Geom_Surface) aSurf = BRep_Tool::Surface(aFace, aLoc);
  if (aSurf.IsNull())
    return aResult;
  Handle(Geom_RectangularTrimmedSurface) aTrimmed =
    Handle(Geom_RectangularTrimmedSurface)::DownCast(aSurf);
  if (!aTrimmed.IsNull())
    aSurf = aTrimmed->BasisSurface();
  Handle(Geom_CylindricalSurface) aCyl = Handle(Geom_CylindricalSurface)::DownCast(aSurf);
  if (aCyl.IsNull())
    return aResult;
  gp_Ax1 anAxis = aCyl->Axis();
  // compute the start and the end points of the resulting edge by the bounding box of the face
  // (vertices projected to the axis) plus 10%
  Bnd_Box aFaceBnd;
  BRepBndLib::Add(aFace, aFaceBnd);
  gp_Pnt aBoxMin(aFaceBnd.CornerMin()), aBoxMax(aFaceBnd.CornerMax());
  bool isFirst = true;
  double aParamMin = 0, aParamMax = 0;
  for(int aX = 0; aX < 2; aX++) {
    for(int aY = 0; aY < 2; aY++) {
      for(int aZ = 0; aZ < 2; aZ++) {
        gp_XYZ aBoxVertex(aX == 0 ? aBoxMin.X() : aBoxMax.X(),
          aY == 0 ? aBoxMin.Y() : aBoxMax.Y(), aZ == 0 ? aBoxMin.Z() : aBoxMax.Z());
        gp_XYZ aVec(aBoxVertex - anAxis.Location().XYZ());
        double aProjParam = aVec.Dot(anAxis.Direction().XYZ());
        if (isFirst) {
          isFirst = false;
          aParamMin = aProjParam;
          aParamMax = aProjParam;
        } else {
          if (aParamMin > aProjParam)
            aParamMin = aProjParam;
          else if (aParamMax < aProjParam)
            aParamMax = aProjParam;
        }
      }
    }
  }
  // add 10%
  double aDelta = aParamMax - aParamMin;
  if (aDelta < 1.e-4) aDelta = 1.e-4;
  aParamMin -= aDelta * 0.1;
  aParamMax += aDelta * 0.1;

  gp_Pnt aStart(aParamMin * anAxis.Direction().XYZ() + anAxis.Location().XYZ());
  aStart.Transform(aLoc.Transformation());
  gp_Pnt anEnd(aParamMax * anAxis.Direction().XYZ() + anAxis.Location().XYZ());
  anEnd.Transform(aLoc.Transformation());
  /*
  gp_Pnt aStart(anAxis.Location().Transformed(aLoc.Transformation()));
  // edge length is 100, "-" because cylinder of extrusion has negative direction with the cylinder
  gp_Pnt anEnd(anAxis.Location().XYZ() - anAxis.Direction().XYZ() * 100.);
  anEnd.Transform(aLoc.Transformation());
  */

  BRepBuilderAPI_MakeEdge anEdgeBuilder(aStart, anEnd);
  std::shared_ptr<GeomAPI_Edge> aRes(new GeomAPI_Edge);
  TopoDS_Edge anEdge = anEdgeBuilder.Edge();
  // an axis is an infinite object
  anEdge.Infinite(Standard_True);
  aRes->setImpl(new TopoDS_Shape(anEdge));
  return aRes;
}

std::shared_ptr<GeomAPI_Edge> GeomAlgoAPI_EdgeBuilder::lineCircle(
    std::shared_ptr<GeomAPI_Pnt> theCenter, std::shared_ptr<GeomAPI_Dir> theNormal,
    double theRadius, double theRotationAngle)
{
  const gp_Pnt& aCenter = theCenter->impl<gp_Pnt>();
  const gp_Dir& aDir = theNormal->impl<gp_Dir>();

  gp_Ax1 anAx(aCenter, aDir);

  gp_Circ aCircle(gp_Ax2(aCenter, aDir), theRadius);
  if (Abs(theRotationAngle) > 1.e-7) // Tolerance
    aCircle.Rotate(anAx, theRotationAngle);

  BRepBuilderAPI_MakeEdge anEdgeBuilder(aCircle);
  std::shared_ptr<GeomAPI_Edge> aRes(new GeomAPI_Edge);
  TopoDS_Edge anEdge = anEdgeBuilder.Edge();
  aRes->setImpl(new TopoDS_Shape(anEdge));
  return aRes;
}

std::shared_ptr<GeomAPI_Edge> GeomAlgoAPI_EdgeBuilder::lineCircle(
    std::shared_ptr<GeomAPI_Circ> theCircle)
{
  GeomEdgePtr aRes;
  if (theCircle.get()) {
    const gp_Circ& aCirc = theCircle->impl<gp_Circ>();
    BRepBuilderAPI_MakeEdge anEdgeBuilder(aCirc);
    TopoDS_Edge anEdge = anEdgeBuilder.Edge();
    aRes = GeomEdgePtr(new GeomAPI_Edge);
    aRes->setImpl(new TopoDS_Shape(anEdge));
  }
  return aRes;
}

std::shared_ptr<GeomAPI_Edge> GeomAlgoAPI_EdgeBuilder::lineCircleArc(
    std::shared_ptr<GeomAPI_Pnt> theCenter, std::shared_ptr<GeomAPI_Pnt> theStartPoint,
    std::shared_ptr<GeomAPI_Pnt> theEndPoint, std::shared_ptr<GeomAPI_Dir> theNormal)
{
  std::shared_ptr<GeomAPI_Edge> aRes;

  const gp_Pnt& aCenter = theCenter->impl<gp_Pnt>();
  const gp_Dir& aDir = theNormal->impl<gp_Dir>();

  /// OCCT creates an edge on a circle with empty radius, but visualization
  /// is not able to process it
  if (theCenter->isEqual(theStartPoint) || theCenter->isEqual(theEndPoint))
    return aRes;

  double aRadius = theCenter->distance(theStartPoint);
  gp_Circ aCircle(gp_Ax2(aCenter, aDir), aRadius);

  const gp_Pnt& aStart = theStartPoint->impl<gp_Pnt>();
  const gp_Pnt& anEndInter = theEndPoint->impl<gp_Pnt>();

  // project end point to a circle
  gp_XYZ aEndDir = anEndInter.XYZ() - aCenter.XYZ();
  gp_Pnt anEnd(aCenter.XYZ() + aEndDir.Normalized() * aRadius);

  BRepBuilderAPI_MakeEdge anEdgeBuilder;
  anEdgeBuilder = BRepBuilderAPI_MakeEdge(aCircle, aStart, anEnd);

  anEdgeBuilder.Build();

  if (anEdgeBuilder.IsDone()) {
    aRes = std::shared_ptr<GeomAPI_Edge>(new GeomAPI_Edge);
    aRes->setImpl(new TopoDS_Shape(anEdgeBuilder.Edge()));
  }
  return aRes;
}

std::shared_ptr<GeomAPI_Edge> GeomAlgoAPI_EdgeBuilder::ellipse(
    const std::shared_ptr<GeomAPI_Pnt>& theCenter,
    const std::shared_ptr<GeomAPI_Dir>& theNormal,
    const std::shared_ptr<GeomAPI_Dir>& theMajorAxis,
    const double                        theMajorRadius,
    const double                        theMinorRadius)
{
  const gp_Pnt& aCenter = theCenter->impl<gp_Pnt>();
  const gp_Dir& aNormal = theNormal->impl<gp_Dir>();
  const gp_Dir& aMajorAxis = theMajorAxis->impl<gp_Dir>();

  gp_Elips anEllipse(gp_Ax2(aCenter, aNormal, aMajorAxis), theMajorRadius, theMinorRadius);

  BRepBuilderAPI_MakeEdge anEdgeBuilder(anEllipse);
  std::shared_ptr<GeomAPI_Edge> aRes(new GeomAPI_Edge);
  TopoDS_Edge anEdge = anEdgeBuilder.Edge();
  aRes->setImpl(new TopoDS_Shape(anEdge));
  return aRes;
}

std::shared_ptr<GeomAPI_Edge> GeomAlgoAPI_EdgeBuilder::ellipticArc(
    const std::shared_ptr<GeomAPI_Pnt>& theCenter,
    const std::shared_ptr<GeomAPI_Dir>& theNormal,
    const std::shared_ptr<GeomAPI_Dir>& theMajorAxis,
    const double                        theMajorRadius,
    const double                        theMinorRadius,
    const std::shared_ptr<GeomAPI_Pnt>& theStart,
    const std::shared_ptr<GeomAPI_Pnt>& theEnd)
{
  std::shared_ptr<GeomAPI_Ax2> anAx2(new GeomAPI_Ax2(theCenter, theNormal, theMajorAxis));
  GeomAPI_Ellipse anEllipse(anAx2, theMajorRadius, theMinorRadius);

  GeomPointPtr aStartPnt = anEllipse.project(theStart);
  GeomPointPtr aEndPnt   = anEllipse.project(theEnd);

  double aStartParam, aEndParam;
  anEllipse.parameter(aStartPnt, Precision::Confusion(), aStartParam);
  anEllipse.parameter(aEndPnt, Precision::Confusion(), aEndParam);

  BRepBuilderAPI_MakeEdge anEdgeBuilder(anEllipse.impl<gp_Elips>(), aStartParam, aEndParam);
  GeomEdgePtr aRes(new GeomAPI_Edge);
  TopoDS_Edge anEdge = anEdgeBuilder.Edge();
  aRes->setImpl(new TopoDS_Shape(anEdge));
  return aRes;
}

GeomEdgePtr GeomAlgoAPI_EdgeBuilder::bsplineOnPlane(
    const std::shared_ptr<GeomAPI_Ax3>& thePlane,
    const std::list<GeomPnt2dPtr>& thePoles,
    const std::list<double>& theWeights,
    const std::list<double>& theKnots,
    const std::list<int>& theMults,
    const int theDegree,
    const bool thePeriodic)
{
  std::shared_ptr<GeomAPI_BSpline2d> aBSplineCurve(
      new GeomAPI_BSpline2d(theDegree, thePoles, theWeights, theKnots, theMults, thePeriodic));
  return bsplineOnPlane(thePlane, aBSplineCurve);
}

GeomEdgePtr GeomAlgoAPI_EdgeBuilder::bsplineOnPlane(
    const std::shared_ptr<GeomAPI_Ax3>& thePlane,
    const std::shared_ptr<GeomAPI_BSpline2d>& theCurve)
{
  Handle(Geom_Curve) aCurve3D = GeomLib::To3d(thePlane->impl<gp_Ax3>().Ax2(),
                                              theCurve->impl<Handle_Geom2d_BSplineCurve>());

  BRepBuilderAPI_MakeEdge anEdgeBuilder(aCurve3D);
  GeomEdgePtr aRes(new GeomAPI_Edge);
  TopoDS_Edge anEdge = anEdgeBuilder.Edge();
  aRes->setImpl(new TopoDS_Shape(anEdge));
  return aRes;
}
