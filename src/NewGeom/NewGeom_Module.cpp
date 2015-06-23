// Copyright (C) 2014-20xx CEA/DEN, EDF R&D


#include "NewGeom_Module.h"
#include "NewGeom_DataModel.h"
#include "NewGeom_OCCSelector.h"
#include <NewGeom_NestedButton.h>

#include <XGUI_Workshop.h>
#include <XGUI_PropertyPanel.h>
#include <XGUI_ContextMenuMgr.h>
#include <XGUI_ObjectsBrowser.h>
#include <XGUI_OperationMgr.h>
#include <XGUI_Displayer.h>

#include <ModuleBase_Operation.h>
#include <ModuleBase_Preferences.h>
#include <ModuleBase_ActionInfo.h>

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
#include <QtxResourceMgr.h>

#include <Config_PropManager.h>
#include <Config_ModuleReader.h>

#include <AIS_ListOfInteractive.hxx>
#include <AIS_ListIteratorOfListOfInteractive.hxx>

#include <QDockWidget>
#include <QAction>
#include <QTimer>


extern "C" {
NewGeom_EXPORT CAM_Module* createModule()
{
  return new NewGeom_Module();
}

NewGeom_EXPORT char* getModuleVersion()
{
  return "0.0";
}
}

/** 
* Class for preferences management
*/
class NewGeom_PrefMgr: public ModuleBase_IPrefMgr
{
public:
  /// Constructor
  /// \param theMgr preferences manager of SALOME
  /// \param theModName name of the module
  NewGeom_PrefMgr(LightApp_Preferences* theMgr, const QString& theModName):myMgr(theMgr), myModName(theModName) {}

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
NewGeom_Module::NewGeom_Module()
    : LightApp_Module("NewGeom"),
      mySelector(0), myIsOpened(0), myPopupMgr(0)
{
  myWorkshop = new XGUI_Workshop(this);
  connect(myWorkshop, SIGNAL(commandStatusUpdated()),
          this, SLOT(onUpdateCommandStatus()));

  myProxyViewer = new NewGeom_SalomeViewer(this);

  ModuleBase_Preferences::setResourceMgr(application()->resourceMgr());
  ModuleBase_Preferences::loadCustomProps();
}

//******************************************************
NewGeom_Module::~NewGeom_Module()
{
}

//******************************************************
void NewGeom_Module::initialize(CAM_Application* theApp)
{
  LightApp_Module::initialize(theApp);
  inspectSalomeModules();

  myWorkshop->startApplication();
  LightApp_Application* anApp = dynamic_cast<LightApp_Application*>(theApp);
  if (anApp)
    connect(anApp, SIGNAL(preferenceResetToDefaults()), this, SLOT(onDefaultPreferences()));
}

//******************************************************
void NewGeom_Module::windows(QMap<int, int>& theWndMap) const
{
  theWndMap.insert(LightApp_Application::WT_PyConsole, Qt::BottomDockWidgetArea);
}

//******************************************************
void NewGeom_Module::viewManagers(QStringList& theList) const
{
  theList.append(OCCViewer_Viewer::Type());
}

//******************************************************
bool NewGeom_Module::activateModule(SUIT_Study* theStudy)
{
  bool isDone = LightApp_Module::activateModule(theStudy);
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
      // the display all results is not necessary anymore, it was commented in XGUI_Workshop,
      // so it should be commented here
      //QTimer::singleShot(1000, myWorkshop, SLOT(displayAllResults()));
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
  if (mySelector && mySelector->viewer()) {
    Handle(AIS_InteractiveContext) aContext = mySelector->viewer()->getAISContext();
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

  return isDone;
}

//******************************************************
bool NewGeom_Module::deactivateModule(SUIT_Study* theStudy)
{
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
void NewGeom_Module::onViewManagerAdded(SUIT_ViewManager* theMgr)
{
  if (!mySelector) {
    mySelector = createSelector(theMgr);
  }
}

//******************************************************
void NewGeom_Module::onViewManagerRemoved(SUIT_ViewManager* theMgr)
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
      }
    }
  }
}

//******************************************************
QtxPopupMgr* NewGeom_Module::popupMgr()
{
  if (!myPopupMgr)
    myPopupMgr = new QtxPopupMgr( 0, this );
  return myPopupMgr;
}

//******************************************************
void NewGeom_Module::onDefaultPreferences()
{
  ModuleBase_Preferences::resetConfig();
  ModuleBase_Preferences::updateResourcesByConfig();

  LightApp_Preferences* pref = preferences();
  if (pref)
    pref->retrieve();
}

//******************************************************
void NewGeom_Module::onUpdateCommandStatus()
{
  getApp()->updateActions();
}

//******************************************************
NewGeom_OCCSelector* NewGeom_Module::createSelector(SUIT_ViewManager* theMgr)
{
  if (theMgr->getType() == OCCViewer_Viewer::Type()) {
    OCCViewer_Viewer* aViewer = static_cast<OCCViewer_Viewer*>(theMgr->getViewModel());
    NewGeom_OCCSelector* aSelector = new NewGeom_OCCSelector(aViewer, getApp()->selectionMgr());
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
CAM_DataModel* NewGeom_Module::createDataModel()
{
  NewGeom_DataModel* aDataModel = new NewGeom_DataModel(this);

  // Calling addComponent() for persistent functionality work in the SalomeApp_Study
  LightApp_Study* aStudy = dynamic_cast<LightApp_Study*>(application()->activeStudy() );
  aStudy->addComponent(aDataModel);

  return aDataModel;
}

QAction* NewGeom_Module::addFeature(const QString& theWBName, const ActionInfo& theInfo)
{
  return addFeature(theWBName,
                    theInfo.id,
                    theInfo.text,
                    theInfo.toolTip,
                    theInfo.icon,
                    theInfo.shortcut,
                    theInfo.checkable);
}

//******************************************************
QAction* NewGeom_Module::addFeature(const QString& theWBName, const QString& theId,
                                    const QString& theTitle, const QString& theTip,
                                    const QIcon& theIcon, const QKeySequence& theKeys,
                                    bool isCheckable)
{
  int aMenu = createMenu(theWBName, -1, -1, 50);
  int aTool = createTool(theWBName);

  int aId = myActionsList.size();
  myActionsList.append(theId);
  SUIT_Desktop* aDesk = application()->desktop();
  int aKeys = 0;
  for (unsigned int i = 0; i < theKeys.count(); i++)
    aKeys += theKeys[i];
  QAction* aAction = createAction(aId, theTip, theIcon, theTitle, theTip, aKeys, aDesk,
                                  isCheckable);
  aAction->setData(theId);
  int aItemId = createMenu(aId, aMenu, -1, 10);
  int aToolId = createTool(aId, aTool);

  return aAction;
}


QAction* NewGeom_Module::addNestedFeature(const QString& theWBName,
                                          const ActionInfo& theInfo,
                                          const QList<QAction*>& theNestedActions)
{
  int aMenu = createMenu(theWBName, -1, -1, 50);
  int aTool = createTool(theWBName);

  int aId = myActionsList.size();
  myActionsList.append(theInfo.id);
  SUIT_Desktop* aDesk = application()->desktop();
  NewGeom_NestedButton* anAction = new NewGeom_NestedButton(aDesk, theNestedActions);
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

  //int aItemId = createMenu(aId, aMenu, -1, 10);
  int aToolId = createTool(anAction, aTool, aId);

  return anAction;
}


//******************************************************
QAction* NewGeom_Module::addDesktopCommand(const QString& theId, const QString& theTitle,
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
void NewGeom_Module::addDesktopMenuSeparator(const char* theMenuSourceText, const int theMenuPosition)
{
  int aMenu = createMenu(tr(theMenuSourceText), -1, -1);
  createMenu(separator(), aMenu, -1, theMenuPosition);
}

//******************************************************
QList<QAction*> NewGeom_Module::commandList() const
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
QStringList NewGeom_Module::commandIdList() const
{
  return myActionsList;
}

//******************************************************
QMainWindow* NewGeom_Module::desktop() const
{
  return application()->desktop();
}

//******************************************************
QString NewGeom_Module::commandId(const QAction* theCmd) const
{
  int aId = actionId(theCmd);
  if (aId < myActionsList.size())
    return myActionsList[aId];
  return QString();
}

//******************************************************
QAction* NewGeom_Module::command(const QString& theId) const
{
  int aId = myActionsList.indexOf(theId);
  if ((aId != -1) && (aId < myActionsList.size())) {
    return action(aId);
  }
  return 0;
}

//******************************************************
void NewGeom_Module::setNestedActions(const QString& theId, const QStringList& theActions)
{
  myNestedActions[theId] = theActions;
}

//******************************************************
QStringList NewGeom_Module::nestedActions(const QString& theId) const
{
  if (myNestedActions.contains(theId))
    return myNestedActions[theId];
  return QStringList();
}

//******************************************************
void NewGeom_Module::setDocumentKind(const QString& theId, const QString& theKind)
{
  myDocumentType[theId] = theKind;
}

//******************************************************
QString NewGeom_Module::documentKind(const QString& theId) const
{
  if (myDocumentType.contains(theId))
    return myDocumentType[theId];
  return QString();

}

//******************************************************
void NewGeom_Module::selectionChanged()
{
  LightApp_Module::selectionChanged();
  myWorkshop->salomeViewerSelectionChanged();
}

//******************************************************
void NewGeom_Module::contextMenuPopup(const QString& theClient, QMenu* theMenu, QString& theTitle)
{
  myWorkshop->contextMenuMgr()->addViewerMenu(theMenu);
  LightApp_Module::contextMenuPopup(theClient, theMenu, theTitle);
}


//******************************************************
void NewGeom_Module::createPreferences()
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
  NewGeom_PrefMgr aMgr(pref, aModName);
  ModuleBase_Preferences::createEditContent(&aMgr, catId);
  pref->retrieve();
}

//******************************************************
void NewGeom_Module::preferencesChanged(const QString& theSection, const QString& theParam)
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

}

void NewGeom_Module::inspectSalomeModules()
{
  QStringList aModuleNames;
  getApp()->modules(aModuleNames, false);
  foreach(QString eachModule, aModuleNames) {
    Config_ModuleReader::addDependencyModule(eachModule.toStdString());
  }
}
