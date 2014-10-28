
#ifndef NewGeom_Module_H
#define NewGeom_Module_H

#include "NewGeom.h"
#include "NewGeom_SalomeViewer.h"

#include <LightApp_Module.h>
#include <XGUI_SalomeConnector.h>

#include <QStringList>
#include <QMap>

class XGUI_Workshop;
class NewGeom_OCCSelector;
class OCCViewer_Viewer;

/** 
 * An implementation of SALOME connector class for implementation of
 * XGUI functionality as a module of SALOME
 */
class NewGeom_EXPORT NewGeom_Module : public LightApp_Module, public XGUI_SalomeConnector
{
Q_OBJECT
 public:
  NewGeom_Module();
  virtual ~NewGeom_Module();

  //----- LightAPP_Module interface ---------------
  virtual void initialize(CAM_Application* theApp);
  virtual void windows(QMap<int, int>& theWndMap) const;
  virtual void viewManagers(QStringList& theList) const;
  virtual void selectionChanged();

  //--- XGUI connector interface -----
  virtual QAction* addFeature(const QString& theWBName, const QString& theId,
                              const QString& theTitle, const QString& theTip, const QIcon& theIcon,
                              const QKeySequence& theKeys = QKeySequence(),
                              bool isCheckable = false);

  virtual QAction* addEditCommand(const QString& theId, const QString& theTitle,
                                  const QString& theTip, const QIcon& theIcon,
                                  const QKeySequence& theKeys, bool isCheckable);

  virtual void addEditMenuSeparator();

  virtual QMainWindow* desktop() const;

  virtual QString commandId(const QAction* theCmd) const;

  virtual QAction* command(const QString& theId) const;

  //! Set nested actions dependent on command Id
  //! \param theId - the command ID
  //! \param theActions - the list of nested actions
  virtual void setNestedActions(const QString& theId, const QStringList& theActions);

  //! Returns list of nested actions according to the given command ID
  virtual QStringList nestedActions(const QString& theId) const;

  //! Returns interface to Salome viewer
  virtual ModuleBase_IViewer* viewer() const
  {
    return myProxyViewer;
  }

  //! Returns list of defined actions (just by NewGeom module)
  virtual QList<QAction*> commandList() const;

  //! Returns list of Ids of defined actions (just by NewGeom module)
  virtual QStringList commandIdList() const;

  virtual void contextMenuPopup(const QString& theClient, QMenu* theMenu, QString& theTitle);

  virtual void createPreferences();
  virtual void preferencesChanged(const QString& theSection, const QString& theParam);

  XGUI_Workshop* workshop() const { return myWorkshop; }

  void setIsOpened(bool theOpened) { myIsOpened = theOpened; }

 public slots:
  virtual bool activateModule(SUIT_Study* theStudy);
  virtual bool deactivateModule(SUIT_Study* theStudy);

 protected slots:
  virtual void onViewManagerAdded(SUIT_ViewManager* theMgr);
  void onDefaultPreferences();

 protected:
  CAM_DataModel* createDataModel();

 private:
  NewGeom_OCCSelector* createSelector(SUIT_ViewManager* theMgr);

  QStringList myActionsList;

  XGUI_Workshop* myWorkshop;

  NewGeom_OCCSelector* mySelector;

  NewGeom_SalomeViewer* myProxyViewer;

  QMap<QString, QStringList> myNestedActions;

  bool myIsOpened;
  bool myIsStorePositions;

};

#endif
