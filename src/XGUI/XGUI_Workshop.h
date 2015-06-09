// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

#ifndef XGUI_WORKSHOP_H
#define XGUI_WORKSHOP_H

#include "XGUI.h"
//#include "XGUI_Constants.h"
#include <Events_Listener.h>
#include <ModuleBase_Definitions.h>
#include <ModelAPI_Document.h>
#include <ModelAPI_Feature.h>

#include <ModuleBase_ActionInfo.h>

#include <QObject>
#include <QMap>
#include <QKeySequence>
#include <QIcon>

class AppElements_MainWindow;
class AppElements_Command;
class AppElements_Workbench;

class XGUI_SelectionMgr;
class XGUI_Displayer;
class XGUI_OperationMgr;
class XGUI_SalomeConnector;
class XGUI_ObjectsBrowser;
class XGUI_ActionsMgr;
class XGUI_ErrorDialog;
class XGUI_SalomeViewer;
class XGUI_ViewerProxy;
class XGUI_PropertyPanel;
class XGUI_ContextMenuMgr;
class XGUI_ModuleConnector;

class ModuleBase_Operation;
class ModuleBase_IModule;
class ModuleBase_IViewer;

class Config_FeatureMessage;
class Config_PointerMessage;

class QWidget;
class QDockWidget;
class QMainWindow;

class ModelAPI_ObjectUpdatedMessage;
class ModelAPI_ObjectDeletedMessage;
class ModelAPI_ResultPart;
class QAction;

/**\class XGUI_Workshop
 * \ingroup GUI
 * \brief Class which defines a configuration of the application (Workshop) and launches it.
 */
class XGUI_EXPORT XGUI_Workshop : public QObject, public Events_Listener
{
Q_OBJECT
 public:
  /// Constructor
  /// \param theConnector a Salome connector object. 
  /// Used only if the workshop is launched in Salome environment
  XGUI_Workshop(XGUI_SalomeConnector* theConnector = 0);
  virtual ~XGUI_Workshop();

  //! Starting of the application
  void startApplication();

  //! Returns main window (Desktop) of the application
  AppElements_MainWindow* mainWindow() const
  {
    return myMainWindow;
  }

  //! Returns selection manager object
  XGUI_SelectionMgr* selector() const
  {
    return mySelector;
  }

  //! Returns displayer
  XGUI_Displayer* displayer() const
  {
    return myDisplayer;
  }

  //! ! Returns operation manager.
  XGUI_OperationMgr* operationMgr() const
  {
    return myOperationMgr;
  }

  //! ! Returns an actions manager
  XGUI_ActionsMgr* actionsMgr() const
  {
    return myActionsMgr;
  }

  //! Returns property panel widget
  XGUI_PropertyPanel* propertyPanel() const
  {
    return myPropertyPanel;
  }

  //! Returns context menu manager object
  XGUI_ContextMenuMgr* contextMenuMgr() const
  {
    return myContextMenuMgr;
  }

  //! Creates and adds a new workbench (menu group) with the given name and returns it
  AppElements_Workbench* addWorkbench(const QString& theName);

  //! Redefinition of Events_Listener method
  virtual void processEvent(const std::shared_ptr<Events_Message>& theMessage);

  //! Returns an object which provides interface to Salome Module (LightApp_Module)
  XGUI_SalomeConnector* salomeConnector() const
  {
    return mySalomeConnector;
  }

  //! Provides an object which provides interface to Salome Viewer
  ModuleBase_IViewer* salomeViewer() const;

  //! Returns true if the application works as SALOME module
  bool isSalomeMode() const
  {
    return mySalomeConnector != 0;
  }

  //! Returns Object browser
  XGUI_ObjectsBrowser* objectBrowser() const
  {
    return myObjectBrowser;
  }

  /// This method is called by Salome module when selection is changed
  void salomeViewerSelectionChanged();

  /// Returns viewer which unifies access as to Salome viewer as to standalone viewer
  XGUI_ViewerProxy* viewer() const
  {
    return myViewerProxy;
  }

  /// Returns the module connectory
  /// \returns the instance of connector
  XGUI_ModuleConnector* moduleConnector() const
  {
    return myModuleConnector;
  }

  /// Returns a desktop
  /// \return a desktop instance
  QMainWindow* desktop() const;

  //! Delete features
  void deleteObjects();

  //! Returns true if there is at least one selected body/construction/group result
  //! \return boolean value
  bool canChangeColor() const;

  //! Change color of the features if it is possible
  //! The operation is available for construction, body and group results
  //! theObjects a list of selected objects
  void changeColor(const QObjectPtrList& theObjects);

  //! Show the given features in 3d Viewer
  void showObjects(const QObjectPtrList& theList, bool isVisible);

  //! Show the given features in 3d Viewer
  void showOnlyObjects(const QObjectPtrList& theList);

  /// Set display mode for the given onjects
  /// \param theList a list of displayed objects
  /// \param theMode a mode to set (see \ref XGUI_Displayer)
  void setDisplayMode(const QObjectPtrList& theList, int theMode);

  /// Returns current module
  ModuleBase_IModule* module() const
  {
    return myModule;
  }

  /// Returns current directory whic contains data files
  QString currentDataDir() const { return myCurrentDir; }

  /// Returns current directory whic contains data files
  void setCurrentDataDir(const QString& theDir) { myCurrentDir = theDir; }

  /**
  * Save the current document into a directory
  * \param theName - path to the directory
  * \param theFileNames - returned file names created in this directory
  */
  void saveDocument(const QString& theName, std::list<std::string>& theFileNames);

  /**
   * If there is an active (uncommited) operation shows a prompt to abort it
   * and performs abortion if user agreed. Returns true if
   * - operation aborted successfully
   * - there is no active operation
   */
  bool isActiveOperationAborted();

  //! Delete features. Delete the referenced features. There can be a question with a list of referenced
  //! objects.
  //! \param theList an objects to be deleted
  //! \param theIgnoredFeatures a list of features to be ignored during delete
  //! \param theParent a parent widget for the question message box
  //! \param theAskAboutDeleteReferences if true, the message box with a list of references to the
  //! objects features appear. If the user chose do not continue, the deletion is not performed
  //! \return the success of the delete 
  static bool deleteFeatures(const QObjectPtrList& theList,
                             std::set<FeaturePtr> theIgnoredFeatures = std::set<FeaturePtr>(),
                             QWidget* theParent = 0,
                             const bool theAskAboutDeleteReferences = false);

signals:
  /// Emitted when selection happens in Salome viewer
  void salomeViewerSelection();

  /// Emitted when error in applivation happens
  void errorOccurred(const QString&);

  //! the signal about the workshop actions states are updated.
  void commandStatusUpdated();

  //! the application is started
  void applicationStarted();

  //! Signal to update Undo history list
  void updateUndoHistory(const QList<ActionInfo>&);

  //! Signal to update Redo history list
  void updateRedoHistory(const QList<ActionInfo>&);

 public slots:
   /// Update of commands status
  void updateCommandStatus();

  /// update history list (undo/redo commands)
  void updateHistory();

  /// Create a new dokument
  void onNew();

  /// Open document from file
  void onOpen();

  /// Save current document
  bool onSave();

  /// Save current document to a file selected by user
  bool onSaveAs();

  /// Exit application
  void onExit();

  /// Undo last command
  void onUndo(int times = 1);

  /// Redo previous command
  void onRedo(int times = 1);

  /// Rebuild data tree
  void onRebuild();

  /// Open preferences dialog box
  void onPreferences();

  /// Show property panel
  void showPropertyPanel();

  /// Hide property panel
  void hidePropertyPanel();

  /// Show object Browser
  void showObjectBrowser();

  /// Hide object Browser
  void hideObjectBrowser();

  /// Reaction on command call
  //void onFeatureTriggered();

  /// Close document
  void closeDocument();

 protected:
  /// Find the nested features and set them into the operation
  /// \param theOperation an operation
  void setNestedFeatures(ModuleBase_Operation* theOperation);

  /// Update the property panel content by the XML description of the operation and set the panel
  /// into the operation
  /// \param theOperation an operation
  void setPropertyPanel(ModuleBase_Operation* theOperation);

  /// Procedure to process postponed events
  bool event(QEvent * theEvent);

  //Event-loop processing methods:

  /// Process event "Add a feature"
  void addFeature(const std::shared_ptr<Config_FeatureMessage>&);

  /// Connect to operation signals
  /// \param theOperation an operation
  void connectWithOperation(ModuleBase_Operation* theOperation);

  /// Process feature update message
  void onFeatureUpdatedMsg(const std::shared_ptr<ModelAPI_ObjectUpdatedMessage>& );

  /// Process feature created message
  void onFeatureCreatedMsg(const std::shared_ptr<ModelAPI_ObjectUpdatedMessage>& );

  /// Process feature redisplay message
  void onFeatureRedisplayMsg(const std::shared_ptr<ModelAPI_ObjectUpdatedMessage>& );

  /// Deactivates the object, if it is active and the module returns that the activation
  /// of selection for the object is not possible currently(the current operation uses it)
  /// \param theObject an object
  /// \param theUpdateViewer a boolean flag to update viewer immediately
  void deactivateActiveObject(const ObjectPtr& theObject, const bool theUpdateViewer);

  /// Display all results
  //void displayAllResults();

  /// Displau results from document
  /// \param theDoc a document
  void displayDocumentResults(DocumentPtr theDoc);

  /// Display results from a group
  void displayGroupResults(DocumentPtr theDoc, std::string theGroup);

 private slots:
  /// SLOT, that is called after the operation is started. Update workshop state according to
  /// the started operation, e.g. visualizes the property panel and connect to it.
  /// \param theOpertion a started operation
  void onOperationStarted(ModuleBase_Operation* theOperation);

  /// SLOT, that is called after the operation is resumed. Update workshop state according to
  /// the started operation, e.g. visualizes the property panel and connect to it.
  /// \param theOpertion a resumed operation
  virtual void onOperationResumed(ModuleBase_Operation* theOperation);

  /// SLOT, that is called after the operation is stopped. Update workshop state, e.g.
  /// hides the property panel and udpate the command status.
  /// \param theOpertion a stopped operation
  virtual void onOperationStopped(ModuleBase_Operation* theOperation);

  /// SLOT, that is called after the operation is committed.
  /// \param theOpertion a commmitted operation
  virtual void onOperationCommitted(ModuleBase_Operation* theOperation);

  /// SLOT, that is called after the operation is aborted.
  /// \param theOpertion an aborted operation
  void onOperationAborted(ModuleBase_Operation* theOperation);

  /// Slot, which reacts to the context popup menu call
  /// \param theId the data value of the clicked action
  /// \param isChecked a state of toggle if the action is checkable
  void onContextMenuCommand(const QString& theId, bool isChecked);

  /// Set waiting cursor
  void onStartWaiting();

 private:
   /// Init menu
  void initMenu();

  /// Register validators
  void registerValidators() const;

  /// Load module from external library
  /// \param theModule name of the module
  ModuleBase_IModule* loadModule(const QString& theModule);

  /// Activate module
  bool activateModule();

  /// Create object browser widget
  /// \param theParent a parent of widget
  QDockWidget* createObjectBrowser(QWidget* theParent);

  /// Create property panel widget
  /// \param theParent a parent of widget
  QDockWidget* createPropertyPanel(QWidget* theParent);

  // Creates Dock widgets: Object browser and Property panel
  void createDockWidgets();

  /// Displaus object and fit all viewer if the object is first (update viewer will not be called)
  /// Asks the module whether the object can be displayed
  /// \param theObj an object
  /// \return true if the object is displayed
  bool displayObject(ObjectPtr theObj);

  //! Extends undo/redo toolbutton's with history menu
  //! \param theObject - in the OpenParts it is a QToolButton by itself,
  //! in salome mode - QAction that creates a button.
  //! \param theSignal - void "updateUndoHistory" or "updateRedoHistory" SIGNAL;
  //! \param theSlot - onUndo(int) or onRedo(int) SLOT
  void addHistoryMenu(QObject* theObject, const char* theSignal, const char* theSlot);

  //! Creates list of actions (commands) by given history list from session
  QList<ActionInfo> processHistoryList(const std::list<std::string>&) const;

private:
  AppElements_MainWindow* myMainWindow;
  ModuleBase_IModule* myModule;
  XGUI_ObjectsBrowser* myObjectBrowser;
  XGUI_PropertyPanel* myPropertyPanel;
  XGUI_SelectionMgr* mySelector;
  XGUI_Displayer* myDisplayer;
  XGUI_OperationMgr* myOperationMgr;  ///< manager to manipulate through the operations
  XGUI_ActionsMgr* myActionsMgr;
  XGUI_SalomeConnector* mySalomeConnector;
  XGUI_ErrorDialog* myErrorDlg;
  XGUI_ViewerProxy* myViewerProxy;
  XGUI_ContextMenuMgr* myContextMenuMgr;
  XGUI_ModuleConnector* myModuleConnector;

  QString myCurrentDir;

  bool myUpdatePrefs;

  // Flag to check that part document is in process of activating
  bool myPartActivating;

  /// The flag is true when we loading a document
  bool myIsLoadingData;
};

#endif
