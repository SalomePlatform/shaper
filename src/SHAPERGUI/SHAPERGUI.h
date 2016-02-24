// Copyright (C) 2014-20xx CEA/DEN, EDF R&D


#ifndef SHAPERGUI_H
#define SHAPERGUI_H

#include "SHAPER_SHAPERGUI.h"
#include "SHAPERGUI_SalomeViewer.h"

#include <LightApp_Module.h>
#include <XGUI_SalomeConnector.h>

#include <ModuleBase_ActionInfo.h>

#include <QStringList>
#include <QMap>

class XGUI_Workshop;
class SHAPERGUI_OCCSelector;
class OCCViewer_Viewer;
class CAM_Study;

/** 
 * \ingroup Salome
 * An implementation of SALOME connector class for implementation of
 * XGUI functionality as a module of SALOME
 */
class SHAPERGUI_EXPORT SHAPERGUI : public LightApp_Module, public XGUI_SalomeConnector
{
Q_OBJECT
 public:
  SHAPERGUI();
  virtual ~SHAPERGUI();

  //----- LightAPP_Module interface ---------------

  /// \brief Initializing of the module
  /// \param theApp application instance
  virtual void initialize(CAM_Application* theApp);

  /// \brief Definition of module standard windows
  virtual void windows(QMap<int, int>& theWndMap) const;

  /// \brief Definition of module viewer 
  virtual void viewManagers(QStringList& theList) const;

  /// \brief The method is called on selection changed event
  virtual void selectionChanged();

  //--- XGUI connector interface -----

  virtual QAction* addFeature(const QString& theWBName, const QString& theId,
                              const QString& theTitle, const QString& theTip, const QIcon& theIcon,
                              const QKeySequence& theKeys = QKeySequence(),
                              bool isCheckable = false);

  //! Add feature (QAction) in the \a theWBName toolbar with given \a theInfo about action
  virtual QAction* addFeature(const QString& theWBName,
                              const ActionInfo& theInfo);

  /// Add a nested feature
  /// \param theWBName a workbench name
  /// \param theInfo the action  parameters
  /// \param theNestedActions a list of nested actions
  virtual QAction* addFeatureOfNested(const QString& theWBName,
                                    const ActionInfo& theInfo,
                                    const QList<QAction*>& theNestedActions);

  //! Returns true if the feature action is a nested action, in other words,
  //! it is created by addNestedFeature().
  //! \param theAction - an action of a feature
  //! returns boolean result
  virtual bool isFeatureOfNested(const QAction* theAction);

  virtual QAction* addDesktopCommand(const QString& theId, const QString& theTitle,
                                     const QString& theTip, const QIcon& theIcon,
                                     const QKeySequence& theKeys, bool isCheckable,
                                     const char* theMenuSourceText,
                                     const int theMenuPosition = 10);

  virtual void addDesktopMenuSeparator(const char* theMenuSourceText,
                                       const int theMenuPosition = 10);

  /// Add an action to a tool bar
  /// \param theAction an ation to add
  /// \param theToolBarTitle a name of tool bar
  virtual bool addActionInToolbar( QAction* theAction, const QString& theToolBarTitle );
    
  virtual QMainWindow* desktop() const;

  virtual QString commandId(const QAction* theCmd) const;

  virtual QAction* command(const QString& theId) const;

  //! Set nested actions dependent on command Id
  //! \param theId - the command ID
  //! \param theActions - the list of nested actions
  virtual void setNestedActions(const QString& theId, const QStringList& theActions);

  //! Returns list of nested actions according to the given command ID
  virtual QStringList nestedActions(const QString& theId) const;

  //! Set the document kind of the action by the given command Id
  //! \param theId - the command ID
  //! \param theKind - the document kind
  virtual void setDocumentKind(const QString& theId, const QString& theKind);

  //! Returns the document kind of the action by the given command ID
  virtual QString documentKind(const QString& theId) const;

  //! Returns interface to Salome viewer
  virtual ModuleBase_IViewer* viewer() const
  {
    return myProxyViewer;
  }

  //! Returns list of defined actions (just by NewGeom module)
  virtual QList<QAction*> commandList() const;

  //! Returns list of Ids of defined actions (just by NewGeom module)
  virtual QStringList commandIdList() const;

  /// Redefinition of virtual function. 
  /// \param theClient name of pop-up client
  /// \param theMenu popup menu instance
  /// \param theTitle menu title.
  virtual void contextMenuPopup(const QString& theClient, QMenu* theMenu, QString& theTitle);

  /// Redefinition of virtual function for preferences creation. 
  virtual void createPreferences();

  /// Redefinition of virtual function for preferences changed event. 
  virtual void preferencesChanged(const QString& theSection, const QString& theParam);
  
  /// \return Workshop class instance
  XGUI_Workshop* workshop() const { return myWorkshop; }

  /// \brief Set flag about opened document state
  void setIsOpened(bool theOpened) { myIsOpened = theOpened; }

  /// Register current modules of SALOME
  void inspectSalomeModules();

 public slots:
  /// \brief The method is redefined to connect to the study viewer before the data
  /// model is filled by opened file. This file open will flush redisplay signals for,
  /// objects which should be visualized
  virtual void connectToStudy(CAM_Study* theStudy);

   /// \brief The method is called on the module activation
   /// \param theStudy current study
  virtual bool activateModule(SUIT_Study* theStudy);

   /// \brief The method is called on the module activation
   /// \param theStudy current study
  virtual bool deactivateModule(SUIT_Study* theStudy);

 protected slots:
   /// Redefinition of virtual function
   /// \param theMgr view manager
  virtual void onViewManagerAdded(SUIT_ViewManager* theMgr);

   /// Redefinition of virtual function
   /// \param theMgr view manager
  virtual void onViewManagerRemoved(SUIT_ViewManager* theMgr);

  /// Set preferences to default
  void onDefaultPreferences();

  /// Obtains the current application and updates its actions
  void onUpdateCommandStatus();

 protected:
   /// Create data model
  CAM_DataModel* createDataModel();

  /// Create popup menu manager
  virtual QtxPopupMgr* popupMgr();

  /// Abort all operations
  virtual bool abortAllOperations();

 private:
   /// Create selector for OCC Viewer
   /// \param theMgr view manager
  SHAPERGUI_OCCSelector* createSelector(SUIT_ViewManager* theMgr);

  /// List of registered actions
  QStringList myActionsList;

  /// Reference to workshop
  XGUI_Workshop* myWorkshop;

  /// OCC viewer selector instance
  SHAPERGUI_OCCSelector* mySelector;

  /// Proxy viewer for connection to OCC Viewer in SALOME
  SHAPERGUI_SalomeViewer* myProxyViewer;

  /// Map of nested actions [ActionID: list of nested actions Id]
  QMap<QString, QStringList> myNestedActions;

  /// Map of document types
  QMap<QString, QString> myDocumentType;

  /// Flag of opened document state
  bool myIsOpened;

  // the next parameters should be restored after this module deactivation

  /// The application value of the preferences parameter
  bool myIsStorePositions; 

  /// The application value
  bool myIsEditEnabled;    

  /// Popup manager
  QtxPopupMgr* myPopupMgr;
};

#endif
