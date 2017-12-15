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

#include "XGUI_FacesPanel.h"

#include <Config_PropManager.h>
#include <Events_Loop.h>
#include <GeomAlgoAPI_CompoundBuilder.h>

#include <ModelAPI_Events.h>

#include <ModuleBase_IModule.h>
#include <ModuleBase_ISelection.h>
#include "ModuleBase_IWorkshop.h"
#include "ModuleBase_ListView.h"
#include "ModuleBase_ResultPrs.h"
#include "ModuleBase_Tools.h"
#include "ModuleBase_ViewerPrs.h"

#include "XGUI_ObjectsBrowser.h"
#include "XGUI_SelectionMgr.h"
#include "XGUI_SelectionFilterType.h"
#include "XGUI_Tools.h"
#include "XGUI_Workshop.h"

#include <QAction>
#include <QCheckBox>
#include <QFocusEvent>
#include <QGridLayout>
#include <QListWidget>
#include <QMainWindow>

static const int LayoutMargin = 3;

//********************************************************************
XGUI_FacesPanel::XGUI_FacesPanel(QWidget* theParent, ModuleBase_IWorkshop* theWorkshop)
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

  // clear internal containers
  myListView->getControl()->clear();
  myItems.clear();

  // restore previous view of presentations
  bool isModified = redisplayObjects(myItemObjects);
  std::set<std::shared_ptr<ModelAPI_Object> > aHiddenObjects = myHiddenObjects;
  isModified = displayHiddenObjects(aHiddenObjects, myHiddenObjects) || isModified;
  if (isModified)// && isToFlushRedisplay) // flush signal immediatelly until container is filled
    flushRedisplay();

  updateProcessedObjects(myItems, myItemObjects);
  myHiddenObjects.clear();
  myLastItemIndex = 0; // it should be after redisplay as flag used in customize
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

  theSelectionFilters.Append(aModule->selectionFilter(SF_GlobalFilter));
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
    XGUI_Tools::workshop(myWorkshop)->selector()->clearSelection();
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
void XGUI_FacesPanel::processSelection()
{
  QList<ModuleBase_ViewerPrsPtr> aSelected = myWorkshop->selection()->getSelected(
                                                       ModuleBase_ISelection::Viewer);
  bool isModified = false;
  static Events_ID aDispEvent = Events_Loop::loop()->eventByName(EVENT_OBJECT_TO_REDISPLAY);

  std::map<ObjectPtr, NCollection_List<TopoDS_Shape> > anObjectToShapes;
  std::map<ObjectPtr, Handle(ModuleBase_ResultPrs) > anObjectToPrs;
  for (int i = 0; i < aSelected.size(); i++) {
    ModuleBase_ViewerPrsPtr aPrs = aSelected[i];
    ObjectPtr anObject = aPrs->object();
    if (!anObject.get())
      continue;
    if (ModuleBase_Tools::getSelectedShape(aPrs).ShapeType() != TopAbs_FACE)
      continue;

    Handle(ModuleBase_ResultPrs) aResultPrs = Handle(ModuleBase_ResultPrs)::DownCast(
      aPrs->interactive());
    if (aResultPrs.IsNull())
      continue;

    myItems.insert(myLastItemIndex, aPrs);
    myListView->addItem(generateName(aPrs), myLastItemIndex);
    myLastItemIndex++;
    isModified = true;

    if (anObjectToShapes.find(anObject) != anObjectToShapes.end())
      anObjectToShapes.at(anObject).Append(ModuleBase_Tools::getSelectedShape(aPrs));
    else {
      NCollection_List<TopoDS_Shape> aListOfShapes;
      aListOfShapes.Append(ModuleBase_Tools::getSelectedShape(aPrs));
      anObjectToShapes[anObject] = aListOfShapes;
      anObjectToPrs[anObject] = aResultPrs;
    }
  }
  for (std::map<ObjectPtr, NCollection_List<TopoDS_Shape> >::const_iterator
    anIt = anObjectToShapes.begin(); anIt != anObjectToShapes.end(); anIt++) {
    ObjectPtr anObject = anIt->first;
    if (!anObject.get() || anObjectToPrs.find(anObject) == anObjectToPrs.end())
      continue;
    Handle(ModuleBase_ResultPrs) aResultPrs = anObjectToPrs.at(anObject);

    if (aResultPrs->hasSubShapeVisible(anIt->second) || useTransparency()) // redisplay
      ModelAPI_EventCreator::get()->sendUpdated(anObject, aDispEvent);
    else { // erase object because it is entirely hidden
      anObject->setDisplayed(false);
      myHiddenObjects.insert(anObject);
      ModelAPI_EventCreator::get()->sendUpdated(anObject, aDispEvent);
    }
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
  std::set<ObjectPtr> aRestoredObjects;
  for (std::set<int>::const_iterator anIt = aSelectedIds.begin(); anIt != aSelectedIds.end();
       anIt++) {
    ModuleBase_ViewerPrsPtr aPrs = myItems[*anIt];
    if (aRestoredObjects.find(aPrs->object()) == aRestoredObjects.end())
      aRestoredObjects.insert(aPrs->object());
    myItems.remove(*anIt);
    isModified = true;
  }
  if (isModified) {
    bool isRedisplayed = redisplayObjects(aRestoredObjects);
    isRedisplayed = displayHiddenObjects(aRestoredObjects, myHiddenObjects)
                    || isRedisplayed;
    if (isRedisplayed)
      flushRedisplay();
    // should be after flush of redisplay to have items object to be updated
    updateProcessedObjects(myItems, myItemObjects);
  }

  myListView->removeSelectedItems();
  // Restore selection
  myListView->restoreSelection(anIndices);
  //appendSelectionInHistory();
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
bool XGUI_FacesPanel::displayHiddenObjects(
  const std::set<std::shared_ptr<ModelAPI_Object> >& theObjects,
  std::set<std::shared_ptr<ModelAPI_Object> >& theHiddenObjects)
{
  if (theObjects.empty())
    return false;

  bool isModified = false;
  static Events_ID aDispEvent = Events_Loop::loop()->eventByName(EVENT_OBJECT_TO_REDISPLAY);

  for (std::set<ObjectPtr>::const_iterator anIt = theObjects.begin(); anIt != theObjects.end();
       anIt++)
  {
    ObjectPtr anObject = *anIt;
    // if the object was hidden by this panel
    if (anObject->isDisplayed() || theHiddenObjects.find(anObject) == theHiddenObjects.end())
      continue;
    theHiddenObjects.erase(anObject);
    anObject->setDisplayed(true); // it means that the object is hidden by hide all faces
    ModelAPI_EventCreator::get()->sendUpdated(anObject, aDispEvent);
    isModified = true;
  }
  return isModified;
}

//********************************************************************
bool XGUI_FacesPanel::hideEmptyObjects()
{
  bool isModified = false;
  static Events_ID aDispEvent = Events_Loop::loop()->eventByName(EVENT_OBJECT_TO_REDISPLAY);
  std::map<ObjectPtr, NCollection_List<TopoDS_Shape> > anObjectToShapes;
  std::map<ObjectPtr, Handle(ModuleBase_ResultPrs) > anObjectToPrs;

  for (QMap<int, ModuleBase_ViewerPrsPtr>::const_iterator anIt = myItems.begin();
       anIt != myItems.end(); anIt++) {
    ModuleBase_ViewerPrsPtr aPrs = anIt.value();
    ObjectPtr anObject = aPrs->object();
    if (!anObject.get() || !anObject->isDisplayed())
      continue;

    Handle(ModuleBase_ResultPrs) aResultPrs = Handle(ModuleBase_ResultPrs)::DownCast(
      aPrs->interactive());
    if (aResultPrs.IsNull())
      continue;

    if (anObjectToShapes.find(anObject) != anObjectToShapes.end())
      anObjectToShapes.at(anObject).Append(ModuleBase_Tools::getSelectedShape(aPrs));
    else {
      NCollection_List<TopoDS_Shape> aListOfShapes;
      aListOfShapes.Append(ModuleBase_Tools::getSelectedShape(aPrs));
      anObjectToShapes[anObject] = aListOfShapes;
      anObjectToPrs[anObject] = aResultPrs;
    }
  }
  for (std::map<ObjectPtr, NCollection_List<TopoDS_Shape> >::const_iterator
    anIt = anObjectToShapes.begin(); anIt != anObjectToShapes.end(); anIt++) {
    ObjectPtr anObject = anIt->first;
    if (!anObject.get() || anObjectToPrs.find(anObject) == anObjectToPrs.end())
      continue;
    Handle(ModuleBase_ResultPrs) aResultPrs = anObjectToPrs.at(anObject);

    if (!aResultPrs->hasSubShapeVisible(anIt->second)) {
      // erase object because it is entirely hidden
      anObject->setDisplayed(false);
      myHiddenObjects.insert(anObject);
      ModelAPI_EventCreator::get()->sendUpdated(anObject, aDispEvent);
      isModified = true;
    }
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
    if (anObject.get() && theObjects.find(anObject) != theObjects.end())
      continue;
    theObjects.insert(anObject);
  }
}

//********************************************************************
void XGUI_FacesPanel::closeEvent(QCloseEvent* theEvent)
{
  QDockWidget::closeEvent(theEvent);
  emit closed();
}

//********************************************************************
QString XGUI_FacesPanel::generateName(const ModuleBase_ViewerPrsPtr& thePrs)
{
  if (!thePrs.get() || !thePrs->object().get())
    return "Undefined";

  GeomShapePtr aContext;
  ObjectPtr anObject = thePrs->object();
  ResultPtr aResult = std::dynamic_pointer_cast<ModelAPI_Result>(anObject);
  if (aResult.get())
    aContext = aResult->shape();
  else {
    // TODO if there is this case
  }

  QString aName = anObject->data()->name().c_str();
  if (aContext.get()) {
    GeomShapePtr aSubShape(new GeomAPI_Shape());
    aSubShape->setImpl(new TopoDS_Shape(ModuleBase_Tools::getSelectedShape(thePrs)));
    if (!aSubShape->isEqual(aContext))
      aName += QString("_%1").arg(GeomAlgoAPI_CompoundBuilder::id(aContext, aSubShape));
  }
  return aName;
}

//********************************************************************
bool XGUI_FacesPanel::customizeObject(const ObjectPtr& theObject,
                                      const AISObjectPtr& thePresentation)
{
  if (myLastItemIndex == 0) // do nothing because there was no activity in the pane after reset
    return false;

  if (thePresentation.get() == NULL)
    return false;

  if (myItemObjects.find(theObject) == myItemObjects.end()) // not found
    return false;

  // if the object is displayed, the hidden faces are collected and set to the presentation
  bool isModified = false;
  NCollection_List<TopoDS_Shape> aHiddenSubShapes;
  for (QMap<int, ModuleBase_ViewerPrsPtr>::const_iterator anIt = myItems.begin();
    anIt != myItems.end(); anIt++) {
    ModuleBase_ViewerPrsPtr aPrs = anIt.value();
    if (aPrs.get() && aPrs->object() != theObject)
      continue;
    TopoDS_Shape aShape = ModuleBase_Tools::getSelectedShape(aPrs);
    if (!aHiddenSubShapes.Contains(aShape))
      aHiddenSubShapes.Append(aShape);
  }

  Handle(ModuleBase_ResultPrs) aResultPrs = Handle(ModuleBase_ResultPrs)::DownCast(
    thePresentation->impl<Handle(AIS_InteractiveObject)>());
  if (aResultPrs.IsNull())
    return false;

  isModified = aResultPrs->setSubShapeHidden(aHiddenSubShapes);

  double aTransparency = !useTransparency() ? 1
    : Config_PropManager::real("Visualization", "hidden_face_transparency");
  isModified = aResultPrs->setHiddenSubShapeTransparency(aTransparency) || isModified;

  return isModified;
}

//********************************************************************
void XGUI_FacesPanel::onDeleteItem()
{
  processDelete();
}

//********************************************************************
void XGUI_FacesPanel::onTransparencyChanged()
{
  bool isModified = false;
  if (useTransparency()) {
    std::set<std::shared_ptr<ModelAPI_Object> > aHiddenObjects = myHiddenObjects;
    isModified = displayHiddenObjects(aHiddenObjects, myHiddenObjects);
  }
  else
    isModified = hideEmptyObjects();

  isModified = redisplayObjects(myItemObjects) || isModified;
  if (isModified)
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
  XGUI_ObjectsBrowser* anObjectBrowser = XGUI_Tools::workshop(myWorkshop)->objectBrowser();
  if (anObjectBrowser)
    anObjectBrowser->updateAllIndexes();
}
