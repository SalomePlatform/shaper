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

#include <ModuleBase_IModule.h>
#include <ModuleBase_Tools.h>

#include <QAction>
#include <QContextMenuEvent>
#include <QMenu>
#include <QMdiArea>

XGUI_ContextMenuMgr::XGUI_ContextMenuMgr(XGUI_Workshop* theParent)
    : QObject(theParent),
      myWorkshop(theParent)
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

  aAction = new QAction(QIcon(":pictures/color.png"), tr("Color"), this);
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
  if (sender() == myWorkshop->objectBrowser())
    aMenu = objectBrowserMenu();
  else if (sender() == myWorkshop->viewer()) {
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

QMenu* XGUI_ContextMenuMgr::objectBrowserMenu() const
{
  QMenu* aMenu = new QMenu();
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
              if (aDisplayer->displayMode(aObject) == XGUI_Displayer::Shading)
                aMenu->addAction(action("WIREFRAME_CMD"));
              else
                aMenu->addAction(action("SHADING_CMD"));
            }
            aMenu->addSeparator();
            aMenu->addAction(action("HIDE_CMD"));
          } else if (hasResult && (!hasParameter)) {
            aMenu->addAction(action("SHOW_CMD"));
          }
          if (!(hasParameter || hasFeature))
            aMenu->addAction(action("SHOW_ONLY_CMD"));
        }
      } 
    } else {
      if (hasResult && (!hasParameter)) {
        aMenu->addAction(action("SHOW_CMD"));
        aMenu->addAction(action("HIDE_CMD"));
        aMenu->addAction(action("SHOW_ONLY_CMD"));
        aMenu->addSeparator();
        aMenu->addAction(action("SHADING_CMD"));
        aMenu->addAction(action("WIREFRAME_CMD"));
      }
    }
    if (hasFeature || hasParameter)
      aMenu->addAction(action("DELETE_CMD"));
  }
  if (myWorkshop->canChangeColor())
    aMenu->addAction(action("COLOR_CMD"));

  aMenu->addSeparator();
  ModuleBase_IModule* aModule = myWorkshop->module();
  if (aModule) {
    aModule->addObjectBrowserMenu(aMenu);
    aMenu->addSeparator();
  }
  aMenu->addActions(myWorkshop->objectBrowser()->actions());


  if (aMenu->actions().size() > 0) {
    return aMenu;
  }
  delete aMenu;
  return 0;
}

QMenu* XGUI_ContextMenuMgr::viewerMenu() const
{
  QMenu* aMenu = new QMenu();
  addViewerMenu(aMenu);
  if (aMenu->actions().size() > 0) {
    return aMenu;
  }
  delete aMenu;
  return 0;
}

void XGUI_ContextMenuMgr::addViewerMenu(QMenu* theMenu) const
{
  bool aIsDone = false;
  ModuleBase_IModule* aModule = myWorkshop->module();
  if (aModule) 
    aIsDone = aModule->addViewerMenu(theMenu, myActions);

  if (!aIsDone) {
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
          if (isShading)
            theMenu->addAction(action("WIREFRAME_CMD"));
          else
            theMenu->addAction(action("SHADING_CMD"));
        }
        theMenu->addSeparator();
        theMenu->addAction(action("SHOW_ONLY_CMD"));
        theMenu->addAction(action("HIDE_CMD"));
      } else
        theMenu->addAction(action("SHOW_CMD"));
    }
    if (myWorkshop->displayer()->objectsCount() > 0)
      theMenu->addAction(action("HIDEALL_CMD"));
    if (myWorkshop->canChangeColor())
      theMenu->addAction(action("COLOR_CMD"));
  }
  if (!myWorkshop->isSalomeMode()) {
    theMenu->addSeparator();
    QMdiArea* aMDI = myWorkshop->mainWindow()->mdiArea();
    if (aMDI->actions().size() > 0) {
      QMenu* aSubMenu = theMenu->addMenu(tr("Windows"));
      aSubMenu->addActions(aMDI->actions());
    }
  }

}

void XGUI_ContextMenuMgr::connectObjectBrowser() const
{
  connect(myWorkshop->objectBrowser(), SIGNAL(contextMenuRequested(QContextMenuEvent*)), this,
          SLOT(onContextMenuRequest(QContextMenuEvent*)));
}

void XGUI_ContextMenuMgr::connectViewer() const
{
  connect(myWorkshop->viewer(), SIGNAL(contextMenuRequested(QContextMenuEvent*)), this,
          SLOT(onContextMenuRequest(QContextMenuEvent*)));
}

