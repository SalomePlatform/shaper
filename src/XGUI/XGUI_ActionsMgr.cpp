/*
 * XGUI_ActionsMgr.cpp
 */

#include "XGUI_ActionsMgr.h"
#include "XGUI_Command.h"
#include "XGUI_Workshop.h"
#include "XGUI_OperationMgr.h"
#include "XGUI_SalomeConnector.h"

#include <ModuleBase_Operation.h>
#include <Events_Error.h>

#include <QAction>

#ifdef _DEBUG
#include <iostream>
#include <QDebug>
#endif

XGUI_ActionsMgr::XGUI_ActionsMgr(XGUI_Workshop* theParent)
    : QObject(theParent),
      myWorkshop(theParent),
      myOperationMgr(theParent->operationMgr())
{
  // Default shortcuts
  myShortcuts << QKeySequence::Save;
  myShortcuts << QKeySequence::Undo;
  myShortcuts << QKeySequence::Redo;
  myShortcuts << QKeySequence::Open;
  myShortcuts << QKeySequence::Close;
}

XGUI_ActionsMgr::~XGUI_ActionsMgr()
{
}

void XGUI_ActionsMgr::addCommand(QAction* theCmd)
{
  QString aId = theCmd->data().toString();
  if (aId.isEmpty()) {
    return;
  }
  myActions.insert(aId, theCmd);
  XGUI_Command* aXCmd = dynamic_cast<XGUI_Command*>(theCmd);
  if (aXCmd) {
    myNestedActions[aId] = aXCmd->nestedCommands();
  } else {
    XGUI_Workshop* aWorkshop = static_cast<XGUI_Workshop*>(parent());
    myNestedActions[aId] = aWorkshop->salomeConnector()->nestedActions(aId);
  }
}

void XGUI_ActionsMgr::addNestedCommands(const QString& theId, const QStringList& theCommands)
{
  myNestedActions[theId] = theCommands;
}

void XGUI_ActionsMgr::update()
{
  if (myOperationMgr->hasOperation()) {
    setAllEnabled(false);
    ModuleBase_Operation* anOperation = myOperationMgr->currentOperation();
    QString anOperationId = anOperation->id();
    setActionEnabled(anOperationId, true);
    bool isNestedEnabled = anOperation->isNestedOperationsEnabled();
    setNestedCommandsEnabled(isNestedEnabled, anOperationId);
  } else {
    setAllEnabled(true);
    setNestedCommandsEnabled(false);
  }
  updateCheckState();
}

void XGUI_ActionsMgr::setAllEnabled(bool isEnabled)
{
  foreach(QString eachAction, myActions.keys())
  {
    setActionEnabled(eachAction, isEnabled);
  }
}

//!
void XGUI_ActionsMgr::setNestedCommandsEnabled(bool theEnabled, const QString& theParent)
{
  QStringList ltNestedActions;
  if (theParent.isEmpty()) {  //Disable ALL nested
    foreach(QString eachParent, myNestedActions.keys())
    {
      ltNestedActions << myNestedActions[eachParent];
    }
  } else {
    ltNestedActions << myNestedActions[theParent];
  }
  foreach(QString eachNested, ltNestedActions)
  {
    setActionEnabled(eachNested, theEnabled);
  }
}

void XGUI_ActionsMgr::setActionChecked(const QString& theId, const bool theChecked)
{
  QAction* anAction = myActions[theId];
  if (anAction && anAction->isCheckable()) {
    anAction->setChecked(theChecked);
  }
}

void XGUI_ActionsMgr::setActionEnabled(const QString& theId, const bool theEnabled)
{
  QAction* anAction = myActions[theId];
  if (anAction) {
    anAction->setEnabled(theEnabled);
  }
}

void XGUI_ActionsMgr::updateCheckState()
{
  QString eachCommand = QString();
  foreach(eachCommand, myActions.keys())
  {
    setActionChecked(eachCommand, false);
  }
  QStringList ltActiveCommands = myOperationMgr->operationList();
  foreach(eachCommand, ltActiveCommands)
  {
    setActionChecked(eachCommand, true);
  }
}

QStringList XGUI_ActionsMgr::nestedCommands(const QString& theId) const
{
  if (myNestedActions.contains(theId))
    return myNestedActions[theId];
  return QStringList();
}

bool XGUI_ActionsMgr::isNested(const QString& theId) const
{
  foreach(QString aId, myNestedActions.keys())
  {
    QStringList aList = myNestedActions[aId];
    if (aList.contains(theId))
      return true;
  }
  return false;
}

QKeySequence XGUI_ActionsMgr::registerShortcut(const QString& theKeySequence)
{
  if (theKeySequence.isEmpty()) {
    return QKeySequence();
  }
  QKeySequence aResult(theKeySequence);
  if (myShortcuts.contains(aResult)) {
    QString aMessage = tr("Shortcut %1 is already defined. Ignore.").arg(theKeySequence);
    Events_Error::send(aMessage.toStdString());
    return QKeySequence();
  }
  myShortcuts.append(aResult);
  return aResult;
}
