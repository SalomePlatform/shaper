// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        PartSet_SketcherMgr.cpp
// Created:     19 Dec 2014
// Author:      Vitaly SMETANNIKOV

#include "PartSet_SketcherMgr.h"
#include "PartSet_Module.h"
#include "PartSet_WidgetPoint2D.h"
#include "PartSet_Tools.h"

#include <XGUI_ModuleConnector.h>
#include <XGUI_Displayer.h>
#include <XGUI_Workshop.h>

#include <ModuleBase_IViewer.h>
#include <ModuleBase_IWorkshop.h>
#include <ModuleBase_IViewWindow.h>
#include <ModuleBase_Operation.h>
#include <ModuleBase_ISelection.h>
#include <ModuleBase_IPropertyPanel.h>
#include <ModuleBase_Operation.h>

#include <Events_Loop.h>

#include <SketchPlugin_Line.h>
#include <SketchPlugin_Sketch.h>
#include <SketchPlugin_Point.h>
#include <SketchPlugin_Arc.h>
#include <SketchPlugin_Circle.h>
#include <SketchPlugin_ConstraintLength.h>
#include <SketchPlugin_ConstraintDistance.h>
#include <SketchPlugin_ConstraintParallel.h>
#include <SketchPlugin_ConstraintPerpendicular.h>
#include <SketchPlugin_ConstraintRadius.h>
#include <SketchPlugin_ConstraintRigid.h>

#include <ModelAPI_Events.h>

#include <QMouseEvent>
#include <QApplication>




/// Returns list of unique objects by sum of objects from List1 and List2
QList<ObjectPtr> getSumList(const QList<ModuleBase_ViewerPrs>& theList1,
                                       const QList<ModuleBase_ViewerPrs>& theList2)
{
  QList<ObjectPtr> aRes;
  foreach (ModuleBase_ViewerPrs aPrs, theList1) {
    if (!aRes.contains(aPrs.object()))
      aRes.append(aPrs.object());
  }
  foreach (ModuleBase_ViewerPrs aPrs, theList2) {
    if (!aRes.contains(aPrs.object()))
      aRes.append(aPrs.object());
  }
  return aRes;
}




PartSet_SketcherMgr::PartSet_SketcherMgr(PartSet_Module* theModule)
  : QObject(theModule), myModule(theModule), myIsDragging(false), myDragDone(false)
{
  ModuleBase_IWorkshop* aWorkshop = myModule->workshop();
  ModuleBase_IViewer* aViewer = aWorkshop->viewer();

  connect(aViewer, SIGNAL(mousePress(ModuleBase_IViewWindow*, QMouseEvent*)),
          this, SLOT(onMousePressed(ModuleBase_IViewWindow*, QMouseEvent*)));

  connect(aViewer, SIGNAL(mouseRelease(ModuleBase_IViewWindow*, QMouseEvent*)),
          this, SLOT(onMouseReleased(ModuleBase_IViewWindow*, QMouseEvent*)));

  connect(aViewer, SIGNAL(mouseMove(ModuleBase_IViewWindow*, QMouseEvent*)),
          this, SLOT(onMouseMoved(ModuleBase_IViewWindow*, QMouseEvent*)));

  connect(aViewer, SIGNAL(mouseDoubleClick(ModuleBase_IViewWindow*, QMouseEvent*)),
          this, SLOT(onMouseDoubleClick(ModuleBase_IViewWindow*, QMouseEvent*)));
}

PartSet_SketcherMgr::~PartSet_SketcherMgr()
{
  if (!myPlaneFilter.IsNull())
    myPlaneFilter.Nullify();
}

void PartSet_SketcherMgr::onMousePressed(ModuleBase_IViewWindow* theWnd, QMouseEvent* theEvent)
{
  // 
  if (!(theEvent->buttons() & Qt::LeftButton))
    return;

  ModuleBase_IWorkshop* aWorkshop = myModule->workshop();
  ModuleBase_Operation* aOperation = aWorkshop->currentOperation();
  // Use only for sketch operations
  if (aOperation && myCurrentSketch) {
    if (!PartSet_Tools::sketchPlane(myCurrentSketch))
      return;

    bool isSketcher = (aOperation->id().toStdString() == SketchPlugin_Sketch::ID());
    bool isSketchOpe = sketchOperationIdList().contains(aOperation->id());

    // Avoid non-sketch operations
    if ((!isSketchOpe) && (!isSketcher))
      return;

    bool isEditing = aOperation->isEditOperation();

    // Ignore creation sketch operation
    if ((!isSketcher) && (!isEditing))
      return;

    if (theEvent->modifiers()) {
      // If user performs multiselection
      if (isSketchOpe /* && (!isSketcher)*/)
        if (!aOperation->commit())
          aOperation->abort();
      return;
    }
    // Remember highlighted objects for editing
    ModuleBase_ISelection* aSelect = aWorkshop->selection();
    QList<ModuleBase_ViewerPrs> aHighlighted = aSelect->getHighlighted();
    QList<ModuleBase_ViewerPrs> aSelected = aSelect->getSelected();
    myEditingFeatures.clear();
    myEditingAttr.clear();

    bool aHasShift = (theEvent->modifiers() & Qt::ShiftModifier);
    QObjectPtrList aSelObjects;
    if (aHasShift)
      aSelObjects = getSumList(aHighlighted, aSelected);
    else {
      foreach (ModuleBase_ViewerPrs aPrs, aHighlighted) {
        aSelObjects.append(aPrs.object());
      }
    }
    if ((aSelObjects.size() == 0)) {
      if (isSketchOpe && (!isSketcher))
        // commit previous operation
        if (!aOperation->commit())
          aOperation->abort();
      return;
    }
    if ((aHighlighted.size() == 1) && (aSelected.size() == 0)) {
      // Move by selected shape (vertex). Can be used only for single selection
      foreach(ModuleBase_ViewerPrs aPrs, aHighlighted) {
        FeaturePtr aFeature = ModelAPI_Feature::feature(aHighlighted.first().object());
        if (aFeature) {
          myEditingFeatures.append(aFeature);
          TopoDS_Shape aShape = aPrs.shape();
          if (!aShape.IsNull()) {
            if (aShape.ShapeType() == TopAbs_VERTEX) {
              AttributePtr aAttr = PartSet_Tools::findAttributeBy2dPoint(myEditingFeatures.first(), 
                                                                         aShape, myCurrentSketch);
              if (aAttr)
                myEditingAttr.append(aAttr);
            }
          }
        }
      }
    } else {
      // Provide multi-selection. Can be used only for features
      foreach (ObjectPtr aObj, aSelObjects) {
        FeaturePtr aFeature = ModelAPI_Feature::feature(aObj);
        if (aFeature && (!myEditingFeatures.contains(aFeature)))
          myEditingFeatures.append(aFeature);
      }

    }
    // If nothing highlighted - return
    if (myEditingFeatures.size() == 0)
      return;

    if (isSketcher) {
      myIsDragging = true;
      get2dPoint(theWnd, theEvent, myCurX, myCurY);
      myDragDone = false;
      aWorkshop->viewer()->enableMultiselection(false);
      launchEditing();

    } else if (isSketchOpe && isEditing) {
      // If selected another object
      aOperation->abort();

      myIsDragging = true;
      get2dPoint(theWnd, theEvent, myCurX, myCurY);
      myDragDone = false;
      aWorkshop->viewer()->enableMultiselection(false);

      // This is necessary in order to finalize previous operation
      QApplication::processEvents();
      launchEditing();
    }
  }
}

void PartSet_SketcherMgr::onMouseReleased(ModuleBase_IViewWindow* theWnd, QMouseEvent* theEvent)
{
  ModuleBase_IWorkshop* aWorkshop = myModule->workshop();
  ModuleBase_Operation* aOp = aWorkshop->currentOperation();
  if (!aOp)
    return;
  if (!sketchOperationIdList().contains(aOp->id()))
    return;

  // Only for sketcher operations
  ModuleBase_IViewer* aViewer = aWorkshop->viewer();
  if (myIsDragging) {
    myIsDragging = false;
    if (myDragDone) {
      aViewer->enableMultiselection(true);
      aOp->commit();
      myEditingFeatures.clear();
      myEditingAttr.clear();
      return;
    }
  }
  if (!aViewer->isMultiSelectionEnabled()) {
    aViewer->enableMultiselection(true);
  }
  //aViewer->AISContext()->MoveTo(theEvent->x(), theEvent->y(), theWnd->v3dView());
  //if (theEvent->modifiers() & Qt::ShiftModifier)
  //  aViewer->AISContext()->ShiftSelect();
  //else
  //  aViewer->AISContext()->Select();
}

void PartSet_SketcherMgr::onMouseMoved(ModuleBase_IViewWindow* theWnd, QMouseEvent* theEvent)
{
  if (myIsDragging) {
    ModuleBase_Operation* aOperation = myModule->workshop()->currentOperation();
    if (aOperation->id().toStdString() == SketchPlugin_Sketch::ID())
      return; // No edit operation activated

    static Events_ID anEvent = Events_Loop::eventByName(EVENT_OBJECT_MOVED);
    Handle(V3d_View) aView = theWnd->v3dView();
    gp_Pnt aPoint = PartSet_Tools::convertClickToPoint(theEvent->pos(), aView);
    double aX, aY;
    PartSet_Tools::convertTo2D(aPoint, myCurrentSketch, aView, aX, aY);
    double dX =  aX - myCurX;
    double dY =  aY - myCurY;

    if ((aOperation->id().toStdString() == SketchPlugin_Line::ID()) &&
        (myEditingAttr.size() == 1) && 
        myEditingAttr.first()) {
      // probably we have prehighlighted point
      AttributePtr aAttr = myEditingAttr.first();
      std::string aAttrId = aAttr->id();
      ModuleBase_IPropertyPanel* aPanel = aOperation->propertyPanel();
      QList<ModuleBase_ModelWidget*> aWidgets = aPanel->modelWidgets();
      // Find corresponded widget to provide dragging
      foreach (ModuleBase_ModelWidget* aWgt, aWidgets) {
        if (aWgt->attributeID() == aAttrId) {
          PartSet_WidgetPoint2D* aWgt2d = dynamic_cast<PartSet_WidgetPoint2D*>(aWgt);
          if (aWgt2d) {
            aWgt2d->setPoint(aWgt2d->x() + dX, aWgt2d->y() + dY);
            break;
          }
        }
      }
    } else {
      foreach(FeaturePtr aFeature, myEditingFeatures) {
        std::shared_ptr<SketchPlugin_Feature> aSketchFeature =
          std::dynamic_pointer_cast<SketchPlugin_Feature>(aFeature);
        if (aSketchFeature) { 
          aSketchFeature->move(dX, dY);
          ModelAPI_EventCreator::get()->sendUpdated(aSketchFeature, anEvent);
        }
      }
      Events_Loop::loop()->flush(Events_Loop::eventByName(EVENT_OBJECT_MOVED));
      Events_Loop::loop()->flush(Events_Loop::eventByName(EVENT_OBJECT_UPDATED));
    }
    myDragDone = true;
    myCurX = aX;
    myCurY = aY;
  }
}

void PartSet_SketcherMgr::onMouseDoubleClick(ModuleBase_IViewWindow* theWnd, QMouseEvent* theEvent)
{
  ModuleBase_Operation* aOperation = myModule->workshop()->currentOperation();
  if (aOperation && aOperation->isEditOperation()) {
    std::string aId = aOperation->id().toStdString();
    if ((aId == SketchPlugin_ConstraintLength::ID()) ||
      (aId == SketchPlugin_ConstraintDistance::ID()) ||
      (aId == SketchPlugin_ConstraintRadius::ID())) 
    {
      // Activate dimension value editing on double click
      ModuleBase_IPropertyPanel* aPanel = aOperation->propertyPanel();
      QList<ModuleBase_ModelWidget*> aWidgets = aPanel->modelWidgets();
      // Find corresponded widget to activate value editing
      foreach (ModuleBase_ModelWidget* aWgt, aWidgets) {
        if (aWgt->attributeID() == "ConstraintValue") {
          aWgt->focusTo();
          return;
        }
      }
    }
  }
}

void PartSet_SketcherMgr::get2dPoint(ModuleBase_IViewWindow* theWnd, QMouseEvent* theEvent, 
                                double& theX, double& theY)
{
  Handle(V3d_View) aView = theWnd->v3dView();
  gp_Pnt aPoint = PartSet_Tools::convertClickToPoint(theEvent->pos(), aView);
  PartSet_Tools::convertTo2D(aPoint, myCurrentSketch, aView, theX, theY);
}

void PartSet_SketcherMgr::launchEditing()
{
  if (myEditingFeatures.size() > 0) {
    FeaturePtr aFeature = myEditingFeatures.first();
    std::shared_ptr<SketchPlugin_Feature> aSPFeature = 
              std::dynamic_pointer_cast<SketchPlugin_Feature>(aFeature);
    if (aSPFeature) {
      myModule->editFeature(aSPFeature);
    }
  }
}


QStringList PartSet_SketcherMgr::sketchOperationIdList()
{
  static QStringList aIds;
  if (aIds.size() == 0) {
    aIds << SketchPlugin_Line::ID().c_str();
    aIds << SketchPlugin_Point::ID().c_str();
    aIds << SketchPlugin_Arc::ID().c_str();
    aIds << SketchPlugin_Circle::ID().c_str();
    aIds << SketchPlugin_ConstraintLength::ID().c_str();
    aIds << SketchPlugin_ConstraintDistance::ID().c_str();
    aIds << SketchPlugin_ConstraintRigid::ID().c_str();
    aIds << SketchPlugin_ConstraintRadius::ID().c_str();
    aIds << SketchPlugin_ConstraintPerpendicular::ID().c_str();
    aIds << SketchPlugin_ConstraintParallel::ID().c_str();
  }
  return aIds;
}

void PartSet_SketcherMgr::startSketch(ModuleBase_Operation* theOperation)
{
  // Display all sketcher sub-Objects
  myCurrentSketch = std::dynamic_pointer_cast<ModelAPI_CompositeFeature>(theOperation->feature());
  XGUI_ModuleConnector* aConnector = dynamic_cast<XGUI_ModuleConnector*>(myModule->workshop());
  XGUI_Displayer* aDisplayer = aConnector->workshop()->displayer();

  // Hide sketcher result
  std::list<ResultPtr> aResults = myCurrentSketch->results();
  std::list<ResultPtr>::const_iterator aIt;
  for (aIt = aResults.begin(); aIt != aResults.end(); ++aIt) {
    aDisplayer->erase((*aIt), false);
  }
  aDisplayer->erase(myCurrentSketch, false);

  // Display sketcher objects
  for (int i = 0; i < myCurrentSketch->numberOfSubs(); i++) {
    FeaturePtr aFeature = myCurrentSketch->subFeature(i);
    std::list<ResultPtr> aResults = aFeature->results();
    std::list<ResultPtr>::const_iterator aIt;
    for (aIt = aResults.begin(); aIt != aResults.end(); ++aIt) {
      aDisplayer->display((*aIt), false);
    }
    aDisplayer->display(aFeature, false);
  }

  if (myPlaneFilter.IsNull()) 
    myPlaneFilter = new ModuleBase_ShapeInPlaneFilter();

  myModule->workshop()->viewer()->addSelectionFilter(myPlaneFilter);
  if (theOperation->isEditOperation()) {
    // If it is editing of sketch then it means that plane is already defined
    std::shared_ptr<GeomAPI_Pln> aPln = PartSet_Tools::sketchPlane(myCurrentSketch);
    myPlaneFilter->setPlane(aPln->impl<gp_Pln>());
  }
  aDisplayer->updateViewer();
}

void PartSet_SketcherMgr::stopSketch(ModuleBase_Operation* theOperation)
{
  DataPtr aData = myCurrentSketch->data();
  if ((!aData) || (!aData->isValid())) {
    // The sketch was aborted
    myCurrentSketch = CompositeFeaturePtr();
    myModule->workshop()->viewer()->removeSelectionFilter(myPlaneFilter);
    return; 
  }
  // Hide all sketcher sub-Objects
  XGUI_ModuleConnector* aConnector = dynamic_cast<XGUI_ModuleConnector*>(myModule->workshop());
  XGUI_Displayer* aDisplayer = aConnector->workshop()->displayer();
  for (int i = 0; i < myCurrentSketch->numberOfSubs(); i++) {
    FeaturePtr aFeature = myCurrentSketch->subFeature(i);
    std::list<ResultPtr> aResults = aFeature->results();
    std::list<ResultPtr>::const_iterator aIt;
    for (aIt = aResults.begin(); aIt != aResults.end(); ++aIt) {
      aDisplayer->erase((*aIt), false);
    }
    aDisplayer->erase(aFeature, false);
  }
  // Display sketcher result
  std::list<ResultPtr> aResults = myCurrentSketch->results();
  std::list<ResultPtr>::const_iterator aIt;
  for (aIt = aResults.begin(); aIt != aResults.end(); ++aIt) {
    aDisplayer->display((*aIt), false);
  }
  aDisplayer->display(myCurrentSketch);
    
  myCurrentSketch = CompositeFeaturePtr();
  myModule->workshop()->viewer()->removeSelectionFilter(myPlaneFilter);
  aDisplayer->updateViewer();
}


void PartSet_SketcherMgr::onPlaneSelected(const std::shared_ptr<GeomAPI_Pln>& thePln)
{
  myPlaneFilter->setPlane(thePln->impl<gp_Pln>());
}
