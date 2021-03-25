// Copyright (C) 2017-2021  CEA/DEN, EDF R&D
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

#include "GeomAlgoAPI_Filling.h"

#include <BRep_Tool.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_BSplineSurface.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <GeomAPI_PointsToBSplineSurface.hxx>
#include <GeomFill_AppSurf.hxx>
#include <GeomFill_Line.hxx>
#include <GeomFill_SectionGenerator.hxx>
#include <Precision.hxx>
#include <ShapeFix_Face.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>

static void edgesToCurves(const std::list<GeomEdgePtr>& theEdges,
                          std::list<Handle(Geom_Curve)>& theCurves)
{
  for (std::list<GeomEdgePtr>::const_iterator anIt = theEdges.begin();
       anIt != theEdges.end(); ++anIt) {
    const TopoDS_Edge& anEdge = (*anIt)->impl<TopoDS_Edge>();
    if (BRep_Tool::Degenerated(anEdge))
      continue;

    double aFirst, aLast;
    Handle(Geom_Curve) aCurve = BRep_Tool::Curve(anEdge, aFirst, aLast);

    aCurve = new Geom_TrimmedCurve(aCurve, aFirst, aLast);
    if (anEdge.Orientation() == TopAbs_REVERSED)
      aCurve->Reverse();

    theCurves.push_back(aCurve);
  }
}


GeomAlgoAPI_Filling::GeomAlgoAPI_Filling(const int theMinDegree,
                                         const int theMaxDegree,
                                         const int theNbIter,
                                         const double theTol2D,
                                         const double theTol3D)
  : myMinDegree(theMinDegree),
    myMaxDegree(theMaxDegree),
    myNbIter(theNbIter),
    myTol2D(theTol2D),
    myTol3D(theTol3D)
{
}

void GeomAlgoAPI_Filling::add(const GeomEdgePtr theEdge)
{
  myConstraints.push_back(theEdge);
}

void GeomAlgoAPI_Filling::build(bool isApproximate)
{
  if (myConstraints.size() <= 1) // not enough edges
    return;

  if (isApproximate)
    buildByControlPoints();
  else
    buildByEdges();
}

void GeomAlgoAPI_Filling::buildByEdges()
{
  GeomFill_SectionGenerator aSection;

  // obtain section curves
  std::list<Handle(Geom_Curve)> aCurves;
  edgesToCurves(myConstraints, aCurves);
  for (std::list<Handle(Geom_Curve)>::iterator anIt = aCurves.begin();
       anIt != aCurves.end(); ++anIt)
    aSection.AddCurve(*anIt);

  // a 'tolerance' is used to compare 2 knots
  aSection.Perform(Precision::PConfusion());
  int aNbCurves = (int)aCurves.size();
  Handle(GeomFill_Line) aLine = new GeomFill_Line(aNbCurves);

  // check myMaxDegree >= aCurves.size() - 1 to be able to interpolate a surface
  if (myMaxDegree + 1 < aNbCurves) {
    myError = "Unable to interpolate surface,"
       " Max deg + 1 should be greater or equal than number of sections.";
    return;
  }

  // perform filling by sections
  GeomFill_AppSurf anAppSurf(myMinDegree, myMaxDegree, myTol3D, myTol2D, myNbIter);
  anAppSurf.Perform(aLine, aSection);
  if (!anAppSurf.IsDone()) {
    myError = "Approximation algorithm failed.";
    return;
  }

  // build calculated surface
  Standard_Integer UDegree, VDegree, NbUPoles, NbVPoles, NbUKnots, NbVKnots;
  anAppSurf.SurfShape(UDegree, VDegree, NbUPoles, NbVPoles, NbUKnots, NbVKnots);
  Handle(Geom_BSplineSurface) GBS = new Geom_BSplineSurface(
    anAppSurf.SurfPoles(), anAppSurf.SurfWeights(), anAppSurf.SurfUKnots(), anAppSurf.SurfVKnots(),
    anAppSurf.SurfUMults(), anAppSurf.SurfVMults(), anAppSurf.UDegree(), anAppSurf.VDegree());

  if (GBS.IsNull())
    return;

  // store result
  TopoDS_Face aFace = BRepBuilderAPI_MakeFace(GBS, Precision::Confusion());
  std::shared_ptr<GeomAPI_Shape> aShape(new GeomAPI_Shape());
  aShape->setImpl(new TopoDS_Shape(aFace));
  setShape(aShape);
  setDone(true);
}

static Handle(Geom_Curve) removeTrim(const Handle(Geom_Curve)& theCurve)
{
  Handle(Geom_Curve) aCurve = theCurve;
  Handle(Geom_TrimmedCurve) aTC = Handle(Geom_TrimmedCurve)::DownCast(aCurve);
  while (!aTC.IsNull()) {
    aCurve = aTC->BasisCurve();
    aTC = Handle(Geom_TrimmedCurve)::DownCast(aCurve);
  }
  return aCurve;
}

void GeomAlgoAPI_Filling::buildByControlPoints()
{
  // obtain section curves
  std::list<Handle(Geom_Curve)> aCurves;
  edgesToCurves(myConstraints, aCurves);

  // compute maximal number of poles in B-spline curves
  int aMaxPoles = 0;
  std::list<Handle(Geom_Curve)>::iterator anIt = aCurves.begin();
  for (; anIt != aCurves.end(); ++anIt) {
    Handle(Geom_BSplineCurve) aBC = Handle(Geom_BSplineCurve)::DownCast(removeTrim(*anIt));
    if (!aBC.IsNull())
      aMaxPoles = Max(aMaxPoles, aBC->NbPoles());
  }

  // prepare array of points for creation bspline surface
  // size of this array: by U parameter - number of curves,
  // by V parameter - determ using MaxNbPoles but it's
  // value must be between 21(min) and 101(max)
  int aNbSections = (int) aCurves.size();
  int aNbPntInSection = Max(21, 2 * aMaxPoles - 1);
  TColgp_Array2OfPnt aPoints(1, aNbSections, 1, aNbPntInSection);
  anIt = aCurves.begin();
  for (int i = 1; anIt != aCurves.end(); ++i, ++anIt) {
    Handle(Geom_Curve) aC = *anIt;
    double fp = aC->FirstParameter();
    double lp = aC->LastParameter();
    double dp = (lp - fp) / (aNbPntInSection - 1);

    gp_Pnt aPnt;
    for (int j = 0; j < aNbPntInSection; j++) {
      aC->D0(fp + dp * j, aPnt);
      aPoints.SetValue(i, j+1, aPnt);
    }
  }

  // convert a grid of points to B-spline surface
  GeomAPI_PointsToBSplineSurface aPTB(aPoints, myMinDegree, myMaxDegree, GeomAbs_C2, myTol3D);
  Handle(Geom_BSplineSurface) aBS = aPTB.Surface();
  if (aBS.IsNull())
    return;

  // fix face orientation
  TopoDS_Face aFace = BRepBuilderAPI_MakeFace(aBS, Precision::Confusion());
  Handle(ShapeFix_Face) aFix = new ShapeFix_Face(aFace);
  aFix->Perform();
  aFix->FixOrientation();
  aFace = aFix->Face();

  // store result
  std::shared_ptr<GeomAPI_Shape> aShape(new GeomAPI_Shape());
  aShape->setImpl(new TopoDS_Shape(aFace));
  setShape(aShape);
  setDone(true);
}
