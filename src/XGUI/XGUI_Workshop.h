#ifndef XGUI_WORKSHOP_H
#define XGUI_WORKSHOP_H

#include <Event_Message.h>
#include <Event_Listener.h>

#include <QObject>
#include <QMap>
#include <QIcon>
#include <QKeySequence>

class XGUI_MainWindow;
class XGUI_Command;
class XGUI_Module;
class XGUI_Workbench;
class ModuleBase_Operation;

class Config_FeatureMessage;
class Config_PointerMessage;

/**\class XGUI_Workshop
 * \ingroup GUI
 * \brief Class which defines a configuration of the application (Workshop) and launches it.
 */
class XGUI_Workshop: public QObject, public Event_Listener
{
Q_OBJECT
public:

  XGUI_Workshop();
  virtual ~XGUI_Workshop();

  //! Starting of the application
  void startApplication();

  //! Returns main window (Desktop) of the application
  XGUI_MainWindow* mainWindow() const
  {
    return myMainWindow;
  }

  //! Creates and adds a new workbench (menu group) with the given name and returns it
  XGUI_Workbench* addWorkbench(const QString& theName);

  //! Redefinition of Event_Listener method
  virtual void processEvent(const Event_Message* theMessage);

public slots:
  void onNew();
  void onOpen();
  void onSave();
  void onSaveAs();
  void onExit();

protected:
  //Event-loop processing methods:
  void addFeature(const Config_FeatureMessage*);
  void fillPropertyPanel(ModuleBase_Operation* theOperation);
  void connectToPropertyPanel(ModuleBase_Operation* theOperation);
  void setCurrentOperation(ModuleBase_Operation* theOperation);

private:
  void initMenu();

  XGUI_Module* loadModule(const QString& theModule);
  bool activateModule();

  XGUI_MainWindow* myMainWindow;
  XGUI_Module* myPartSetModule;

  ModuleBase_Operation* myCurrentOperation;
};

#endif
