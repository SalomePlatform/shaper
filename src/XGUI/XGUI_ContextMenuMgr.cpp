// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

#include "XGUI_ContextMenuMgr.h"
#include "XGUI_Workshop.h"
#include "XGUI_ObjectsBrowser.h"
#include "XGUI_SelectionMgr.h"
#include "XGUI_Displayer.h"
#include "XGUI_ViewerProxy.h"
#include "XGUI_Selection.h"
#include "XGUI_SalomeConnector.h"
#include "XGUI_DataModel.h"

#include <AppElements_MainWindow.h>

//#include "PartSetPlugin_Part.h"

#include <ModelAPI_Data.h>
#include <ModelAPI_AttributeDocRef.h>
#include <ModelAPI_Object.h>
#include <ModelAPI_Session.h>
#include <ModelAPI_ResultGroup.h>
#include <ModelAPI_ResultParameter.h>
#include <ModelAPI_ResultConstruction.h>
#include <ModelAPI_ResultBody.h>
#include <ModelAPI_Tools.h>

#include <ModuleBase_IModule.h>
#include <ModuleBase_Tools.h>

#include <QAction>
#include <QContextMenuEvent>
#include <QMenu>
#include <QMdiArea>


XGUI_ContextMenuMgr::XGUI_ContextMenuMgr(XGUI_Workshop* theParent)
    : QObject(theParent),
      myWorkshop(theParent),
      mySeparator(0)
{
}

XGUI_ContextMenuMgr::~XGUI_ContextMenuMgr()
{
}

void XGUI_ContextMenuMgr::createActions()
{
  QAction* aAction = new QAction(QIcon(":pictures/delete.png"), tr("Delete"), this);
  QMainWindow* aDesktop = myWorkshop->mainWindow();
  if (!aDesktop)
    aDesktop = myWorkshop->salomeConnector()->desktop();
  aDesktop->addAction(aAction);

  addAction("DELETE_CMD", aAction);
  aAction->setShortcut(Qt::Key_Delete);
  aAction->setShortcutContext(Qt::ApplicationShortcut);

  aAction = new QAction(QIcon(":pictures/rename_edit.png"), tr("Rename"), this);
  addAction("RENAME_CMD", aAction);
  connect(aAction, SIGNAL(triggered(bool)), this, SLOT(onRename()));

  aAction = new QAction(QIcon(":pictures/move.png"), tr("Move down"), this);
  addAction("MOVE_CMD", aAction);

  aAction = new QAction(QIcon(":pictures/color.png"), tr("Color..."), this);
  addAction("COLOR_CMD", aAction);

  aAction = new QAction(QIcon(":pictures/eye_pencil.png"), tr("Show"), this);
  addAction("SHOW_CMD", aAction);

  aAction = new QAction(QIcon(":pictures/eye_pencil.png"), tr("Show only"), this);
  addAction("SHOW_ONLY_CMD", aAction);

  aAction = new QAction(QIcon(":pictures/eye_pencil_closed.png"), tr("Hide"), this);
  addAction("HIDE_CMD", aAction);

  aAction = new QAction(QIcon(":pictures/eye_pencil_closed.png"), tr("Hide all"), this);
  addAction("HIDEALL_CMD", aAction);

  aAction = new QAction(QIcon(":pictures/shading.png"), tr("Shading"), this);
  addAction("SHADING_CMD", aAction);

  aAction = new QAction(QIcon(":pictures/wireframe.png"), tr("Wireframe"), this);
  addAction("WIREFRAME_CMD", aAction);

  mySeparator = new QAction(this);
  mySeparator->setSeparator(true);


  buildObjBrowserMenu();
  buildViewerMenu();
}

void XGUI_ContextMenuMgr::addAction(const QString& theId, QAction* theAction)
{
  if (myActions.contains(theId))
    qCritical("A command with Id = '%s' already defined!", qPrintable(theId));
  theAction->setData(theId);
  connect(theAction, SIGNAL(triggered(bool)), this, SLOT(onAction(bool)));
  myActions[theId] = theAction;
}

QAction* XGUI_ContextMenuMgr::action(const QString& theId) const
{
  if (myActions.contains(theId))
    return myActions[theId];
  return 0;
}

QAction* XGUI_ContextMenuMgr::actionByName(const QString& theName) const
{
  foreach(QAction* eachAction, myActions) {
    if (eachAction->text() == theName) {
      return eachAction;
    }
  }
  return NULL;
}

QStringList XGUI_ContextMenuMgr::actionIds() const
{
  return myActions.keys();
}

void XGUI_ContextMenuMgr::onAction(bool isChecked)
{
  QAction* aAction = static_cast<QAction*>(sender());
  emit actionTriggered(aAction->data().toString(), isChecked);
}

void XGUI_ContextMenuMgr::updateCommandsStatus()
{
}

void XGUI_ContextMenuMgr::onContextMenuRequest(QContextMenuEvent* theEvent)
{
  QMenu* aMenu = new QMenu();
  if (sender() == myWorkshop->objectBrowser()) {
    updateObjectBrowserMenu();
    addObjBrowserMenu(aMenu);
  } else if (sender() == myWorkshop->viewer()) {
    updateViewerMenu();
    addViewerMenu(aMenu);
  }

  if (aMenu && (aMenu->actions().size() > 0)) {
    // it is possible that some objects should do something before and after the popup menu exec
    // e.g. a sketch manager changes an internal flag on this signals in order to do not hide
    // a created entity
    emit beforeContextMenu();
    aMenu->exec(theEvent->globalPos());
    emit afterContextMenu();
    delete aMenu;
  }
}

void XGUI_ContextMenuMgr::updateObjectBrowserMenu() 
{
  foreach(QAction* aAction, myActions)
    aAction->setEnabled(false);

  XGUI_SelectionMgr* aSelMgr = myWorkshop->selector();
  QObjectPtrList aObjects = aSelMgr->selection()->selectedObjects();
  int aSelected = aObjects.size();
  if (aSelected > 0) {
    SessionPtr aMgr = ModelAPI_Session::get();
    XGUI_Displayer* aDisplayer = myWorkshop->displayer();
    bool hasResult = false;
    bool hasFeature = false;
    bool hasParameter = false;
    bool hasSubFeature = false;
    ModuleBase_Tools::checkObjects(aObjects, hasResult, hasFeature, hasParameter, hasSubFeature);

    //Process Feature
    if (aSelected == 1) {
      ObjectPtr aObject = aObjects.first();
      if (aObject) {
        if (hasResult && myWorkshop->canBeShaded(aObject)) {
          action("WIREFRAME_CMD")->setEnabled(true);
          action("SHADING_CMD")->setEnabled(true);
        }
        if (!hasFeature) {
          bool aHasSubResults = ModelAPI_Tools::hasSubResults(
                                            std::dynamic_pointer_cast<ModelAPI_Result>(aObject));
          if (aHasSubResults) {
            action("HIDE_CMD")->setEnabled(true);
            action("SHOW_CMD")->setEnabled(true);
          }
          else {
            if (aObject->isDisplayed()) {
              action("HIDE_CMD")->setEnabled(true);
            } else if (hasResult && (!hasParameter)) {
              action("SHOW_CMD")->setEnabled(true);
            }
          }
          if (!(hasParameter || hasFeature))
            action("SHOW_ONLY_CMD")->setEnabled(true);
        }
        else if (hasFeature && myWorkshop->canMoveFeature())
          action("MOVE_CMD")->setEnabled(true);

        if( aMgr->activeDocument() == aObject->document() )
        {
          action("RENAME_CMD")->setEnabled(true);
          action("DELETE_CMD")->setEnabled(true);
        }
      }
    } else {
      // parameter is commented because the actions are not in the list of result parameter actions
      if (hasResult /*&& (!hasParameter)*/) {
        action("SHOW_CMD")->setEnabled(true);
        action("HIDE_CMD")->setEnabled(true);
        action("SHOW_ONLY_CMD")->setEnabled(true);
        action("SHADING_CMD")->setEnabled(true);
        action("WIREFRAME_CMD")->setEnabled(true);
      }
    }
    bool allActive = true;
    foreach( ObjectPtr aObject, aObjects )
      if( aMgr->activeDocument() != aObject->document() )  {
        allActive = false;
        break;
      }
    if (!hasSubFeature && allActive ) {
      if (hasFeature || hasParameter)
        action("DELETE_CMD")->setEnabled(true);
    }
  }

  // Show/Hide command has to be disabled for objects from non active document
  bool aDeactivate = false;
  foreach (ObjectPtr aObj, aObjects) {
    if (!aObj->document()->isActive()) {
      if ((aObj->document() != ModelAPI_Session::get()->moduleDocument()) ||
           aObj->groupName() == ModelAPI_ResultPart::group()) {
        aDeactivate = true;
        break;
      }
    }
  }
  if (aDeactivate) {
    // If at leas a one objec can not be edited then Show/Hide has to be disabled
    action("SHOW_CMD")->setEnabled(false);
    action("HIDE_CMD")->setEnabled(false);
    action("SHOW_ONLY_CMD")->setEnabled(false);
  }

  if (myWorkshop->canChangeColor())
    action("COLOR_CMD")->setEnabled(true);

  ModuleBase_IModule* aModule = myWorkshop->module();
  if (aModule)
    aModule->updateObjectBrowserMenu(myActions);
}

void XGUI_ContextMenuMgr::updateViewerMenu()
{
  foreach(QAction* aAction, myActions)
    aAction->setEnabled(false);

  XGUI_SelectionMgr* aSelMgr = myWorkshop->selector();
  QList<ModuleBase_ViewerPrs> aPrsList = aSelMgr->selection()->getSelected(ModuleBase_ISelection::Viewer);
  if (aPrsList.size() > 0) {
    bool isVisible = false;
    bool isShading = false;
    bool canBeShaded = false;
    ObjectPtr aObject;
    foreach(ModuleBase_ViewerPrs aPrs, aPrsList) {
      aObject = aPrs.object();
      ResultPtr aRes = std::dynamic_pointer_cast<ModelAPI_Result>(aObject);
      if (aRes && aRes->isDisplayed()) {
        isVisible = true;
        canBeShaded = myWorkshop->displayer()->canBeShaded(aObject);
        isShading = (myWorkshop->displayer()->displayMode(aObject) == XGUI_Displayer::Shading);      
        break;
      }
    }
    if (isVisible) {
      if (canBeShaded) {
        action("WIREFRAME_CMD")->setEnabled(true);
        action("SHADING_CMD")->setEnabled(true);
      }
      action("SHOW_ONLY_CMD")->setEnabled(true);
      action("HIDE_CMD")->setEnabled(true);
    } else
      action("SHOW_CMD")->setEnabled(true);
  }
  if (myWorkshop->displayer()->objectsCount() > 0)
    action("HIDEALL_CMD")->setEnabled(true);
  if (myWorkshop->canChangeColor())
    action("COLOR_CMD")->setEnabled(true);

  action("DELETE_CMD")->setEnabled(true);

  ModuleBase_IModule* aModule = myWorkshop->module();
  if (aModule)
    aModule->updateViewerMenu(myActions);
}

void XGUI_ContextMenuMgr::connectObjectBrowser()
{
  connect(myWorkshop->objectBrowser(), SIGNAL(contextMenuRequested(QContextMenuEvent*)), this,
          SLOT(onContextMenuRequest(QContextMenuEvent*)));
}

void XGUI_ContextMenuMgr::connectViewer()
{
  connect(myWorkshop->viewer(), SIGNAL(contextMenuRequested(QContextMenuEvent*)), this,
          SLOT(onContextMenuRequest(QContextMenuEvent*)));
}


void XGUI_ContextMenuMgr::buildObjBrowserMenu()
{
  QAction* aSeparator = new QAction(this);
  aSeparator->setSeparator(true);

  QActionsList aList;
  
  // Result construction menu
  aList.append(action("SHOW_CMD"));
  aList.append(action("HIDE_CMD"));
  aList.append(action("SHOW_ONLY_CMD"));
  aList.append(action("COLOR_CMD"));
  aList.append(mySeparator);
  aList.append(action("RENAME_CMD"));
  myObjBrowserMenus[ModelAPI_ResultConstruction::group()] = aList;
  //-------------------------------------
  // Result body menu
  aList.clear();
  aList.append(action("WIREFRAME_CMD"));
  aList.append(action("SHADING_CMD"));
  aList.append(action("COLOR_CMD"));
  aList.append(mySeparator);
  aList.append(action("SHOW_CMD"));
  aList.append(action("HIDE_CMD"));
  aList.append(action("SHOW_ONLY_CMD"));
  aList.append(mySeparator);
  aList.append(action("RENAME_CMD"));
  myObjBrowserMenus[ModelAPI_ResultBody::group()] = aList;
  // Group menu
  myObjBrowserMenus[ModelAPI_ResultGroup::group()] = aList;
  // Result part menu
  myObjBrowserMenus[ModelAPI_ResultPart::group()] = aList;
  //-------------------------------------
  // Feature menu
  aList.clear();
  aList.append(action("DELETE_CMD"));
  aList.append(action("MOVE_CMD"));
  aList.append(mySeparator);
  aList.append(action("RENAME_CMD"));
  myObjBrowserMenus[ModelAPI_Feature::group()] = aList;

  aList.clear();
  aList.append(action("DELETE_CMD"));
  aList.append(mySeparator);
  aList.append(action("RENAME_CMD"));
  myObjBrowserMenus[ModelAPI_ResultParameter::group()] = aList;
  //-------------------------------------
}

void XGUI_ContextMenuMgr::buildViewerMenu()
{
  QActionsList aList;
  // Result construction menu
  aList.append(action("HIDE_CMD"));
  aList.append(action("SHOW_ONLY_CMD"));
  aList.append(action("HIDEALL_CMD"));
  aList.append(action("COLOR_CMD"));
  myViewerMenu[ModelAPI_ResultConstruction::group()] = aList;
  // Result part menu
  myViewerMenu[ModelAPI_ResultPart::group()] = aList;
  //-------------------------------------
  // Result body menu
  aList.clear();
  aList.append(action("WIREFRAME_CMD"));
  aList.append(action("SHADING_CMD"));
  aList.append(action("COLOR_CMD"));
  aList.append(mySeparator);
  aList.append(action("HIDE_CMD"));
  aList.append(action("SHOW_ONLY_CMD"));
  aList.append(action("HIDEALL_CMD"));
  myViewerMenu[ModelAPI_ResultBody::group()] = aList;
  // Group menu
  myViewerMenu[ModelAPI_ResultGroup::group()] = aList;
  //-------------------------------------

}


void XGUI_ContextMenuMgr::addObjBrowserMenu(QMenu* theMenu) const
{
  XGUI_SelectionMgr* aSelMgr = myWorkshop->selector();
  QObjectPtrList aObjects = aSelMgr->selection()->selectedObjects();
  int aSelected = aObjects.size();
  QActionsList aActions;
  if (aSelected == 1) {
    ObjectPtr aObject = aObjects.first();
    std::string aName = aObject->groupName();
    if (myObjBrowserMenus.contains(aName))
      aActions = myObjBrowserMenus[aName];
  } else if (aSelected > 1) {
      aActions.append(action("SHADING_CMD"));
      aActions.append(action("WIREFRAME_CMD"));
      aActions.append(mySeparator);
      aActions.append(action("SHOW_CMD"));
      aActions.append(action("HIDE_CMD"));
      aActions.append(action("SHOW_ONLY_CMD"));
      aActions.append(mySeparator);
      aActions.append(action("DELETE_CMD"));
      //aActions.append(action("MOVE_CMD"));
  }
  theMenu->addActions(aActions);

  ModuleBase_IModule* aModule = myWorkshop->module();
  if (aModule) {
    theMenu->addSeparator();
    aModule->addObjectBrowserMenu(theMenu);
  }
  theMenu->addSeparator();
  theMenu->addActions(myWorkshop->objectBrowser()->actions());
}

void XGUI_ContextMenuMgr::addViewerMenu(QMenu* theMenu) const
{
  ModuleBase_IModule* aModule = myWorkshop->module();
  if (aModule) {
    if (aModule->addViewerMenu(theMenu, myActions))
      theMenu->addSeparator();
  }
  XGUI_SelectionMgr* aSelMgr = myWorkshop->selector();
  QList<ModuleBase_ViewerPrs> aPrsList = aSelMgr->selection()->getSelected(ModuleBase_ISelection::Viewer);
  int aSelected = aPrsList.size();
  QActionsList aActions;
  if (aSelected == 1) {
    ObjectPtr aObject = aPrsList.first().object();
    std::string aName = aObject->groupName();
    if (myViewerMenu.contains(aName))
      aActions = myViewerMenu[aName];
  } else if (aSelected > 1) {
    aActions.append(action("HIDE_CMD"));
  }
  theMenu->addActions(aActions);

  if (!myWorkshop->isSalomeMode()) {
    theMenu->addSeparator();
    QMdiArea* aMDI = myWorkshop->mainWindow()->mdiArea();
    if (aMDI->actions().size() > 0) {
      QMenu* aSubMenu = theMenu->addMenu(tr("Windows"));
      aSubMenu->addActions(aMDI->actions());
    }
  }
}

QStringList XGUI_ContextMenuMgr::actionObjectGroups(const QString& theName)
{
  QStringList aGroups;

  QMap<std::string, QActionsList>::const_iterator anIt = myObjBrowserMenus.begin(),
                                                  aLast = myObjBrowserMenus.end();
  for (; anIt != aLast; anIt++) {
    QString aGroupName(anIt.key().c_str());
    if (aGroups.contains(aGroupName))
      continue;
    QActionsList anActions = anIt.value();
    QActionsList::const_iterator anAIt = anActions.begin(), anALast = anActions.end();
    bool aFound = false;
    for (; anAIt != anALast && !aFound; anAIt++)
      aFound = (*anAIt)->data().toString() == theName;
    if (aFound)
      aGroups.append(aGroupName);
  }
  return aGroups;
}

void XGUI_ContextMenuMgr::onRename()
{
  myWorkshop->objectBrowser()->onEditItem();
}
