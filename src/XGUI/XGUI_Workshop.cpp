// Copyright (C) 2014-2019  CEA/DEN, EDF R&D
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

#include "XGUI_Workshop.h"

#include "XGUI_ActionsMgr.h"
#include "XGUI_ActiveControlMgr.h"
#include "XGUI_ActiveControlSelector.h"
#include "XGUI_MenuMgr.h"
#include "XGUI_ColorDialog.h"
#include "XGUI_DeflectionDialog.h"
#include "XGUI_TransparencyWidget.h"
#include "XGUI_ContextMenuMgr.h"
#include "XGUI_Displayer.h"
#include "XGUI_ErrorDialog.h"
#include "XGUI_ErrorMgr.h"
#include "XGUI_FacesPanel.h"
#include "XGUI_FacesPanelSelector.h"
#include "XGUI_ModuleConnector.h"
#include "XGUI_ObjectsBrowser.h"
#include "XGUI_OperationMgr.h"
#include "XGUI_PropertyPanel.h"
#include "XGUI_PropertyPanelSelector.h"
#include "XGUI_PropertyDialog.h"
#include "XGUI_SalomeConnector.h"
#include "XGUI_Selection.h"
#include "XGUI_SelectionActivate.h"
#include "XGUI_SelectionMgr.h"
#include "XGUI_Tools.h"
#include "XGUI_ViewerProxy.h"
#include "XGUI_WorkshopListener.h"
#include <XGUI_HistoryMenu.h>
#include <XGUI_QtEvents.h>
#include <XGUI_DataModel.h>
#include <XGUI_InspectionPanel.h>
#include <XGUI_CompressFiles.h>

#ifndef HAVE_SALOME
#include <AppElements_Button.h>
#include <AppElements_Command.h>
#include <AppElements_MainMenu.h>
#include <AppElements_MainWindow.h>
#include <AppElements_MenuGroupPanel.h>
#include <AppElements_Viewer.h>
#include <AppElements_Workbench.h>
#endif

#include <Config_XMLReader.h>

#include <ModelAPI_AttributeDocRef.h>
#include <ModelAPI_AttributeIntArray.h>
#include <ModelAPI_AttributeDouble.h>
#include <ModelAPI_AttributeString.h>
#include <ModelAPI_Data.h>
#include <ModelAPI_Events.h>
#include <ModelAPI_Feature.h>
#include <ModelAPI_Object.h>
#include <ModelAPI_ResultBody.h>
#include <ModelAPI_ResultConstruction.h>
#include <ModelAPI_ResultGroup.h>
#include <ModelAPI_ResultParameter.h>
#include <ModelAPI_ResultField.h>
#include <ModelAPI_Session.h>
#include <ModelAPI_Validator.h>
#include <ModelAPI_Tools.h>
#include <ModelAPI_ResultField.h>
#include <ModuleBase_IconFactory.h>

//#include <PartSetPlugin_Part.h>

#include <Events_Loop.h>
#include <Events_InfoMessage.h>
#include <Events_LongOp.h>

#include <ExchangePlugin_ExportPart.h>
#include <ExchangePlugin_ImportPart.h>
#include <ExchangePlugin_Import.h>
#include <ExchangePlugin_ExportFeature.h>

#include <GeomAPI_Pnt.h>
#include <GeomAPI_ShapeExplorer.h>

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
#include <ModuleBase_PagedContainer.h>
#include <ModuleBase_WidgetValidated.h>
#include <ModuleBase_ModelWidget.h>
#include <ModuleBase_ResultPrs.h>
#include <ModuleBase_ActionIntParameter.h>
#include <ModuleBase_IStepPrs.h>

#include <Config_Common.h>
#include <Config_FeatureMessage.h>
#include <Config_ModuleReader.h>
#include <Config_PointerMessage.h>
#include <Config_PropManager.h>
#include <Config_DataModelReader.h>
#include <Config_Translator.h>
#include <Config_WidgetAPI.h>
#include <Config_Keywords.h>

#include <SUIT_ResourceMgr.h>

#include <AIS_Trihedron.hxx>
#ifdef BEFORE_TRIHEDRON_PATCH
#include <AIS_Point.hxx>
#include <AIS_Axis.hxx>
#endif

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
#include <QProcess>
#include <QDesktopServices>
#include <QFormLayout>
#include <QSpinBox>
#include <QDialogButtonBox>

#include <iterator>

#ifdef TINSPECTOR
#include <CDF_Session.hxx>
#include <CDF_Application.hxx>
#include <inspector/TInspector_Communicator.hxx>
#include <inspector/VInspector_CallBack.hxx>
static TInspector_Communicator* MyTCommunicator;
static Handle(VInspector_CallBack) MyVCallBack;

#endif

#ifdef _DEBUG
#include <QDebug>
#include <iostream>
#endif

#ifdef WIN32
#include <windows.h>
#else
#include <dlfcn.h>
#endif

//#define DEBUG_WITH_MESSAGE_REPORT

QString XGUI_Workshop::MOVE_TO_END_COMMAND = QObject::tr("Move to the end");
QString XGUI_Workshop::MOVE_TO_END_SPLIT_COMMAND = QObject::tr("Move to the end and split");

//#define DEBUG_DELETE
//#define DEBUG_FEATURE_NAME
//#define DEBUG_CLEAN_HISTORY

#ifdef HAVE_SALOME
static QString MyFilter(QObject::tr("SHAPER files (*.shaper *.cadbld)"));
static QString MyFilter2(QObject::tr("SHAPER files (*.shaper)"));
static QString MyExtension(".shaper");
#else
static QString MyFilter(QObject::tr("CAD Builder files (*.cadbld);;All files (*.*)"));
static QString MyFilter2(QObject::tr("CAD Builder files (*.cadbld)"));
static QString MyExtension(".cadbld");
#endif

static QString MyImportPartFilter(QObject::tr("Part files (*.shaperpart);;All files (*.*)"));


//******************************************************
XGUI_Workshop::XGUI_Workshop(XGUI_SalomeConnector* theConnector)
    : QObject(),
      myModule(NULL),
      mySalomeConnector(theConnector),
      myPropertyPanel(0),
      myInspectionPanel(0),
      myFacesPanel(0),
      myObjectBrowser(0),
      myDisplayer(0)
      //myViewerSelMode(TopAbs_FACE)
{
  mySelector = new XGUI_SelectionMgr(this);
  myModuleConnector = new XGUI_ModuleConnector(this);
  myOperationMgr = new XGUI_OperationMgr(this, 0);
  ModuleBase_IWorkshop* aWorkshop = moduleConnector();
  // Has to be defined first in order to get errors and messages from other components
  myEventsListener = new XGUI_WorkshopListener(this);
  mySelectionActivate = new XGUI_SelectionActivate(aWorkshop);

  SUIT_ResourceMgr* aResMgr = ModuleBase_Preferences::resourceMgr();
#ifndef HAVE_SALOME
  myMainWindow = new AppElements_MainWindow();

  bool aCloc = aResMgr->booleanValue("language", "locale", true);
  if (aCloc)
    QLocale::setDefault( QLocale::c() );
  else
    QLocale::setDefault( QLocale::system() );
#endif
  QString aPath = Config_XMLReader::pluginConfigFile().c_str();
  QDir aDir(aPath);

  // Load translations
  QStringList aLangs;
  aLangs << "*_en.ts"; // load by default eng translations

  /// If version of OCCT is 7.4.0 or more then it means that
  /// this is version of SALOME older then 9.4.0
#if OCC_VERSION_HEX >= 0x070400
  QString aCurrLang = aResMgr->language();
#else
  QString aCurrLang = aResMgr->stringValue("language", "language", "en");
#endif

  if(aCurrLang != "en") {
    aLangs << "*_" + aCurrLang + ".ts"; // then replace with translated files
  }

  foreach(QString aLang, aLangs) {
    QStringList aFilters;
    aFilters << aLang;
    QStringList aTsFiles = aDir.entryList(aFilters, QDir::Files);
    foreach(QString aFileName, aTsFiles) {
      Config_Translator::load(aFileName.toStdString());
    }
  }

  myDataModelXMLReader = new Config_DataModelReader();
  //myDataModelXMLReader->readAll();

  myDisplayer = new XGUI_Displayer(this);

  connect(mySelector, SIGNAL(selectionChanged()), this, SLOT(updateCommandStatus()));

  myActionsMgr = new XGUI_ActionsMgr(this);
  myActiveControlMgr = new XGUI_ActiveControlMgr(myModuleConnector);
  myMenuMgr = new XGUI_MenuMgr(this);
  myErrorDlg = new XGUI_ErrorDialog(QApplication::desktop());
  myContextMenuMgr = new XGUI_ContextMenuMgr(this);
  connect(myContextMenuMgr, SIGNAL(actionTriggered(const QString&, bool)), this,
          SLOT(onContextMenuCommand(const QString&, bool)));

  myViewerProxy = new XGUI_ViewerProxy(this);
  //connect(myViewerProxy, SIGNAL(selectionChanged()),
  //        myActionsMgr,  SLOT(updateOnViewSelection()));

  myOperationMgr->setWorkshop(aWorkshop);

  myErrorMgr = new XGUI_ErrorMgr(this, aWorkshop);

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
  myDisplayer->displayTrihedron(true);
#endif

  connect(myEventsListener, SIGNAL(errorOccurred(std::shared_ptr<Events_InfoMessage>)),
          myErrorDlg, SLOT(addError(std::shared_ptr<Events_InfoMessage>)));

  Config_PropManager::registerProp("Visualization", "selection_color", "Selection color",
    Config_Prop::Color, "255,255,255");

  if (ModuleBase_Preferences::resourceMgr()->booleanValue("Viewer", "face-selection", true))
    myViewerSelMode.append(TopAbs_FACE);
  if (ModuleBase_Preferences::resourceMgr()->booleanValue("Viewer", "edge-selection", true))
    myViewerSelMode.append(TopAbs_EDGE);
  if (ModuleBase_Preferences::resourceMgr()->booleanValue("Viewer", "vertex-selection", true))
    myViewerSelMode.append(TopAbs_VERTEX);
  //IMP: an attempt to use result selection with other selection modes
  myViewerSelMode.append(ModuleBase_ResultPrs::Sel_Result);//TopAbs_VERTEX);
  myViewerSelMode.append(TopAbs_COMPSOLID);
}

//******************************************************
XGUI_Workshop::~XGUI_Workshop(void)
{
#ifdef _DEBUG
#ifdef MISSED_TRANSLATION
  // Save Missed translations
  Config_Translator::saveMissedTranslations();
#endif
#endif

  delete myDisplayer;
  delete myDataModelXMLReader;
  delete mySelectionActivate;
  delete myMenuMgr;
  clearTemporaryDir();
}

//******************************************************
void XGUI_Workshop::startApplication()
{
  //Initialize event listening
  myEventsListener->initializeEventListening();

  myDataModelXMLReader->readAll();
  initMenu();

  Config_PropManager::registerProp("Plugins", "default_path", "Default Path",
                                   Config_Prop::Directory, "");

  std::string aDir = Config_XMLReader::resourcesConfigFile();
  Config_PropManager::registerProp("Plugins", "import_initial_path", "Import initial directory",
                                   Config_Prop::Directory, aDir);

#ifdef _DEBUG
  Config_PropManager::registerProp("Plugins", "create_part_by_start", "Create Part by Start",
    Config_Prop::Boolean, "false");

  Config_PropManager::registerProp("Plugins", "show_hide_faces", "Show Hide Faces (on the right)",
    Config_Prop::Boolean, "false");
#endif
  registerValidators();

  // Calling of  loadCustomProps before activating module is required
  // by Config_PropManger to restore user-defined path to plugins
  ModuleBase_Preferences::loadCustomProps();
  std::vector<int> aColor;
  try {
    aColor = Config_PropManager::color("Visualization", "selection_color");
  }
  catch (...) {
  }
  if (aColor.size() == 3)
    myDisplayer->setSelectionColor(aColor);

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

#ifdef _DEBUG
  bool aNewPart = Config_PropManager::boolean("Plugins", "create_part_by_start");
  if (aNewPart) {
      module()->launchOperation("Part", false); // PartSetPlugin_Part::ID()
  }
#endif
}

//******************************************************
void XGUI_Workshop::activateModule()
{
  selectionActivate()->updateSelectionFilters();

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

//******************************************************
void XGUI_Workshop::deactivateModule()
{
  // remove internal displayer filter
  displayer()->deactivateSelectionFilters(false);

  disconnect(myDisplayer, SIGNAL(objectDisplayed(ObjectPtr, AISObjectPtr)),
    myModule, SLOT(onObjectDisplayed(ObjectPtr, AISObjectPtr)));
  disconnect(myDisplayer, SIGNAL(beforeObjectErase(ObjectPtr, AISObjectPtr)),
    myModule, SLOT(onBeforeObjectErase(ObjectPtr, AISObjectPtr)));

  XGUI_Displayer* aDisplayer = displayer();
  QObjectPtrList aDisplayed = aDisplayer->displayedObjects();
  aDisplayer->deactivateObjects(aDisplayed, true);
  selectionActivate()->deactivateTrihedronInSelectionModes();

#ifdef BEFORE_TRIHEDRON_PATCH
  //Handle(AIS_Trihedron) aTrihedron = Handle(AIS_Trihedron)::DownCast(aDisplayer->getTrihedron());
  /// Trihedron problem: objects stayed in the viewer, should be removed manually
  /// otherwise in SALOME happens crash by HideAll in the viewer
  aContext->Remove(aTrihedron->Position(), true);
  aContext->Remove(aTrihedron->Axis(), true);
  aContext->Remove(aTrihedron->XAxis(), true);
  aContext->Remove(aTrihedron->YAxis(), true);
#endif

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
                                                        QKeySequence::Undo, false,
                                                        "MEN_DESK_EDIT");
  QString aToolBarTitle = tr( "INF_DESK_TOOLBAR_STANDARD" );
  salomeConnector()->addActionInToolbar( aAction,aToolBarTitle );

  connect(aAction, SIGNAL(triggered(bool)), this, SLOT(onUndo()));
  addHistoryMenu(aAction, SIGNAL(updateUndoHistory(const QList<ActionInfo>&)), SLOT(onUndo(int)));

  aAction = salomeConnector()->addDesktopCommand("REDO_CMD", tr("Redo"), tr("Redo last command"),
                                              QIcon(":pictures/redo.png"), QKeySequence::Redo,
                                              false, "MEN_DESK_EDIT");
  salomeConnector()->addActionInToolbar( aAction, aToolBarTitle );

  connect(aAction, SIGNAL(triggered(bool)), this, SLOT(onRedo()));
  addHistoryMenu(aAction, SIGNAL(updateRedoHistory(const QList<ActionInfo>&)), SLOT(onRedo(int)));

  salomeConnector()->addDesktopMenuSeparator("MEN_DESK_EDIT");

  //aAction = salomeConnector()->addDesktopCommand("AUTOCOMPUTE_CMD", tr("Auto rebuild"),
  //                                            tr("Blocks immediate apply of modifications"),
  //                                            QIcon(":pictures/autoapply.png"), QKeySequence(),
  //                                            false, "MEN_DESK_EDIT");
  //salomeConnector()->addActionInToolbar( aAction, aToolBarTitle );

  //connect(aAction, SIGNAL(triggered(bool)), this, SLOT(onAutoApply()));

  salomeConnector()->addDesktopMenuSeparator("MEN_DESK_EDIT");


  // Add commands to a file menu
  // Import sub-menu
  aAction = salomeConnector()->addDesktopCommand("OPEN_CMD", tr("Part set..."),
                                              tr("Import native file"),
                                              QIcon(), QKeySequence(),
                                              false, "MEN_DESK_FILE", tr("Import"), 10, 10);
  connect(aAction, SIGNAL(triggered(bool)), this, SLOT(onOpen()));

  aAction = salomeConnector()->addDesktopCommand("IMPORT_PART_CMD", tr("Part..."),
                                          tr("Import structure of a part"),
                                          QIcon(), QKeySequence(),
                                          false, "MEN_DESK_FILE", tr("Import"), 10, 10);
  connect(aAction, SIGNAL(triggered(bool)), this, SLOT(onImportPart()));

  aAction = salomeConnector()->addDesktopCommand("IMPORT_SHAPE_CMD", tr("From CAD format..."),
    tr("Import shape from a CAD format file"),
    ModuleBase_IconFactory::loadIcon("icons/Exchange/import.png"),
    QKeySequence(), false, "MEN_DESK_FILE", tr("Import"), 10, 10);
  connect(aAction, SIGNAL(triggered(bool)), this, SLOT(onImportShape()));

  // Export sub-menu
  aAction = salomeConnector()->addDesktopCommand("SAVEAS_CMD", tr("Part set..."),
                                             tr("Export the current document into a native file"),
                                              QIcon(), QKeySequence(),
                                              false, "MEN_DESK_FILE", tr("Export"), 10, 11);
  connect(aAction, SIGNAL(triggered(bool)), this, SLOT(onSaveAs()));

  aAction = salomeConnector()->addDesktopCommand("EXPORT_PART_CMD", tr("Part..."),
                                          tr("Export a part of the current document into a file"),
                                          QIcon(), QKeySequence(),
                                          false, "MEN_DESK_FILE", tr("Export"), 10, 11);
  connect(aAction, SIGNAL(triggered(bool)), this, SLOT(onExportPart()));

  aAction = salomeConnector()->addDesktopCommand("EXPORT_SHAPE_CMD", tr("To CAD format..."),
    tr("Export shape to a CAD format file"),
    ModuleBase_IconFactory::loadIcon("icons/Exchange/export.png"),
    QKeySequence(), false, "MEN_DESK_FILE", tr("Export"), 10, 11);
  connect(aAction, SIGNAL(triggered(bool)), this, SLOT(onExportShape()));

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

  aCommand = aGroup->addFeature("OPEN_CMD", tr("Open..."), tr("Open a new document"),
                                QIcon(":pictures/open.png"), QKeySequence::Open);
  aCommand->connectTo(this, SLOT(onOpen()));


  aCommand = aGroup->addFeature("AUTOCOMPUTE_CMD", tr("Auto rebuild"),
                                tr("Blocks immediate apply of modifications"),
                                QIcon(":pictures/autoapply_start.png"), QKeySequence());
  aCommand->setChecked(ModelAPI_Session::get()->isAutoUpdateBlocked());
  aCommand->connectTo(this, SLOT(onAutoApply()));

  aCommand = aGroup->addFeature("PREF_CMD", tr("Preferences"), tr("Edit preferences"),
                                QIcon(":pictures/preferences.png"), QKeySequence::Preferences);
  aCommand->connectTo(this, SLOT(onPreferences()));

  aCommand = aGroup->addFeature("EXIT_CMD", tr("Exit"), tr("Exit application"),
                                QIcon(":pictures/close.png"), QKeySequence::Close);
  aCommand->connectTo(this, SLOT(onExit()));
#endif
}

#ifndef HAVE_SALOME
//******************************************************
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
      myOperationMgr->commitOperation();
    }
  }
}

//******************************************************
void XGUI_Workshop::onAcceptPlusActionClicked()
{
  QAction* anAction = dynamic_cast<QAction*>(sender());
  XGUI_OperationMgr* anOperationMgr = operationMgr();
  if (anOperationMgr) {
    ModuleBase_OperationFeature* aFOperation = dynamic_cast<ModuleBase_OperationFeature*>
                                                    (anOperationMgr->currentOperation());
    if (aFOperation) {
      if (myOperationMgr->commitOperation())
        module()->launchOperation(aFOperation->id(), false);
    }
  }
}

//******************************************************
void XGUI_Workshop::onPreviewActionClicked()
{
  ModuleBase_IPropertyPanel* aPanel = propertyPanel();
  if (aPanel) {
    ModuleBase_ModelWidget* anActiveWidget = aPanel->activeWidget();
    if (anActiveWidget && anActiveWidget->getValueState() == ModuleBase_ModelWidget::ModifiedInPP) {
      anActiveWidget->storeValue();
    }
  }
  std::shared_ptr<Events_Message> aMsg = std::shared_ptr<Events_Message>(
                new Events_Message(Events_Loop::eventByName(EVENT_PREVIEW_REQUESTED)));
  Events_Loop::loop()->send(aMsg);
}


//******************************************************
void XGUI_Workshop::onHelpActionClicked()
{
  XGUI_OperationMgr* anOperationMgr = operationMgr();
  if (anOperationMgr) {
    ModuleBase_Operation* aOperation = anOperationMgr->currentOperation();
    if (aOperation) {
      QString aHelpPage = aOperation->helpFileName();
      if (!aHelpPage.isEmpty()) {
        QString aDocDir;
        const QChar aSep = QDir::separator();
//        QString platform;
//        SUIT_ResourceMgr* aResMgr = ModuleBase_Preferences::resourceMgr();
//#ifdef WIN32
//        platform = "winapplication";
//#else
//        platform = "application";
//#endif
//        QString aBrowserName = aResMgr->stringValue("ExternalBrowser", platform);

#ifdef HAVE_SALOME
        QString aDir(getenv("SHAPER_ROOT_DIR"));
        if (!aDir.isEmpty()) {
          aDocDir = aDir + aSep + "share" + aSep + "doc" + aSep +
            "salome" + aSep + "gui" + aSep + "SHAPER";
        }
#else
        QString aDir(getenv("CADBUILDER_ROOT_DIR"));
        aDocDir = aDir + aSep + "doc" + aSep + "gui";
#endif
        QString aFileName = aDocDir + aSep + aHelpPage;
        if (QFile::exists(aFileName)) {
          QUrl aUrl = QUrl::fromLocalFile(aFileName);
          QDesktopServices::openUrl(aUrl);
        }
      }
    }
  }
}


//******************************************************
void XGUI_Workshop::deactivateActiveObject(const ObjectPtr& theObject, const bool theUpdateViewer)
{
  if (!myModule->canActivateSelection(theObject)) {
    if (selectionActivate()->isActive(theObject)) {
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

//******************************************************
void XGUI_Workshop::fillPropertyPanel(ModuleBase_Operation* theOperation)
{
  ModuleBase_OperationFeature* aFOperation =
    dynamic_cast<ModuleBase_OperationFeature*>(theOperation);
  if (!aFOperation)
    return;

  myPropertyPanel->cleanContent();

  QList<ModuleBase_ModelWidget*> aWidgets;
  QString aXmlRepr = aFOperation->getDescription()->xmlRepresentation();
  if (!module()->createWidgets(aFOperation->feature(), aXmlRepr, aWidgets)) {
    ModuleBase_WidgetFactory aFactory(aXmlRepr.toStdString(), myModuleConnector);
    aFactory.createWidget(myPropertyPanel->contentWidget());
    aWidgets = aFactory.getModelWidgets();
  }

  // check compatibility of feature and widgets
  FeaturePtr aFeature = aFOperation->feature();
  std::string aFeatureKind = aFeature->getKind();
  foreach (ModuleBase_ModelWidget* aWidget, aWidgets) {
    if (aWidget->usesAttribute()) {
      if (!aWidget->attributeID().empty() && !aFeature->attribute(aWidget->attributeID()).get()) {
        std::string anErrorMsg = "The feature '%1' has no attribute '%2' used by widget '%3'.";
        Events_InfoMessage("XGUI_Workshop", anErrorMsg)
          .arg(aFeatureKind).arg(aWidget->attributeID())
          .arg(aWidget->metaObject()->className()).send();
        myPropertyPanel->cleanContent();
        return;
      }
    }
  }
  // for performance purpose, flush should be done after all controls are filled
  bool isUpdateFlushed = false;
  foreach (ModuleBase_ModelWidget* aWidget, aWidgets) {
    bool isStoreValue = !aFOperation->isEditOperation() &&
                        !aWidget->getDefaultValue().empty() &&
                        !aWidget->isComputedDefault();
    aWidget->setFeature(aFeature, isStoreValue, isUpdateFlushed);
    if (!isStoreValue)
      aWidget->restoreValue();
    aWidget->enableFocusProcessing();
  }
  // update visible state of Preview button
  std::shared_ptr<Config_FeatureMessage> aFeatureInfo;
#ifdef HAVE_SALOME
  aFeatureInfo = mySalomeConnector->featureInfo(aFeatureKind.c_str());
#else
  AppElements_MainMenu* aMenuBar = mainWindow()->menuObject();
  AppElements_Command* aCommand = aMenuBar->feature(aFeatureKind.c_str());
  if (aCommand)
    aFeatureInfo = aCommand->featureMessage();
#endif
  bool anIsAutoPreview = true;
  if (aFeatureInfo.get()) {
    anIsAutoPreview = aFeatureInfo->isAutoPreview();
    theOperation->setHelpFileName(aFeatureInfo->helpFileName().c_str());
  } else {
    std::string aXmlCfg, aDescription;
    module()->getXMLRepresentation(aFeatureKind, aXmlCfg, aDescription);
    ModuleBase_WidgetFactory aFactory(aXmlCfg, moduleConnector());
    anIsAutoPreview = aFactory.widgetAPI()->getBooleanAttribute(FEATURE_AUTO_PREVIEW, true);
  }

  if (!anIsAutoPreview) {
    myPropertyPanel->findButton(PROP_PANEL_PREVIEW)->setVisible(true);
    // send signal about preview should not be computed automatically, click on preview
    // button should initiate it
    std::shared_ptr<Events_Message> aMsg = std::shared_ptr<Events_Message>(
                  new Events_Message(Events_Loop::eventByName(EVENT_PREVIEW_BLOCKED)));
    Events_Loop::loop()->send(aMsg);
  }
  // if update happens after preview is blocked, it does nothing when blocked
  // it improves performance for valid objects on feature start
  ModuleBase_Tools::flushUpdated(aFeature);

  myPropertyPanel->setModelWidgets(aWidgets);
  aFOperation->setPropertyPanel(myPropertyPanel);

  myModule->propertyPanelDefined(theOperation);

#ifndef DEBUG_FEATURE_NAME
  myPropertyPanel->setWindowTitle(ModuleBase_Tools::translate("workshop",
    theOperation->getDescription()->description().toStdString()));
#else
  std::string aFeatureName = aFeature->name();
  myPropertyPanel->setWindowTitle(QString("%1: %2")
    .arg(translate(theOperation->getDescription()->description()))
    .arg(translate(aFeatureName.c_str())));
#endif

  myErrorMgr->setPropertyPanel(myPropertyPanel);
  if (Config_PropManager::boolean("Windows", "use_hide_faces_panel")) {
    theOperation->setHideFacesVisible(myFacesPanel->isVisible());
    if (aFeatureInfo.get() && aFeatureInfo->isHideFacesPanel() && !myFacesPanel->isVisible())
      myFacesPanel->show();
  }
  showPanel(myPropertyPanel);
}

//******************************************************
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
        connect(aWidget, SIGNAL(objectUpdated()), this, SLOT(onWidgetObjectUpdated()));
       }
      else {
        disconnect(aWidget, SIGNAL(valueStateChanged(int)), this, SLOT(onWidgetStateChanged(int)));
        disconnect(aWidget, SIGNAL(valuesChanged()), this, SLOT(onValuesChanged()));
        disconnect(aWidget, SIGNAL(objectUpdated()), this, SLOT(onWidgetObjectUpdated()));
      }
    }
  }
}

//******************************************************
void XGUI_Workshop::onOperationResumed(ModuleBase_Operation* theOperation)
{
  setGrantedFeatures(theOperation);

  if (theOperation->getDescription()->hasXmlRepresentation()) {  //!< No need for property panel
    fillPropertyPanel(theOperation);
    connectToPropertyPanel(true);
    ModuleBase_OperationFeature* aFOperation = dynamic_cast<ModuleBase_OperationFeature*>
      (theOperation);
    if (aFOperation)
      myPropertyPanel->updateApplyPlusButton(aFOperation->feature());
    else
      myPropertyPanel->updateApplyPlusButton(FeaturePtr());
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
  hidePanel(myPropertyPanel);
  myPropertyPanel->cleanContent();

  connectToPropertyPanel(false);
  myModule->operationStopped(aFOperation);

  // the deactivated objects of the current operation should be activated back.
  // They were deactivated on operation start or an object redisplay
  QObjectPtrList anObjects;
  FeaturePtr aFeature = aFOperation->feature();
  if (aFeature.get()) { // feature may be not created (plugin load fail)
    if (myDisplayer->isVisible(aFeature) && !selectionActivate()->isActive(aFeature))
      anObjects.append(aFeature);
    std::list<ResultPtr> aResults;
    ModelAPI_Tools::allResults(aFeature, aResults);
    std::list<ResultPtr>::const_iterator aIt;
    for (aIt = aResults.begin(); aIt != aResults.end(); ++aIt) {
      ResultPtr anObject = *aIt;
      if (myDisplayer->isVisible(anObject) && !selectionActivate()->isActive(anObject)) {
        anObjects.append(anObject);
      }
    }
  }
  activateObjectsSelection(anObjects);

  if (Config_PropManager::boolean("Windows", "use_hide_faces_panel")) {
    if (!theOperation->isHideFacesVisible())
      myFacesPanel->close();
  }
}

//******************************************************
void XGUI_Workshop::onOperationCommitted(ModuleBase_Operation* theOperation)
{
  myModule->operationCommitted(theOperation);
}

//******************************************************
void XGUI_Workshop::onOperationAborted(ModuleBase_Operation* theOperation)
{
  myModule->operationAborted(theOperation);
}

//******************************************************
void XGUI_Workshop::setGrantedFeatures(ModuleBase_Operation* theOperation)
{
  ModuleBase_OperationFeature* aFOperation =
    dynamic_cast<ModuleBase_OperationFeature*>(theOperation);
  if (!aFOperation)
    return;

  QStringList aGrantedIds;
  if (isSalomeMode()) {
    const std::shared_ptr<Config_FeatureMessage>& anInfo =
                         mySalomeConnector->featureInfo(theOperation->id());
    if (anInfo.get())
      aGrantedIds = QString::fromStdString(anInfo->nestedFeatures())
                                   .split(" ", QString::SkipEmptyParts);
  }
  else
    aGrantedIds = myActionsMgr->nestedCommands(theOperation->id());

  ModuleBase_IModule* aModule = module();
  if (aModule)
    aModule->grantedOperationIds(theOperation, aGrantedIds);

  aFOperation->setGrantedOperationIds(aGrantedIds);
}

//******************************************************
void XGUI_Workshop::saveDocument(const QString& theName, std::list<std::string>& theFileNames)
{
  QApplication::restoreOverrideCursor();
  SessionPtr aMgr = ModelAPI_Session::get();

  std::list<DocumentPtr> aDocList = aMgr->allOpenedDocuments();
  std::list<DocumentPtr>::const_iterator aIt;
  for (aIt = aDocList.cbegin(); aIt != aDocList.cend(); aIt++) {
    std::list<bool> aState = myObjectBrowser->getStateForDoc(*aIt);
    (*aIt)->storeNodesState(aState);
  }

  aMgr->save(theName.toLatin1().constData(), theFileNames);

  QApplication::restoreOverrideCursor();
}

//******************************************************
bool XGUI_Workshop::abortAllOperations()
{
  return myOperationMgr->abortAllOperations();
}

//******************************************************
void XGUI_Workshop::operationStarted(ModuleBase_Operation* theOperation)
{
  setGrantedFeatures(theOperation);
  if (!theOperation->getDescription()->hasXmlRepresentation()) {  //!< No need for property panel
    updateCommandStatus();
  }
  else {
    myModule->operationStarted(theOperation);
  }
}

//******************************************************
void XGUI_Workshop::onOpen()
{
  if(!abortAllOperations())
    return;
  //save current file before close if modified
  SessionPtr aSession = ModelAPI_Session::get();
  if (aSession->isModified()) {
    int anAnswer = QMessageBox::question(
        desktop(), tr("Save current file"),
        tr("The document is modified, save before opening another?"),
        QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel, QMessageBox::Cancel);
    if (anAnswer == QMessageBox::Save) {
      onSave();
    } else if (anAnswer == QMessageBox::Cancel) {
      return;
    }
    myCurrentFile = QString();
  }

  //show file dialog, check if readable and open
  qreal aRatio = ModuleBase_Tools::currentPixelRatio();
  // If the ratio is > 1 (HD screen) then QT has a bug in
  // displaying of system open file dialog (too small)
  QString aFile = QFileDialog::getOpenFileName(desktop(), tr("Open file"), QString(), MyFilter,
    Q_NULLPTR, ((aRatio > 1)? QFileDialog::DontUseNativeDialog : QFileDialog::Options()));
  if (!aFile.isNull())
    openFile(aFile);
}

//******************************************************
void XGUI_Workshop::openFile(const QString& theDirectory)
{
  myCurrentFile = theDirectory;
  if (myCurrentFile.isEmpty())
    return;

  QFileInfo aFileInfo(myCurrentFile);
  if (!aFileInfo.exists() || !aFileInfo.isReadable()) {
    QMessageBox::critical(desktop(), tr("Warning"), tr("Unable to open the file."));
    myCurrentFile = QString();
    return;
  }

  QApplication::setOverrideCursor(Qt::WaitCursor);
  module()->closeDocument();
  SessionPtr aSession = ModelAPI_Session::get();
  aSession->closeAll();

  clearTemporaryDir();
  if (!XGUI_CompressFiles::uncompress(myCurrentFile, myTmpDir.path()))
    return;

  aSession->load(myTmpDir.path().toLatin1().constData());
  myObjectBrowser->rebuildDataTree();

  // Open first level of data tree
  DocumentPtr aRootDoc = aSession->moduleDocument();
  std::list<bool> aStates;
  aRootDoc->restoreNodesState(aStates);
  myObjectBrowser->setStateForDoc(aRootDoc, aStates);

  updateCommandStatus();
#ifndef HAVE_SALOME
  myMainWindow->setCurrentDir(myCurrentFile, true);
#endif

#ifdef _DEBUG
  bool aNewPart = Config_PropManager::boolean("Plugins", "create_part_by_start");
  if (aNewPart) {

    DocumentPtr aRootDoc = ModelAPI_Session::get()->moduleDocument();
    int aSize = aRootDoc->size(ModelAPI_ResultPart::group());
    if (aSize > 0 ) {
      ObjectPtr aObject = aRootDoc->object(ModelAPI_ResultPart::group(), 0);
      ResultPartPtr aPart = std::dynamic_pointer_cast<ModelAPI_ResultPart>(aObject);
      if (aPart.get())
        aPart->activate();
    }
  }
#endif

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
  PyConsole_Console* aConsole = myMainWindow->pythonConsole();
  connect(aConsole, SIGNAL(scriptLoaded()), SLOT(updateCommandStatus()));
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
    foreach (ModuleBase_Pref aPref, aModif) {
      aSection = aPref.first;
      if (aSection == ModuleBase_Preferences::VIEWER_SECTION) {
        myMainWindow->viewer()->updateFromResources();
      } else if (aSection == ModuleBase_Preferences::MENU_SECTION) {
        myMainWindow->menuObject()->updateFromResources();
      }
    }
    std::vector<int> aColor;
    try {
      aColor = Config_PropManager::color("Visualization", "selection_color");
    }
    catch (...) {
    }
    if (aColor.size() == 3)
      displayer()->setSelectionColor(aColor);

    displayer()->redisplayObjects();
  }
}
#endif

//******************************************************
void XGUI_Workshop::onTrihedronVisibilityChanged(bool theState)
{
  XGUI_Displayer* aDisplayer = displayer();
  if (aDisplayer) {
    aDisplayer->displayTrihedron(theState);
    aDisplayer->updateViewer();
  }
}

//******************************************************
bool XGUI_Workshop::onSave()
{
  if(!myOperationMgr->abortAllOperations(XGUI_OperationMgr::XGUI_InformationMessage))
    return false;
  if (myCurrentFile.isEmpty()) {
    return onSaveAs();
  }
  SessionPtr aMgr = ModelAPI_Session::get();
  if (aMgr->isAutoUpdateBlocked())
    aMgr->blockAutoUpdate(false);

  std::list<std::string> aFiles;
  // issue #2899: create a temporary directory, save and then remove it
#ifdef HAVE_SALOME
  std::string aTmpDir = XGUI_Tools::getTmpDirByEnv("SALOME_TMP_DIR");
#else
  std::string aTmpDir = XGUI_Tools::getTmpDirByEnv("");
#endif
  saveDocument(QString(aTmpDir.c_str()), aFiles);
  bool aResult = XGUI_CompressFiles::compress(myCurrentFile, aFiles);
  XGUI_Tools::removeTemporaryFiles(aTmpDir, aFiles);

  if (aResult) {
    updateCommandStatus();
#ifndef HAVE_SALOME
    myMainWindow->setModifiedState(false);
#endif
  }
  return aResult;
}

//******************************************************
bool XGUI_Workshop::onSaveAs()
{
  if(!myOperationMgr->abortAllOperations(XGUI_OperationMgr::XGUI_InformationMessage))
    return false;
  qreal aRatio = ModuleBase_Tools::currentPixelRatio();
  myCurrentFile = QFileDialog::getSaveFileName(desktop(), tr("Select name to save file..."),
    QString(), MyFilter2,
    Q_NULLPTR, ((aRatio > 1) ? QFileDialog::DontUseNativeDialog : QFileDialog::Options()));
  if (!myCurrentFile.isNull()) {
    if (!myCurrentFile.endsWith(MyExtension)) {
      myCurrentFile += MyExtension;
    }
  }
  else
    return false;
#ifndef HAVE_SALOME
    myMainWindow->setCurrentDir(myCurrentFile, false);
    myMainWindow->setModifiedState(false);
#endif
  return onSave();
}

//******************************************************
void XGUI_Workshop::onUndo(int theTimes)
{
  processUndoRedo(ActionUndo, theTimes);
  myObjectBrowser->updateAllIndexes(1);
}

//******************************************************
void XGUI_Workshop::onRedo(int theTimes)
{
  processUndoRedo(ActionRedo, theTimes);
  myObjectBrowser->updateAllIndexes(1);
}

//******************************************************
void XGUI_Workshop::processUndoRedo(const ModuleBase_ActionType theActionType, int theTimes)
{
  ModuleBase_ModelWidget* anActiveWidget = myOperationMgr->activeWidget();
  if (anActiveWidget) {
    ActionIntParamPtr aParam(new ModuleBase_ActionIntParameter(theTimes));
    if (anActiveWidget->processAction(theActionType, aParam))
      return;
  }
  // the viewer update should be blocked in order to avoid the features blinking. For the created
  // feature a results are created, the flush of the created signal caused the viewer redisplay for
  // each created result. After a redisplay signal is flushed. So, the viewer update is blocked
  // until redo of all possible objects happens
  bool isUpdateEnabled = myDisplayer->enableUpdateViewer(false);

  SessionPtr aMgr = ModelAPI_Session::get();
  if (aMgr->isOperation()) {
    XGUI_OperationMgr* aOpMgr = operationMgr();
    /// this is important for nested operations
    /// when sketch operation is active, this condition is false and
    /// the sketch operation is not aborted
    if (aOpMgr->canStopOperation(aOpMgr->currentOperation()))
      aOpMgr->abortOperation(aOpMgr->currentOperation());
    else
    {
      myDisplayer->enableUpdateViewer(isUpdateEnabled);
      return;
    }
  }
  objectBrowser()->treeView()->setCurrentIndex(QModelIndex());
  std::list<std::string> anActionList = theActionType == ActionUndo ? aMgr->undoList()
    : aMgr->redoList();
  std::list<std::string>::const_iterator aIt = anActionList.cbegin();
  for (int i = 0; (i < theTimes) && (aIt != anActionList.cend()); ++i, ++aIt) {
    if (theActionType == ActionUndo)
      aMgr->undo();
    else
      aMgr->redo();

    if (QString((*aIt).c_str()) == MOVE_TO_END_COMMAND ||
        QString((*aIt).c_str()) == MOVE_TO_END_SPLIT_COMMAND)
      myObjectBrowser->rebuildDataTree();
  }
  operationMgr()->updateApplyOfOperations();
  facesPanel()->reset(true);
  updateCommandStatus();

  // unblock the viewer update functionality and make update on purpose
  myDisplayer->enableUpdateViewer(isUpdateEnabled);
  myDisplayer->updateViewer();
  // Clear messages in status bar from previous operations if exists
  setStatusBarMessage("");
}

//******************************************************
void XGUI_Workshop::onWidgetStateChanged(int thePreviousState)
{
  ModuleBase_ModelWidget* anActiveWidget = myOperationMgr->activeWidget();
  //ModuleBase_Operation* anOperation = myOperationMgr->currentOperation();
  //if (anOperation) {
  //  ModuleBase_IPropertyPanel* aPanel = anOperation->propertyPanel();
  //  if (aPanel)
  //    anActiveWidget = aPanel->activeWidget();
  //}
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

//******************************************************
void XGUI_Workshop::onWidgetObjectUpdated()
{
  operationMgr()->onValidateOperation();
  myDisplayer->updateViewer();
}

//******************************************************
void XGUI_Workshop::onImportPart()
{
  if (abortAllOperations()) {
    ModuleBase_OperationFeature* anImportPartOp = dynamic_cast<ModuleBase_OperationFeature*>(
        module()->createOperation(ExchangePlugin_ImportPart::ID()));
    anImportPartOp->setHelpFileName(QString("ExchangePlugin") + QDir::separator() +
      "importFeature.html");
    myPropertyPanel->updateApplyPlusButton(anImportPartOp->feature());
    operationMgr()->startOperation(anImportPartOp);
  }
}

//******************************************************
void XGUI_Workshop::onImportShape()
{
  if (abortAllOperations()) {
    ModuleBase_OperationFeature* anImportOp = dynamic_cast<ModuleBase_OperationFeature*>(
        module()->createOperation(ExchangePlugin_Import::ID()));
    anImportOp->setHelpFileName(QString("ExchangePlugin") + QDir::separator() +
      "importFeature.html");
    myPropertyPanel->updateApplyPlusButton(anImportOp->feature());
    operationMgr()->startOperation(anImportOp);
  }
}

//******************************************************
void XGUI_Workshop::onExportShape()
{
  if (abortAllOperations()) {
    ModuleBase_OperationFeature* anExportOp = dynamic_cast<ModuleBase_OperationFeature*>(
        module()->createOperation(ExchangePlugin_ExportFeature::ID()));
    anExportOp->setHelpFileName(QString("ExchangePlugin") + QDir::separator() +
      "exportFeature.html");
    myPropertyPanel->updateApplyPlusButton(anExportOp->feature());
    operationMgr()->startOperation(anExportOp);
  }
}

//******************************************************
void XGUI_Workshop::onExportPart()
{
  if (abortAllOperations()) {
    ModuleBase_OperationFeature* anExportPartOp = dynamic_cast<ModuleBase_OperationFeature*>(
        module()->createOperation(ExchangePlugin_ExportPart::ID()));
    anExportPartOp->setHelpFileName(QString("ExchangePlugin") + QDir::separator() +
      "exportFeature.html");
    myPropertyPanel->updateApplyPlusButton(anExportPartOp->feature());
    operationMgr()->startOperation(anExportPartOp);
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
  HINSTANCE modLib = ::LoadLibraryA(qPrintable(libName));
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
      Events_InfoMessage("XGUI_Workshop", err.toStdString()).send();
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
#ifdef HAVE_SALOME
  salomeConnector()->createFeatureActions();
#endif
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
      if (aId == "UNDO_CMD") {
        bool isActionEnabled = false;
        // if ultimate is true -> using result of operation only, or using OR combination
        ModuleBase_ModelWidget* anActiveWidget = myOperationMgr->activeWidget();
        if (anActiveWidget && anActiveWidget->canProcessAction(ActionUndo, isActionEnabled))
          aCmd->setEnabled(isActionEnabled);
        else
          aCmd->setEnabled(myModule->canUndo());
      }
      else if (aId == "REDO_CMD") {
        bool isActionEnabled = false;
        // if ultimate is true -> using result of operation only, or using OR combination
        ModuleBase_ModelWidget* anActiveWidget = myOperationMgr->activeWidget();
        if (anActiveWidget && anActiveWidget->canProcessAction(ActionRedo, isActionEnabled))
          aCmd->setEnabled(isActionEnabled);
        else
          aCmd->setEnabled(myModule->canRedo());
      }
      else if (aId == "AUTOCOMPUTE_CMD") {
        aCmd->setIcon(aMgr->isAutoUpdateBlocked() ?
          QIcon(":pictures/autoapply_stop.png") :
          QIcon(":pictures/autoapply_start.png"));
      }
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

//******************************************************
void XGUI_Workshop::updateHistory()
{
  bool isActionEnabled = false;
  ModuleBase_ModelWidget* anActiveWidget = myOperationMgr->activeWidget();
  QList<ActionInfo> aUndoRes;
  QList<ActionInfo> aRedoRes;
  if (anActiveWidget && anActiveWidget->canProcessAction(ActionUndo, isActionEnabled)) {
    aUndoRes = anActiveWidget->actionsList(ActionUndo);
    aRedoRes = anActiveWidget->actionsList(ActionRedo);
  } else {
    std::list<std::string> aUndoList = ModelAPI_Session::get()->undoList();
    aUndoRes = processHistoryList(aUndoList);

    std::list<std::string> aRedoList = ModelAPI_Session::get()->redoList();
    aRedoRes = processHistoryList(aRedoList);
  }
  emit updateUndoHistory(aUndoRes);
  emit updateRedoHistory(aRedoRes);
}

//******************************************************
QDockWidget* XGUI_Workshop::createObjectBrowser(QWidget* theParent)
{
  QDockWidget* aObjDock = new QDockWidget(theParent);
  aObjDock->setAllowedAreas(Qt::LeftDockWidgetArea |
                            Qt::RightDockWidgetArea);
  aObjDock->setWindowTitle(tr("Object browser"));
  aObjDock->setStyleSheet(
      "::title { position: relative; padding-left: 5px; text-align: left center }");
  myObjectBrowser = new XGUI_ObjectsBrowser(aObjDock, this);
  myObjectBrowser->initialize(myModule->rootNode());
  myModule->customizeObjectBrowser(myObjectBrowser);
  aObjDock->setWidget(myObjectBrowser);
  aObjDock->setObjectName("Object browser");

  connect(myObjectBrowser, SIGNAL(sizeChanged()), SLOT(onDockSizeChanged()));

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
  myActiveControlMgr->addSelector(new XGUI_PropertyPanelSelector(myPropertyPanel));

  myPropertyPanel->setupActions(myActionsMgr);
  myPropertyPanel->setAllowedAreas(Qt::LeftDockWidgetArea |
                                   Qt::RightDockWidgetArea);
  aDesktop->addDockWidget(Qt::LeftDockWidgetArea, myPropertyPanel);

  hidePanel(myPropertyPanel);  ///<! Invisible by default

  myFacesPanel = new XGUI_FacesPanel(aDesktop, this);
  myActiveControlMgr->addSelector(new XGUI_FacesPanelSelector(myFacesPanel));
  myFacesPanel->setAllowedAreas(Qt::LeftDockWidgetArea |
                                Qt::RightDockWidgetArea |
                                Qt::BottomDockWidgetArea);
  connect(myFacesPanel, SIGNAL(closed()), myFacesPanel, SLOT(onClosed()));

  myInspectionPanel = new XGUI_InspectionPanel(aDesktop, this);
  myInspectionPanel->setAllowedAreas(Qt::LeftDockWidgetArea |
    Qt::RightDockWidgetArea);
  aDesktop->addDockWidget(Qt::RightDockWidgetArea, myInspectionPanel);

  myInspectionPanel->hide();

  aDesktop->addDockWidget(
#ifdef HAVE_SALOME
    Qt::RightDockWidgetArea,
#else
    Qt::LeftDockWidgetArea,
#endif
    myFacesPanel);
  hidePanel(myFacesPanel);  ///<! Invisible by default

#ifdef _DEBUG
  bool aShowOnTheRight = Config_PropManager::boolean("Plugins", "show_hide_faces");
  if (aShowOnTheRight) {
    aDesktop->addDockWidget(Qt::RightDockWidgetArea, myFacesPanel);
    showPanel(myFacesPanel);
  }
#endif
  hideObjectBrowser();

#ifndef HAVE_SALOME
#ifdef _DEBUG
  if (!aShowOnTheRight)
  {
#endif // _DEBUG
  aDesktop->tabifyDockWidget(myFacesPanel, aObjDock);
#ifdef _DEBUG
  }
#endif // _DEBUG

#endif // HAVE_SALOME

  aDesktop->tabifyDockWidget(aObjDock, myPropertyPanel);
  myPropertyPanel->installEventFilter(myOperationMgr);

  QAction* aOkAct = myActionsMgr->operationStateAction(XGUI_ActionsMgr::Accept);
  connect(aOkAct, SIGNAL(triggered()), this, SLOT(onAcceptActionClicked()));

  QAction* aOkContAct = myActionsMgr->operationStateAction(XGUI_ActionsMgr::AcceptPlus);
  connect(aOkContAct, SIGNAL(triggered()), this, SLOT(onAcceptPlusActionClicked()));

  QAction* aCancelAct = myActionsMgr->operationStateAction(XGUI_ActionsMgr::Abort);
  connect(aCancelAct, SIGNAL(triggered()), myOperationMgr, SLOT(onAbortOperation()));

  QAction* aPreviewAct = myActionsMgr->operationStateAction(XGUI_ActionsMgr::Preview);
  connect(aPreviewAct, SIGNAL(triggered()), this, SLOT(onPreviewActionClicked()));

  QAction* aHelpAct = myActionsMgr->operationStateAction(XGUI_ActionsMgr::Help);
  connect(aHelpAct, SIGNAL(triggered()), this, SLOT(onHelpActionClicked()));

  connect(myPropertyPanel, SIGNAL(keyReleased(QObject*, QKeyEvent*)),
          myOperationMgr,  SLOT(onKeyReleased(QObject*, QKeyEvent*)));
  connect(myPropertyPanel, SIGNAL(enterClicked(QObject*)),
          myOperationMgr,  SLOT(onProcessEnter(QObject*)));
}

//******************************************************
void XGUI_Workshop::showPanel(QDockWidget* theDockWidget)
{
  if (theDockWidget == myPropertyPanel) {
    QAction* aViewAct = myPropertyPanel->toggleViewAction();
    ///<! Restore ability to close panel from the window's menu
    aViewAct->setEnabled(true);
  }
  theDockWidget->show();
  theDockWidget->raise();

  // The next code is necessary to made the property panel the active window
  // in order to operation manager could process key events of the panel.
  // otherwise they are ignored. It happens only if the same(activateWindow) is
  // not happened by property panel activation(e.g. resume operation of Sketch)
  //ModuleBase_Tools::setFocus(theDockWidget, "XGUI_Workshop::showPanel()");
}

//******************************************************
void XGUI_Workshop::hidePanel(QDockWidget* theDockWidget)
{
  if (theDockWidget && theDockWidget == myPropertyPanel) {
    QAction* aViewAct = theDockWidget->toggleViewAction();
    ///<! Do not allow to show empty property panel
    aViewAct->setEnabled(false);
  }
  theDockWidget->hide();

  // the property panel is active window of the desktop, when it is
  // hidden, it is undefined which window becomes active. By this reason
  // it is defined to perform the desktop as the active window.
  // in SALOME mode, work-stack made the PyConsole the active window,
  // set the focus on it. As a result, shortcuts of the application, like
  // are processed by this console. For example Undo actions.
  // It is possible that this code is to be moved to SHAPER package
  QMainWindow* aDesktop = desktop();
  ModuleBase_Tools::setFocus(aDesktop, "XGUI_Workshop::hidePanel()");
}

//******************************************************
void XGUI_Workshop::showObjectBrowser()
{
  if (!isSalomeMode()) {
    myObjectBrowser->parentWidget()->show();
  }
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
  else if (theId == "MOVE_CMD" || theId == "MOVE_SPLIT_CMD")
    moveObjects(theId == "MOVE_SPLIT_CMD");
  else if (theId == "COLOR_CMD")
    changeColor(aObjects);
  else if (theId == "ISOLINES_CMD")
    changeIsoLines(aObjects);
  else if (theId == "SHOW_ISOLINES_CMD") {
    foreach(ObjectPtr aObj, aObjects) {
      ResultPtr aResult = std::dynamic_pointer_cast<ModelAPI_Result>(aObj);
      if (aResult.get())
        ModelAPI_Tools::showIsoLines(aResult, !ModelAPI_Tools::isShownIsoLines(aResult));
    }
    mySelector->clearSelection();
    Events_Loop::loop()->flush(Events_Loop::eventByName(EVENT_OBJECT_TO_REDISPLAY));
  }
  else if (theId == "DEFLECTION_CMD")
    changeDeflection(aObjects);
  else if (theId == "TRANSPARENCY_CMD")
    changeTransparency(aObjects);
  else if (theId == "SHOW_CMD") {
    showObjects(aObjects, true);
    mySelector->updateSelectionBy(ModuleBase_ISelection::Browser);
    updateCommandStatus();
  }
  else if (theId == "HIDE_CMD") {
    showObjects(aObjects, false);
    updateCommandStatus();
  }
  else if (theId == "SHOW_ONLY_CMD") {
    showOnlyObjects(aObjects);
    mySelector->updateSelectionBy(ModuleBase_ISelection::Browser);
    updateCommandStatus();
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
#ifdef HAVE_SALOME
    //issue #2159 Hide all incomplete behavior
    viewer()->eraseAll();
#endif
    updateCommandStatus();
    // Necessary for update icons in ObjectBrowser on Linux
    myObjectBrowser->updateAllIndexes();
  } else if (theId == "SELECT_VERTEX_CMD") {
    setViewerSelectionMode(TopAbs_VERTEX);
  } else if (theId == "SELECT_EDGE_CMD") {
    setViewerSelectionMode(TopAbs_EDGE);
  } else if (theId == "SELECT_FACE_CMD") {
    setViewerSelectionMode(TopAbs_FACE);
  } else if (theId == "INSERT_FOLDER_CMD") {
    insertFeatureFolder();
  } else if (theId == "ADD_TO_FOLDER_BEFORE_CMD") {
    insertToFolder(true);
  } else if (theId == "ADD_TO_FOLDER_AFTER_CMD") {
    insertToFolder(false);
  } else if (theId == "ADD_OUT_FOLDER_BEFORE_CMD") {
    moveOutFolder(true);
  } else if (theId == "ADD_OUT_FOLDER_AFTER_CMD") {
    moveOutFolder(false);
  } else if (theId == "SELECT_RESULT_CMD") {
    //setViewerSelectionMode(-1);
    //IMP: an attempt to use result selection with other selection modes
    setViewerSelectionMode(ModuleBase_ResultPrs::Sel_Result);
    setViewerSelectionMode(TopAbs_COMPSOLID);
  } else if (theId == "SHOW_RESULTS_CMD") {
    highlightResults(aObjects);
  } else if (theId == "SHOW_FEATURE_CMD") {
    highlightFeature(aObjects);
  } else if (theId == "SET_VIEW_NORMAL_CMD") {
    setNormalView();
  } else if (theId == "SET_VIEW_INVERTEDNORMAL_CMD") {
    setNormalView(true);
  }
#ifdef TINSPECTOR
  else if (theId == "TINSPECTOR_VIEW") {
    Handle(CDF_Application) anApplication = CDF_Session::CurrentSession()->CurrentApplication();
    if (!anApplication.IsNull())
    {
      if (!MyTCommunicator)
      {
        MyTCommunicator = new TInspector_Communicator();

        NCollection_List<Handle(Standard_Transient)> aParameters;
        aParameters.Append(anApplication);
        Handle(AIS_InteractiveContext) aContext = viewer()->AISContext();
        if (!aContext.IsNull())
          aParameters.Append(aContext);

#ifdef DEBUG_WITH_MESSAGE_REPORT
        Handle(Message_Report) aContextReport = aContext->GetReport();
        aContext->SetReportActive (Standard_True);
        aContextReport->SetLimit (1000);
        if (!aContextReport.IsNull())
          aParameters.Append(aContextReport);
#endif
        MyVCallBack = new VInspector_CallBack();
        myDisplayer->setCallBack(MyVCallBack);
        #ifndef HAVE_SALOME
        AppElements_Viewer* aViewer = mainWindow()->viewer();
        if (aViewer)
          aViewer->setCallBack(MyVCallBack);
        #endif
        aParameters.Append(MyVCallBack);

        MyTCommunicator->RegisterPlugin("TKDFBrowser");
        MyTCommunicator->RegisterPlugin("TKShapeView");
        MyTCommunicator->RegisterPlugin("TKVInspector");
#ifdef DEBUG_WITH_MESSAGE_REPORT
        MyTCommunicator->RegisterPlugin("TKMessageView");
#endif
        //MyTCommunicator->RegisterPlugin("SMBrowser"); // custom plugin to view ModelAPI
        //MyTCommunicator->RegisterPlugin("TKSMBrowser"); // custom plugin to view ModelAPI

        MyTCommunicator->Init(aParameters);
        MyTCommunicator->Activate("TKSMBrowser"); // to have button in TInspector
#ifndef DEBUG_WITH_MESSAGE_REPORT
        MyTCommunicator->Activate("TKVInspector"); // to have filled callback by model
#endif
        MyTCommunicator->Activate("TKDFBrowser");

#ifdef DEBUG_WITH_MESSAGE_REPORT
        MyTCommunicator->Activate("TKMessageView"); // temporary
        MyTCommunicator->Activate("TKVInspector"); // to have filled callback by model
#endif
      }
      MyTCommunicator->SetVisible(true);
    }
  }
#endif
}

//**************************************************************
void XGUI_Workshop::setViewerSelectionMode(int theMode)
{
  if (theMode == -1)
    myViewerSelMode.clear();
  else {
    if (myViewerSelMode.contains(theMode))
      myViewerSelMode.removeAll(theMode);
    else
      myViewerSelMode.append(theMode);
  }
  selectionActivate()->updateSelectionModes();
}

//**************************************************************
void XGUI_Workshop::activateObjectsSelection(const QObjectPtrList& theList)
{
  QIntList aModes;
  module()->activeSelectionModes(aModes);
  if (aModes.isEmpty() && (myViewerSelMode.length() > 0))
    aModes.append(myViewerSelMode);
  selectionActivate()->activateObjects(aModes, theList);
}

//**************************************************************
bool XGUI_Workshop::prepareForDisplay(const std::set<ObjectPtr>& theObjects) const
{
  if (facesPanel()->isEmpty())
    return true;

  // generate container of objects taking into account sub elments of compsolid
  std::set<ObjectPtr> anAllProcessedObjects;
  for (std::set<ObjectPtr>::const_iterator anObjectsIt = theObjects.begin();
    anObjectsIt != theObjects.end(); anObjectsIt++) {
    ObjectPtr anObject = *anObjectsIt;
    ResultBodyPtr aCompRes = std::dynamic_pointer_cast<ModelAPI_ResultBody>(anObject);
    if (aCompRes.get()) {
      std::list<ResultPtr> allRes;
      ModelAPI_Tools::allSubs(aCompRes, allRes);
      if (allRes.empty()) {
        anAllProcessedObjects.insert(anObject);
      } else {
        for(std::list<ResultPtr>::iterator aRes = allRes.begin(); aRes != allRes.end(); aRes++) {
          ResultBodyPtr aBody = std::dynamic_pointer_cast<ModelAPI_ResultBody>(*aRes);
          if (aBody.get() && aBody->numberOfSubs() == 0)
            anAllProcessedObjects.insert(aBody);
        }
      }
    } else
      anAllProcessedObjects.insert(anObject);
  }

  // find hidden objects in faces panel
  std::set<ObjectPtr> aHiddenObjects;
  QStringList aHiddenObjectNames;
  for (std::set<ObjectPtr>::const_iterator anObjectsIt = anAllProcessedObjects.begin();
       anObjectsIt != anAllProcessedObjects.end(); anObjectsIt++) {
    if (!facesPanel()->isObjectHiddenByPanel(*anObjectsIt))
      continue;
    aHiddenObjects.insert(*anObjectsIt);
    aHiddenObjectNames.append((*anObjectsIt)->data()->name().c_str());
  }
  if (aHiddenObjects.empty()) // in parameter objects there are no hidden objects in hide face
    return true;

  int anAnswer = QMessageBox::question(
        desktop(), tr("Show object"),
        tr("'%1'\n are hidden by %2:\nRemove objects from the panel to be displayed?")
        .arg(aHiddenObjectNames.join(", ")).arg(facesPanel()->windowTitle()),
        QMessageBox::Yes | QMessageBox::No, QMessageBox::No);

  bool aToBeDisplayed = anAnswer == QMessageBox::Yes;
  if (aToBeDisplayed)
    facesPanel()->restoreObjects(aHiddenObjects);

  return aToBeDisplayed;
}

//**************************************************************
void XGUI_Workshop::deleteObjects()
{
  ModuleBase_IModule* aModule = module();
  // allow the module to delete objects, do nothing if it has succeed
  if (aModule->deleteObjects()) {
    updateCommandStatus();
    myDisplayer->updateViewer();
    return;
  }

  QObjectPtrList anObjects = mySelector->selection()->selectedObjects();
  if (anObjects.isEmpty())
    return;

  if (!abortAllOperations())
    return;

  bool hasResult = false;
  bool hasFeature = false;
  bool hasParameter = false;
  bool hasCompositeOwner = false;
  bool hasResultInHistory = false;
  bool hasFolder = false;
  ModuleBase_Tools::checkObjects(anObjects, hasResult, hasFeature, hasParameter, hasCompositeOwner,
                                 hasResultInHistory, hasFolder);
  if (!(hasResult || hasFeature || hasParameter || hasFolder))
    return;

  // Remove from the list non-deletable objects: infinite constructions which are not in history
  bool notDelete = true;
  QObjectPtrList::iterator aIt;
  for (aIt = anObjects.begin(); aIt != anObjects.end(); aIt++) {
    ObjectPtr aObj = (*aIt);
    ResultConstructionPtr aConstr = std::dynamic_pointer_cast<ModelAPI_ResultConstruction>(aObj);
    FeaturePtr aFeature = ModelAPI_Feature::feature(aObj);
    if (aFeature) {
      notDelete = (!aFeature->isInHistory()) && aConstr->isInfinite();
      if (notDelete) {
        anObjects.removeAll(aObj);
        aIt--;
      }
    }
  }
  // delete objects
  std::map<FeaturePtr, std::set<FeaturePtr> > aReferences;
  std::set<FeaturePtr> aFeatures;
  ModuleBase_Tools::convertToFeatures(anObjects, aFeatures);
  ModelAPI_Tools::findAllReferences(aFeatures, aReferences);

  std::set<FolderPtr> aFolders;
  ModuleBase_Tools::convertToFolders(anObjects, aFolders);

  bool aDone = false;
  QString aDescription = contextMenuMgr()->action("DELETE_CMD")->text() + " %1";
  aDescription = aDescription.arg(XGUI_Tools::unionOfObjectNames(anObjects, ", "));
  ModuleBase_Operation* anOpAction = new ModuleBase_Operation(aDescription, module());

  operationMgr()->startOperation(anOpAction);

  std::set<FeaturePtr> aFeatureRefsToDelete;
  if (ModuleBase_Tools::askToDelete(aFeatures, aReferences, desktop(), aFeatureRefsToDelete)) {
    // WORKAROUND, should be done before each object remove, if it presents in XGUI_DataModel tree
    // It is necessary to clear selection in order to avoid selection changed event during
    // deletion and negative consequences connected with processing of already deleted items
    mySelector->clearSelection();

    if (!aFeatureRefsToDelete.empty())
      aFeatures.insert(aFeatureRefsToDelete.begin(), aFeatureRefsToDelete.end());
    aDone = ModelAPI_Tools::removeFeatures(aFeatures, false);
  }
  if (aFolders.size() > 0) {
    std::set<FolderPtr>::const_iterator anIt = aFolders.begin(),
                                         aLast = aFolders.end();
    for (; anIt != aLast; anIt++) {
      FolderPtr aFolder = *anIt;
      if (aFolder.get()) {
        DocumentPtr aDoc = aFolder->document();
        aDoc->removeFolder(aFolder);
      }
    }
  }
  // remove results selected
  std::list<ResultPtr> aResults;
  for(QObjectPtrList::const_iterator anIt = anObjects.begin(); anIt != anObjects.end(); anIt++) {
    ResultPtr aRes = std::dynamic_pointer_cast<ModelAPI_Result>(*anIt);
    if (aRes.get() && aRes->data()->isValid() && !aRes->data()->isDeleted() &&
        aRes->groupName() != ModelAPI_ResultGroup::group() && // don't remove groups and fields
        aRes->groupName() != ModelAPI_ResultField::group()) // because they are badly selected
      aResults.push_back(aRes);
  }
  if (!aResults.empty()) {
    ModelAPI_Tools::removeResults(aResults);
  }

  if (aDone)
    operationMgr()->commitOperation();
  else
    operationMgr()->abortOperation(operationMgr()->currentOperation());

  myDisplayer->updateViewer();
}

//**************************************************************
void addRefsToFeature(const FeaturePtr& theFeature,
                      const std::map<FeaturePtr, std::set<FeaturePtr> >& theMainList,
                      std::set<FeaturePtr>& theReferences)
{
  //if (theReferences.find(theFeature) != theReferences.end())
  //  return;
  if (theMainList.find(theFeature) == theMainList.end())
    return; // this feature is not in the selection list, so exists without references to it
  std::set<FeaturePtr> aMainReferences = theMainList.at(theFeature);

  std::set<FeaturePtr>::const_iterator anIt = aMainReferences.begin(),
                                       aLast = aMainReferences.end();
  for (; anIt != aLast; anIt++) {
    FeaturePtr aRefFeature = *anIt;
    if (theReferences.find(aRefFeature) == theReferences.end())
      theReferences.insert(aRefFeature);
    addRefsToFeature(aRefFeature, theMainList, theReferences);
  }
}

//**************************************************************
void XGUI_Workshop::cleanHistory()
{
  if (!abortAllOperations())
    return;

  QObjectPtrList anObjects = mySelector->selection()->selectedObjects();
  std::set<FeaturePtr> aFeatures;
  ModuleBase_Tools::convertToFeatures(anObjects, aFeatures);

#ifdef DEBUG_CLEAN_HISTORY
  QStringList anInfo;
  std::set<FeaturePtr>::const_iterator aFIt;
  for (aFIt = aFeatures.begin(); aFIt != aFeatures.end(); ++aFIt) {
    FeaturePtr aFeature = ModelAPI_Feature::feature(*aFIt);
    anInfo.append(aFeature->name().c_str());
  }
  QString anInfoStr = anInfo.join(";\t");
  qDebug(QString("cleanHistory for: [%1] - %2").
    arg(aFeatures.size()).arg(anInfoStr).toStdString().c_str());
#endif

  std::map<FeaturePtr, std::set<FeaturePtr> > aReferences;
  ModelAPI_Tools::findAllReferences(aFeatures, aReferences, true, false);
  // find for each object whether all reference values are in the map as key, that means that there
  // is no other reference in the model to this object, so it might be removed by cleaning history
  // sk_1(ext_1, vertex_1) + (sk_3, bool_1) - can't be deleted, dependency to bool_1
  // ext_1(bool_1, sk_3)  - can't be deleted, dependency to bool_1
  // vertex_1()
  // sk_2(ext_2) + (bool_1)  - can't be deleted, dependency to bool_1
  // ext_2(bool_1)  - can't be deleted, dependency to bool_1
  // sk_3()
  // Information: bool_1 is not selected
  std::set<FeaturePtr> anUnusedObjects;
  std::map<FeaturePtr, std::set<FeaturePtr> >::const_iterator aMainIt = aReferences.begin(),
                                                              aMainLast = aReferences.end();
  for (; aMainIt != aMainLast; aMainIt++) {
    FeaturePtr aMainListFeature = aMainIt->first;
    std::set<FeaturePtr> aMainRefList = aMainIt->second;
    std::set<FeaturePtr>::const_iterator aRefIt = aMainRefList.begin(),
                                                  aRefLast = aMainRefList.end();
    bool aFeatureOutOfTheList = false;
    for (; aRefIt != aRefLast && !aFeatureOutOfTheList; aRefIt++) {
      FeaturePtr aRefFeature = *aRefIt;
      aFeatureOutOfTheList = aReferences.find(aRefFeature) == aReferences.end();
    }
    if (!aFeatureOutOfTheList)
      anUnusedObjects.insert(aMainListFeature);
  }

#ifdef DEBUG_CLEAN_HISTORY
  anInfo.clear();
  for (aFIt = anUnusedObjects.begin(); aFIt != anUnusedObjects.end(); ++aFIt) {
    FeaturePtr aFeature = *aFIt;
    anInfo.append(aFeature->name().c_str());
  }
  qDebug(QString("unused objects: [%1] - %2").
    arg(anInfo.size()).arg(anInfo.join(";\t")).toStdString().c_str());
#endif

  // warn about the references remove, break the delete operation if the user chose it
  if (!anUnusedObjects.empty()) {
    QStringList aNames;
    foreach (const FeaturePtr& aFeature, anUnusedObjects) {
      aNames.append(aFeature->name().c_str());
    }
    aNames.sort();
    QString anUnusedNames = aNames.join(", ");

    QString anActionId = "CLEAN_HISTORY_CMD";
    QString aDescription = ModuleBase_Tools::translate("workshop",
        contextMenuMgr()->action(anActionId)->text().toStdString());

    QMessageBox aMessageBox(desktop());
    aMessageBox.setWindowTitle(aDescription);
    aMessageBox.setIcon(QMessageBox::Warning);
    aMessageBox.setStandardButtons(QMessageBox::No | QMessageBox::Yes);
    aMessageBox.setDefaultButton(QMessageBox::No);

    const char* aKeyStr = "Unused features are the following: "
                          "%1.\nThese features will be deleted.\nWould you like to continue?";
    QString aText = QString(tr(aKeyStr).arg(anUnusedNames));
    aMessageBox.setText(aText);
    if (aMessageBox.exec() == QMessageBox::No)
      return;

    // 1. start operation
    aDescription += "by deleting of " +
      aDescription.arg(XGUI_Tools::unionOfObjectNames(anObjects, ", "));
    ModuleBase_Operation* anOpAction = new ModuleBase_Operation(aDescription, module());
    operationMgr()->startOperation(anOpAction);

    // WORKAROUND, should be done before each object remove, if it presents in XGUI_DataModel tree
    // It is necessary to clear selection in order to avoid selection changed event during
    // deletion and negative consequences connected with processing of already deleted items
    mySelector->clearSelection();

    std::set<FeaturePtr> anIgnoredFeatures;
    if (ModelAPI_Tools::removeFeatures(anUnusedObjects, true)) {
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
bool compareFeature(const FeaturePtr& theF1, const FeaturePtr& theF2) {
  DocumentPtr aDoc = theF1->document();
  return aDoc->index(theF1) < aDoc->index(theF2);
}
void XGUI_Workshop::moveObjects(const bool theSplit)
{
  if (!abortAllOperations())
    return;

  SessionPtr aMgr = ModelAPI_Session::get();

  QObjectPtrList anObjects = mySelector->selection()->selectedObjects();
  // It is necessary to clear selection in order to avoid selection changed event during
  // moving and negative consequences connected with processing of already moved items
  mySelector->clearSelection();
  // check whether the object can be moved. There should not be parts which are not loaded
  std::set<FeaturePtr> aFeatures;
  ModuleBase_Tools::convertToFeatures(anObjects, aFeatures);
  if (!XGUI_Tools::canRemoveOrRename(desktop(), aFeatures))
    return;

  QString anActionId = theSplit ? "MOVE_CMD" : "MOVE_SPLIT_CMD";
  QString aDescription = contextMenuMgr()->action(anActionId)->text();
  aMgr->startOperation(aDescription.toStdString());

  // Sort features by index in document
  std::list<FeaturePtr> aFList(aFeatures.begin(), aFeatures.end());
  aFList.sort(compareFeature);

  DocumentPtr anActiveDocument = aMgr->activeDocument();
  FeaturePtr aCurrentFeature = anActiveDocument->currentFeature(true);
  std::list<FeaturePtr>::const_iterator anIt = aFList.begin(), aLast = aFList.end();
  for (; anIt != aLast; anIt++) {
    FeaturePtr aFeature = *anIt;
    if (!aFeature.get() || !myModule->canApplyAction(aFeature, anActionId))
      continue;

    anActiveDocument->moveFeature(aFeature, aCurrentFeature, theSplit);
    aCurrentFeature = anActiveDocument->currentFeature(true);
  }
  aMgr->finishOperation();
  updateCommandStatus();
  myViewerProxy->update();
}

//**************************************************************
bool XGUI_Workshop::deleteFeatures(const QObjectPtrList& theObjects)
{
  std::set<FeaturePtr> aFeatures;
  ModuleBase_Tools::convertToFeatures(theObjects, aFeatures);

  return ModelAPI_Tools::removeFeaturesAndReferences(aFeatures);
}

//******************************************************
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
    std::list<ResultPtr> aResults;
    ModelAPI_Tools::allResults(aFeature, aResults);
    foreach (const ResultPtr& aResult, aResults) {
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
  std::list<FeaturePtr>::iterator aObjectIt =
    std::find(anAllFeatures.begin(), anAllFeatures.end(), theObject);
  if (aObjectIt == anAllFeatures.end())
    return aResult;
  // find the current feature iterator
  std::list<FeaturePtr>::iterator aCurrentIt =
    std::find(anAllFeatures.begin(), anAllFeatures.end(), aDocument->currentFeature(true));
  if (aCurrentIt == anAllFeatures.end())
    return aResult;
  // check the right order
  if (std::distance(aObjectIt, anAllFeatures.end()) <=
      std::distance(aCurrentIt, anAllFeatures.end()))
    return aResult;
  // exclude the object
  std::advance(aObjectIt, 1);
  // include the current feature
  std::advance(aCurrentIt, 1);
  return std::list<FeaturePtr>(aObjectIt, aCurrentIt);
}

//******************************************************
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
    if (!aObject.get() || !aObject->data().get() || !aObject->data()->isValid()) {
      aCanMove = false;
      break;
    }
    FeaturePtr aFeat = std::dynamic_pointer_cast<ModelAPI_Feature>(aObject);
    // only groups can be moved to the end for now (#2451)
    if (aFeat.get() && aFeat->getKind() != "Group") {
      aCanMove = false;
      break;
    }

    // 1. Get features placed between selected and current in the document
    std::list<FeaturePtr> aFeaturesBetween = toCurrentFeatures(aObject);
    // if aFeaturesBetween is empty it means wrong order or aObject is the current feature
    if (aFeaturesBetween.empty())
      aCanMove = false;
    else {
      std::set<FeaturePtr> aPlacedFeatures(aFeaturesBetween.begin(), aFeaturesBetween.end());
      // 2. Get all reference features to the selected object in the document
      std::set<FeaturePtr> aRefFeatures;
      ModuleBase_Tools::refsToFeatureInFeatureDocument(aObject, aRefFeatures);

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
    ResultBodyPtr aCompRes = std::dynamic_pointer_cast<ModelAPI_ResultBody>(theObject);
    if (aCompRes.get() != NULL) { // change colors for all sub-solids
      std::list<ResultPtr> allRes;
      ModelAPI_Tools::allSubs(aCompRes, allRes);
      std::list<ResultPtr>::iterator aRes = allRes.begin();
      for(; aRes != allRes.end() && !aCanBeShaded; aRes++) {
        aCanBeShaded = myDisplayer->canBeShaded(*aRes);
      }
    }
  }
  return aCanBeShaded;
}

//**************************************************************
bool XGUI_Workshop::canChangeProperty(const QString& theActionName) const
{
  if (theActionName == "COLOR_CMD" ||
      theActionName == "DEFLECTION_CMD" ||
      theActionName == "TRANSPARENCY_CMD") {
    QObjectPtrList aObjects = mySelector->selection()->selectedObjects();

    std::set<std::string> aTypes;
    aTypes.insert(ModelAPI_ResultGroup::group());
    aTypes.insert(ModelAPI_ResultConstruction::group());
    aTypes.insert(ModelAPI_ResultBody::group());
    aTypes.insert(ModelAPI_ResultPart::group());

    return hasResults(aObjects, aTypes);
  }
  return false;
}


//**************************************************************
void getDefaultColor(ObjectPtr theObject, const bool isEmptyColorValid,
  std::vector<int>& theColor)
{
  theColor.clear();
  // get default color from the preferences manager for the given result
  if (theColor.empty()) {
    std::string aSection, aName, aDefault;
    theObject->colorConfigInfo(aSection, aName, aDefault);
    if (!aSection.empty() && !aName.empty()) {
      theColor = Config_PropManager::color(aSection, aName);
    }
  }
  if (!isEmptyColorValid && theColor.empty()) {
    // all AIS objects, where the color is not set, are in black.
    // The color should be defined in XML or set in the attribute
    theColor = Config_PropManager::color("Visualization", "object_default_color");
    Events_InfoMessage("XGUI_CustomPrs",
      "A default color is not defined in the preferences for this kind of result").send();
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
      ModelAPI_Tools::getColor(aResult, aColor);
      if (aColor.empty())
        getDefaultColor(aResult, false, aColor);
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
      ResultBodyPtr aBodyResult = std::dynamic_pointer_cast<ModelAPI_ResultBody>(aResult);
      if (aBodyResult.get() != NULL) { // change colors for all sub-solids
        std::list<ResultPtr> allRes;
        ModelAPI_Tools::allSubs(aBodyResult, allRes);
        for(std::list<ResultPtr>::iterator aRes = allRes.begin(); aRes != allRes.end(); aRes++) {
          ModelAPI_Tools::setColor(*aRes, !isRandomColor ? aColorResult : aDlg->getRandomColor());
        }
      }
      ModelAPI_Tools::setColor(aResult, !isRandomColor ? aColorResult : aDlg->getRandomColor());
    }
  }
  Events_Loop::loop()->flush(Events_Loop::eventByName(EVENT_OBJECT_TO_REDISPLAY));
  aMgr->finishOperation();
  updateCommandStatus();
  myViewerProxy->update();
}

//**************************************************************
void setTransparency(double theTransparency, const QObjectPtrList& theObjects)
{
  foreach(ObjectPtr anObj, theObjects) {
    ResultPtr aResult = std::dynamic_pointer_cast<ModelAPI_Result>(anObj);
    if (aResult.get() != NULL) {
      ResultBodyPtr aBodyResult = std::dynamic_pointer_cast<ModelAPI_ResultBody>(aResult);
      if (aBodyResult.get() != NULL) { // change property for all sub-solids
        std::list<ResultPtr> allRes;
        ModelAPI_Tools::allSubs(aBodyResult, allRes);
        std::list<ResultPtr>::iterator aRes;
        for(aRes = allRes.begin(); aRes != allRes.end(); aRes++) {
          ModelAPI_Tools::setTransparency(*aRes, theTransparency);
        }
      }
      ModelAPI_Tools::setTransparency(aResult, theTransparency);
    }
  }
}

//**************************************************************
double getDefaultDeflection(const ObjectPtr& theObject)
{
  double aDeflection = -1;
  ResultPtr aResult = std::dynamic_pointer_cast<ModelAPI_Result>(theObject);
  if (aResult.get()) {
    bool isConstruction = false;

    std::string aResultGroup = aResult->groupName();
    if (aResultGroup == ModelAPI_ResultConstruction::group())
      isConstruction = true;
    else if (aResultGroup == ModelAPI_ResultBody::group()) {
      GeomShapePtr aGeomShape = aResult->shape();
      if (aGeomShape.get()) {
        // if the shape could not be exploded on faces, it contains only wires, edges, and vertices
        // correction of deviation for them should not influence to the application performance
        GeomAPI_ShapeExplorer anExp(aGeomShape, GeomAPI_Shape::FACE);
        isConstruction = !anExp.more();
      }
    }
    if (isConstruction)
      aDeflection = Config_PropManager::real("Visualization", "construction_deflection");
    else
      aDeflection = Config_PropManager::real("Visualization", "body_deflection");
  }
  return aDeflection;
}

//**************************************************************
void XGUI_Workshop::changeDeflection(const QObjectPtrList& theObjects)
{
  AttributeDoublePtr aDoubleAttr;
  // 1. find the current property of the object. This is a property of AIS presentation
  // The objects are iterated until a first valid property is found
  double aDeflection = -1;
  foreach(ObjectPtr anObject, theObjects) {
    ResultPtr aResult = std::dynamic_pointer_cast<ModelAPI_Result>(anObject);
    if (aResult.get()) {
      aDeflection = ModelAPI_Tools::getDeflection(aResult);
      if (aDeflection < 0)
        aDeflection = getDefaultDeflection(aResult);
    }
    else {
      // TODO: remove the obtaining a property from the AIS object
      // this does not happen never because:
      // 1. The property can be changed only on results
      // 2. The result can be not visualized in the viewer(e.g. Origin Construction)
      AISObjectPtr anAISObj = myDisplayer->getAISObject(anObject);
      if (anAISObj.get()) {
        aDeflection = anAISObj->getDeflection();
      }
    }
    if (aDeflection > 0)
      break;
  }
  if (aDeflection < 0)
    return;

  if (!abortAllOperations())
  return;
  // 2. show the dialog to change the value
  XGUI_DeflectionDialog* aDlg = new XGUI_DeflectionDialog(desktop());
  aDlg->setDeflection(aDeflection);
  aDlg->move(QCursor::pos());
  bool isDone = aDlg->exec() == QDialog::Accepted;
  if (!isDone)
    return;

  // 3. abort the previous operation and start a new one
  SessionPtr aMgr = ModelAPI_Session::get();
  QString aDescription = contextMenuMgr()->action("DEFLECTION_CMD")->text();
  aMgr->startOperation(aDescription.toStdString());

  // 4. set the value to all results
  aDeflection = aDlg->getDeflection();
  foreach(ObjectPtr anObj, theObjects) {
    ResultPtr aResult = std::dynamic_pointer_cast<ModelAPI_Result>(anObj);
    if (aResult.get() != NULL) {
      ResultBodyPtr aBodyResult = std::dynamic_pointer_cast<ModelAPI_ResultBody>(aResult);
      if (aBodyResult.get() != NULL) { // change property for all sub-solids
        std::list<ResultPtr> allRes;
        ModelAPI_Tools::allSubs(aBodyResult, allRes);
        for(std::list<ResultPtr>::iterator aRes = allRes.begin(); aRes != allRes.end(); aRes++) {
          ModelAPI_Tools::setDeflection(*aRes, aDeflection);
        }
      }
      ModelAPI_Tools::setDeflection(aResult, aDeflection);
    }
  }
  Events_Loop::loop()->flush(Events_Loop::eventByName(EVENT_OBJECT_TO_REDISPLAY));
  aMgr->finishOperation();
  updateCommandStatus();
}

//**************************************************************
double getDefaultTransparency(const ResultPtr& theResult)
{
  return Config_PropManager::integer("Visualization", "shaper_default_transparency") / 100.;
}

//**************************************************************
void XGUI_Workshop::changeTransparency(const QObjectPtrList& theObjects)
{
  AttributeDoublePtr aDoubleAttr;
  // 1. find the current property of the object. This is a property of AIS presentation
  // The objects are iterated until a first valid property is found
  double aCurrentValue = -1;
  foreach(ObjectPtr anObject, theObjects) {
    ResultPtr aResult = std::dynamic_pointer_cast<ModelAPI_Result>(anObject);
    if (aResult.get()) {
      aCurrentValue = ModelAPI_Tools::getTransparency(aResult);
      if (aCurrentValue < 0)
        aCurrentValue = getDefaultTransparency(aResult);
    }
    if (aCurrentValue > 0)
      break;
  }
  if (aCurrentValue < 0)
    return;

  if (!abortAllOperations())
    return;

  // 2. show the dialog to change the value
  XGUI_PropertyDialog* aDlg = new XGUI_PropertyDialog(desktop());
  aDlg->setWindowTitle(tr("Transparency"));
  XGUI_TransparencyWidget* aTransparencyWidget = new XGUI_TransparencyWidget(aDlg);
  connect(aTransparencyWidget, SIGNAL(transparencyValueChanged()),
          this, SLOT(onTransparencyValueChanged()));
  aDlg->setContent(aTransparencyWidget);
  aTransparencyWidget->setValue(aCurrentValue);

  // 3. abort the previous operation and start a new one
  SessionPtr aMgr = ModelAPI_Session::get();
  QString aDescription = contextMenuMgr()->action("TRANSPARENCY_CMD")->text();
  aMgr->startOperation(aDescription.toStdString());

  if (aDlg->exec() == QDialog::Accepted) {
    // 4. set the value to all results
    aCurrentValue = aTransparencyWidget->getValue();
    setTransparency(aCurrentValue, theObjects);
    aMgr->finishOperation();
  } else {
    aMgr->abortOperation();
    Events_Loop::loop()->flush(Events_Loop::eventByName(EVENT_OBJECT_TO_REDISPLAY));
  }

  updateCommandStatus();
}

//**************************************************************
void XGUI_Workshop::onTransparencyValueChanged()
{
  XGUI_TransparencyWidget* aTransparencyWidget = (XGUI_TransparencyWidget*)sender();
  if (!aTransparencyWidget)
    return;

  QObjectPtrList anObjects = mySelector->selection()->selectedObjects();
  setTransparency(aTransparencyWidget->getValue(), anObjects);
  static const Events_ID kRedisplayEvent =
    Events_Loop::loop()->eventByName(EVENT_OBJECT_TO_REDISPLAY);
  Events_Loop::loop()->flush(kRedisplayEvent);

  myViewerProxy->update();
}


//******************************************************
void XGUI_Workshop::showObjects(const QObjectPtrList& theList, bool isVisible)
{
  if (isVisible) {
    std::set<ObjectPtr> anObjects;
    foreach (ObjectPtr aObj, theList) {
      anObjects.insert(aObj);
    }
    if (!prepareForDisplay(anObjects))
      return;
  }

  foreach (ObjectPtr aObj, theList) {
    aObj->setDisplayed(isVisible);
  }
  Events_Loop::loop()->flush(Events_Loop::eventByName(EVENT_OBJECT_TO_REDISPLAY));
  myObjectBrowser->updateAllIndexes();

  updateColorScaleVisibility();
  displayer()->updateViewer();
}

//**************************************************************
void XGUI_Workshop::showOnlyObjects(const QObjectPtrList& theList)
{
  QObjectPtrList aSrcList = theList;
  // Hide all displayed objects
  QObjectPtrList aList = myDisplayer->displayedObjects();
  foreach (ObjectPtr aObj, aList) {
    if ((!aSrcList.contains(aObj)) && (module()->canEraseObject(aObj)))
      aObj->setDisplayed(false);
    else
      aSrcList.removeAll(aObj);
  }
  //Do not use eraseAll if you didn't send Redisplay event:
  //all objects are erased from viewer, but considered as displayed in displayer
  // Problem in bug 2218
  Events_Loop::loop()->flush(Events_Loop::eventByName(EVENT_OBJECT_TO_REDISPLAY));
#ifdef HAVE_SALOME
    //issue #2159 Hide all incomplete behavior
    viewer()->eraseAll();
#endif

  std::set<ObjectPtr> anObjects;
  foreach (ObjectPtr aObj, aSrcList) {
    anObjects.insert(aObj);
  }

  if (!prepareForDisplay(anObjects))
    return;

  // Show only objects from the list
  foreach (ObjectPtr aObj, aSrcList) {
    aObj->setDisplayed(true);
  }
  Events_Loop::loop()->flush(Events_Loop::eventByName(EVENT_OBJECT_TO_REDISPLAY));

  // Necessary for update icons in ObjectBrowser on Linux
  myObjectBrowser->updateAllIndexes();
  updateColorScaleVisibility();
  displayer()->updateViewer();
}


//**************************************************************
void XGUI_Workshop::updateColorScaleVisibility()
{
  QObjectPtrList aObjects = mySelector->selection()->selectedObjects();
  viewer()->setColorScaleShown(false);
  if (aObjects.size() == 1) {
    FieldStepPtr aStep =
      std::dynamic_pointer_cast<ModelAPI_ResultField::ModelAPI_FieldStep>(aObjects.first());
    if (aStep.get() && myDisplayer->isVisible(aStep)) {
      AISObjectPtr aAisPtr = myDisplayer->getAISObject(aStep);
      Handle(AIS_InteractiveObject) aIO = aAisPtr->impl<Handle(AIS_InteractiveObject)>();
      ModuleBase_IStepPrs* aPrs = dynamic_cast<ModuleBase_IStepPrs*>(aIO.get());
      if (aPrs) {
        ModelAPI_AttributeTables::ValueType aType = aPrs->dataType();
        if ((aType == ModelAPI_AttributeTables::DOUBLE) ||
          (aType == ModelAPI_AttributeTables::INTEGER) ||
          (aType == ModelAPI_AttributeTables::BOOLEAN)) {
          myViewerProxy->setupColorScale();
          if (aType == ModelAPI_AttributeTables::BOOLEAN) {
            myViewerProxy->setColorScaleIntervals(2);
            myViewerProxy->setColorScaleRange(0., 1.);
          }
          else {
            double aMin, aMax;
            aPrs->dataRange(aMin, aMax);
            myViewerProxy->setColorScaleRange(aMin, aMax);
          }
          myViewerProxy->setColorScaleTitle(aStep->name().c_str());
          myViewerProxy->setColorScaleShown(true);
        }
      }
    }
  }
}


//**************************************************************
void XGUI_Workshop::setNormalView(bool toInvert)
{
  QList<ModuleBase_ViewerPrsPtr> aPrsList =
    mySelector->selection()->getSelected(ModuleBase_ISelection::Viewer);
  GeomShapePtr aPlanarFace;
  foreach(ModuleBase_ViewerPrsPtr aPrs, aPrsList) {
    GeomShapePtr aShape = aPrs->shape();
    if (aShape.get() && aShape->isPlanar()) {
      aPlanarFace = aShape;
      break;
    }
  }
  if (aPlanarFace.get()) {
    GeomFacePtr aFace(new GeomAPI_Face(aPlanarFace));
    GeomPlanePtr aPlane = aFace->getPlane();
    GeomDirPtr aNormal = aPlane->direction();
    if (toInvert)
      aNormal->reverse();
    GeomPointPtr aPos = aPlane->location();

    double aXmin, aYmin, aZmin, aXmax, aYmax, aZmax;
    aFace->computeSize(aXmin, aYmin, aZmin, aXmax, aYmax, aZmax);

    Handle(V3d_View) aView = myViewerProxy->activeView();
    double aScale = aView->Scale();
    aView->SetAt(aPos->x(), aPos->y(), aPos->z());
    aView->SetProj(aNormal->x(), aNormal->y(), aNormal->z());
    Bnd_Box aBox;
    aBox.Update(aXmin, aYmin, aZmin, aXmax, aYmax, aZmax);
    aView->FitAll(aBox);
  }
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
  foreach(ObjectPtr anObj, theList) {
    myDisplayer->setDisplayMode(anObj, (XGUI_Displayer::DisplayMode)theMode, false);

    ResultBodyPtr aBodyResult = std::dynamic_pointer_cast<ModelAPI_ResultBody>(anObj);
    if (aBodyResult.get() != NULL) { // change display mode for all sub-solids
      std::list<ResultPtr> allRes;
      ModelAPI_Tools::allSubs(aBodyResult, allRes);
      for(std::list<ResultPtr>::iterator aRes = allRes.begin(); aRes != allRes.end(); aRes++) {
        myDisplayer->setDisplayMode(*aRes, (XGUI_Displayer::DisplayMode)theMode, false);
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
  //myDisplayer->closeLocalContexts();
  myDisplayer->eraseAll();
  objectBrowser()->clearContent();

  module()->closeDocument();
  // we need to clear viewer (with created filters) to do not have problems in 2nd SALOME study
  module()->clearViewer();


  // data model need not process the document's signals about objects modifications as
  // the document is closed
  //bool isBlocked = objectBrowser()->dataModel()->blockEventsProcessing(true);

  SessionPtr aMgr = ModelAPI_Session::get();
  aMgr->closeAll();

  //objectBrowser()->dataModel()->blockEventsProcessing(isBlocked);
}

//******************************************************
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

//******************************************************
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

//******************************************************
void XGUI_Workshop::setStatusBarMessage(const QString& theMessage)
{
#ifdef HAVE_SALOME
  salomeConnector()->putInfo(theMessage, -1);
#else
  myMainWindow->putInfo(theMessage, -1);
#endif
}

#ifdef HAVE_SALOME
//******************************************************
void XGUI_Workshop::synchronizeViewer()
{
  SessionPtr aMgr = ModelAPI_Session::get();
  QList<DocumentPtr> aDocs;
  aDocs.append(aMgr->activeDocument());
  aDocs.append(aMgr->moduleDocument());

  foreach(DocumentPtr aDoc, aDocs) {
    synchronizeGroupInViewer(aDoc, false);
  }
}

//******************************************************
void XGUI_Workshop::synchronizeGroupInViewer(const DocumentPtr& theDoc,
                                             bool theUpdateViewer)
{
  FeaturePtr aFeature;
  ResultPtr aRes;
  int aSize = theDoc->numInternalFeatures();
  for (int i = 0; i < aSize; i++) {
    aFeature = theDoc->internalFeature(i);
    if (!aFeature.get())
      continue;
    const std::list<ResultPtr>& aResults = aFeature->results();
    std::list<ResultPtr>::const_iterator aIt;
    aFeature->setDisplayed(false);
    for (aIt = aResults.cbegin(); aIt != aResults.cend(); aIt++) {
      aRes = (*aIt);
      if (aRes->isDisplayed() && !aRes->isConcealed()) {
        // Hide the presentation with an empty shape. But isDisplayed state of the object should not
        // be changed to the object becomes visible when the shape becomes not empty
        if (!aRes->shape().get() || aRes->shape()->isNull())
          continue;
        ResultBodyPtr aResBody = std::dynamic_pointer_cast<ModelAPI_ResultBody>(aRes);
        if (aResBody.get())
          synchronizeResultTree(aResBody, false);
        else {
          if (aRes->isInHistory()) {
            if (aRes->isDisplayed())
              myDisplayer->display(aRes, false);
            else
              myDisplayer->erase(aRes, false);
          }
          else
            aRes->setDisplayed(false);
        }
      }
    }
  }
  if (theUpdateViewer)
    myDisplayer->updateViewer();
}

void XGUI_Workshop::synchronizeResultTree(const ResultBodyPtr& theRes, bool theUpdateViewer)
{
  if (theRes->numberOfSubs() > 0)
    for (int i = 0; i < theRes->numberOfSubs(); i++) {
      ResultBodyPtr aRes = theRes->subResult(i);
      if (aRes.get())
        synchronizeResultTree(aRes, theUpdateViewer);
    }
  else {
    if (theRes->isDisplayed())
      myDisplayer->display(theRes, theUpdateViewer);
    else
      myDisplayer->erase(theRes, theUpdateViewer);
  }
}
#endif

//******************************************************
void XGUI_Workshop::highlightResults(const QObjectPtrList& theObjects)
{
  FeaturePtr aFeature;
  QObjectPtrList aSelList = theObjects;
  QObjectPtrList aNewSel;
  bool aHasHidden = false;
  foreach(ObjectPtr aObj, theObjects) {
    aFeature = std::dynamic_pointer_cast<ModelAPI_Feature>(aObj);
    if (aFeature.get()) {
      std::list<ResultPtr> aResults;
      ModelAPI_Tools::allResults(aFeature, aResults);
      std::list<std::shared_ptr<ModelAPI_Result> >::const_iterator aIt;
      for(aIt = aResults.cbegin(); aIt != aResults.cend(); aIt++) {
        aHasHidden |= (*aIt)->isConcealed();
        aSelList.append(*aIt);
        aNewSel.append(*aIt);
      }
    }
  }
  if (aSelList.count() > theObjects.count()) {
    // if something was found
    objectBrowser()->setObjectsSelected(aSelList);
    objectBrowser()->ensureVisible(aNewSel.first());
  }
  if (aHasHidden)
    QMessageBox::information(desktop(), tr("Find results"),
                             tr("Results not found"), QMessageBox::Ok);
}

//******************************************************
void XGUI_Workshop::highlightFeature(const QObjectPtrList& theObjects)
{
  ResultPtr aResult;
  QObjectPtrList aSelList = theObjects;
  QObjectPtrList aNewSel;
  FeaturePtr aFeature;
  foreach(ObjectPtr aObj, theObjects) {
    aResult = std::dynamic_pointer_cast<ModelAPI_Result>(aObj);
    if (aResult.get()) {
      aFeature = ModelAPI_Feature::feature(aResult);
      if (aFeature.get()) {
        aSelList.append(aFeature);
        aNewSel.append(aFeature);
      }
    }
  }
  if (aSelList.count() > theObjects.count()) {
    // if something was found
    objectBrowser()->setObjectsSelected(aSelList);
    objectBrowser()->ensureVisible(aNewSel.first());
  }
}

void XGUI_Workshop::insertFeatureFolder()
{
  QObjectPtrList aObjects = mySelector->selection()->selectedObjects();
  if (aObjects.isEmpty())
    return;
  ObjectPtr aObj = aObjects.first();
  FeaturePtr aFeature = std::dynamic_pointer_cast<ModelAPI_Feature>(aObj);
  if (aFeature.get() == NULL)
    return;
  SessionPtr aMgr = ModelAPI_Session::get();
  DocumentPtr aDoc = aMgr->activeDocument();

  QString aDescription = contextMenuMgr()->action("INSERT_FOLDER_CMD")->text();

  aMgr->startOperation(aDescription.toStdString());
  aDoc->addFolder(aFeature);
  aMgr->finishOperation();

  updateCommandStatus();
}


void XGUI_Workshop::insertToFolder(bool isBefore)
{
  std::list<FeaturePtr> aFeatures = mySelector->getSelectedFeatures();
  if (aFeatures.empty())
    return;

  SessionPtr aMgr = ModelAPI_Session::get();
  DocumentPtr aDoc = aMgr->activeDocument();

  FolderPtr aFolder = isBefore? aDoc->findFolderAbove(aFeatures):
                                aDoc->findFolderBelow(aFeatures);
  if (!aFolder.get())
    return;

  QString aDescription = contextMenuMgr()->action(
    isBefore ? "ADD_TO_FOLDER_BEFORE_CMD" : "ADD_TO_FOLDER_AFTER_CMD")->text();

  QMap<ObjectPtr, bool> aStates = myObjectBrowser->getFoldersState(aDoc);

  aMgr->startOperation(aDescription.toStdString());
  aDoc->moveToFolder(aFeatures, aFolder);
  aMgr->finishOperation();

  myObjectBrowser->setFoldersState(aStates);

  updateCommandStatus();
}

void XGUI_Workshop::moveOutFolder(bool isBefore)
{
  std::list<FeaturePtr> aFeatures = mySelector->getSelectedFeatures();
  if (aFeatures.empty())
    return;

  SessionPtr aMgr = ModelAPI_Session::get();
  DocumentPtr aDoc = aMgr->activeDocument();

  QString aDescription = contextMenuMgr()->action(
    isBefore ? "ADD_OUT_FOLDER_BEFORE_CMD" : "ADD_OUT_FOLDER_AFTER_CMD")->text();

  QMap<ObjectPtr, bool> aStates = myObjectBrowser->getFoldersState(aDoc);

  aMgr->startOperation(aDescription.toStdString());
  aDoc->removeFromFolder(aFeatures, isBefore);
  aMgr->finishOperation();

  myObjectBrowser->setFoldersState(aStates);

  updateCommandStatus();
}

void XGUI_Workshop::onAutoApply()
{
  SessionPtr aMgr = ModelAPI_Session::get();
  bool isBlocked = aMgr->isAutoUpdateBlocked();
  aMgr->blockAutoUpdate(!isBlocked);
  myDisplayer->updateViewer();
}

void XGUI_Workshop::updateAutoComputeState()
{
  SessionPtr aMgr = ModelAPI_Session::get();
  bool isComputeBlocked = aMgr->isAutoUpdateBlocked();
#ifdef HAVE_SALOME
//  QAction* aUpdateCmd;
//  QList<QAction*> aCommands = mySalomeConnector->commandList();
//  foreach(QAction* aCmd, aCommands) {
//    if (aCmd->data().toString() == "AUTOCOMPUTE_CMD") {
//      aUpdateCmd = aCmd;
//      break;
//    }
//  }
//  aUpdateCmd->setIcon(isComputeBlocked? QIcon(":pictures/autoapply_stop.png") :
//    QIcon(":pictures/autoapply_start.png"));
#else
  AppElements_MainMenu* aMenuBar = myMainWindow->menuObject();
  AppElements_Command* aUpdateCmd = aMenuBar->feature("AUTOCOMPUTE_CMD");
  aUpdateCmd->button()->setIcon(isComputeBlocked? QIcon(":pictures/autoapply_stop.png") :
    QIcon(":pictures/autoapply_start.png"));
#endif
}


void XGUI_Workshop::clearTemporaryDir()
{
  QDir aDir(myTmpDir.path());
  if (!aDir.isEmpty()) {
    QStringList aEntries;
    aDir.entryList(aEntries);
    foreach(QString aFile, aEntries) {
      aDir.remove(aFile);
    }
  }
}


void XGUI_Workshop::onDockSizeChanged()
{
  QDockWidget* aDockWgt = dynamic_cast<QDockWidget*>(myObjectBrowser->parentWidget());
  int aObWidth = aDockWgt->size().width();
  if (myPropertyPanel->width() != aObWidth) {
    QList<QDockWidget*> aWgtList;
    aWgtList << myPropertyPanel << aDockWgt;
    QList<int> aSizeList;
    aSizeList << aObWidth << aObWidth;
    desktop()->resizeDocks(aWgtList, aSizeList, Qt::Horizontal);
    disconnect(myObjectBrowser, SIGNAL(sizeChanged()), this, SLOT(onDockSizeChanged()));
  }
}

void XGUI_Workshop::deactivateCurrentSelector()
{
  myActiveControlMgr->deactivateSelector(myActiveControlMgr->activeSelector());
}

void XGUI_Workshop::changeIsoLines(const QObjectPtrList& theObjects)
{
  if (theObjects.isEmpty())
    return;

  QList<ResultPtr> aResultList;
  ResultPtr aRes;
  foreach(ObjectPtr aObj, theObjects) {
    aRes = std::dynamic_pointer_cast<ModelAPI_Result>(aObj);
    if (aRes.get())
      aResultList.append(aRes);
  }

  std::vector<int> aValues;
  bool isVisible;
  if (aResultList.size() == 1) {
    ResultPtr aRes = aResultList.first();
    if (aRes.get())
      ModelAPI_Tools::getIsoLines(aRes, isVisible, aValues);
    else
      return;
  }
  if (aValues.size() == 0) {
    aValues.push_back(1);
    aValues.push_back(1);
  }

  if (!abortAllOperations())
    return;

  XGUI_PropertyDialog aDlg(desktop());
  aDlg.setWindowTitle(tr("Number of Iso-lines"));

  QWidget* aIsosWgt = new QWidget(&aDlg);
  QFormLayout* aLayout = new QFormLayout(aIsosWgt);
  ModuleBase_Tools::adjustMargins(aLayout);
  aDlg.setContent(aIsosWgt);

  QSpinBox* aUNb = new QSpinBox(&aDlg);
  aUNb->setValue(aValues[0]);
  aLayout->addRow("U:", aUNb);

  QSpinBox* aVNb = new QSpinBox(&aDlg);
  aVNb->setValue(aValues[1]);
  aLayout->addRow("V:", aVNb);

  if (aDlg.exec() == QDialog::Accepted) {
    SessionPtr aMgr = ModelAPI_Session::get();
    QString aDescription = contextMenuMgr()->action("ISOLINES_CMD")->text();
    aMgr->startOperation(aDescription.toStdString());

    aValues[0] = aUNb->value();
    aValues[1] = aVNb->value();
    foreach(ResultPtr aRes, aResultList) {
      ModelAPI_Tools::setIsoLines(aRes, aValues);
    }
    mySelector->clearSelection();
    Events_Loop::loop()->flush(Events_Loop::eventByName(EVENT_OBJECT_TO_REDISPLAY));
    aMgr->finishOperation();
    updateCommandStatus();
  }
}