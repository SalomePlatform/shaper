/*
 * XGUI_ActionsMgr.cpp
 */

#include "XGUI_ActionsMgr.h"
#include "XGUI_Workshop.h"
#include "XGUI_OperationMgr.h"
#include "XGUI_SalomeConnector.h"

#include <AppElements_Command.h>

#include <ModelAPI_Session.h>

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
  AppElements_Command* aXCmd = dynamic_cast<AppElements_Command*>(theCmd);
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
    ModuleBase_Operation* anOperation = myOperationMgr->currentOperation();
    FeaturePtr aFeature = anOperation->feature();
    if(aFeature) {
      setAllEnabled(false);
      QString aFeatureId = QString::fromStdString(aFeature->getKind());
      setActionEnabled(aFeatureId, true);
      setNestedStackEnabled(anOperation);
    }
  } else {
    setAllEnabled(true);
    setNestedCommandsEnabled(false);
  }
  updateByDocumentKind();
  updateCheckState();
}

void XGUI_ActionsMgr::setAllEnabled(bool isEnabled)
{
  foreach(QString eachAction, myActions.keys())
  {
    setActionEnabled(eachAction, isEnabled);
  }
}

void XGUI_ActionsMgr::setNestedStackEnabled(ModuleBase_Operation* theOperation)
{
  if(!theOperation || !theOperation->feature())
    return;
  FeaturePtr aFeature = theOperation->feature();
  QString aFeatureId = QString::fromStdString(aFeature->getKind());
  bool isNestedEnabled = theOperation->isNestedOperationsEnabled();
  setNestedCommandsEnabled(isNestedEnabled, aFeatureId);

  setNestedStackEnabled(myOperationMgr->previousOperation(theOperation));
}

//!
void XGUI_ActionsMgr::setNestedCommandsEnabled(bool theEnabled, const QString& theParent)
{
  QStringList ltNestedActions;
  if (theParent.isEmpty()) {  //Disable ALL nested
    foreach(QString eachParent, myNestedActions.keys()) {
      ltNestedActions << myNestedActions[eachParent];
    }
  } else {
    ltNestedActions << myNestedActions[theParent];
  }
  foreach(QString eachNested, ltNestedActions) {
    setActionEnabled(eachNested, theEnabled);
  }
}

void XGUI_ActionsMgr::setActionChecked(const QString& theId, const bool theChecked)
{
  if (myActions.contains(theId)) {
    QAction* anAction = myActions[theId];
    if (anAction->isCheckable()) {
      anAction->setChecked(theChecked);
    }
  }
}

/*
 * Disables all actions which have the Document Kind different to
 * the current document's kind
 */
void XGUI_ActionsMgr::updateByDocumentKind()
{
  std::string aStdDocKind = ModelAPI_Session::get()->activeDocument()->kind();
  QString aDocKind = QString::fromStdString(aStdDocKind);
  XGUI_Workshop* aWorkshop = static_cast<XGUI_Workshop*>(parent());
  foreach(QAction* eachAction, myActions.values()) {
    AppElements_Command* aCmd = dynamic_cast<AppElements_Command*>(eachAction);
    QString aCmdDocKind;
    if(aCmd) {
      aCmdDocKind = aCmd->documentKind();
    } else {
      QString aId = eachAction->data().toString();
      if (!aId.isEmpty()) {
        aCmdDocKind = aWorkshop->salomeConnector()->documentKind(aId);
      }
    }
    if(!aCmdDocKind.isEmpty() && aCmdDocKind != aDocKind) {
      eachAction->setEnabled(false);
    }
  }
}

void XGUI_ActionsMgr::setActionEnabled(const QString& theId, const bool theEnabled)
{
  if (myActions.contains(theId)) {
    myActions[theId]->setEnabled(theEnabled);
  }
}

void XGUI_ActionsMgr::updateCheckState()
{
  QString eachCommand = QString();
  foreach(eachCommand, myActions.keys()) {
    setActionChecked(eachCommand, false);
  }
  QStringList ltActiveCommands = myOperationMgr->operationList();
  foreach(eachCommand, ltActiveCommands) {
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
