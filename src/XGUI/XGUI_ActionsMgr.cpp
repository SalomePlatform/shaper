/*
 * XGUI_ActionsMgr.cpp
 */

#include "XGUI_ActionsMgr.h"
#include "XGUI_Command.h"
#include "XGUI_Workshop.h"
#include "XGUI_SalomeConnector.h"

#include <QAction>


XGUI_ActionsMgr::XGUI_ActionsMgr(XGUI_Workshop* theParent)
 : QObject(theParent), myWorkshop(theParent)
{
  
}

XGUI_ActionsMgr::~XGUI_ActionsMgr()
{
}


void XGUI_ActionsMgr::addCommand(QAction* theCmd)
{
  QString aId = theCmd->data().toString();
  myActions.insert(aId, theCmd);
  myActionsState.insert(aId, theCmd->isEnabled());
  connect(theCmd, SIGNAL(triggered(bool)), this, SLOT(setActionsDisabled(bool)));
}

void XGUI_ActionsMgr::setActionsDisabled(bool isDisabled)
{
  //Re-enable actions (just restore their state)
  if (!isDisabled) {
    myNestedActions.clear();
    restoreCommandState();
    return;
  }
  //Disable all actions, but caller and unblockable (defined in a xml)
  saveCommandsState();

  QString aSkippedId;
  QAction* aToggledFeature = dynamic_cast<QAction*>(sender());
  aSkippedId = aToggledFeature->data().toString();

  QStringList anActionIdsList = myActions.keys();
  foreach(QString eachKey, anActionIdsList) {
    if (eachKey == aSkippedId) {
      continue;
    }
    myActions[eachKey]->setEnabled(false);
  }
  if (myWorkshop->isSalomeMode()) {
    myNestedActions = myWorkshop->salomeConnector()->nestedActions(aSkippedId);
  } else {
    XGUI_Command* aToggledFeature = dynamic_cast<XGUI_Command*>(sender());
    myNestedActions = aToggledFeature->unblockableCommands();
  }
}

void XGUI_ActionsMgr::saveCommandsState()
{
  myActionsState.clear();
  QStringList anActionIdsList = myActions.keys();
  foreach(QString eachKey, anActionIdsList) {
    myActionsState.insert(eachKey, myActions[eachKey]->isEnabled());
  }

}

void XGUI_ActionsMgr::restoreCommandState()
{
  QStringList anActionIdsList = myActions.keys();
  foreach(QString eachKey, anActionIdsList) {
    myActions[eachKey]->setEnabled(myActionsState[eachKey]);
    myActions[eachKey]->setChecked(false);
  }
}

void XGUI_ActionsMgr::setActionChecked(const QString& theId, const bool theChecked)
{
  if(myActions.contains(theId)) {
    myActions[theId]->setChecked(theChecked);
  }
}

void XGUI_ActionsMgr::updateAction(const QString& theId)
{
  if(myActions.contains(theId)){
    myActions[theId]->setEnabled(myActionsState[theId]);
    myActions[theId]->setChecked(false);
  }
}

void XGUI_ActionsMgr::setNestedActionsEnabled(bool isEnabled)
{
  foreach(QString eachKey, myNestedActions) {
    if (myActions.contains(eachKey))
      myActions[eachKey]->setEnabled(isEnabled);
  }
}
