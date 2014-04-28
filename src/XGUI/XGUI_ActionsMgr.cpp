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
    restoreCommandState();
    return;
  }
  //Disable all actions, but caller and unblockable (defined in a xml)
  saveCommandsState();
  QStringList aSkippedIds;
  XGUI_Command* aToggledFeature = dynamic_cast<XGUI_Command*>(sender());
  aSkippedIds.append(aToggledFeature->unblockableCommands());
  aSkippedIds.append(aToggledFeature->id());
  QStringList anActionIdsList = myActions.keys();
  foreach(QString eachKey, anActionIdsList) {
    if (aSkippedIds.removeAll(eachKey) > 0) {
      continue;
    }
    myActions[eachKey]->setEnabled(false);
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
