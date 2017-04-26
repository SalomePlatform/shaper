// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        PartSet_ExternalPointsMgr.cpp
// Created:     26 April 2017
// Author:      Vitaly SMETANNIKOV

#include "PartSet_ExternalPointsMgr.h"
#include "PartSet_CenterPrs.h"
#include "PartSet_Tools.h"

#include <ModelAPI_Tools.h>

#include <ModuleBase_IWorkshop.h>
#include <ModuleBase_ViewerPrs.h>

#include <GeomAPI_Circ.h>
#include <GeomAPI_Edge.h>
#include <GeomAPI_Ellipse.h>
#include <GeomAPI_Pnt.h>
#include <GeomAPI_ShapeExplorer.h>

#include <XGUI_Tools.h>
#include <XGUI_Displayer.h>
#include <XGUI_Workshop.h>

PartSet_ExternalPointsMgr::PartSet_ExternalPointsMgr(ModuleBase_IWorkshop* theWorkshop,
                                                     const CompositeFeaturePtr& theSketch)
  : QObject(theWorkshop), myWorkshop(theWorkshop), mySketch(theSketch)
{
  XGUI_Workshop* aWorkshop = XGUI_Tools::workshop(myWorkshop);
  XGUI_Displayer* aDisplayer = aWorkshop->displayer();
  connect(aDisplayer, SIGNAL(objectDisplayed(ObjectPtr, AISObjctPtr)),
                      SLOT(onDisplayObject(ObjectPtr, AISObjectPtr)));
  connect(aDisplayer, SIGNAL(beforeObjectErase(ObjectPtr, AISObjectPtr)),
                      SLOT(onEraseObject(ObjectPtr, AISObjectPtr)));

  updateCenterPresentations();
}


PartSet_ExternalPointsMgr::~PartSet_ExternalPointsMgr()
{
  if (myPresentations.isEmpty())
    return;
  XGUI_Workshop* aWorkshop = XGUI_Tools::workshop(myWorkshop);
  XGUI_Displayer* aDisplayer = aWorkshop->displayer();
  QMapIterator<ObjectPtr, ListOfAIS> aIt(myPresentations);
  while (aIt.hasNext()) {
    aIt.next();
    ListOfAIS aAISList = aIt.value();
    foreach (AISObjectPtr aAIS, aAISList) {
      aDisplayer->eraseAIS(aAIS, false);
    }
  }
}


//******************************************************
QList<std::shared_ptr<ModuleBase_ViewerPrs>> PartSet_ExternalPointsMgr::findCircularEdgesInPlane()
{
  QList<std::shared_ptr<ModuleBase_ViewerPrs>> aResult;
  XGUI_Workshop* aWorkshop = XGUI_Tools::workshop(myWorkshop);
  XGUI_Displayer* aDisplayer = aWorkshop->displayer();
  QObjectPtrList aDispObjects = aDisplayer->displayedObjects();

  std::shared_ptr<GeomAPI_Pln> aPlane = plane();
  foreach(ObjectPtr aObj, aDispObjects) {
    if (myPresentations.contains(aObj))
      continue;

    // Do not process objects of the current sketch
    if (isSketchObject(aObj))
      continue;

    ResultPtr aResObj = std::dynamic_pointer_cast<ModelAPI_Result>(aObj);
    if (aResObj.get()) {
      GeomShapePtr aShape = aResObj->shape();
      if (aShape.get()) {
        GeomAPI_ShapeExplorer aExplorer(aShape, GeomAPI_Shape::EDGE);
        for(; aExplorer.more(); aExplorer.next()) {
          GeomShapePtr aEdgeShape = aExplorer.current();
          GeomAPI_Edge anEdge(aEdgeShape);
          if ((anEdge.isCircle() || anEdge.isArc() || anEdge.isEllipse()) &&
               anEdge.isInPlane(aPlane)) {
            bool isContains = false;
            // Check that edge is not already used.
            // It is possible that the same edge will be taken from different faces
            foreach(std::shared_ptr<ModuleBase_ViewerPrs> aPrs, aResult) {
              GeomAPI_Edge aUsedEdge(aPrs->shape());
              if (aUsedEdge.isEqual(aEdgeShape)) {
                isContains = true;
                break;
              }
            }
            if (!isContains) {
              std::shared_ptr<ModuleBase_ViewerPrs>
                aPrs(new ModuleBase_ViewerPrs(aResObj, aEdgeShape));
              aResult.append(aPrs);
            }
          }
        }
      }
    }
  }
  return aResult;
}


void PartSet_ExternalPointsMgr::updateCenterPresentations()
{
  // Return if there is no plane defined
  if (!plane().get())
    return;

  XGUI_Workshop* aWorkshop = XGUI_Tools::workshop(myWorkshop);
  XGUI_Displayer* aDisplayer = aWorkshop->displayer();

  QList<std::shared_ptr<ModuleBase_ViewerPrs>> aEdgesPrs = findCircularEdgesInPlane();
  foreach(std::shared_ptr<ModuleBase_ViewerPrs> aPrs, aEdgesPrs) {
    GeomAPI_Edge aEdge(aPrs->shape());
    if (aEdge.isArc() || aEdge.isCircle()) {
      GeomCirclePtr aCircle = aEdge.circle();
      GeomPointPtr aCenter = aCircle->center();
      Handle(PartSet_CenterPrs) aCentPrs =
        new PartSet_CenterPrs(aPrs->object(), aPrs->shape(), aCenter->impl<gp_Pnt>());

      AISObjectPtr anAIS(new GeomAPI_AISObject());
      anAIS->setImpl(new Handle(AIS_InteractiveObject)(aCentPrs));
      aDisplayer->displayAIS(anAIS, false);
      ListOfAIS aList;
      aList.append(anAIS);
      myPresentations[aPrs->object()] = aList;
    } else if (aEdge.isEllipse()) {
      GeomEllipsePtr aEllipse = aEdge.ellipse();
      GeomPointPtr aF1 = aEllipse->firstFocus();
      GeomPointPtr aF2 = aEllipse->secondFocus();
      Handle(PartSet_CenterPrs) aF1Prs =
        new PartSet_CenterPrs(aPrs->object(), aPrs->shape(), aF1->impl<gp_Pnt>());
      Handle(PartSet_CenterPrs) aF2Prs =
        new PartSet_CenterPrs(aPrs->object(), aPrs->shape(), aF2->impl<gp_Pnt>());

      ListOfAIS aList;
      AISObjectPtr anAIS1(new GeomAPI_AISObject());
      anAIS1->setImpl(new Handle(AIS_InteractiveObject)(aF1Prs));
      aDisplayer->displayAIS(anAIS1, false);
      aList.append(anAIS1);

      AISObjectPtr anAIS2(new GeomAPI_AISObject());
      anAIS2->setImpl(new Handle(AIS_InteractiveObject)(aF2Prs));
      aDisplayer->displayAIS(anAIS2, false);
      aList.append(anAIS2);
      myPresentations[aPrs->object()] = aList;
    }
  }
}

std::shared_ptr<GeomAPI_Pln> PartSet_ExternalPointsMgr::plane() const
{
  return PartSet_Tools::sketchPlane(mySketch);
}

void PartSet_ExternalPointsMgr::onDisplayObject(ObjectPtr theObj, AISObjectPtr theAIS)
{
  updateCenterPresentations();
}

void PartSet_ExternalPointsMgr::onEraseObject(ObjectPtr theObj, AISObjectPtr theAIS)
{
  updateCenterPresentations();
}


bool PartSet_ExternalPointsMgr::isSketchObject(const ObjectPtr& theRes) const
{
  FeaturePtr aFeature = ModelAPI_Feature::feature(theRes);
  if (!aFeature.get())
    return false;
  CompositeFeaturePtr aComp = ModelAPI_Tools::compositeOwner(aFeature);
  return aComp == mySketch;
}