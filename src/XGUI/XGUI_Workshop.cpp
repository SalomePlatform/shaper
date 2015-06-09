// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

//#include "XGUI_Constants.h"
#include "XGUI_Tools.h"
#include "XGUI_Workshop.h"
#include "XGUI_SelectionMgr.h"
#include "XGUI_Selection.h"
#include "XGUI_ObjectsBrowser.h"
#include "XGUI_Displayer.h"
#include "XGUI_OperationMgr.h"
#include "XGUI_SalomeConnector.h"
#include "XGUI_ActionsMgr.h"
#include "XGUI_ErrorDialog.h"
#include "XGUI_ColorDialog.h"
#include "XGUI_ViewerProxy.h"
#include "XGUI_PropertyPanel.h"
#include "XGUI_ContextMenuMgr.h"
#include "XGUI_ModuleConnector.h"
#include <XGUI_QtEvents.h>
#include <XGUI_HistoryMenu.h>
#include <XGUI_CustomPrs.h>

#include <AppElements_Workbench.h>
#include <AppElements_Viewer.h>
#include <AppElements_Command.h>
#include <AppElements_MainMenu.h>
#include <AppElements_MainWindow.h>
#include <AppElements_MenuGroupPanel.h>
#include <AppElements_Button.h>

#include <ModuleBase_IModule.h>
#include <ModuleBase_Preferences.h>

#include <ModelAPI_Events.h>
#include <ModelAPI_Session.h>
#include <ModelAPI_Feature.h>
#include <ModelAPI_Data.h>
#include <ModelAPI_AttributeDocRef.h>
#include <ModelAPI_Object.h>
#include <ModelAPI_Validator.h>
#include <ModelAPI_ResultGroup.h>
#include <ModelAPI_ResultConstruction.h>
#include <ModelAPI_ResultBody.h>
#include <ModelAPI_AttributeIntArray.h>
#include <ModelAPI_ResultParameter.h>

//#include <PartSetPlugin_Part.h>

#include <Events_Loop.h>
#include <Events_Error.h>
#include <Events_LongOp.h>

#include <ModuleBase_Operation.h>
#include <ModuleBase_Operation.h>
#include <ModuleBase_OperationDescription.h>
#include <ModuleBase_SelectionValidator.h>
#include <ModuleBase_WidgetFactory.h>
#include <ModuleBase_Tools.h>
#include <ModuleBase_IViewer.h>
#include <ModuleBase_FilterFactory.h>
#include <ModuleBase_PageBase.h>
#include <ModuleBase_Tools.h>

#include <Config_Common.h>
#include <Config_FeatureMessage.h>
#include <Config_PointerMessage.h>
#include <Config_ModuleReader.h>
#include <Config_PropManager.h>
#include <Config_SelectionFilterMessage.h>

#include <QApplication>
#include <QFileDialog>
#include <QMessageBox>
#include <QMdiSubWindow>
#include <QPushButton>
#include <QDockWidget>
#include <QLayout>
#include <QThread>
#include <QObject>
#include <QMenu>
#include <QToolButton>
#include <QAction>

#ifdef _DEBUG
#include <QDebug>
#include <iostream>
#endif

#ifdef WIN32
#include <windows.h>
#else
#include <dlfcn.h>
#endif

//#define DEBUG_FEATURE_CREATED
//#define DEBUG_FEATURE_REDISPLAY
//#define DEBUG_DELETE

XGUI_Workshop::XGUI_Workshop(XGUI_SalomeConnector* theConnector)
    : QObject(),
      myCurrentDir(QString()),
      myModule(NULL),
      mySalomeConnector(theConnector),
      myPropertyPanel(0),
      myObjectBrowser(0),
      myDisplayer(0),
      myUpdatePrefs(false),
      myPartActivating(false),
      myIsLoadingData(false)
{
  myMainWindow = mySalomeConnector ? 0 : new AppElements_MainWindow();

  myDisplayer = new XGUI_Displayer(this);

  mySelector = new XGUI_SelectionMgr(this);
  //connect(mySelector, SIGNAL(selectionChanged()), this, SLOT(updateModuleCommands()));

  myOperationMgr = new XGUI_OperationMgr(this);
  myActionsMgr = new XGUI_ActionsMgr(this);
  myErrorDlg = new XGUI_ErrorDialog(myMainWindow);
  myContextMenuMgr = new XGUI_ContextMenuMgr(this);
  connect(myContextMenuMgr, SIGNAL(actionTriggered(const QString&, bool)), this,
          SLOT(onContextMenuCommand(const QString&, bool)));

  myViewerProxy = new XGUI_ViewerProxy(this);
  connect(myViewerProxy, SIGNAL(selectionChanged()),
          myActionsMgr,  SLOT(updateOnViewSelection()));

  myModuleConnector = new XGUI_ModuleConnector(this);

  connect(myOperationMgr, SIGNAL(operationStarted(ModuleBase_Operation*)), 
          SLOT(onOperationStarted(ModuleBase_Operation*)));
  connect(myOperationMgr, SIGNAL(operationResumed(ModuleBase_Operation*)),
          SLOT(onOperationResumed(ModuleBase_Operation*)));
  connect(myOperationMgr, SIGNAL(operationStopped(ModuleBase_Operation*)),
          SLOT(onOperationStopped(ModuleBase_Operation*)));
  connect(myOperationMgr, SIGNAL(operationCommitted(ModuleBase_Operation*)), 
          SLOT(onOperationCommitted(ModuleBase_Operation*)));
  connect(myOperationMgr, SIGNAL(operationAborted(ModuleBase_Operation*)), 
          SLOT(onOperationAborted(ModuleBase_Operation*)));
  connect(myMainWindow, SIGNAL(exitKeySequence()), SLOT(onExit()));
  connect(this, SIGNAL(errorOccurred(const QString&)), myErrorDlg, SLOT(addError(const QString&)));
}

//******************************************************
XGUI_Workshop::~XGUI_Workshop(void)
{
  delete myDisplayer;
}

//******************************************************
void XGUI_Workshop::startApplication()
{
  initMenu();

  Config_PropManager::registerProp("Plugins", "default_path", "Default Path",
                                   Config_Prop::Directory, "");

  //Initialize event listening
  Events_Loop* aLoop = Events_Loop::loop();
  aLoop->registerListener(this, Events_Error::errorID());  //!< Listening application errors.
  aLoop->registerListener(this, Events_Loop::eventByName(Config_FeatureMessage::GUI_EVENT()));
  aLoop->registerListener(this, Events_Loop::eventByName(EVENT_OPERATION_LAUNCHED));
  aLoop->registerListener(this, Events_Loop::eventByName(EVENT_OBJECT_UPDATED));
  aLoop->registerListener(this, Events_Loop::eventByName(EVENT_OBJECT_CREATED));
  aLoop->registerListener(this, Events_Loop::eventByName(EVENT_OBJECT_TO_REDISPLAY));
  aLoop->registerListener(this, Events_LongOp::eventID());
  aLoop->registerListener(this, Events_Loop::eventByName(EVENT_PLUGIN_LOADED));
  aLoop->registerListener(this, Events_Loop::eventByName(EVENT_SELFILTER_LOADED));

  aLoop->registerListener(this, Events_Loop::eventByName(EVENT_UPDATE_VIEWER_BLOCKED));
  aLoop->registerListener(this, Events_Loop::eventByName(EVENT_UPDATE_VIEWER_UNBLOCKED));

  registerValidators();

  // Calling of  loadCustomProps before activating module is required
  // by Config_PropManger to restore user-defined path to plugins
  ModuleBase_Preferences::loadCustomProps();
  activateModule();
  if (myMainWindow) {
    myMainWindow->show();
    updateCommandStatus();
  }
  
  onNew();

  emit applicationStarted();
}

//******************************************************
void XGUI_Workshop::initMenu()
{
  myContextMenuMgr->createActions();

  if (isSalomeMode()) {
    // Create only Undo, Redo commands
    QAction* aAction = salomeConnector()->addDesktopCommand("UNDO_CMD", tr("Undo"),
                                                         tr("Undo last command"),
                                                         QIcon(":pictures/undo.png"),
                                                         QKeySequence::Undo, false, "MEN_DESK_EDIT");
    connect(aAction, SIGNAL(triggered(bool)), this, SLOT(onUndo()));
    addHistoryMenu(aAction, SIGNAL(updateUndoHistory(const QList<ActionInfo>&)), SLOT(onUndo(int)));

    aAction = salomeConnector()->addDesktopCommand("REDO_CMD", tr("Redo"), tr("Redo last command"),
                                                QIcon(":pictures/redo.png"), QKeySequence::Redo,
                                                false, "MEN_DESK_EDIT");
    connect(aAction, SIGNAL(triggered(bool)), this, SLOT(onRedo()));
    addHistoryMenu(aAction, SIGNAL(updateRedoHistory(const QList<ActionInfo>&)), SLOT(onRedo(int)));

    salomeConnector()->addDesktopMenuSeparator("MEN_DESK_EDIT");
    aAction = salomeConnector()->addDesktopCommand("REBUILD_CMD", tr("Rebuild"), tr("Rebuild data objects"),
                                                QIcon(":pictures/rebuild.png"), QKeySequence(),
                                                false, "MEN_DESK_EDIT");
    connect(aAction, SIGNAL(triggered(bool)), this, SLOT(onRebuild()));
    salomeConnector()->addDesktopMenuSeparator("MEN_DESK_EDIT");

    aAction = salomeConnector()->addDesktopCommand("SAVEAS_CMD", tr("Export NewGeom..."), tr("Export the current document into a NewGeom file"),
                                                QIcon(), QKeySequence(),
                                                false, "MEN_DESK_FILE");
    connect(aAction, SIGNAL(triggered(bool)), this, SLOT(onSaveAs()));

    aAction = salomeConnector()->addDesktopCommand("OPEN_CMD", tr("Import NewGeom..."), tr("Import a NewGeom file"),
                                                QIcon(), QKeySequence(),
                                                false, "MEN_DESK_FILE");
    connect(aAction, SIGNAL(triggered(bool)), this, SLOT(onOpen()));
    salomeConnector()->addDesktopMenuSeparator("MEN_DESK_FILE");

    return;
  }
  // File commands group
  AppElements_MenuGroupPanel* aGroup = myMainWindow->menuObject()->generalPage();

  AppElements_Command* aCommand;

  aCommand = aGroup->addFeature("SAVE_CMD", tr("Save..."), tr("Save the document"),
                                QIcon(":pictures/save.png"), QKeySequence::Save);
  aCommand->connectTo(this, SLOT(onSave()));
  //aCommand->disable();

  QString aUndoId = "UNDO_CMD";
  aCommand = aGroup->addFeature(aUndoId, tr("Undo"), tr("Undo last command"),
                                QIcon(":pictures/undo.png"), QKeySequence::Undo);
  aCommand->connectTo(this, SLOT(onUndo()));
  AppElements_Button* aUndoButton = qobject_cast<AppElements_Button*>(aGroup->widget(aUndoId));
  addHistoryMenu(aUndoButton,
                 SIGNAL(updateUndoHistory(const QList<ActionInfo>&)),
                 SLOT(onUndo(int)));

  QString aRedoId = "REDO_CMD";
  aCommand = aGroup->addFeature(aRedoId, tr("Redo"), tr("Redo last command"),
                                QIcon(":pictures/redo.png"), QKeySequence::Redo);
  aCommand->connectTo(this, SLOT(onRedo()));
  AppElements_Button* aRedoButton = qobject_cast<AppElements_Button*>(aGroup->widget(aRedoId));
  addHistoryMenu(aRedoButton,
                 SIGNAL(updateRedoHistory(const QList<ActionInfo>&)),
                 SLOT(onRedo(int)));

  aCommand = aGroup->addFeature("REBUILD_CMD", tr("Rebuild"), tr("Rebuild data objects"),
    QIcon(":pictures/rebuild.png"), QKeySequence());
  aCommand->connectTo(this, SLOT(onRebuild()));

  aCommand = aGroup->addFeature("SAVEAS_CMD", tr("Save as..."), tr("Save the document into a file"),
                                QIcon(":pictures/save.png"), QKeySequence());
  aCommand->connectTo(this, SLOT(onSaveAs()));
  //aCommand->disable();

  aCommand = aGroup->addFeature("OPEN_CMD", tr("Open..."), tr("Open a new document"),
                                QIcon(":pictures/open.png"), QKeySequence::Open);
  aCommand->connectTo(this, SLOT(onOpen()));

  //aCommand = aGroup->addFeature("NEW_CMD", tr("New"), tr("Create a new document"),
  //                              QIcon(":pictures/new.png"), QKeySequence::New);
  //aCommand->connectTo(this, SLOT(onNew()));

  aCommand = aGroup->addFeature("PREF_CMD", tr("Preferences"), tr("Edit preferences"),
                                QIcon(":pictures/preferences.png"), QKeySequence::Preferences);
  aCommand->connectTo(this, SLOT(onPreferences()));

  aCommand = aGroup->addFeature("EXIT_CMD", tr("Exit"), tr("Exit application"),
                                QIcon(":pictures/close.png"), QKeySequence::Close);
  aCommand->connectTo(this, SLOT(onExit()));
  //FIXME: SBH's test action. Can be used for some GUI tests.
//  #ifdef _DEBUG
//    aCommand = aGroup->addFeature("TEST_CMD", "Test!", "Private debug button",
//                                  QIcon(":pictures/close.png"), QKeySequence(), true);
//    aCommand->connectTo(myMainWindow, SLOT(dockPythonConsole()));
//  #endif
}

//******************************************************
AppElements_Workbench* XGUI_Workshop::addWorkbench(const QString& theName)
{
  AppElements_MainMenu* aMenuBar = myMainWindow->menuObject();
  return aMenuBar->addWorkbench(theName);
}

//******************************************************
void XGUI_Workshop::processEvent(const std::shared_ptr<Events_Message>& theMessage)
{
  if (QApplication::instance()->thread() != QThread::currentThread()) {
    #ifdef _DEBUG
    std::cout << "XGUI_Workshop::processEvent: " << "Working in another thread." << std::endl;
    #endif
    SessionPtr aMgr = ModelAPI_Session::get();
    PostponeMessageQtEvent* aPostponeEvent = new PostponeMessageQtEvent(theMessage);
    QApplication::postEvent(this, aPostponeEvent);
    return;
  }

  //A message to start feature creation received.
  if (theMessage->eventID() == Events_Loop::loop()->eventByName(Config_FeatureMessage::GUI_EVENT())) {
    std::shared_ptr<Config_FeatureMessage> aFeatureMsg =
       std::dynamic_pointer_cast<Config_FeatureMessage>(theMessage);
    if (!aFeatureMsg->isInternal()) {
      addFeature(aFeatureMsg);
    }
  }
  // Process creation of Part
  else if (theMessage->eventID() == Events_Loop::loop()->eventByName(EVENT_OBJECT_CREATED)) {
    std::shared_ptr<ModelAPI_ObjectUpdatedMessage> aUpdMsg =
        std::dynamic_pointer_cast<ModelAPI_ObjectUpdatedMessage>(theMessage);
    onFeatureCreatedMsg(aUpdMsg);
    if (myUpdatePrefs) {
      if (mySalomeConnector)
        mySalomeConnector->createPreferences();
      myUpdatePrefs = false;
    }
  }
  else if (theMessage->eventID() == Events_Loop::loop()->eventByName(EVENT_PLUGIN_LOADED)) {
    myUpdatePrefs = true;
  }
  // Redisplay feature
  else if (theMessage->eventID() == Events_Loop::loop()->eventByName(EVENT_OBJECT_TO_REDISPLAY)) {
    std::shared_ptr<ModelAPI_ObjectUpdatedMessage> aUpdMsg =
        std::dynamic_pointer_cast<ModelAPI_ObjectUpdatedMessage>(theMessage);
    onFeatureRedisplayMsg(aUpdMsg);
  }
  //Update property panel on corresponding message. If there is no current operation (no
  //property panel), or received message has different feature to the current - do nothing.
  else if (theMessage->eventID() == Events_Loop::loop()->eventByName(EVENT_OBJECT_UPDATED)) {
    std::shared_ptr<ModelAPI_ObjectUpdatedMessage> anUpdateMsg =
        std::dynamic_pointer_cast<ModelAPI_ObjectUpdatedMessage>(theMessage);
    onFeatureUpdatedMsg(anUpdateMsg);
  } else if (theMessage->eventID() == Events_LongOp::eventID()) {
    if (Events_LongOp::isPerformed()) {
      QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    } else {
      QApplication::restoreOverrideCursor();
    }
  }
  //An operation passed by message. Start it, process and commit.
  else if (theMessage->eventID() == Events_Loop::loop()->eventByName(EVENT_OPERATION_LAUNCHED)) {
    std::shared_ptr<Config_PointerMessage> aPartSetMsg =
        std::dynamic_pointer_cast<Config_PointerMessage>(theMessage);
    //myPropertyPanel->cleanContent();
    ModuleBase_Operation* anOperation = (ModuleBase_Operation*) aPartSetMsg->pointer();

    if (myOperationMgr->startOperation(anOperation)) {
      myPropertyPanel->updateContentWidget(anOperation->feature());
      if (!anOperation->getDescription()->hasXmlRepresentation()) {
        if (anOperation->commit())
          updateCommandStatus();
      }
    }
  } 
  else if (theMessage->eventID() == Events_Loop::eventByName(EVENT_SELFILTER_LOADED)) {
    std::shared_ptr<Config_SelectionFilterMessage> aMsg = 
      std::dynamic_pointer_cast<Config_SelectionFilterMessage>(theMessage);
    if (aMsg) {
      ModuleBase_FilterFactory* aFactory = moduleConnector()->selectionFilters();
      if (!aMsg->attributeId().empty()) {
        aFactory->assignFilter(aMsg->selectionFilterId(), aMsg->featureId(), aMsg->attributeId(),
                               aMsg->parameters());
      }
    }
  } else if (theMessage->eventID() == Events_Loop::eventByName(EVENT_UPDATE_VIEWER_BLOCKED)) {
    // the viewer's update context will not happens until viewer updated is emitted
    myDisplayer->enableUpdateViewer(false);
  } else if (theMessage->eventID() == Events_Loop::eventByName(EVENT_UPDATE_VIEWER_UNBLOCKED)) {
    // the viewer's update context is unblocked, the viewer's update works
    myDisplayer->enableUpdateViewer(true);
    myDisplayer->updateViewer();
  } else {
    //Show error dialog if error message received.
    std::shared_ptr<Events_Error> anAppError = std::dynamic_pointer_cast<Events_Error>(theMessage);
    if (anAppError) {
      emit errorOccurred(QString::fromLatin1(anAppError->description()));
    }
    return;
  }
  if (!isSalomeMode()) {
    SessionPtr aMgr = ModelAPI_Session::get();
    if (aMgr->isModified() != myMainWindow->isModifiedState())
      myMainWindow->setModifiedState(aMgr->isModified());
  }
}

//******************************************************
QMainWindow* XGUI_Workshop::desktop() const
{
  return isSalomeMode() ? salomeConnector()->desktop() : myMainWindow;
}

//******************************************************
void XGUI_Workshop::onStartWaiting()
{
  if (Events_LongOp::isPerformed()) {
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
  }
}

//******************************************************
void XGUI_Workshop::onFeatureUpdatedMsg(const std::shared_ptr<ModelAPI_ObjectUpdatedMessage>& theMsg)
{
  std::set<ObjectPtr> aFeatures = theMsg->objects();
  if (myOperationMgr->hasOperation()) {
    FeaturePtr aCurrentFeature = myOperationMgr->currentOperation()->feature();
    std::set<ObjectPtr>::const_iterator aIt;
    for (aIt = aFeatures.begin(); aIt != aFeatures.end(); ++aIt) {
      ObjectPtr aNewFeature = (*aIt);
      if (aNewFeature == aCurrentFeature) {
        myPropertyPanel->updateContentWidget(aCurrentFeature);
        break;
      }
    }
  }
  myOperationMgr->onValidateOperation();
  //if (myObjectBrowser)
  //  myObjectBrowser->processEvent(theMsg);
}

//******************************************************
void XGUI_Workshop::onFeatureRedisplayMsg(const std::shared_ptr<ModelAPI_ObjectUpdatedMessage>& theMsg)
{
  std::set<ObjectPtr> aObjects = theMsg->objects();
  std::set<ObjectPtr>::const_iterator aIt;

#ifdef DEBUG_FEATURE_REDISPLAY
  QStringList anInfo;
  for (aIt = aObjects.begin(); aIt != aObjects.end(); ++aIt) {
    anInfo.append(ModuleBase_Tools::objectInfo((*aIt)));
  }
  QString anInfoStr = anInfo.join(", ");
  qDebug(QString("onFeatureRedisplayMsg: %1, %2").arg(aObjects.size()).arg(anInfoStr).toStdString().c_str());
#endif

  for (aIt = aObjects.begin(); aIt != aObjects.end(); ++aIt) {
    ObjectPtr aObj = (*aIt);

    // Hide the object if it is invalid or concealed one
    bool aHide = !aObj->data() || !aObj->data()->isValid() || 
      aObj->isDisabled() || (!aObj->isDisplayed());
    if (!aHide) { // check that this is not hidden result
      ResultPtr aRes = std::dynamic_pointer_cast<ModelAPI_Result>(aObj);
      aHide = aRes && aRes->isConcealed();
    }
    if (aHide)
      myDisplayer->erase(aObj, false);
    else {
      // Redisplay the visible object or the object of the current operation
      bool isVisibleObject = myDisplayer->isVisible(aObj);
      #ifdef DEBUG_FEATURE_REDISPLAY
      //QString anObjInfo = ModuleBase_Tools::objectInfo((aObj));
      //qDebug(QString("visible=%1 : display= %2").arg(isVisibleObject).arg(anObjInfo).toStdString().c_str());
      #endif

      if (isVisibleObject)  { // redisplay visible object
        //displayObject(aObj);  // In order to update presentation
        // in order to avoid the check whether the object can be redisplayed, the exact method
        // of redisplay is called. This modification is made in order to have the line is updated
        // by creation of a horizontal constraint on the line by preselection
        myDisplayer->redisplay(aObj, false);
        // Deactivate object of current operation from selection
        deactivateActiveObject(aObj, false);
      } else { // display object if the current operation has it
        if (displayObject(aObj)) {
          // Deactivate object of current operation from selection
          deactivateActiveObject(aObj, false);
        }
      }
    }
  }
  myDisplayer->updateViewer();
}

//******************************************************
void XGUI_Workshop::deactivateActiveObject(const ObjectPtr& theObject, const bool theUpdateViewer)
{
  if (!myModule->canActivateSelection(theObject)) {
    if (myDisplayer->isActive(theObject))
      myDisplayer->deactivate(theObject, theUpdateViewer);
  }
}

//******************************************************
void XGUI_Workshop::onFeatureCreatedMsg(const std::shared_ptr<ModelAPI_ObjectUpdatedMessage>& theMsg)
{
  std::set<ObjectPtr> aObjects = theMsg->objects();
  std::set<ObjectPtr>::const_iterator aIt;
#ifdef DEBUG_FEATURE_CREATED
  QStringList anInfo;
  for (aIt = aObjects.begin(); aIt != aObjects.end(); ++aIt) {
    anInfo.append(ModuleBase_Tools::objectInfo((*aIt)));
  }
  QString anInfoStr = anInfo.join(", ");
  qDebug(QString("onFeatureCreatedMsg: %1, %2").arg(aObjects.size()).arg(anInfoStr).toStdString().c_str());
#endif

  //bool aHasPart = false;
  bool isDisplayed = false;
  for (aIt = aObjects.begin(); aIt != aObjects.end(); ++aIt) {
    ObjectPtr anObject = *aIt;
    // the validity of the data should be checked here in order to avoid display of the objects,
    // which were created, then deleted, but flush for the creation event happens after that
    // we should not display disabled objects
    bool aHide = !anObject->data()->isValid() || 
                 anObject->isDisabled() ||
                 !anObject->isDisplayed();
    if (!aHide) {
      // setDisplayed has to be called in order to synchronize internal state of the object 
      // with list of displayed objects
      if (myModule->canDisplayObject(anObject)) {
        anObject->setDisplayed(true);
        isDisplayed = displayObject(*aIt);
      } else 
        anObject->setDisplayed(false);
    }
  }
  //if (myObjectBrowser)
  //  myObjectBrowser->processEvent(theMsg);
  if (isDisplayed)
    myDisplayer->updateViewer();
  //if (aHasPart) { // TODO: Avoid activate last part on loading of document
  //  activateLastPart();
  //}
}

//******************************************************
void XGUI_Workshop::onOperationStarted(ModuleBase_Operation* theOperation)
{
  setNestedFeatures(theOperation);

  if (theOperation->getDescription()->hasXmlRepresentation()) {  //!< No need for property panel
    connectWithOperation(theOperation);
    setPropertyPanel(theOperation);
  }
  updateCommandStatus();

  myModule->operationStarted(theOperation);

  // the objects of the current operation should be deactivated
  QObjectPtrList anObjects;
  FeaturePtr aFeature = theOperation->feature();
  anObjects.append(aFeature);
  std::list<ResultPtr> aResults = aFeature->results();
  std::list<ResultPtr>::const_iterator aIt;
  for (aIt = aResults.begin(); aIt != aResults.end(); ++aIt) {
    anObjects.append(*aIt);
  }
  QObjectPtrList::const_iterator anIt = anObjects.begin(), aLast = anObjects.end();
  for (; anIt != aLast; anIt++)
    deactivateActiveObject(*anIt, false);
  if (anObjects.size() > 0)
    myDisplayer->updateViewer();
}

//******************************************************
void XGUI_Workshop::onOperationResumed(ModuleBase_Operation* theOperation)
{
  setNestedFeatures(theOperation);

  if (theOperation->getDescription()->hasXmlRepresentation()) {  //!< No need for property panel
    // connectWithOperation(theOperation); already connected
    setPropertyPanel(theOperation);
  }
  updateCommandStatus();

  myModule->operationResumed(theOperation);
}


//******************************************************
void XGUI_Workshop::onOperationStopped(ModuleBase_Operation* theOperation)
{
  ModuleBase_ISelection* aSel = mySelector->selection();
  QObjectPtrList aObj = aSel->selectedPresentations();
  //!< No need for property panel
  updateCommandStatus();
  hidePropertyPanel();
  myPropertyPanel->cleanContent();

  myModule->operationStopped(theOperation);

  // the deactivated objects of the current operation should be activated back.
  // They were deactivated on operation start or an object redisplay
  QObjectPtrList anObjects;
  FeaturePtr aFeature = theOperation->feature();
  if (myDisplayer->isVisible(aFeature) && !myDisplayer->isActive(aFeature))
    anObjects.append(aFeature);
  std::list<ResultPtr> aResults = aFeature->results();
  std::list<ResultPtr>::const_iterator aIt;
  for (aIt = aResults.begin(); aIt != aResults.end(); ++aIt) {
    ResultPtr anObject = *aIt;
    if (myDisplayer->isVisible(anObject) && !myDisplayer->isActive(anObject)) {
      anObjects.append(anObject);
    }
  }
  QIntList aModes;
  module()->activeSelectionModes(aModes);
  myDisplayer->activateObjects(aModes, anObjects);
}


void XGUI_Workshop::onOperationCommitted(ModuleBase_Operation* theOperation)
{
  myModule->operationCommitted(theOperation);
}

void XGUI_Workshop::onOperationAborted(ModuleBase_Operation* theOperation)
{
  myModule->operationAborted(theOperation);
}

void XGUI_Workshop::setNestedFeatures(ModuleBase_Operation* theOperation)
{
  if (this->isSalomeMode()) 
    theOperation->setNestedFeatures(mySalomeConnector->nestedActions(theOperation->id()));
  else 
    theOperation->setNestedFeatures(myActionsMgr->nestedCommands(theOperation->id()));
}

void XGUI_Workshop::setPropertyPanel(ModuleBase_Operation* theOperation)
{
  showPropertyPanel();
  QString aXmlRepr = theOperation->getDescription()->xmlRepresentation();
  ModuleBase_WidgetFactory aFactory = ModuleBase_WidgetFactory(aXmlRepr.toStdString(),
                                                                myModuleConnector);

  myPropertyPanel->cleanContent();
  aFactory.createWidget(myPropertyPanel->contentWidget());

  QList<ModuleBase_ModelWidget*> aWidgets = aFactory.getModelWidgets();
  foreach (ModuleBase_ModelWidget* aWidget, aWidgets) {
    bool isStoreValue = !theOperation->isEditOperation() &&
                        !aWidget->getDefaultValue().empty() &&
                        !aWidget->isComputedDefault();
    aWidget->setFeature(theOperation->feature(), isStoreValue);
    aWidget->enableFocusProcessing();
  }
  
  myPropertyPanel->setModelWidgets(aWidgets);
  theOperation->setPropertyPanel(myPropertyPanel);

  myModule->propertyPanelDefined(theOperation);

  myPropertyPanel->setWindowTitle(theOperation->getDescription()->description());
}

bool XGUI_Workshop::event(QEvent * theEvent)
{
  PostponeMessageQtEvent* aPostponedEv = dynamic_cast<PostponeMessageQtEvent*>(theEvent);
  if (aPostponedEv) {
    std::shared_ptr<Events_Message> aEventPtr = aPostponedEv->postponedMessage();
    processEvent(aEventPtr);
    return true;
  }
  return false;
}

/*
 *
 */
void XGUI_Workshop::addFeature(const std::shared_ptr<Config_FeatureMessage>& theMessage)
{
  if (!theMessage) {
#ifdef _DEBUG
    qDebug() << "XGUI_Workshop::addFeature: NULL message.";
#endif
    return;
  }
  ActionInfo aFeatureInfo;
  aFeatureInfo.initFrom(theMessage);

  QString aWchName = QString::fromStdString(theMessage->workbenchId());
  QStringList aNestedFeatures =
      QString::fromStdString(theMessage->nestedFeatures()).split(" ", QString::SkipEmptyParts);
  QString aDocKind = QString::fromStdString(theMessage->documentKind());
  QList<QAction*> aNestedActList;
  bool isColumnButton = !aNestedFeatures.isEmpty();
  if (isColumnButton) {
    QString aNestedActions = QString::fromStdString(theMessage->actionsWhenNested());
    if (aNestedActions.contains("accept")) {
      QAction* anAction = myActionsMgr->operationStateAction(XGUI_ActionsMgr::AcceptAll, NULL);
      connect(anAction, SIGNAL(triggered()), myOperationMgr, SLOT(commitAllOperations()));
      aNestedActList << anAction;
    }
    if (aNestedActions.contains("abort")) {
      QAction* anAction = myActionsMgr->operationStateAction(XGUI_ActionsMgr::AbortAll, NULL);
      connect(anAction, SIGNAL(triggered()), myOperationMgr, SLOT(abortAllOperations()));
      aNestedActList << anAction;
    }
  }

  if (isSalomeMode()) {
    QAction* aAction;
    if (isColumnButton) {
      aAction = salomeConnector()->addNestedFeature(aWchName, aFeatureInfo, aNestedActList);
    } else {
      aAction = salomeConnector()->addFeature(aWchName, aFeatureInfo);
    }
    salomeConnector()->setNestedActions(aFeatureInfo.id, aNestedFeatures);
    salomeConnector()->setDocumentKind(aFeatureInfo.id, aDocKind);

    myActionsMgr->addCommand(aAction);
    myModule->actionCreated(aAction);
  } else {
    //Find or create Workbench
    AppElements_MainMenu* aMenuBar = myMainWindow->menuObject();
    AppElements_Workbench* aPage = aMenuBar->findWorkbench(aWchName);
    if (!aPage) {
      aPage = addWorkbench(aWchName);
    }
    //Find or create Group
    QString aGroupName = QString::fromStdString(theMessage->groupId());
    AppElements_MenuGroupPanel* aGroup = aPage->findGroup(aGroupName);
    if (!aGroup) {
      aGroup = aPage->addGroup(aGroupName);
    }
    // Check if hotkey sequence is already defined:
    QKeySequence aHotKey = myActionsMgr->registerShortcut(aFeatureInfo.shortcut);
    if(aHotKey != aFeatureInfo.shortcut) {
      aFeatureInfo.shortcut = aHotKey;
    }
    // Create feature...
    AppElements_Command* aCommand = aGroup->addFeature(aFeatureInfo,
                                                       aDocKind,
                                                       aNestedFeatures);
    // Enrich created button with accept/abort buttons if necessary
    AppElements_Button* aButton = aCommand->button();
    if (aButton->isColumnButton()) {
      aButton->setAdditionalButtons(aNestedActList);
    }
    myActionsMgr->addCommand(aCommand);
    myModule->actionCreated(aCommand);
  }
}

/*
 * Makes a signal/slot connections between Property Panel
 * and given operation. The given operation becomes a
 * current operation and previous operation if exists
 */
void XGUI_Workshop::connectWithOperation(ModuleBase_Operation* theOperation)
{
  QAction* aCommand = 0;
  if (isSalomeMode()) {
    aCommand = salomeConnector()->command(theOperation->getDescription()->operationId());
  } else {
    AppElements_MainMenu* aMenu = myMainWindow->menuObject();
    FeaturePtr aFeature = theOperation->feature();
    if(aFeature)
      aCommand = aMenu->feature(QString::fromStdString(aFeature->getKind()));
  }
  //Abort operation on uncheck the command
  if (aCommand) {
    connect(aCommand, SIGNAL(triggered(bool)), theOperation, SLOT(setRunning(bool)));
  }
}

/*
 * Saves document with given name.
 */
void XGUI_Workshop::saveDocument(const QString& theName, std::list<std::string>& theFileNames)
{
  QApplication::restoreOverrideCursor();
  SessionPtr aMgr = ModelAPI_Session::get();
  aMgr->save(theName.toLatin1().constData(), theFileNames);
  QApplication::restoreOverrideCursor();
}

bool XGUI_Workshop::isActiveOperationAborted()
{
  return myOperationMgr->abortAllOperations();
}

//******************************************************
void XGUI_Workshop::onExit()
{
  SessionPtr aMgr = ModelAPI_Session::get();
  if (aMgr->isModified()) {
    int anAnswer = QMessageBox::question(
        myMainWindow, tr("Save current file"), tr("The document is modified, save before exit?"),
        QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel, QMessageBox::Cancel);
    if (anAnswer == QMessageBox::Save) {
      bool saved = onSave();
      if (!saved) {
        return;
      }
    } else if (anAnswer == QMessageBox::Cancel) {
      return;
    }
  }
  qApp->exit();
}

//******************************************************
void XGUI_Workshop::onNew()
{
  QApplication::setOverrideCursor(Qt::WaitCursor);
  if (objectBrowser() == 0) {
    createDockWidgets();
    mySelector->connectViewers();
  }
  myViewerProxy->connectToViewer();
  showObjectBrowser();
  if (!isSalomeMode()) {
    myMainWindow->showPythonConsole();
    QMdiSubWindow* aWnd = myMainWindow->viewer()->createView();
    aWnd->showMaximized();
    updateCommandStatus();
  }
  myContextMenuMgr->connectViewer();
  QApplication::restoreOverrideCursor();
}

//******************************************************
void XGUI_Workshop::onOpen()
{
  if(!isActiveOperationAborted())
    return;
  //save current file before close if modified
  SessionPtr aSession = ModelAPI_Session::get();
  if (aSession->isModified()) {
    //TODO(sbh): re-launch the app?
    int anAnswer = QMessageBox::question(
        myMainWindow, tr("Save current file"),
        tr("The document is modified, save before opening another?"),
        QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel, QMessageBox::Cancel);
    if (anAnswer == QMessageBox::Save) {
      onSave();
    } else if (anAnswer == QMessageBox::Cancel) {
      return;
    }
    myCurrentDir = "";
  }

  //show file dialog, check if readable and open
  myCurrentDir = QFileDialog::getExistingDirectory(mainWindow());
  if (myCurrentDir.isEmpty())
    return;
  QFileInfo aFileInfo(myCurrentDir);
  if (!aFileInfo.exists() || !aFileInfo.isReadable()) {
    QMessageBox::critical(myMainWindow, tr("Warning"), tr("Unable to open the file."));
    myCurrentDir = "";
    return;
  }
  QApplication::setOverrideCursor(Qt::WaitCursor);
  myIsLoadingData = true;
  aSession->closeAll();
  aSession->load(myCurrentDir.toLatin1().constData());
  myObjectBrowser->rebuildDataTree();
  //displayAllResults();
  updateCommandStatus();
  myIsLoadingData = false;
  QApplication::restoreOverrideCursor();
}

//******************************************************
bool XGUI_Workshop::onSave()
{
  if(!isActiveOperationAborted())
    return false;
  if (myCurrentDir.isEmpty()) {
    return onSaveAs();
  }
  std::list<std::string> aFiles;
  saveDocument(myCurrentDir, aFiles);
  updateCommandStatus();
  if (!isSalomeMode())
    myMainWindow->setModifiedState(false);
  return true;
}

//******************************************************
bool XGUI_Workshop::onSaveAs()
{
  if(!isActiveOperationAborted())
    return false;
  QFileDialog dialog(mainWindow());
  dialog.setWindowTitle(tr("Select directory to save files..."));
  dialog.setFileMode(QFileDialog::Directory);
  dialog.setFilter(tr("Folders (*)"));
  dialog.setOptions(QFileDialog::HideNameFilterDetails | QFileDialog::ShowDirsOnly);
  dialog.setViewMode(QFileDialog::Detail);

  if (!dialog.exec()) {
    return false;
  }
  QString aTempDir = dialog.selectedFiles().first();
  QDir aDir(aTempDir);
  if (aDir.exists() && !aDir.entryInfoList(QDir::NoDotAndDotDot | QDir::AllEntries).isEmpty()) {
    int answer = QMessageBox::question(
        myMainWindow,
        //: Title of the dialog which asks user if he wants to save study in existing non-empty folder
        tr("Save"),
        tr("The folder already contains some files, save anyway?"),
        QMessageBox::Save | QMessageBox::Cancel);
    if (answer == QMessageBox::Cancel) {
      return false;
    }
  }
  myCurrentDir = aTempDir;
  if (!isSalomeMode()) {
    myMainWindow->setCurrentDir(myCurrentDir, false);
    myMainWindow->setModifiedState(false);
  }
  return onSave();
}

//******************************************************
void XGUI_Workshop::onUndo(int theTimes)
{
  objectBrowser()->treeView()->setCurrentIndex(QModelIndex());
  SessionPtr aMgr = ModelAPI_Session::get();
  if (aMgr->isOperation())
    operationMgr()->onAbortOperation();
  for (int i = 0; i < theTimes; ++i) {
    aMgr->undo();
  }
  updateCommandStatus();
}

//******************************************************
void XGUI_Workshop::onRedo(int theTimes)
{
  // the viewer update should be blocked in order to avoid the features blinking. For the created
  // feature a results are created, the flush of the created signal caused the viewer redisplay for
  // each created result. After a redisplay signal is flushed. So, the viewer update is blocked until
  // redo of all possible objects happens
  bool isUpdateEnabled = myDisplayer->enableUpdateViewer(false);

  objectBrowser()->treeView()->setCurrentIndex(QModelIndex());
  SessionPtr aMgr = ModelAPI_Session::get();
  if (aMgr->isOperation())
    operationMgr()->onAbortOperation();
  for (int i = 0; i < theTimes; ++i) {
    aMgr->redo();
  }
  updateCommandStatus();

  // unblock the viewer update functionality and make update on purpose
  myDisplayer->enableUpdateViewer(isUpdateEnabled);
  myDisplayer->updateViewer();
}

//******************************************************
void XGUI_Workshop::onRebuild()
{
  SessionPtr aMgr = ModelAPI_Session::get();
  bool aWasOperation = aMgr->isOperation(); // keep this value
  if (!aWasOperation) {
    aMgr->startOperation("Rebuild");
  }
  static const Events_ID aRebuildEvent = Events_Loop::loop()->eventByName("Rebuild");
  Events_Loop::loop()->send(std::shared_ptr<Events_Message>(
    new Events_Message(aRebuildEvent, this)));
  if (!aWasOperation) {
    aMgr->finishOperation();
  }
  updateCommandStatus();
}

//******************************************************
void XGUI_Workshop::onPreferences()
{
  ModuleBase_Prefs aModif;
  ModuleBase_Preferences::editPreferences(aModif);
  if (aModif.size() > 0) {
    QString aSection;
    foreach (ModuleBase_Pref aPref, aModif)
    {
      aSection = aPref.first;
      if (aSection == ModuleBase_Preferences::VIEWER_SECTION) {
        if (!isSalomeMode())
          myMainWindow->viewer()->updateFromResources();
      } else if (aSection == ModuleBase_Preferences::MENU_SECTION) {
        if (!isSalomeMode())
          myMainWindow->menuObject()->updateFromResources();
      }
    }
  }
}

//******************************************************
ModuleBase_IModule* XGUI_Workshop::loadModule(const QString& theModule)
{
  QString libName = QString::fromStdString(library(theModule.toStdString()));
  if (libName.isEmpty()) {
    qWarning(qPrintable(tr("Information about module \"%1\" doesn't exist.").arg(theModule)));
    return 0;
  }

  QString err;
  CREATE_FUNC crtInst = 0;

#ifdef WIN32
  HINSTANCE modLib = ::LoadLibrary((LPTSTR) qPrintable(libName));
  if (!modLib) {
    LPVOID lpMsgBuf;
    ::FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        0, ::GetLastError(), 0, (LPTSTR) & lpMsgBuf, 0, 0);
    QString aMsg((char*) &lpMsgBuf);
    err = QString("Failed to load  %1. %2").arg(libName).arg(aMsg);
    ::LocalFree(lpMsgBuf);
  } else {
    crtInst = (CREATE_FUNC) ::GetProcAddress(modLib, CREATE_MODULE);
    if (!crtInst) {
      LPVOID lpMsgBuf;
      ::FormatMessage(
          FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM
              | FORMAT_MESSAGE_IGNORE_INSERTS,
          0, ::GetLastError(), 0, (LPTSTR) & lpMsgBuf, 0, 0);
      QString aMsg((char*) &lpMsgBuf);
      err = QString("Failed to find  %1 function. %2").arg( CREATE_MODULE).arg(aMsg);
      ::LocalFree(lpMsgBuf);
    }
  }
#else
  void* modLib = dlopen( libName.toLatin1(), RTLD_LAZY | RTLD_GLOBAL );
  if ( !modLib ) {
    err = QString( "Can not load library %1. %2" ).arg( libName ).arg( dlerror() );
  } else {
    crtInst = (CREATE_FUNC)dlsym( modLib, CREATE_MODULE );
    if ( !crtInst ) {
      err = QString( "Failed to find function %1. %2" ).arg( CREATE_MODULE ).arg( dlerror() );
    }
  }
#endif

  ModuleBase_IModule* aModule = crtInst ? crtInst(myModuleConnector) : 0;

  if (!err.isEmpty()) {
    if (mainWindow()) {
      Events_Error::send(err.toStdString());
    } else {
      qWarning(qPrintable(err));
    }
  }
  return aModule;
}

//******************************************************
bool XGUI_Workshop::activateModule()
{
  Config_ModuleReader aModuleReader;
  QString moduleName = QString::fromStdString(aModuleReader.getModuleName());
  myModule = loadModule(moduleName);
  if (!myModule)
    return false;

  connect(myDisplayer, SIGNAL(objectDisplayed(ObjectPtr, AISObjectPtr)),
    myModule, SLOT(onObjectDisplayed(ObjectPtr, AISObjectPtr)));
  connect(myDisplayer, SIGNAL(beforeObjectErase(ObjectPtr, AISObjectPtr)),
    myModule, SLOT(onBeforeObjectErase(ObjectPtr, AISObjectPtr)));

  myModule->createFeatures();
  myActionsMgr->update();
  return true;
}

//******************************************************
void XGUI_Workshop::updateCommandStatus()
{
  QList<QAction*> aCommands;
  if (isSalomeMode()) {  // update commands in SALOME mode
    aCommands = salomeConnector()->commandList();
  } else {
    AppElements_MainMenu* aMenuBar = myMainWindow->menuObject();
    foreach (AppElements_Command* aCmd, aMenuBar->features())
      aCommands.append(aCmd);
  }
  SessionPtr aMgr = ModelAPI_Session::get();
  if (aMgr->hasModuleDocument()) {
    QAction *aUndoCmd, *aRedoCmd;
    foreach(QAction* aCmd, aCommands) {
      QString aId = aCmd->data().toString();
      if (aId == "UNDO_CMD")
        aUndoCmd = aCmd;
      else if (aId == "REDO_CMD")
        aRedoCmd = aCmd;
      else
        // Enable all commands
        aCmd->setEnabled(true);
    }

    aUndoCmd->setEnabled(myModule->canUndo());
    aRedoCmd->setEnabled(myModule->canRedo());
    updateHistory();
  } else {
    foreach(QAction* aCmd, aCommands) {
      QString aId = aCmd->data().toString();
      if (aId == "NEW_CMD")
        aCmd->setEnabled(true);
      else if (aId == "EXIT_CMD")
        aCmd->setEnabled(true);
      else
        aCmd->setEnabled(false);
    }
  }
  myActionsMgr->update();
  emit commandStatusUpdated();
}

void XGUI_Workshop::updateHistory()
{
  std::list<std::string> aUndoList = ModelAPI_Session::get()->undoList();
  QList<ActionInfo> aUndoRes = processHistoryList(aUndoList);
  emit updateUndoHistory(aUndoRes);

  std::list<std::string> aRedoList = ModelAPI_Session::get()->redoList();
  QList<ActionInfo> aRedoRes = processHistoryList(aRedoList);
  emit updateRedoHistory(aRedoRes);
}

//******************************************************
QDockWidget* XGUI_Workshop::createObjectBrowser(QWidget* theParent)
{
  QDockWidget* aObjDock = new QDockWidget(theParent);
  aObjDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea | Qt::BottomDockWidgetArea);
  aObjDock->setWindowTitle(tr("Object browser"));
  aObjDock->setStyleSheet(
      "::title { position: relative; padding-left: 5px; text-align: left center }");
  myObjectBrowser = new XGUI_ObjectsBrowser(aObjDock);
  myObjectBrowser->setDataModel(myModule->dataModel());
  myModule->customizeObjectBrowser(myObjectBrowser);
  aObjDock->setWidget(myObjectBrowser);

  myContextMenuMgr->connectObjectBrowser();
  return aObjDock;
}

//******************************************************
/*
 * Creates dock widgets, places them in corresponding area
 * and tabifies if necessary.
 */
void XGUI_Workshop::createDockWidgets()
{
  QMainWindow* aDesktop = isSalomeMode() ? salomeConnector()->desktop() : myMainWindow;
  QDockWidget* aObjDock = createObjectBrowser(aDesktop);
  aDesktop->addDockWidget(Qt::LeftDockWidgetArea, aObjDock);
  myPropertyPanel = new XGUI_PropertyPanel(aDesktop);
  myPropertyPanel->setupActions(myActionsMgr);
  myPropertyPanel->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea | Qt::BottomDockWidgetArea);
  aDesktop->addDockWidget(Qt::LeftDockWidgetArea, myPropertyPanel);
  hidePropertyPanel();  ///<! Invisible by default
  hideObjectBrowser();
  aDesktop->tabifyDockWidget(aObjDock, myPropertyPanel);
  myPropertyPanel->installEventFilter(myOperationMgr);

  QAction* aOkAct = myActionsMgr->operationStateAction(XGUI_ActionsMgr::Accept);
  connect(aOkAct, SIGNAL(triggered()), myOperationMgr, SLOT(onCommitOperation()));
  QAction* aCancelAct = myActionsMgr->operationStateAction(XGUI_ActionsMgr::Abort);
  connect(aCancelAct, SIGNAL(triggered()), myOperationMgr, SLOT(onAbortOperation()));
  connect(myPropertyPanel, SIGNAL(noMoreWidgets()), myModule, SLOT(onNoMoreWidgets()));
  connect(myPropertyPanel, SIGNAL(keyReleased(QKeyEvent*)),
          myOperationMgr,  SLOT(onKeyReleased(QKeyEvent*)));
  connect(myOperationMgr,  SIGNAL(validationStateChanged(bool)),
          aOkAct,          SLOT(setEnabled(bool)));
  QAction* aAcceptAllAct = myActionsMgr->operationStateAction(XGUI_ActionsMgr::AcceptAll);
  connect(myOperationMgr,  SIGNAL(nestedStateChanged(bool)),
          aAcceptAllAct,   SLOT(setEnabled(bool)));

}

//******************************************************
void XGUI_Workshop::showPropertyPanel()
{
  QAction* aViewAct = myPropertyPanel->toggleViewAction();
  ///<! Restore ability to close panel from the window's menu
  aViewAct->setEnabled(true);
  myPropertyPanel->show();
  myPropertyPanel->raise();
}

//******************************************************
void XGUI_Workshop::hidePropertyPanel()
{
  QAction* aViewAct = myPropertyPanel->toggleViewAction();
  ///<! Do not allow to show empty property panel
  aViewAct->setEnabled(false);
  myPropertyPanel->hide();
}

//******************************************************
void XGUI_Workshop::showObjectBrowser()
{
  myObjectBrowser->parentWidget()->show();
}

//******************************************************
void XGUI_Workshop::hideObjectBrowser()
{
  myObjectBrowser->parentWidget()->hide();
}

//******************************************************
//void XGUI_Workshop::onFeatureTriggered()
//{
//  QAction* aCmd = dynamic_cast<QAction*>(sender());
//  if (aCmd) {
//    QString aId = salomeConnector()->commandId(aCmd);
//    if (!aId.isNull())
//      myModule->launchOperation(aId);
//  }
//}

//******************************************************
void XGUI_Workshop::salomeViewerSelectionChanged()
{
  emit salomeViewerSelection();
}

//**************************************************************
ModuleBase_IViewer* XGUI_Workshop::salomeViewer() const
{
  return mySalomeConnector->viewer();
}

//**************************************************************
void XGUI_Workshop::onContextMenuCommand(const QString& theId, bool isChecked)
{
  QObjectPtrList aObjects = mySelector->selection()->selectedObjects();
  if (theId == "DELETE_CMD")
    deleteObjects();
  else if (theId == "COLOR_CMD")
    changeColor(aObjects);
  else if (theId == "SHOW_CMD")
    showObjects(aObjects, true);
  else if (theId == "HIDE_CMD")
    showObjects(aObjects, false);
  else if (theId == "SHOW_ONLY_CMD")
    showOnlyObjects(aObjects);
  else if (theId == "SHADING_CMD")
    setDisplayMode(aObjects, XGUI_Displayer::Shading);
  else if (theId == "WIREFRAME_CMD")
    setDisplayMode(aObjects, XGUI_Displayer::Wireframe);
  else if (theId == "HIDEALL_CMD") {
    QObjectPtrList aList = myDisplayer->displayedObjects();
    foreach (ObjectPtr aObj, aList)
      aObj->setDisplayed(false);
    Events_Loop::loop()->flush(Events_Loop::eventByName(EVENT_OBJECT_TO_REDISPLAY));
  }
}

//**************************************************************
void XGUI_Workshop::deleteObjects()
{
  ModuleBase_IModule* aModule = module();
  // 1. allow the module to delete objects, do nothing if it has succeed
  if (aModule->deleteObjects()) {
    updateCommandStatus();
    return;
  }

  if (!isActiveOperationAborted())
    return;
  QObjectPtrList anObjects = mySelector->selection()->selectedObjects();
  bool hasResult = false;
  bool hasFeature = false;
  bool hasParameter = false;
  ModuleBase_Tools::checkObjects(anObjects, hasResult, hasFeature, hasParameter);
  if (!(hasFeature || hasParameter))
    return;

  // 1. start operation
  QString aDescription = contextMenuMgr()->action("DELETE_CMD")->text();
  aDescription += tr(" %1");
  QStringList aObjectNames;
  foreach (ObjectPtr aObj, anObjects) {
    if (!aObj->data()->isValid())
      continue;
    aObjectNames << QString::fromStdString(aObj->data()->name());
  }
  aDescription = aDescription.arg(aObjectNames.join(", "));

  SessionPtr aMgr = ModelAPI_Session::get();
  aMgr->startOperation(aDescription.toStdString());
  // 2. close the documents of the removed parts if the result part is in a list of selected objects
  // this is performed in the RemoveFeature of Part object.
  /*foreach (ObjectPtr aObj, anObjects)
  {
    ResultPartPtr aPart = std::dynamic_pointer_cast<ModelAPI_ResultPart>(aObj);
    if (aPart) {
      DocumentPtr aDoc = aObj->document();
      if (aDoc == aMgr->activeDocument()) {
        aDoc->close();
      }
    }
  }*/
  // 3. delete objects
  QMainWindow* aDesktop = isSalomeMode() ? salomeConnector()->desktop() : myMainWindow;
  std::set<FeaturePtr> anIgnoredFeatures;
  if (deleteFeatures(anObjects, anIgnoredFeatures, aDesktop, true)) {
    myDisplayer->updateViewer();
    aMgr->finishOperation();
    updateCommandStatus();
  }
  else {
    aMgr->abortOperation();
  }
}

//**************************************************************
bool XGUI_Workshop::deleteFeatures(const QObjectPtrList& theList,
                                   std::set<FeaturePtr> theIgnoredFeatures,
                                   QWidget* theParent,
                                   const bool theAskAboutDeleteReferences)
{
  // 1. find all referenced features
  std::set<FeaturePtr> aRefFeatures;
  foreach (ObjectPtr aDeletedObj, theList) {
    FeaturePtr aDeletedFeature = ModelAPI_Feature::feature(aDeletedObj);
    if (aDeletedFeature.get() != NULL) {
      DocumentPtr aDeletedFeatureDoc = aDeletedObj->document();
      // 1.1 find references in the current document
      aDeletedFeatureDoc->refsToFeature(aDeletedFeature, aRefFeatures, false);
      // 1.2 find references in all documents if the document of the feature is
      // "PartSet". Features of this document can be used in all other documents
      SessionPtr aMgr = ModelAPI_Session::get();
      DocumentPtr aModuleDoc = aMgr->moduleDocument();
      if (aDeletedFeatureDoc == aModuleDoc) {
        // the deleted feature and results of the feature should be found in references
        std::list<ObjectPtr> aDeletedObjects;
        aDeletedObjects.push_back(aDeletedFeature);
        typedef std::list<std::shared_ptr<ModelAPI_Result> > ResultsList;
        const ResultsList& aDeletedResults = aDeletedFeature->results();
        ResultsList::const_iterator aRIter = aDeletedResults.begin();
        for (; aRIter != aDeletedResults.cend(); aRIter++) {
          ResultPtr aRes = *aRIter;
          if (aRes.get())
            aDeletedObjects.push_back(aRes);
        }
        // get all opened documents; found features in the documents;
        // get a list of objects where a feature refers;
        // search in these objects the deleted objects.
        std::list<DocumentPtr> anOpenedDocs = aMgr->allOpenedDocuments();
        std::list<DocumentPtr>::const_iterator anIt = anOpenedDocs.begin(),
                                               aLast = anOpenedDocs.end();
        std::list<std::pair<std::string, std::list<ObjectPtr> > > aRefs;
        for (; anIt != aLast; anIt++) {
          DocumentPtr aDocument = *anIt;
          if (aDocument == aDeletedFeatureDoc)
            continue; // this document has been already processed in 1.1

          int aFeaturesCount = aDocument->size(ModelAPI_Feature::group());
          for (int aId = 0; aId < aFeaturesCount; aId++) {
            ObjectPtr anObject = aDocument->object(ModelAPI_Feature::group(), aId);
            FeaturePtr aFeature = std::dynamic_pointer_cast<ModelAPI_Feature>(anObject);
            if (!aFeature.get())
              continue;

            aRefs.clear();
            aFeature->data()->referencesToObjects(aRefs);
            std::list<std::pair<std::string, std::list<ObjectPtr> > >::iterator aRef = aRefs.begin();
            bool aHasReferenceToDeleted = false;
            for(; aRef != aRefs.end() && !aHasReferenceToDeleted; aRef++) {
              std::list<ObjectPtr>::iterator aRefObj = aRef->second.begin();
              for(; aRefObj != aRef->second.end() && !aHasReferenceToDeleted; aRefObj++) {
                std::list<ObjectPtr>::const_iterator aDelIt = aDeletedObjects.begin();
                for(; aDelIt != aDeletedObjects.end() && !aHasReferenceToDeleted; aDelIt++) {
                  aHasReferenceToDeleted = *aDelIt == *aRefObj;
                }
              }
            }
            if (aHasReferenceToDeleted)
              aRefFeatures.insert(aFeature);
          }
        }
      }
    }
  }
  // 2. warn about the references remove, break the delete operation if the user chose it
  if (theAskAboutDeleteReferences && !aRefFeatures.empty()) {
    QStringList aRefNames;
    std::set<FeaturePtr>::const_iterator anIt = aRefFeatures.begin(),
                                         aLast = aRefFeatures.end();
    for (; anIt != aLast; anIt++) {
      aRefNames.append((*anIt)->name().c_str());
    }
    QString aNames = aRefNames.join(", ");

    QMessageBox::StandardButton aRes = QMessageBox::warning(
        theParent, tr("Delete features"),
        QString(tr("Selected features are used in the following features: %1.\
These features will be deleted also. Would you like to continue?")).arg(aNames),
        QMessageBox::No | QMessageBox::Yes, QMessageBox::No);
    if (aRes != QMessageBox::Yes)
      return false;
  }

  // 3. remove referenced features
  std::set<FeaturePtr>::const_iterator anIt = aRefFeatures.begin(),
                                       aLast = aRefFeatures.end();
#ifdef DEBUG_DELETE
  QStringList anInfo;
#endif
  for (; anIt != aLast; anIt++) {
    FeaturePtr aFeature = (*anIt);
    DocumentPtr aDoc = aFeature->document();
    if (theIgnoredFeatures.find(aFeature) == theIgnoredFeatures.end()) {
      aDoc->removeFeature(aFeature);
#ifdef DEBUG_DELETE
      anInfo.append(ModuleBase_Tools::objectInfo(aFeature).toStdString().c_str());
#endif
    }
  }
#ifdef DEBUG_DELETE
  qDebug(QString("remove references:%1").arg(anInfo.join("; ")).toStdString().c_str());
  anInfo.clear();
#endif

  // 4. remove the parameter features
  foreach (ObjectPtr aObj, theList) {
    ResultPtr aResult = std::dynamic_pointer_cast<ModelAPI_Result>(aObj);
    if (aResult.get() != NULL) { // results could not be removed,
      // they are removed by a corresponded feature remove
      continue;
    }
    FeaturePtr aFeature = ModelAPI_Feature::feature(aObj);
    if (aFeature) {
      // TODO: to learn the workshop to delegate the Part object deletion to the PartSet module
      // part features are removed in the PartSet module. This condition should be moved there
      if (aFeature->getKind() == "Part")
        continue;

      DocumentPtr aDoc = aObj->document();
      if (theIgnoredFeatures.find(aFeature) == theIgnoredFeatures.end()) {
        aDoc->removeFeature(aFeature);
#ifdef DEBUG_DELETE
        QString anInfoStr = ModuleBase_Tools::objectInfo(aFeature);
        anInfo.append(anInfoStr);
        qDebug(QString("remove feature :%1").arg(anInfoStr).toStdString().c_str());
#endif
      }
    }
  }
#ifdef DEBUG_DELETE
  qDebug(QString("remove features:%1").arg(anInfo.join("; ")).toStdString().c_str());
#endif
  return true;
}

bool hasResults(QObjectPtrList theObjects, const std::set<std::string>& theTypes)
{
  bool isFoundResultType = false;
  foreach(ObjectPtr anObj, theObjects)
  {
    ResultPtr aResult = std::dynamic_pointer_cast<ModelAPI_Result>(anObj);
    if (aResult.get() == NULL)
      continue;

    isFoundResultType = theTypes.find(aResult->groupName()) != theTypes.end();
    if (isFoundResultType)
      break;
  }
  return isFoundResultType;
}

//**************************************************************
bool XGUI_Workshop::canChangeColor() const
{
  QObjectPtrList aObjects = mySelector->selection()->selectedObjects();

  std::set<std::string> aTypes;
  aTypes.insert(ModelAPI_ResultGroup::group());
  aTypes.insert(ModelAPI_ResultConstruction::group());
  aTypes.insert(ModelAPI_ResultBody::group());
  aTypes.insert(ModelAPI_ResultPart::group());

  return hasResults(aObjects, aTypes);
}

void setColor(ResultPtr theResult, std::vector<int>& theColor)
{
  if (!theResult.get())
    return;

  AttributeIntArrayPtr aColorAttr = theResult->data()->intArray(ModelAPI_Result::COLOR_ID());
  if (aColorAttr.get() != NULL) {
    if (!aColorAttr->size()) {
      aColorAttr->setSize(3);
    }
    aColorAttr->setValue(0, theColor[0]);
    aColorAttr->setValue(1, theColor[1]);
    aColorAttr->setValue(2, theColor[2]);
  }
}

//**************************************************************
void XGUI_Workshop::changeColor(const QObjectPtrList& theObjects)
{
  AttributeIntArrayPtr aColorAttr;
  // 1. find the current color of the object. This is a color of AIS presentation
  // The objects are iterated until a first valid color is found 
  std::vector<int> aColor;
  foreach(ObjectPtr anObject, theObjects) {
    if (anObject->groupName() == ModelAPI_ResultPart::group()) {
      ResultPartPtr aPart = std::dynamic_pointer_cast<ModelAPI_ResultPart>(anObject);
      DocumentPtr aPartDoc = aPart->partDoc();
      // the document should be checked on null, because in opened document if the part
      // has not been activated yet, the part document is empty
      if (!aPartDoc.get()) {
        emit errorOccurred(QString::fromLatin1("Color can not be changed on a part with an empty document"));
      }
      else {
        if (aPartDoc->size(ModelAPI_ResultBody::group()) > 0) {
          ObjectPtr aObject = aPartDoc->object(ModelAPI_ResultBody::group(), 0);
          ResultBodyPtr aBody = std::dynamic_pointer_cast<ModelAPI_ResultBody>(aObject);
          if (aBody.get()) {
            XGUI_CustomPrs::getResultColor(aBody, aColor);
          }
        }
      }
    }
    else {
      ResultPtr aResult = std::dynamic_pointer_cast<ModelAPI_Result>(anObject);
      if (aResult.get())
        XGUI_CustomPrs::getResultColor(aResult, aColor);
      else {
        // TODO: remove the obtaining a color from the AIS object
        // this does not happen never because:
        // 1. The color can be changed only on results
        // 2. The result can be not visualized in the viewer(e.g. Origin Construction)
        AISObjectPtr anAISObj = myDisplayer->getAISObject(anObject);
        if (anAISObj.get()) {
          aColor.resize(3);
          anAISObj->getColor(aColor[0], aColor[1], aColor[2]);
        }
      }
    }
    if (!aColor.empty())
      break;
  }
  if (aColor.size() != 3)
    return;

  // 2. show the dialog to change the value
  XGUI_ColorDialog* aDlg = new XGUI_ColorDialog(mainWindow());
  aDlg->setColor(aColor);
  aDlg->move(QCursor::pos());
  bool isDone = aDlg->exec() == QDialog::Accepted;
  if (!isDone)
    return;

  bool isRandomColor = aDlg->isRandomColor();

  // 3. abort the previous operation and start a new one
  SessionPtr aMgr = ModelAPI_Session::get();
  bool aWasOperation = aMgr->isOperation(); // keep this value
  if (!aWasOperation) {
    QString aDescription = contextMenuMgr()->action("COLOR_CMD")->text();
    aMgr->startOperation(aDescription.toStdString());
  }

  // 4. set the value to all results
  foreach(ObjectPtr anObj, theObjects) {
    ResultPtr aResult = std::dynamic_pointer_cast<ModelAPI_Result>(anObj);
    if (aResult.get() != NULL) {
      if (aResult->groupName() == ModelAPI_ResultPart::group()) {
        ResultPartPtr aPart = std::dynamic_pointer_cast<ModelAPI_ResultPart>(aResult);
        DocumentPtr aPartDoc = aPart->partDoc();
        // the document should be checked on null, because in opened document if the part
        // has not been activated yet, the part document is empty
        if (aPartDoc.get()) {
          for (int i = 0; i < aPartDoc->size(ModelAPI_ResultBody::group()); i++) {
            ObjectPtr aObject = aPartDoc->object(ModelAPI_ResultBody::group(), i);
            ResultBodyPtr aBody = std::dynamic_pointer_cast<ModelAPI_ResultBody>(aObject);
            std::vector<int> aColorResult = aDlg->getColor();
            setColor(aBody, aColorResult);
          }
        }
      }
      else {
        std::vector<int> aColorResult = aDlg->getColor();
        setColor(aResult, aColorResult);
      }
    }
  }
  if (!aWasOperation)
    aMgr->finishOperation();
  updateCommandStatus();
}

//**************************************************************
#define SET_DISPLAY_GROUP(aGroupName, aDisplay) \
for (int i = 0; i < aDoc->size(aGroupName); i++) { \
  aDoc->object(aGroupName, i)->setDisplayed(aDisplay); \
}
void XGUI_Workshop::showObjects(const QObjectPtrList& theList, bool isVisible)
{
  foreach (ObjectPtr aObj, theList) {
    ResultPartPtr aPartRes = std::dynamic_pointer_cast<ModelAPI_ResultPart>(aObj);
    if (aPartRes) {
      DocumentPtr aDoc = aPartRes->partDoc();
      SET_DISPLAY_GROUP(ModelAPI_ResultBody::group(), isVisible)
      SET_DISPLAY_GROUP(ModelAPI_ResultConstruction::group(), isVisible)
      SET_DISPLAY_GROUP(ModelAPI_ResultGroup::group(), isVisible)
    } else {
      aObj->setDisplayed(isVisible);
    }
  }
  Events_Loop::loop()->flush(Events_Loop::eventByName(EVENT_OBJECT_TO_REDISPLAY));
}

//**************************************************************
void XGUI_Workshop::showOnlyObjects(const QObjectPtrList& theList)
{
  // Hide all displayed objects
  QObjectPtrList aList = myDisplayer->displayedObjects();
  foreach (ObjectPtr aObj, aList)
    aObj->setDisplayed(false);

  // Show only objects from the list
  foreach (ObjectPtr aObj, theList) {
    ResultPartPtr aPartRes = std::dynamic_pointer_cast<ModelAPI_ResultPart>(aObj);
    if (aPartRes) {
      DocumentPtr aDoc = aPartRes->partDoc();
      SET_DISPLAY_GROUP(ModelAPI_ResultBody::group(), true)
      SET_DISPLAY_GROUP(ModelAPI_ResultConstruction::group(), true)
      SET_DISPLAY_GROUP(ModelAPI_ResultGroup::group(), true)
    } else {
      aObj->setDisplayed(true);
    }
  }
  Events_Loop::loop()->flush(Events_Loop::eventByName(EVENT_OBJECT_TO_REDISPLAY));

}


//**************************************************************
void XGUI_Workshop::registerValidators() const
{
  SessionPtr aMgr = ModelAPI_Session::get();
  ModelAPI_ValidatorsFactory* aFactory = aMgr->validators();
}

//**************************************************************
/*void XGUI_Workshop::displayAllResults()
{
  SessionPtr aMgr = ModelAPI_Session::get();
  DocumentPtr aRootDoc = aMgr->moduleDocument();
  displayDocumentResults(aRootDoc);
  for (int i = 0; i < aRootDoc->size(ModelAPI_ResultPart::group()); i++) {
    ObjectPtr aObject = aRootDoc->object(ModelAPI_ResultPart::group(), i);
    ResultPartPtr aPart = std::dynamic_pointer_cast<ModelAPI_ResultPart>(aObject);
    displayDocumentResults(aPart->partDoc());
  }
  myDisplayer->updateViewer();
}*/

//**************************************************************
void XGUI_Workshop::displayDocumentResults(DocumentPtr theDoc)
{
  if (!theDoc)
    return;
  displayGroupResults(theDoc, ModelAPI_ResultConstruction::group());
  displayGroupResults(theDoc, ModelAPI_ResultBody::group());
}

//**************************************************************
void XGUI_Workshop::displayGroupResults(DocumentPtr theDoc, std::string theGroup)
{
  for (int i = 0; i < theDoc->size(theGroup); i++) 
    theDoc->object(theGroup, i)->setDisplayed(true);
    //displayObject(theDoc->object(theGroup, i));
  Events_Loop::loop()->flush(Events_Loop::eventByName(EVENT_OBJECT_TO_REDISPLAY));
}

//**************************************************************
void XGUI_Workshop::setDisplayMode(const QObjectPtrList& theList, int theMode)
{
  foreach(ObjectPtr aObj, theList) {
    myDisplayer->setDisplayMode(aObj, (XGUI_Displayer::DisplayMode)theMode, false);
  }
  if (theList.size() > 0)
    myDisplayer->updateViewer();
}

//**************************************************************
void XGUI_Workshop::closeDocument()
{
  ModuleBase_Operation* anOperation = operationMgr()->currentOperation();
  while (anOperation) {
    anOperation->abort();
    anOperation = operationMgr()->currentOperation();
  }
  myDisplayer->closeLocalContexts();
  myDisplayer->eraseAll();
  objectBrowser()->clearContent();

  SessionPtr aMgr = ModelAPI_Session::get();
  aMgr->closeAll();
}

//**************************************************************
bool XGUI_Workshop::displayObject(ObjectPtr theObj)
{
  if (!myModule->canDisplayObject(theObj))
    return false;

  ResultBodyPtr aBody = std::dynamic_pointer_cast<ModelAPI_ResultBody>(theObj);
  if (aBody.get() != NULL) {
    int aNb = myDisplayer->objectsCount();
    myDisplayer->display(theObj, false);
    if (aNb == 0)
      viewer()->fitAll();
  } else if (!(myIsLoadingData || myPartActivating))
    myDisplayer->display(theObj, false);

  return true;
}

void XGUI_Workshop::addHistoryMenu(QObject* theObject, const char* theSignal, const char* theSlot)
{
  XGUI_HistoryMenu* aMenu = NULL;
  if (isSalomeMode()) {
    QAction* anAction = qobject_cast<QAction*>(theObject);
    if (!anAction)
      return;
    aMenu = new XGUI_HistoryMenu(anAction);
  } else {
    QToolButton* aButton =  qobject_cast<QToolButton*>(theObject);
    aMenu = new XGUI_HistoryMenu(aButton);
  }
  connect(this, theSignal, aMenu, SLOT(setHistory(const QList<ActionInfo>&)));
  connect(aMenu, SIGNAL(actionSelected(int)), this, theSlot);
}

QList<ActionInfo> XGUI_Workshop::processHistoryList(const std::list<std::string>& theList) const
{
  QList<ActionInfo> aResult;
  std::list<std::string>::const_iterator it = theList.cbegin();
  for (; it != theList.cend(); it++) {
    QString anId = QString::fromStdString(*it);
    bool isEditing = anId.endsWith(ModuleBase_Operation::EditSuffix());
    if (isEditing) {
      anId.chop(ModuleBase_Operation::EditSuffix().size());
    }
    ActionInfo anInfo;
    QAction* aContextMenuAct = myContextMenuMgr->actionByName(anId);
    if (aContextMenuAct) {
      anInfo.initFrom(aContextMenuAct);
    } else {
      anInfo = myActionsMgr->actionInfoById(anId);
    }
    if (isEditing) {
      anInfo.text = anInfo.text.prepend("Modify ");
    }
    aResult << anInfo;
  }
  return aResult;
}
