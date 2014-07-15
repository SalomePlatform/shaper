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
#include "ModuleBase_WidgetFactory.h"
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

#include <Model_Events.h>
#include <ModelAPI_PluginManager.h>
#include <ModelAPI_Feature.h>
#include <ModelAPI_Data.h>
#include <ModelAPI_AttributeDocRef.h>
#include <ModelAPI_Object.h>
#include <ModelAPI_Validator.h>
#include <ModelAPI_ResultPart.h>

#include <PartSetPlugin_Part.h>

#include <Events_Loop.h>
#include <Events_Error.h>

#include <ModuleBase_Operation.h>
#include <ModuleBase_Operation.h>
#include <ModuleBase_OperationDescription.h>
#include <ModuleBase_SelectionValidator.h>

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
#include <QTimer>

#ifdef _DEBUG
#include <QDebug>
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
  myDisplayer(0)
{
  myMainWindow = mySalomeConnector? 0 : new XGUI_MainWindow();

  myDisplayer = new XGUI_Displayer(this);

  mySelector = new XGUI_SelectionMgr(this);
  //connect(mySelector, SIGNAL(selectionChanged()), this, SLOT(updateModuleCommands()));

  myOperationMgr = new XGUI_OperationMgr(this);
  myActionsMgr = new XGUI_ActionsMgr(this);
  myErrorDlg = new XGUI_ErrorDialog(myMainWindow);
  myContextMenuMgr = new XGUI_ContextMenuMgr(this);
  connect(myContextMenuMgr, SIGNAL(actionTriggered(const QString&, bool)), 
          this, SLOT(onContextMenuCommand(const QString&, bool)));

  myViewerProxy = new XGUI_ViewerProxy(this);
  connect(myViewerProxy, SIGNAL(selectionChanged()), this, SLOT(updateCommandsOnViewSelection()));
  
  myModuleConnector = new XGUI_ModuleConnector(this);

  connect(myOperationMgr, SIGNAL(operationStarted()), SLOT(onOperationStarted()));
  connect(myOperationMgr, SIGNAL(operationResumed()), SLOT(onOperationStarted()));
  connect(myOperationMgr, SIGNAL(operationStopped(ModuleBase_Operation*)), SLOT(onOperationStopped(ModuleBase_Operation*)));
  connect(myMainWindow, SIGNAL(exitKeySequence()), SLOT(onExit()));
  connect(myOperationMgr, SIGNAL(operationStarted()), myActionsMgr, SLOT(update()));
  connect(myOperationMgr, SIGNAL(operationStopped(ModuleBase_Operation*)), myActionsMgr, SLOT(update()));
  connect(this, SIGNAL(errorOccurred(const QString&)), myErrorDlg, SLOT(addError(const QString&)));
}

//******************************************************
XGUI_Workshop::~XGUI_Workshop(void)
{
}

//******************************************************
void XGUI_Workshop::startApplication()
{
  initMenu();
  //Initialize event listening
  Events_Loop* aLoop = Events_Loop::loop();
  aLoop->registerListener(this, Events_Error::errorID()); //!< Listening application errors.
  //TODO(sbh): Implement static method to extract event id [SEID]
  aLoop->registerListener(this, Events_Loop::eventByName(EVENT_FEATURE_LOADED));
  // TODO Is it good to use non standard event within workshop?
  aLoop->registerListener(this, Events_Loop::eventByName(EVENT_OPERATION_LAUNCHED));
  aLoop->registerListener(this, Events_Loop::eventByName(EVENT_OBJECT_UPDATED));
  aLoop->registerListener(this, Events_Loop::eventByName(EVENT_OBJECT_CREATED));
  aLoop->registerListener(this, Events_Loop::eventByName(EVENT_OBJECT_TO_REDISPLAY));

  activateModule();
  if (myMainWindow) {
    myMainWindow->show();
    updateCommandStatus();
  }
  onNew();
}

//******************************************************
void XGUI_Workshop::initMenu()
{
  myContextMenuMgr->createActions();

  if (isSalomeMode()) {
    // Create only Undo, Redo commands
    QAction* aAction = salomeConnector()->addEditCommand("UNDO_CMD", 
                                      tr("Undo"), tr("Undo last command"),
                                      QIcon(":pictures/undo.png"), 
                                      QKeySequence::Undo, false);
    connect(aAction, SIGNAL(triggered(bool)), this, SLOT(onUndo()));
    aAction = salomeConnector()->addEditCommand("REDO_CMD", 
                                      tr("Redo"), tr("Redo last command"),
                                      QIcon(":pictures/redo.png"), 
                                      QKeySequence::Redo, false);
    connect(aAction, SIGNAL(triggered(bool)), this, SLOT(onRedo()));
    salomeConnector()->addEditMenuSeparator();
    return;
  }
  XGUI_Workbench* aPage = myMainWindow->menuObject()->generalPage();

  // File commands group
  XGUI_MenuGroupPanel* aGroup = aPage->addGroup("Default");

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
                                QIcon(":pictures/rebuild.png"));

  aCommand = aGroup->addFeature("SAVEAS_CMD", tr("Save as..."), tr("Save the document into a file"),
                                QIcon(":pictures/save.png"));
  aCommand->connectTo(this, SLOT(onSaveAs()));
  //aCommand->disable();

  aCommand = aGroup->addFeature("OPEN_CMD", tr("Open..."), tr("Open a new document"),
                                QIcon(":pictures/open.png"), QKeySequence::Open);
  aCommand->connectTo(this, SLOT(onOpen()));

  //aCommand = aGroup->addFeature("NEW_CMD", tr("New"), tr("Create a new document"),
  //                              QIcon(":pictures/new.png"), QKeySequence::New);
  //aCommand->connectTo(this, SLOT(onNew()));

  aCommand = aGroup->addFeature("EXIT_CMD", tr("Exit"), tr("Exit application"),
                                QIcon(":pictures/close.png"), QKeySequence::Close);
  aCommand->connectTo(this, SLOT(onExit()));
  //FIXME: SBH's test action. Can be used for some GUI tests.
  //#ifdef _DEBUG
  //  aCommand = aGroup->addFeature("TEST_CMD", "Test!", "Private debug button",
  //                                QIcon(":pictures/close.png"));
  //  aCommand->connectTo(myActionsMgr, SLOT(update()));
  //#endif
}

//******************************************************
XGUI_Workbench* XGUI_Workshop::addWorkbench(const QString& theName)
{
  XGUI_MainMenu* aMenuBar = myMainWindow->menuObject();
  return aMenuBar->addWorkbench(theName);
}

//******************************************************
void XGUI_Workshop::processEvent(const Events_Message* theMessage)
{
  //A message to start feature creation received.
  static Events_ID aFeatureLoadedId = Events_Loop::loop()->eventByName(EVENT_FEATURE_LOADED);
  if (theMessage->eventID() == aFeatureLoadedId) {
    const Config_FeatureMessage* aFeatureMsg = dynamic_cast<const Config_FeatureMessage*>(theMessage);
    if(!aFeatureMsg->isInternal()) {
      addFeature(aFeatureMsg);
    }
    return;
  }

  // Process creation of Part
  if (theMessage->eventID() == Events_Loop::loop()->eventByName(EVENT_OBJECT_CREATED)) {
    const Model_ObjectUpdatedMessage* aUpdMsg = dynamic_cast<const Model_ObjectUpdatedMessage*>(theMessage);
    onFeatureCreatedMsg(aUpdMsg);
  }

  // Redisplay feature
  if (theMessage->eventID() == Events_Loop::loop()->eventByName(EVENT_OBJECT_TO_REDISPLAY)) {
    const Model_ObjectUpdatedMessage* aUpdMsg = dynamic_cast<const Model_ObjectUpdatedMessage*>(theMessage);
    onFeatureRedisplayMsg(aUpdMsg);
  }

  //Update property panel on corresponding message. If there is no current operation (no
  //property panel), or received message has different feature to the current - do nothing.
  if (theMessage->eventID() == Events_Loop::loop()->eventByName(EVENT_OBJECT_UPDATED)) {
    const Model_ObjectUpdatedMessage* anUpdateMsg =
        dynamic_cast<const Model_ObjectUpdatedMessage*>(theMessage);
    onFeatureUpdatedMsg(anUpdateMsg);
  }

  //An operation passed by message. Start it, process and commit.
  if (theMessage->eventID() == Events_Loop::loop()->eventByName(EVENT_OPERATION_LAUNCHED)) {
    const Config_PointerMessage* aPartSetMsg = dynamic_cast<const Config_PointerMessage*>(theMessage);
    //myPropertyPanel->cleanContent();
    ModuleBase_Operation* anOperation = (ModuleBase_Operation*)aPartSetMsg->pointer();

    if (myOperationMgr->startOperation(anOperation)) {
      myPropertyPanel->updateContentWidget(anOperation->feature());
      if (!anOperation->getDescription()->hasXmlRepresentation()) {
        anOperation->commit();
        updateCommandStatus();
      }
    }
    return;
  }
  //Show error dialog if error message received.
  const Events_Error* anAppError = dynamic_cast<const Events_Error*>(theMessage);
  if (anAppError) {
    emit errorOccurred(QString::fromLatin1(anAppError->description()));
  }
}

//******************************************************
void XGUI_Workshop::onFeatureUpdatedMsg(const Model_ObjectUpdatedMessage* theMsg)
{
  std::set<ObjectPtr> aFeatures = theMsg->objects();
  if (myOperationMgr->hasOperation())
  {
    FeaturePtr aCurrentFeature = myOperationMgr->currentOperation()->feature();
    std::set<ObjectPtr>::const_iterator aIt;
    for (aIt = aFeatures.begin(); aIt != aFeatures.end(); ++aIt) {
      ObjectPtr aNewFeature = (*aIt);
      if(aNewFeature == aCurrentFeature) {
        myPropertyPanel->updateContentWidget(aCurrentFeature);
        break;
      } 
    }
  }
}

//******************************************************
void XGUI_Workshop::onFeatureRedisplayMsg(const Model_ObjectUpdatedMessage* theMsg)
{
  std::set<ObjectPtr> aObjects = theMsg->objects();
  std::set<ObjectPtr>::const_iterator aIt;
  bool isDisplayed = false;
  for (aIt = aObjects.begin(); aIt != aObjects.end(); ++aIt) {
    ResultPtr aRes = boost::dynamic_pointer_cast<ModelAPI_Result>(*aIt);
    if (aRes) {
      isDisplayed = myDisplayer->redisplay(aRes, false);
    }
  }
  if (isDisplayed)
    myDisplayer->updateViewer();
}

//******************************************************
void XGUI_Workshop::onFeatureCreatedMsg(const Model_ObjectUpdatedMessage* theMsg)
{
  std::set<ObjectPtr> aFeatures = theMsg->objects();

  std::set<ObjectPtr>::const_iterator aIt;
  bool aHasPart = false;
  bool isDisplayed = false;
  for (aIt = aFeatures.begin(); aIt != aFeatures.end(); ++aIt) {
     ResultPartPtr aPart = boost::dynamic_pointer_cast<ModelAPI_ResultPart>(*aIt);
    if (aPart) {
      aHasPart = true;
      //break;
    } else {
      ResultPtr aRes = boost::dynamic_pointer_cast<ModelAPI_Result>(*aIt);
      if (aRes)
        isDisplayed = myDisplayer->display(aRes, false);
    }
  }
  if (isDisplayed)
    myDisplayer->updateViewer();
  if (aHasPart) {
    //The created part will be created in Object Browser later and we have to activate it
    // only when it is created everywere
    QTimer::singleShot(50, this, SLOT(activateLastPart()));
  }
}
 
//******************************************************
void XGUI_Workshop::onOperationStarted()
{
  ModuleBase_Operation* aOperation = myOperationMgr->currentOperation();

  if(aOperation->getDescription()->hasXmlRepresentation()) { //!< No need for property panel
    connectWithOperation(aOperation);

    showPropertyPanel();
    QString aXmlRepr = aOperation->getDescription()->xmlRepresentation();
    ModuleBase_WidgetFactory aFactory = ModuleBase_WidgetFactory(aXmlRepr.toStdString(), myModuleConnector);

    myPropertyPanel->cleanContent();
    aFactory.createWidget(myPropertyPanel->contentWidget());
    
    QList<ModuleBase_ModelWidget*> aWidgets = aFactory.getModelWidgets();
    QList<ModuleBase_ModelWidget*>::const_iterator anIt = aWidgets.begin(), aLast = aWidgets.end();
    ModuleBase_ModelWidget* aWidget;
    for (; anIt != aLast; anIt++) {
      aWidget = *anIt;
      //QObject::connect(aWidget, SIGNAL(valuesChanged()),  aOperation, SLOT(storeCustomValue()));
      QObject::connect(aWidget, SIGNAL(valuesChanged()),
                       this, SLOT(onWidgetValuesChanged()));
      // Init default values
      if (!aOperation->isEditOperation() && aWidget->hasDefaultValue()) {
        aWidget->storeValue(aOperation->feature());
      }
    }

    myPropertyPanel->setModelWidgets(aWidgets);
    myPropertyPanel->setWindowTitle(aOperation->getDescription()->description());
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

/*
 *
 */
void XGUI_Workshop::addFeature(const Config_FeatureMessage* theMessage)
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
  QString aId = QString::fromStdString(theMessage->id());
  if (isSalomeMode()) {
    QAction* aAction = salomeConnector()->addFeature(aWchName,
                              aId,
                              QString::fromStdString(theMessage->text()),
                              QString::fromStdString(theMessage->tooltip()),
                              QIcon(theMessage->icon().c_str()),
                              QKeySequence(), isUsePropPanel);
    salomeConnector()->setNestedActions(aId, aNestedFeatures.split(" "));
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
    //Create feature...
    XGUI_Command* aCommand = aGroup->addFeature(aId,
                                                QString::fromStdString(theMessage->text()),
                                                QString::fromStdString(theMessage->tooltip()),
                                                QIcon(theMessage->icon().c_str()),
                                                QKeySequence(), isUsePropPanel);
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
    aCommand = aMenu->feature(theOperation->getDescription()->operationId());
  }
  //Abort operation on uncheck the command
  connect(aCommand, SIGNAL(triggered(bool)), theOperation, SLOT(setRunning(bool)));
}

/*
 * Saves document with given name.
 */
void XGUI_Workshop::saveDocument(QString theName)
{
  QApplication::restoreOverrideCursor();
  PluginManagerPtr aMgr = ModelAPI_PluginManager::get();
  DocumentPtr aDoc = aMgr->rootDocument();
  aDoc->save(theName.toLatin1().constData());
  QApplication::restoreOverrideCursor();
}

//******************************************************
void XGUI_Workshop::onExit()
{
  PluginManagerPtr aMgr = ModelAPI_PluginManager::get();
  DocumentPtr aDoc = aMgr->rootDocument();
  if(aDoc->isModified()) {
    int anAnswer = QMessageBox::question(
        myMainWindow, tr("Save current file"),
        tr("The document is modified, save before exit?"),
        QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel, QMessageBox::Cancel);
    if(anAnswer == QMessageBox::Save) {
      bool saved = onSave();
      if(!saved) {
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
  //save current file before close if modified
  PluginManagerPtr aMgr = ModelAPI_PluginManager::get();
  DocumentPtr aDoc = aMgr->rootDocument();
  if(aDoc->isModified()) {
    //TODO(sbh): re-launch the app?
    int anAnswer = QMessageBox::question(
        myMainWindow, tr("Save current file"),
        tr("The document is modified, save before opening another?"),
        QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel, QMessageBox::Cancel);
    if(anAnswer == QMessageBox::Save) {
      onSave();
    } else if (anAnswer == QMessageBox::Cancel) {
      return;
    }
    aDoc->close();
    myCurrentDir = "";
  }

  //show file dialog, check if readable and open
  myCurrentDir = QFileDialog::getExistingDirectory(mainWindow());
  if(myCurrentDir.isEmpty())
    return;
  QFileInfo aFileInfo(myCurrentDir);
  if(!aFileInfo.exists() || !aFileInfo.isReadable()) {
    QMessageBox::critical(myMainWindow, tr("Warning"), tr("Unable to open the file."));
    myCurrentDir = "";
    return;
  }
  QApplication::setOverrideCursor(Qt::WaitCursor);
  aDoc->load(myCurrentDir.toLatin1().constData());
  updateCommandStatus();
  myObjectBrowser->rebuildDataTree();
  QApplication::restoreOverrideCursor();
}

//******************************************************
bool XGUI_Workshop::onSave()
{
  if(myCurrentDir.isEmpty()) {
    return onSaveAs();
  }
  saveDocument(myCurrentDir);
  updateCommandStatus();
  return true;
}

//******************************************************
bool XGUI_Workshop::onSaveAs()
{
  QFileDialog dialog(mainWindow());
  dialog.setWindowTitle(tr("Select directory to save files..."));
  dialog.setFileMode(QFileDialog::Directory);
  dialog.setFilter(tr("Folders (*)"));
  dialog.setOptions(QFileDialog::HideNameFilterDetails | QFileDialog::ShowDirsOnly);
  dialog.setViewMode(QFileDialog::Detail);

  if(!dialog.exec()) {
    return false;
  }
  QString aTempDir = dialog.selectedFiles().first();
  QDir aDir(aTempDir);
  if(aDir.exists() && !aDir.entryInfoList(QDir::NoDotAndDotDot|QDir::AllEntries).isEmpty()) {
    int answer = QMessageBox::question(myMainWindow,
                                       //: Title of the dialog which asks user if he wants to save study in existing non-empty folder
                                       tr("Save"),
                                       tr("The folder already contains some files, save anyway?"),
                                       QMessageBox::Save|QMessageBox::Cancel);
    if(answer == QMessageBox::Cancel) {
      return false;
    }
  }
  myCurrentDir = aTempDir;
  return onSave();
}

//******************************************************
void XGUI_Workshop::onUndo()
{
  objectBrowser()->treeView()->setCurrentIndex(QModelIndex());
  PluginManagerPtr aMgr = ModelAPI_PluginManager::get();
  DocumentPtr aDoc = aMgr->rootDocument();
  if (aDoc->isOperation())
    operationMgr()->abortOperation();
  aDoc->undo();
  updateCommandStatus();
}

//******************************************************
void XGUI_Workshop::onRedo()
{
  objectBrowser()->treeView()->setCurrentIndex(QModelIndex());
  PluginManagerPtr aMgr = ModelAPI_PluginManager::get();
  DocumentPtr aDoc = aMgr->rootDocument();
  if (aDoc->isOperation())
    operationMgr()->abortOperation();
  aDoc->redo();
  updateCommandStatus();
}

//******************************************************
ModuleBase_IModule* XGUI_Workshop::loadModule(const QString& theModule)
{
  QString libName =
      QString::fromStdString(library(theModule.toStdString()));
  if (libName.isEmpty()) {
    qWarning(
    qPrintable( tr( "Information about module \"%1\" doesn't exist." ).arg( theModule ) ));
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
      QMessageBox::warning(mainWindow(), tr("Error"), err);
    } else {
      qWarning( qPrintable( err ));
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
  if (isSalomeMode()) { // update commands in SALOME mode
    aCommands = salomeConnector()->commandList();
  } else {
    XGUI_MainMenu* aMenuBar = myMainWindow->menuObject();
    foreach (XGUI_Command* aCmd, aMenuBar->features())
      aCommands.append(aCmd);
  }
  PluginManagerPtr aMgr = ModelAPI_PluginManager::get();
  if (aMgr->hasRootDocument()) {
    QAction* aUndoCmd;
    QAction* aRedoCmd;
    foreach(QAction* aCmd, aCommands) {
      QString aId = aCmd->data().toString();
      if (aId == "UNDO_CMD")
        aUndoCmd = aCmd;
      else if (aId == "REDO_CMD")
        aRedoCmd = aCmd;
      else // Enable all commands
        aCmd->setEnabled(true);
    }
    DocumentPtr aDoc = aMgr->rootDocument();
    aUndoCmd->setEnabled(aDoc->canUndo());
    aRedoCmd->setEnabled(aDoc->canRedo());
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
  if (isSalomeMode()) { // update commands in SALOME mode
    aCommands = salomeConnector()->commandList();
  } else {
    XGUI_MainMenu* aMenuBar = myMainWindow->menuObject();
    foreach (XGUI_Workbench* aWb, aMenuBar->workbenches()) {
      if (aWb != aMenuBar->generalPage()) {
        foreach(XGUI_Command* aCmd, aWb->features())
          aCommands.append(aCmd);
      }
    }
  }
  return aCommands;
}

//******************************************************
QDockWidget* XGUI_Workshop::createObjectBrowser(QWidget* theParent)
{
  QDockWidget* aObjDock = new QDockWidget(theParent);
  aObjDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
  aObjDock->setWindowTitle(tr("Object browser"));
  aObjDock->setStyleSheet("::title { position: relative; padding-left: 5px; text-align: left center }");
  myObjectBrowser = new XGUI_ObjectsBrowser(aObjDock);
  connect(myObjectBrowser, SIGNAL(activePartChanged(FeaturePtr)), this, SLOT(changeCurrentDocument(FeaturePtr)));
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
  QMainWindow* aDesktop = isSalomeMode()? salomeConnector()->desktop() :
                                          myMainWindow;
  QDockWidget* aObjDock = createObjectBrowser(aDesktop);
  aDesktop->addDockWidget(Qt::LeftDockWidgetArea, aObjDock);
  myPropertyPanel = new XGUI_PropertyPanel(aDesktop);
  aDesktop->addDockWidget(Qt::LeftDockWidgetArea, myPropertyPanel);
  hidePropertyPanel(); //<! Invisible by default
  hideObjectBrowser();
  aDesktop->tabifyDockWidget(aObjDock, myPropertyPanel);

  QPushButton* aOkBtn = myPropertyPanel->findChild<QPushButton*>(XGUI::PROP_PANEL_OK);
  connect(aOkBtn, SIGNAL(clicked()), myOperationMgr, SLOT(onCommitOperation()));
  QPushButton* aCancelBtn = myPropertyPanel->findChild<QPushButton*>(XGUI::PROP_PANEL_CANCEL);
  connect(aCancelBtn, SIGNAL(clicked()), myOperationMgr, SLOT(onAbortOperation()));

  connect(myPropertyPanel, SIGNAL(keyReleased(const std::string&, QKeyEvent*)),
          myOperationMgr, SLOT(onKeyReleased(const std::string&, QKeyEvent*)));

  connect(myPropertyPanel, SIGNAL(widgetActivated(ModuleBase_ModelWidget*)),
          myOperationMgr, SLOT(onWidgetActivated(ModuleBase_ModelWidget*)));
  connect(myOperationMgr, SIGNAL(activateNextWidget(ModuleBase_ModelWidget*)),
          myPropertyPanel, SLOT(onActivateNextWidget(ModuleBase_ModelWidget*)));
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
  PluginManagerPtr aMgr = ModelAPI_PluginManager::get();
  if (theObj) {
    DocumentPtr aPartDoc = theObj->document();
    if (aPartDoc)
      aMgr->setCurrentDocument(aPartDoc);
  } else {
    aMgr->setCurrentDocument(aMgr->rootDocument());
  }
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
    if (aPart)
      activatePart(aPart);
  } else if (theId == "DEACTIVATE_PART_CMD") 
    activatePart(ResultPartPtr());
  else if (theId == "DELETE_CMD")
    deleteObjects(aObjects);
  else if (theId == "SHOW_CMD")
    showObjects(aObjects, true);
  else if (theId == "HIDE_CMD")
    showObjects(aObjects, false);
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
    if (aCustom && (/*!aCustom->isInitialized(aFeature) ||*/ aCustom == aSenderWidget)) {
      aCustom->storeValue(aFeature);
    }
  }
}

//**************************************************************
void XGUI_Workshop::activatePart(ResultPartPtr theFeature)
{
  changeCurrentDocument(theFeature);
  myObjectBrowser->activatePart(theFeature);
}

//**************************************************************
void XGUI_Workshop::activateLastPart()
{
  PluginManagerPtr aMgr = ModelAPI_PluginManager::get();
  DocumentPtr aDoc = aMgr->rootDocument();
  std::string aGrpName = ModelAPI_ResultPart::group();
  ObjectPtr aLastPart = aDoc->object(aGrpName, aDoc->size(aGrpName) - 1);
  ResultPartPtr aPart = boost::dynamic_pointer_cast<ModelAPI_ResultPart>(aLastPart);
  if (aPart)
    activatePart(aPart);
}

//**************************************************************
void XGUI_Workshop::deleteObjects(const QList<ObjectPtr>& theList)
{
  QMainWindow* aDesktop = isSalomeMode()? salomeConnector()->desktop() : myMainWindow;
  QMessageBox::StandardButton aRes = QMessageBox::warning(aDesktop, tr("Delete features"), 
                                                          tr("Seleted features will be deleted. Continue?"), 
                                                          QMessageBox::No | QMessageBox::Yes, QMessageBox::No);
  // ToDo: definbe deleting method
  /*  if (aRes == QMessageBox::Yes) {
    PluginManagerPtr aMgr = ModelAPI_PluginManager::get();
    aMgr->rootDocument()->startOperation();
    foreach (ObjectPtr aObj, theList) {
      ResultPartPtr aPart = boost::dynamic_pointer_cast<ModelAPI_ResultPart>(aObj);
      if (aPart) {
        DocumentPtr aDoc = aPart->document();
        if (aDoc == aMgr->currentDocument()) {
          aDoc->close();
        }
        aMgr->rootDocument()->removeFeature(aPart->owner());
      } else {
        aObj->document()->removeFeature(aObj);
      }
    }
    myDisplayer->updateViewer();
    aMgr->rootDocument()->finishOperation();
  }*/
}

//**************************************************************
void XGUI_Workshop::showObjects(const QList<ObjectPtr>& theList, bool isVisible)
{
  foreach (ObjectPtr aObj, theList) {
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
void XGUI_Workshop::updateCommandsOnViewSelection()
{
  PluginManagerPtr aMgr = ModelAPI_PluginManager::get();
  ModelAPI_ValidatorsFactory* aFactory = aMgr->validators();
  XGUI_Selection* aSelection = mySelector->selection();

  QList<QAction*> aActions = getModuleCommands();
  foreach(QAction* aAction, aActions) {
    QString aId = aAction->data().toString();
    const ModelAPI_Validator* aValidator = aFactory->validator(aId.toStdString());
    if (aValidator) {
      const ModuleBase_SelectionValidator* aSelValidator = 
        dynamic_cast<const ModuleBase_SelectionValidator*>(aValidator);
      if (aSelValidator) {
        aAction->setEnabled(aSelValidator->isValid(aSelection));
      }
    }
  }
}
