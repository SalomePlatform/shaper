// Copyright (C) 2014-20xx CEA/DEN, EDF R&D


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
  SHAPERGUI_PrefMgr(LightApp_Preferences* theMgr, const QString& theModName):myMgr(theMgr), myModName(theModName) {}

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
  inspectSalomeModules();

  myWorkshop->startApplication();
  LightApp_Application* anApp = dynamic_cast<LightApp_Application*>(theApp);
  if (anApp)
  {
    connect(anApp, SIGNAL(preferenceResetToDefaults()), this, SLOT(onDefaultPreferences()));
  }
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
void SHAPERGUI::connectToStudy(CAM_Study* theStudy)
{
  // if there are created viewer managers, we should try to create viewer
  // selector and initialize viewer with it. It sets interactive contect to the 
  // proxy viewer. If study is opened, CAM application calls this method before the open() of data model
  // the SHAPER data model is specific and during open(load) redisplay signals are flushed, so
  // we need to connect to the viewer before it. Here, it seems the most appropriate place for this
  // according to SALOME architecture.
  if (!mySelector) {
    ViewManagerList OCCViewManagers;
    application()->viewManagers(OCCViewer_Viewer::Type(), OCCViewManagers);
    if (OCCViewManagers.size() > 0) {
      mySelector = createSelector(OCCViewManagers.first());
    }
  }
  LightApp_Module::connectToStudy(theStudy);
}

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
        if (anAISIO.Access() == aAIS.Access()) {
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

  //myWorkshop->contextMenuMgr()->disconnectViewer();

  SUIT_ResourceMgr* aResMgr = application()->resourceMgr();
  aResMgr->setValue("Study", "store_positions", myIsStorePositions);
  getApp()->setEditEnabled(myIsEditEnabled);

  return LightApp_Module::deactivateModule(theStudy);
}

//******************************************************
void SHAPERGUI::onViewManagerAdded(SUIT_ViewManager* theMgr)
{
  if (!mySelector) {
    mySelector = createSelector(theMgr);
    myWorkshop->module()->activateSelectionFilters();
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
void SHAPERGUI::onUpdateCommandStatus()
{
  getApp()->updateActions();
}

//******************************************************
SHAPERGUI_OCCSelector* SHAPERGUI::createSelector(SUIT_ViewManager* theMgr)
{
  if (theMgr->getType() == OCCViewer_Viewer::Type()) {
    OCCViewer_Viewer* aViewer = static_cast<OCCViewer_Viewer*>(theMgr->getViewModel());
    SHAPERGUI_OCCSelector* aSelector = new SHAPERGUI_OCCSelector(aViewer, getApp()->selectionMgr());
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
                               const bool isAddSeparator,
                               bool isMenuAction, bool isToolAction)
{
  return addFeature(theWBName,
                    theInfo.id,
                    theInfo.text,
                    theInfo.toolTip,
                    theInfo.icon,
                    theInfo.shortcut,
                    theInfo.checkable,
                    isAddSeparator, isMenuAction, isToolAction);
}

//******************************************************
QAction* SHAPERGUI::addFeature(const QString& theWBName, const QString& theId,
                               const QString& theTitle, const QString& theTip,
                               const QIcon& theIcon, const QKeySequence& theKeys,
                               bool isCheckable, const bool isAddSeparator,
                               bool isMenuAction, bool isToolAction)
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
  for (unsigned int i = 0; i < theKeys.count(); i++)
    aKeys += theKeys[i];
  QAction* aAction = createAction(aId, theTip, theIcon, theTitle, theTip, aKeys, aDesk,
                                  isCheckable);
  aAction->setData(theId);

  if (isMenuAction) {
    int aWBMenu = createMenu(theWBName, -1, -1, 50/*10-Window, 1000 - Help*/);
    int aItemId = createMenu(aId, aWBMenu);
    if (isAddSeparator)
      createMenu(separator(), aWBMenu);
  }
  if (isToolAction) {
    int aWBTool = createTool(theWBName, theWBName);
    int aToolId = createTool(aId, aWBTool);
    if (isAddSeparator)
      createTool(separator(), aWBTool);
  }
  return aAction;
}

bool SHAPERGUI::isFeatureOfNested(const QAction* theAction)
{
  return dynamic_cast<const SHAPERGUI_NestedButton*>(theAction);
}

QAction* SHAPERGUI::addFeatureOfNested(const QString& theWBName,
                                       const ActionInfo& theInfo,
                                       const QList<QAction*>& theNestedActions,
                                       bool isMenuAction, bool isToolAction)
{
  int aId = myActionsList.size();
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

  if (isMenuAction) {
    int aWBMenu = createMenu(theWBName, -1, -1, 50);
    int aItemId = createMenu(anAction, aWBMenu);
  }
  if (isToolAction) {
    int aWBTool = createTool(theWBName, theWBName);
    int aToolId = createTool(anAction, aWBTool);
  }
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
  for (unsigned int i = 0; i < theKeys.count(); i++)
    aKeys += theKeys[i];
  QAction* aAction = createAction(aId, theTip, theIcon, theTitle, theTip, aKeys, aDesk,
                                  isCheckable);
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
QStringList SHAPERGUI::commandIdList() const
{
  return myActionsList;
}

//******************************************************
QMainWindow* SHAPERGUI::desktop() const
{
  return application()->desktop();
}

//******************************************************
QString SHAPERGUI::commandId(const QAction* theCmd) const
{
  int aId = actionId(theCmd);
  if (aId < myActionsList.size())
    return myActionsList[aId];
  return QString();
}

//******************************************************
QAction* SHAPERGUI::command(const QString& theId) const
{
  int aId = myActionsList.indexOf(theId);
  if ((aId != -1) && (aId < myActionsList.size())) {
    return action(aId);
  }
  return 0;
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
  int otherGroup = pref->addItem(tr("Other parameters"), viewTab);
  int selId = pref->addItem(tr("Default selection type"), otherGroup, 
                                     SUIT_PreferenceMgr::Selector,
                                     ModuleBase_Preferences::VIEWER_SECTION, "selection");
  QStringList aSelectionList;
  aSelectionList.append( tr("Vertices") );
  aSelectionList.append( tr("Edges") );
  aSelectionList.append( tr("Faces") );
  aSelectionList.append( tr("Results") );

  QList<QVariant> anIndexesList;
  anIndexesList.append(TopAbs_VERTEX);
  anIndexesList.append(TopAbs_EDGE);
  anIndexesList.append(TopAbs_FACE);
  anIndexesList.append(-1);

  pref->setItemProperty( "strings", aSelectionList, selId );
  pref->setItemProperty( "indexes", anIndexesList, selId );
  
  pref->retrieve();
}

//******************************************************
void SHAPERGUI::preferencesChanged(const QString& theSection, const QString& theParam)
{
  SUIT_ResourceMgr* aResMgr = application()->resourceMgr();
  QString aVal = aResMgr->stringValue(theSection, theParam);
  Config_Prop* aProp = Config_PropManager::findProp(theSection.toStdString(), theParam.toStdString());
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

void SHAPERGUI::inspectSalomeModules()
{
  QStringList aModuleNames;
  getApp()->modules(aModuleNames, false);
  foreach(QString eachModule, aModuleNames) {
    Config_ModuleReader::addDependencyModule(eachModule.toStdString());
  }
}

bool SHAPERGUI::abortAllOperations()
{
  return workshop()->operationMgr()->abortAllOperations();
}

void SHAPERGUI::createFeatureActions()
{
  myWorkshop->menuMgr()->createFeatureActions();
}
