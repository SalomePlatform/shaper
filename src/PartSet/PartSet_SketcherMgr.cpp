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
#include <XGUI_Selection.h>
#include <XGUI_SelectionMgr.h>

#include <ModuleBase_IViewer.h>
#include <ModuleBase_IWorkshop.h>
#include <ModuleBase_IViewWindow.h>
#include <ModuleBase_Operation.h>
#include <ModuleBase_ISelection.h>
#include <ModuleBase_IPropertyPanel.h>
#include <ModuleBase_Operation.h>

#include <GeomDataAPI_Point2D.h>

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

#include <SelectMgr_IndexedMapOfOwner.hxx>
#include <StdSelect_BRepOwner.hxx>

#include <ModelAPI_Events.h>

#include <QMouseEvent>
#include <QApplication>


/// Returns list of unique objects by sum of objects from List1 and List2
/*QList<ModuleBase_ViewerPrs> getSumList(const QList<ModuleBase_ViewerPrs>& theList1,
                                       const QList<ModuleBase_ViewerPrs>& theList2)
{
  QList<ModuleBase_ViewerPrs> aRes;
  foreach (ModuleBase_ViewerPrs aPrs, theList1) {
    if (!aRes.contains(aPrs))
      aRes.append(aPrs);
  }
  foreach (ModuleBase_ViewerPrs aPrs, theList2) {
    if (!aRes.contains(aPrs))
      aRes.append(aPrs);
  }
  return aRes;
}*/

void fillFeature2Attribute(const QList<ModuleBase_ViewerPrs>& theList,
                           QMap<FeaturePtr, QList<AttributePtr> >& theFeature2AttributeMap,
                           const FeaturePtr theSketch)
{
  QList<ModuleBase_ViewerPrs> aRes;

  QList<ModuleBase_ViewerPrs>::const_iterator anIt = theList.begin(),
                                              aLast = theList.end();
  for (; anIt != aLast; anIt++)
  {
    ModuleBase_ViewerPrs aPrs = *anIt;
    FeaturePtr aFeature = ModelAPI_Feature::feature(aPrs.object());
    if (aFeature.get() == NULL)
      continue;

    QList<AttributePtr> anAttributes;
    if (theFeature2AttributeMap.contains(aFeature)) {
      anAttributes = theFeature2AttributeMap[aFeature];
    }
    AttributePtr anAttr;
    TopoDS_Shape aShape = aPrs.shape();
    if (!aShape.IsNull()) {
      if (aShape.ShapeType() == TopAbs_VERTEX) {
        anAttr = PartSet_Tools::findAttributeBy2dPoint(aFeature, aShape, theSketch);
        if (anAttr.get() != NULL && !anAttributes.contains(anAttr))
          anAttributes.push_back(anAttr);
      }
    }
    theFeature2AttributeMap[aFeature] = anAttributes;
  }
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

    // MoveTo in order to highlight current object
    ModuleBase_IViewer* aViewer = aWorkshop->viewer();
    aViewer->AISContext()->MoveTo(theEvent->x(), theEvent->y(), theWnd->v3dView());

    // Remember highlighted objects for editing
    ModuleBase_ISelection* aSelect = aWorkshop->selection();
    QList<ModuleBase_ViewerPrs> aHighlighted = aSelect->getHighlighted();
    QList<ModuleBase_ViewerPrs> aSelected = aSelect->getSelected();
    myFeature2AttributeMap.clear();

    bool aHasShift = (theEvent->modifiers() & Qt::ShiftModifier);
    if (aHasShift) {
      fillFeature2Attribute(aHighlighted, myFeature2AttributeMap, myCurrentSketch);
      fillFeature2Attribute(aSelected, myFeature2AttributeMap, myCurrentSketch);
    }
    else {
      fillFeature2Attribute(aHighlighted, myFeature2AttributeMap, myCurrentSketch);
    }

    if (myFeature2AttributeMap.empty()) {
      if (isSketchOpe && (!isSketcher))
        // commit previous operation
        if (!aOperation->commit())
          aOperation->abort();
      return;
    }

    if (isSketcher) {
      myIsDragging = true;
      get2dPoint(theWnd, theEvent, myCurX, myCurY);
      myDragDone = false;
      aWorkshop->viewer()->enableMultiselection(false);
      launchEditing();

    } else if (isSketchOpe && isEditing) {
      // If selected another object commit current result
      aOperation->commit();

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
      //aOp->commit();
      myFeature2AttributeMap.clear();

      // Reselect edited object
      aViewer->AISContext()->MoveTo(theEvent->x(), theEvent->y(), theWnd->v3dView());
      if (theEvent->modifiers() & Qt::ShiftModifier)
        aViewer->AISContext()->ShiftSelect();
      else
        aViewer->AISContext()->Select();
      return;
    }
  }
  if (!aViewer->isMultiSelectionEnabled()) {
    aViewer->enableMultiselection(true);
  }
}

void PartSet_SketcherMgr::onMouseMoved(ModuleBase_IViewWindow* theWnd, QMouseEvent* theEvent)
{
  if (myIsDragging) {
    ModuleBase_Operation* aOperation = myModule->workshop()->currentOperation();
    if (aOperation->id().toStdString() == SketchPlugin_Sketch::ID())
      return; // No edit operation activated

    Handle(V3d_View) aView = theWnd->v3dView();
    gp_Pnt aPoint = PartSet_Tools::convertClickToPoint(theEvent->pos(), aView);
    double aX, aY;
    PartSet_Tools::convertTo2D(aPoint, myCurrentSketch, aView, aX, aY);
    double dX =  aX - myCurX;
    double dY =  aY - myCurY;

    ModuleBase_IWorkshop* aWorkshop = myModule->workshop();
    XGUI_ModuleConnector* aConnector = dynamic_cast<XGUI_ModuleConnector*>(aWorkshop);
    XGUI_Displayer* aDisplayer = aConnector->workshop()->displayer();
    bool isEnableUpdateViewer = aDisplayer->enableUpdateViewer(false);

    static Events_ID aMoveEvent = Events_Loop::eventByName(EVENT_OBJECT_MOVED);
    //static Events_ID aUpdateEvent = Events_Loop::eventByName(EVENT_OBJECT_UPDATED);

    FeatureToAttributesMap::const_iterator anIt = myFeature2AttributeMap.begin(),
                                           aLast = myFeature2AttributeMap.end();
    FeatureToSelectionMap aCurrentSelection;
    for (; anIt != aLast; anIt++) {
      FeaturePtr aFeature = anIt.key();
      AttributePtr anAttr;

      AttributeList anAttributes = anIt.value();
      if (!anAttributes.empty()) {
        anAttr = anAttributes.first();
      }

      // save the previous selection
      getCurrentSelection(aFeature, myCurrentSketch, aWorkshop, aCurrentSelection);
      // save the previous selection: end

      // Process selection by attribute: the priority to the attribute
      if (anAttr.get() != NULL) {
        std::string aAttrId = anAttr->id();
        DataPtr aData = aFeature->data();
        if (aData.get() != NULL) {
          std::shared_ptr<GeomDataAPI_Point2D> aPoint = 
            std::dynamic_pointer_cast<GeomDataAPI_Point2D>(aData->attribute(aAttrId));
          if (aPoint.get() != NULL) {
            bool isImmutable = aPoint->setImmutable(true);
            aPoint->move(dX, dY);
            ModelAPI_EventCreator::get()->sendUpdated(aFeature, aMoveEvent);
            aPoint->setImmutable(isImmutable);
          }
        }
      } else {
        // Process selection by feature
        std::shared_ptr<SketchPlugin_Feature> aSketchFeature =
          std::dynamic_pointer_cast<SketchPlugin_Feature>(aFeature);
        if (aSketchFeature) {
          aSketchFeature->move(dX, dY);
          ModelAPI_EventCreator::get()->sendUpdated(aSketchFeature, aMoveEvent);
        }
      }
    }
    Events_Loop::loop()->flush(aMoveEvent); // up all move events - to be processed in the solver
    //Events_Loop::loop()->flush(aUpdateEvent); // up update events - to redisplay presentations

    // restore the previous selection
    FeatureToSelectionMap::const_iterator aSIt = aCurrentSelection.begin(),
                                          aSLast = aCurrentSelection.end();
    SelectMgr_IndexedMapOfOwner anOwnersToSelect;
    for (; aSIt != aSLast; aSIt++) {
      anOwnersToSelect.Clear();
      getSelectionOwners(aSIt->first, myCurrentSketch, aWorkshop, aCurrentSelection,
                         anOwnersToSelect);
      aConnector->workshop()->selector()->setSelectedOwners(anOwnersToSelect, false);
    }
    // restore the previous selection: end

    aDisplayer->enableUpdateViewer(isEnableUpdateViewer);
    aDisplayer->updateViewer();
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
  // there should be activate the vertex selection mode because the edit can happens by the selected
  // point
  QIntList aModes;
  aModes << TopAbs_VERTEX << TopAbs_EDGE;
  XGUI_ModuleConnector* aConnector = dynamic_cast<XGUI_ModuleConnector*>(myModule->workshop());
  aConnector->activateSubShapesSelection(aModes);

  if (!myFeature2AttributeMap.empty()) {
    FeaturePtr aFeature = myFeature2AttributeMap.begin().key();
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

void PartSet_SketcherMgr::getCurrentSelection(const FeaturePtr& theFeature,
                                              const FeaturePtr& theSketch,
                                              ModuleBase_IWorkshop* theWorkshop,
                                              FeatureToSelectionMap& theSelection)
                                              //std::set<AttributePtr>& theSelectedAttributes,
                                              //std::set<ResultPtr>& theSelectedResults)
{
  if (theFeature.get() == NULL)
    return;

  std::set<AttributePtr> aSelectedAttributes;
  std::set<ResultPtr> aSelectedResults;

  ModuleBase_IViewer* aViewer = theWorkshop->viewer();
  Handle(AIS_InteractiveContext) aContext = aViewer->AISContext();
  XGUI_ModuleConnector* aConnector = dynamic_cast<XGUI_ModuleConnector*>(theWorkshop);
  XGUI_Displayer* aDisplayer = aConnector->workshop()->displayer();

  std::list<ResultPtr> aResults = theFeature->results();
  std::list<ResultPtr>::const_iterator aIt;
  for (aIt = aResults.begin(); aIt != aResults.end(); ++aIt)
  {
    ResultPtr aResult = *aIt;
    AISObjectPtr aAISObj = aDisplayer->getAISObject(aResult);
    if (aAISObj.get() == NULL)
      continue;
    Handle(AIS_InteractiveObject) anAISIO = aAISObj->impl<Handle(AIS_InteractiveObject)>();
    for (aContext->InitSelected(); aContext->MoreSelected(); aContext->NextSelected())
    {
      Handle(StdSelect_BRepOwner) aBRepOwner = Handle(StdSelect_BRepOwner)::DownCast(
                                                                      aContext->SelectedOwner());
      if (aBRepOwner.IsNull())
        continue;
      Handle(AIS_InteractiveObject) anIO = Handle(AIS_InteractiveObject)::DownCast(
                                                                        aBRepOwner->Selectable());
      if (aBRepOwner->HasShape()) {
        const TopoDS_Shape& aShape = aBRepOwner->Shape();
        TopAbs_ShapeEnum aShapeType = aShape.ShapeType();
        if (aShapeType == TopAbs_VERTEX) {
          AttributePtr aPntAttr = PartSet_Tools::findAttributeBy2dPoint(theFeature,
                                                                        aShape, theSketch);
          if (aPntAttr.get() != NULL)
            aSelectedAttributes.insert(aPntAttr);
        }
        else if (aShapeType == TopAbs_EDGE &&
                 aSelectedResults.find(aResult) == aSelectedResults.end()) {
          aSelectedResults.insert(aResult);
        }
      }
    }
  }
  theSelection[theFeature] = std::make_pair(aSelectedAttributes, aSelectedResults);
}

void PartSet_SketcherMgr::getSelectionOwners(const FeaturePtr& theFeature,
                                             const FeaturePtr& theSketch,
                                             ModuleBase_IWorkshop* theWorkshop,
                                             //const std::set<AttributePtr>& theSelectedAttributes,
                                             //const std::set<ResultPtr>& theSelectedResults,
                                             const FeatureToSelectionMap& theSelection,
                                             SelectMgr_IndexedMapOfOwner& anOwnersToSelect)
{
  if (theFeature.get() == NULL)
    return;

  FeatureToSelectionMap::const_iterator anIt = theSelection.find(theFeature);
  std::set<AttributePtr> aSelectedAttributes = anIt->second.first;
  std::set<ResultPtr> aSelectedResults = anIt->second.second;

  ModuleBase_IViewer* aViewer = theWorkshop->viewer();
  Handle(AIS_InteractiveContext) aContext = aViewer->AISContext();
  XGUI_ModuleConnector* aConnector = dynamic_cast<XGUI_ModuleConnector*>(theWorkshop);
  XGUI_Displayer* aDisplayer = aConnector->workshop()->displayer();

  std::list<ResultPtr> aResults = theFeature->results();
  std::list<ResultPtr>::const_iterator aIt;
  for (aIt = aResults.begin(); aIt != aResults.end(); ++aIt)
  {
    ResultPtr aResult = *aIt;
    AISObjectPtr aAISObj = aDisplayer->getAISObject(aResult);
    if (aAISObj.get() == NULL)
      continue; 
    Handle(AIS_InteractiveObject) anAISIO = aAISObj->impl<Handle(AIS_InteractiveObject)>();

    SelectMgr_IndexedMapOfOwner aSelectedOwners;  
    aConnector->workshop()->selector()->selection()->entityOwners(anAISIO, aSelectedOwners);
    for  ( Standard_Integer i = 1, n = aSelectedOwners.Extent(); i <= n; i++ ) {
      Handle(StdSelect_BRepOwner) anOwner = Handle(StdSelect_BRepOwner)::DownCast(aSelectedOwners(i));
      if ( anOwner.IsNull() || !anOwner->HasShape() )
        continue;
      const TopoDS_Shape& aShape = anOwner->Shape();
      TopAbs_ShapeEnum aShapeType = aShape.ShapeType();
      if (aShapeType == TopAbs_VERTEX) {
        AttributePtr aPntAttr = PartSet_Tools::findAttributeBy2dPoint(theFeature, aShape, theSketch);
        if (aPntAttr.get() != NULL &&
            aSelectedAttributes.find(aPntAttr) != aSelectedAttributes.end()) {
          anOwnersToSelect.Add(anOwner);
        }
      }
      else if (aShapeType == TopAbs_EDGE) {
        bool aFound = aSelectedResults.find(aResult) != aSelectedResults.end();
        if (aSelectedResults.find(aResult) != aSelectedResults.end() &&
            anOwnersToSelect.FindIndex(anOwner) <= 0)
          anOwnersToSelect.Add(anOwner);
      }
    }
  }
}
