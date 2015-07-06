// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

#include "XGUI_ContextMenuMgr.h"
#include "XGUI_Workshop.h"
#include "XGUI_ObjectsBrowser.h"
#include "XGUI_SelectionMgr.h"
#include "XGUI_Displayer.h"
#include "XGUI_ViewerProxy.h"
#include "XGUI_Selection.h"
#include "XGUI_SalomeConnector.h"

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
  QMenu* aMenu = 0;
  if (sender() == myWorkshop->objectBrowser()) {
    updateObjectBrowserMenu();
    aMenu = objBrowserMenu();
  } else if (sender() == myWorkshop->viewer()) {
    updateViewerMenu();
    aMenu = viewerMenu();
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
    ModuleBase_Tools::checkObjects(aObjects, hasResult, hasFeature, hasParameter);

    //Process Feature
    if (aSelected == 1) {
      ObjectPtr aObject = aObjects.first();
      if (aObject) {
        if (!hasFeature) {
          if (aObject->isDisplayed()) {
            if (aDisplayer->canBeShaded(aObject)) {
              action("WIREFRAME_CMD")->setEnabled(true);
              action("SHADING_CMD")->setEnabled(true);
            }
            action("HIDE_CMD")->setEnabled(true);
          } else if (hasResult && (!hasParameter)) {
            action("SHOW_CMD")->setEnabled(true);
          }

          ResultPartPtr aPartRes = std::dynamic_pointer_cast<ModelAPI_ResultPart>(aObject);
          if (aPartRes) {
            action("SHOW_CMD")->setEnabled(true);
          }

          if (!(hasParameter || hasFeature))
            action("SHOW_ONLY_CMD")->setEnabled(true);
        }
      } 
    } else {
      if (hasResult && (!hasParameter)) {
        action("SHOW_CMD")->setEnabled(true);
        action("HIDE_CMD")->setEnabled(true);
        action("SHOW_ONLY_CMD")->setEnabled(true);
        action("SHADING_CMD")->setEnabled(true);
        action("WIREFRAME_CMD")->setEnabled(true);
      }
    }
    if (hasFeature || hasParameter)
      action("DELETE_CMD")->setEnabled(true);
  }
  if (myWorkshop->canChangeColor())
    action("COLOR_CMD")->setEnabled(true);
}

void XGUI_ContextMenuMgr::updateViewerMenu()
{
  foreach(QAction* aAction, myActions)
    aAction->setEnabled(false);

  XGUI_SelectionMgr* aSelMgr = myWorkshop->selector();
  QObjectPtrList aObjects = aSelMgr->selection()->selectedObjects();
  if (aObjects.size() > 0) {
    bool isVisible = false;
    bool isShading = false;
    bool canBeShaded = false;
    foreach(ObjectPtr aObject, aObjects)
    {
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
  myObjBrowserMenus[ModelAPI_ResultConstruction::group()] = aList;
  // Result part menu
  myObjBrowserMenus[ModelAPI_ResultPart::group()] = aList;
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
  myObjBrowserMenus[ModelAPI_ResultBody::group()] = aList;
  // Group menu
  myObjBrowserMenus[ModelAPI_ResultGroup::group()] = aList;
  //-------------------------------------
  // Feature menu
  aList.clear();
  aList.append(action("DELETE_CMD"));
  myObjBrowserMenus[ModelAPI_Feature::group()] = aList;
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


QMenu* XGUI_ContextMenuMgr::objBrowserMenu() const
{
  QMenu* aMenu = new QMenu();
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
  }
  aMenu->addActions(aActions);

  ModuleBase_IModule* aModule = myWorkshop->module();
  if (aModule) {
    aMenu->addSeparator();
    aModule->addObjectBrowserMenu(aMenu);
  }
  aMenu->addSeparator();
  aMenu->addActions(myWorkshop->objectBrowser()->actions());

  return aMenu;
}

QMenu* XGUI_ContextMenuMgr::viewerMenu() const
{
  QMenu* aMenu = new QMenu();
  ModuleBase_IModule* aModule = myWorkshop->module();
  if (aModule) 
    aModule->addViewerMenu(aMenu, myActions);

  XGUI_SelectionMgr* aSelMgr = myWorkshop->selector();
  QObjectPtrList aObjects = aSelMgr->selection()->selectedObjects();
  int aSelected = aObjects.size();
  QActionsList aActions;
  if (aSelected == 1) {
    ObjectPtr aObject = aObjects.first();
    std::string aName = aObject->groupName();
    if (myViewerMenu.contains(aName))
      aActions = myViewerMenu[aName];
  } else if (aSelected > 1) {
    aActions.append(action("HIDE_CMD"));
  }
  aMenu->addActions(aActions);

  if (!myWorkshop->isSalomeMode()) {
    aMenu->addSeparator();
    QMdiArea* aMDI = myWorkshop->mainWindow()->mdiArea();
    if (aMDI->actions().size() > 0) {
      QMenu* aSubMenu = aMenu->addMenu(tr("Windows"));
      aSubMenu->addActions(aMDI->actions());
    }
  }
  return aMenu;
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
