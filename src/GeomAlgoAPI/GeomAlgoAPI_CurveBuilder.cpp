// Copyright (C) 2014-2020  CEA/DEN, EDF R&D
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

#include "GeomAlgoAPI_CurveBuilder.h"

#include <GeomAPI_Edge.h>
#include <GeomAPI_Pnt.h>
#include <GeomAPI_Vertex.h>
#include <GeomAPI_ShapeExplorer.h>

#include <Approx_ParametrizationType.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BSplSLib.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_BSplineSurface.hxx>
#include <GeomAPI_Interpolate.hxx>
#include <GeomAPI_PointsToBSplineSurface.hxx>
#include <gp_Pnt.hxx>
#include <Precision.hxx>
#include <TColgp_Array2OfPnt.hxx>
#include <TColgp_HArray1OfPnt.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>


GeomEdgePtr GeomAlgoAPI_CurveBuilder::edge(const std::list<GeomPointPtr>& thePoints,
                                           const bool thePeriodic,
                                           const bool theIsToReorder,
                                           const GeomDirPtr& theStartTangent,
                                           const GeomDirPtr& theEndTangent)
{
  std::list<GeomPointPtr> aPointsCopy = thePoints;

  // If the curve to be closed - remove last point if it is too close to the first one
  bool isClose = aPointsCopy.front()->distance(aPointsCopy.back()) <= gp::Resolution();
  if (isClose && thePeriodic) {
    aPointsCopy.pop_back();
  }

  // Reorder points if required
  if (theIsToReorder) {
    reorderPoints(aPointsCopy);
  }

  // Prepare points array
  Handle(TColgp_HArray1OfPnt) aPoints = new TColgp_HArray1OfPnt(1, (int)aPointsCopy.size());
  std::list<GeomPointPtr>::const_iterator anIt = aPointsCopy.begin();
  for (int i = 1; anIt != aPointsCopy.end(); anIt++, i++) {
    GeomPointPtr aPoint = *anIt;
    aPoints->SetValue(i, aPoint->impl<gp_Pnt>());
  }

  // Initialize interpolator
  GeomAPI_Interpolate anInterp(aPoints, thePeriodic, gp::Resolution());

  // Assign tangents if defined
  if (theStartTangent && theEndTangent) {
    gp_Dir aDir = theStartTangent->impl<gp_Dir>();
    gp_Vec anInitialTangent(aDir.XYZ());
    aDir = theEndTangent->impl<gp_Dir>();
    gp_Vec aFinalTangent(aDir.XYZ());

    anInterp.Load(anInitialTangent, aFinalTangent);
  }

  // Compute
  if (aPoints->Length() > 1) {
    anInterp.Perform();
  }

  // Set result in form of edge
  TopoDS_Edge anEdge;
  if (anInterp.IsDone()) {
    anEdge = BRepBuilderAPI_MakeEdge(anInterp.Curve()).Edge();
  }

  GeomEdgePtr aResultShape(new GeomAPI_Edge);
  aResultShape->setImpl(new TopoDS_Shape(anEdge));

  return aResultShape;
}

GeomEdgePtr GeomAlgoAPI_CurveBuilder::approximate(const std::list<GeomPointPtr>& thePoints,
                                                  const bool thePeriodic,
                                                  const double thePrecision)
{
  // Prepare points array to be able to build a surface.
  // This surface is based on two sets of points: the first is an original and
  // the second is shifted along orthogonal direction.
  // This is a workaround, because GeomAPI_PointsToBSpline algorithm cannot produce
  // the periodic curve, but GeomAPI_PointsToBSplineSurface can.
  TColgp_Array2OfPnt aPoints(1, (int)thePoints.size(), 1, 2);
  gp_Pnt aPlaneBase[3]; // base points to calculate the normal direction
  int aNbPlanePoints = 0;
  gp_Dir aNormal;
  std::list<GeomPointPtr>::const_iterator anIt = thePoints.begin();
  for (int i = 1; anIt != thePoints.end(); anIt++, i++) {
    const gp_Pnt& aPoint = (*anIt)->impl<gp_Pnt>();
    aPoints.SetValue(i, 1, aPoint);
    aPoints.SetValue(i, 2, aPoint);
    if (aNbPlanePoints < 3) {
      if (aNbPlanePoints == 0 ||
          aPoint.SquareDistance(aPlaneBase[0]) > Precision::SquareConfusion())
        aPlaneBase[aNbPlanePoints++] = aPoint;
      if (aNbPlanePoints == 3) {
        gp_Vec aVec12(aPlaneBase[0], aPlaneBase[1]);
        gp_Vec aVec13(aPlaneBase[0], aPlaneBase[2]);
        if (aVec12.CrossSquareMagnitude(aVec13) > Precision::SquareConfusion())
          aNormal = gp_Dir(aVec12 ^ aVec13);
        else
          --aNbPlanePoints;
      }
    }
  }
  if (aNbPlanePoints < 3)
    aNormal = gp::DZ();
  // shifted points
  for (int i = aPoints.LowerRow(); i <= aPoints.UpperRow(); i++)
    aPoints.ChangeValue(i, 2).ChangeCoord() += aNormal.XYZ();

  // If the curve to be closed - remove last point if it is too close to the first one
  bool isClose = aPoints.Value(aPoints.LowerRow(), 1).Distance(
                 aPoints.Value(aPoints.UpperRow(), 1)) <= gp::Resolution();
  if (isClose && thePeriodic) {
    aPoints.Resize(aPoints.LowerRow(), aPoints.UpperRow() - 1,
                   aPoints.LowerCol(), aPoints.UpperCol(), Standard_True);
  }

  // Initialize and perform approximator
  static const Standard_Integer DEGREE_MIN = 3;
  static const Standard_Integer DEGREE_MAX = 8;
  GeomAPI_PointsToBSplineSurface anApprox;
  anApprox.Init(aPoints, Approx_ChordLength, DEGREE_MIN, DEGREE_MAX,
                GeomAbs_C2, thePrecision, thePeriodic);

  // Set result in form of edge
  TopoDS_Edge anEdge;
  if (anApprox.IsDone()) {
    // build a curve along U-direction of the surface
    Handle(Geom_BSplineSurface) aSurface = anApprox.Surface();
    Handle(Geom_Curve) aCurve = aSurface->VIso(aSurface->VKnots().First());

    anEdge = BRepBuilderAPI_MakeEdge(aCurve).Edge();
  }

  GeomEdgePtr aResultShape(new GeomAPI_Edge);
  aResultShape->setImpl(new TopoDS_Shape(anEdge));

  return aResultShape;
}

void GeomAlgoAPI_CurveBuilder::reorderPoints(std::list<GeomPointPtr>& thePoints)
{
  if (thePoints.size() < 3) {
    return;
  }

  int aNbDup = 0;
  std::list<GeomPointPtr>::iterator aPIt = thePoints.begin();
  GeomPointPtr aPrevPnt = *aPIt;
  for (; aPIt != thePoints.end(); ++aPIt) {
    GeomPointPtr aPnt = *aPIt;
    std::list<GeomPointPtr>::iterator aNextIt = aPIt;
    std::list<GeomPointPtr>::iterator aNearestIt = ++aNextIt;
    double aMinDist = RealLast();
    while (aNextIt != thePoints.end()) {
      double aDist = aPnt->distance(*aNextIt);
      if (aDist < Precision::Confusion()) {
        // remove duplicates
        std::list<GeomPointPtr>::iterator aRemoveIt = aNextIt++;
        thePoints.erase(aRemoveIt);
        continue;
      }
      if (aDist < aMinDist && (aMinDist - aDist) > Precision::Confusion()) {
        aNearestIt = aNextIt;
        aMinDist = aDist;
      }
      ++aNextIt;
    }
    aNextIt = aPIt; ++aNextIt;
    if (aNearestIt != aNextIt) {
      // Keep given order of points to use it in case of equidistant candidates
      //              .--<---<--.
      //             |           |
      // o  o  o  c  o->o->o->o->n  o  o
      //          |  |           |
      //          i i+1       nearest
      GeomPointPtr aPointToMove = *aNearestIt;
      thePoints.erase(aNearestIt);
      thePoints.insert(aNextIt, aPointToMove);
    }
  }
}