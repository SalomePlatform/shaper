

#include "NewGeom_Module.h"
#include "NewGeom_DataModel.h"
#include "NewGeom_OCCSelector.h"

#include <XGUI_Workshop.h>
#include <XGUI_PropertyPanel.h>

#include <LightApp_Application.h>
#include <LightApp_SelectionMgr.h>
#include <LightApp_OCCSelector.h>
#include <OCCViewer_ViewModel.h>

#include <SUIT_Selector.h>
#include <SUIT_Desktop.h>
#include <SUIT_ViewManager.h>

#include <QtxActionMenuMgr.h>

#include <QDockWidget>

extern "C" {
  NewGeom_EXPORT CAM_Module* createModule() {
    return new NewGeom_Module();
  }
    
  NewGeom_EXPORT char* getModuleVersion() {
    return "0.0";
  }
}


//******************************************************
NewGeom_Module::NewGeom_Module()
: LightApp_Module( "NewGeom" ), mySelector(0)
{
  myWorkshop = new XGUI_Workshop(this);
  myProxyViewer = new NewGeom_SalomeViewer(this);
}

//******************************************************
NewGeom_Module::~NewGeom_Module()
{
}

//******************************************************
void NewGeom_Module::initialize(CAM_Application* theApp)
{
  LightApp_Module::initialize(theApp);
  
  myWorkshop->startApplication();
}

//******************************************************
void NewGeom_Module::windows(QMap<int, int>& theWndMap) const
{
  theWndMap.insert( LightApp_Application::WT_PyConsole, Qt::BottomDockWidgetArea );
}

//******************************************************
void NewGeom_Module::viewManagers(QStringList& theList) const
{
  theList.append( OCCViewer_Viewer::Type() );
}

//******************************************************
bool NewGeom_Module::activateModule(SUIT_Study* theStudy)
{
  bool isDone = LightApp_Module::activateModule(theStudy);
  if (isDone) {
    setMenuShown( true );
    setToolShown( true );
    myWorkshop->propertyPanel()->hide();

    if (!mySelector) {
      ViewManagerList OCCViewManagers;
      application()->viewManagers(OCCViewer_Viewer::Type(), OCCViewManagers);
      if (OCCViewManagers.size() > 0) {
        mySelector = createSelector(OCCViewManagers.first());
      }
    }
  }
  return isDone;
}

//******************************************************
void NewGeom_Module::onViewManagerAdded( SUIT_ViewManager* theMgr )
{
  if ((!mySelector)) {
    mySelector = createSelector(theMgr);
  }
}

//******************************************************
NewGeom_OCCSelector* NewGeom_Module::createSelector(SUIT_ViewManager* theMgr)
{
  if (theMgr->getType() == OCCViewer_Viewer::Type()) {
    OCCViewer_Viewer* aViewer = static_cast<OCCViewer_Viewer*>(theMgr->getViewModel());
    NewGeom_OCCSelector* aSelector = new NewGeom_OCCSelector(aViewer, 
                                                             getApp()->selectionMgr());
    LightApp_SelectionMgr* aMgr = getApp()->selectionMgr();
    QList<SUIT_Selector*> aList;
    aMgr->selectors(aList);
    foreach(SUIT_Selector* aSel, aList) {
      aSel->setEnabled(aSel == aSelector);
    }
    myProxyViewer->setSelector(aSelector);
    return aSelector;
  }
  return 0;
}

//******************************************************
bool NewGeom_Module::deactivateModule(SUIT_Study* theStudy)
{
  setMenuShown( false );
  setToolShown( false );
  return LightApp_Module::deactivateModule(theStudy);
}

//******************************************************
CAM_DataModel* NewGeom_Module::createDataModel()
{
  return new NewGeom_DataModel( this );
}

//******************************************************
void NewGeom_Module::addFeature(const QString& theWBName,
                                const QString& theId, 
                                const QString& theTitle, 
                                const QString& theTip,
                                const QIcon& theIcon, 
                                bool isCheckable,
                                QObject* theReciever,
                                const char* theMember,
                                const QKeySequence& theKeys)
{
  int aMenu = createMenu(theWBName, -1, -1, 50);
  int aTool = createTool(theWBName);

  int aId = myActionsList.size();
  myActionsList.append(theId);
  SUIT_Desktop* aDesk = application()->desktop();
  int aKeys = 0;
  for (int i = 0; i < theKeys.count(); i++) 
    aKeys += theKeys[i];
  createAction(aId, theTip, theIcon, theTitle, theTip, aKeys, aDesk, 
               isCheckable, theReciever, theMember);
  int aItemId = createMenu( aId,  aMenu, -1, 10 );
  int aToolId = createTool( aId, aTool );
}

//******************************************************
void NewGeom_Module::addEditCommand(const QString& theId,
                                    const QString& theTitle,
                                    const QString& theTip,
                                    const QIcon& theIcon, 
                                    bool isCheckable,
                                    QObject* theReciever,
                                    const char* theMember,
                                    const QKeySequence& theKeys)
{
  int aMenu = createMenu(tr( "MEN_DESK_EDIT" ), -1, -1);

  int aId = myActionsList.size();
  myActionsList.append(theId);
  SUIT_Desktop* aDesk = application()->desktop();
  int aKeys = 0;
  for (int i = 0; i < theKeys.count(); i++) 
    aKeys += theKeys[i];
  createAction(aId, theTip, theIcon, theTitle, theTip, aKeys, aDesk, 
               isCheckable, theReciever, theMember);
  createMenu( aId, aMenu, 10 );
}

//******************************************************
void NewGeom_Module::addEditMenuSeparator()
{
  int aMenu = createMenu(tr( "MEN_DESK_EDIT" ), -1, -1);
  createMenu( separator(), aMenu, -1, 10 );
}

//******************************************************
QMainWindow* NewGeom_Module::desktop() const
{
  return application()->desktop();
}

//******************************************************
QString NewGeom_Module::commandId(const QAction* theCmd) const
{
  int aId = actionId(theCmd);
  if (aId < myActionsList.size())
    return myActionsList[aId];
  return QString();
}

//******************************************************
QAction* NewGeom_Module::command(const QString& theId) const
{
  int aId = myActionsList.indexOf(theId);
  if ((aId != -1) && (aId < myActionsList.size())) {
    return action(aId);
  }
  return 0;
}

//******************************************************
void NewGeom_Module::setNestedActions(const QString& theId, const QStringList& theActions)
{
  myNestedActions[theId] = theActions;
}

//******************************************************
QStringList NewGeom_Module::nestedActions(const QString& theId) const
{
  if (myNestedActions.contains(theId))
    return myNestedActions[theId];
  return QStringList();
}

//******************************************************
void NewGeom_Module::selectionChanged()
{
  LightApp_Module::selectionChanged();
  myWorkshop->salomeViewerSelectionChanged();
}
