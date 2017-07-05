// Copyright (C) 2014-2017  CEA/DEN, EDF R&D
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
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
//
// See http://www.salome-platform.org/ or
// email : webmaster.salome@opencascade.com<mailto:webmaster.salome@opencascade.com>
//

#include "SketcherPrs_PositionMgr.h"
#include "SketcherPrs_Tools.h"

#include <GeomAPI_Edge.h>
#include <GeomAPI_Curve.h>
#include <GeomAPI_Vertex.h>
#include <GeomAPI_Dir.h>

#include <BRepExtrema_ExtPC.hxx>
#include <TopoDS_Vertex.hxx>
#include <Geom_Curve.hxx>

static SketcherPrs_PositionMgr* MyPosMgr = NULL;

// The class is implemented as a singlton
SketcherPrs_PositionMgr* SketcherPrs_PositionMgr::get()
{
  if (MyPosMgr == NULL)
    MyPosMgr = new SketcherPrs_PositionMgr();
  return MyPosMgr;
}

SketcherPrs_PositionMgr::SketcherPrs_PositionMgr()
{
}


int SketcherPrs_PositionMgr::getPositionIndex(ObjectPtr theLine,
                                              const SketcherPrs_SymbolPrs* thePrs)
{
  if (myShapes.count(theLine) == 1) {
    // Find the map and add new [Presentation - Index] pair
    PositionsMap& aPosMap = myShapes[theLine];
    if (aPosMap.count(thePrs) == 1) {
      // return existing index
      return aPosMap[thePrs];
    } else {
      // Add a new [Presentation - Index] pair
      int aInd = int(aPosMap.size());
      aPosMap[thePrs] = aInd;
      return aInd;
    }
  } else {
    // Create a new map with initial index
    PositionsMap aPosMap;
    aPosMap[thePrs] = 0;
    myShapes[theLine] = aPosMap;
    return 0;
  }
}


gp_Vec getVector(ObjectPtr theShape, GeomDirPtr theDir, gp_Pnt theP)
{
  gp_Vec aVec;
  std::shared_ptr<GeomAPI_Shape> aShape = SketcherPrs_Tools::getShape(theShape);
  if (aShape->isEdge()) {
    std::shared_ptr<GeomAPI_Curve> aCurve =
      std::shared_ptr<GeomAPI_Curve>(new GeomAPI_Curve(aShape));

    if (aCurve->isCircle()) {
      GeomEdgePtr aEdgePtr(new GeomAPI_Edge(aShape));
      GeomVertexPtr aVertexPtr(new GeomAPI_Vertex(theP.X(), theP.Y(), theP.Z()));
      BRepExtrema_ExtPC aExtrema(aVertexPtr->impl<TopoDS_Vertex>(),
                                 aEdgePtr->impl<TopoDS_Edge>());
      int aNb = aExtrema.NbExt();
      if (aNb > 0) {
        for (int i = 1; i <= aNb; i++) {
          if (aExtrema.IsMin(i)) {
            double aParam = aExtrema.Parameter(i);
            Handle(Geom_Curve) aCurv = aCurve->impl<Handle_Geom_Curve>();
            gp_Pnt aP;
            aCurv->D1(aParam, aP, aVec);
            break;
          }
        }
      }
    } else {
      double aMidParam = (aCurve->startParam() + aCurve->endParam()) / 2.;
      GeomPointPtr aPnt1 = aCurve->getPoint((aMidParam + aCurve->endParam()) / 2.);
      GeomPointPtr aPnt2 = aCurve->getPoint((aMidParam + aCurve->startParam()) / 2.);

      aVec = gp_Vec(aPnt1->impl<gp_Pnt>(), aPnt2->impl<gp_Pnt>());
    }
  } else {
    aVec = gp_Vec(theDir->impl<gp_Dir>());
  }
  return aVec;
}

gp_Pnt SketcherPrs_PositionMgr::getPosition(ObjectPtr theShape,
                                            const SketcherPrs_SymbolPrs* thePrs,
                                            double theStep, GeomPointPtr thePnt)
{
  std::shared_ptr<GeomAPI_Shape> aShape = SketcherPrs_Tools::getShape(theShape);
  gp_Pnt aP; // Central point

  if (thePnt.get()) {
    aP = thePnt->impl<gp_Pnt>();
  } else {
    if (aShape->isEdge()) {
      std::shared_ptr<GeomAPI_Curve> aCurve =
        std::shared_ptr<GeomAPI_Curve>(new GeomAPI_Curve(aShape));
      // this is a circle or arc
      double aMidParam = (aCurve->startParam() + aCurve->endParam()) / 2.;
      std::shared_ptr<GeomAPI_Pnt> aPnt = aCurve->getPoint(aMidParam);
      aP = aPnt->impl<gp_Pnt>();
    } else {
      // This is a point
      std::shared_ptr<GeomAPI_Vertex> aVertex =
        std::shared_ptr<GeomAPI_Vertex>(new GeomAPI_Vertex(aShape));
      std::shared_ptr<GeomAPI_Pnt> aPnt = aVertex->point();
      aP = aPnt->impl<gp_Pnt>();
    }
  }
  // main vector
  gp_Vec aVec1 = getVector(theShape, thePrs->plane()->dirX(), aP);

  // Compute shifting vector for a one symbol
  gp_Vec aShift = aVec1.Crossed(thePrs->plane()->normal()->impl<gp_Dir>());
  aShift.Normalize();
  // For point based symbols step = 1.2, for line based = 0.8
  aShift.Multiply(theStep * (thePnt.get()? 1.2 : 0.8));

  // Shift the position coordinate according to position index
  int aPos = getPositionIndex(theShape, thePrs);
  int aM = 1;
  if ((aPos % 2) == 0) {
    // Even position
    aP.Translate(aShift);
    if (aPos > 0) {
      if (aPos % 4 == 0)
        aM = aPos / 4;
      else
        aM = -(aPos + 2) / 4;
    }
  } else {
    // Odd position
    aP.Translate(-aShift);
    if (aPos > 1) {
      if ((aPos - 1) % 4 == 0)
        aM = (aPos - 1) / 4;
      else
        aM = -(aPos + 1) / 4;
    }
  }
  if (aPos > 1) {
    // Normalize vector along the line
    aVec1.Normalize();
    aVec1.Multiply(theStep);
    aP.Translate(aVec1.Multiplied(aM));
  }
  return aP;
}

void SketcherPrs_PositionMgr::deleteConstraint(const SketcherPrs_SymbolPrs* thePrs)
{
  std::map<ObjectPtr, PositionsMap>::iterator aIt;
  std::list<ObjectPtr> aToDel;
  // Clear map for deleted presentation
  for (aIt = myShapes.begin(); aIt != myShapes.end(); ++aIt) {
    PositionsMap& aPosMap = aIt->second;
    if (aPosMap.count(thePrs) > 0) {
      // Erase index
      aPosMap.erase(aPosMap.find(thePrs));
      if (aPosMap.size() == 0)
        // Delete the map
        aToDel.push_back(aIt->first);
      else {
        // Reindex objects positions in order to avoid spaces
        PositionsMap::iterator aIt;
        int i = 0;
        for (aIt = aPosMap.begin(); aIt != aPosMap.end(); aIt++, i++)
          aIt->second = i;
      }
    }
  }
  std::list<ObjectPtr>::const_iterator aListIt;
  for (aListIt = aToDel.cbegin(); aListIt != aToDel.cend(); ++aListIt) {
    myShapes.erase(*aListIt);
  }
}
