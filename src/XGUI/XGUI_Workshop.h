// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

#ifndef XGUI_WORKSHOP_H
#define XGUI_WORKSHOP_H

#include "XGUI.h"
//#include "XGUI_Constants.h"
#include <ModuleBase_Definitions.h>
#include <ModelAPI_Document.h>
#include <ModelAPI_Feature.h>

#include <ModuleBase_ActionInfo.h>

#include <QObject>
#include <QMap>
#include <QKeySequence>
#include <QIcon>

#ifndef HAVE_SALOME
class AppElements_MainWindow;
class AppElements_Command;
class AppElements_Workbench;
#endif

class XGUI_ActionsMgr;
class XGUI_ContextMenuMgr;
class XGUI_Displayer;
class XGUI_ErrorDialog;
class XGUI_ErrorMgr;
class XGUI_MenuMgr;
class XGUI_ModuleConnector;
class XGUI_ObjectsBrowser;
class XGUI_OperationMgr;
class XGUI_PropertyPanel;
class XGUI_SalomeConnector;
class XGUI_SalomeViewer;
class XGUI_SelectionMgr;
class XGUI_ViewerProxy;
class XGUI_WorkshopListener;

class ModuleBase_IModule;
class ModuleBase_IViewer;
class ModuleBase_Operation;

class QWidget;
class QDockWidget;
class QMainWindow;

class QAction;


/**\class XGUI_Workshop
 * \ingroup GUI
 * \brief Class which defines a configuration of the application (Workshop) and launches it.
 */
class XGUI_EXPORT XGUI_Workshop : public QObject
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

  /// Activates the module controls. Should be called after module creation
  void activateModule();

  /// Deactivates the module controls. Should be called after module creation
  void deactivateModule();

#ifndef HAVE_SALOME
  //! Returns main window (Desktop) of the application
  AppElements_MainWindow* mainWindow() const
  {
    return myMainWindow;
  }

  //! Creates and adds a new workbench (menu group) with the given name and returns it
  AppElements_Workbench* addWorkbench(const QString& theName);
#endif

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

  //! ! Returns error manager.
  XGUI_ErrorMgr* errorMgr() const
  {
    return myErrorMgr;
  }

  //! ! Returns an actions manager
  XGUI_ActionsMgr* actionsMgr() const
  {
    return myActionsMgr;
  }

  //! ! Returns an actions manager
  XGUI_MenuMgr* menuMgr() const
  {
    return myMenuMgr;
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

  /// Returns the module connector
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

  //! Searches for selected features unused in other (not selected) features. If one or several
  //! selected features are found, a warning message proposes to delete them. It contains 
  //! the list of features to be deleted.
  void cleanHistory();

  //! Returns true if the selected feature can be moved to the position after the current feature
  //! \return boolean value
  bool canMoveFeature();

  //! Move selected features to be after the current feature
  void moveObjects();

  //! Returns true if the object can be shaded. If the object is a compsolid result, the method
  //! checks subobjects of the result
  //! \return boolean value
  bool canBeShaded(const ObjectPtr& theObject) const;

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

  /// Set display mode for the given objects
  /// \param theList a list of displayed objects
  /// \param theMode a mode to set (see \ref XGUI_Displayer)
  void setDisplayMode(const QObjectPtrList& theList, int theMode);

  /// Set selection mode in viewer. If theMode=-1 then activate default mode
  /// \param theMode the selection mode (according to TopAbs_ShapeEnum)
  void setViewerSelectionMode(int theMode);

  /// Activates current selection mode for the given list of objects
  void activateObjectsSelection(const QObjectPtrList& theList);

  /// Returns current module
  ModuleBase_IModule* module() const
  {
    return myModule;
  }

  /// Returns current directory which contains data files
  QString currentDataDir() const { return myCurrentDir; }

  /// Returns current directory which contains data files
  void setCurrentDataDir(const QString& theDir) { myCurrentDir = theDir; }

  /**
  * Save the current document into a directory
  * \param theName - path to the directory
  * \param theFileNames - returned file names created in this directory
  */
  void saveDocument(const QString& theName, std::list<std::string>& theFileNames);

  /**
   * If there is an active (uncommitted) operation shows a prompt to abort it
   * and performs abortion if user agreed. Returns true if
   * - operation aborted successfully
   * - there is no active operation
   */
  bool abortAllOperations();

  //! Delete features. Delete the referenced features. There can be a question with a list of
  //! referenced objects.
  //! \param theList an objects to be deleted
  //! \param theIgnoredFeatures a list of features to be ignored during delete
  bool deleteFeatures(const QObjectPtrList& theFeatures,
                      const std::set<FeaturePtr>& theIgnoredFeatures = std::set<FeaturePtr>());

  /// Deactivates the object, if it is active and the module returns that the activation
  /// of selection for the object is not possible currently(the current operation uses it)
  /// \param theObject an object
  /// \param theUpdateViewer a boolean flag to update viewer immediately
  void deactivateActiveObject(const ObjectPtr& theObject, const bool theUpdateViewer);

  /// Returns true if the action of the feature is created to contain Accept/Cancel button
  /// \param theFeature a feature
  bool isFeatureOfNested(const FeaturePtr& theFeature);

  /// Has to be called in order to display objects with visibility status = true
  void synchronizeViewer();

  /// Has to be called in order to display objects from a cpecifed group with visibility status = true
  /// \param theDoc the document for objects synchronisation
  /// \param theGroup the group name
  /// \param theUpdateViewer update viewer flag
  void synchronizeGroupInViewer(const DocumentPtr& theDoc, const std::string& theGroup, bool theUpdateViewer);

  /// Update the property panel content by the XML description of the operation and set the panel
  /// into the operation
  /// \param theOperation an operation
  void setPropertyPanel(ModuleBase_Operation* theOperation);

  /// Connects or disconnects to the value changed signal of the property panel widgets
  /// \param isToConnect a boolean value whether connect or disconnect
  void connectToPropertyPanel(const bool isToConnect);

  /// Returns defailt selection mode in 3d viewer
  int viewerSelectionMode() const { return myViewerSelMode; }

  /// Highlights result objects in Object Browser according to
  /// features found in the given list
  void highlightResults(const QObjectPtrList& theObjects);

  /// A constant string used for "Move to end" command definition
  /// It is used for specific processing of Undo/Redo for this command.
  static QString MOVE_TO_END_COMMAND;

signals:
  /// Emitted when selection happens in Salome viewer
  void salomeViewerSelection();

  /// Emitted when error in application happens
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

  /// Save current document
  bool onSave();

  /// Save current document to a file selected by user
  bool onSaveAs();

  /// Undo last command
  void onUndo(int times = 1);

  /// Redo previous command
  void onRedo(int times = 1);

  // Rebuild data tree
  //void onRebuild();

  /// Validates the operation to change the "Apply" button state.
  /// \param thePreviousState the previous state of the widget
  void onWidgetStateChanged(int thePreviousState);

  /// Calls activate of the current widget again. It is possible that selection filters are
  /// differs in the new state of paged container
  void onValuesChanged();

  /// Listens the corresponded signal of model widget and updates Apply button state by feature
  void onWidgetObjectUpdated();

  /// Show property panel
  void showPropertyPanel();

  /// Hide property panel
  void hidePropertyPanel();

  /// Show object Browser
  void showObjectBrowser();

  /// Hide object Browser
  void hideObjectBrowser();

  /// Close document
  void closeDocument();

  /// Open document from file
  void onOpen();

  /// Create a new document
  void onNew();

#ifndef HAVE_SALOME
  /// Exit application
  void onExit();

  /// Open preferences dialog box
  void onPreferences();
#endif

  /// Activates/deactivates the trihedron in the viewer AIS context
  void onTrihedronVisibilityChanged(bool theState);


 protected:
  /// Sets the granted operations for the parameter operation. Firstly, it finds the nested features
  /// and set them into the operation. Secondly, it asks the module about ids of granted operations.
  /// \param theOperation an operation
   void setGrantedFeatures(ModuleBase_Operation* theOperation);

  //! Find all referenced features. Return direct and indirect lists of referenced object
  //! \param theList an objects to be checked
  //! \param aDirectRefFeatures a list of direct reference features
  //! \param aIndirectRefFeatures a list of features which depend on the feature through others
  void findReferences(const QObjectPtrList& theList,
                      std::set<FeaturePtr>& aDirectRefFeatures,
                      std::set<FeaturePtr>& aIndirectRefFeatures);

  //! Shows a dialog box about references. Ask whether they should be also removed.
  //! \param theList an objects to be checked
  //! \param aDirectRefFeatures a list of direct reference features
  //! \param aIndirectRefFeatures a list of features which depend on the feature through others
  //! \param theParent a parent widget for the question message box
  //! \param doDeleteReferences if there are parameters between features, ask if they should be
  //! replaced to their meaning without corresponded features remove
  //! \return true if in message box answer is Yes
  bool isDeleteFeatureWithReferences(const QObjectPtrList& theList,
                                     const std::set<FeaturePtr>& aDirectRefFeatures,
                                     const std::set<FeaturePtr>& aIndirectRefFeatures,
                                     QWidget* theParent,
                                     bool& doDeleteReferences);

  //! \param theIgnoredFeatures a list of features to be ignored during delete
  //! \param theList an objects to be checked
  //! \param aDirectRefFeatures a list of direct reference features
  //! \param aIndirectRefFeatures a list of features which depend on the feature through others
  //! \param doDeleteReferences flag if referenced features should be removed also
  bool deleteFeaturesInternal(const QObjectPtrList& theList,
                              const std::set<FeaturePtr>& aDirectRefFeatures,
                              const std::set<FeaturePtr>& aIndirectRefFeatures,
                              const std::set<FeaturePtr>& theIgnoredFeatures,
                              const bool doDeleteReferences = true);

private:
  /// Display all results
  //void displayAllResults();

  /// Display results from document
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
  /// hides the property panel and update the command status.
  /// \param theOpertion a stopped operation
  virtual void onOperationStopped(ModuleBase_Operation* theOperation);

  /// SLOT, that is called after the operation is committed.
  /// \param theOpertion a committed operation
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

  /// Called by Ok button clicked in the property panel. Asks the error manager whether
  /// the operation can be committed and do it if it returns true.
  void onAcceptActionClicked();

  /// Called by Preview button clicked in the property panel. Sends signal to model to
  /// compute preview.
  void onPreviewActionClicked();

 private:
   /// Init menu
  void initMenu();

  /// Register validators
  void registerValidators() const;

  /// Load module from external library
  /// \param theModule name of the module
  ModuleBase_IModule* loadModule(const QString& theModule);

  /// Create module
  bool createModule();

  /// Create object browser widget
  /// \param theParent a parent of widget
  QDockWidget* createObjectBrowser(QWidget* theParent);

  /// Create property panel widget
  /// \param theParent a parent of widget
  QDockWidget* createPropertyPanel(QWidget* theParent);

  // Creates Dock widgets: Object browser and Property panel
  void createDockWidgets();

  //! Extends undo/redo toolbutton's with history menu
  //! \param theObject - in the OpenParts it is a QToolButton by itself,
  //! in salome mode - QAction that creates a button.
  //! \param theSignal - void "updateUndoHistory" or "updateRedoHistory" SIGNAL;
  //! \param theSlot - onUndo(int) or onRedo(int) SLOT
  void addHistoryMenu(QObject* theObject, const char* theSignal, const char* theSlot);


  /// Calls removeFeature of the document for each object in the list
  //! \param theList an objects to be deleted
  //! \param theIgnoredFeatures a list of features to be ignored during delete
  //! \param theActionId an action command key to find context menu object types enabled for remove
  //! \param theFlushRedisplay a boolean value if the redisplay signal should be flushed
  bool removeFeatures(const QObjectPtrList& theList,
                      const std::set<FeaturePtr>& theIgnoredFeatures,
                      const QString& theActionId,
                      const bool theFlushRedisplay);

  //! Creates list of actions (commands) by given history list from session
  QList<ActionInfo> processHistoryList(const std::list<std::string>&) const;

private:
#ifndef HAVE_SALOME
  AppElements_MainWindow* myMainWindow;
#endif

  ModuleBase_IModule* myModule;
  XGUI_ErrorMgr* myErrorMgr;
  XGUI_ObjectsBrowser* myObjectBrowser;
  XGUI_PropertyPanel* myPropertyPanel;
  XGUI_SelectionMgr* mySelector;
  XGUI_Displayer* myDisplayer;
  XGUI_OperationMgr* myOperationMgr;  ///< manager to manipulate through the operations
  XGUI_ActionsMgr* myActionsMgr;
  XGUI_MenuMgr* myMenuMgr; ///< manager to build menu/tool bar using order defined in XML
  XGUI_SalomeConnector* mySalomeConnector;
  XGUI_ErrorDialog* myErrorDlg;
  XGUI_ViewerProxy* myViewerProxy;
  XGUI_ContextMenuMgr* myContextMenuMgr;
  XGUI_ModuleConnector* myModuleConnector;
  XGUI_WorkshopListener* myEventsListener;

  QString myCurrentDir;

  int myViewerSelMode;
};

#endif
