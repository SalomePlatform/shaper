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

#include "SHAPERGUI.h"
#include "SHAPERGUI_DataModel.h"
#include "SHAPERGUI_OCCSelector.h"
#include <SHAPERGUI_NestedButton.h>

#include <XGUI_Workshop.h>
#include <XGUI_PropertyPanel.h>
#include <XGUI_ContextMenuMgr.h>
#include <XGUI_ObjectsBrowser.h>
#include <XGUI_OperationMgr.h>
#include <XGUI_Displayer.h>
#include <XGUI_MenuMgr.h>
#include <XGUI_FacesPanel.h>
#include <XGUI_SelectionActivate.h>
#include <XGUI_InspectionPanel.h>

#include <ModuleBase_Operation.h>
#include <ModuleBase_Preferences.h>
#include <ModuleBase_ActionInfo.h>
#include <ModuleBase_IModule.h>

#include <LightApp_Application.h>
#include <LightApp_SelectionMgr.h>
#include <LightApp_OCCSelector.h>
#include <LightApp_Study.h>
#include <OCCViewer_ViewModel.h>

#include <SUIT_Selector.h>
#include <SUIT_Desktop.h>
#include <SUIT_ViewManager.h>
#include <SUIT_ResourceMgr.h>
#include <SUIT_DataBrowser.h>

#include <QtxPopupMgr.h>
#include <QtxActionMenuMgr.h>
#include <QtxActionToolMgr.h>
#include <QtxResourceMgr.h>

#include <Config_PropManager.h>
#include <Config_ModuleReader.h>

#include <AIS_ListOfInteractive.hxx>
#include <AIS_ListIteratorOfListOfInteractive.hxx>

#include <QDockWidget>
#include <QAction>
#include <QTimer>
#include <QMenu>

#define SALOME_PATCH_FOR_CTRL_WHEEL

extern "C" {
SHAPERGUI_EXPORT CAM_Module* createModule()
{
  return new SHAPERGUI();
}

SHAPERGUI_EXPORT char* getModuleVersion()
{
  return (char*)"0.0";
}
} // extern "C"

/** 
* Class for preferences management
*/
class SHAPERGUI_PrefMgr: public ModuleBase_IPrefMgr
{
public:
  /// Constructor
  /// \param theMgr preferences manager of SALOME
  /// \param theModName name of the module
  SHAPERGUI_PrefMgr(LightApp_Preferences* theMgr, const QString& theModName):
    myMgr(theMgr), myModName(theModName) {}

  virtual int addPreference(const QString& theLbl, int pId,
                            SUIT_PreferenceMgr::PrefItemType theType,
                            const QString& theSection, const QString& theName )
  {
    return myMgr->addPreference(myModName, theLbl, pId, theType, theSection, theName);
  }

  virtual void setItemProperty(const QString& thePropName,
                               const QVariant& theValue,
                               const int theId = -1)
  {
    myMgr->setItemProperty(thePropName, theValue, theId);
  }


  virtual SUIT_PreferenceMgr* prefMgr() const { return myMgr; }

private:
  LightApp_Preferences* myMgr;
  QString myModName;
};




//******************************************************
SHAPERGUI::SHAPERGUI()
    : LightApp_Module("SHAPER"),
      mySelector(0), myIsOpened(0), myPopupMgr(0)
{
  myWorkshop = new XGUI_Workshop(this);
  connect(myWorkshop, SIGNAL(commandStatusUpdated()),
          this, SLOT(onUpdateCommandStatus()));

  myProxyViewer = new SHAPERGUI_SalomeViewer(this);

  ModuleBase_Preferences::setResourceMgr(application()->resourceMgr());
  ModuleBase_Preferences::loadCustomProps();
}

//******************************************************
SHAPERGUI::~SHAPERGUI()
{
}

//******************************************************
void SHAPERGUI::initialize(CAM_Application* theApp)
{
  LightApp_Module::initialize(theApp);

  myWorkshop->startApplication();
  LightApp_Application* anApp = dynamic_cast<LightApp_Application*>(theApp);
  if (anApp)
  {
    connect(anApp, SIGNAL(preferenceResetToDefaults()), this, SLOT(onDefaultPreferences()));
  }

  int aMenu = createMenu(tr("Inspection"), -1, -1, 30);
  int aSubMenu = createMenu(tr("Information"), aMenu);

  int aId = myActionsList.size();
  myActionsList.append("INSPECTION_CMD");
  SUIT_Desktop* aDesk = application()->desktop();
  QString aTip = tr("Show inspection window");
  myWhatIsAction = createAction(aId, aTip, QIcon(":pictures/whatis.png"), tr("What Is"),
    aTip, QKeySequence(), aDesk, true, this, SLOT(onWhatIs(bool)));
  myWhatIsAction->setStatusTip(aTip);
  myWhatIsAction->setData("INSPECTION_CMD");
  createMenu(aId, aSubMenu, 0);

  int aTool = createTool(tr("Inspection tool"));
  int aToolId = createTool(myWhatIsAction, aTool);
}

//******************************************************
void SHAPERGUI::windows(QMap<int, int>& theWndMap) const
{
  theWndMap.insert(LightApp_Application::WT_PyConsole, Qt::BottomDockWidgetArea);
}

//******************************************************
void SHAPERGUI::viewManagers(QStringList& theList) const
{
  theList.append(OCCViewer_Viewer::Type());
}

//******************************************************
// We can not create selector in this method because it can be called when
// SHAPER module is not active. Take into account that creation of our selector
// leads to switching OFF all other selectors
//void SHAPERGUI::connectToStudy(CAM_Study* theStudy)
//{
//  // if there are created viewer managers, we should try to create viewer
//  // selector and initialize viewer with it. It sets interactive contect to the
//  // proxy viewer. If study is opened, CAM application calls this method before the open()
//  // of data model
//  // the SHAPER data model is specific and during open(load) redisplay signals are flushed, so
//  // we need to connect to the viewer before it. Here,
//  // it seems the most appropriate place for this
//  // according to SALOME architecture.
//  if (!mySelector) {
//    ViewManagerList OCCViewManagers;
//    application()->viewManagers(OCCViewer_Viewer::Type(), OCCViewManagers);
//    if (OCCViewManagers.size() > 0) {
//      mySelector = createSelector(OCCViewManagers.first());
//    }
//  }
//  LightApp_Module::connectToStudy(theStudy);
//}

//******************************************************
bool SHAPERGUI::activateModule(SUIT_Study* theStudy)
{
  bool isDone = LightApp_Module::activateModule(theStudy);
  SHAPERGUI_DataModel* aDataModel = dynamic_cast<SHAPERGUI_DataModel*>(dataModel());
  aDataModel->initRootObject();

  if (isDone) {
    setMenuShown(true);
    setToolShown(true);

    QObject* aObj = myWorkshop->objectBrowser()->parent();
    QDockWidget* aObjDoc = dynamic_cast<QDockWidget*>(aObj);
    if (aObjDoc) {
      QAction* aViewAct = aObjDoc->toggleViewAction();
      aViewAct->setEnabled(true);
      myWorkshop->objectBrowser()->setVisible(true);
      aObjDoc->setVisible(true);
      desktop()->tabifyDockWidget(aObjDoc, myWorkshop->propertyPanel());
    }

    QDockWidget* aInspection = myWorkshop->inspectionPanel();
    if (aInspection) {
      QAction* aViewAct = aInspection->toggleViewAction();
      aViewAct->setEnabled(true);
      connect(aViewAct, SIGNAL(toggled(bool)), this, SLOT(onWhatIs(bool)));
    }

    if (!mySelector) {
      ViewManagerList OCCViewManagers;
      application()->viewManagers(OCCViewer_Viewer::Type(), OCCViewManagers);
      if (OCCViewManagers.size() > 0) {
        mySelector = createSelector(OCCViewManagers.first());
      }
    }
    // it should be pefromed after the selector creation in order to have AISContext
    myWorkshop->activateModule();
    //action(myEraseAll)->setEnabled(false);

    if (myIsOpened) {
      myWorkshop->objectBrowser()->rebuildDataTree();
      myWorkshop->updateCommandStatus();
      myIsOpened = false;
    }
    else
      myWorkshop->updateCommandStatus();
  }
  SUIT_ResourceMgr* aResMgr = application()->resourceMgr();
  myIsStorePositions = aResMgr->booleanValue("Study", "store_positions", true);
  myIsEditEnabled = getApp()->isEditEnabled();
  getApp()->setEditEnabled(false);

  // this following row is caused by #187 bug.
  // SALOME saves the dock widget positions before deactivateModule() and
  // load it after the module activation. So, if the panel is visible before
  // deactivate, it becomes visible after activate.
  // In order to avoid the visible property panel, the widget position save is
  // switch off in this module
  aResMgr->setValue("Study", "store_positions", false);

  // Synchronize displayed objects
  Handle(AIS_InteractiveContext) aContext;
  if (mySelector && mySelector->viewer())
    aContext = mySelector->viewer()->getAISContext();

  if (!aContext.IsNull()) {
    XGUI_Displayer* aDisp = myWorkshop->displayer();
    QObjectPtrList aObjList = aDisp->displayedObjects();

    AIS_ListOfInteractive aList;
    aContext->DisplayedObjects(aList);
    AIS_ListIteratorOfListOfInteractive aLIt;
    Handle(AIS_InteractiveObject) anAISIO;
    foreach (ObjectPtr aObj, aObjList) {
      AISObjectPtr aPrs = aDisp->getAISObject(aObj);
      Handle(AIS_InteractiveObject) aAIS = aPrs->impl<Handle(AIS_InteractiveObject)>();
      bool aFound = false;
      for (aLIt.Initialize(aList); aLIt.More(); aLIt.Next()) {
        anAISIO = aLIt.Value();
        if (anAISIO.get() == aAIS.get()) {
          aFound = true;
          break;
        }
      }
      if (!aFound) {
        aObj->setDisplayed(false);
        //aDisp->erase(aObj, false);
      }
    }
    Events_Loop::loop()->flush(Events_Loop::eventByName(EVENT_OBJECT_TO_REDISPLAY));
  }
  myProxyViewer->activateViewer(true);

  // Postrrocessing for LoadScriptId to remove created(if it was created) SALOME Object Browser
  connect(getApp()->action(LightApp_Application::UserID+1), SIGNAL(triggered(bool)),
          this, SLOT(onScriptLoaded()));

  disconnect(getApp()->action(LightApp_Application::FileSaveId), SIGNAL(triggered(bool)),
             getApp(), SLOT(onSaveDoc()));
  disconnect(getApp()->action(LightApp_Application::FileSaveAsId), SIGNAL(triggered(bool)),
             getApp(), SLOT(onSaveAsDoc()));

  connect(getApp()->action(LightApp_Application::FileSaveId), SIGNAL(triggered(bool)),
          this, SLOT(onSaveDocByShaper()));
  connect(getApp()->action(LightApp_Application::FileSaveAsId), SIGNAL(triggered(bool)),
          this, SLOT(onSaveAsDocByShaper()));

  return isDone;
}

//******************************************************
bool SHAPERGUI::deactivateModule(SUIT_Study* theStudy)
{
  myProxyViewer->activateViewer(false);
  setMenuShown(false);
  setToolShown(false);

  myWorkshop->deactivateModule();

  QObject* aObj = myWorkshop->objectBrowser()->parent();
  QDockWidget* aObjDoc = dynamic_cast<QDockWidget*>(aObj);
  if (aObjDoc) {
    aObjDoc->setVisible(false);
    myWorkshop->objectBrowser()->setVisible(false);
    QAction* aViewAct = aObjDoc->toggleViewAction();
    aViewAct->setEnabled(false);
  }
  QDockWidget* aInspection = myWorkshop->inspectionPanel();
  if (aInspection) {
    aInspection->setVisible(false);
    QAction* aViewAct = aInspection->toggleViewAction();
    aViewAct->setEnabled(false);
  }

  // the active operation should be stopped for the next activation.
  // There should not be active operation and visualized preview.
  // Abort operation should be performed before the selection's remove
  // because the displayed objects should be removed from the viewer, but
  // the AIS context is obtained from the selector.
  ModuleBase_Operation* anOperation = myWorkshop->operationMgr()->currentOperation();
  while (anOperation) {
    anOperation->abort();
    anOperation = myWorkshop->operationMgr()->currentOperation();
  }
  // Delete selector because it has to be redefined on next activation
  if (mySelector) {
    myProxyViewer->setSelector(0);
    delete mySelector;
    mySelector = 0;
  }

  myWorkshop->hidePanel(myWorkshop->facesPanel());
  //myWorkshop->contextMenuMgr()->disconnectViewer();

  SUIT_ResourceMgr* aResMgr = application()->resourceMgr();
  aResMgr->setValue("Study", "store_positions", myIsStorePositions);
  getApp()->setEditEnabled(myIsEditEnabled);

  // Postrrocessing for LoadScriptId to remove created(if it was created) SALOME Object Browser
  disconnect(getApp()->action(LightApp_Application::UserID+1), SIGNAL(triggered(bool)),
             this, SLOT(onScriptLoaded()));

  disconnect(getApp()->action(LightApp_Application::FileSaveId), SIGNAL(triggered(bool)),
             this, SLOT(onSaveDocByShaper()));
  disconnect(getApp()->action(LightApp_Application::FileSaveAsId), SIGNAL(triggered(bool)),
             this, SLOT(onSaveAsDocByShaper()));

  connect(getApp()->action(LightApp_Application::FileSaveId), SIGNAL(triggered(bool)),
          getApp(), SLOT(onSaveDoc()));
  connect(getApp()->action(LightApp_Application::FileSaveAsId), SIGNAL(triggered(bool)),
          getApp(), SLOT(onSaveAsDoc()));


  return LightApp_Module::deactivateModule(theStudy);
}

//******************************************************
void SHAPERGUI::onViewManagerAdded(SUIT_ViewManager* theMgr)
{
  if (!mySelector) {
    mySelector = createSelector(theMgr);
    myWorkshop->selectionActivate()->updateSelectionFilters();
    myWorkshop->selectionActivate()->updateSelectionModes();
    myWorkshop->synchronizeViewer();
  }
}

//******************************************************
void SHAPERGUI::onViewManagerRemoved(SUIT_ViewManager* theMgr)
{
  if (mySelector) {
    if (theMgr->getType() == OCCViewer_Viewer::Type()) {
      OCCViewer_Viewer* aViewer = static_cast<OCCViewer_Viewer*>(theMgr->getViewModel());
      if (mySelector->viewer() == aViewer) {
        XGUI_Displayer* aDisp = myWorkshop->displayer();
        QObjectPtrList aObjects = aDisp->displayedObjects();
        foreach(ObjectPtr aObj, aObjects)
          aObj->setDisplayed(false);
        Events_Loop::loop()->flush(Events_Loop::eventByName(EVENT_OBJECT_TO_REDISPLAY));
        myProxyViewer->setSelector(0);
        delete mySelector;
        mySelector = 0;

        myWorkshop->module()->clearViewer();
      }
    }
  }
}

//******************************************************
QtxPopupMgr* SHAPERGUI::popupMgr()
{
  if (!myPopupMgr)
    myPopupMgr = new QtxPopupMgr( 0, this );
  return myPopupMgr;
}

//******************************************************
void SHAPERGUI::onDefaultPreferences()
{
  // reset main resources
  ModuleBase_Preferences::resetResourcePreferences(preferences());
  // reset plugin's resources
  ModuleBase_Preferences::resetConfigPropPreferences(preferences());

  myWorkshop->displayer()->redisplayObjects();
}

//******************************************************
void SHAPERGUI::onScriptLoaded()
{
  // this slot is called after processing of the LoadScriptId action of SalomeApp Application
  // Each dumped script contains updateObjBrowser() that creates a new instance of Object
  // Browser. When SHAPER module is active, this browser should not be used. It might be removed
  // as hidden by means of updateWindows() of SalomeApp_Application or to remove
  // it manually (because this method of application is protected)
  SUIT_DataBrowser* aBrowser = getApp()->objectBrowser();
  if (aBrowser)
    delete aBrowser;
}

//******************************************************
void SHAPERGUI::onSaveDocByShaper()
{
  if(!workshop()->operationMgr()->abortAllOperations(XGUI_OperationMgr::XGUI_InformationMessage))
    return;

  getApp()->onSaveDoc();
}

//******************************************************
void SHAPERGUI::onSaveAsDocByShaper()
{
  if(!workshop()->operationMgr()->abortAllOperations(XGUI_OperationMgr::XGUI_InformationMessage))
    return;

  getApp()->onSaveAsDoc();
}

//******************************************************
void SHAPERGUI::onUpdateCommandStatus()
{
  getApp()->updateActions();
}

//******************************************************
SHAPERGUI_OCCSelector* SHAPERGUI::createSelector(SUIT_ViewManager* theMgr)
{
  if (theMgr->getType() == OCCViewer_Viewer::Type()) {
    OCCViewer_Viewer* aViewer = static_cast<OCCViewer_Viewer*>(theMgr->getViewModel());
    SHAPERGUI_OCCSelector* aSelector = new SHAPERGUI_OCCSelector(aViewer,
                                                                 getApp()->selectionMgr());
#ifdef SALOME_PATCH_FOR_CTRL_WHEEL
    aViewer->setUseLocalSelection(true);
#endif
    LightApp_SelectionMgr* aMgr = getApp()->selectionMgr();
    QList<SUIT_Selector*> aList;
    aMgr->selectors(aList);
    foreach(SUIT_Selector* aSel, aList)
    {
      aSel->setEnabled(aSel == aSelector);
    }
    myProxyViewer->setSelector(aSelector);
    return aSelector;
  }
  return 0;
}

//******************************************************
CAM_DataModel* SHAPERGUI::createDataModel()
{
  return new SHAPERGUI_DataModel(this);
}

QAction* SHAPERGUI::addFeature(const QString& theWBName, const ActionInfo& theInfo,
                               const bool isAddSeparator)
{
  return addFeature(theWBName,
                    theInfo.toolBar,
                    theInfo.id,
                    theInfo.text,
                    //Issue #650: in the SALOME mode the tooltip should be same as text
                    theInfo.text,
                    theInfo.icon,
                    theInfo.shortcut,
                    theInfo.checkable,
                    isAddSeparator,
                    theInfo.toolTip);
}

//******************************************************
QAction* SHAPERGUI::addFeature(const QString& theWBName, const QString& theTBName,
                               const QString& theId, const QString& theTitle, const QString& theTip,
                               const QIcon& theIcon, const QKeySequence& theKeys,
                               bool isCheckable, const bool isAddSeparator,
                               const QString& theStatusTip)
{
  static QString aLastTool = "";
  static int aNb = 0;
  if (aLastTool.isEmpty())
    aLastTool = theWBName;
  else if (theWBName != aLastTool) {
    aLastTool = theWBName;
    if (aNb > 20) {
      desktop()->addToolBarBreak();
      aNb = 0;
    }
  }
  aNb++;

  int aId = myActionsList.size();
  myActionsList.append(theId);
  SUIT_Desktop* aDesk = application()->desktop();
  int aKeys = 0;
  for (int i = 0; i < theKeys.count(); i++)
    aKeys += theKeys[i];
  QAction* aAction = createAction(aId, theTip, theIcon, theTitle, theTip, aKeys, aDesk,
                                  isCheckable);
  aAction->setStatusTip(theStatusTip);

  aAction->setData(theId);

  int aWBMenu = createMenu(theWBName, -1, -1, 30/*10-Window, 1000 - Help*/);
  int aItemId = createMenu(aId, aWBMenu);
  if (isAddSeparator)
    createMenu(separator(), aWBMenu);

  int aWBTool = createTool(theTBName, theTBName);
  int aToolId = createTool(aId, aWBTool);
  if (isAddSeparator)
    createTool(separator(), aWBTool);

  return aAction;
}

bool SHAPERGUI::isFeatureOfNested(const QAction* theAction)
{
  return dynamic_cast<const SHAPERGUI_NestedButton*>(theAction);
}

QAction* SHAPERGUI::addFeatureOfNested(const QString& theWBName,
                                       const ActionInfo& theInfo,
                                       const QList<QAction*>& theNestedActions)
{
  myActionsList.append(theInfo.id);
  SUIT_Desktop* aDesk = application()->desktop();
  SHAPERGUI_NestedButton* anAction = new SHAPERGUI_NestedButton(aDesk, theNestedActions);
  anAction->setData(theInfo.id);
  anAction->setCheckable(theInfo.checkable);
  anAction->setChecked(theInfo.checked);
  anAction->setEnabled(theInfo.enabled);
  anAction->setVisible(theInfo.visible);
  anAction->setIcon(theInfo.icon);
  anAction->setText(theInfo.text);
  anAction->setToolTip(theInfo.toolTip);
  anAction->setShortcut(theInfo.shortcut);
  anAction->setFont(theInfo.font);

  int aWBMenu = createMenu(theWBName, -1, -1, 30);
  int aItemId = createMenu(anAction, aWBMenu);
  createMenu(separator(), aWBMenu); /// nested action is always separated of others

  int aWBTool = createTool(theWBName, theWBName);
  int aToolId = createTool(anAction, aWBTool);
  createTool(separator(), aWBTool); /// nested action is always separated of others

  return anAction;
}


//******************************************************
QAction* SHAPERGUI::addDesktopCommand(const QString& theId, const QString& theTitle,
                                           const QString& theTip, const QIcon& theIcon,
                                           const QKeySequence& theKeys, bool isCheckable,
                                           const char* theMenuSourceText, const int theMenuPosition)
{
  int aMenu = createMenu(tr(theMenuSourceText), -1, -1);

  int aId = myActionsList.size();
  myActionsList.append(theId);
  SUIT_Desktop* aDesk = application()->desktop();
  int aKeys = 0;
  for (int i = 0; i < theKeys.count(); i++)
    aKeys += theKeys[i];
  QAction* aAction = createAction(aId, theTip, theIcon, theTitle, theTip, aKeys, aDesk,
                                  isCheckable);
  aAction->setStatusTip(theTip);
  aAction->setData(theId);
  createMenu(aId, aMenu, theMenuPosition);
  return aAction;
}

//******************************************************
void SHAPERGUI::addDesktopMenuSeparator(const char* theMenuSourceText, const int theMenuPosition)
{
  int aMenu = createMenu(tr(theMenuSourceText), -1, -1);
  createMenu(separator(), aMenu, -1, theMenuPosition);
}

bool SHAPERGUI::addActionInToolbar( QAction* theAction, const QString& theToolBarTitle )
{
  if( !theAction )
    return false;

  SUIT_Desktop* aDesktop = application()->desktop();
  if( !aDesktop )
    return false;

  QtxActionToolMgr* aToolMgr = aDesktop->toolMgr();
  if( !aToolMgr )
    return false;

  aToolMgr->append( theAction, theToolBarTitle );
  return true;
}

//******************************************************
QList<QAction*> SHAPERGUI::commandList() const
{
  QList<QAction*> aActions;
  for (int i = 0; i < myActionsList.size(); i++) {
    QAction* aCmd = action(i);
    if (aCmd && myActionsList.contains(aCmd->data().toString()))
      aActions.append(aCmd);
  }

  return aActions;
}

//******************************************************
QMainWindow* SHAPERGUI::desktop() const
{
  return application()->desktop();
}

void SHAPERGUI::setFeatureInfo(const QString& theFeatureId,
                               const std::shared_ptr<Config_FeatureMessage>& theMessage)
{
  myFeaturesInfo.insert(theFeatureId, theMessage);
}

std::shared_ptr<Config_FeatureMessage> SHAPERGUI::featureInfo(const QString& theFeatureId)
{
  std::shared_ptr<Config_FeatureMessage> aMessage;
  if (myFeaturesInfo.contains(theFeatureId))
    aMessage =  myFeaturesInfo[theFeatureId];
  return aMessage;
}

//******************************************************
void SHAPERGUI::selectionChanged()
{
  LightApp_Module::selectionChanged();
  myWorkshop->salomeViewerSelectionChanged();
}

//******************************************************
void SHAPERGUI::contextMenuPopup(const QString& theClient, QMenu* theMenu, QString& theTitle)
{
  myWorkshop->contextMenuMgr()->updateViewerMenu();
  myWorkshop->contextMenuMgr()->addViewerMenu(theMenu);
  LightApp_Module::contextMenuPopup(theClient, theMenu, theTitle);
}


//******************************************************
void SHAPERGUI::createPreferences()
{
  LightApp_Preferences* pref = preferences();
  if (!pref)
    return;
  ModuleBase_Preferences::updateConfigByResources();
  QString aModName = moduleName();

  QtxPreferenceItem* item = pref->findItem(aModName, true );
  if ( item && (!item->isEmpty() )) {
    item->parentItem()->removeItem(item);
    delete item;
  }

  int catId = pref->addPreference(aModName, -1 );
  if ( catId == -1 )
    return;
  SHAPERGUI_PrefMgr aMgr(pref, aModName);
  ModuleBase_Preferences::createEditContent(&aMgr, catId);

  int viewTab = pref->addItem(tr("Viewer"), catId);
  // Create other parameters group in viewer tab
  int otherGroup = pref->addItem(tr("Default selection"), viewTab);
  pref->setItemProperty("columns", 3, otherGroup);
  pref->addItem(tr("Faces"), otherGroup,
                         SUIT_PreferenceMgr::Bool,
                         ModuleBase_Preferences::VIEWER_SECTION, "face-selection");
  pref->addItem(tr("Edges"), otherGroup,
                         SUIT_PreferenceMgr::Bool,
                         ModuleBase_Preferences::VIEWER_SECTION, "edge-selection");
  pref->addItem(tr("Vertices"), otherGroup,
                         SUIT_PreferenceMgr::Bool,
                         ModuleBase_Preferences::VIEWER_SECTION, "vertex-selection");

  int sensitivityGroup = pref->addItem(tr("Selection sensitivity"), viewTab);
  pref->setItemProperty("columns", 2, sensitivityGroup);
  pref->addItem(tr("Vertex"), sensitivityGroup, SUIT_PreferenceMgr::DblSpin,
                ModuleBase_Preferences::VIEWER_SECTION, "point-selection-sensitivity");
  pref->addItem(tr("Edge"), sensitivityGroup, SUIT_PreferenceMgr::DblSpin,
                ModuleBase_Preferences::VIEWER_SECTION, "edge-selection-sensitivity");
  pref->retrieve();
}

//******************************************************
void SHAPERGUI::preferencesChanged(const QString& theSection, const QString& theParam)
{
  SUIT_ResourceMgr* aResMgr = application()->resourceMgr();
  QString aVal = aResMgr->stringValue(theSection, theParam);
  Config_Prop* aProp = Config_PropManager::findProp(theSection.toStdString(),
                                                    theParam.toStdString());
  std::string aValue = aVal.toStdString();
  if (aValue.empty()) {
    aValue = aProp->defaultValue();
    aResMgr->setValue(theSection, theParam, QString(aValue.c_str()));

    LightApp_Preferences* pref = preferences();
    if (pref)
      pref->retrieve();
  }
  aProp->setValue(aValue);

  myWorkshop->displayer()->redisplayObjects();
}

void SHAPERGUI::putInfo(const QString& theInfo, const int theMSecs)
{
  application()->putInfo(theInfo, theMSecs);
}

bool SHAPERGUI::abortAllOperations()
{
  return workshop()->operationMgr()->abortAllOperations();
}

void SHAPERGUI::createFeatureActions()
{
  myWorkshop->menuMgr()->createFeatureActions();
}

void SHAPERGUI::onWhatIs(bool isToggled)
{
  QDockWidget* aInspection = myWorkshop->inspectionPanel();
  if (sender() == myWhatIsAction) {
    QAction* aViewAct = aInspection->toggleViewAction();
    aViewAct->blockSignals(true);
    aViewAct->setChecked(isToggled);
    aViewAct->blockSignals(false);
    aInspection->setVisible(isToggled);
  }
  else {
    myWhatIsAction->blockSignals(true);
    myWhatIsAction->setChecked(isToggled);
    myWhatIsAction->blockSignals(false);
  }
}