#ifndef XGUI_WORKSHOP_H
#define XGUI_WORKSHOP_H

#include "XGUI.h"
#include <Events_Listener.h>

#include <QObject>
#include <QMap>
#include <QIcon>
#include <QKeySequence>

class XGUI_MainWindow;
class XGUI_Command;
class XGUI_Module;
class XGUI_Workbench;
class XGUI_SelectionMgr;
class XGUI_Displayer;
class XGUI_OperationMgr;
class XGUI_SalomeConnector;
class XGUI_ObjectsBrowser;
class ModuleBase_Operation;
class ModuleBase_PropPanelOperation;

class Config_FeatureMessage;
class Config_PointerMessage;

class QWidget;
class QDockWidget;

/**\class XGUI_Workshop
 * \ingroup GUI
 * \brief Class which defines a configuration of the application (Workshop) and launches it.
 */
class XGUI_EXPORT XGUI_Workshop: public QObject, public Events_Listener
{
Q_OBJECT
public:

  XGUI_Workshop(XGUI_SalomeConnector* theConnector = 0);
  virtual ~XGUI_Workshop();

  //! Starting of the application
  void startApplication();

  //! Returns main window (Desktop) of the application
  XGUI_MainWindow* mainWindow() const
  {
    return myMainWindow;
  }

  //! Returns selection manager object
  XGUI_SelectionMgr* selector() const { return mySelector; }

  //! Returns displayer
  XGUI_Displayer* displayer() const;

  //! ! Returns operation manager.
  XGUI_OperationMgr* operationMgr() const { return myOperationMgr; }

  //! Creates and adds a new workbench (menu group) with the given name and returns it
  XGUI_Workbench* addWorkbench(const QString& theName);

  //! Redefinition of Events_Listener method
  virtual void processEvent(const Events_Message* theMessage);

  XGUI_SalomeConnector* salomeConnector() const { return mySalomeConnector; }

  //! Returns true if the application works as SALOME module
  bool isSalomeMode() const { return mySalomeConnector != 0; }

  //! Returns Object browser
  XGUI_ObjectsBrowser* objectBrowser() const { return myObjectBrowser; }

public slots:
  void updateCommandStatus();

  void onNew();
  void onOpen();
  void onSave();
  void onSaveAs();
  void onExit();
  void onUndo();
  void onRedo();

  void showPropertyPanel();
  void hidePropertyPanel();
  void showObjectBrowser();
  void hideObjectBrowser();

  void onFeatureTriggered();

protected:
  //Event-loop processing methods:
  void addFeature(const Config_FeatureMessage*);
  void connectWithOperation(ModuleBase_Operation* theOperation);

protected slots:
  /// SLOT, that is called after the operation is started. Update workshop state according to
  /// the started operation, e.g. visualizes the property panel and connect to it.
  void onOperationStarted();
  /// SLOT, that is called after the operation is stopped. Update workshop state, e.g.
  /// hides the property panel and udpate the command status.
  /// \param theOpertion a stopped operation
  void onOperationStopped(ModuleBase_Operation* theOperation);

private:
  void initMenu();

  XGUI_Module* loadModule(const QString& theModule);
  bool activateModule();

  QDockWidget* createObjectBrowser(QWidget* theParent);
  QDockWidget* createPropertyPanel(QWidget* theParent);

  // Creates Dock widgets: Object browser and Property panel
  void createDockWidgets();
  void setPropertyPannelTitle(const QString& theTitle);


  XGUI_MainWindow* myMainWindow;
  XGUI_Module* myPartSetModule;

  XGUI_ObjectsBrowser* myObjectBrowser;
  QDockWidget* myPropertyPanelDock;

  XGUI_SelectionMgr* mySelector;
  XGUI_Displayer* myDisplayer;

  XGUI_OperationMgr* myOperationMgr; ///< manager to manipulate through the operations

  XGUI_SalomeConnector* mySalomeConnector;
};

#endif
