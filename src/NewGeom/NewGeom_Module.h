

#ifndef NewGeom_Module_H
#define NewGeom_Module_H

#include "NewGeom.h"

#include <LightApp_Module.h>
#include <XGUI_SalomeConnector.h>

#include <QStringList>

class XGUI_Workshop; 
class NewGeom_OCCSelector;
class OCCViewer_Viewer;
/** 
* An implementation of SALOME connector class for implementation of
* XGUI functionality as a module of SALOME
*/
class NewGeom_EXPORT NewGeom_Module: public LightApp_Module, public XGUI_SalomeConnector
{
  Q_OBJECT
public:
  NewGeom_Module();
  virtual ~NewGeom_Module();

  //----- LightAPP_Module interface ---------------
  virtual void initialize( CAM_Application* theApp);
  virtual void windows( QMap<int, int>& theWndMap) const;
  virtual void viewManagers( QStringList& theList) const;
  virtual void selectionChanged();

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

  //! Returns AIS_InteractiveContext from current OCCViewer
  virtual Handle(AIS_InteractiveContext) AISContext() const;

public slots:
  virtual bool activateModule( SUIT_Study* theStudy);
  virtual bool deactivateModule( SUIT_Study* theStudy);

protected slots:
  virtual void onViewManagerAdded( SUIT_ViewManager* theMgr );

protected:
  CAM_DataModel* createDataModel();


private:
  NewGeom_OCCSelector* createSelector(SUIT_ViewManager* theMgr);

  QStringList myActionsList;

  XGUI_Workshop* myWorkshop;

  NewGeom_OCCSelector* mySelector;
};

#endif
