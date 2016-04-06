// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        SketcherPrs_PositionMgr.cpp
// Created:     13 March 2015
// Author:      Vitaly SMETANNIKOV

#include "SketcherPrs_PositionMgr.h"
#include "SketcherPrs_Tools.h"

#include <GeomAPI_Edge.h>
#include <GeomAPI_Curve.h>
#include <GeomAPI_Vertex.h>
#include <GeomAPI_Dir.h>

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

gp_Pnt SketcherPrs_PositionMgr::getPosition(ObjectPtr theShape, 
                                            const SketcherPrs_SymbolPrs* thePrs,
                                            double theStep)
{
  std::shared_ptr<GeomAPI_Shape> aShape = SketcherPrs_Tools::getShape(theShape);
  gp_Pnt aP; // Central point
  gp_Vec aVec1; // main vector
  if (aShape->isEdge()) {
    std::shared_ptr<GeomAPI_Curve> aCurve = std::shared_ptr<GeomAPI_Curve>(new GeomAPI_Curve(aShape));
    std::shared_ptr<GeomAPI_Pnt> aPnt1; // Start point of main vector
    std::shared_ptr<GeomAPI_Pnt> aPnt2; // End point of main vector
    if (aCurve->isLine()) {
      std::shared_ptr<GeomAPI_Edge> aEdge = 
        std::shared_ptr<GeomAPI_Edge>(new GeomAPI_Edge(aShape));

      aPnt1 = aEdge->firstPoint();
      aPnt2 = aEdge->lastPoint();

      // Find the middle point
      aP = gp_Pnt((aPnt1->x() + aPnt2->x())/2.,
                  (aPnt1->y() + aPnt2->y())/2.,
                  (aPnt1->z() + aPnt2->z())/2.);

    } else {
      // this is a circle or arc
      double aMidParam = (aCurve->startParam() + aCurve->endParam()) / 2.;
      std::shared_ptr<GeomAPI_Pnt> aPnt = aCurve->getPoint(aMidParam);
      aP = aPnt->impl<gp_Pnt>();

      aPnt1 = aCurve->getPoint((aMidParam + aCurve->endParam()) / 2.);
      aPnt2 = aCurve->getPoint((aMidParam + aCurve->startParam()) / 2.);
    }
    aVec1 = gp_Vec(aPnt1->impl<gp_Pnt>(), aPnt2->impl<gp_Pnt>());
  } else {
    // This is a point
    std::shared_ptr<GeomAPI_Vertex> aVertex = std::shared_ptr<GeomAPI_Vertex>(new GeomAPI_Vertex(aShape));
    std::shared_ptr<GeomAPI_Pnt> aPnt = aVertex->point();
    aP = aPnt->impl<gp_Pnt>();

    std::shared_ptr<GeomAPI_Dir> aDir = thePrs->plane()->dirX();
    aVec1 = gp_Vec(aDir->impl<gp_Dir>());
  }
  // Compute shifting vector for a one symbol
  gp_Vec aShift = aVec1.Crossed(thePrs->plane()->normal()->impl<gp_Dir>());
  aShift.Normalize();
  aShift.Multiply(theStep * 0.8);

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
