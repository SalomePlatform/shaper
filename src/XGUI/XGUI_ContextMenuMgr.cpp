
#include "XGUI_ContextMenuMgr.h"
#include "XGUI_Workshop.h"
#include "XGUI_ObjectsBrowser.h"
#include "XGUI_SelectionMgr.h"

#include <ModelAPI_Data.h>
#include <ModelAPI_AttributeDocRef.h>

#include <QAction>
#include <QContextMenuEvent>
#include <QMenu>

XGUI_ContextMenuMgr::XGUI_ContextMenuMgr(XGUI_Workshop* theParent) :
QObject(theParent), myWorkshop(theParent)
{

}

XGUI_ContextMenuMgr::~XGUI_ContextMenuMgr()
{
}

void XGUI_ContextMenuMgr::createActions()
{
  QAction* aAction = new QAction(QIcon(":pictures/edit.png"), tr("Edit..."), this);
  addAction("EDIT_CMD", aAction);

  aAction = new QAction(QIcon(":pictures/activate.png"), tr("Activate"), this);
  addAction("ACTIVATE_PART_CMD", aAction);

  aAction = new QAction(QIcon(":pictures/assembly.png"), tr("Deactivate"), this);
  addAction("DEACTIVATE_PART_CMD", aAction);
}

void XGUI_ContextMenuMgr::addAction(const QString& theId, QAction* theAction)
{
  if (myActions.contains(theId))
    qCritical("A command with Id = '%s' already defined!", qPrintable(theId));
  theAction->setData(theId);
  connect(theAction, SIGNAL(triggered(bool)), this, SLOT(onAction(bool)));
  myActions[theId] = theAction;
}

QAction* XGUI_ContextMenuMgr::action(const QString& theId) const
{
  if (myActions.contains(theId))
    return myActions[theId];
  return 0;
}

QStringList XGUI_ContextMenuMgr::actionIds() const
{
  return myActions.keys();
}

void XGUI_ContextMenuMgr::onAction(bool isChecked)
{
  QAction* aAction = static_cast<QAction*>(sender());
  emit actionTriggered(aAction->data().toString(), isChecked);
}

void XGUI_ContextMenuMgr::updateCommandsStatus()
{
}

void XGUI_ContextMenuMgr::onContextMenuRequest(QContextMenuEvent* theEvent)
{
  QMenu* aMenu = 0;
  if (sender() == myWorkshop->objectBrowser())
    aMenu = objectBrowserMenu();

  if (aMenu) {
    aMenu->exec(theEvent->globalPos());
    delete aMenu;
  }
}

QMenu* XGUI_ContextMenuMgr::objectBrowserMenu() const
{
  QList<QAction*> aActions;
  XGUI_SelectionMgr* aSelMgr = myWorkshop->selector();
  QFeatureList aFeatures = aSelMgr->selectedFeatures();
  if (aFeatures.size() == 1) {
    PluginManagerPtr aMgr = ModelAPI_PluginManager::get();
    FeaturePtr aFeature = aFeatures.first();
    //Process Feature
    if (aFeature) {
      if (aFeature->getKind() == "Part") {
        boost::shared_ptr<ModelAPI_Document> aFeaDoc = aFeature->data()->docRef("PartDocument")->value();
        if (aMgr->currentDocument() == aFeaDoc)
          aActions.append(action("DEACTIVATE_PART_CMD"));
        else 
          aActions.append(action("ACTIVATE_PART_CMD"));
      } else {
        aActions.append(action("EDIT_CMD"));
      }

    // Process Root object (document)
    } else { // If feature is 0 the it means that selected root object (document)
      if (aMgr->currentDocument() != aMgr->rootDocument()) {
        aActions.append(action("ACTIVATE_PART_CMD"));
      }
    }
  }
  aActions.append(myWorkshop->objectBrowser()->actions());
  if (aActions.size() > 0) {
    QMenu* aMenu = new QMenu();
    aMenu->addActions(aActions);
    return aMenu;
  }
  return 0;
}

void XGUI_ContextMenuMgr::connectObjectBrowser() const
{
  connect(myWorkshop->objectBrowser(), SIGNAL(contextMenuRequested(QContextMenuEvent*)), 
    this, SLOT(onContextMenuRequest(QContextMenuEvent*)));
}