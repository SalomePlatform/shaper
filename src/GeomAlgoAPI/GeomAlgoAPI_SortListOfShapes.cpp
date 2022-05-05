// Copyright (C) 2017-2022  CEA/DEN, EDF R&D
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

#include "GeomAlgoAPI_SortListOfShapes.h"

#include <Bnd_Box.hxx>
#include <Bnd_Box2d.hxx>
#include <BRep_Tool.hxx>
#include <BRepBndLib.hxx>
#include <BRepTools.hxx>
#include <gp_Pnt.hxx>
#include <Precision.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>

#include <algorithm>
#include <map>

class CompareShapes
{
  std::map<TopoDS_TShape*, Bnd_Box> myShapes;
  std::map<TopoDS_TShape*, Bnd_Box2d> myUVBounds;

  bool compareEdges(const GeomShapePtr& theLHS, const GeomShapePtr& theRHS)
  {
    const TopoDS_Edge& aLHSEdge = TopoDS::Edge(theLHS->impl<TopoDS_Shape>());
    const TopoDS_Edge& aRHSEdge = TopoDS::Edge(theRHS->impl<TopoDS_Shape>());

    double aLF, aLE, aRF, aRE;
    Handle(Geom_Curve) aLHSCurve = BRep_Tool::Curve(aLHSEdge, aLF, aLE);
    Handle(Geom_Curve) aRHSCurve = BRep_Tool::Curve(aRHSEdge, aRF, aRE);

    if (aLHSCurve == aRHSCurve) {
      // compare by first parameter
      if (isLessWithTol(aLF, aRF, Precision::PConfusion()))
        return true;
      else if (isLessWithTol(aRF, aLF, Precision::PConfusion()))
        return false;
      // compare by last parameter
      return isLessWithTol(aLE, aRE, Precision::PConfusion());
    }
    // different underlying curves => compare bounding boxe
    return compareByBoundingBox(theLHS, theRHS);
  }

  bool compareFaces(const GeomShapePtr& theLHS, const GeomShapePtr& theRHS)
  {
    const TopoDS_Face& aLHSFace = TopoDS::Face(theLHS->impl<TopoDS_Shape>());
    const TopoDS_Face& aRHSFace = TopoDS::Face(theRHS->impl<TopoDS_Shape>());

    Handle(Geom_Surface) aLHSSurf = BRep_Tool::Surface(aLHSFace);
    Handle(Geom_Surface) aRHSSurf = BRep_Tool::Surface(aRHSFace);

    if (aLHSSurf == aRHSSurf) {
      // compare parametric space for faces on the same surface
      Bnd_Box2d aLHSBox = boundingBoxUV(aLHSFace);
      Bnd_Box2d aRHSBox = boundingBoxUV(aRHSFace);

      double aLHSBB[4], aRHSBB[4];
      aLHSBox.Get(aLHSBB[0], aLHSBB[1], aLHSBB[2], aLHSBB[3]);
      aRHSBox.Get(aRHSBB[0], aRHSBB[1], aRHSBB[2], aRHSBB[3]);
      for (int anIndex = 0; anIndex < 4; ++anIndex) {
        if (isLessWithTol(aLHSBB[anIndex], aRHSBB[anIndex], Precision::PConfusion()))
          return true;
        else if (isLessWithTol(aRHSBB[anIndex], aLHSBB[anIndex], Precision::PConfusion()))
          return false;
      }
      // equal parametric boxes
      return false;
    }
    // different underlying surfaces => compare bounding boxes
    return compareByBoundingBox(theLHS, theRHS);
  }

  bool compareByBoundingBox(const GeomShapePtr& theLHS, const GeomShapePtr& theRHS)
  {
    Bnd_Box aLHSBox = boundingBox(theLHS);
    Bnd_Box aRHSBox = boundingBox(theRHS);

    gp_Pnt aLHSMin = aLHSBox.CornerMin();
    gp_Pnt aLHSMax = aLHSBox.CornerMax();

    gp_Pnt aRHSMin = aRHSBox.CornerMin();
    gp_Pnt aRHSMax = aRHSBox.CornerMax();

    if (isLess(aLHSMin, aRHSMin))
      return true;
    else if (isLess(aRHSMin, aLHSMin))
      return false;

    return isLess(aLHSMax, aRHSMax);
  }

  bool isLessWithTol(const double theLHS, const double theRHS, const double theTolerance)
  {
    return theLHS + theTolerance < theRHS;
  }

  bool isLess(const gp_Pnt& theLHS, const gp_Pnt& theRHS)
  {
    static const double aTol = 10. * Precision::Confusion();
    for (int anIndex = 1; anIndex <= 3; ++anIndex) {
      if (isLessWithTol(theLHS.Coord(anIndex), theRHS.Coord(anIndex), aTol))
        return true;
      else if (isLessWithTol(theRHS.Coord(anIndex), theLHS.Coord(anIndex), aTol))
        return false;
    }
    // equal points
    return false;
  }

  Bnd_Box boundingBox(const GeomShapePtr& theShape)
  {
    const TopoDS_Shape& aShape = theShape->impl<TopoDS_Shape>();
    TopoDS_TShape* aS = aShape.TShape().get();
    std::map<TopoDS_TShape*, Bnd_Box>::iterator aFound = myShapes.find(aS);
    if (aFound == myShapes.end()) {
      Bnd_Box aBB;
      BRepBndLib::AddOptimal(aShape, aBB, false);
      myShapes[aS] = aBB;
      aFound = myShapes.find(aS);
    }
    return aFound->second;
  }

  Bnd_Box2d boundingBoxUV(const TopoDS_Face& theFace)
  {
    TopoDS_TShape* aFacePtr = theFace.TShape().get();
    std::map<TopoDS_TShape*, Bnd_Box2d>::iterator aFound = myUVBounds.find(aFacePtr);
    if (aFound == myUVBounds.end()) {
      Bnd_Box2d aBB;
      BRepTools::AddUVBounds(theFace, aBB);
      myUVBounds[aFacePtr] = aBB;
      aFound = myUVBounds.find(aFacePtr);
    }
    return aFound->second;
  }

public:
  // Verify theLHS is less than theRHS
  bool operator() (const GeomShapePtr& theLHS, const GeomShapePtr& theRHS)
  {
    if (theLHS->shapeType() == theRHS->shapeType()) {
      // edges and faces are compared by geometric properties
      if (theLHS->shapeType() == GeomAPI_Shape::EDGE)
        return compareEdges(theLHS, theRHS);
      else if (theLHS->shapeType() == GeomAPI_Shape::FACE)
        return compareFaces(theLHS, theRHS);
      // all other comparisons are made by bounding boxes
      return compareByBoundingBox(theLHS, theRHS);
    }

    // shapes of different types are compared by the type
    return theLHS->shapeType() < theRHS->shapeType();
  }
};


void GeomAlgoAPI_SortListOfShapes::sort(ListOfShape& theShapes)
{
  CompareShapes aComparator;
  theShapes.sort(aComparator);
}
