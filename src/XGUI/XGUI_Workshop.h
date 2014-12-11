#ifndef XGUI_WORKSHOP_H
#define XGUI_WORKSHOP_H

#include "XGUI.h"
//#include "XGUI_Constants.h"
#include <Events_Listener.h>
#include <ModuleBase_Definitions.h>
#include <ModelAPI_Document.h>
#include <ModelAPI_Feature.h>

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
  ;

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

  void salomeViewerSelectionChanged();

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

  //! Returns icon name according to feature
  static QIcon featureIcon(const FeaturePtr& theFeature);

  //! Activates or deactivates a part
  //! If PartPtr is Null pointer then PartSet will be activated
  void activatePart(std::shared_ptr<ModelAPI_ResultPart> theFeature);

  //! Delete features
  void deleteObjects(const QObjectPtrList& theList);

  //! Show the given features in 3d Viewer
  void showObjects(const QObjectPtrList& theList, bool isVisible);

  //! Show the given features in 3d Viewer
  void showOnlyObjects(const QObjectPtrList& theList);

  void setDisplayMode(const QObjectPtrList& theList, int theMode);

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

  /*
   * If there is an active (uncommited) operation shows a prompt to abort it
   * and performs abortion if user agreed. Returns true if
   * - operation aborted successfully
   * - there is no active operation
   */
  bool isActiveOperationAborted();

signals:
  void salomeViewerSelection();
  void errorOccurred(const QString&);

 public slots:
  void updateCommandStatus();
  void updateCommandsOnViewSelection();

  void onNew();
  void onOpen();
  bool onSave();
  bool onSaveAs();
  void onExit();
  void onUndo();
  void onRedo();
  void onRebuild();
  void onPreferences();

  void showPropertyPanel();
  void hidePropertyPanel();
  void showObjectBrowser();
  void hideObjectBrowser();

  void onFeatureTriggered();
  void changeCurrentDocument(ObjectPtr theObj);

  void activateLastPart();

  void closeDocument();

 protected:
  // Find the nested features and set them into the operation
  // \param theOperation an operation
  void setNestedFeatures(ModuleBase_Operation* theOperation);
  // Update the property panel content by the XML description of the operation and set the panel
  // into the operation
  // \param theOperation an operation
  void setPropertyPanel(ModuleBase_Operation* theOperation);

  bool event(QEvent * theEvent);
  //Event-loop processing methods:
  void addFeature(const std::shared_ptr<Config_FeatureMessage>&);
  void connectWithOperation(ModuleBase_Operation* theOperation);

  void onFeatureUpdatedMsg(const std::shared_ptr<ModelAPI_ObjectUpdatedMessage>& theMsg);
  void onFeatureCreatedMsg(const std::shared_ptr<ModelAPI_ObjectUpdatedMessage>& theMsg);
  void onFeatureRedisplayMsg(const std::shared_ptr<ModelAPI_ObjectUpdatedMessage>& theMsg);
  void onObjectDeletedMsg(const std::shared_ptr<ModelAPI_ObjectDeletedMessage>& theMsg);

  void validateOperation(const QString& theOperationId);

  QList<QAction*> getModuleCommands() const;

  void displayAllResults();
  void displayDocumentResults(DocumentPtr theDoc);
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
  void onOperationStopped(ModuleBase_Operation* theOperation);

  void onContextMenuCommand(const QString& theId, bool isChecked);

  void onWidgetValuesChanged();

  void onStartWaiting();

 private:
  void initMenu();

  void registerValidators() const;

  ModuleBase_IModule* loadModule(const QString& theModule);
  bool activateModule();

  QDockWidget* createObjectBrowser(QWidget* theParent);
  QDockWidget* createPropertyPanel(QWidget* theParent);

  // Creates Dock widgets: Object browser and Property panel
  void createDockWidgets();

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
  static QMap<QString, QString> myIcons;

  bool myUpdatePrefs;

  // Flag to check that part document is in process of activating
  bool myPartActivating;
};

#endif
