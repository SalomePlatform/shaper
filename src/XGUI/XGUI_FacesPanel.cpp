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

#include "XGUI_FacesPanel.h"
#include "XGUI_ObjectsBrowser.h"
#include "XGUI_SelectionMgr.h"
#include "XGUI_Selection.h"
#include "XGUI_Tools.h"
#include "XGUI_Workshop.h"
#include "XGUI_Displayer.h"
#include "XGUI_ViewerProxy.h"

#include <ModuleBase_IModule.h>
#include <ModuleBase_ISelection.h>
#include <ModuleBase_IWorkshop.h>
#include <ModuleBase_IViewer.h>
#include <ModuleBase_ListView.h>
#include <ModuleBase_ResultPrs.h>
#include <ModuleBase_Tools.h>
#include <ModuleBase_ViewerPrs.h>
#include <ModuleBase_SelectionFilterType.h>

#include <Config_PropManager.h>
#include <Events_Loop.h>
#include <GeomAlgoAPI_CompoundBuilder.h>
#include <GeomAPI_Shape.h>

#include <ModelAPI_Events.h>
#include <ModelAPI_ResultGroup.h>
#include <ModelAPI_AttributeSelectionList.h>

#include <QAction>
#include <QCheckBox>
#include <QFocusEvent>
#include <QGridLayout>
#include <QListWidget>
#include <QMainWindow>

static const int LayoutMargin = 3;

//********************************************************************
XGUI_FacesPanel::XGUI_FacesPanel(QWidget* theParent, XGUI_Workshop* theWorkshop)
  : QDockWidget(theParent), myIsActive(false), myWorkshop(theWorkshop)
{
  setWindowTitle(tr("Hide Faces"));
  QAction* aViewAct = toggleViewAction();
  setStyleSheet("::title { position: relative; padding-left: 5px; text-align: left center }");

  QWidget* aContent = new QWidget(this);
  QGridLayout* aMainLayout = new QGridLayout(aContent);
  aMainLayout->setContentsMargins(LayoutMargin, LayoutMargin, LayoutMargin, LayoutMargin);
  setWidget(aContent);

  myHiddenOrTransparent = new QCheckBox(tr("Transparent"), aContent);
  connect(myHiddenOrTransparent, SIGNAL(toggled(bool)), SLOT(onTransparencyChanged()));

  myListView = new ModuleBase_ListView(aContent, "", "Hidden/transparent faces in 3D view");
  connect(myListView, SIGNAL(deleteActionClicked()), SLOT(onDeleteItem()));

  aMainLayout->addWidget(myHiddenOrTransparent, 0, 0);
  aMainLayout->addWidget(myListView->getControl(), 1, 0);

  myListView->getControl()->setFocusPolicy(Qt::StrongFocus);
  myListView->getControl()->viewport()->installEventFilter(this);
}

//********************************************************************
void XGUI_FacesPanel::reset(const bool isToFlushRedisplay)
{
  if (myLastItemIndex == 0) // do nothing because there was no activity in the pane after reset
    return;

  std::map<ObjectPtr, TopoDS_ListOfShape> anObjectToShapes;
  std::map<ObjectPtr, Handle(ModuleBase_ResultPrs) > anObjectToPrs;
  QMap<int, std::shared_ptr<ModuleBase_ViewerPrs> >::const_iterator aIt;
  for (aIt = myItems.cbegin(); aIt != myItems.cend(); aIt++) {
    getObjectsMapFromPrs(aIt.value(), anObjectToShapes, anObjectToPrs);
  }

  std::set<ObjectPtr> aObjects;
  TopoDS_ListOfShape anEmpty;
  std::map<ObjectPtr, Handle(ModuleBase_ResultPrs)>::const_iterator aPrsIt;
  for (aPrsIt = anObjectToPrs.cbegin(); aPrsIt != anObjectToPrs.cend(); aPrsIt++) {
    aObjects.insert(aPrsIt->first);
    aPrsIt->second->setSubShapeHidden(anEmpty);
  }
  std::set<std::shared_ptr<ModelAPI_Object> >::const_iterator aGrpIt;
  for (aGrpIt = myHiddenGroups.cbegin(); aGrpIt != myHiddenGroups.cend(); aGrpIt++)
    (*aGrpIt)->setDisplayed(true);
  myHiddenGroups.clear();

  if (redisplayObjects(aObjects))
    flushRedisplay();

  // clear internal containers
  myListView->getControl()->clear();
  myItems.clear();
  updateProcessedObjects(myItems, myItemObjects);
  myLastItemIndex = 0; // it should be after redisplay as flag used in customize
  myHiddenObjects.clear();
}

//********************************************************************
bool XGUI_FacesPanel::isEmpty() const
{
  return myItems.size() == 0;
}

//********************************************************************
void XGUI_FacesPanel::selectionModes(QIntList& theModes)
{
  theModes.append(TopAbs_FACE);
}

//********************************************************************
void XGUI_FacesPanel::selectionFilters(SelectMgr_ListOfFilter& theSelectionFilters)
{
  ModuleBase_IModule* aModule = myWorkshop->module();
  QIntList aModuleSelectionFilters = myWorkshop->module()->selectionFilters();

  // The global filter makes problem for groups selection when any operation is launched
  // theSelectionFilters.Append(aModule->selectionFilter(SF_GlobalFilter));
  theSelectionFilters.Append(aModule->selectionFilter(SF_FilterInfinite));
  theSelectionFilters.Append(aModule->selectionFilter(SF_ResultGroupNameFilter));
}

//********************************************************************
bool XGUI_FacesPanel::eventFilter(QObject* theObject, QEvent *theEvent)
{
  QWidget* aWidget = qobject_cast<QWidget*>(theObject);
  if (theEvent->type() == QEvent::MouseButtonRelease)
  {
    if (myListView->getControl()->viewport() == aWidget)
      setActivePanel(true);
  }
  // pass the event on to the parent class
  return QObject::eventFilter(theObject, theEvent);
}

//********************************************************************
void XGUI_FacesPanel::setActivePanel(const bool theIsActive)
{
  if (myIsActive == theIsActive)
    return;

  ModuleBase_Tools::setShadowEffect(myListView->getControl(), theIsActive);
  myIsActive = theIsActive;

  if (myIsActive) {
    emit activated();
    // selection should be cleared after emit of signal to do not process selection change
    // event by the previous selector
    // the selection is cleared by activating selection control
    myWorkshop->selector()->clearSelection();
  }
  else
    emit deactivated();
}

//********************************************************************
bool XGUI_FacesPanel::useTransparency() const
{
  return myHiddenOrTransparent->isChecked();
}

//********************************************************************
double XGUI_FacesPanel::transparency() const
{
  return useTransparency() ?
    Config_PropManager::real("Visualization", "hidden_face_transparency") : 1;
}


//********************************************************************
void XGUI_FacesPanel::restoreObjects(const std::set<ObjectPtr>& theHiddenObjects)
{
  std::set<int> anIndicesToBeRemoved;
  for (QMap<int, ModuleBase_ViewerPrsPtr>::const_iterator anItemsIt = myItems.begin();
    anItemsIt != myItems.end(); anItemsIt++)
  {
    ModuleBase_ViewerPrsPtr aPrs = anItemsIt.value();
    ObjectPtr anObject = aPrs->object();
    if (theHiddenObjects.find(anObject) == theHiddenObjects.end()) // not found
      continue;
    anIndicesToBeRemoved.insert(anItemsIt.key());
  }

  // remove from myItes container
  for (std::set<int>::const_iterator aToBeRemovedIt = anIndicesToBeRemoved.begin();
    aToBeRemovedIt != anIndicesToBeRemoved.end(); aToBeRemovedIt++)
  {
    myItems.remove(*aToBeRemovedIt);
  }
  if (!anIndicesToBeRemoved.empty()) // means that myItems has been changed
    updateProcessedObjects(myItems, myItemObjects);
  myListView->removeItems(anIndicesToBeRemoved);

  // remove from container of hidden objects
  for (std::set<ObjectPtr>::const_iterator aHiddenIt = theHiddenObjects.begin();
    aHiddenIt != theHiddenObjects.end(); aHiddenIt++)
  {
    if (myHiddenObjects.find(*aHiddenIt) != myHiddenObjects.end()) /// found objects
      myHiddenObjects.erase(*aHiddenIt);
  }
}

//********************************************************************
bool XGUI_FacesPanel::processAction(ModuleBase_ActionType theActionType)
{
  switch (theActionType) {
    //case ActionEnter:
    //  return processEnter();
    case ActionEscape:
      setActivePanel(false);
      return true;
    case ActionDelete:
      return processDelete();
    //case ActionUndo:
    //case ActionRedo:
    default:
      return false;
  }
}

//********************************************************************
void XGUI_FacesPanel::getObjectsMapFromPrs(ModuleBase_ViewerPrsPtr thePrs,
  std::map<ObjectPtr, TopoDS_ListOfShape>& theObjectToShapes,
  std::map<ObjectPtr, Handle(ModuleBase_ResultPrs) >& theObjectToPrs)
{
  ObjectPtr anObject = thePrs->object();
  if (!anObject.get())
    return;

  XGUI_Displayer* aDisplayer = myWorkshop->displayer();
  ResultGroupPtr aResGroup = std::dynamic_pointer_cast<ModelAPI_ResultGroup>(anObject);
  if (aResGroup.get()) {
    // Process a grouip result
    FeaturePtr aGroupFeature = ModelAPI_Feature::feature(aResGroup);
    AttributeSelectionListPtr aSelectionList = aGroupFeature->selectionList("group_list");
    AISObjectPtr aPrs;
    for (int i = 0; i < aSelectionList->size(); i++) {
      AttributeSelectionPtr aSelection = aSelectionList->value(i);
      ResultPtr aRes = aSelection->context();
      aPrs = aDisplayer->getAISObject(aRes);
      if (aPrs.get()) {
        Handle(ModuleBase_ResultPrs) aResultPrs = Handle(ModuleBase_ResultPrs)::DownCast(
          aPrs->impl<Handle(AIS_InteractiveObject)>());
        if (!aResultPrs.IsNull()) {
          GeomShapePtr aShape = aSelection->value();
          if (theObjectToShapes.find(aRes) != theObjectToShapes.end())
            theObjectToShapes.at(aRes).Append(aShape->impl<TopoDS_Shape>());
          else {
            TopoDS_ListOfShape aListOfShapes;
            aListOfShapes.Append(aShape->impl<TopoDS_Shape>());
            theObjectToShapes[aRes] = aListOfShapes;
            theObjectToPrs[aRes] = aResultPrs;
          }
        }
      }
    }
  }
  else {
    // Process bodies
    Handle(ModuleBase_ResultPrs) aResultPrs = Handle(ModuleBase_ResultPrs)::DownCast(
      thePrs->interactive());
    if (aResultPrs.IsNull())
      return;

    if (theObjectToShapes.find(anObject) != theObjectToShapes.end())
      theObjectToShapes.at(anObject).Append(ModuleBase_Tools::getSelectedShape(thePrs));
    else {
      TopoDS_ListOfShape aListOfShapes;
      aListOfShapes.Append(ModuleBase_Tools::getSelectedShape(thePrs));
      theObjectToShapes[anObject] = aListOfShapes;
      theObjectToPrs[anObject] = aResultPrs;
    }
  }
}

//********************************************************************
void XGUI_FacesPanel::processSelection()
{
  QList<ModuleBase_ViewerPrsPtr> aSelected =
    myWorkshop->selector()->selection()->getSelected(ModuleBase_ISelection::Viewer);

  if (aSelected.size() == 0)
    return;

  bool isModified = false;
  static Events_ID aDispEvent = Events_Loop::loop()->eventByName(EVENT_OBJECT_TO_REDISPLAY);

  std::map<ObjectPtr, TopoDS_ListOfShape> anObjectToShapes;
  std::map<ObjectPtr, Handle(ModuleBase_ResultPrs) > anObjectToPrs;
  std::set<int> aToRemove;

  for (int i = 0; i < aSelected.size(); i++) {
    ModuleBase_ViewerPrsPtr aPrs = aSelected[i];
    ObjectPtr anObject = aPrs->object();
    if (!anObject.get())
      continue;

    ResultGroupPtr aResGroup = std::dynamic_pointer_cast<ModelAPI_ResultGroup>(anObject);
    if (aResGroup.get()) {
      FeaturePtr aFeature = ModelAPI_Feature::feature(aResGroup);
      if (aFeature.get()) {
        AttributeSelectionListPtr aSelectionListAttr = aFeature->data()->selectionList("group_list");
        std::string aType = aSelectionListAttr->selectionType();
        if (aType != "Faces")
          continue;
      }
    }
    else {
      GeomShapePtr aShapePtr = aPrs->shape();
      if (!aShapePtr.get() || !aShapePtr->isFace())
        continue;
    }

    QString aItemName = aResGroup.get()?
      aResGroup->data()->name().c_str() : XGUI_Tools::generateName(aPrs);
    if (myListView->hasItem(aItemName))
      continue;

    getObjectsMapFromPrs(aPrs, anObjectToShapes, anObjectToPrs);
    if (aResGroup.get() && aResGroup->isDisplayed()) {
      aResGroup->setDisplayed(false);
      myHiddenGroups.insert(aResGroup);
    }

    // The code is dedicated to remove already selected items if they are selected twice
    // It can happen in case of groups selection
    QMap<int, ModuleBase_ViewerPrsPtr>::const_iterator aIt;
    for (aIt = myItems.cbegin(); aIt != myItems.cend(); aIt++) {
      ModuleBase_ViewerPrsPtr aPrs = aIt.value();
      ObjectPtr aObject = aPrs->object();
      ResultGroupPtr aResGroup = std::dynamic_pointer_cast<ModelAPI_ResultGroup>(aObject);
      if (aResGroup.get())
        continue;
      if (anObjectToShapes.find(aObject) != anObjectToShapes.end()) {
        TopoDS_ListOfShape aShapes = anObjectToShapes[aObject];
        GeomShapePtr aShapePtr = aPrs->shape();
        if (aShapes.Contains(aShapePtr->impl<TopoDS_Shape>())) {
          aToRemove.insert(aIt.key());
        }
      }
    }

    myItems.insert(myLastItemIndex, aPrs);
    myListView->addItem(aItemName, myLastItemIndex);
    myLastItemIndex++;
    isModified = true;
  }

  // Hide fully hidden shapes
  std::map<ObjectPtr, TopoDS_ListOfShape>::const_iterator anIt;
  for (anIt = anObjectToShapes.begin(); anIt != anObjectToShapes.end(); anIt++) {
    ObjectPtr anObject = anIt->first;
    if (!anObject.get() || anObjectToPrs.find(anObject) == anObjectToPrs.end())
      continue;
    Handle(ModuleBase_ResultPrs) aResultPrs = anObjectToPrs.at(anObject);

    if (!aResultPrs->hasSubShapeVisible(anIt->second)) { // redisplay
      // erase object because it is entirely hidden
      anObject->setDisplayed(false);
      myHiddenObjects.insert(anObject);
    }
    ModelAPI_EventCreator::get()->sendUpdated(anObject, aDispEvent);
  }

  // Process selected presentations
  double aTransp = transparency();
  std::map<ObjectPtr, Handle(ModuleBase_ResultPrs)>::iterator aPrsIt;
  for (aPrsIt = anObjectToPrs.begin(); aPrsIt != anObjectToPrs.end(); aPrsIt++) {
    ObjectPtr anObject = aPrsIt->first;
    Handle(ModuleBase_ResultPrs) aPrs = aPrsIt->second;
    TopoDS_ListOfShape aAlreadyHidden = aPrs->hiddenSubShapes();
    TopoDS_ListOfShape aListOfShapes = anObjectToShapes[anObject];
    TopoDS_ListOfShape::Iterator aShapesIt(aListOfShapes);
    for (; aShapesIt.More(); aShapesIt.Next()) {
      if (!aAlreadyHidden.Contains(aShapesIt.Value()))
        aAlreadyHidden.Append(aShapesIt.Value());
    }
    aPrs->setSubShapeHidden(aAlreadyHidden);
    aPrs->setHiddenSubShapeTransparency(aTransp);
  }

  // Remove duplicate items
  if (aToRemove.size() > 0) {
    myListView->removeItems(aToRemove);
    std::set<int>::const_iterator aIntIt;
    for (aIntIt = aToRemove.cbegin(); aIntIt != aToRemove.cend(); aIntIt++)
      myItems.remove(*aIntIt);
  }
  if (isModified) {
    updateProcessedObjects(myItems, myItemObjects);
    flushRedisplay();
  }
}

//********************************************************************
bool XGUI_FacesPanel::processDelete()
{
  //appendFirstSelectionInHistory();
  QModelIndexList anIndices = myListView->getControl()->selectionModel()->selectedIndexes();

  std::set<int> aSelectedIds;
  myListView->getSelectedIndices(aSelectedIds);
  if (aSelectedIds.empty())
    return false;

  bool isModified = false;
  std::set<ModuleBase_ViewerPrsPtr> aRestored;
  std::set<int>::const_iterator anIt;
  for (anIt = aSelectedIds.begin(); anIt != aSelectedIds.end(); anIt++) {
    ModuleBase_ViewerPrsPtr aPrs = myItems[*anIt];
    if (aRestored.find(aPrs) == aRestored.end()) {
      aRestored.insert(aPrs);
      myItems.remove(*anIt);
      isModified = true;
    }
  }
  std::map<ObjectPtr, TopoDS_ListOfShape> anObjectToShapes;
  std::map<ObjectPtr, Handle(ModuleBase_ResultPrs) > anObjectToPrs;

  std::set<ModuleBase_ViewerPrsPtr>::const_iterator aIt;
  for (aIt = aRestored.cbegin(); aIt != aRestored.cend(); aIt++) {
    getObjectsMapFromPrs((*aIt), anObjectToShapes, anObjectToPrs);
    ResultGroupPtr aResGroup = std::dynamic_pointer_cast<ModelAPI_ResultGroup>((*aIt)->object());
    if (aResGroup.get()) {
      std::set<std::shared_ptr<ModelAPI_Object> >::iterator aGrpIt = myHiddenGroups.find(aResGroup);
      if (aGrpIt != myHiddenGroups.end()) {
        aResGroup->setDisplayed(true);
        myHiddenGroups.erase(aGrpIt);
      }
    }
  }

  std::set<ObjectPtr> aRestoredObjects;
  std::map<ObjectPtr, TopoDS_ListOfShape>::const_iterator aShapesIt;
  for (aShapesIt = anObjectToShapes.begin(); aShapesIt != anObjectToShapes.end(); aShapesIt++) {
    TopoDS_ListOfShape aShapes = aShapesIt->second;
    aRestoredObjects.insert(aShapesIt->first);
    Handle(ModuleBase_ResultPrs) aPrs = anObjectToPrs[aShapesIt->first];
    TopoDS_ListOfShape aHiddenShapes = aPrs->hiddenSubShapes();
    TopoDS_ListOfShape::Iterator aSubShapesIt(aShapes);
    for (; aSubShapesIt.More(); aSubShapesIt.Next()) {
      if (aHiddenShapes.Contains(aSubShapesIt.Value()))
        aHiddenShapes.Remove(aSubShapesIt.Value());
    }
    aPrs->setSubShapeHidden(aHiddenShapes);
  }
  if (redisplayObjects(aRestoredObjects))
    flushRedisplay();

  myListView->removeSelectedItems();
  return true;
}

//********************************************************************
bool XGUI_FacesPanel::redisplayObjects(
  const std::set<std::shared_ptr<ModelAPI_Object> >& theObjects)
{
  if (theObjects.empty())
    return false;

  bool isModified = false;
  static Events_ID aDispEvent = Events_Loop::loop()->eventByName(EVENT_OBJECT_TO_REDISPLAY);
  for (std::set<ObjectPtr>::const_iterator anIt = theObjects.begin(); anIt != theObjects.end();
       anIt++)
  {
    ObjectPtr anObject = *anIt;
    if (!anObject->isDisplayed())
      continue;
    ModelAPI_EventCreator::get()->sendUpdated(anObject, aDispEvent);
    isModified = true;
  }
  return isModified;
}

//********************************************************************
void XGUI_FacesPanel::updateProcessedObjects(QMap<int, ModuleBase_ViewerPrsPtr> theItems,
                                             std::set<ObjectPtr>& theObjects)
{
  theObjects.clear();
  for (QMap<int, ModuleBase_ViewerPrsPtr>::const_iterator anIt = theItems.begin();
       anIt != theItems.end(); anIt++) {
    ModuleBase_ViewerPrsPtr aPrs = anIt.value();
    ObjectPtr anObject = aPrs.get() ? aPrs->object() : ObjectPtr();
    if (anObject.get()) {
      ResultGroupPtr aResGroup = std::dynamic_pointer_cast<ModelAPI_ResultGroup>(anObject);
      if (aResGroup.get()) {
        FeaturePtr aGroupFeature = ModelAPI_Feature::feature(aResGroup);
        AttributeSelectionListPtr aSelectionList = aGroupFeature->selectionList("group_list");
        for (int i = 0; i < aSelectionList->size(); i++) {
          AttributeSelectionPtr aSelection = aSelectionList->value(i);
          ResultPtr aRes = aSelection->context();
          if (theObjects.find(aRes) == theObjects.end())
            theObjects.insert(aRes);
        }
      }
      else {
        if (theObjects.find(anObject) == theObjects.end())
          theObjects.insert(anObject);
      }
    }
  }
}

//********************************************************************
void XGUI_FacesPanel::closeEvent(QCloseEvent* theEvent)
{
  QDockWidget::closeEvent(theEvent);
  emit closed();
}

//********************************************************************
bool XGUI_FacesPanel::customizeObject(const ObjectPtr& theObject,
  const AISObjectPtr& thePresentation)
{
  return myItems.size() > 0;
}

//********************************************************************
void XGUI_FacesPanel::onDeleteItem()
{
  processDelete();
}

//********************************************************************
void XGUI_FacesPanel::onTransparencyChanged()
{
  std::map<ObjectPtr, TopoDS_ListOfShape> anObjectToShapes;
  std::map<ObjectPtr, Handle(ModuleBase_ResultPrs) > anObjectToPrs;
  QMap<int, std::shared_ptr<ModuleBase_ViewerPrs> >::const_iterator aIt;
  for (aIt = myItems.cbegin(); aIt != myItems.cend(); aIt++) {
    getObjectsMapFromPrs(aIt.value(), anObjectToShapes, anObjectToPrs);
  }

  double aTransp = Config_PropManager::real("Visualization", "hidden_face_transparency");
  std::set<ObjectPtr> aObjects;
  std::map<ObjectPtr, Handle(ModuleBase_ResultPrs)>::const_iterator aPrsIt;
  for (aPrsIt = anObjectToPrs.cbegin(); aPrsIt != anObjectToPrs.cend(); aPrsIt++) {
    aObjects.insert(aPrsIt->first);
    aPrsIt->second->setHiddenSubShapeTransparency(useTransparency()? aTransp : 1);
  }
  if (redisplayObjects(aObjects))
    flushRedisplay();
}

//********************************************************************
void XGUI_FacesPanel::onClosed()
{
  setActivePanel(false);
  reset(true);
}

//********************************************************************
void XGUI_FacesPanel::flushRedisplay() const
{
  Events_Loop::loop()->flush(Events_Loop::loop()->eventByName(EVENT_OBJECT_TO_REDISPLAY));
  // Necessary for update visibility icons in ObjectBrowser
  XGUI_ObjectsBrowser* anObjectBrowser = myWorkshop->objectBrowser();
  if (anObjectBrowser)
    anObjectBrowser->updateAllIndexes();
  myWorkshop->viewer()->update();
}
