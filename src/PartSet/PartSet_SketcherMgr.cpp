// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        PartSet_SketcherMgr.cpp
// Created:     19 Dec 2014
// Author:      Vitaly SMETANNIKOV

#include "PartSet_SketcherMgr.h"
#include "PartSet_Module.h"
#include "PartSet_WidgetPoint2d.h"
#include "PartSet_Tools.h"

#include <XGUI_ModuleConnector.h>
#include <XGUI_Displayer.h>
#include <XGUI_Workshop.h>
#include <XGUI_Selection.h>
#include <XGUI_SelectionMgr.h>
#include <ModuleBase_ModelWidget.h>
#include <XGUI_ModuleConnector.h>
#include <XGUI_PropertyPanel.h>

#include <AppElements_MainWindow.h>

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

//#include <AIS_DimensionSelectionMode.hxx>
//#include <AIS_Shape.hxx>

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
  : QObject(theModule), myModule(theModule), myIsDragging(false), myDragDone(false),
     myIsMouseOverWindow(false), myIsPropertyPanelValueChanged(false)
{
  ModuleBase_IWorkshop* anIWorkshop = myModule->workshop();
  ModuleBase_IViewer* aViewer = anIWorkshop->viewer();

  myPreviousSelectionEnabled = true;//aViewer->isSelectionEnabled();

  connect(aViewer, SIGNAL(mousePress(ModuleBase_IViewWindow*, QMouseEvent*)),
          this, SLOT(onMousePressed(ModuleBase_IViewWindow*, QMouseEvent*)));

  connect(aViewer, SIGNAL(mouseRelease(ModuleBase_IViewWindow*, QMouseEvent*)),
          this, SLOT(onMouseReleased(ModuleBase_IViewWindow*, QMouseEvent*)));

  connect(aViewer, SIGNAL(mouseMove(ModuleBase_IViewWindow*, QMouseEvent*)),
          this, SLOT(onMouseMoved(ModuleBase_IViewWindow*, QMouseEvent*)));

  connect(aViewer, SIGNAL(mouseDoubleClick(ModuleBase_IViewWindow*, QMouseEvent*)),
          this, SLOT(onMouseDoubleClick(ModuleBase_IViewWindow*, QMouseEvent*)));

  XGUI_ModuleConnector* aConnector = dynamic_cast<XGUI_ModuleConnector*>(anIWorkshop);
  XGUI_Workshop* aWorkshop = aConnector->workshop();
  connect(aWorkshop, SIGNAL(applicationStarted()), this, SLOT(onApplicationStarted()));
}

PartSet_SketcherMgr::~PartSet_SketcherMgr()
{
  if (!myPlaneFilter.IsNull())
    myPlaneFilter.Nullify();
}

void PartSet_SketcherMgr::onMouseMoveOverWindow(bool theOverWindow)
{
  myIsMouseOverWindow = theOverWindow;
  if (theOverWindow)
    myIsPropertyPanelValueChanged = false;

  updateVisibilityOfCreatedFeature();
}

void PartSet_SketcherMgr::onValuesChangedInPropertyPanel()
{
  myIsPropertyPanelValueChanged = true;

  updateVisibilityOfCreatedFeature();
}

void PartSet_SketcherMgr::onMousePressed(ModuleBase_IViewWindow* theWnd, QMouseEvent* theEvent)
{
  get2dPoint(theWnd, theEvent, myClickedPoint);

  if (!(theEvent->buttons() & Qt::LeftButton))
    return;

  // Clear dragging mode
  myIsDragging = false;

  ModuleBase_IWorkshop* aWorkshop = myModule->workshop();
  ModuleBase_IViewer* aViewer = aWorkshop->viewer();
  if (!aViewer->canDragByMouse())
    return;

  ModuleBase_Operation* aOperation = aWorkshop->currentOperation();
  if (aOperation && aOperation->isEditOperation()) {
    ModuleBase_IPropertyPanel* aPanel = aOperation->propertyPanel();
    ModuleBase_ModelWidget* aActiveWgt = aPanel->activeWidget();
    // If the current widget is a selector, do do nothing, it processes the mouse press
    if(aActiveWgt && aActiveWgt->isViewerSelector()) {
      return;
    }
  }

  // Use only for sketch operations
  if (aOperation && myCurrentSketch) {
    if (!PartSet_Tools::sketchPlane(myCurrentSketch))
      return;

    bool isSketcher = isSketchOperation(aOperation);
    bool isSketchOpe = isNestedSketchOperation(aOperation);

    // Avoid non-sketch operations
    if ((!isSketchOpe) && (!isSketcher))
      return;

    bool isEditing = aOperation->isEditOperation();

    // Ignore creation sketch operation
    if ((!isSketcher) && (!isEditing))
      return;

    // MoveTo in order to highlight current object
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
      get2dPoint(theWnd, theEvent, myCurrentPoint);
      myDragDone = false;
      launchEditing();

    } else if (isSketchOpe && isEditing) {
      // If selected another object commit current result
      aOperation->commit();

      myIsDragging = true;
      get2dPoint(theWnd, theEvent, myCurrentPoint);
      myDragDone = false;

      // This is necessary in order to finalize previous operation
      QApplication::processEvents();
      launchEditing();
    }
  }
}

void PartSet_SketcherMgr::onMouseReleased(ModuleBase_IViewWindow* theWnd, QMouseEvent* theEvent)
{
  ModuleBase_IWorkshop* aWorkshop = myModule->workshop();
  ModuleBase_IViewer* aViewer = aWorkshop->viewer();
  if (!aViewer->canDragByMouse())
    return;
  ModuleBase_Operation* aOp = aWorkshop->currentOperation();
  if (aOp) {
    if (isNestedSketchOperation(aOp)) {
      get2dPoint(theWnd, theEvent, myClickedPoint);

      // Only for sketcher operations
      if (myIsDragging) {
        if (myDragDone) {
          //aOp->commit();
          myFeature2AttributeMap.clear();

          // Reselect edited object
          /*aViewer->AISContext()->MoveTo(theEvent->x(), theEvent->y(), theWnd->v3dView());
          if (theEvent->modifiers() & Qt::ShiftModifier)
            aViewer->AISContext()->ShiftSelect();
          else
            aViewer->AISContext()->Select();
            */
        }
      }
    }
  }
  aWorkshop->viewer()->enableSelection(myPreviousSelectionEnabled);
  myIsDragging = false;
}

void PartSet_SketcherMgr::onMouseMoved(ModuleBase_IViewWindow* theWnd, QMouseEvent* theEvent)
{
  myClickedPoint.clear();

  if (myIsDragging) {
    ModuleBase_IWorkshop* aWorkshop = myModule->workshop();
    // 1. it is necessary to save current selection in order to restore it after the features moving
    FeatureToSelectionMap aCurrentSelection;
    getCurrentSelection(myFeature2AttributeMap, myCurrentSketch, aWorkshop, aCurrentSelection);

    // 2. the enable selection in the viewer should be temporary switched off in order to ignore
    // mouse press signal in the viewer(it call Select for AIS context and the dragged objects are
    // deselected). This flag should be restored in the slot, processed the mouse release signal.
    ModuleBase_IViewer* aViewer = myModule->workshop()->viewer();
    aViewer->enableSelection(false);

    ModuleBase_Operation* aOperation = myModule->workshop()->currentOperation();
    if (!aOperation)
      return;
    if (isSketchOperation(aOperation))
      return; // No edit operation activated

    Handle(V3d_View) aView = theWnd->v3dView();
    gp_Pnt aPoint = PartSet_Tools::convertClickToPoint(theEvent->pos(), aView);
    double aX, aY;
    PartSet_Tools::convertTo2D(aPoint, myCurrentSketch, aView, aX, aY);
    double dX =  aX - myCurrentPoint.myCurX;
    double dY =  aY - myCurrentPoint.myCurY;

    XGUI_ModuleConnector* aConnector = dynamic_cast<XGUI_ModuleConnector*>(aWorkshop);
    XGUI_Displayer* aDisplayer = aConnector->workshop()->displayer();
    // 3. the flag to disable the update viewer should be set in order to avoid blinking in the 
    // viewer happens by deselect/select the modified objects. The flag should be restored after
    // the selection processing. The update viewer should be also called.
    bool isEnableUpdateViewer = aDisplayer->enableUpdateViewer(false);

    static Events_ID aMoveEvent = Events_Loop::eventByName(EVENT_OBJECT_MOVED);
    //static Events_ID aUpdateEvent = Events_Loop::eventByName(EVENT_OBJECT_UPDATED);

    FeatureToAttributesMap::const_iterator anIt = myFeature2AttributeMap.begin(),
                                           aLast = myFeature2AttributeMap.end();
    // 4. the features and attributes modification(move)
    for (; anIt != aLast; anIt++) {
      FeaturePtr aFeature = anIt.key();

      AttributeList anAttributes = anIt.value();
      // Process selection by attribute: the priority to the attribute
      if (!anAttributes.empty()) {
        AttributeList::const_iterator anAttIt = anAttributes.begin(), anAttLast = anAttributes.end();
        for (; anAttIt != anAttLast; anAttIt++) {
          AttributePtr anAttr = *anAttIt;
          if (anAttr.get() == NULL)
            continue;
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

    // 5. it is necessary to save current selection in order to restore it after the features moving
    FeatureToSelectionMap::const_iterator aSIt = aCurrentSelection.begin(),
                                          aSLast = aCurrentSelection.end();
    SelectMgr_IndexedMapOfOwner anOwnersToSelect;
    for (; aSIt != aSLast; aSIt++) {
      anOwnersToSelect.Clear();
      getSelectionOwners(aSIt->first, myCurrentSketch, aWorkshop, aCurrentSelection,
                         anOwnersToSelect);
      aConnector->workshop()->selector()->setSelectedOwners(anOwnersToSelect, false);
    }

    // 6. restore the update viewer flag and call this update
    aDisplayer->enableUpdateViewer(isEnableUpdateViewer);
    aDisplayer->updateViewer();
    myDragDone = true;
    myCurrentPoint.setValue(aX, aY);
  }
}

void PartSet_SketcherMgr::onMouseDoubleClick(ModuleBase_IViewWindow* theWnd, QMouseEvent* theEvent)
{
  ModuleBase_Operation* aOperation = myModule->workshop()->currentOperation();
  if (aOperation && aOperation->isEditOperation()) {
    std::string aId = aOperation->id().toStdString();
    if (isDistanceOperation(aOperation))
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

void PartSet_SketcherMgr::onApplicationStarted()
{
  ModuleBase_IWorkshop* anIWorkshop = myModule->workshop();
  XGUI_ModuleConnector* aConnector = dynamic_cast<XGUI_ModuleConnector*>(anIWorkshop);
  XGUI_Workshop* aWorkshop = aConnector->workshop();
  XGUI_PropertyPanel* aPropertyPanel = aWorkshop->propertyPanel();
  if (aPropertyPanel) {
    connect(aPropertyPanel, SIGNAL(beforeWidgetActivated(ModuleBase_ModelWidget*)),
            this, SLOT(onBeforeWidgetActivated(ModuleBase_ModelWidget*)));
  }

  AppElements_MainWindow* aMainWindow = aWorkshop->mainWindow();
  connect(aMainWindow, SIGNAL(mouseMoveOverWindow(bool)), this, SLOT(onMouseMoveOverWindow(bool)));
}

void PartSet_SketcherMgr::onBeforeWidgetActivated(ModuleBase_ModelWidget* theWidget)
{
  if (!myClickedPoint.myIsInitialized)
    return;

  ModuleBase_Operation* aOperation = myModule->workshop()->currentOperation();
  // the distance constraint feature should not use the clickedd point
  // this is workaround in order to don't throw down the flyout point value,
  // set by execute() method of these type of features
  if (isDistanceOperation(aOperation))
    return;

  PartSet_WidgetPoint2D* aPnt2dWgt = dynamic_cast<PartSet_WidgetPoint2D*>(theWidget);
  if (aPnt2dWgt) {
    aPnt2dWgt->setPoint(myClickedPoint.myCurX, myClickedPoint.myCurY);
  }
}

void PartSet_SketcherMgr::get2dPoint(ModuleBase_IViewWindow* theWnd, QMouseEvent* theEvent, 
                                     Point& thePoint)
{
  Handle(V3d_View) aView = theWnd->v3dView();
  gp_Pnt aPoint = PartSet_Tools::convertClickToPoint(theEvent->pos(), aView);
  double aX, anY;
  PartSet_Tools::convertTo2D(aPoint, myCurrentSketch, aView, aX, anY);
  thePoint.setValue(aX, anY);
}

void PartSet_SketcherMgr::launchEditing()
{
  // there should be activate the vertex selection mode because the edit can happens by the selected
  // point
  QIntList aModes;
  aModes << TopAbs_VERTEX << TopAbs_EDGE;
  // TODO: #391 - to be uncommented
  /*aModes.append(AIS_DSM_Text);
  aModes.append(AIS_DSM_Line);
  aModes.append(AIS_Shape::SelectionMode((TopAbs_ShapeEnum) TopAbs_VERTEX));
  aModes.append(AIS_Shape::SelectionMode((TopAbs_ShapeEnum) TopAbs_EDGE));*/

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

bool PartSet_SketcherMgr::isSketchOperation(ModuleBase_Operation* theOperation)
{
  return theOperation && theOperation->id().toStdString() == SketchPlugin_Sketch::ID();
}

bool PartSet_SketcherMgr::isNestedSketchOperation(ModuleBase_Operation* theOperation)
{
  return theOperation &&
         PartSet_SketcherMgr::sketchOperationIdList().contains(theOperation->id());
}

bool PartSet_SketcherMgr::isDistanceOperation(ModuleBase_Operation* theOperation)
{
  std::string aId = theOperation ? theOperation->id().toStdString() : "";

  return (aId == SketchPlugin_ConstraintLength::ID()) ||
         (aId == SketchPlugin_ConstraintDistance::ID()) ||
         (aId == SketchPlugin_ConstraintRadius::ID());
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
  XGUI_ModuleConnector* aConnector = dynamic_cast<XGUI_ModuleConnector*>(myModule->workshop());
  XGUI_Displayer* aDisplayer = aConnector->workshop()->displayer();

  DataPtr aData = myCurrentSketch->data();
  if ((!aData) || (!aData->isValid())) {
    // The sketch was aborted
    myCurrentSketch = CompositeFeaturePtr();
    myModule->workshop()->viewer()->removeSelectionFilter(myPlaneFilter);

    // Erase all sketcher objects
    QStringList aSketchIds = sketchOperationIdList();
    QObjectPtrList aObjects = aDisplayer->displayedObjects();
    foreach (ObjectPtr aObj, aObjects) {
      DataPtr aObjData = aObj->data();
      if ((!aObjData) || (!aObjData->isValid()))
        aDisplayer->erase(aObj);
    }
    return; 
  }
  // Hide all sketcher sub-Objects
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

void PartSet_SketcherMgr::startNestedSketch(ModuleBase_Operation* )
{
  connectToPropertyPanel(true);
}

void PartSet_SketcherMgr::stopNestedSketch(ModuleBase_Operation* )
{
  connectToPropertyPanel(false);
}

bool PartSet_SketcherMgr::canDisplayObject(const ObjectPtr& theObject) const
{
  return myIsMouseOverWindow || myIsPropertyPanelValueChanged;
}

void PartSet_SketcherMgr::onPlaneSelected(const std::shared_ptr<GeomAPI_Pln>& thePln)
{
  myPlaneFilter->setPlane(thePln->impl<gp_Pln>());
}

void PartSet_SketcherMgr::getCurrentSelection(const FeatureToAttributesMap& theFeatureToAttributes,
                                              const FeaturePtr& theSketch,
                                              ModuleBase_IWorkshop* theWorkshop,
                                              FeatureToSelectionMap& theSelection)
{
  FeatureToAttributesMap::const_iterator anIt = theFeatureToAttributes.begin(),
                                         aLast = theFeatureToAttributes.end();
  for (; anIt != aLast; anIt++) {
    FeaturePtr aFeature = anIt.key();
    getCurrentSelection(aFeature, theSketch, theWorkshop, theSelection);
  }
}

void PartSet_SketcherMgr::getCurrentSelection(const FeaturePtr& theFeature,
                                              const FeaturePtr& theSketch,
                                              ModuleBase_IWorkshop* theWorkshop,
                                              FeatureToSelectionMap& theSelection)
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
      if (anIO != anAISIO)
        continue;

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

void PartSet_SketcherMgr::connectToPropertyPanel(const bool isToConnect)
{
  ModuleBase_IWorkshop* anIWorkshop = myModule->workshop();
  XGUI_ModuleConnector* aConnector = dynamic_cast<XGUI_ModuleConnector*>(anIWorkshop);
  XGUI_Workshop* aWorkshop = aConnector->workshop();
  XGUI_PropertyPanel* aPropertyPanel = aWorkshop->propertyPanel();
  if (aPropertyPanel) {
    const QList<ModuleBase_ModelWidget*>& aWidgets = aPropertyPanel->modelWidgets();
    foreach (ModuleBase_ModelWidget* aWidget, aWidgets) {
      if (isToConnect)
        connect(aWidget, SIGNAL(valuesChanged()), this, SLOT(onValuesChangedInPropertyPanel()));
      else
        disconnect(aWidget, SIGNAL(valuesChanged()), this, SLOT(onValuesChangedInPropertyPanel()));
    }
  }
}

void PartSet_SketcherMgr::updateVisibilityOfCreatedFeature()
{
  ModuleBase_IWorkshop* aWorkshop = myModule->workshop();
  ModuleBase_Operation* aOperation = aWorkshop->currentOperation();
  if (!aOperation || aOperation->isEditOperation())
    return;
  XGUI_ModuleConnector* aConnector = dynamic_cast<XGUI_ModuleConnector*>(aWorkshop);
  XGUI_Displayer* aDisplayer = aConnector->workshop()->displayer();

  FeaturePtr aFeature = aOperation->feature();
  std::list<ResultPtr> aResults = aFeature->results();
  std::list<ResultPtr>::const_iterator aIt;
  for (aIt = aResults.begin(); aIt != aResults.end(); ++aIt) {
    if (canDisplayObject(aFeature)) {
      aDisplayer->display(*aIt, false);
    }
    else {
      aDisplayer->erase(*aIt, false);
    }
  }
  aDisplayer->updateViewer();
}