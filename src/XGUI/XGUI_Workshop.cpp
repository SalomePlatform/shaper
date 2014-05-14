#include "XGUI_Module.h"
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
#include "XGUI_ObjectsBrowser.h"
#include "XGUI_Displayer.h"
#include "XGUI_OperationMgr.h"
#include "XGUI_SalomeConnector.h"
#include "XGUI_SalomeViewer.h"
#include "XGUI_ActionsMgr.h"
#include "XGUI_ErrorDialog.h"
#include "XGUI_ViewerProxy.h"
#include "XGUI_PropertyPanel.h"

#include <Model_Events.h>
#include <ModelAPI_PluginManager.h>
#include <ModelAPI_Feature.h>
#include <ModelAPI_Data.h>
#include <ModelAPI_AttributeDocRef.h>

#include <Events_Loop.h>
#include <Events_Error.h>
#include <ModuleBase_Operation.h>
#include <ModuleBase_Operation.h>
#include <ModuleBase_OperationDescription.h>
#include <Config_FeatureMessage.h>
#include <Config_PointerMessage.h>

#include <QApplication>
#include <QFileDialog>
#include <QMessageBox>
#include <QMdiSubWindow>
#include <QPushButton>
#include <QDockWidget>
#include <QLayout>

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
  myCurrentFile(QString()),
  myPartSetModule(NULL),
  mySalomeConnector(theConnector),
  myPropertyPanel(0),
  myObjectBrowser(0),
  myDisplayer(0)
{
  myMainWindow = mySalomeConnector? 0 : new XGUI_MainWindow();

  myDisplayer = new XGUI_Displayer(this);

  mySelector = new XGUI_SelectionMgr(this);

  myOperationMgr = new XGUI_OperationMgr(this);
  myActionsMgr = new XGUI_ActionsMgr(this);
  myErrorDlg = new XGUI_ErrorDialog(myMainWindow);

  myViewerProxy = new XGUI_ViewerProxy(this);

  connect(myOperationMgr, SIGNAL(operationStarted()),  this, SLOT(onOperationStarted()));
  connect(myOperationMgr, SIGNAL(operationStopped(ModuleBase_Operation*)),
          this, SLOT(onOperationStopped(ModuleBase_Operation*)));
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
  Events_ID aFeatureId = aLoop->eventByName(EVENT_FEATURE_LOADED);
  aLoop->registerListener(this, aFeatureId);
  Events_ID aPartSetId = aLoop->eventByName("PartSetModuleEvent");
  aLoop->registerListener(this, aPartSetId);
  Events_ID aFeatureUpdatedId = aLoop->eventByName(EVENT_FEATURE_UPDATED);
  aLoop->registerListener(this, aFeatureUpdatedId);
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
  if (isSalomeMode()) {
    // Create only Undo, Redo commands
    salomeConnector()->addEditCommand("UNDO_CMD", 
                                      tr("Undo"), tr("Undo last command"),
                                      QIcon(":pictures/undo.png"), 
                                      false, this, SLOT(onUndo()),
                                      QKeySequence::Undo);
    salomeConnector()->addEditCommand("REDO_CMD", 
                                      tr("Redo"), tr("Redo last command"),
                                      QIcon(":pictures/redo.png"), 
                                      false, this, SLOT(onRedo()),
                                      QKeySequence::Redo);
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
    addFeature(aFeatureMsg);
    return;
  }
  //Update property panel on corresponding message. If there is no current operation (no
  //property panel), or received message has different feature to the current - do nothing.
  static Events_ID aFeatureUpdatedId = Events_Loop::loop()->eventByName(EVENT_FEATURE_UPDATED);
  if (theMessage->eventID() == aFeatureUpdatedId && myOperationMgr->hasOperation())
  {
    const Model_FeatureUpdatedMessage* anUpdateMsg =
        dynamic_cast<const Model_FeatureUpdatedMessage*>(theMessage);
    boost::shared_ptr<ModelAPI_Feature> aNewFeature = anUpdateMsg->feature();
    boost::shared_ptr<ModelAPI_Feature> aCurrentFeature = myOperationMgr->currentOperation()->feature();
    if(aNewFeature == aCurrentFeature) {
      myPropertyPanel->updateContentWidget(aCurrentFeature);
    }
  }
  //An operation passed by message. Start it, process and commit.
  const Config_PointerMessage* aPartSetMsg = dynamic_cast<const Config_PointerMessage*>(theMessage);
  if (aPartSetMsg) {
    ModuleBase_Operation* anOperation =
        (ModuleBase_Operation*)(aPartSetMsg->pointer());

    if (myOperationMgr->startOperation(anOperation)) {
      myPropertyPanel->updateContentWidget(anOperation->feature());
      if (anOperation->getDescription()->xmlRepresentation().isEmpty()) {
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
    myErrorDlg->show();
    myErrorDlg->raise();
    myErrorDlg->activateWindow();
  }

}

//******************************************************
void XGUI_Workshop::onOperationStarted()
{
  ModuleBase_Operation* aOperation = myOperationMgr->currentOperation();

  if(!aOperation->getDescription()->xmlRepresentation().isEmpty()) { //!< No need for property panel
    connectWithOperation(aOperation);

    showPropertyPanel();

    ModuleBase_WidgetFactory aFactory = ModuleBase_WidgetFactory(aOperation);
    QWidget* aContent = myPropertyPanel->contentWidget();
    qDeleteAll(aContent->children());
    aFactory.createWidget(aContent);
    myPropertyPanel->setModelWidgets(aFactory.getModelWidgets());
    myPropertyPanel->setWindowTitle(aOperation->getDescription()->description());
  }
}

//******************************************************
void XGUI_Workshop::onOperationStopped(ModuleBase_Operation* theOperation)
{
  ModuleBase_Operation* aOperation = myOperationMgr->currentOperation();

  //!< No need for property panel
  updateCommandStatus();
  hidePropertyPanel();
  if(myOperationMgr->operationsCount() > 1) {
    myActionsMgr->updateAction(theOperation->getDescription()->operationId());
    return;
  }
  if(!aOperation->getDescription()->xmlRepresentation().isEmpty()) { 
    myActionsMgr->restoreCommandState();
  }
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
  myIcons[QString(theMessage->id().c_str())] = QString(theMessage->icon().c_str());

  //Find or create Workbench
  QString aWchName = QString::fromStdString(theMessage->workbenchId());
  QString aNestedFeatures = QString::fromStdString(theMessage->nestedFeatures());
  bool isUsePropPanel = theMessage->isUseInput();
  if (isSalomeMode()) {
    QString aId = QString::fromStdString(theMessage->id());
    salomeConnector()->addFeature(aWchName,
                                  QString::fromStdString(theMessage->id()),
                                  aId,
                                  QString::fromStdString(theMessage->tooltip()),
                                  QIcon(theMessage->icon().c_str()),
                                  isUsePropPanel, this, 
                                  SLOT(onFeatureTriggered()), QKeySequence());
    myActionsMgr->addCommand(aId, salomeConnector()->command(aId));
    salomeConnector()->setNestedActions(aId, aNestedFeatures.split(" "));

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
    XGUI_Command* aCommand = aGroup->addFeature(QString::fromStdString(theMessage->id()),
                                                QString::fromStdString(theMessage->text()),
                                                QString::fromStdString(theMessage->tooltip()),
                                                QIcon(theMessage->icon().c_str()),
                                                QKeySequence(), isUsePropPanel);
    aCommand->setUnblockableCommands(aNestedFeatures.split(" "));
    myActionsMgr->addCommand(aCommand);
    myPartSetModule->featureCreated(aCommand);
  }
}

/*
 * Makes a signal/slot connections between Property Panel
 * and given operation. The given operation becomes a
 * current operation and previous operation if exists
 */
void XGUI_Workshop::connectWithOperation(ModuleBase_Operation* theOperation)
{
  QPushButton* aOkBtn = myPropertyPanel->findChild<QPushButton*>(XGUI::PROP_PANEL_OK);
  connect(aOkBtn, SIGNAL(clicked()), theOperation, SLOT(commit()));
  QPushButton* aCancelBtn = myPropertyPanel->findChild<QPushButton*>(XGUI::PROP_PANEL_CANCEL);
  connect(aCancelBtn, SIGNAL(clicked()), theOperation, SLOT(abort()));

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
  boost::shared_ptr<ModelAPI_PluginManager> aMgr = ModelAPI_PluginManager::get();
  boost::shared_ptr<ModelAPI_Document> aDoc = aMgr->rootDocument();
  aDoc->save(theName.toLatin1().constData());
  QApplication::restoreOverrideCursor();
}

//******************************************************
void XGUI_Workshop::onExit()
{
  boost::shared_ptr<ModelAPI_PluginManager> aMgr = ModelAPI_PluginManager::get();
  boost::shared_ptr<ModelAPI_Document> aDoc = aMgr->rootDocument();
  if(aDoc->isModified()) {
    int anAnswer = QMessageBox::question(
        myMainWindow, tr("Save current file"),
        tr("The document is modified, save before exit?"),
        QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel, QMessageBox::Cancel);
    if(anAnswer == QMessageBox::Save) {
      onSave();
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
  QApplication::restoreOverrideCursor();
}

//******************************************************
void XGUI_Workshop::onOpen()
{
  //save current file before close if modified
  boost::shared_ptr<ModelAPI_PluginManager> aMgr = ModelAPI_PluginManager::get();
  boost::shared_ptr<ModelAPI_Document> aDoc = aMgr->rootDocument();
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
    myCurrentFile = "";
  }

  //show file dialog, check if readable and open
  myCurrentFile = QFileDialog::getOpenFileName(mainWindow());
  if(myCurrentFile.isEmpty())
    return;
  QFileInfo aFileInfo(myCurrentFile);
  if(!aFileInfo.exists() || !aFileInfo.isReadable()) {
    QMessageBox::critical(myMainWindow, tr("Warning"), tr("Unable to open the file."));
    myCurrentFile = "";
    return;
  }
  QApplication::setOverrideCursor(Qt::WaitCursor);
  aDoc->load(myCurrentFile.toLatin1().constData());
  QApplication::restoreOverrideCursor();
  updateCommandStatus();
}

//******************************************************
void XGUI_Workshop::onSave()
{
  if(myCurrentFile.isEmpty()) {
    onSaveAs();
    return;
  }
  saveDocument(myCurrentFile);
  updateCommandStatus();
}

//******************************************************
void XGUI_Workshop::onSaveAs()
{
  QString aTemp = myCurrentFile;
  myCurrentFile = QFileDialog::getSaveFileName(mainWindow());
  if(myCurrentFile.isEmpty()) {
    myCurrentFile = aTemp;
    return;
  }
  QFileInfo aFileInfo(myCurrentFile);
  if(aFileInfo.exists() && !aFileInfo.isWritable()) {
    QMessageBox::critical(myMainWindow, tr("Warning"), tr("Unable to save the file."));
    return;
  }
  onSave();
}

//******************************************************
void XGUI_Workshop::onUndo()
{
  objectBrowser()->setCurrentIndex(QModelIndex());
  boost::shared_ptr<ModelAPI_PluginManager> aMgr = ModelAPI_PluginManager::get();
  boost::shared_ptr<ModelAPI_Document> aDoc = aMgr->rootDocument();
  aDoc->undo();
  updateCommandStatus();
}

//******************************************************
void XGUI_Workshop::onRedo()
{
  objectBrowser()->setCurrentIndex(QModelIndex());
  boost::shared_ptr<ModelAPI_PluginManager> aMgr = ModelAPI_PluginManager::get();
  boost::shared_ptr<ModelAPI_Document> aDoc = aMgr->rootDocument();
  aDoc->redo();
  updateCommandStatus();
}

//******************************************************
XGUI_Module* XGUI_Workshop::loadModule(const QString& theModule)
{
  QString libName = library(theModule);
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
  void* modLib = dlopen( libName.toLatin1(), RTLD_LAZY );
  if ( !modLib )
  err = QString( "Can not load library %1. %2" ).arg( libName ).arg( dlerror() );
  else
  {
    crtInst = (CREATE_FUNC)dlsym( modLib, CREATE_MODULE );
    if ( !crtInst )
    err = QString( "Failed to find function %1. %2" ).arg( CREATE_MODULE ).arg( dlerror() );
  }
#endif

  XGUI_Module* aModule = crtInst ? crtInst(this) : 0;

  if (!err.isEmpty()) {
    if (mainWindow() && mainWindow()->isVisible())
      QMessageBox::warning(mainWindow(), tr("Error"), err);
    else
      qWarning( qPrintable( err ));
  }
  return aModule;
}

//******************************************************
bool XGUI_Workshop::activateModule()
{
  myPartSetModule = loadModule("PartSet");
  if (!myPartSetModule)
    return false;
  myPartSetModule->createFeatures();
  return true;
}

//******************************************************
void XGUI_Workshop::updateCommandStatus()
{
  if (isSalomeMode()) // TODO: update commands in SALOME
    return;
  XGUI_MainMenu* aMenuBar = myMainWindow->menuObject();

  QList<XGUI_Command*> aCommands = aMenuBar->features();
  QList<XGUI_Command*>::const_iterator aIt;

  boost::shared_ptr<ModelAPI_PluginManager> aMgr = ModelAPI_PluginManager::get();
  if (aMgr->hasRootDocument()) {
    XGUI_Command* aUndoCmd;
    XGUI_Command* aRedoCmd;
    for (aIt = aCommands.constBegin(); aIt != aCommands.constEnd(); ++aIt) {
      if ((*aIt)->id() == "UNDO_CMD")
        aUndoCmd = (*aIt);
      else if ((*aIt)->id() == "REDO_CMD")
        aRedoCmd = (*aIt);
      else // Enable all commands
        (*aIt)->enable();
    }
    boost::shared_ptr<ModelAPI_Document> aDoc = aMgr->rootDocument();
    aUndoCmd->setEnabled(aDoc->canUndo());
    aRedoCmd->setEnabled(aDoc->canRedo());
  } else {
    for (aIt = aCommands.constBegin(); aIt != aCommands.constEnd(); ++aIt) {
      if ((*aIt)->id() == "NEW_CMD")
        (*aIt)->enable();
      else if ((*aIt)->id() == "EXIT_CMD")
        (*aIt)->enable();
      else 
        (*aIt)->disable();
    }
  }
}

//******************************************************
QDockWidget* XGUI_Workshop::createObjectBrowser(QWidget* theParent)
{
  QDockWidget* aObjDock = new QDockWidget(theParent);
  aObjDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
  aObjDock->setWindowTitle(tr("Object browser"));
  myObjectBrowser = new XGUI_ObjectsBrowser(aObjDock);
  connect(myObjectBrowser, SIGNAL(activePartChanged(FeaturePtr)), this, SLOT(changeCurrentDocument(FeaturePtr)));
  aObjDock->setWidget(myObjectBrowser);
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
      myPartSetModule->launchOperation(aId);
  }
}

//******************************************************
void XGUI_Workshop::changeCurrentDocument(FeaturePtr thePart)
{
  boost::shared_ptr<ModelAPI_PluginManager> aMgr = ModelAPI_PluginManager::get();
  if (thePart) {
    boost::shared_ptr<ModelAPI_AttributeDocRef> aDocRef = thePart->data()->docRef("PartDocument");
    if (aDocRef)
      aMgr->setCurrentDocument(aDocRef->value());
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
