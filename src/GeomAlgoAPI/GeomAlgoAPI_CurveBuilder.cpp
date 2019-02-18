// Copyright (C) 2014-2019  CEA/DEN, EDF R&D
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

#include <BRepBuilderAPI_MakeEdge.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopExp_Explorer.hxx>
#include <TColgp_HArray1OfPnt.hxx>
#include <GeomAPI_Interpolate.hxx>
#include <gp_Pnt.hxx>
#include <Precision.hxx>


static void reorder(Handle(TColgp_HArray1OfPnt)& thePoints);

//=================================================================================================
GeomEdgePtr GeomAlgoAPI_CurveBuilder::edge(const std::list<GeomPointPtr>& thePoints,
  const bool theIsClosed,
  const bool theIsToReorder,
  const GeomDirPtr& theStartTangent,
  const GeomDirPtr& theEndTangent)
{
  // Prepare points array
  Handle(TColgp_HArray1OfPnt) aPoints = new TColgp_HArray1OfPnt(1, (int)thePoints.size());
  std::list<GeomPointPtr>::const_iterator anIt = thePoints.begin();
  for (int i = 1; anIt != thePoints.end(); anIt++, i++) {
    GeomPointPtr aPoint = *anIt;
    aPoints->SetValue(i, aPoint->impl<gp_Pnt>());
  }

  // If the curve to be closed - remove last point if it is too close to the first one
  bool isClose = aPoints->First().Distance(aPoints->Last()) <= gp::Resolution();
  if (isClose && theIsClosed) {
    aPoints->Resize(aPoints->Lower(), aPoints->Upper() - 1, Standard_True);
  }

  // Reorder points if required
  if (theIsToReorder) {
    reorder(aPoints);
  }

  // Initialize interpolator
  GeomAPI_Interpolate anInterp(aPoints, theIsClosed, gp::Resolution());

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

//================   Auxiliary functions   ========================================================
void reorder(Handle(TColgp_HArray1OfPnt)& thePoints)
{
  if (thePoints->Length() < 3) {
    return;
  }

  int aNbPoints = thePoints->Length();
  int aNbDup = 0;
  gp_Pnt aPrevPnt = thePoints->Value(1);
  for (int i = 1; i < aNbPoints; i++) {
    gp_Pnt aPnt = thePoints->Value(i);
    int aNearest = 0;
    double aMinDist = RealLast();
    for (int j = i + 1; j <= aNbPoints; j++) {
      double aDist = aPnt.SquareDistance(thePoints->Value(j));
      if (aDist < aMinDist && (aMinDist - aDist) > Precision::Confusion()) {
        aNearest = j;
        aMinDist = aDist;
      }
    }
    if (aNearest > 0 && aNearest != i + 1) {
      // Keep given order of points to use it in case of equidistant candidates
      //               .-<---<-.
      //              /         \
      // o  o  o  c  o->o->o->o->n  o  o
      //          |  |           |
      //          i i+1       nearest
      gp_Pnt aNearestPnt = thePoints->Value(aNearest);
      for (int j = aNearest; j > i + 1; j--) {
        thePoints->SetValue(j, thePoints->Value(j - 1));
      }
      thePoints->SetValue(i + 1, aNearestPnt);
    }
    if (aPrevPnt.Distance(thePoints->Value(i + 1)) <= Precision::Confusion())
      aNbDup++;
    else
      aPrevPnt = thePoints->Value(i + 1);
  }

  if (aNbDup > 0) {
    Handle(TColgp_HArray1OfPnt) aTmpPoints = new TColgp_HArray1OfPnt(1, aNbPoints - aNbDup);
    for (int j = 1, i = 1; i <= aNbPoints; i++) {
      if (i == 1 || aPrevPnt.Distance(thePoints->Value(i)) > Precision::Confusion()) {
        aTmpPoints->SetValue(j++, thePoints->Value(i));
        aPrevPnt = thePoints->Value(i);
      }
    }
    thePoints = aTmpPoints;
  }
}