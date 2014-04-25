

#ifndef NewGeom_Module_H
#define NewGeom_Module_H

#include "NewGeom.h"

#include <LightApp_Module.h>
#include <XGUI_SalomeConnector.h>

#include <QStringList>

class XGUI_Workshop;

class NewGeom_EXPORT NewGeom_Module: public LightApp_Module, public XGUI_SalomeConnector
{
  Q_OBJECT
public:
  NewGeom_Module();
  virtual ~NewGeom_Module();

  virtual void initialize( CAM_Application* theApp);
  virtual void windows( QMap<int, int>& theWndMap) const;
  virtual void viewManagers( QStringList& theList) const;

  //--- XGUI connector interface -----
  virtual void addFeature(const QString& theWBName,
                          const QString& theId, 
                          const QString& theTitle, 
                          const QString& theTip,
                          const QIcon& theIcon, 
                          bool isCheckable = false,
                          QObject* reciever = 0,
                          const char* member = 0,
                          const QKeySequence& theKeys = QKeySequence());

  virtual void addEditCommand(const QString& theId,
                              const QString& theTitle,
                              const QString& theTip,
                              const QIcon& theIcon, 
                              bool isCheckable,
                              QObject* reciever,
                              const char* member,
                              const QKeySequence& theKeys);

  virtual void addEditMenuSeparator();

  virtual QMainWindow* desktop() const;

  virtual QString commandId(const QAction* theCmd) const;

  virtual QAction* command(const QString& theId) const;

public slots:
  bool activateModule( SUIT_Study* theStudy);
  bool deactivateModule( SUIT_Study* theStudy);

protected:
  CAM_DataModel* createDataModel();


private:
  QStringList myActionsList;

  XGUI_Workshop* myWorkshop;
};

#endif
