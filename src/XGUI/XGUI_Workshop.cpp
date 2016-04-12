// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

//#include "XGUI_Constants.h"
#include "XGUI_Workshop.h"

#include "XGUI_ActionsMgr.h"
#include "XGUI_ColorDialog.h"
#include "XGUI_ContextMenuMgr.h"
#include "XGUI_Displayer.h"
#include "XGUI_ErrorDialog.h"
#include "XGUI_ErrorMgr.h"
#include "XGUI_ModuleConnector.h"
#include "XGUI_ObjectsBrowser.h"
#include "XGUI_OperationMgr.h"
#include "XGUI_PropertyPanel.h"
#include "XGUI_SalomeConnector.h"
#include "XGUI_Selection.h"
#include "XGUI_SelectionMgr.h"
#include "XGUI_Tools.h"
#include "XGUI_ViewerProxy.h"
#include "XGUI_WorkshopListener.h"
#include <XGUI_CustomPrs.h>
#include <XGUI_HistoryMenu.h>
#include <XGUI_QtEvents.h>

#ifndef HAVE_SALOME
#include <AppElements_Button.h>
#include <AppElements_Command.h>
#include <AppElements_MainMenu.h>
#include <AppElements_MainWindow.h>
#include <AppElements_MenuGroupPanel.h>
#include <AppElements_Viewer.h>
#include <AppElements_Workbench.h>
#endif

#include <ModelAPI_AttributeDocRef.h>
#include <ModelAPI_AttributeIntArray.h>
#include <ModelAPI_Data.h>
#include <ModelAPI_Events.h>
#include <ModelAPI_Feature.h>
#include <ModelAPI_Object.h>
#include <ModelAPI_ResultBody.h>
#include <ModelAPI_ResultConstruction.h>
#include <ModelAPI_ResultGroup.h>
#include <ModelAPI_ResultParameter.h>
#include <ModelAPI_Session.h>
#include <ModelAPI_Validator.h>
#include <ModelAPI_ResultCompSolid.h>
#include <ModelAPI_Tools.h>

//#include <PartSetPlugin_Part.h>

#include <Events_Loop.h>
#include <Events_Error.h>
#include <Events_LongOp.h>

#include <ModuleBase_FilterFactory.h>
#include <ModuleBase_IModule.h>
#include <ModuleBase_IViewer.h>
#include <ModuleBase_Operation.h>
#include <ModuleBase_OperationDescription.h>
#include <ModuleBase_PageBase.h>
#include <ModuleBase_Preferences.h>
#include <ModuleBase_SelectionValidator.h>
#include <ModuleBase_Tools.h>
#include <ModuleBase_WidgetFactory.h>
#include <ModuleBase_OperationFeature.h>
#include <ModuleBase_OperationAction.h>
#include <ModuleBase_PagedContainer.h>
#include <ModuleBase_WidgetValidated.h>

#include <Config_Common.h>
#include <Config_FeatureMessage.h>
#include <Config_ModuleReader.h>
#include <Config_PointerMessage.h>
#include <Config_PropManager.h>
#include <Config_SelectionFilterMessage.h>

#include <SUIT_ResourceMgr.h>

#include <QApplication>
#include <QFileDialog>
#include <QMessageBox>
#include <QMdiSubWindow>
#include <QMainWindow>
#include <QPushButton>
#include <QDockWidget>
#include <QLayout>
#include <QThread>
#include <QObject>
#include <QMenu>
#include <QToolButton>
#include <QAction>
#include <QDesktopWidget>

#include <iterator>

#ifdef _DEBUG
#include <QDebug>
#include <iostream>
#endif

#ifdef WIN32
#include <windows.h>
#else
#include <dlfcn.h>
#endif


QString XGUI_Workshop::MOVE_TO_END_COMMAND = QObject::tr("Move to the end");

//#define DEBUG_DELETE
//#define DEBUG_FEATURE_NAME

XGUI_Workshop::XGUI_Workshop(XGUI_SalomeConnector* theConnector)
    : QObject(),
      myCurrentDir(QString()),
      myModule(NULL),
      mySalomeConnector(theConnector),
      myPropertyPanel(0),
      myObjectBrowser(0),
      myDisplayer(0),
      myViewerSelMode(TopAbs_FACE)
{
#ifndef HAVE_SALOME
  myMainWindow = new AppElements_MainWindow();

  SUIT_ResourceMgr* aResMgr = ModuleBase_Preferences::resourceMgr();
  bool aCloc = aResMgr->booleanValue("language", "locale", true);
  if (aCloc)
    QLocale::setDefault( QLocale::c() );
  else 
    QLocale::setDefault( QLocale::system() );
#endif

  myDisplayer = new XGUI_Displayer(this);

  mySelector = new XGUI_SelectionMgr(this);
  connect(mySelector, SIGNAL(selectionChanged()), this, SLOT(updateCommandStatus()));

  myOperationMgr = new XGUI_OperationMgr(this, 0);
  myActionsMgr = new XGUI_ActionsMgr(this);
  myErrorDlg = new XGUI_ErrorDialog(QApplication::desktop());
  myContextMenuMgr = new XGUI_ContextMenuMgr(this);
  connect(myContextMenuMgr, SIGNAL(actionTriggered(const QString&, bool)), this,
          SLOT(onContextMenuCommand(const QString&, bool)));

  myViewerProxy = new XGUI_ViewerProxy(this);
  connect(myViewerProxy, SIGNAL(selectionChanged()),
          myActionsMgr,  SLOT(updateOnViewSelection()));

  myModuleConnector = new XGUI_ModuleConnector(this);

  ModuleBase_IWorkshop* aWorkshop = moduleConnector();
  myOperationMgr->setWorkshop(aWorkshop);

  myErrorMgr = new XGUI_ErrorMgr(this, aWorkshop);
  myEventsListener = new XGUI_WorkshopListener(aWorkshop);

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

#ifndef HAVE_SALOME
  connect(myMainWindow, SIGNAL(exitKeySequence()), SLOT(onExit()));
  onTrihedronVisibilityChanged(true);
#endif

  connect(this, SIGNAL(errorOccurred(const QString&)), myErrorDlg, SLOT(addError(const QString&)));
  connect(myEventsListener, SIGNAL(errorOccurred(const QString&)),
          myErrorDlg, SLOT(addError(const QString&)));

  //Config_PropManager::registerProp("Visualization", "object_default_color", "Object color",
  //                                 Config_Prop::Color, "225,225,225");

  Config_PropManager::registerProp("Visualization", "result_body_color", "Body color",
                                   Config_Prop::Color, ModelAPI_ResultBody::DEFAULT_COLOR());
  Config_PropManager::registerProp("Visualization", "result_group_color", "Group color",
                                   Config_Prop::Color, ModelAPI_ResultGroup::DEFAULT_COLOR());
  Config_PropManager::registerProp("Visualization", "result_construction_color", "Construction color",
                                   Config_Prop::Color, ModelAPI_ResultConstruction::DEFAULT_COLOR());
  Config_PropManager::registerProp("Visualization", "result_part_color", "Part color",
                                   Config_Prop::Color, ModelAPI_ResultPart::DEFAULT_COLOR());
  
  myViewerSelMode = 
    ModuleBase_Preferences::resourceMgr()->integerValue("Viewer", "selection", TopAbs_FACE);
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
  myEventsListener->initializeEventListening();

  registerValidators();

  // Calling of  loadCustomProps before activating module is required
  // by Config_PropManger to restore user-defined path to plugins
  ModuleBase_Preferences::loadCustomProps();
  createModule();

#ifndef HAVE_SALOME
  myMainWindow->show();
  updateCommandStatus();
#endif
  
  onNew();

  myViewerProxy->connectViewProxy();
  connect(myViewerProxy, SIGNAL(trihedronVisibilityChanged(bool)),
          SLOT(onTrihedronVisibilityChanged(bool)));

  emit applicationStarted();
}

void XGUI_Workshop::activateModule()
{
  myModule->activateSelectionFilters();

  connect(myDisplayer, SIGNAL(objectDisplayed(ObjectPtr, AISObjectPtr)),
    myModule, SLOT(onObjectDisplayed(ObjectPtr, AISObjectPtr)));
  connect(myDisplayer, SIGNAL(beforeObjectErase(ObjectPtr, AISObjectPtr)),
    myModule, SLOT(onBeforeObjectErase(ObjectPtr, AISObjectPtr)));

  updateCommandStatus();

  // TODO: get default selection mode

  // activate visualized objects in the viewer
  activateObjectsSelection(displayer()->displayedObjects());
  myOperationMgr->activate();
}

void XGUI_Workshop::deactivateModule()
{
  myModule->deactivateSelectionFilters();

  // remove internal displayer filter
  displayer()->deactivateSelectionFilters();

  disconnect(myDisplayer, SIGNAL(objectDisplayed(ObjectPtr, AISObjectPtr)),
    myModule, SLOT(onObjectDisplayed(ObjectPtr, AISObjectPtr)));
  disconnect(myDisplayer, SIGNAL(beforeObjectErase(ObjectPtr, AISObjectPtr)),
    myModule, SLOT(onBeforeObjectErase(ObjectPtr, AISObjectPtr)));

  XGUI_Displayer* aDisplayer = displayer();
  QObjectPtrList aDisplayed = aDisplayer->displayedObjects();
  aDisplayer->deactivateObjects(aDisplayed, true);

  myOperationMgr->deactivate();
}

//******************************************************
void XGUI_Workshop::initMenu()
{
  myContextMenuMgr->createActions();

#ifdef HAVE_SALOME
  // Create only Undo, Redo commands
  QAction* aAction = salomeConnector()->addDesktopCommand("UNDO_CMD", tr("Undo"),
                                                        tr("Undo last command"),
                                                        QIcon(":pictures/undo.png"),
                                                        QKeySequence::Undo, false, "MEN_DESK_EDIT");
  QString aToolBarTitle = tr( "INF_DESK_TOOLBAR_STANDARD" );
  salomeConnector()->addActionInToolbar( aAction,aToolBarTitle  );

  connect(aAction, SIGNAL(triggered(bool)), this, SLOT(onUndo()));
  addHistoryMenu(aAction, SIGNAL(updateUndoHistory(const QList<ActionInfo>&)), SLOT(onUndo(int)));

  aAction = salomeConnector()->addDesktopCommand("REDO_CMD", tr("Redo"), tr("Redo last command"),
                                              QIcon(":pictures/redo.png"), QKeySequence::Redo,
                                              false, "MEN_DESK_EDIT");
  salomeConnector()->addActionInToolbar( aAction, aToolBarTitle );

  connect(aAction, SIGNAL(triggered(bool)), this, SLOT(onRedo()));
  addHistoryMenu(aAction, SIGNAL(updateRedoHistory(const QList<ActionInfo>&)), SLOT(onRedo(int)));

  salomeConnector()->addDesktopMenuSeparator("MEN_DESK_EDIT");
  //aAction = salomeConnector()->addDesktopCommand("REBUILD_CMD", tr("Rebuild"), tr("Rebuild data objects"),
  //                                            QIcon(":pictures/rebuild.png"), QKeySequence(),
  //                                            false, "MEN_DESK_EDIT");
  //salomeConnector()->addActionInToolbar( aAction, aToolBarTitle );

  //connect(aAction, SIGNAL(triggered(bool)), this, SLOT(onRebuild()));
  //salomeConnector()->addDesktopMenuSeparator("MEN_DESK_EDIT");

  aAction = salomeConnector()->addDesktopCommand("SAVEAS_CMD", tr("Export native..."), tr("Export the current document into a native file"),
                                              QIcon(), QKeySequence(),
                                              false, "MEN_DESK_FILE");
  connect(aAction, SIGNAL(triggered(bool)), this, SLOT(onSaveAs()));

  aAction = salomeConnector()->addDesktopCommand("OPEN_CMD", tr("Import native..."), tr("Import native file"),
                                              QIcon(), QKeySequence(),
                                              false, "MEN_DESK_FILE");
  connect(aAction, SIGNAL(triggered(bool)), this, SLOT(onOpen()));
  salomeConnector()->addDesktopMenuSeparator("MEN_DESK_FILE");

#else
  // File commands group
  AppElements_MenuGroupPanel* aGroup = myMainWindow->menuObject()->generalPage();

  AppElements_Command* aCommand;

  aCommand = aGroup->addFeature("SAVE_CMD", tr("Save"), tr("Save the document"),
                                QIcon(":pictures/save.png"), QKeySequence::Save);
  aCommand->connectTo(this, SLOT(onSave()));
  //aCommand->disable();

  aCommand = aGroup->addFeature("SAVEAS_CMD", tr("Save as..."), tr("Save the document into a file"),
                                QIcon(":pictures/save.png"), QKeySequence());
  aCommand->connectTo(this, SLOT(onSaveAs()));

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

  //aCommand = aGroup->addFeature("REBUILD_CMD", tr("Rebuild"), tr("Rebuild data objects"),
  //  QIcon(":pictures/rebuild.png"), QKeySequence());
  //aCommand->connectTo(this, SLOT(onRebuild()));

  //aCommand->disable();

  aCommand = aGroup->addFeature("OPEN_CMD", tr("Open..."), tr("Open a new document"),
                                QIcon(":pictures/open.png"), QKeySequence::Open);
  aCommand->connectTo(this, SLOT(onOpen()));

  aCommand = aGroup->addFeature("PREF_CMD", tr("Preferences"), tr("Edit preferences"),
                                QIcon(":pictures/preferences.png"), QKeySequence::Preferences);
  aCommand->connectTo(this, SLOT(onPreferences()));

  aCommand = aGroup->addFeature("EXIT_CMD", tr("Exit"), tr("Exit application"),
                                QIcon(":pictures/close.png"), QKeySequence::Close);
  aCommand->connectTo(this, SLOT(onExit()));
#endif
}

#ifndef HAVE_SALOME
AppElements_Workbench* XGUI_Workshop::addWorkbench(const QString& theName)
{
  AppElements_MainMenu* aMenuBar = myMainWindow->menuObject();
  return aMenuBar->addWorkbench(theName);
}
#endif

//******************************************************
QMainWindow* XGUI_Workshop::desktop() const
{
#ifdef HAVE_SALOME
  return salomeConnector()->desktop();
#else
  return myMainWindow;
#endif
}

//******************************************************
void XGUI_Workshop::onStartWaiting()
{
  if (Events_LongOp::isPerformed()) {
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
  }
}

//******************************************************
void XGUI_Workshop::onAcceptActionClicked()
{
  QAction* anAction = dynamic_cast<QAction*>(sender());
  XGUI_OperationMgr* anOperationMgr = operationMgr();
  if (anOperationMgr) {
    ModuleBase_OperationFeature* aFOperation = dynamic_cast<ModuleBase_OperationFeature*>
                                                    (anOperationMgr->currentOperation());
    if (aFOperation) {
      //if (errorMgr()->canProcessClick(anAction, aFOperation->feature()))
      myOperationMgr->onCommitOperation();
    }
  }
}

//******************************************************
void XGUI_Workshop::onPreivewActionClicked()
{
  std::shared_ptr<Events_Message> aMsg = std::shared_ptr<Events_Message>(
                new Events_Message(Events_Loop::eventByName(EVENT_PREVIEW_REQUESTED)));
  Events_Loop::loop()->send(aMsg);
}

//******************************************************
void XGUI_Workshop::deactivateActiveObject(const ObjectPtr& theObject, const bool theUpdateViewer)
{
  if (!myModule->canActivateSelection(theObject)) {
    if (myDisplayer->isActive(theObject)) {
      QObjectPtrList anObjects;
      anObjects.append(theObject);
      myDisplayer->deactivateObjects(anObjects, theUpdateViewer);
    }
  }
}

//******************************************************
bool XGUI_Workshop::isFeatureOfNested(const FeaturePtr& theFeature)
{
  bool aHasNested = false;
  std::string aFeatureKind = theFeature->getKind();
#ifdef HAVE_SALOME
    XGUI_SalomeConnector* aSalomeConnector = salomeConnector();
    if (aSalomeConnector->isFeatureOfNested(actionsMgr()->action(aFeatureKind.c_str())))
      aHasNested = true;
#else 
    AppElements_MainMenu* aMenuBar = mainWindow()->menuObject();
    AppElements_Command* aCommand = aMenuBar->feature(aFeatureKind.c_str());
    if (aCommand && aCommand->button()->additionalButtonWidget())
      aHasNested = true;
#endif
  return aHasNested;
}

void XGUI_Workshop::setPropertyPanel(ModuleBase_Operation* theOperation)
{
  ModuleBase_OperationFeature* aFOperation = dynamic_cast<ModuleBase_OperationFeature*>(theOperation);
  if (!aFOperation)
    return;

  showPropertyPanel();
  myPropertyPanel->cleanContent();

  QList<ModuleBase_ModelWidget*> aWidgets;
  if (!module()->createWidgets(theOperation, aWidgets)) {
    QString aXmlRepr = aFOperation->getDescription()->xmlRepresentation();
    ModuleBase_WidgetFactory aFactory(aXmlRepr.toStdString(), myModuleConnector);
    aFactory.createWidget(myPropertyPanel->contentWidget());
    aWidgets = aFactory.getModelWidgets();
  }

  // check compatibility of feature and widgets
  FeaturePtr aFeature = aFOperation->feature();
  std::string aFeatureKind = aFeature->getKind();
  foreach (ModuleBase_ModelWidget* aWidget, aWidgets) {
    if (!aWidget->attributeID().empty() && !aFeature->attribute(aWidget->attributeID()).get()) {
      std::string anErrorMsg = "The feature '" + aFeatureKind + "' has no attribute '"
          + aWidget->attributeID() + "' used by widget '"
          + aWidget->metaObject()->className() + "'.";
      Events_Error::send(anErrorMsg);
      myPropertyPanel->cleanContent();
      return;
    }
  }
  foreach (ModuleBase_ModelWidget* aWidget, aWidgets) {
    bool isStoreValue = !aFOperation->isEditOperation() &&
                        !aWidget->getDefaultValue().empty() &&
                        !aWidget->isComputedDefault();
    aWidget->setFeature(aFeature, isStoreValue);
    aWidget->enableFocusProcessing();
  }

  // update visible state of Preview button
#ifdef HAVE_SALOME
  bool anIsAutoPreview = true;//mySalomeConnector->featureInfo(aFeatureKind)->isAutoPreview();
#else
  AppElements_MainMenu* aMenuBar = mainWindow()->menuObject();
  AppElements_Command* aCommand = aMenuBar->feature(aFeatureKind.c_str());
  bool anIsAutoPreview = aCommand && aCommand->featureMessage()->isAutoPreview();
#endif
  if (!anIsAutoPreview) {
    myPropertyPanel->findButton(PROP_PANEL_PREVIEW)->setVisible(true);
    // send signal about preview should not be computed automatically, click on preview
    // button should initiate it
    std::shared_ptr<Events_Message> aMsg = std::shared_ptr<Events_Message>(
                  new Events_Message(Events_Loop::eventByName(EVENT_PREVIEW_BLOCKED)));
    Events_Loop::loop()->send(aMsg);
  }
  myPropertyPanel->setModelWidgets(aWidgets);
  aFOperation->setPropertyPanel(myPropertyPanel);

  myModule->propertyPanelDefined(theOperation);

#ifndef DEBUG_FEATURE_NAME
  myPropertyPanel->setWindowTitle(theOperation->getDescription()->description());
#else
  std::string aFeatureName = aFeature->name();
  myPropertyPanel->setWindowTitle(QString("%1: %2").arg(theOperation->getDescription()->description())
                                                  .arg(aFeatureName.c_str()));
#endif

  myErrorMgr->setPropertyPanel(myPropertyPanel);
}

void XGUI_Workshop::connectToPropertyPanel(const bool isToConnect)
{
  XGUI_PropertyPanel* aPropertyPanel = propertyPanel();
  if (aPropertyPanel) {
    const QList<ModuleBase_ModelWidget*>& aWidgets = aPropertyPanel->modelWidgets();
    foreach (ModuleBase_ModelWidget* aWidget, aWidgets) {
       myModule->connectToPropertyPanel(aWidget, isToConnect);
       if (isToConnect) {
        connect(aWidget, SIGNAL(valueStateChanged(int)), this, SLOT(onWidgetStateChanged(int)));
        connect(aWidget, SIGNAL(valuesChanged()), this, SLOT(onValuesChanged()));
      }
      else {
        disconnect(aWidget, SIGNAL(valueStateChanged(int)), this, SLOT(onWidgetStateChanged(int)));
        disconnect(aWidget, SIGNAL(valuesChanged()), this, SLOT(onValuesChanged()));
      }
    }
  }
}

//******************************************************
void XGUI_Workshop::onOperationStarted(ModuleBase_Operation* theOperation)
{
  setGrantedFeatures(theOperation);
  myModule->operationStarted(theOperation);
}

//******************************************************
void XGUI_Workshop::onOperationResumed(ModuleBase_Operation* theOperation)
{
  setGrantedFeatures(theOperation);

  if (theOperation->getDescription()->hasXmlRepresentation()) {  //!< No need for property panel
    setPropertyPanel(theOperation);
    connectToPropertyPanel(true);
  }
  updateCommandStatus();

  myModule->operationResumed(theOperation);
}


//******************************************************
void XGUI_Workshop::onOperationStopped(ModuleBase_Operation* theOperation)
{
  updateCommandStatus();

  ModuleBase_OperationFeature* aFOperation = dynamic_cast<ModuleBase_OperationFeature*>
                                                                        (theOperation);
  if (!aFOperation)
    return;

  ModuleBase_ISelection* aSel = mySelector->selection();
  QObjectPtrList aObj = aSel->selectedPresentations();
  //!< No need for property panel
  hidePropertyPanel();
  myPropertyPanel->cleanContent();

  connectToPropertyPanel(false);
  myModule->operationStopped(aFOperation);

  // the deactivated objects of the current operation should be activated back.
  // They were deactivated on operation start or an object redisplay
  QObjectPtrList anObjects;
  FeaturePtr aFeature = aFOperation->feature();
  if (aFeature.get()) { // feature may be not created (plugin load fail)
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
  }
  activateObjectsSelection(anObjects);
}


void XGUI_Workshop::onOperationCommitted(ModuleBase_Operation* theOperation)
{
  myModule->operationCommitted(theOperation);
}

void XGUI_Workshop::onOperationAborted(ModuleBase_Operation* theOperation)
{
  myModule->operationAborted(theOperation);
}

void XGUI_Workshop::setGrantedFeatures(ModuleBase_Operation* theOperation)
{
  ModuleBase_OperationFeature* aFOperation = dynamic_cast<ModuleBase_OperationFeature*>(theOperation);
  if (!aFOperation)
    return;

  QStringList aGrantedIds;
  if (isSalomeMode())
    aGrantedIds = mySalomeConnector->nestedActions(theOperation->id());
  else
    aGrantedIds = myActionsMgr->nestedCommands(theOperation->id());

  ModuleBase_IModule* aModule = module();
  if (aModule)
    aModule->grantedOperationIds(theOperation, aGrantedIds);

  aFOperation->setGrantedOperationIds(aGrantedIds);
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

bool XGUI_Workshop::abortAllOperations()
{
  return myOperationMgr->abortAllOperations();
}

//******************************************************
void XGUI_Workshop::onOpen()
{
  if(!abortAllOperations())
    return;
  //save current file before close if modified
  SessionPtr aSession = ModelAPI_Session::get();
  if (aSession->isModified()) {
    //TODO(sbh): re-launch the app?
    int anAnswer = QMessageBox::question(
        desktop(), tr("Save current file"),
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
  myCurrentDir = QFileDialog::getExistingDirectory(desktop(), tr("Select directory"));
  if (myCurrentDir.isEmpty())
    return;
  QFileInfo aFileInfo(myCurrentDir);
  if (!aFileInfo.exists() || !aFileInfo.isReadable()) {
    QMessageBox::critical(desktop(), tr("Warning"), tr("Unable to open the file."));
    myCurrentDir = "";
    return;
  }
  QApplication::setOverrideCursor(Qt::WaitCursor);
  aSession->closeAll();
  aSession->load(myCurrentDir.toLatin1().constData());
  myObjectBrowser->rebuildDataTree();
  updateCommandStatus();
  QApplication::restoreOverrideCursor();
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
#ifndef HAVE_SALOME
  myMainWindow->showPythonConsole();
  QMdiSubWindow* aWnd = myMainWindow->viewer()->createView();
  aWnd->showMaximized();
  updateCommandStatus();
#endif
  myContextMenuMgr->connectViewer();
  QApplication::restoreOverrideCursor();
}

#ifndef HAVE_SALOME
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
        myMainWindow->viewer()->updateFromResources();
      } else if (aSection == ModuleBase_Preferences::MENU_SECTION) {
        myMainWindow->menuObject()->updateFromResources();
      }
    }
    displayer()->redisplayObjects();
  }
}
#endif

//******************************************************
void XGUI_Workshop::onTrihedronVisibilityChanged(bool theState)
{
  XGUI_Displayer* aDisplayer = displayer();
  if (aDisplayer)
    aDisplayer->displayTrihedron(theState);
}

//******************************************************
bool XGUI_Workshop::onSave()
{
  if(!abortAllOperations())
    return false;
  if (myCurrentDir.isEmpty()) {
    return onSaveAs();
  }
  std::list<std::string> aFiles;
  saveDocument(myCurrentDir, aFiles);
  updateCommandStatus();
#ifndef HAVE_SALOME
    myMainWindow->setModifiedState(false);
#endif
  return true;
}

//******************************************************
bool XGUI_Workshop::onSaveAs()
{
  if(!abortAllOperations())
    return false;
  QFileDialog dialog(desktop());
  dialog.setWindowTitle(tr("Select directory to save files..."));
  dialog.setFileMode(QFileDialog::Directory);
  dialog.setFilter(tr("Directories (*)"));
  dialog.setOptions(QFileDialog::HideNameFilterDetails | QFileDialog::ShowDirsOnly);
  dialog.setViewMode(QFileDialog::Detail);

  if (!dialog.exec()) {
    return false;
  }

  QString aTempDir = dialog.selectedFiles().first();
  QDir aDir(aTempDir);
  if (aDir.exists() && !aDir.entryInfoList(QDir::NoDotAndDotDot | QDir::AllEntries).isEmpty()) {
    int answer = QMessageBox::question(
        desktop(),
        //: Title of the dialog which asks user if he wants to save study in existing non-empty folder
        tr("Save"),
        tr("The directory already contains some files, save anyway?"),
        QMessageBox::Save | QMessageBox::Cancel);
    if (answer == QMessageBox::Cancel) {
      return false;
    }
  }
  myCurrentDir = aTempDir;
#ifndef HAVE_SALOME
    myMainWindow->setCurrentDir(myCurrentDir, false);
    myMainWindow->setModifiedState(false);
#endif
  return onSave();
}

//******************************************************
void XGUI_Workshop::onUndo(int theTimes)
{
  objectBrowser()->treeView()->setCurrentIndex(QModelIndex());
  SessionPtr aMgr = ModelAPI_Session::get();
  std::list<std::string> aUndoList = aMgr->undoList();
  if (aMgr->isOperation()) {
    /// this is important for nested operations
    /// when sketch operation is active, this condition is false and
    /// the sketch operation is not aborted
    operationMgr()->onAbortOperation();
  }
  std::list<std::string>::const_iterator aIt = aUndoList.cbegin();
  for (int i = 0; (i < theTimes) && (aIt != aUndoList.cend()); ++i, ++aIt) {
    aMgr->undo();
    if (QString((*aIt).c_str()) == MOVE_TO_END_COMMAND)
      myObjectBrowser->rebuildDataTree();
  }

  operationMgr()->updateApplyOfOperations();
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
  std::list<std::string> aRedoList = aMgr->redoList();
  if (aMgr->isOperation()) {
    /// this is important for nested operations
    /// when sketch operation is active, this condition is false and
    /// the sketch operation is not aborted
    operationMgr()->onAbortOperation();
  }
  std::list<std::string>::const_iterator aIt = aRedoList.cbegin();
  for (int i = 0; (i < theTimes) && (aIt != aRedoList.cend()); ++i, ++aIt) {
    aMgr->redo();
    if (QString((*aIt).c_str()) == MOVE_TO_END_COMMAND)
      myObjectBrowser->rebuildDataTree();
  }
  operationMgr()->updateApplyOfOperations();
  updateCommandStatus();

  // unblock the viewer update functionality and make update on purpose
  myDisplayer->enableUpdateViewer(isUpdateEnabled);
  myDisplayer->updateViewer();
}

//******************************************************
//void XGUI_Workshop::onRebuild()
//{
//  SessionPtr aMgr = ModelAPI_Session::get();
//  bool aWasOperation = aMgr->isOperation(); // keep this value
//  if (!aWasOperation) {
//    aMgr->startOperation("Rebuild");
//  }
//  static const Events_ID aRebuildEvent = Events_Loop::loop()->eventByName("Rebuild");
//  Events_Loop::loop()->send(std::shared_ptr<Events_Message>(
//    new Events_Message(aRebuildEvent, this)));
//  if (!aWasOperation) {
//    aMgr->finishOperation();
//  }
//  updateCommandStatus();
//}

//******************************************************
void XGUI_Workshop::onWidgetStateChanged(int thePreviousState)
{
  ModuleBase_ModelWidget* anActiveWidget = 0;
  ModuleBase_Operation* anOperation = myOperationMgr->currentOperation();
  if (anOperation) {
    ModuleBase_IPropertyPanel* aPanel = anOperation->propertyPanel();
    if (aPanel)
      anActiveWidget = aPanel->activeWidget();
  }
  if (anActiveWidget)
    operationMgr()->onValidateOperation();

  myModule->widgetStateChanged(thePreviousState);
}

//******************************************************
void XGUI_Workshop::onValuesChanged()
{
  ModuleBase_ModelWidget* aSenderWidget = (ModuleBase_ModelWidget*)(sender());
  if (!aSenderWidget || aSenderWidget->canAcceptFocus())
    return;

  ModuleBase_ModelWidget* anActiveWidget = 0;
  ModuleBase_Operation* anOperation = myOperationMgr->currentOperation();
  if (anOperation) {
    ModuleBase_IPropertyPanel* aPanel = anOperation->propertyPanel();
    if (aPanel)
      anActiveWidget = aPanel->activeWidget();
  }
  if (anActiveWidget) {
    ModuleBase_WidgetValidated* aWidgetValidated = dynamic_cast<ModuleBase_WidgetValidated*>
                                                                           (anActiveWidget);
    if (aWidgetValidated)
      aWidgetValidated->clearValidatedCash();
  }
}

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
    if (desktop()) {
      Events_Error::send(err.toStdString());
    } else {
      qWarning(qPrintable(err));
    }
  }
  return aModule;
}

//******************************************************
bool XGUI_Workshop::createModule()
{
  Config_ModuleReader aModuleReader;
  QString moduleName = QString::fromStdString(aModuleReader.getModuleName());
  myModule = loadModule(moduleName);
  if (!myModule)
    return false;

  //connect(myDisplayer, SIGNAL(objectDisplayed(ObjectPtr, AISObjectPtr)),
  //  myModule, SLOT(onObjectDisplayed(ObjectPtr, AISObjectPtr)));
  //connect(myDisplayer, SIGNAL(beforeObjectErase(ObjectPtr, AISObjectPtr)),
  //  myModule, SLOT(onBeforeObjectErase(ObjectPtr, AISObjectPtr)));

  myModule->createFeatures();
  //myActionsMgr->update();
  return true;
}

//******************************************************
void XGUI_Workshop::updateCommandStatus()
{
  QList<QAction*> aCommands;
#ifdef HAVE_SALOME
    aCommands = salomeConnector()->commandList();
#else
    AppElements_MainMenu* aMenuBar = myMainWindow->menuObject();
    foreach (AppElements_Command* aCmd, aMenuBar->features())
      aCommands.append(aCmd);
#endif
  SessionPtr aMgr = ModelAPI_Session::get();
  if (aMgr->hasModuleDocument()) {
    foreach(QAction* aCmd, aCommands) {
      QString aId = aCmd->data().toString();
      if (aId == "UNDO_CMD")
        aCmd->setEnabled(myModule->canUndo());
      else if (aId == "REDO_CMD")
        aCmd->setEnabled(myModule->canRedo());
      else
        // Enable all commands
        aCmd->setEnabled(true);
    }
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
  myActionsMgr->updateCommandsStatus();
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
  QMainWindow* aDesktop = desktop();
  QDockWidget* aObjDock = createObjectBrowser(aDesktop);
  aDesktop->addDockWidget(Qt::LeftDockWidgetArea, aObjDock);
  myPropertyPanel = new XGUI_PropertyPanel(aDesktop, myOperationMgr);
  myPropertyPanel->setupActions(myActionsMgr);
  myPropertyPanel->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea | Qt::BottomDockWidgetArea);
  aDesktop->addDockWidget(Qt::LeftDockWidgetArea, myPropertyPanel);
  hidePropertyPanel();  ///<! Invisible by default
  hideObjectBrowser();
  aDesktop->tabifyDockWidget(aObjDock, myPropertyPanel);
  myPropertyPanel->installEventFilter(myOperationMgr);

  QAction* aOkAct = myActionsMgr->operationStateAction(XGUI_ActionsMgr::Accept);
  connect(aOkAct, SIGNAL(triggered()), this, SLOT(onAcceptActionClicked()));

  QAction* aCancelAct = myActionsMgr->operationStateAction(XGUI_ActionsMgr::Abort);
  connect(aCancelAct, SIGNAL(triggered()), myOperationMgr, SLOT(onAbortOperation()));

  QAction* aPreviewAct = myActionsMgr->operationStateAction(XGUI_ActionsMgr::Preview);
  connect(aPreviewAct, SIGNAL(triggered()), this, SLOT(onPreivewActionClicked()));

  connect(myPropertyPanel, SIGNAL(keyReleased(QObject*, QKeyEvent*)),
          myOperationMgr,  SLOT(onKeyReleased(QObject*, QKeyEvent*)));
  connect(myPropertyPanel, SIGNAL(enterClicked(QObject*)),
          myOperationMgr,  SLOT(onProcessEnter(QObject*)));
}

//******************************************************
void XGUI_Workshop::showPropertyPanel()
{
  QAction* aViewAct = myPropertyPanel->toggleViewAction();
  ///<! Restore ability to close panel from the window's menu
  aViewAct->setEnabled(true);
  myPropertyPanel->show();
  myPropertyPanel->raise();

  // The next code is necessary to made the property panel the active window
  // in order to operation manager could process key events of the panel.
  // otherwise they are ignored. It happens only if the same(activateWindow) is
  // not happened by property panel activation(e.g. resume operation of Sketch)
  ModuleBase_Tools::activateWindow(myPropertyPanel, "XGUI_Workshop::showPropertyPanel()");
  ModuleBase_Tools::setFocus(myPropertyPanel, "XGUI_Workshop::showPropertyPanel()");
}

//******************************************************
void XGUI_Workshop::hidePropertyPanel()
{
  QAction* aViewAct = myPropertyPanel->toggleViewAction();
  ///<! Do not allow to show empty property panel
  aViewAct->setEnabled(false);
  myPropertyPanel->hide();

  // the property panel is active window of the desktop, when it is
  // hidden, it is undefined which window becomes active. By this reason
  // it is defined to perform the desktop as the active window.
  // in SALOME mode, workstack made the PyConsole the active window,
  // set the focus on it. As a result, shortcuts of the application, like
  // are processed by this console. For example Undo actions.
  // It is possible that this code is to be moved to SHAPER package
  QMainWindow* aDesktop = desktop();
  ModuleBase_Tools::activateWindow(aDesktop, "XGUI_Workshop::hidePropertyPanel()");
  ModuleBase_Tools::setFocus(aDesktop, "XGUI_Workshop::showPropertyPanel()");
}

//******************************************************
void XGUI_Workshop::showObjectBrowser()
{
  if (!isSalomeMode())
    myObjectBrowser->parentWidget()->show();
}

//******************************************************
void XGUI_Workshop::hideObjectBrowser()
{
  if (!isSalomeMode())
    myObjectBrowser->parentWidget()->hide();
}

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
  else if (theId == "CLEAN_HISTORY_CMD")
    cleanHistory();
  else if (theId == "MOVE_CMD")
    moveObjects();
  else if (theId == "COLOR_CMD")
    changeColor(aObjects);
  else if (theId == "SHOW_CMD") {
    showObjects(aObjects, true);
    mySelector->updateSelectionBy(ModuleBase_ISelection::Browser);
  }
  else if (theId == "HIDE_CMD")
    showObjects(aObjects, false);
  else if (theId == "SHOW_ONLY_CMD") {
    showOnlyObjects(aObjects);
    mySelector->updateSelectionBy(ModuleBase_ISelection::Browser);
  }
  else if (theId == "SHADING_CMD")
    setDisplayMode(aObjects, XGUI_Displayer::Shading);
  else if (theId == "WIREFRAME_CMD")
    setDisplayMode(aObjects, XGUI_Displayer::Wireframe);
  else if (theId == "HIDEALL_CMD") {
    QObjectPtrList aList = myDisplayer->displayedObjects();
    foreach (ObjectPtr aObj, aList) {
      if (module()->canEraseObject(aObj))
        aObj->setDisplayed(false);
    }
    Events_Loop::loop()->flush(Events_Loop::eventByName(EVENT_OBJECT_TO_REDISPLAY));
  } else if (theId == "SELECT_VERTEX_CMD") {
    setViewerSelectionMode(TopAbs_VERTEX);
  } else if (theId == "SELECT_EDGE_CMD") {
    setViewerSelectionMode(TopAbs_EDGE);
  } else if (theId == "SELECT_FACE_CMD") {
    setViewerSelectionMode(TopAbs_FACE);
  } else if (theId == "SELECT_RESULT_CMD") {
    setViewerSelectionMode(-1);
  } else if (theId == "SHOW_RESULTS_CMD") {
    highlightResults(aObjects);
  }
}

//**************************************************************
void XGUI_Workshop::setViewerSelectionMode(int theMode)
{
  myViewerSelMode = theMode;
  activateObjectsSelection(myDisplayer->displayedObjects());
}

//**************************************************************
void XGUI_Workshop::activateObjectsSelection(const QObjectPtrList& theList)
{
  QIntList aModes;
  module()->activeSelectionModes(aModes);
  if (aModes.isEmpty() && (myViewerSelMode != -1))
    aModes.append(myViewerSelMode);
  myDisplayer->activateObjects(aModes, theList);
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

  QObjectPtrList anObjects = mySelector->selection()->selectedObjects();
  if (!abortAllOperations())
    return;
  // check whether the object can be deleted. There should not be parts which are not loaded
  if (!XGUI_Tools::canRemoveOrRename(desktop(), anObjects))
    return;

  bool hasResult = false;
  bool hasFeature = false;
  bool hasParameter = false;
  bool hasCompositeOwner = false;
  ModuleBase_Tools::checkObjects(anObjects, hasResult, hasFeature, hasParameter, hasCompositeOwner);
  if (!(hasFeature || hasParameter))
    return;

  // 3. delete objects
  std::set<FeaturePtr> anIgnoredFeatures;
  std::set<FeaturePtr> aDirectRefFeatures, aIndirectRefFeatures;
  findReferences(anObjects, aDirectRefFeatures, aIndirectRefFeatures);

  bool doDeleteReferences = true;
  if (isDeleteFeatureWithReferences(anObjects, aDirectRefFeatures, aIndirectRefFeatures,
                                    desktop(), doDeleteReferences)) {
    // start operation
    QString aDescription = contextMenuMgr()->action("DELETE_CMD")->text();
    aDescription += " " + aDescription.arg(XGUI_Tools::unionOfObjectNames(anObjects, ", "));
    ModuleBase_OperationAction* anOpAction = new ModuleBase_OperationAction(aDescription, module());
    operationMgr()->startOperation(anOpAction);

    // WORKAROUND, should be done before each object remove, if it presents in XGUI_DataModel tree
    // It is necessary to clear selection in order to avoid selection changed event during
    // deletion and negative consequences connected with processing of already deleted items
    mySelector->clearSelection();

    // delete and commit/abort operation in model
    if (deleteFeaturesInternal(anObjects, aDirectRefFeatures, aIndirectRefFeatures,
                               anIgnoredFeatures, doDeleteReferences))
      operationMgr()->commitOperation();
    else
      operationMgr()->abortOperation(operationMgr()->currentOperation());
  }
}

//**************************************************************
void XGUI_Workshop::cleanHistory()
{
  if (!abortAllOperations())
    return;

  QObjectPtrList anObjects = mySelector->selection()->selectedObjects();
  QObjectPtrList aFeatures;
  foreach (ObjectPtr anObject, anObjects) {
    FeaturePtr aFeature = std::dynamic_pointer_cast<ModelAPI_Feature>(anObject);
    // for parameter result, use the corresponded reature to be removed
    if (!aFeature.get() && anObject->groupName() == ModelAPI_ResultParameter::group()) {
      aFeature = ModelAPI_Feature::feature(anObject);
    }
    aFeatures.append(aFeature);
  }

  // 1. find all referenced features
  QList<ObjectPtr> anUnusedObjects;
  std::set<FeaturePtr> aDirectRefFeatures;
  //foreach (ObjectPtr anObject, anObjects) {
  foreach (ObjectPtr anObject, aFeatures) {
    FeaturePtr aFeature = std::dynamic_pointer_cast<ModelAPI_Feature>(anObject);
    // for parameter result, use the corresponded reature to be removed
    //if (!aFeature.get() && anObject->groupName() == ModelAPI_ResultParameter::group()) {
    //  aFeature = ModelAPI_Feature::feature(anObject);
    //}
    if (aFeature.get()) {
      std::set<FeaturePtr> alreadyProcessed;
      aDirectRefFeatures.clear();
      XGUI_Tools::refsDirectToFeatureInAllDocuments(aFeature, aFeature, aFeatures,
                                                    aDirectRefFeatures, alreadyProcessed);
      if (aDirectRefFeatures.empty() && !anUnusedObjects.contains(aFeature))
        anUnusedObjects.append(aFeature);
    }
  }

  // 2. warn about the references remove, break the delete operation if the user chose it
  if (!anUnusedObjects.empty()) {
    QStringList aNames;
    foreach (const ObjectPtr& anObject, anUnusedObjects) {
      FeaturePtr aFeature = std::dynamic_pointer_cast<ModelAPI_Feature>(anObject);
      aNames.append(aFeature->name().c_str());
    }
    QString anUnusedNames = aNames.join(", ");

    QString anActionId = "CLEAN_HISTORY_CMD";
    QString aDescription = contextMenuMgr()->action(anActionId)->text();

    QMessageBox aMessageBox(desktop());
    aMessageBox.setWindowTitle(aDescription);
    aMessageBox.setIcon(QMessageBox::Warning);
    aMessageBox.setStandardButtons(QMessageBox::No | QMessageBox::Yes);
    aMessageBox.setDefaultButton(QMessageBox::No);

    QString aText = QString(tr("Unused features are the following: %1.\nThese features will be deleted.\nWould you like to continue?")
                   .arg(anUnusedNames));
    /*QString aText;
    if (anUnusedNames.isEmpty()) {
      aMessageBox.setStandardButtons(QMessageBox::Ok);
      aMessageBox.setDefaultButton(QMessageBox::Ok);

      aText = QString(tr("All features are relevant, there is nothing to be deleted"));
    }
    else {
      aMessageBox.setStandardButtons(QMessageBox::No | QMessageBox::Yes);
      aMessageBox.setDefaultButton(QMessageBox::No);

      aText = QString(tr("Unused features are the following: %1.\nThese features will be deleted.\nWould you like to continue?")
        .arg(anUnusedNames));
    }*/

    aMessageBox.setText(aText);
    if (aMessageBox.exec() == QMessageBox::No)
      return;

    // 1. start operation
    aDescription += "by deleting of " + aDescription.arg(XGUI_Tools::unionOfObjectNames(anObjects, ", "));
    ModuleBase_OperationAction* anOpAction = new ModuleBase_OperationAction(aDescription, module());
    operationMgr()->startOperation(anOpAction);

    // WORKAROUND, should be done before each object remove, if it presents in XGUI_DataModel tree
    // It is necessary to clear selection in order to avoid selection changed event during
    // deletion and negative consequences connected with processing of already deleted items
    mySelector->clearSelection();

    std::set<FeaturePtr> anIgnoredFeatures;
    if (removeFeatures(anUnusedObjects, anIgnoredFeatures, anActionId)) {
      operationMgr()->commitOperation();
    }
    else {
      operationMgr()->abortOperation(operationMgr()->currentOperation());
    }
  }
  else {
    QString anActionId = "CLEAN_HISTORY_CMD";
    QString aDescription = contextMenuMgr()->action(anActionId)->text();

    QMessageBox aMessageBox(desktop());
    aMessageBox.setWindowTitle(aDescription);
    aMessageBox.setIcon(QMessageBox::Warning);
    aMessageBox.setStandardButtons(QMessageBox::No | QMessageBox::Yes);
    aMessageBox.setDefaultButton(QMessageBox::No);

    QString aText;
    aMessageBox.setStandardButtons(QMessageBox::Ok);
    aMessageBox.setDefaultButton(QMessageBox::Ok);

    aText = QString(tr("All features are relevant, there is nothing to be deleted"));
    aMessageBox.setText(aText);

    if (aMessageBox.exec() == QMessageBox::No)
      return;
  }
}

//**************************************************************
void XGUI_Workshop::moveObjects()
{
  if (!abortAllOperations())
    return;

  SessionPtr aMgr = ModelAPI_Session::get();

  QString anActionId = "MOVE_CMD";
  QString aDescription = contextMenuMgr()->action(anActionId)->text();
  aMgr->startOperation(aDescription.toStdString());

  QObjectPtrList anObjects = mySelector->selection()->selectedObjects();
  // It is necessary to clear selection in order to avoid selection changed event during
  // moving and negative consequences connected with processing of already moved items
  mySelector->clearSelection();
  // check whether the object can be moved. There should not be parts which are not loaded
  if (!XGUI_Tools::canRemoveOrRename(desktop(), anObjects))
    return;

  DocumentPtr anActiveDocument = aMgr->activeDocument();
  FeaturePtr aCurrentFeature = anActiveDocument->currentFeature(true);
  foreach (ObjectPtr aObject, anObjects) {
    if (!myModule->canApplyAction(aObject, anActionId))
      continue;

    FeaturePtr aFeature = std::dynamic_pointer_cast<ModelAPI_Feature>(aObject);
    if (aFeature.get()) {
      anActiveDocument->moveFeature(aFeature, aCurrentFeature);
      aCurrentFeature = anActiveDocument->currentFeature(true);
    }
  }
  aMgr->finishOperation();
}

//**************************************************************
void XGUI_Workshop::findReferences(const QObjectPtrList& theList,
                                   std::set<FeaturePtr>& aDirectRefFeatures,
                                   std::set<FeaturePtr>& aIndirectRefFeatures)
{
  foreach (ObjectPtr aDeletedObj, theList) {
    std::set<FeaturePtr> alreadyProcessed;
    XGUI_Tools::refsToFeatureInAllDocuments(aDeletedObj, aDeletedObj, theList, aDirectRefFeatures,
                                            aIndirectRefFeatures, alreadyProcessed);
    std::set<FeaturePtr> aDifference;
    std::set_difference(aIndirectRefFeatures.begin(), aIndirectRefFeatures.end(), 
                        aDirectRefFeatures.begin(), aDirectRefFeatures.end(), 
                        std::inserter(aDifference, aDifference.begin()));
    aIndirectRefFeatures = aDifference;
  }
}

bool XGUI_Workshop::isDeleteFeatureWithReferences(const QObjectPtrList& theList,
                                   const std::set<FeaturePtr>& aDirectRefFeatures,
                                   const std::set<FeaturePtr>& aIndirectRefFeatures,
                                   QWidget* theParent,
                                   bool& doDeleteReferences)
{
  doDeleteReferences = true;

  QString aDirectNames, aIndirectNames;
  if (!aDirectRefFeatures.empty()) {
    QStringList aDirectRefNames;
    foreach (const FeaturePtr& aFeature, aDirectRefFeatures)
      aDirectRefNames.append(aFeature->name().c_str());
    aDirectNames = aDirectRefNames.join(", ");

    QStringList aIndirectRefNames;
    foreach (const FeaturePtr& aFeature, aIndirectRefFeatures)
      aIndirectRefNames.append(aFeature->name().c_str());
    aIndirectNames = aIndirectRefNames.join(", ");
  }

  bool aCanReplaceParameters = !aDirectRefFeatures.empty();
  QStringList aPartFeatureNames;
  foreach (ObjectPtr aObj, theList) {
    FeaturePtr aFeature = ModelAPI_Feature::feature(aObj);
    // invalid feature data means that the feature is already removed in model,
    // we needn't process it. E.g. delete of feature from create operation. The operation abort
    // will delete the operation
    if (!aFeature->data()->isValid())
      continue;
    ResultPtr aFirstResult = aFeature->firstResult();
    if (!aFirstResult.get())
      continue;
    std::string aResultGroupName = aFirstResult->groupName();
    if (aResultGroupName == ModelAPI_ResultPart::group())
      aPartFeatureNames.append(aFeature->name().c_str());

    if (aCanReplaceParameters && aResultGroupName != ModelAPI_ResultParameter::group())
      aCanReplaceParameters = false;
  }
  QString aPartNames = aPartFeatureNames.join(", ");

  QMessageBox aMessageBox(theParent);
  aMessageBox.setWindowTitle(tr("Delete features"));
  aMessageBox.setIcon(QMessageBox::Warning);
  aMessageBox.setStandardButtons(QMessageBox::No | QMessageBox::Yes);
  aMessageBox.setDefaultButton(QMessageBox::No);

  QString aText;
  if (!aDirectNames.isEmpty() || !aIndirectNames.isEmpty()) {
    if (aCanReplaceParameters) {
      aText = QString(tr("Selected parameters are used in the following features: %1.\nThese features will be deleted.\nOr parameters could be replaced by their values.\n")
                      .arg(aDirectNames));
      if (!aIndirectNames.isEmpty())
        aText += QString(tr("(Also these features will be deleted: %1)\n")).arg(aIndirectNames);
      QPushButton *aReplaceButton = aMessageBox.addButton(tr("Replace"), QMessageBox::ActionRole);
    } else {
      aText = QString(tr("Selected features are used in the following features: %1.\nThese features will be deleted.\n")).arg(aDirectNames);
      if (!aIndirectNames.isEmpty())
        aText += QString(tr("(Also these features will be deleted: %1)\n")).arg(aIndirectNames);
    }
  }
  if (!aPartNames.isEmpty())
    aText += QString(tr("The following parts will be deleted: %1.\n")).arg(aPartNames);

  if (!aText.isEmpty()) {
    aText += "Would you like to continue?";
    aMessageBox.setText(aText);
    aMessageBox.exec();
    QMessageBox::ButtonRole aButtonRole = aMessageBox.buttonRole(aMessageBox.clickedButton());

    if (aButtonRole == QMessageBox::NoRole)
      return false;

    if (aButtonRole == QMessageBox::ActionRole) {
      foreach (ObjectPtr aObj, theList)
        ModelAPI_ReplaceParameterMessage::send(aObj, this);
      doDeleteReferences = false;
    }
  }
  return true;
}

bool XGUI_Workshop::deleteFeatures(const QObjectPtrList& theFeatures,
                                   const std::set<FeaturePtr>& theIgnoredFeatures)
{
  std::set<FeaturePtr> aDirectRefFeatures, aIndirectRefFeatures;
  findReferences(theFeatures, aDirectRefFeatures, aIndirectRefFeatures);
  return deleteFeaturesInternal(theFeatures, aDirectRefFeatures, aIndirectRefFeatures,
                                theIgnoredFeatures);
}

bool XGUI_Workshop::deleteFeaturesInternal(const QObjectPtrList& theList,
                                           const std::set<FeaturePtr>& aDirectRefFeatures,
                                           const std::set<FeaturePtr>& aIndirectRefFeatures,
                                           const std::set<FeaturePtr>& theIgnoredFeatures,
                                           const bool doDeleteReferences)
{
  if (doDeleteReferences) {
    std::set<FeaturePtr> aFeaturesToDelete = aDirectRefFeatures;
    aFeaturesToDelete.insert(aIndirectRefFeatures.begin(), aIndirectRefFeatures.end());
    std::set<FeaturePtr>::const_iterator anIt = aFeaturesToDelete.begin(),
                                         aLast = aFeaturesToDelete.end();
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
  }

  QString anActionId = "DELETE_CMD";
  return removeFeatures(theList, theIgnoredFeatures, anActionId);
}

//**************************************************************
bool XGUI_Workshop::removeFeatures(const QObjectPtrList& theList,
                                   const std::set<FeaturePtr>& theIgnoredFeatures,
                                   const QString& theActionId)
{
  bool isDone = false;

  QString anId = QString::fromStdString(theActionId.toStdString().c_str());
  QStringList anObjectGroups = contextMenuMgr()->actionObjectGroups(anId);
  // 4. remove the parameter features
  foreach (ObjectPtr aObj, theList) {
    // features and parameters can be removed here,
    // the results are removed only by a corresponded feature remove
    std::string aGroupName = aObj->groupName();
    if (!anObjectGroups.contains(aGroupName.c_str()))
      continue;

    FeaturePtr aFeature = ModelAPI_Feature::feature(aObj);
    if (aFeature) {
      /*// TODO: to learn the workshop to delegate the Part object deletion to the PartSet module
      // part features are removed in the PartSet module. This condition should be moved there
      if (aFeature->getKind() == "Part")
        continue;
        */
      DocumentPtr aDoc = aObj->document();
      if (theIgnoredFeatures.find(aFeature) == theIgnoredFeatures.end()) {
#ifdef DEBUG_DELETE
        QString anInfoStr = ModuleBase_Tools::objectInfo(aFeature);
        anInfo.append(anInfoStr);
        qDebug(QString("remove feature :%1").arg(anInfoStr).toStdString().c_str());
#endif
        aDoc->removeFeature(aFeature);
        isDone = true;
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
// Returns the list of all features for theDocument and all features of
// all nested parts.
std::list<FeaturePtr> allFeatures(const DocumentPtr& theDocument)
{
  std::list<FeaturePtr> aResultList;
  std::list<FeaturePtr> anAllFeatures = theDocument->allFeatures();
  foreach (const FeaturePtr& aFeature, anAllFeatures) {
    // The order of appending features of the part and the part itself is important

    // Append features from a part feature
    foreach (const ResultPtr& aResult, aFeature->results()) {
      ResultPartPtr aResultPart =
          std::dynamic_pointer_cast<ModelAPI_ResultPart>(aResult);
      if (aResultPart.get() && aResultPart->partDoc().get()) {
        // Recursion
        std::list<FeaturePtr> anAllFeatures = allFeatures(aResultPart->partDoc());
        aResultList.insert(aResultList.end(), anAllFeatures.begin(), anAllFeatures.end());
      }
    }

    aResultList.push_back(aFeature);
  }
  return aResultList;
}

//**************************************************************
// Returns the list of features placed between theObject and the current feature
// in the same document. Excludes theObject, includes the current feature.
std::list<FeaturePtr> toCurrentFeatures(const ObjectPtr& theObject)
{
  std::list<FeaturePtr> aResult;
  DocumentPtr aDocument = theObject->document();
  std::list<FeaturePtr> anAllFeatures = allFeatures(aDocument);
  // find the object iterator
  std::list<FeaturePtr>::iterator aObjectIt = std::find(anAllFeatures.begin(), anAllFeatures.end(), theObject);
  if (aObjectIt == anAllFeatures.end())
    return aResult;
  // find the current feature iterator
  std::list<FeaturePtr>::iterator aCurrentIt = std::find(anAllFeatures.begin(), anAllFeatures.end(), aDocument->currentFeature(true));
  if (aCurrentIt == anAllFeatures.end())
    return aResult;
  // check the right order
  if (std::distance(aObjectIt, anAllFeatures.end()) <= std::distance(aCurrentIt, anAllFeatures.end()))
    return aResult;
  // exclude the object
  std::advance(aObjectIt, 1);
  // include the current feature
  std::advance(aCurrentIt, 1);
  return std::list<FeaturePtr>(aObjectIt, aCurrentIt);
}

bool XGUI_Workshop::canMoveFeature()
{
  QString anActionId = "MOVE_CMD";

  QObjectPtrList aObjects = mySelector->selection()->selectedObjects();
  QObjectPtrList aValidatedObjects;
  foreach (ObjectPtr aObject, aObjects) {
    if (!myModule->canApplyAction(aObject, anActionId))
      continue;
    // To be moved feature should be in active document
    if (aObject->document() != ModelAPI_Session::get()->activeDocument())
      continue;
    aValidatedObjects.append(aObject);
  }
  if (aValidatedObjects.size() != aObjects.size())
    aObjects = aValidatedObjects;

  bool aCanMove = !aObjects.empty();

  QObjectPtrList::const_iterator anIt = aObjects.begin(), aLast = aObjects.end();
  for (; anIt != aLast && aCanMove; anIt++) {
    ObjectPtr aObject = *anIt;
    // 1. Get features placed between selected and current in the document 
    std::list<FeaturePtr> aFeaturesBetween = toCurrentFeatures(aObject);
    // if aFeaturesBetween is empty it means wrong order or aObject is the current feature
    if (aFeaturesBetween.empty())
      aCanMove = false;
    else {
      std::set<FeaturePtr> aPlacedFeatures(aFeaturesBetween.begin(), aFeaturesBetween.end());
      // 2. Get all reference features to the selected object in the document 
      std::set<FeaturePtr> aRefFeatures;
      XGUI_Tools::refsToFeatureInFeatureDocument(aObject, aRefFeatures);

      if (aRefFeatures.empty())
        continue;
      else {
        // 3. Find any placed features in all reference features
        std::set<FeaturePtr> aIntersectionFeatures;
        std::set_intersection(aRefFeatures.begin(), aRefFeatures.end(),
                              aPlacedFeatures.begin(), aPlacedFeatures.end(),
                              std::inserter(aIntersectionFeatures, aIntersectionFeatures.begin()));
        // 4. Return false if any reference feature is placed before current feature
        if (!aIntersectionFeatures.empty())
          aCanMove = false;
      }
    }
  }
  return aCanMove;
}

//**************************************************************
bool XGUI_Workshop::canBeShaded(const ObjectPtr& theObject) const
{
  bool aCanBeShaded = myDisplayer->canBeShaded(theObject);
  if (!aCanBeShaded) {
    ResultCompSolidPtr aCompsolidResult =
                std::dynamic_pointer_cast<ModelAPI_ResultCompSolid>(theObject);
    if (aCompsolidResult.get() != NULL) { // change colors for all sub-solids
      for(int i = 0; i < aCompsolidResult->numberOfSubs() && !aCanBeShaded; i++)
        aCanBeShaded = myDisplayer->canBeShaded(aCompsolidResult->subResult(i));
    }
  }
  return aCanBeShaded;
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

void setColor(ResultPtr theResult, const std::vector<int>& theColor)
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
    ResultPtr aResult = std::dynamic_pointer_cast<ModelAPI_Result>(anObject);
    if (aResult.get()) {
      XGUI_CustomPrs::getResultColor(aResult, aColor);
    }
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
    if (!aColor.empty())
      break;
  }
  if (aColor.size() != 3)
    return;

  if (!abortAllOperations())
  return; 
  // 2. show the dialog to change the value
  XGUI_ColorDialog* aDlg = new XGUI_ColorDialog(desktop());
  aDlg->setColor(aColor);
  aDlg->move(QCursor::pos());
  bool isDone = aDlg->exec() == QDialog::Accepted;
  if (!isDone)
    return;

  bool isRandomColor = aDlg->isRandomColor();

  // 3. abort the previous operation and start a new one
  SessionPtr aMgr = ModelAPI_Session::get();
  QString aDescription = contextMenuMgr()->action("COLOR_CMD")->text();
  aMgr->startOperation(aDescription.toStdString());

  // 4. set the value to all results
  std::vector<int> aColorResult = aDlg->getColor();
  foreach(ObjectPtr anObj, theObjects) {
    ResultPtr aResult = std::dynamic_pointer_cast<ModelAPI_Result>(anObj);
    if (aResult.get() != NULL) {
      ResultCompSolidPtr aCompsolidResult = std::dynamic_pointer_cast<ModelAPI_ResultCompSolid>(aResult);
      if (aCompsolidResult.get() != NULL) { // change colors for all sub-solids
        for(int i = 0; i < aCompsolidResult->numberOfSubs(); i++) {
          setColor(aCompsolidResult->subResult(i), !isRandomColor ? aColorResult : aDlg->getRandomColor());
        }
      }
      setColor(aResult, !isRandomColor ? aColorResult : aDlg->getRandomColor());
    }
  }
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
    /*
    ResultPartPtr aPartRes = std::dynamic_pointer_cast<ModelAPI_ResultPart>(aObj);
    if (aPartRes) {
      DocumentPtr aDoc = aPartRes->partDoc();
      SET_DISPLAY_GROUP(ModelAPI_ResultBody::group(), isVisible)
      SET_DISPLAY_GROUP(ModelAPI_ResultConstruction::group(), isVisible)
      SET_DISPLAY_GROUP(ModelAPI_ResultGroup::group(), isVisible)
    } else {
    */
      aObj->setDisplayed(isVisible);
    //}
  }
  Events_Loop::loop()->flush(Events_Loop::eventByName(EVENT_OBJECT_TO_REDISPLAY));
}

//**************************************************************
void XGUI_Workshop::showOnlyObjects(const QObjectPtrList& theList)
{
  // Hide all displayed objects
  QObjectPtrList aList = myDisplayer->displayedObjects();
  foreach (ObjectPtr aObj, aList) {
    if (module()->canEraseObject(aObj))
      aObj->setDisplayed(false);
  }

  // Show only objects from the list
  foreach (ObjectPtr aObj, theList) {
    /*
    ResultPartPtr aPartRes = std::dynamic_pointer_cast<ModelAPI_ResultPart>(aObj);
    if (aPartRes) {
      DocumentPtr aDoc = aPartRes->partDoc();
      SET_DISPLAY_GROUP(ModelAPI_ResultBody::group(), true)
      SET_DISPLAY_GROUP(ModelAPI_ResultConstruction::group(), true)
      SET_DISPLAY_GROUP(ModelAPI_ResultGroup::group(), true)
    } else {
    */
      aObj->setDisplayed(true);
    //}
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

    ResultCompSolidPtr aCompsolidResult = std::dynamic_pointer_cast<ModelAPI_ResultCompSolid>(aObj);
    if (aCompsolidResult.get() != NULL) { // change colors for all sub-solids
      for(int i = 0; i < aCompsolidResult->numberOfSubs(); i++) {
          myDisplayer->setDisplayMode(aCompsolidResult->subResult(i),
                                      (XGUI_Displayer::DisplayMode)theMode, false);
      }
    }
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

  module()->closeDocument();

  SessionPtr aMgr = ModelAPI_Session::get();
  aMgr->closeAll();
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
    bool isEditing = anId.endsWith(ModuleBase_OperationFeature::EditSuffix());
    if (isEditing) {
      anId.chop(ModuleBase_OperationFeature::EditSuffix().size());
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

void XGUI_Workshop::synchronizeViewer()
{
  SessionPtr aMgr = ModelAPI_Session::get();
  DocumentPtr aDoc = aMgr->activeDocument();

  synchronizeGroupInViewer(aDoc, ModelAPI_ResultConstruction::group(), false);
  synchronizeGroupInViewer(aDoc, ModelAPI_ResultBody::group(), false);
  synchronizeGroupInViewer(aDoc, ModelAPI_ResultPart::group(), false);
  synchronizeGroupInViewer(aDoc, ModelAPI_ResultGroup::group(), false);
}

void XGUI_Workshop::synchronizeGroupInViewer(const DocumentPtr& theDoc, 
                                             const std::string& theGroup, 
                                             bool theUpdateViewer)
{
  ObjectPtr aObj;
  int aSize = theDoc->size(theGroup);
  for (int i = 0; i < aSize; i++) {
    aObj = theDoc->object(theGroup, i);
    if (aObj->isDisplayed()) {
      // Hide the presentation with an empty shape. But isDisplayed state of the object should not
      // be changed to the object becomes visible when the shape becomes not empty
      ResultPtr aRes = std::dynamic_pointer_cast<ModelAPI_Result>(aObj);
      if (aRes.get() && (!aRes->shape().get() || aRes->shape()->isNull()))
        continue;
      myDisplayer->display(aObj, false);
    }
  }
  if (theUpdateViewer)
    myDisplayer->updateViewer();
}

void XGUI_Workshop::highlightResults(const QObjectPtrList& theObjects)
{
  FeaturePtr aFeature;
  QObjectPtrList aSelList = theObjects;
  std::list<ResultPtr> aResList;
  foreach(ObjectPtr aObj, theObjects) {
    aFeature = std::dynamic_pointer_cast<ModelAPI_Feature>(aObj);
    if (aFeature.get()) {
      aResList = aFeature->results();
      std::list<std::shared_ptr<ModelAPI_Result> >::const_iterator aIt;
      for(aIt = aResList.cbegin(); aIt != aResList.cend(); aIt++) {
        aSelList.append(*aIt);
      }
    }
  }
  if (aSelList.count() > theObjects.count()) {
    // if something was found
    bool aBlocked = objectBrowser()->blockSignals(true);
    objectBrowser()->setObjectsSelected(aSelList);
    objectBrowser()->blockSignals(aBlocked);
  }
}
