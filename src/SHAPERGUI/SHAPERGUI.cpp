// Copyright (C) 2014-2023  CEA, EDF
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

#include "SHAPERGUI.h"
#include "SHAPERGUI_DataModel.h"
#include "SHAPERGUI_OCCSelector.h"
#include "SHAPERGUI_NestedButton.h"
#include "SHAPERGUI_ToolbarsMgr.h"

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
#include <XGUI_ViewerProxy.h>

#include <ModuleBase_Operation.h>
#include <ModuleBase_Preferences.h>
#include <ModuleBase_ActionInfo.h>
#include <ModuleBase_IModule.h>

#include <ModelAPI_Tools.h>

#include <LightApp_Application.h>
#include <LightApp_SelectionMgr.h>
#include <LightApp_OCCSelector.h>
#include <LightApp_Study.h>

#include <OCCViewer_ViewModel.h>
#include <OCCViewer_ViewPort3d.h>

#include <SUIT_Selector.h>
#include <SUIT_Desktop.h>
#include <SUIT_ViewManager.h>
#include <SUIT_ViewWindow.h>
#include <SUIT_ResourceMgr.h>
#include <SUIT_DataBrowser.h>

#include <QtxPopupMgr.h>
#include <QtxActionMenuMgr.h>
#include <QtxActionToolMgr.h>
#include <QtxResourceMgr.h>
#include <QtxInfoPanel.h>

#include <Config_PropManager.h>
#include <Config_ModuleReader.h>

#include <AIS_ListOfInteractive.hxx>
#include <AIS_ListIteratorOfListOfInteractive.hxx>
#include <Standard_Version.hxx>

#include <QDockWidget>
#include <QAction>
#include <QTimer>
#include <QMenu>
#include <QToolBar>

#include <ModelAPI_Session.h>
#include <Events_MessageBool.h>

#if OCC_VERSION_HEX < 0x070400
  #define SALOME_PATCH_FOR_CTRL_WHEEL
#endif

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


static const QString ToolbarsSection("SHAPER_Toolbars");
static const QString FreeCommandsParam("OutOFToolbars");


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
      mySelector(0), myIsOpened(0), myPopupMgr(0), myIsInspectionVisible(false),
  myInspectionPanel(0), myIsFacesPanelVisible(false), myIsToolbarsModified(false),
  myAxisArrowRate(-1)
{
  myWorkshop = new XGUI_Workshop(this);
  connect(myWorkshop, SIGNAL(commandStatusUpdated()),
          this, SLOT(onUpdateCommandStatus()));

  myProxyViewer = new SHAPERGUI_SalomeViewer(this);

  ModuleBase_Preferences::setResourceMgr(application()->resourceMgr());

  // It will be called in XGUI_Workshop::startApplication
  // ModuleBase_Preferences::loadCustomProps();
}

//******************************************************
SHAPERGUI::~SHAPERGUI()
{
  delete myWorkshop;
  delete myProxyViewer;
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

  int aId = getNextCommandId();
  myActionsList.append(aId);
  SUIT_Desktop* aDesk = application()->desktop();
  QString aTip = tr("Show inspection window");
  myWhatIsAction = createAction(aId, aTip, QIcon(":pictures/whatis.png"), tr("What Is"),
    aTip, QKeySequence(), aDesk, true, this, SLOT(onWhatIs(bool)));
  myWhatIsAction->setStatusTip(aTip);
  myWhatIsAction->setData("INSPECTION_CMD");
  createMenu(aId, aMenu, 0);

  QString aToolName = tr("Inspection");
  int aTool = createTool(aToolName);
#ifdef _DEBUG
  int aToolId =
#endif
    createTool(myWhatIsAction, aTool);
  registerCommandToolbar(aToolName, aId);

  // Define Edit toolbars command
  aId = getNextCommandId();
  //myActionsList.append(aId); Do not use it for editing of toolbars
  aTip = tr("Edit toolbars of the module");
#ifdef _DEBUG
  QAction* aAction =
#endif
    createAction(aId, aTip, QIcon(":pictures/configure_toolbars.png"),
    tr("Edit toolbars..."), aTip, QKeySequence(), aDesk, false, this, SLOT(onEditToolbars()));
  int aEditMenu = createMenu(tr("MEN_DESK_EDIT"), -1, -1, 30);
#ifdef _DEBUG
  int aEditItem =
#endif
    createMenu(aId, aEditMenu);

  if (!myInspectionPanel) {
    myInspectionPanel = myWorkshop->inspectionPanel();
    connect(myInspectionPanel->toggleViewAction(), SIGNAL(toggled(bool)),
      this, SLOT(onWhatIs(bool)));
  }
  hideInternalWindows();

  // Initialize viewer proxy if OCC viewer is already exist
  ViewManagerList aOCCViewManagers;
  application()->viewManagers(OCCViewer_Viewer::Type(), aOCCViewManagers);
  if (aOCCViewManagers.size() > 0) {
    SUIT_ViewManager* aMgr = aOCCViewManagers.first();
    SUIT_ViewWindow* aWnd = aMgr->getActiveView();
    if (aWnd) {
      OCCViewer_ViewWindow* aOccWnd = static_cast<OCCViewer_ViewWindow*>(aWnd);
      OCCViewer_ViewPort3d* aViewPort = aOccWnd->getViewPort();
      if (aViewPort) {
        XGUI_ViewerProxy* aViewer = myWorkshop->viewer();
        aViewPort->installEventFilter(aViewer);
        Handle(V3d_View) aView = aViewPort->getView();
        aViewer->SetScale(aView, aView->Camera()->Scale());
        // We can not create selector here because other modules will be deactivated later
        //onViewManagerAdded(aMgr);
      }
    }
  }
  SHAPERGUI_DataModel* aDataModel = dynamic_cast<SHAPERGUI_DataModel*>(dataModel());
  aDataModel->initRootObject();
}

//******************************************************
void SHAPERGUI::windows(QMap<int, int>& theWndMap) const
{
  theWndMap.insert(LightApp_Application::WT_PyConsole, Qt::BottomDockWidgetArea);
  theWndMap.insert(LightApp_Application::WT_InfoPanel, Qt::RightDockWidgetArea);
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
//  // selector and initialize viewer with it. It sets interactive context to the
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
  ModelAPI_Session::get()->moduleDocument(); // initialize a root document if not done yet

  // this must be done in the initialization and in activation (on the second activation
  // initialization in not called, so SComponent must be added anyway
  SHAPERGUI_DataModel* aDataModel = dynamic_cast<SHAPERGUI_DataModel*>(dataModel());
  aDataModel->initRootObject();


  bool isDone = LightApp_Module::activateModule(theStudy);
  loadToolbarsConfig();

  if (isDone) {
    setMenuShown(true);
    setToolShown(true);

    QObject* aObj = myWorkshop->objectBrowser()->parent();
    QDockWidget* aObjDoc = dynamic_cast<QDockWidget*>(aObj);
    if (aObjDoc) {
      myWorkshop->objectBrowser()->setVisible(true);
      aObjDoc->setVisible(true);
      desktop()->tabifyDockWidget(aObjDoc, myWorkshop->propertyPanel());
      aObjDoc->toggleViewAction()->setVisible(true);
    }

    myInspectionPanel->toggleViewAction()->setVisible(true);

    myWorkshop->facesPanel()->toggleViewAction()->setVisible(true);
    if (myIsFacesPanelVisible)
      myWorkshop->facesPanel()->show();
    myWorkshop->propertyPanel()->toggleViewAction()->setVisible(true);

    if (!mySelector) {
      ViewManagerList OCCViewManagers;
      application()->viewManagers(OCCViewer_Viewer::Type(), OCCViewManagers);
      if (OCCViewManagers.size() > 0) {
        onViewManagerAdded(OCCViewManagers.first());
      }
    }
    // it should be performed after the selector creation in order to have AISContext
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
  myIsEditEnabled = getApp()->isEditEnabled();
  getApp()->setEditEnabled(false);

  // Synchronize displayed objects
  Handle(AIS_InteractiveContext) aContext;
  if (mySelector && mySelector->viewer())
    aContext = mySelector->viewer()->getAISContext();

  if (!aContext.IsNull()) {
    XGUI_Displayer* aDisp = myWorkshop->displayer();
    QObjectPtrList aObjList = aDisp->displayedObjects();

    if (myOldSelectionColor.size() == 0)
      myOldSelectionColor = aDisp->selectionColor();

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

  // Post-processing for LoadScriptId to remove created(if it was created) SALOME Object Browser
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
  updateInfoPanel();

  //connect(myWorkshop->operationMgr(), SIGNAL(operationResumed(ModuleBase_Operation*)),
  //        this, SLOT(onOperationResumed(ModuleBase_Operation*)));
  //connect(myWorkshop->operationMgr(), SIGNAL(operationStopped(ModuleBase_Operation*)),
  //        this, SLOT(onOperationStopped(ModuleBase_Operation*)));
  connect(myWorkshop->operationMgr(), SIGNAL(operationCommitted(ModuleBase_Operation*)),
          this, SLOT(onOperationCommitted(ModuleBase_Operation*)));
  connect(myWorkshop->operationMgr(), SIGNAL(operationAborted(ModuleBase_Operation*)),
          this, SLOT(onOperationAborted(ModuleBase_Operation*)));

  return isDone;
}

//******************************************************
void SHAPERGUI::hideInternalWindows()
{
  myProxyViewer->activateViewer(false);
  setMenuShown(false);
  setToolShown(false);

  QObject* aObj = myWorkshop->objectBrowser()->parent();
  QDockWidget* aObjDoc = dynamic_cast<QDockWidget*>(aObj);
  if (aObjDoc) {
    aObjDoc->setVisible(false);
    myWorkshop->objectBrowser()->setVisible(false);
    aObjDoc->toggleViewAction()->setVisible(false);
  }

  myInspectionPanel->hide();
  myInspectionPanel->toggleViewAction()->setVisible(false);

  myWorkshop->facesPanel()->hide();
  myWorkshop->facesPanel()->toggleViewAction()->setVisible(false);

  myWorkshop->propertyPanel()->hide();
  myWorkshop->propertyPanel()->toggleViewAction()->setVisible(false);

  myWorkshop->hidePanel(myWorkshop->facesPanel());
}


//******************************************************
bool SHAPERGUI::deactivateModule(SUIT_Study* theStudy)
{
  saveToolbarsConfig();
  myWorkshop->deactivateModule();

  myIsInspectionVisible = myInspectionPanel->isVisible();
  myIsFacesPanelVisible = myWorkshop->facesPanel()->isVisible();
  hideInternalWindows();


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
    // Restore size of arrows of trihedron
    if (myAxisArrowRate > 0) {
      Handle(AIS_Trihedron) aTrihedron = mySelector->viewer()->getTrihedron();
      Handle(Prs3d_DatumAspect) aDatumAspect = aTrihedron->Attributes()->DatumAspect();
      aDatumAspect->SetAttribute(Prs3d_DP_ShadingConeLengthPercent, myAxisArrowRate);
      Handle(AIS_InteractiveContext) aContext = mySelector->viewer()->getAISContext();
      aContext->Redisplay(aTrihedron, false);
    }
    myWorkshop->displayer()->setSelectionColor(myOldSelectionColor);
    myProxyViewer->setSelector(0);

    LightApp_SelectionMgr* aMgr = getApp()->selectionMgr();
    QList<SUIT_Selector*> aList;
    aMgr->selectors(aList);
    foreach(SUIT_Selector* aSel, aList) {
      aSel->setEnabled(aSel != mySelector);
    }

    delete mySelector;
    mySelector = 0;
  }

  //myWorkshop->contextMenuMgr()->disconnectViewer();

  getApp()->setEditEnabled(myIsEditEnabled);

  myOldSelectionColor.clear();

  // Post-processing for LoadScriptId to remove created(if it was created) SALOME Object Browser
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

  publishToStudy();

  return LightApp_Module::deactivateModule(theStudy);
}

//******************************************************
void SHAPERGUI::logShaperGUIEvent()
{
  QAction* anAction = static_cast<QAction*>(sender());
  QString anId = anAction->data().toString();
  QString anEventDescription ("SHAPER");
  if (anId.contains("Sketch"))
    anEventDescription += " sketcher";
  anEventDescription += ": ";
  anEventDescription += anAction->text();
  anEventDescription += " has been started";
  CAM_Application::logUserEvent(anEventDescription);
}

//******************************************************
void SHAPERGUI::onOperationCommitted(ModuleBase_Operation* theOperation)
{
  //QString anEventDescription ("SHAPER operation has been committed");
  QString anEventDescription ("SHAPER operation ");
  anEventDescription += theOperation->id();
  anEventDescription += " has been committed";
  CAM_Application::logUserEvent(anEventDescription);
}

//******************************************************
void SHAPERGUI::onOperationAborted(ModuleBase_Operation* theOperation)
{
  QString anEventDescription ("SHAPER operation ");
  anEventDescription += theOperation->id();
  anEventDescription += " has been aborted";
  CAM_Application::logUserEvent(anEventDescription);
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
        ResultPtr aRes;
        foreach(ObjectPtr aObj, aObjects) {
          aObj->setDisplayed(false);
          aRes = std::dynamic_pointer_cast<ModelAPI_Result>(aObj);
          if (aRes.get()) {
            while (aRes = ModelAPI_Tools::bodyOwner(aRes)) {
              aRes->setDisplayed(false);
            }
          }
        }
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
  myWorkshop->displayer()->updateViewer();
  myWorkshop->updateCommandStatus();
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

  LightApp_Application* aApp = dynamic_cast<LightApp_Application*>(application());
  QtxInfoPanel* aInfoPanel = aApp->infoPanel();
  if (aInfoPanel->isVisible())
    updateInfoPanel();
}

//******************************************************
SHAPERGUI_OCCSelector* SHAPERGUI::createSelector(SUIT_ViewManager* theMgr)
{
  if (theMgr->getType() == OCCViewer_Viewer::Type()) {
    OCCViewer_Viewer* aViewer = static_cast<OCCViewer_Viewer*>(theMgr->getViewModel());

    // Remember current length of arrow of axis
    Handle(AIS_Trihedron) aTrihedron = aViewer->getTrihedron();
    Handle(Prs3d_DatumAspect) aDatumAspect = aTrihedron->Attributes()->DatumAspect();
    myAxisArrowRate = aDatumAspect->Attribute(Prs3d_DP_ShadingConeLengthPercent);

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

    if (myOldSelectionColor.size() == 0)
      myOldSelectionColor = myWorkshop->displayer()->selectionColor();

    std::vector<int> aColor = Config_PropManager::color("Visualization", "selection_color");
    myWorkshop->displayer()->setSelectionColor(aColor);

    // Cause scaling of arrows of axis and dimensions
    myWorkshop->module()->onViewTransformed();

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

  int aId = getNextCommandId();
  myActionsList.append(aId);
  SUIT_Desktop* aDesk = application()->desktop();
  int aKeys = 0;
  for (int i = 0; i < theKeys.count(); i++)
    aKeys += theKeys[i];
  QAction* aAction = createAction(aId, theTip, theIcon, theTitle, theTip, aKeys, aDesk,
                                  isCheckable);
  aAction->setStatusTip(theStatusTip);

  aAction->setData(theId);

  int aWBMenu = createMenu(theWBName, -1, -1, 30/*10-Window, 1000 - Help*/);

  if( theId == "PointCoordinates" )
    createMenu(separator(), aWBMenu);

#ifdef _DEBUG
  int aItemId =
#endif
    createMenu(aId, aWBMenu);

  if (isAddSeparator)
    createMenu(separator(), aWBMenu);

  int aWBTool = createTool(theTBName, theTBName);
#ifdef _DEBUG
  int aToolId =
#endif
    createTool(aId, aWBTool);
  registerCommandToolbar(theTBName, aId);
  if (isAddSeparator) {
    createTool(separator(), aWBTool);
    registerCommandToolbar(theTBName, -1);
  }
  connect(aAction, SIGNAL(triggered(bool)), this, SLOT(logShaperGUIEvent()));
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
  myActionsList.append(aItemId);
  createMenu(separator(), aWBMenu); /// nested action is always separated of others

  int aWBTool = createTool(theWBName, theWBName);
#ifdef _DEBUG
  int aToolId =
#endif
    createTool(anAction, aWBTool);
  registerCommandToolbar(theWBName, aItemId);
  createTool(separator(), aWBTool); /// nested action is always separated of others
  registerCommandToolbar(theWBName, -1);

  return anAction;
}


//******************************************************
QAction* SHAPERGUI::addDesktopCommand(const QString& theId, const QString& theTitle,
                                           const QString& theTip, const QIcon& theIcon,
                                           const QKeySequence& theKeys, bool isCheckable,
                                           const char* theMenuSourceText,
                                           const QString& theSubMenu,
                                           const int theMenuPosition,
                                           const int theSuibMenuPosition)
{
  int aMenu = createMenu(tr(theMenuSourceText), -1, -1);
  if (!theSubMenu.isNull())
    aMenu = createMenu(theSubMenu, aMenu, -1, theSuibMenuPosition);

  int aId = getNextCommandId();
  myActionsList.append(aId);
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

//******************************************************
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
  foreach (int aId, myActionsList) {
    QAction* aCmd = action(aId);
    if (aCmd)
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
  LightApp_Preferences* aPref = preferences();
  if (!aPref)
    return;
  ModuleBase_Preferences::updateConfigByResources();
  QString aModName = moduleName();

  QtxPreferenceItem* item = aPref->findItem(aModName, true );
  if ( item && (!item->isEmpty() )) {
    item->parentItem()->removeItem(item);
    delete item;
  }

  int catId = aPref->addPreference(aModName, -1 );
  if ( catId == -1 )
    return;
  SHAPERGUI_PrefMgr aMgr(aPref, aModName);
  ModuleBase_Preferences::createEditContent(&aMgr, catId);

  int viewTab = aPref->addItem(tr("Viewer"), catId);
  // Create other parameters group in viewer tab
  int otherGroup = aPref->addItem(tr("Default selection"), viewTab);
  aPref->setItemProperty("columns", 3, otherGroup);
  aPref->addItem(tr("Faces"), otherGroup,
                         SUIT_PreferenceMgr::Bool,
                         ModuleBase_Preferences::VIEWER_SECTION, "face-selection");
  aPref->addItem(tr("Edges"), otherGroup,
                         SUIT_PreferenceMgr::Bool,
                         ModuleBase_Preferences::VIEWER_SECTION, "edge-selection");
  aPref->addItem(tr("Vertices"), otherGroup,
                         SUIT_PreferenceMgr::Bool,
                         ModuleBase_Preferences::VIEWER_SECTION, "vertex-selection");

  int sensitivityGroup = aPref->addItem(tr("Selection sensitivity"), viewTab);
  aPref->setItemProperty("columns", 2, sensitivityGroup);
  aPref->addItem(tr("Vertex"), sensitivityGroup, SUIT_PreferenceMgr::DblSpin,
                ModuleBase_Preferences::VIEWER_SECTION, "point-selection-sensitivity");
  aPref->addItem(tr("Edge"), sensitivityGroup, SUIT_PreferenceMgr::DblSpin,
                ModuleBase_Preferences::VIEWER_SECTION, "edge-selection-sensitivity");

  int highlightGroup = aPref->addItem(tr("Additional highlighting"), viewTab);
  aPref->setItemProperty("columns", 2, highlightGroup);
  aPref->addItem(tr("In 3d mode"), highlightGroup,
    SUIT_PreferenceMgr::Bool, ModuleBase_Preferences::VIEWER_SECTION, "highlighting-3d");
  aPref->addItem(tr("In 2d mode"), highlightGroup,
    SUIT_PreferenceMgr::Bool, ModuleBase_Preferences::VIEWER_SECTION, "highlighting-2d");

  int colorScaleGroup = aPref->addItem(tr("Color scale"), viewTab);
  aPref->setItemProperty("columns", 4, colorScaleGroup);
  int aItem = aMgr.addPreference(tr("X position"), colorScaleGroup,
    SUIT_PreferenceMgr::Double, ModuleBase_Preferences::VIEWER_SECTION, "scalar_bar_x_position");
  aPref->setItemProperty("min", 0, aItem);
  aPref->setItemProperty("max", 1, aItem);
  aItem = aMgr.addPreference(tr("Y position"), colorScaleGroup,
    SUIT_PreferenceMgr::Double, ModuleBase_Preferences::VIEWER_SECTION, "scalar_bar_y_position");
  aPref->setItemProperty("min", 0, aItem);
  aPref->setItemProperty("max", 1, aItem);
  aItem = aMgr.addPreference(tr("Width"), colorScaleGroup,
    SUIT_PreferenceMgr::Double, ModuleBase_Preferences::VIEWER_SECTION, "scalar_bar_width");
  aPref->setItemProperty("min", 0, aItem);
  aPref->setItemProperty("max", 1, aItem);
  aItem = aMgr.addPreference(tr("Height"), colorScaleGroup,
    SUIT_PreferenceMgr::Double, ModuleBase_Preferences::VIEWER_SECTION, "scalar_bar_height");
  aPref->setItemProperty("min", 0, aItem);
  aPref->setItemProperty("max", 1, aItem);
  aItem = aMgr.addPreference(tr("Intervals number"), colorScaleGroup,
    SUIT_PreferenceMgr::Integer, ModuleBase_Preferences::VIEWER_SECTION, "scalar_bar_nb_intervals");
  aPref->setItemProperty("min", 0, aItem);
  aPref->setItemProperty("max", 100, aItem);
  aItem = aMgr.addPreference(tr("Text height"), colorScaleGroup,
    SUIT_PreferenceMgr::Integer, ModuleBase_Preferences::VIEWER_SECTION, "scalar_bar_text_height");
  aPref->setItemProperty("min", 0, aItem);
  aPref->setItemProperty("max", 100, aItem);
  aItem = aMgr.addPreference(tr("Text color"), colorScaleGroup,
    SUIT_PreferenceMgr::Color, ModuleBase_Preferences::VIEWER_SECTION, "scalar_bar_text_color");

  int aGroupNamesGroup = aMgr.addPreference(tr("Group names display"), viewTab,
    SUIT_PreferenceMgr::GroupBox , ModuleBase_Preferences::VIEWER_SECTION, "group_names_display");
  aPref->setItemProperty("columns", 3, aGroupNamesGroup);
  aMgr.addPreference(tr("Text font"), aGroupNamesGroup,
    SUIT_PreferenceMgr::String, ModuleBase_Preferences::VIEWER_SECTION, "group_names_font");
  aItem = aMgr.addPreference(tr("Text size"), aGroupNamesGroup,
    SUIT_PreferenceMgr::Integer, ModuleBase_Preferences::VIEWER_SECTION, "group_names_size");
  aPref->setItemProperty("min", 8, aItem);
  aPref->setItemProperty("max", 100, aItem);
  aItem = aMgr.addPreference(tr("Text color"), aGroupNamesGroup,
    SUIT_PreferenceMgr::Color, ModuleBase_Preferences::VIEWER_SECTION, "group_names_color");

  aPref->retrieve();
}

//******************************************************
void SHAPERGUI::preferencesChanged(const QString& theSection, const QString& theParam)
{
  SUIT_ResourceMgr* aResMgr = application()->resourceMgr();
  QString aVal = aResMgr->stringValue(theSection, theParam);
  Config_Prop* aProp = Config_PropManager::findProp(theSection.toStdString(),
                                                    theParam.toStdString());
  if (!aProp)
    return; // invalid case, the property default value must be registered in XML file
  std::string aValue = aVal.toStdString();
  if (aValue.empty()) {
    aValue = aProp->defaultValue();
    aResMgr->setValue(theSection, theParam, QString(aValue.c_str()));

    LightApp_Preferences* aPref = preferences();
    if (aPref)
      aPref->retrieve();
  }
  aProp->setValue(aValue);

  if (theSection == "Visualization") {
    if (theParam == "selection_color") {
      std::vector<int> aColor = Config_PropManager::color("Visualization", "selection_color");
      myWorkshop->displayer()->setSelectionColor(aColor);
    }
    if ((theParam == "zoom_trihedron_arrows") || (theParam == "axis_arrow_size")) {
      if (mySelector) {
        Handle(AIS_Trihedron) aTrihedron = mySelector->viewer()->getTrihedron();
        if (!aTrihedron.IsNull()) {
          bool aZoom = Config_PropManager::boolean("Visualization", "zoom_trihedron_arrows");
          Handle(AIS_InteractiveContext) aContext = mySelector->viewer()->getAISContext();

          ModuleBase_IViewer* aViewer = myWorkshop->viewer();
          Handle(V3d_View) aView = aViewer->activeView();
          if (aZoom) {
            double aAxLen =
              aView->Convert(Config_PropManager::integer("Visualization", "axis_arrow_size"));
            Handle(Prs3d_DatumAspect) aDatumAspect = aTrihedron->Attributes()->DatumAspect();
            double aAxisLen = aDatumAspect->AxisLength(Prs3d_DP_XAxis);
            myAxisArrowRate = aDatumAspect->Attribute(Prs3d_DP_ShadingConeLengthPercent);
            aDatumAspect->SetAttribute(Prs3d_DP_ShadingConeLengthPercent, aAxLen / aAxisLen);
            aTrihedron->Attributes()->SetDatumAspect(aDatumAspect);
            aContext->Redisplay(aTrihedron, true);

          }
          else if (myAxisArrowRate > 0) {
            Handle(Prs3d_DatumAspect) aDatumAspect = aTrihedron->Attributes()->DatumAspect();
            aDatumAspect->SetAttribute(Prs3d_DP_ShadingConeLengthPercent, myAxisArrowRate);
            aContext->Redisplay(aTrihedron, true);
          }
        }
      }
    }
  }
  else if (theSection == ModuleBase_Preferences::GENERAL_SECTION && theParam == "create_init_part") {
    bool aCreate = ModuleBase_Preferences::resourceMgr()->booleanValue(
      ModuleBase_Preferences::GENERAL_SECTION, "create_init_part", true);
    Events_MessageBool aCreateMsg(Events_Loop::eventByName(EVENT_CREATE_PART_ON_START), aCreate);
    aCreateMsg.send();
  }
  else if (theSection == ModuleBase_Preferences::VIEWER_SECTION &&
           theParam.startsWith("group_names_"))
  { // one of the group names parameter changed, so, update the groups names vizualization
    myWorkshop->updateGroupsText();
    myWorkshop->displayer()->updateViewer();
  }
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
  if (sender() == myWhatIsAction) {
    QAction* aViewAct = myInspectionPanel->toggleViewAction();
    aViewAct->blockSignals(true);
    aViewAct->setChecked(isToggled);
    aViewAct->blockSignals(false);
    myInspectionPanel->setVisible(isToggled);
  }
  else {
    myWhatIsAction->blockSignals(true);
    myWhatIsAction->setChecked(isToggled);
    myWhatIsAction->blockSignals(false);
    myInspectionPanel->setVisible(isToggled);
  }
}

void SHAPERGUI::updateModuleVisibilityState()
{
  LightApp_Module::updateModuleVisibilityState();
  onWhatIs(myIsInspectionVisible);

  // the following code is caused by #187 bug.
  // SALOME saves the dock widget positions before deactivateModule() and
  // load it after the module activation. So, if the panel is visible before
  // deactivate, it becomes visible after activate.
  // In order to avoid the visible property panel, we hide it here
  ModuleBase_Operation* anOperation = myWorkshop->module()->currentOperation();
  if (!anOperation) {
    myWorkshop->hidePanel(myWorkshop->propertyPanel());
  }
}

void SHAPERGUI::onEditToolbars()
{
  SHAPERGUI_ToolbarsDlg aDlg(this);
  if (aDlg.exec() == QDialog::Accepted) {
    if (aDlg.isReset())
      resetToolbars();
    else
      updateToolbars(aDlg.result());
  }
}

void SHAPERGUI::registerCommandToolbar(const QString& theToolName, int theCommandId)
{
  if (!myToolbars.contains(theToolName))
    myToolbars[theToolName] = QList<int>();
  myToolbars[theToolName].append(theCommandId);
}

int SHAPERGUI::getNextCommandId() const
{
  QtxActionMenuMgr* aMenuMgr = menuMgr();
  QIntList aIds = aMenuMgr->idList();
  int aId = aIds.count();
  while (action(aId) || myActionsList.contains(aId))
    aId++;
  return aId;
}

void SHAPERGUI::updateToolbars(const QMap<QString, QIntList>& theNewToolbars)
{
  // Store default toolbars
  if (myDefaultToolbars.size() == 0)
    myDefaultToolbars = myToolbars;

  QtxActionToolMgr* aMgr = toolMgr();
  QStringList aToolbars = theNewToolbars.keys();
  QIntList aCommands, aOldCmd;
  int aToolbarId;
  QAction* aAction;
  int aActionId;
  foreach(QString aName, aToolbars) {
    aCommands = theNewToolbars[aName];
    // Find or create toolbar
    if (aMgr->hasToolBar(aName)) {
      aToolbarId = aMgr->find(aMgr->toolBar(aName));
      aOldCmd = myToolbars[aName];
    }
    else {
      aToolbarId = aMgr->createToolBar(aName);
    }
    int aPos = 0;
    foreach(int aCmd, aCommands) {
      // Find action
      if (aCmd == -1)
        aAction = separator();
      else
        aAction = action(aCmd);
      aActionId = aMgr->actionId(aAction);
      if (aActionId == -1) {
        // Add new action
        aMgr->insert(aAction, aToolbarId, aPos);
      }
      else {
        // Change position of action
        if (aMgr->index(aActionId, aToolbarId) != aPos) {
          if (aMgr->containsAction(aActionId, aToolbarId))
            aMgr->remove(aActionId, aToolbarId);
          aMgr->insert(aActionId, aToolbarId, aPos);
        }
      }
      aOldCmd.removeAll(aCmd);
      aPos++;
    }
    // remove extra actions
    foreach(int aCmd, aOldCmd) {
      aAction = action(aCmd);
      aActionId = aMgr->actionId(aAction);
      aMgr->remove(aActionId, aToolbarId);
    }
    myToolbars.remove(aName);
  }
  // Remove extra toolbars
  aToolbars = myToolbars.keys();
  foreach(QString aName, aToolbars) {
    aMgr->removeToolBar(aName);
  }
  // Set new toolbars structure
  myToolbars = theNewToolbars;
  myIsToolbarsModified = true;
}

void SHAPERGUI::saveToolbarsConfig()
{
  if (!myIsToolbarsModified)
    return;
  // Save toolbars configuration into map
  QMap<QString, QStringList> aToolbarsConfig;
#ifdef _DEBUG
  QtxActionToolMgr* aMgr =
#endif
    toolMgr();
  QStringList aToolbars = myToolbars.keys();
  QIntList aActionsIds;
  foreach(QString aName, aToolbars) {
    aActionsIds = myToolbars[aName];
    QStringList aContent;
    foreach(int aId, aActionsIds) {
      if (aId == -1)
        aContent.append("");
      else
        aContent.append(action(aId)->data().toString());
    }
    aToolbarsConfig[aName] = aContent;
  }
  // Store the configuration into resources
  SUIT_ResourceMgr* aResMgr = application()->resourceMgr();
  QStringList aNames = aToolbarsConfig.keys();
  QStringList aValues;
  foreach(QString aToolbar, aNames) {
    aResMgr->setValue(ToolbarsSection, aToolbar, aToolbarsConfig[aToolbar].join(","));
  }
  // Remove obsolete parameters from resources
  QStringList aOldParams = aResMgr->parameters(ToolbarsSection);
  foreach(QString aName, aOldParams) {
    if (!aToolbars.contains(aName))
      aResMgr->remove(ToolbarsSection, aName);
  }
  // Store current list of free commands
  QIntList aFreeCommands = getFreeCommands();
  QStringList aFreeList;
  foreach(int aId, aFreeCommands) {
    aFreeList.append(action(aId)->data().toString());
  }
  if (aFreeList.size() > 0)
    aResMgr->setValue(ToolbarsSection, FreeCommandsParam, aFreeList.join(","));

  myIsToolbarsModified = false;
}

void SHAPERGUI::loadToolbarsConfig()
{
  SUIT_ResourceMgr* aResMgr = application()->resourceMgr();
  QStringList aToolbarNames = aResMgr->parameters(ToolbarsSection);
  if (aToolbarNames.size() == 0)
    return;

  // Create commands map
  QMap<QString, int> aCommandsMap;
  QString aCmdIdStr;
  foreach(int aId, myActionsList) {
    aCmdIdStr = action(aId)->data().toString();
    aCommandsMap[aCmdIdStr] = aId;
  }

  // Create new toolbars structure
  QMap<QString, QIntList> aToolbars;
  QStringList aCommands;
  QIntList aKnownCommands;
  QList<QAction*> aActions;
  foreach(QString aName, aToolbarNames) {
    aCommands = aResMgr->stringValue(ToolbarsSection, aName).split(",");
    if (aName == FreeCommandsParam) {
      // The value is a list of free commands
      foreach(QString aCommand, aCommands) {
        aKnownCommands.append(aCommandsMap[aCommand]);
      }
    }
    else {
      aToolbars[aName] = QIntList();
      if (aCommands.size() > 0) {
        foreach(QString aCommand, aCommands) {
          if (aCommand.isEmpty())
            aToolbars[aName].append(-1);
          else if (aCommandsMap.contains(aCommand)) {
            int aId = aCommandsMap[aCommand];
            aToolbars[aName].append(aId);
            aKnownCommands.append(aId);
          }
        }
      }
    }
  }
  // Find new and obsolete commands
  QIntList aNewCommands = myActionsList;
  foreach(int aId, myActionsList) {
    if (aKnownCommands.contains(aId)) {
      aKnownCommands.removeAll(aId);
      aNewCommands.removeAll(aId);
    }
  }
  if (aNewCommands.size() > 0) {
    // Add new commands to toolbars structure
    QStringList aKeys = myToolbars.keys();
    foreach(int aNewId, aNewCommands) {
      foreach(QString aName, aKeys) {
        if (myToolbars[aName].contains(aNewId)) {
          if (!aToolbars.contains(aName)) {
            aToolbars[aName] = QIntList();
          }
          aToolbars[aName].append(aNewId);
        }
      }
    }
  }
  if (aKnownCommands.size() > 0) {
    // Remove obsolete commands from the toolbars structure
    QStringList aKeys = aToolbars.keys();
    foreach(int aOldId, aKnownCommands) {
      foreach(QString aName, aKeys) {
        if (aToolbars[aName].contains(aOldId)) {
          aToolbars[aName].removeAll(aOldId);
          if (aToolbars[aName].size() == 0)
            aToolbars.remove(aName);
        }
      }
    }
  }
  updateToolbars(aToolbars);
  myIsToolbarsModified = false;
}


QIntList SHAPERGUI::getFreeCommands() const
{
  QIntList aFreeCommands;
  QtxActionToolMgr* aMgr = toolMgr();
  QAction* anAction;
  int aId;
  QMap<QString, QIntList>::const_iterator aIt;
  QIntList aShaperActions = shaperActions();
  foreach(int aCmd, aShaperActions) {
    anAction = action(aCmd);
    aId = aMgr->actionId(anAction);
    if (!aMgr->containsAction(aId))
      aFreeCommands.append(aCmd);
  }
  return aFreeCommands;
}

void SHAPERGUI::resetToolbars()
{
  if (!myDefaultToolbars.isEmpty())
    updateToolbars(myDefaultToolbars);
  myIsToolbarsModified = false;
  SUIT_ResourceMgr* aResMgr = application()->resourceMgr();
  aResMgr->remove(ToolbarsSection);
}

void SHAPERGUI::publishToStudy()
{
  if (isActiveModule() && ModelAPI_Session::get()->hasModuleDocument()) {
    myWorkshop->module()->launchOperation("PublishToStudy", false);

    // update SHAPERSTUDY objects in OCC and VTK viewers
    QStringList aVMList;
    aVMList << "OCCViewer" << "VTKViewer";
    getApp()->updatePresentations("SHAPERSTUDY", aVMList);
  }
}

void SHAPERGUI::fillPartSetInfoPanel(QtxInfoPanel* theInfoPanel)
{
  QIntList aShaperActions = shaperActions();
  theInfoPanel->addLabel(tr("Current mode: Part set mode"));

  addActionsToInfoGroup(theInfoPanel, tr("Parts management"),
    { "Part", "Duplicate", "Remove" });
  addActionsToInfoGroup(theInfoPanel, tr("Import operations"),
    { "OPEN_CMD", "IMPORT_PART_CMD", "IMPORT_SHAPE_CMD" });
  addActionsToInfoGroup(theInfoPanel, tr("Export operations"),
    { "SAVEAS_CMD", "EXPORT_PART_CMD", "EXPORT_SHAPE_CMD" });
  addActionsToInfoGroup(theInfoPanel, tr("Arrangement of parts"),
    { "Placement", "Translation", "Rotation" });
}

void SHAPERGUI::fillPartInfoPanel(QtxInfoPanel* theInfoPanel)
{
  QIntList aShaperActions = shaperActions();
  theInfoPanel->addLabel(tr("Current mode: Part mode"));

  addActionsToInfoGroup(theInfoPanel, tr("Primitives"),
    { "Box", "Cylinder", "Sphere" });
  addActionsToInfoGroup(theInfoPanel, tr("Geometry"),
    { "Vertex", "Edge", "Wire", "Face" });
  addActionsToInfoGroup(theInfoPanel, tr("Features"),
    { "Extrusion", "Revolution", "Cut", "Fuse", "Fillet" });
}

void SHAPERGUI::fillSketcherInfoPanel(QtxInfoPanel* theInfoPanel)
{
  QIntList aShaperActions = shaperActions();
  theInfoPanel->addLabel(tr("Current mode: Sketcher mode"));

  addActionsToInfoGroup(theInfoPanel, tr("Primitives"),
    { "SketchPoint", "SketchLine", "SketchCircle", "SketchRectangle" });
  addActionsToInfoGroup(theInfoPanel, tr("Dimensions"),
    { "SketchConstraintLength", "SketchConstraintRadius", "SketchConstraintAngle" });
  addActionsToInfoGroup(theInfoPanel, tr("Constraints"),
    { "SketchConstraintParallel", "SketchConstraintPerpendicular",
    "SketchConstraintEqual", "SketchConstraintCoincidence" });
}

void SHAPERGUI::addActionsToInfoGroup(QtxInfoPanel* theInfoPanel,
  const QString& theGroup, const QSet<QString>& theActions)
{
  QIntList aShaperActions = shaperActions();

  int aGroup = theInfoPanel->addGroup(theGroup);
  int aCount = 0;
  foreach(int aCmd, aShaperActions) {
    QAction* aAction = action(aCmd);
    if (theActions.contains(aAction->data().toString()))
    {
      theInfoPanel->addAction(aAction, aGroup);
      aCount++;
    }
    if (aCount >= theActions.size())
      break;
  }
}

void SHAPERGUI::updateInfoPanel()
{
  LightApp_Application* aApp = dynamic_cast<LightApp_Application*>(application());
  QtxInfoPanel* aInfoPanel = aApp->infoPanel();
  aInfoPanel->clear();
  aInfoPanel->setTitle(tr("Welcome to SHAPER"));

  SessionPtr aMgr = ModelAPI_Session::get();
  QList<DocumentPtr> aDocs;
  DocumentPtr aActiveDoc = aMgr->activeDocument();
  DocumentPtr aModuleDoc = aMgr->moduleDocument();

  XGUI_OperationMgr* aOpMgr = myWorkshop->operationMgr();
  QStringList aOpList = aOpMgr->operationList();
  bool isSketcher = false;
  if (aOpList.size() > 0)
    isSketcher = (aOpList.first() == "Sketch");

  if (isSketcher) // Sketcher mode
    fillSketcherInfoPanel(aInfoPanel);
  else if (aActiveDoc == aModuleDoc) // Part set mode
    fillPartSetInfoPanel(aInfoPanel);
  else
    fillPartInfoPanel(aInfoPanel);
}
