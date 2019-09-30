// Copyright (C) 2017-2019  CEA/DEN, EDF R&D
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

// File:        PartSet_ExternalPointsMgr.cpp
// Created:     26 April 2017
// Author:      Vitaly SMETANNIKOV

#include "PartSet_ExternalPointsMgr.h"
#include "PartSet_CenterPrs.h"
#include "PartSet_Tools.h"

#include <ModelAPI_Tools.h>
#include <ModelAPI_ResultField.h>
#include <ModelAPI_ResultGroup.h>

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
#include <XGUI_SelectionActivate.h>
#include <XGUI_SelectionMgr.h>

PartSet_ExternalPointsMgr::PartSet_ExternalPointsMgr(ModuleBase_IWorkshop* theWorkshop,
                                                     const CompositeFeaturePtr& theSketch)
  : QObject(theWorkshop), myWorkshop(theWorkshop), mySketch(theSketch)
{
  XGUI_Workshop* aWorkshop = XGUI_Tools::workshop(myWorkshop);
  XGUI_Displayer* aDisplayer = aWorkshop->displayer();
  connect(aDisplayer, SIGNAL(objectDisplayed(ObjectPtr, AISObjectPtr)),
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
  if (!aWorkshop)
    return;

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

    // Do not use Fields and groups in selection in sketcher
    if (!aResObj.get() ||
      (aResObj->groupName() == ModelAPI_ResultField::group()) ||
      (aResObj->groupName() == ModelAPI_ResultGroup::group()))
      continue;

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
  XGUI_Workshop* aWorkshop = XGUI_Tools::workshop(myWorkshop);
  // Return if there is no plane defined
  if (!plane().get()) {
    connect(aWorkshop->selector(), SIGNAL(selectionChanged()),
                                   SLOT(onSelectionChanged()));
    return;
  }
  XGUI_Displayer* aDisplayer = aWorkshop->displayer();

  QList<std::shared_ptr<ModuleBase_ViewerPrs>> aEdgesPrs = findCircularEdgesInPlane();
  foreach(std::shared_ptr<ModuleBase_ViewerPrs> aPrs, aEdgesPrs) {
    GeomEdgePtr aEdge(new GeomAPI_Edge(aPrs->shape()));
    ListOfAIS aList;
    if (aEdge->isArc() || aEdge->isCircle()) {
      GeomCirclePtr aCircle = aEdge->circle();
      GeomPointPtr aCenter = aCircle->center();
      Handle(PartSet_CenterPrs) aCentPrs =
        new PartSet_CenterPrs(aPrs->object(), aEdge, aCenter->impl<gp_Pnt>(),
                              ModelAPI_AttributeSelection::CIRCLE_CENTER);

      AISObjectPtr anAIS(new GeomAPI_AISObject());
      anAIS->setImpl(new Handle(AIS_InteractiveObject)(aCentPrs));
      aList.append(anAIS);
    } else if (aEdge->isEllipse()) {
      GeomEllipsePtr aEllipse = aEdge->ellipse();
      GeomPointPtr aF1 = aEllipse->firstFocus();
      GeomPointPtr aF2 = aEllipse->secondFocus();
      Handle(PartSet_CenterPrs) aF1Prs =
        new PartSet_CenterPrs(aPrs->object(), aEdge, aF1->impl<gp_Pnt>(),
                              ModelAPI_AttributeSelection::ELLIPSE_FIRST_FOCUS);
      Handle(PartSet_CenterPrs) aF2Prs =
        new PartSet_CenterPrs(aPrs->object(), aEdge, aF2->impl<gp_Pnt>(),
                              ModelAPI_AttributeSelection::ELLIPSE_SECOND_FOCUS);

      AISObjectPtr anAIS1(new GeomAPI_AISObject());
      anAIS1->setImpl(new Handle(AIS_InteractiveObject)(aF1Prs));
      aList.append(anAIS1);

      AISObjectPtr anAIS2(new GeomAPI_AISObject());
      anAIS2->setImpl(new Handle(AIS_InteractiveObject)(aF2Prs));
      aList.append(anAIS2);
    }
    if (myPresentations.contains(aPrs->object()))
      myPresentations[aPrs->object()].append(aList);
    else
      myPresentations[aPrs->object()] = aList;
    foreach(AISObjectPtr anAIS, aList) {
      aDisplayer->displayAIS(anAIS, false, 0, false);
      aWorkshop->selectionActivate()->activateAIS(anAIS->impl<Handle(AIS_InteractiveObject)>(),
        TopAbs_VERTEX, false);
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
  if (myPresentations.contains(theObj)) {
    XGUI_Workshop* aWorkshop = XGUI_Tools::workshop(myWorkshop);
    XGUI_Displayer* aDisplayer = aWorkshop->displayer();
    ListOfAIS aList = myPresentations[theObj];
    foreach(AISObjectPtr aAIS, aList) {
      aDisplayer->eraseAIS(aAIS, false);
    }
    myPresentations.remove(theObj);
    aDisplayer->updateViewer();
  }
}


bool PartSet_ExternalPointsMgr::isSketchObject(const ObjectPtr& theRes) const
{
  FeaturePtr aFeature = ModelAPI_Feature::feature(theRes);
  if (!aFeature.get())
    return false;
  CompositeFeaturePtr aComp = ModelAPI_Tools::compositeOwner(aFeature);
  return aComp == mySketch;
}

void PartSet_ExternalPointsMgr::onSelectionChanged()
{
  if (plane().get()) {
    XGUI_Workshop* aWorkshop = XGUI_Tools::workshop(myWorkshop);
    disconnect(aWorkshop->selector(), SIGNAL(selectionChanged()),
               this, SLOT(onSelectionChanged()));
    updateCenterPresentations();
  }
}
