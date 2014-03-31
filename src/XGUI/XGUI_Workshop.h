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

class Config_FeatureMessage;

class XGUI_Workshop: public QObject, public Event_Listener
{
Q_OBJECT
public:

  XGUI_Workshop();
  virtual ~XGUI_Workshop();

  void startApplication();

  XGUI_MainWindow* mainWindow() const
  {
    return myMainWindow;
  }

  XGUI_Workbench* addWorkbench(const QString& theName);

  virtual void ProcessEvent(const Event_Message* theMessage);

public slots:
  void onNew();
  void onOpen();
  void onSave();
  void onSaveAs();
  void onExit();

protected:
  //Event-loop processing methods:
  void addFeature(const Config_FeatureMessage*);

private:
  void initMenu();

  XGUI_Module* loadModule(const QString& theModule);
  bool activateModule();

  XGUI_MainWindow* myMainWindow;
  XGUI_Module* myPartSetModule;
};

#endif
