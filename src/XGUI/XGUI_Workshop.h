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

#ifndef XGUI_WORKSHOP_H
#define XGUI_WORKSHOP_H

#include "XGUI.h"

#include <ModelAPI_Document.h>
#include <ModelAPI_Feature.h>

#include <ModuleBase_ActionInfo.h>
#include <ModuleBase_ActionType.h>
#include <ModuleBase_Definitions.h>

#include <SelectMgr_ListOfFilter.hxx>

#include <QIcon>
#include <QObject>
#include <QKeySequence>
#include <QMap>
#include <QTemporaryDir>

#ifndef HAVE_SALOME
class AppElements_Command;
class AppElements_MainWindow;
class AppElements_Workbench;
#endif

class Config_DataModelReader;

class XGUI_ActionsMgr;
class XGUI_ActiveControlMgr;
class XGUI_ContextMenuMgr;
class XGUI_Displayer;
class XGUI_ErrorDialog;
class XGUI_ErrorMgr;
class XGUI_FacesPanel;
class XGUI_MenuMgr;
class XGUI_ModuleConnector;
class XGUI_ObjectsBrowser;
class XGUI_OperationMgr;
class XGUI_PropertyPanel;
class XGUI_SalomeConnector;
class XGUI_SalomeViewer;
class XGUI_SelectionActivate;
class XGUI_SelectionMgr;
class XGUI_ViewerProxy;
class XGUI_WorkshopListener;
class XGUI_InspectionPanel;

class ModuleBase_IModule;
class ModuleBase_IViewer;
class ModuleBase_Operation;

class QAction;
class QDockWidget;
class QMainWindow;
class QWidget;

/// \class XGUI_Workshop
/// \ingroup GUI
/// \brief Class which defines a configuration of the application (Workshop) and launches it.
class XGUI_EXPORT XGUI_Workshop : public QObject
{
Q_OBJECT
 public:
  /// Constructor
  /// \param theConnector a Salome connector object.
  /// Used only if the workshop is launched in Salome environment
  XGUI_Workshop(XGUI_SalomeConnector* theConnector = 0);
  virtual ~XGUI_Workshop();

  /// Starting of the application
  void startApplication();

  /// Activates the module controls. Should be called after module creation
  void activateModule();

  /// Deactivates the module controls. Should be called after module creation
  void deactivateModule();

#ifndef HAVE_SALOME
  /// Returns main window (Desktop) of the application
  AppElements_MainWindow* mainWindow() const { return myMainWindow; }

  /// Creates and adds a new workbench (menu group) with the given name and returns it
  AppElements_Workbench* addWorkbench(const QString& theName);
#endif

  /// Returns selection manager object
  XGUI_SelectionMgr* selector() const { return mySelector; }

  /// Returns selection activating object
  XGUI_SelectionActivate* selectionActivate() const { return mySelectionActivate; }

  /// Returns displayer
  XGUI_Displayer* displayer() const { return myDisplayer; }

  /// Returns operation manager.
  XGUI_OperationMgr* operationMgr() const { return myOperationMgr; }

  /// Returns error manager.
  XGUI_ErrorMgr* errorMgr() const { return myErrorMgr; }

  /// Returns an actions manager
  XGUI_ActionsMgr* actionsMgr() const { return myActionsMgr; }

  /// Returns an active control manager
  XGUI_ActiveControlMgr* activeControlMgr() const { return myActiveControlMgr; }

  /// Returns an actions manager
  XGUI_MenuMgr* menuMgr() const { return myMenuMgr; }

  /// Returns property panel widget
  XGUI_PropertyPanel* propertyPanel() const { return myPropertyPanel; }

  /// Returns property panel widget
  XGUI_InspectionPanel* inspectionPanel() const { return myInspectionPanel; }

  /// Returns panel for hide object faces
  XGUI_FacesPanel* facesPanel() const { return myFacesPanel; }

  /// Returns context menu manager object
  XGUI_ContextMenuMgr* contextMenuMgr() const { return myContextMenuMgr; }

  /// Returns an object which provides interface to Salome Module (LightApp_Module)
  XGUI_SalomeConnector* salomeConnector() const { return mySalomeConnector; }

  /// Provides an object which provides interface to Salome Viewer
  ModuleBase_IViewer* salomeViewer() const;

  /// Returns true if the application works as SALOME module
  bool isSalomeMode() const { return mySalomeConnector != 0; }

  /// Returns Object browser
  XGUI_ObjectsBrowser* objectBrowser() const { return myObjectBrowser; }

  /// This method is called by Salome module when selection is changed
  void salomeViewerSelectionChanged();

  /// Returns viewer which unifies access as to Salome viewer as to standalone viewer
  XGUI_ViewerProxy* viewer() const { return myViewerProxy; }

  /// Returns the module connector
  /// \returns the instance of connector
  XGUI_ModuleConnector* moduleConnector() const { return myModuleConnector; }

  /// Returns a desktop
  /// \return a desktop instance
  QMainWindow* desktop() const;

  /// If faces panel made the object hidden, show message box whether the object should be
  /// restored (removed from the panel) and displayed, if answer is No, returns false
  /// \param theObject a model object
  /// \return boolean state if the object should not be displayed
  virtual bool prepareForDisplay(const std::set<ObjectPtr>& theObjects) const;

  /// Delete features
  void deleteObjects();

  /// Searches for selected features unused in other (not selected) features. If one or several
  /// selected features are found, a warning message proposes to delete them. It contains
  /// the list of features to be deleted.
  void cleanHistory();

  /// Returns true if the selected feature can be moved to the position after the current feature
  /// \return boolean value
  bool canMoveFeature();

  /// Move selected features to be after the current feature
  void moveObjects();

  /// Returns true if the object can be shaded. If the object is a compsolid result, the method
  /// checks subobjects of the result
  /// \return boolean value
  bool canBeShaded(const ObjectPtr& theObject) const;

  /// Returns true if there is at least one selected body/construction/group result
  /// \param theActionName text of the checked action
  /// \return boolean value
  bool canChangeProperty(const QString& theActionName) const;

  /// Change color of the results if it is possible
  /// The operation is available for construction, body and group results
  /// theObjects a list of selected objects
  void changeColor(const QObjectPtrList& theObjects);

  /// Change deflection of the results if it is possible
  /// The operation is available for construction, body and group results
  /// theObjects a list of selected objects
  void changeDeflection(const QObjectPtrList& theObjects);

  /// Change transparency of the results if it is possible
  /// The operation is available for construction, body and group results
  /// theObjects a list of selected objects
  void changeTransparency(const QObjectPtrList& theObjects);

  /// Show the given features in 3d Viewer
  void showObjects(const QObjectPtrList& theList, bool isVisible);

  /// Show the given features in 3d Viewer
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
  ModuleBase_IModule* module() const { return myModule; }

  /// Returns current file
  QString currentDataFile() const { return myCurrentFile; }

  /// Returns current file
  void setCurrentDataFile(const QString& theDir) { myCurrentFile = theDir; }

  /// Save the current document into a directory
  /// \param theName - path to the directory
  /// \param theFileNames - returned file names created in this directory
  void saveDocument(const QString& theName, std::list<std::string>& theFileNames);

  /// If there is an active (uncommitted) operation shows a prompt to abort it
  /// and performs abortion if user agreed. Returns true if
  /// - operation aborted successfully
  /// - there is no active operation
  bool abortAllOperations();

  /// Updates workshop state according to the started operation, e.g. visualizes the property panel
  /// and connect to it.
  /// \param theOpertion a started operation
  void operationStarted(ModuleBase_Operation* theOperation);

  /// Delete features. Delete the referenced features. There can be a question with a list of
  /// referenced objects.
  /// \param theFeatures a list of objects to be deleted
  bool deleteFeatures(const QObjectPtrList& theFeatures);

  /// Deactivates the object, if it is active and the module returns that the activation
  /// of selection for the object is not possible currently(the current operation uses it)
  /// \param theObject an object
  /// \param theUpdateViewer a boolean flag to update viewer immediately
  void deactivateActiveObject(const ObjectPtr& theObject, const bool theUpdateViewer);

  /// Returns true if the action of the feature is created to contain Accept/Cancel button
  /// \param theFeature a feature
  bool isFeatureOfNested(const FeaturePtr& theFeature);

  /// Shows the message in the status bar
  /// \param theMessage a message
  void setStatusBarMessage(const QString& theMessage);

#ifdef HAVE_SALOME
  /// Has to be called in order to display objects with visibility status = true
  void synchronizeViewer();

  /// Has to be called in order to display objects from a specifed group
  /// with visibility status = true
  /// \param theDoc the document for objects synchronisation
  /// \param theGroup the group name
  /// \param theUpdateViewer update viewer flag
  void synchronizeGroupInViewer(const DocumentPtr& theDoc,
                                const std::string& theGroup,
                                bool theUpdateViewer);

#endif
  /// Update the property panel content by the XML description of the operation and set the panel
  /// into the operation
  /// \param theOperation an operation
  void fillPropertyPanel(ModuleBase_Operation* theOperation);

  /// Connects or disconnects to the value changed signal of the property panel widgets
  /// \param isToConnect a boolean value whether connect or disconnect
  void connectToPropertyPanel(const bool isToConnect);

  /// Returns defailt selection mode in 3d viewer
  QIntList viewerSelectionModes() const { return myViewerSelMode; }

  /// Highlights result objects in Object Browser according to
  /// features found in the given list
  void highlightResults(const QObjectPtrList& theObjects);

  /// Highlights feature objects in Object Browser according to
  /// features found in the given list
  void highlightFeature(const QObjectPtrList& theObjects);

  /// Returns Data Model XML reader which contains information about
  /// Data structure configuration
  const Config_DataModelReader* dataModelXMLReader() const { return myDataModelXMLReader; }

  /// A constant string used for "Move to end" command definition
  /// It is used for specific processing of Undo/Redo for this command.
  static QString MOVE_TO_END_COMMAND;

  /// Closes all in the current session and load the directory
  /// \param theDirectory a path to directory
  void openFile(const QString& theDirectory);

  void updateAutoComputeState();

signals:
  /// Emitted when selection happens in Salome viewer
  void salomeViewerSelection();

  /// the signal about the workshop actions states are updated.
  void commandStatusUpdated();

  /// the application is started
  void applicationStarted();

  /// Signal to update Undo history list
  void updateUndoHistory(const QList<ActionInfo>&);

  /// Signal to update Redo history list
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

  /// Validates the operation to change the "Apply" button state.
  /// \param thePreviousState the previous state of the widget
  void onWidgetStateChanged(int thePreviousState);

  /// Calls activate of the current widget again. It is possible that selection filters are
  /// differs in the new state of paged container
  void onValuesChanged();

  /// Listens the corresponded signal of model widget and updates Apply button state by feature
  void onWidgetObjectUpdated();

  /// Show dock widget panel
  void showPanel(QDockWidget* theDockWidget);

  /// Hide dock widget panel
  void hidePanel(QDockWidget* theDockWidget);

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

  /// A slot calleon toggle of auto-compute button
  void onAutoApply();

  /// Activates/deactivates the trihedron in the viewer AIS context
  void onTrihedronVisibilityChanged(bool theState);

  /// Apply the current transparency value if preview in transparency dialog is switched on
  void onTransparencyValueChanged();

  /// Switch on/off preview of transparency change
  void onPreviewStateChanged();

 protected:
  /// Sets the granted operations for the parameter operation. Firstly, it finds the nested features
  /// and set them into the operation. Secondly, it asks the module about ids of granted operations.
  /// \param theOperation an operation
   void setGrantedFeatures(ModuleBase_Operation* theOperation);

private:
  /// Display results from document
  /// \param theDoc a document
  void displayDocumentResults(DocumentPtr theDoc);

  /// Display results from a group
  void displayGroupResults(DocumentPtr theDoc, std::string theGroup);

  /// Insert folder object before currently selected feature
  void insertFeatureFolder();

  /// Insert an object to a folder above or below
  void insertToFolder(bool isBefore);

  /// Insert an object to a folder above or below
  void moveOutFolder(bool isBefore);

 private slots:
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

  /// Called by OkPlus button clicked in the property panel. Asks the error manager whether
  /// the operation can be committed and do it if it returns true.
  void onAcceptPlusActionClicked();

  /// Called by Preview button clicked in the property panel. Sends signal to model to
  /// compute preview.
  void onPreviewActionClicked();

  /// Called on help button clicked in the property panel.
  void onHelpActionClicked();

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

  // Creates Dock widgets: Object browser and Property panel
  void createDockWidgets();

  /// Extends undo/redo toolbutton's with history menu
  /// \param theObject - in the OpenParts it is a QToolButton by itself,
  /// in salome mode - QAction that creates a button.
  /// \param theSignal - void "updateUndoHistory" or "updateRedoHistory" SIGNAL;
  /// \param theSlot - onUndo(int) or onRedo(int) SLOT
  void addHistoryMenu(QObject* theObject, const char* theSignal, const char* theSlot);

  /// Creates list of actions (commands) by given history list from session
  QList<ActionInfo> processHistoryList(const std::list<std::string>&) const;

  /// Pefrom Undo/Redo and necessary workshop updates(viewer, browser, actions state)
  /// \param theActionType a type of the action (Undo or Redo)
  /// \param theTimes number of applies the given action
  void processUndoRedo(const ModuleBase_ActionType theActionType, int theTimes);

  /// Clear content of temporary directory
  void clearTemporaryDir();

private:
#ifndef HAVE_SALOME
  AppElements_MainWindow* myMainWindow; ///< desktop window
#endif

  ModuleBase_IModule* myModule; ///< current module
  XGUI_ErrorMgr* myErrorMgr; ///< updator of error message
  XGUI_ObjectsBrowser* myObjectBrowser; ///< data tree widget
  XGUI_PropertyPanel* myPropertyPanel; ///< container of feature attributes widgets
  XGUI_FacesPanel* myFacesPanel; ///< panel for hide object faces
  XGUI_SelectionMgr* mySelector; ///< handler of selection processing
  XGUI_SelectionActivate* mySelectionActivate; /// manager of selection activating
  XGUI_Displayer* myDisplayer; ///< handler of objects display
  XGUI_OperationMgr* myOperationMgr;  ///< manager to manipulate through the operations
  XGUI_ActionsMgr* myActionsMgr; ///< manager of workshop actions
  XGUI_ActiveControlMgr* myActiveControlMgr; ///< manager to have none or one active control
  XGUI_MenuMgr* myMenuMgr; ///< manager to build menu/tool bar using order defined in XML
  XGUI_SalomeConnector* mySalomeConnector; ///< connector to SALOME module interface
  XGUI_ErrorDialog* myErrorDlg; ///< dialog to show information of occured error events
  XGUI_ViewerProxy* myViewerProxy; ///< connector to SALOME viewer interface
  XGUI_ContextMenuMgr* myContextMenuMgr; ///< manager of context menu build
  XGUI_ModuleConnector* myModuleConnector; ///< implementation of ModuleBase_IWorkshop
  XGUI_WorkshopListener* myEventsListener; ///< processing of events
  QString myCurrentFile; ///< cached the last open directory
  QIntList myViewerSelMode; ///< selection modes set in the viewer
  Config_DataModelReader* myDataModelXMLReader; ///< XML reader of data model
  XGUI_InspectionPanel* myInspectionPanel; ///< container of feature attributes widgets
  QTemporaryDir myTmpDir; ///< a direcory for uncompressed files
};

#endif
