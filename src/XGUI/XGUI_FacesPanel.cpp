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

#include <ModuleBase_ISelection.h>
#include "ModuleBase_IWorkshop.h"
#include "ModuleBase_ListView.h"
#include "ModuleBase_ResultPrs.h"
#include "ModuleBase_Tools.h"
#include "ModuleBase_ViewerPrs.h"

#include "XGUI_Displayer.h"
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
  // restore presentation state
  bool isModified = false;
  std::set<ObjectPtr> aRestoredObjects;
  for (QMap<int, ModuleBase_ViewerPrsPtr>::const_iterator anIt = myItems.begin();
    anIt != myItems.end(); anIt++) {
    if (aRestoredObjects.find(anIt.value()->object()) == aRestoredObjects.end())
      aRestoredObjects.insert(anIt.value()->object());
  }
  // clear internal containers
  myListView->getControl()->clear();
  myLastItemIndex = 0;
  myItems.clear();

  isModified = redisplayObjects(aRestoredObjects, isToFlushRedisplay);

  if (isToFlushRedisplay && isModified)
    XGUI_Tools::workshop(myWorkshop)->displayer()->updateViewer();
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

  if (myIsActive)
  {
    emit activated();
    // selection should be activated after emit signal, that deactivates current widget(selection)
    activateSelection(theIsActive);
  }
  else
  {
    // selection should be activated after emit signal, that deactivates current widget(selection)
    activateSelection(theIsActive);
    emit deactivated();
  }
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
    myItems.remove(*aToBeRemovedIt);

  myListView->removeItems(anIndicesToBeRemoved);

  // remove from container of hidden objects
  for (std::set<ObjectPtr>::const_iterator aHiddenIt = theHiddenObjects.begin();
    aHiddenIt != theHiddenObjects.end(); aHiddenIt++)
  {
    if (myHiddenObjects.find(*aHiddenIt) != myHiddenObjects.end()) ///< found objects
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
  for (int i = 0; i < aSelected.size(); i++) {
    ModuleBase_ViewerPrsPtr aPrs = aSelected[i];
    ObjectPtr anObject = aPrs->object();
    if (!anObject.get())
      continue;

    if (ModuleBase_Tools::getSelectedShape(aPrs).ShapeType() != TopAbs_FACE)
      continue;

    myItems.insert(myLastItemIndex, aPrs);
    myListView->addItem(generateName(aPrs), myLastItemIndex);
    isModified = hideFace(myLastItemIndex) || isModified;

    myLastItemIndex++;
  }

  if (isModified)
    XGUI_Tools::workshop(myWorkshop)->displayer()->updateViewer();
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
  XGUI_Displayer* aDisplayer = XGUI_Tools::workshop(myWorkshop)->displayer();
  for (std::set<int>::const_iterator anIt = aSelectedIds.begin(); anIt != aSelectedIds.end();
       anIt++) {
    ModuleBase_ViewerPrsPtr aPrs = myItems[*anIt];
    if (aRestoredObjects.find(aPrs->object()) == aRestoredObjects.end())
      aRestoredObjects.insert(aPrs->object());
    myItems.remove(*anIt);
  }
  myListView->removeSelectedItems();

  isModified = redisplayObjects(aRestoredObjects, true) || isModified;
  if (isModified)
    XGUI_Tools::workshop(myWorkshop)->displayer()->updateViewer();

  // Restore selection
  myListView->restoreSelection(anIndices);
  //appendSelectionInHistory();
  return true;
}

//********************************************************************
bool XGUI_FacesPanel::redisplayObjects(
  const std::set<std::shared_ptr<ModelAPI_Object> >& theObjects,
  const bool isToFlushRedisplay)
{
  bool isModified = false;
  XGUI_Displayer* aDisplayer = XGUI_Tools::workshop(myWorkshop)->displayer();
  static Events_ID aDispEvent = Events_Loop::loop()->eventByName(EVENT_OBJECT_TO_REDISPLAY);

  for (std::set<ObjectPtr>::const_iterator anIt = theObjects.begin(); anIt != theObjects.end();
       anIt++)
  {
    ObjectPtr anObject = *anIt;
    if (!anObject->isDisplayed()) {
      // if the object was hidden by this panel
      if (myHiddenObjects.find(anObject) != myHiddenObjects.end())
        myHiddenObjects.erase(anObject);
      anObject->setDisplayed(true); // it means that the object is hidden by hide all faces
      ModelAPI_EventCreator::get()->sendUpdated(anObject, aDispEvent);
      isModified = true;
      //isModified = aDisplayer->display(anObject, false) || isModified;
    }
    else {
      ModelAPI_EventCreator::get()->sendUpdated(anObject, aDispEvent);
      isModified = true;
    }
  }
  if (isToFlushRedisplay)
    Events_Loop::loop()->flush(aDispEvent);
  return isModified;
}

//********************************************************************
bool XGUI_FacesPanel::hideFace(const int theIndex)
{
  XGUI_Displayer* aDisplayer = XGUI_Tools::workshop(myWorkshop)->displayer();

  if (!myItems.contains(theIndex))
    return false;

  ModuleBase_ViewerPrsPtr aPrs = myItems[theIndex];

  AISObjectPtr aAISObj = aDisplayer->getAISObject(aPrs->object());
  if (aAISObj.get() == NULL)
    return false;
  Handle(ModuleBase_ResultPrs) aResultPrs = Handle(ModuleBase_ResultPrs)::DownCast(
    aAISObj->impl<Handle(AIS_InteractiveObject)>());
  if (aResultPrs.IsNull())
    return false;
  // set shape hidden to check whether the presentation should be erased from the viewer
  bool isModified = false;
  if (aResultPrs->hasSubShapeVisible(ModuleBase_Tools::getSelectedShape(aPrs)))
    isModified = aDisplayer->redisplay(aPrs->object(), false) || isModified;
  else
  {
    ObjectPtr anObject = aPrs->object();
    myHiddenObjects.insert(anObject);
    static Events_ID aDispEvent = Events_Loop::loop()->eventByName(EVENT_OBJECT_TO_REDISPLAY);
    anObject->setDisplayed(false);
    isModified = aDisplayer->erase(anObject, false) || isModified;
    ModelAPI_EventCreator::get()->sendUpdated(anObject, aDispEvent);
    Events_Loop::loop()->flush(aDispEvent);
  }
  return isModified;
}

//********************************************************************
void XGUI_FacesPanel::closeEvent(QCloseEvent* theEvent)
{
  QDockWidget::closeEvent(theEvent);
  emit closed();
}

//********************************************************************
void XGUI_FacesPanel::activateSelection(bool toActivate)
{
  QIntList aShapeTypes;
  aShapeTypes.append(TopAbs_FACE);

  if (toActivate) {
    myWorkshop->activateSubShapesSelection(aShapeTypes);
  } else {
    myWorkshop->deactivateSubShapesSelection();
  }
  if (toActivate)
    activateSelectionFilters();
  else
    deactivateSelectionFilters();
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
bool XGUI_FacesPanel::customizeObject(const ObjectPtr& theObject, const bool isDisplayed)
{
  if (isDisplayed && myItems.isEmpty())
    return false;

  XGUI_Displayer* aDisplayer = XGUI_Tools::workshop(myWorkshop)->displayer();

  AISObjectPtr aAISObj = aDisplayer->getAISObject(theObject);
  if (aAISObj.get() == NULL)
    return false;
  Handle(ModuleBase_ResultPrs) aResultPrs = Handle(ModuleBase_ResultPrs)::DownCast(
    aAISObj->impl<Handle(AIS_InteractiveObject)>());
  if (aResultPrs.IsNull())
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
  isModified = aResultPrs->setSubShapeHidden(aHiddenSubShapes);
  double aTransparency = !useTransparency() ? 1
    : Config_PropManager::real("Visualization", "hidden_face_transparency");
  aResultPrs->setHiddenSubShapeTransparency(aTransparency);

  return isModified;
}

//********************************************************************
void XGUI_FacesPanel::onDeleteItem()
{
  processDelete();
}

//********************************************************************
void XGUI_FacesPanel::onClosed()
{
  reset(true);
}
