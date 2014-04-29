/*
 * XGUI_ActionsMgr.cpp
 */

#include <XGUI_ActionsMgr.h>
#include <XGUI_Command.h>
#include <QAction>


XGUI_ActionsMgr::XGUI_ActionsMgr(QObject* theParent)
 : QObject(theParent)
{

}

XGUI_ActionsMgr::~XGUI_ActionsMgr()
{
}


void XGUI_ActionsMgr::addCommand(XGUI_Command* theCmd)
{
  myActions.insert(theCmd->id(),theCmd);
  myActionsState.insert(theCmd->id(), theCmd->enabled());
  theCmd->connectTo(this, SLOT(setActionsDisabled(bool)));
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
  XGUI_Command* aToggledFeature = dynamic_cast<XGUI_Command*>(sender());
  QString aSkippedId = aToggledFeature->id();
  QStringList anActionIdsList = myActions.keys();
  foreach(QString eachKey, anActionIdsList) {
    if (eachKey == aSkippedId) {
      continue;
    }
    myActions[eachKey]->setEnabled(false);
  }
  myNestedActions = aToggledFeature->unblockableCommands();
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

void XGUI_ActionsMgr::setNestedActionsEnabled(bool isEnabled)
{
  foreach(QString eachKey, myNestedActions) {
    myActions[eachKey]->setEnabled(isEnabled);
  }
}
