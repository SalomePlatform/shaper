#include "ModuleBase_IModule.h"
#include "XGUI_Constants.h"
#include "XGUI_Command.h"
#include "XGUI_MainMenu.h"
#include "XGUI_MainWindow.h"
#include "XGUI_MenuGroupPanel.h"
#include "XGUI_Tools.h"
#include "XGUI_Workbench.h"
#include "XGUI_Workshop.h"
#include "XGUI_Viewer.h"
#include "XGUI_SelectionMgr.h"
#include "XGUI_Selection.h"
#include "XGUI_ObjectsBrowser.h"
#include "XGUI_Displayer.h"
#include "XGUI_OperationMgr.h"
#include "XGUI_SalomeConnector.h"
#include "XGUI_SalomeViewer.h"
#include "XGUI_ActionsMgr.h"
#include "XGUI_ErrorDialog.h"
#include "XGUI_ViewerProxy.h"
#include "XGUI_PropertyPanel.h"
#include "XGUI_ContextMenuMgr.h"
#include "XGUI_ModuleConnector.h"
#include "XGUI_Preferences.h"
#include <XGUI_QtEvents.h>

#include <ModelAPI_Events.h>
#include <ModelAPI_Session.h>
#include <ModelAPI_Feature.h>
#include <ModelAPI_Data.h>
#include <ModelAPI_AttributeDocRef.h>
#include <ModelAPI_Object.h>
#include <ModelAPI_Validator.h>
#include <ModelAPI_ResultConstruction.h>
#include <ModelAPI_ResultBody.h>

#include <PartSetPlugin_Part.h>

#include <Events_Loop.h>
#include <Events_Error.h>
#include <Events_LongOp.h>

#include <ModuleBase_Operation.h>
#include <ModuleBase_Operation.h>
#include <ModuleBase_OperationDescription.h>
#include <ModuleBase_SelectionValidator.h>
#include "ModuleBase_WidgetFactory.h"

#include <Config_Common.h>
#include <Config_FeatureMessage.h>
#include <Config_PointerMessage.h>
#include <Config_ModuleReader.h>

#include <QApplication>
#include <QFileDialog>
#include <QMessageBox>
#include <QMdiSubWindow>
#include <QPushButton>
#include <QDockWidget>
#include <QLayout>
#include <QThread>
#include <QObject>

#ifdef _DEBUG
#include <QDebug>
#include <iostream>
#endif

#ifdef WIN32
#include <windows.h>
#else
#include <dlfcn.h>
#endif

QMap<QString, QString> XGUI_Workshop::myIcons;

QString XGUI_Workshop::featureIcon(const std::string& theId)
{
  QString aId(theId.c_str());
  if (myIcons.contains(aId))
    return myIcons[aId];
  return QString();
}

XGUI_Workshop::XGUI_Workshop(XGUI_SalomeConnector* theConnector)
    : QObject(),
      myCurrentDir(QString()),
      myModule(NULL),
      mySalomeConnector(theConnector),
      myPropertyPanel(0),
      myObjectBrowser(0),
      myDisplayer(0),
      myUpdatePrefs(false),
      myPartActivating(false)
{
  myMainWindow = mySalomeConnector ? 0 : new XGUI_MainWindow();

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
  connect(myViewerProxy, SIGNAL(selectionChanged()), this, SLOT(updateCommandsOnViewSelection()));

  myModuleConnector = new XGUI_ModuleConnector(this);

  connect(myOperationMgr, SIGNAL(operationStarted()), SLOT(onOperationStarted()));
  connect(myOperationMgr, SIGNAL(operationResumed()), SLOT(onOperationStarted()));
  connect(myOperationMgr, SIGNAL(operationStopped(ModuleBase_Operation*)),
          SLOT(onOperationStopped(ModuleBase_Operation*)));
  connect(myMainWindow, SIGNAL(exitKeySequence()), SLOT(onExit()));
  connect(myOperationMgr, SIGNAL(operationStarted()), myActionsMgr, SLOT(update()));
  connect(myOperationMgr, SIGNAL(operationStopped(ModuleBase_Operation*)), myActionsMgr,
          SLOT(update()));
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
  //Initialize event listening
  Events_Loop* aLoop = Events_Loop::loop();
  aLoop->registerListener(this, Events_Error::errorID());  //!< Listening application errors.
  //TODO(sbh): Implement static method to extract event id [SEID]
  aLoop->registerListener(this, Events_Loop::eventByName(EVENT_FEATURE_LOADED));
  // TODO Is it good to use non standard event within workshop?
  aLoop->registerListener(this, Events_Loop::eventByName(EVENT_OPERATION_LAUNCHED));
  aLoop->registerListener(this, Events_Loop::eventByName(EVENT_OBJECT_UPDATED));
  aLoop->registerListener(this, Events_Loop::eventByName(EVENT_OBJECT_CREATED));
  aLoop->registerListener(this, Events_Loop::eventByName(EVENT_OBJECT_TO_REDISPLAY));
  aLoop->registerListener(this, Events_Loop::eventByName(EVENT_OBJECT_DELETED));
  aLoop->registerListener(this, Events_Loop::eventByName("LongOperation"));
  aLoop->registerListener(this, Events_Loop::eventByName(EVENT_PLUGIN_LOADED));
  aLoop->registerListener(this, Events_Loop::eventByName("CurrentDocumentChanged"));
  aLoop->registerListener(this, Events_Loop::eventByName(EVENT_OBJECT_TOSHOW));
  aLoop->registerListener(this, Events_Loop::eventByName(EVENT_OBJECT_TOHIDE));

  registerValidators();
  activateModule();
  if (myMainWindow) {
    myMainWindow->show();
    updateCommandStatus();
  }
  XGUI_Preferences::loadCustomProps();
  onNew();
}

//******************************************************
void XGUI_Workshop::initMenu()
{
  myContextMenuMgr->createActions();

  if (isSalomeMode()) {
    // Create only Undo, Redo commands
    QAction* aAction = salomeConnector()->addEditCommand("UNDO_CMD", tr("Undo"),
                                                         tr("Undo last command"),
                                                         QIcon(":pictures/undo.png"),
                                                         QKeySequence::Undo, false);
    connect(aAction, SIGNAL(triggered(bool)), this, SLOT(onUndo()));
    aAction = salomeConnector()->addEditCommand("REDO_CMD", tr("Redo"), tr("Redo last command"),
                                                QIcon(":pictures/redo.png"), QKeySequence::Redo,
                                                false);
    connect(aAction, SIGNAL(triggered(bool)), this, SLOT(onRedo()));
    salomeConnector()->addEditMenuSeparator();
    return;
  }
  // File commands group
  XGUI_MenuGroupPanel* aGroup = myMainWindow->menuObject()->generalPage();

  XGUI_Command* aCommand;

  aCommand = aGroup->addFeature("SAVE_CMD", tr("Save..."), tr("Save the document"),
                                QIcon(":pictures/save.png"), QKeySequence::Save);
  aCommand->connectTo(this, SLOT(onSave()));
  //aCommand->disable();

  aCommand = aGroup->addFeature("UNDO_CMD", tr("Undo"), tr("Undo last command"),
                                QIcon(":pictures/undo.png"), QKeySequence::Undo);
  aCommand->connectTo(this, SLOT(onUndo()));

  aCommand = aGroup->addFeature("REDO_CMD", tr("Redo"), tr("Redo last command"),
                                QIcon(":pictures/redo.png"), QKeySequence::Redo);
  aCommand->connectTo(this, SLOT(onRedo()));

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
XGUI_Workbench* XGUI_Workshop::addWorkbench(const QString& theName)
{
  XGUI_MainMenu* aMenuBar = myMainWindow->menuObject();
  return aMenuBar->addWorkbench(theName);
}

//******************************************************
void XGUI_Workshop::processEvent(const boost::shared_ptr<Events_Message>& theMessage)
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
  if (theMessage->eventID() == Events_Loop::loop()->eventByName(EVENT_FEATURE_LOADED)) {
    boost::shared_ptr<Config_FeatureMessage> aFeatureMsg =
       boost::dynamic_pointer_cast<Config_FeatureMessage>(theMessage);
    if (!aFeatureMsg->isInternal()) {
      addFeature(aFeatureMsg);
    }
  }

  // Process creation of Part
  else if (theMessage->eventID() == Events_Loop::loop()->eventByName(EVENT_OBJECT_CREATED)) {
    boost::shared_ptr<ModelAPI_ObjectUpdatedMessage> aUpdMsg =
        boost::dynamic_pointer_cast<ModelAPI_ObjectUpdatedMessage>(theMessage);
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
    boost::shared_ptr<ModelAPI_ObjectUpdatedMessage> aUpdMsg =
        boost::dynamic_pointer_cast<ModelAPI_ObjectUpdatedMessage>(theMessage);
    onFeatureRedisplayMsg(aUpdMsg);
  }

  //Update property panel on corresponding message. If there is no current operation (no
  //property panel), or received message has different feature to the current - do nothing.
  else if (theMessage->eventID() == Events_Loop::loop()->eventByName(EVENT_OBJECT_UPDATED)) {
    boost::shared_ptr<ModelAPI_ObjectUpdatedMessage> anUpdateMsg =
        boost::dynamic_pointer_cast<ModelAPI_ObjectUpdatedMessage>(theMessage);
    onFeatureUpdatedMsg(anUpdateMsg);
  }

  else if (theMessage->eventID() == Events_Loop::loop()->eventByName(EVENT_OBJECT_DELETED)) {
    boost::shared_ptr<ModelAPI_ObjectDeletedMessage> aDelMsg =
        boost::dynamic_pointer_cast<ModelAPI_ObjectDeletedMessage>(theMessage);
    onObjectDeletedMsg(aDelMsg);
  }

  else if (theMessage->eventID() == Events_LongOp::eventID()) {
    if (Events_LongOp::isPerformed())
      QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    else
      QApplication::restoreOverrideCursor();
  }

  else if (theMessage->eventID() == Events_Loop::loop()->eventByName(EVENT_OBJECT_TOSHOW)) {
    boost::shared_ptr<ModelAPI_ObjectUpdatedMessage> anUpdateMsg =
        boost::dynamic_pointer_cast<ModelAPI_ObjectUpdatedMessage>(theMessage);
    const std::set<ObjectPtr>& aObjList = anUpdateMsg->objects();
    QList<ObjectPtr> aList;
    std::set<ObjectPtr>::const_iterator aIt;
    for (aIt = aObjList.cbegin(); aIt != aObjList.cend(); ++aIt)
      aList.append(*aIt);
    showObjects(aList, true);
  }

  else if (theMessage->eventID() == Events_Loop::loop()->eventByName(EVENT_OBJECT_TOHIDE)) {
    boost::shared_ptr<ModelAPI_ObjectUpdatedMessage> anUpdateMsg =
        boost::dynamic_pointer_cast<ModelAPI_ObjectUpdatedMessage>(theMessage);
    const std::set<ObjectPtr>& aObjList = anUpdateMsg->objects();
    QList<ObjectPtr> aList;
    std::set<ObjectPtr>::const_iterator aIt;
    for (aIt = aObjList.cbegin(); aIt != aObjList.cend(); ++aIt)
      aList.append(*aIt);
    showObjects(aList, false);
  }

  //An operation passed by message. Start it, process and commit.
  else if (theMessage->eventID() == Events_Loop::loop()->eventByName(EVENT_OPERATION_LAUNCHED)) {
    boost::shared_ptr<Config_PointerMessage> aPartSetMsg =
        boost::dynamic_pointer_cast<Config_PointerMessage>(theMessage);
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
  else if (theMessage->eventID() == Events_Loop::loop()->eventByName("CurrentDocumentChanged")) {
    // Find and Activate active part
    if (myPartActivating)
      return;
    myActionsMgr->update();
    SessionPtr aMgr = ModelAPI_Session::get();
    DocumentPtr aActiveDoc = aMgr->activeDocument();
    DocumentPtr aDoc = aMgr->moduleDocument();
    if (aActiveDoc == aDoc) {
      activatePart(ResultPartPtr()); 
      return;
    }
    std::string aGrpName = ModelAPI_ResultPart::group();
    for (int i = 0; i < aDoc->size(aGrpName); i++) {
      ResultPartPtr aPart = boost::dynamic_pointer_cast<ModelAPI_ResultPart>(aDoc->object(aGrpName, i));
      if (aPart->partDoc() == aActiveDoc) {
        activatePart(aPart); // Activate a part which corresponds to active Doc
        return;
      }
    }
    // If not found then activate global document
    activatePart(ResultPartPtr()); 

  } else {
    //Show error dialog if error message received.
    boost::shared_ptr<Events_Error> anAppError = boost::dynamic_pointer_cast<Events_Error>(theMessage);
    if (anAppError) {
      emit errorOccurred(QString::fromLatin1(anAppError->description()));
    }
  }
  if (!isSalomeMode()) {
    SessionPtr aMgr = ModelAPI_Session::get();
    if (aMgr->isModified() != myMainWindow->isModifiedState())
      myMainWindow->setModifiedState(aMgr->isModified());
  }
}


//******************************************************
void XGUI_Workshop::onStartWaiting()
{
  if (Events_LongOp::isPerformed()) {
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
  }
}

//******************************************************
void XGUI_Workshop::onFeatureUpdatedMsg(const boost::shared_ptr<ModelAPI_ObjectUpdatedMessage>& theMsg)
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
  if (myObjectBrowser)
    myObjectBrowser->processEvent(theMsg);
}

//******************************************************
void XGUI_Workshop::onFeatureRedisplayMsg(const boost::shared_ptr<ModelAPI_ObjectUpdatedMessage>& theMsg)
{
  std::set<ObjectPtr> aObjects = theMsg->objects();
  std::set<ObjectPtr>::const_iterator aIt;
  for (aIt = aObjects.begin(); aIt != aObjects.end(); ++aIt) {
    ObjectPtr aObj = (*aIt);
    if (!aObj->data() || !aObj->data()->isValid() || aObj->document()->isConcealed(aObj))
      myDisplayer->erase(aObj, false);
    else {
      if (myDisplayer->isVisible(aObj))  // TODO VSV: Correction sketch drawing
        myDisplayer->display(aObj, false);  // In order to update presentation
      else {
        if (myOperationMgr->hasOperation()) {
          ModuleBase_Operation* aOperation = myOperationMgr->currentOperation();
          if (aOperation->hasObject(aObj)) {  // Display only current operation results
            myDisplayer->display(aObj, false);
          }
        }
      }
    }
  }
  myDisplayer->updateViewer();
}

//******************************************************
void XGUI_Workshop::onFeatureCreatedMsg(const boost::shared_ptr<ModelAPI_ObjectUpdatedMessage>& theMsg)
{
  std::set<ObjectPtr> aObjects = theMsg->objects();

  std::set<ObjectPtr>::const_iterator aIt;
  bool aHasPart = false;
  bool isDisplayed = false;
  for (aIt = aObjects.begin(); aIt != aObjects.end(); ++aIt) {
    ResultPartPtr aPart = boost::dynamic_pointer_cast<ModelAPI_ResultPart>(*aIt);
    if (aPart) {
      aHasPart = true;
      // If a feature is created from the aplication's python console  
      // it doesn't stored in the operation mgr and doesn't displayed
    } else if (myOperationMgr->hasOperation()) {
      ModuleBase_Operation* aOperation = myOperationMgr->currentOperation();
      if (!(*aIt)->document()->isConcealed(*aIt) &&
          aOperation->hasObject(*aIt)) {  // Display only current operation results
        myDisplayer->display(*aIt, false);
        isDisplayed = true;
      }
    }
  }
  if (myObjectBrowser)
    myObjectBrowser->processEvent(theMsg);
  if (isDisplayed)
    myDisplayer->updateViewer();
  if (aHasPart) {
    activateLastPart();
  }
}

//******************************************************
void XGUI_Workshop::onObjectDeletedMsg(const boost::shared_ptr<ModelAPI_ObjectDeletedMessage>& theMsg)
{
  if (myObjectBrowser)
    myObjectBrowser->processEvent(theMsg);
  //std::set<ObjectPtr> aFeatures = theMsg->objects();
}

//******************************************************
void XGUI_Workshop::onOperationStarted()
{
  ModuleBase_Operation* aOperation = myOperationMgr->currentOperation();
  if (this->isSalomeMode()) 
    aOperation->setNestedFeatures(mySalomeConnector->nestedActions(aOperation->id()));
  else 
    aOperation->setNestedFeatures(myActionsMgr->nestedCommands(aOperation->id()));
  
  if (aOperation->getDescription()->hasXmlRepresentation()) {  //!< No need for property panel
    connectWithOperation(aOperation);

    showPropertyPanel();
    QString aXmlRepr = aOperation->getDescription()->xmlRepresentation();
    ModuleBase_WidgetFactory aFactory = ModuleBase_WidgetFactory(aXmlRepr.toStdString(),
                                                                 myModuleConnector);

    myPropertyPanel->cleanContent();
    aFactory.createWidget(myPropertyPanel->contentWidget());

    QList<ModuleBase_ModelWidget*> aWidgets = aFactory.getModelWidgets();
    QList<ModuleBase_ModelWidget*>::const_iterator anIt = aWidgets.begin(), aLast = aWidgets.end();
    ModuleBase_ModelWidget* aWidget;
    for (; anIt != aLast; anIt++) {
      aWidget = *anIt;
      aWidget->setFeature(aOperation->feature());
      aWidget->enableFocusProcessing();
      QObject::connect(aWidget, SIGNAL(valuesChanged()), this, SLOT(onWidgetValuesChanged()));
      // Init default values
      if (!aOperation->isEditOperation() && !aWidget->isComputedDefault()) {
        aWidget->storeValue();
      }
    }

    myPropertyPanel->setModelWidgets(aWidgets);
    myPropertyPanel->onActivateNextWidget(NULL);
    // Widget activation (from the previous method) may commit the current operation
    // if pre-selection is enougth for it. So we shouldn't update prop panel's title
    if(myOperationMgr->isCurrentOperation(aOperation)) {
      myPropertyPanel->setWindowTitle(aOperation->getDescription()->description());
    }
  }
  updateCommandStatus();
}

//******************************************************
void XGUI_Workshop::onOperationStopped(ModuleBase_Operation* theOperation)
{
  //!< No need for property panel
  updateCommandStatus();
  hidePropertyPanel();
  myPropertyPanel->cleanContent();
}

bool XGUI_Workshop::event(QEvent * theEvent)
{
  PostponeMessageQtEvent* aPostponedEv = dynamic_cast<PostponeMessageQtEvent*>(theEvent);
  if (aPostponedEv) {
    boost::shared_ptr<Events_Message> aEventPtr = aPostponedEv->postponedMessage();
    processEvent(aEventPtr);
    return true;
  }
  return false;
}

/*
 *
 */
void XGUI_Workshop::addFeature(const boost::shared_ptr<Config_FeatureMessage>& theMessage)
{
  if (!theMessage) {
#ifdef _DEBUG
    qDebug() << "XGUI_Workshop::addFeature: NULL message.";
#endif
    return;
  }
  // Remember features icons
  myIcons[QString::fromStdString(theMessage->id())] = QString::fromStdString(theMessage->icon());

  //Find or create Workbench
  QString aWchName = QString::fromStdString(theMessage->workbenchId());
  QString aNestedFeatures = QString::fromStdString(theMessage->nestedFeatures());
  bool isUsePropPanel = theMessage->isUseInput();
  QString aFeatureId = QString::fromStdString(theMessage->id());
  if (isSalomeMode()) {
    QAction* aAction = salomeConnector()->addFeature(aWchName, aFeatureId,
                                                     QString::fromStdString(theMessage->text()),
                                                     QString::fromStdString(theMessage->tooltip()),
                                                     QIcon(theMessage->icon().c_str()),
                                                     QKeySequence(),
                                                     isUsePropPanel);
    salomeConnector()->setNestedActions(aFeatureId, aNestedFeatures.split(" ", QString::SkipEmptyParts));
    myActionsMgr->addCommand(aAction);
    myModule->featureCreated(aAction);
  } else {

    XGUI_MainMenu* aMenuBar = myMainWindow->menuObject();
    XGUI_Workbench* aPage = aMenuBar->findWorkbench(aWchName);
    if (!aPage) {
      aPage = addWorkbench(aWchName);
    }
    //Find or create Group
    QString aGroupName = QString::fromStdString(theMessage->groupId());
    XGUI_MenuGroupPanel* aGroup = aPage->findGroup(aGroupName);
    if (!aGroup) {
      aGroup = aPage->addGroup(aGroupName);
    }
    QString aDocKind = QString::fromStdString(theMessage->documentKind());
    // Check if hotkey sequence is already defined:
    QKeySequence aHotKey = myActionsMgr->registerShortcut(
        QString::fromStdString(theMessage->keysequence()));
    // Create feature...
    XGUI_Command* aCommand = aGroup->addFeature(aFeatureId,
                                                QString::fromStdString(theMessage->text()),
                                                QString::fromStdString(theMessage->tooltip()),
                                                QIcon(theMessage->icon().c_str()),
                                                aDocKind,
                                                aHotKey,
                                                isUsePropPanel);
    aCommand->setNestedCommands(aNestedFeatures.split(" ", QString::SkipEmptyParts));
    myActionsMgr->addCommand(aCommand);
    myModule->featureCreated(aCommand);
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
    XGUI_MainMenu* aMenu = myMainWindow->menuObject();
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
    aSession->moduleDocument()->close();
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
  aSession->load(myCurrentDir.toLatin1().constData());
  myObjectBrowser->rebuildDataTree();
  displayAllResults();
  updateCommandStatus();
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
void XGUI_Workshop::onUndo()
{
  objectBrowser()->treeView()->setCurrentIndex(QModelIndex());
  SessionPtr aMgr = ModelAPI_Session::get();
  if (aMgr->isOperation())
    operationMgr()->onAbortOperation();
  aMgr->undo();
  updateCommandStatus();
}

//******************************************************
void XGUI_Workshop::onRedo()
{
  objectBrowser()->treeView()->setCurrentIndex(QModelIndex());
  SessionPtr aMgr = ModelAPI_Session::get();
  if (aMgr->isOperation())
    operationMgr()->onAbortOperation();
  aMgr->redo();
  updateCommandStatus();
}

//******************************************************
void XGUI_Workshop::onRebuild()
{
  SessionPtr aMgr = ModelAPI_Session::get();
  bool aWasOperation = aMgr->isOperation(); // keep this value
  if (!aWasOperation) {
    aMgr->startOperation();
  }
  static const Events_ID aRebuildEvent = Events_Loop::loop()->eventByName("Rebuild");
  Events_Loop::loop()->send(boost::shared_ptr<Events_Message>(
    new Events_Message(aRebuildEvent, this)));
  if (!aWasOperation) {
    aMgr->finishOperation();
  }
}

//******************************************************
void XGUI_Workshop::onPreferences()
{
  XGUI_Prefs aModif;
  XGUI_Preferences::editPreferences(aModif);
  if (aModif.size() > 0) {
    QString aSection;
    foreach (XGUI_Pref aPref, aModif)
    {
      aSection = aPref.first;
      if (aSection == XGUI_Preferences::VIEWER_SECTION) {
        if (!isSalomeMode())
          myMainWindow->viewer()->updateFromResources();
      } else if (aSection == XGUI_Preferences::MENU_SECTION) {
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

  ModuleBase_IModule* aModule = crtInst ? crtInst(this) : 0;

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
    XGUI_MainMenu* aMenuBar = myMainWindow->menuObject();
    foreach (XGUI_Command* aCmd, aMenuBar->features())
      aCommands.append(aCmd);
  }
  SessionPtr aMgr = ModelAPI_Session::get();
  if (aMgr->hasModuleDocument()) {
    QAction* aUndoCmd;
    QAction* aRedoCmd;
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
    aUndoCmd->setEnabled(aMgr->canUndo());
    aRedoCmd->setEnabled(aMgr->canRedo());
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
}

//******************************************************
QList<QAction*> XGUI_Workshop::getModuleCommands() const
{
  QList<QAction*> aCommands;
  if (isSalomeMode()) {  // update commands in SALOME mode
    aCommands = salomeConnector()->commandList();
  } else {
    XGUI_MainMenu* aMenuBar = myMainWindow->menuObject();
    foreach(XGUI_Command* aCmd, aMenuBar->features())
    {
      aCommands.append(aCmd);
    }
  }
  return aCommands;
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
  connect(myObjectBrowser, SIGNAL(activePartChanged(ObjectPtr)), this,
          SLOT(changeCurrentDocument(ObjectPtr)));
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
  myPropertyPanel->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea | Qt::BottomDockWidgetArea);
  aDesktop->addDockWidget(Qt::LeftDockWidgetArea, myPropertyPanel);
  hidePropertyPanel();  //<! Invisible by default
  hideObjectBrowser();
  aDesktop->tabifyDockWidget(aObjDock, myPropertyPanel);

  QPushButton* aOkBtn = myPropertyPanel->findChild<QPushButton*>(XGUI::PROP_PANEL_OK);
  connect(aOkBtn, SIGNAL(clicked()), myOperationMgr, SLOT(onCommitOperation()));
  QPushButton* aCancelBtn = myPropertyPanel->findChild<QPushButton*>(XGUI::PROP_PANEL_CANCEL);
  connect(aCancelBtn, SIGNAL(clicked()), myOperationMgr, SLOT(onAbortOperation()));
  connect(myPropertyPanel, SIGNAL(keyReleased(QKeyEvent*)), myOperationMgr,
          SLOT(onKeyReleased(QKeyEvent*)));
  connect(myPropertyPanel, SIGNAL(widgetActivated(ModuleBase_ModelWidget*)), myOperationMgr,
          SLOT(onWidgetActivated(ModuleBase_ModelWidget*)));
  connect(myOperationMgr, SIGNAL(activateNextWidget(ModuleBase_ModelWidget*)), myPropertyPanel,
          SLOT(onActivateNextWidget(ModuleBase_ModelWidget*)));
  connect(myOperationMgr, SIGNAL(operationValidated(bool)), myPropertyPanel,
          SLOT(setAcceptEnabled(bool)));

}

//******************************************************
void XGUI_Workshop::showPropertyPanel()
{
  QAction* aViewAct = myPropertyPanel->toggleViewAction();
  //<! Restore ability to close panel from the window's menu
  aViewAct->setEnabled(true);
  myPropertyPanel->show();
  myPropertyPanel->raise();
}

//******************************************************
void XGUI_Workshop::hidePropertyPanel()
{
  QAction* aViewAct = myPropertyPanel->toggleViewAction();
  //<! Do not allow to show empty property panel
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
void XGUI_Workshop::onFeatureTriggered()
{
  QAction* aCmd = dynamic_cast<QAction*>(sender());
  if (aCmd) {
    QString aId = salomeConnector()->commandId(aCmd);
    if (!aId.isNull())
      myModule->launchOperation(aId);
  }
}

//******************************************************
void XGUI_Workshop::changeCurrentDocument(ObjectPtr theObj)
{
  SessionPtr aMgr = ModelAPI_Session::get();
  if (theObj) {
    ResultPartPtr aPart = boost::dynamic_pointer_cast<ModelAPI_ResultPart>(theObj);
    if (aPart) {
      DocumentPtr aPartDoc = aPart->partDoc();
      if (aPartDoc) {
        aMgr->setActiveDocument(aPartDoc);
        return;
      }
    }
  }
  aMgr->setActiveDocument(aMgr->moduleDocument());
}

//******************************************************
void XGUI_Workshop::salomeViewerSelectionChanged()
{
  emit salomeViewerSelection();
}

//**************************************************************
XGUI_SalomeViewer* XGUI_Workshop::salomeViewer() const
{
  return mySalomeConnector->viewer();
}

//**************************************************************
void XGUI_Workshop::onContextMenuCommand(const QString& theId, bool isChecked)
{
  QList<ObjectPtr> aObjects = mySelector->selection()->selectedObjects();
  if ((theId == "ACTIVATE_PART_CMD") && (aObjects.size() > 0)) {
    ResultPartPtr aPart = boost::dynamic_pointer_cast<ModelAPI_ResultPart>(aObjects.first());
    activatePart(aPart);
  } else if (theId == "DEACTIVATE_PART_CMD")
    activatePart(ResultPartPtr());
  else if (theId == "DELETE_CMD")
    deleteObjects(aObjects);
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
  else if (theId == "HIDEALL_CMD")
    myDisplayer->eraseAll();
}

//**************************************************************
void XGUI_Workshop::onWidgetValuesChanged()
{
  ModuleBase_Operation* anOperation = myOperationMgr->currentOperation();
  FeaturePtr aFeature = anOperation->feature();

  ModuleBase_ModelWidget* aSenderWidget = dynamic_cast<ModuleBase_ModelWidget*>(sender());
  //if (aCustom)
  //  aCustom->storeValue(aFeature);

  const QList<ModuleBase_ModelWidget*>& aWidgets = myPropertyPanel->modelWidgets();
  QList<ModuleBase_ModelWidget*>::const_iterator anIt = aWidgets.begin(), aLast = aWidgets.end();
  for (; anIt != aLast; anIt++) {
    ModuleBase_ModelWidget* aCustom = *anIt;
    if (aCustom && (/*!aCustom->isInitialized(aFeature) ||*/aCustom == aSenderWidget)) {
      //aCustom->storeValue(aFeature);
      aCustom->storeValue();
    }
  }
}

//**************************************************************
void XGUI_Workshop::activatePart(ResultPartPtr theFeature)
{
  if (!myPartActivating) {
    myPartActivating = true;
    if (theFeature)
      theFeature->activate();
    changeCurrentDocument(theFeature);
    myObjectBrowser->activatePart(theFeature);
    myPartActivating = false;
  }
}

//**************************************************************
void XGUI_Workshop::activateLastPart()
{
  SessionPtr aMgr = ModelAPI_Session::get();
  DocumentPtr aDoc = aMgr->moduleDocument();
  std::string aGrpName = ModelAPI_ResultPart::group();
  ObjectPtr aLastPart = aDoc->object(aGrpName, aDoc->size(aGrpName) - 1);
  ResultPartPtr aPart = boost::dynamic_pointer_cast<ModelAPI_ResultPart>(aLastPart);
  if (aPart) {
    activatePart(aPart);
  }
}

//**************************************************************
void XGUI_Workshop::deleteObjects(const QList<ObjectPtr>& theList)
{
  QMainWindow* aDesktop = isSalomeMode() ? salomeConnector()->desktop() : myMainWindow;
  QMessageBox::StandardButton aRes = QMessageBox::warning(
      aDesktop, tr("Delete features"), tr("Seleted features will be deleted. Continue?"),
      QMessageBox::No | QMessageBox::Yes, QMessageBox::No);
  // ToDo: definbe deleting method
  if (aRes == QMessageBox::Yes) {
    SessionPtr aMgr = ModelAPI_Session::get();
    aMgr->startOperation();
    foreach (ObjectPtr aObj, theList)
    {
      ResultPartPtr aPart = boost::dynamic_pointer_cast<ModelAPI_ResultPart>(aObj);
      if (aPart) {
        DocumentPtr aDoc = aPart->document();
        if (aDoc == aMgr->activeDocument()) {
          aDoc->close();
        }
        //aMgr->moduleDocument()->removeFeature(aPart->owner());
      } else {
        FeaturePtr aFeature = boost::dynamic_pointer_cast<ModelAPI_Feature>(aObj);
        if (aFeature)
          aObj->document()->removeFeature(aFeature);
      }
    }
    myDisplayer->updateViewer();
    aMgr->finishOperation();
  }
}

//**************************************************************
void XGUI_Workshop::showObjects(const QList<ObjectPtr>& theList, bool isVisible)
{
  foreach (ObjectPtr aObj, theList)
  {
    ResultPtr aRes = boost::dynamic_pointer_cast<ModelAPI_Result>(aObj);
    if (aRes) {
      if (isVisible) {
        myDisplayer->display(aRes, false);
      } else {
        myDisplayer->erase(aRes, false);
      }
    }
  }
  myDisplayer->updateViewer();
}

//**************************************************************
void XGUI_Workshop::showOnlyObjects(const QList<ObjectPtr>& theList)
{
  myDisplayer->eraseAll(false);
  showObjects(theList, true);
}


//**************************************************************
void XGUI_Workshop::updateCommandsOnViewSelection()
{
  XGUI_Selection* aSelection = mySelector->selection();
  if (aSelection->getSelected().size() == 0)
    return;

  // Restrict validators to manage only nested (child) features
  // of the current feature i.e. if current feature is Sketch -
  // Sketch Features & Constraints can be validated.
  QStringList aNestedIds;
  if(myOperationMgr->hasOperation()) {
    FeaturePtr aFeature = myOperationMgr->currentOperation()->feature();
    if(aFeature) {
      aNestedIds << myActionsMgr->nestedCommands(QString::fromStdString(aFeature->getKind()));
    }
  }
  SessionPtr aMgr = ModelAPI_Session::get();
  ModelAPI_ValidatorsFactory* aFactory = aMgr->validators();
  QList<QAction*> aActions = getModuleCommands();
  foreach(QAction* aAction, aActions) {
    QString aId = aAction->data().toString();
    if(!aNestedIds.contains(aId))
      continue;
    std::list<ModelAPI_Validator*> aValidators;
    std::list<std::list<std::string> > anArguments;
    aFactory->validators(aId.toStdString(), aValidators, anArguments);
    std::list<ModelAPI_Validator*>::iterator aValidator = aValidators.begin();
    for (; aValidator != aValidators.end(); aValidator++) {
      if (*aValidator) {
        const ModuleBase_SelectionValidator* aSelValidator =
            dynamic_cast<const ModuleBase_SelectionValidator*>(*aValidator);
        if (aSelValidator) {
          aAction->setEnabled(aSelValidator->isValid(aSelection));
        }
      }
    }
  }
}

//**************************************************************
void XGUI_Workshop::registerValidators() const
{
  SessionPtr aMgr = ModelAPI_Session::get();
  ModelAPI_ValidatorsFactory* aFactory = aMgr->validators();
}

//**************************************************************
void XGUI_Workshop::displayAllResults()
{
  SessionPtr aMgr = ModelAPI_Session::get();
  DocumentPtr aRootDoc = aMgr->moduleDocument();
  displayDocumentResults(aRootDoc);
  for (int i = 0; i < aRootDoc->size(ModelAPI_ResultPart::group()); i++) {
    ObjectPtr aObject = aRootDoc->object(ModelAPI_ResultPart::group(), i);
    ResultPartPtr aPart = boost::dynamic_pointer_cast<ModelAPI_ResultPart>(aObject);
    displayDocumentResults(aPart->partDoc());
  }
  myDisplayer->updateViewer();
}

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
    myDisplayer->display(theDoc->object(theGroup, i), false);
}

//**************************************************************
void XGUI_Workshop::setDisplayMode(const QList<ObjectPtr>& theList, int theMode)
{
  foreach(ObjectPtr aObj, theList) {
    myDisplayer->setDisplayMode(aObj, (XGUI_Displayer::DisplayMode)theMode, false);
  }
  if (theList.size() > 0)
    myDisplayer->updateViewer();
}
