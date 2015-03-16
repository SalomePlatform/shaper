// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        SketcherPrs_PositionMgr.cpp
// Created:     13 March 2015
// Author:      Vitaly SMETANNIKOV

#include "SketcherPrs_PositionMgr.h"
#include "SketcherPrs_Tools.h"

#include <GeomAPI_Edge.h>

static const int MyStep = 20;

static SketcherPrs_PositionMgr* MyPosMgr = NULL;


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
                                              Handle(SketcherPrs_SymbolPrs) thePrs)
{
  if (myShapes.count(theLine) == 1) {
    PositionsMap& aPosMap = myShapes[theLine];
    if (aPosMap.count(thePrs.Access()) == 1) {
      return aPosMap[thePrs.Access()];
    } else {
      int aInd = aPosMap.size();
      aPosMap[thePrs.Access()] = aInd;
      return aInd;
    }
  } else {
    PositionsMap aPosMap;
    aPosMap[thePrs.Access()] = 0;
    myShapes[theLine] = aPosMap;
    return 0;
  }
}

gp_Pnt SketcherPrs_PositionMgr::getPosition(ObjectPtr theLine, 
                                            Handle(SketcherPrs_SymbolPrs) thePrs)
{
  std::shared_ptr<GeomAPI_Shape> aShape = SketcherPrs_Tools::getLine(theLine);
  std::shared_ptr<GeomAPI_Edge> aEdge = 
    std::shared_ptr<GeomAPI_Edge>(new GeomAPI_Edge(aShape));

  std::shared_ptr<GeomAPI_Pnt> aPnt1 = aEdge->firstPoint();
  std::shared_ptr<GeomAPI_Pnt> aPnt2 = aEdge->lastPoint();

  // Find the middle point
  gp_Pnt aP((aPnt1->x() + aPnt2->x())/2.,
            (aPnt1->y() + aPnt2->y())/2.,
            (aPnt1->z() + aPnt2->z())/2.);

  gp_Vec aVec1(aPnt1->impl<gp_Pnt>(), aPnt2->impl<gp_Pnt>());
  gp_Vec aShift = aVec1.Crossed(thePrs->plane()->norm()->impl<gp_Dir>());
  aShift.Normalize();
  aShift.Multiply(MyStep);

  int aPos = getPositionIndex(theLine, thePrs);
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
      if (aPos % 4 == 0) 
        aM = (aPos - 1) / 4;
      else
        aM = -(aPos + 1) / 4;
    }
  }
  if (aPos > 1) {
    // Normalize vector along the line
    aVec1.Normalize();
    aVec1.Multiply(MyStep);
    aP.Translate(aVec1.Multiplied(aM));
  }
  return aP;
}

void SketcherPrs_PositionMgr::deleteConstraint(Handle(SketcherPrs_SymbolPrs) thePrs)
{
  std::map<ObjectPtr, PositionsMap>::iterator aIt;
  for (aIt = myShapes.begin(); aIt != myShapes.end(); ++aIt) {
    PositionsMap& aPosMap = aIt->second;
    if (aPosMap.count(thePrs.Access()) > 0) 
      aPosMap.erase(aPosMap.find(thePrs.Access()));
  }
  for (aIt = myShapes.begin(); aIt != myShapes.end(); ++aIt) {
    if (aIt->second.size() == 0) {
      myShapes.erase(aIt);
      aIt = myShapes.begin();
    }
  }
}
