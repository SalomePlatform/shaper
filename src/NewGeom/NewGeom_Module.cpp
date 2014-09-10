
#include "NewGeom_Module.h"
#include "NewGeom_DataModel.h"
#include "NewGeom_OCCSelector.h"

#include <XGUI_Workshop.h>
#include <XGUI_PropertyPanel.h>
#include <XGUI_ContextMenuMgr.h>
#include <XGUI_Preferences.h>

#include <LightApp_Application.h>
#include <LightApp_SelectionMgr.h>
#include <LightApp_OCCSelector.h>
#include <OCCViewer_ViewModel.h>

#include <SUIT_Selector.h>
#include <SUIT_Desktop.h>
#include <SUIT_ViewManager.h>
#include <SUIT_ResourceMgr.h>

#include <QtxPopupMgr.h>
#include <QtxActionMenuMgr.h>
#include <QtxResourceMgr.h>

#include <Config_PropManager.h>

#include <QDockWidget>
#include <QAction>

extern "C" {
NewGeom_EXPORT CAM_Module* createModule()
{
  return new NewGeom_Module();
}

NewGeom_EXPORT char* getModuleVersion()
{
  return "0.0";
}
}

class NewGeom_PrefMgr: public XGUI_IPrefMgr
{
public:
  NewGeom_PrefMgr(LightApp_Preferences* theMgr, const QString& theModName):myMgr(theMgr), myModName(theModName) {}

  virtual int addPreference(const QString& theLbl, int pId, 
                            SUIT_PreferenceMgr::PrefItemType theType,
                            const QString& theSection, const QString& theName )
  {
    return myMgr->addPreference(myModName, theLbl, pId, theType, theSection, theName);
  }

  virtual SUIT_PreferenceMgr* prefMgr() const { return myMgr; }

private:
  LightApp_Preferences* myMgr;
  QString myModName;
};




//******************************************************
NewGeom_Module::NewGeom_Module()
    : LightApp_Module("NewGeom"),
      mySelector(0)
{
  myWorkshop = new XGUI_Workshop(this);
  myProxyViewer = new NewGeom_SalomeViewer(this);

  XGUI_Preferences::setResourceMgr(application()->resourceMgr());
  XGUI_Preferences::loadCustomProps();
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
  theWndMap.insert(LightApp_Application::WT_PyConsole, Qt::BottomDockWidgetArea);
}

//******************************************************
void NewGeom_Module::viewManagers(QStringList& theList) const
{
  theList.append(OCCViewer_Viewer::Type());
}

//******************************************************
bool NewGeom_Module::activateModule(SUIT_Study* theStudy)
{
  bool isDone = LightApp_Module::activateModule(theStudy);
  if (isDone) {
    setMenuShown(true);
    setToolShown(true);

    if (!mySelector) {
      ViewManagerList OCCViewManagers;
      application()->viewManagers(OCCViewer_Viewer::Type(), OCCViewManagers);
      if (OCCViewManagers.size() > 0) {
        mySelector = createSelector(OCCViewManagers.first());
      }
    }
    myWorkshop->propertyPanel()->hide();
    QtxPopupMgr* aMgr = popupMgr();  // Create popup manager
    action(myEraseAll)->setEnabled(false);
  }
  return isDone;
}

//******************************************************
bool NewGeom_Module::deactivateModule(SUIT_Study* theStudy)
{
  setMenuShown(false);
  setToolShown(false);
  //myWorkshop->contextMenuMgr()->disconnectViewer();
  return LightApp_Module::deactivateModule(theStudy);
}

//******************************************************
void NewGeom_Module::onViewManagerAdded(SUIT_ViewManager* theMgr)
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
    NewGeom_OCCSelector* aSelector = new NewGeom_OCCSelector(aViewer, getApp()->selectionMgr());
    LightApp_SelectionMgr* aMgr = getApp()->selectionMgr();
    QList<SUIT_Selector*> aList;
    aMgr->selectors(aList);
    foreach(SUIT_Selector* aSel, aList)
    {
      aSel->setEnabled(aSel == aSelector);
    }
    myProxyViewer->setSelector(aSelector);
    return aSelector;
  }
  return 0;
}

//******************************************************
CAM_DataModel* NewGeom_Module::createDataModel()
{
  return new NewGeom_DataModel(this);
}

//******************************************************
QAction* NewGeom_Module::addFeature(const QString& theWBName, const QString& theId,
                                    const QString& theTitle, const QString& theTip,
                                    const QIcon& theIcon, const QKeySequence& theKeys,
                                    bool isCheckable)
{
  int aMenu = createMenu(theWBName, -1, -1, 50);
  int aTool = createTool(theWBName);

  int aId = myActionsList.size();
  myActionsList.append(theId);
  SUIT_Desktop* aDesk = application()->desktop();
  int aKeys = 0;
  for (unsigned int i = 0; i < theKeys.count(); i++)
    aKeys += theKeys[i];
  QAction* aAction = createAction(aId, theTip, theIcon, theTitle, theTip, aKeys, aDesk,
                                  isCheckable);
  aAction->setData(theId);
  int aItemId = createMenu(aId, aMenu, -1, 10);
  int aToolId = createTool(aId, aTool);
  return aAction;
}

//******************************************************
QAction* NewGeom_Module::addEditCommand(const QString& theId, const QString& theTitle,
                                        const QString& theTip, const QIcon& theIcon,
                                        const QKeySequence& theKeys, bool isCheckable)
{
  int aMenu = createMenu(tr("MEN_DESK_EDIT"), -1, -1);

  int aId = myActionsList.size();
  myActionsList.append(theId);
  SUIT_Desktop* aDesk = application()->desktop();
  int aKeys = 0;
  for (unsigned int i = 0; i < theKeys.count(); i++)
    aKeys += theKeys[i];
  QAction* aAction = createAction(aId, theTip, theIcon, theTitle, theTip, aKeys, aDesk,
                                  isCheckable);
  aAction->setData(theId);
  createMenu(aId, aMenu, 10);
  return aAction;
}

//******************************************************
void NewGeom_Module::addEditMenuSeparator()
{
  int aMenu = createMenu(tr("MEN_DESK_EDIT"), -1, -1);
  createMenu(separator(), aMenu, -1, 10);
}

//******************************************************
QList<QAction*> NewGeom_Module::commandList() const
{
  QList<QAction*> aActions;
  for (int i = 0; i < myActionsList.size(); i++)
    aActions.append(action(i));
  return aActions;
}

//******************************************************
QStringList NewGeom_Module::commandIdList() const
{
  return myActionsList;
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

//******************************************************
void NewGeom_Module::contextMenuPopup(const QString& theClient, QMenu* theMenu, QString& theTitle)
{
  myWorkshop->contextMenuMgr()->addViewerItems(theMenu);
  LightApp_Module::contextMenuPopup(theClient, theMenu, theTitle);
}


//******************************************************
void NewGeom_Module::createPreferences()
{
  XGUI_Preferences::updateCustomProps();
  LightApp_Preferences* pref = preferences();
  QString aModName = moduleName();

  QtxPreferenceItem* item = pref->findItem(aModName, true );
  if ( item && (!item->isEmpty() )) {
    item->parentItem()->removeItem(item);
    delete item;
  }

  int catId = pref->addPreference(aModName, -1 );
  if ( catId == -1 )
    return;
  NewGeom_PrefMgr aMgr(pref, aModName);
  XGUI_Preferences::createEditContent(&aMgr, catId);
  pref->retrieve();
}

//******************************************************
void NewGeom_Module::preferencesChanged(const QString& theSection, const QString& theParam)
{
  SUIT_ResourceMgr* aResMgr = application()->resourceMgr();
  QString aVal = aResMgr->stringValue(theSection, theParam);
  if (!aVal.isNull()) {
    Config_Prop* aProp = Config_PropManager::findProp(theSection.toStdString(), theParam.toStdString());
    aProp->setValue(aVal.toStdString());
  }
}
