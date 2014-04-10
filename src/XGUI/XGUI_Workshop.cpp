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
#include "XGUI_WidgetFactory.h"
#include "XGUI_SelectionMgr.h"

#include <ModelAPI_PluginManager.h>
#include <ModelAPI_Feature.h>
#include <ModelAPI_Object.h>
#include <ModelAPI_AttributeDocRef.h>

#include <Event_Loop.h>
#include <ModuleBase_Operation.h>
#include <Config_FeatureMessage.h>
#include <Config_PointerMessage.h>

#include <QApplication>
#include <QFileDialog>
#include <QMessageBox>
#include <QMdiSubWindow>
#include <QPushButton>
#include <QDockWidget>

#ifdef _DEBUG
#include <QDebug>
#endif

#ifdef WIN32
#include <windows.h>
#else
#include <dlfcn.h>
#endif

XGUI_Workshop::XGUI_Workshop()
  : QObject(), 
  myCurrentOperation(NULL),
  myPartSetModule(NULL)
{
  myMainWindow = new XGUI_MainWindow();
  mySelector = new XGUI_SelectionMgr(this);
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
  Event_Loop* aLoop = Event_Loop::loop();
  //TODO(sbh): Implement static method to extract event id [SEID]
  Event_ID aFeatureId = aLoop->eventByName("FeatureEvent");
  aLoop->registerListener(this, aFeatureId);
  Event_ID aPartSetId = aLoop->eventByName("PartSetModuleEvent");
  aLoop->registerListener(this, aPartSetId);
  activateModule();
  myMainWindow->show();

  // Testing of document creation
  //std::shared_ptr<ModelAPI_PluginManager> aMgr = ModelAPI_PluginManager::get();
  //std::shared_ptr<ModelAPI_Feature> aPoint1 = aMgr->rootDocument()->addFeature("Point");
  //std::shared_ptr<ModelAPI_Feature> aPart = aMgr->rootDocument()->addFeature("Part");
  //aPart->execute();
  //aMgr->setCurrentDocument(aPart->data()->docRef("PartDocument")->value());
  //std::shared_ptr<ModelAPI_Feature> aPoint2 = aMgr->rootDocument()->addFeature("Point");
  //aPoint2 = aMgr->rootDocument()->addFeature("Point");

  //aPart = aMgr->rootDocument()->addFeature("Part");
  //aPart->execute();
}

//******************************************************
void XGUI_Workshop::initMenu()
{
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

  aCommand = aGroup->addFeature("REDO_CMD", tr("Redo"), tr("Redo last command"),
                                QIcon(":pictures/redo.png"), QKeySequence::Redo);

  aCommand = aGroup->addFeature("REBUILD_CMD", tr("Rebuild"), tr("Rebuild data objects"),
                                QIcon(":pictures/rebuild.png"));

  aCommand = aGroup->addFeature("SAVEAS_CMD", tr("Save as..."), tr("Save the document into a file"),
                                QIcon(":pictures/save.png"));
  aCommand->connectTo(this, SLOT(onSaveAs()));
  //aCommand->disable();

  aCommand = aGroup->addFeature("OPEN_CMD", tr("Open..."), tr("Open a new document"),
                                QIcon(":pictures/open.png"), QKeySequence::Open);
  aCommand->connectTo(this, SLOT(onOpen()));

  aCommand = aGroup->addFeature("NEW_CMD", tr("New"), tr("Create a new document"),
                                QIcon(":pictures/new.png"), QKeySequence::New);
  aCommand->connectTo(this, SLOT(onNew()));

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
void XGUI_Workshop::processEvent(const Event_Message* theMessage)
{
  const Config_FeatureMessage* aFeatureMsg =
      dynamic_cast<const Config_FeatureMessage*>(theMessage);
  if (aFeatureMsg) {
    addFeature(aFeatureMsg);
    return;
  }
  const Config_PointerMessage* aPartSetMsg =
      dynamic_cast<const Config_PointerMessage*>(theMessage);
  if (aPartSetMsg) {
    ModuleBase_Operation* aOperation = (ModuleBase_Operation*)(aPartSetMsg->pointer());
    setCurrentOperation(aOperation);
    if(aOperation->xmlRepresentation().isEmpty()) { //!< No need for property panel
      myCurrentOperation->start();
      myCurrentOperation->commit();
    } else {
      fillPropertyPanel(aOperation);
    }
    return;
  }

#ifdef _DEBUG
  qDebug() << "XGUI_Workshop::ProcessEvent: "
  << "Catch message, but it can not be processed.";
#endif

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
  //Find or create Workbench
  XGUI_MainMenu* aMenuBar = myMainWindow->menuObject();
  QString aWchName = QString::fromStdString(theMessage->workbenchId());
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
  QString aFeatureId = QString::fromStdString(theMessage->id());
  XGUI_Command* aCommand = aGroup->addFeature(QString::fromStdString(theMessage->id()),
                                              QString::fromStdString(theMessage->text()),
                                              QString::fromStdString(theMessage->tooltip()),
                                              QIcon(theMessage->icon().c_str())
                                              //TODO(sbh): QKeySequence
                                                  );
  myPartSetModule->featureCreated(aCommand);
}

/*
 *
 */
void XGUI_Workshop::fillPropertyPanel(ModuleBase_Operation* theOperation)
{
  connectToPropertyPanel(theOperation);
  QWidget* aPropWidget = myMainWindow->findChild<QWidget*>(XGUI::PROP_PANEL_WDG);
  qDeleteAll(aPropWidget->children());
  theOperation->start();
  XGUI_WidgetFactory aFactory = XGUI_WidgetFactory(theOperation);
  aFactory.fillWidget(aPropWidget);
}

void XGUI_Workshop::setCurrentOperation(ModuleBase_Operation* theOperation)
{
  //FIXME: Ask user about aborting of current operation?
  if (myCurrentOperation) {
    //TODO get isOperation from document
    if (myCurrentOperation->isRunning())
      myCurrentOperation->abort();

    myCurrentOperation->deleteLater();
  }
  myCurrentOperation = theOperation;
}

/*
 * Makes a signal/slot connections between Property Panel
 * and given operation. The given operation becomes a
 * current operation and previous operation if exists
 */
void XGUI_Workshop::connectToPropertyPanel(ModuleBase_Operation* theOperation)
{
  QDockWidget* aPanel = myMainWindow->findChild<QDockWidget*>(XGUI::PROP_PANEL);
  QPushButton* aOkBtn = aPanel->findChild<QPushButton*>(XGUI::PROP_PANEL_OK);
  connect(aOkBtn, SIGNAL(clicked()), theOperation, SLOT(commit()));
  QPushButton* aCancelBtn = aPanel->findChild<QPushButton*>(XGUI::PROP_PANEL_CANCEL);
  connect(aCancelBtn, SIGNAL(clicked()), theOperation, SLOT(abort()));

  connect(theOperation, SIGNAL(started()), myMainWindow, SLOT(showPropertyPanel()));
  connect(theOperation, SIGNAL(stopped()), myMainWindow, SLOT(hidePropertyPanel()));
}

//******************************************************
void XGUI_Workshop::onExit()
{
  qApp->exit();
}

//******************************************************
void XGUI_Workshop::onNew()
{
  QApplication::setOverrideCursor(Qt::WaitCursor);
  myMainWindow->showObjectBrowser();
  myMainWindow->showPythonConsole();
  QMdiSubWindow* aWnd = myMainWindow->viewer()->createView();
  aWnd->showMaximized();
  QApplication::restoreOverrideCursor();
}

//******************************************************
void XGUI_Workshop::onOpen()
{
  QString aFileName = QFileDialog::getOpenFileName(mainWindow());
}

//******************************************************
void XGUI_Workshop::onSave()
{
}

//******************************************************
void XGUI_Workshop::onSaveAs()
{
  QString aFileName = QFileDialog::getSaveFileName(mainWindow());
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
    crtInst = (CREATE_FUNC)dlsym( modLib, GET_MODULE_NAME );
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
