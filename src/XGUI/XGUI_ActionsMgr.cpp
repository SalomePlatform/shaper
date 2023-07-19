// Copyright (C) 2014-2023  CEA, EDF
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

#ifndef HAVE_SALOME
#include <AppElements_Command.h>
#endif

#include <XGUI_ActionsMgr.h>
#include <XGUI_Workshop.h>
#include <XGUI_OperationMgr.h>
#include <XGUI_SalomeConnector.h>
#include <XGUI_Selection.h>
#include <XGUI_SelectionMgr.h>

#include <Events_Loop.h>
#include <Events_InfoMessage.h>

#include <ModelAPI_Session.h>
#include <ModelAPI_Events.h>
#include <ModelAPI_Validator.h>
#include <ModuleBase_Operation.h>
#include <ModuleBase_OperationFeature.h>
#include <ModuleBase_SelectionValidator.h>
#include <ModuleBase_Tools.h>

#include <QAction>
#include <QMainWindow>

#ifdef WIN32
#pragma warning(disable : 4456) // for nested foreach
#endif

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

  //Initialize event listening
  Events_Loop* aLoop = Events_Loop::loop();
  static Events_ID aStateResponseEventId =
      Events_Loop::loop()->eventByName(EVENT_FEATURE_STATE_RESPONSE);
  aLoop->registerListener(this, aStateResponseEventId, NULL, true);
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
#ifdef HAVE_SALOME
    XGUI_Workshop* aWorkshop = static_cast<XGUI_Workshop*>(parent());
    const std::shared_ptr<Config_FeatureMessage>& anInfo =
                         aWorkshop->salomeConnector()->featureInfo(aId);
    if (anInfo.get())
      myNestedActions[aId] = QString::fromStdString(anInfo->nestedFeatures())
                                   .split(" ", QString::SkipEmptyParts);
#else
  AppElements_Command* aXCmd = dynamic_cast<AppElements_Command*>(theCmd);
  myNestedActions[aId] = aXCmd->nestedCommands();
#endif
}

void XGUI_ActionsMgr::addNestedCommands(const QString& theId, const QStringList& theCommands)
{
  myNestedActions[theId] = theCommands;
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

void XGUI_ActionsMgr::updateCommandsStatus()
{
  setAllEnabled();
  XGUI_Selection* aSelection = myWorkshop->selector()->selection();
  if (aSelection->getSelected(ModuleBase_ISelection::AllControls).size() > 0)
    updateOnViewSelection();

  FeaturePtr anActiveFeature = FeaturePtr();
  ModuleBase_OperationFeature* aFOperation = dynamic_cast<ModuleBase_OperationFeature*>
                                                         (myOperationMgr->currentOperation());
  if (aFOperation) {
    anActiveFeature = aFOperation->feature();
    QStringList aNested = allNestedCommands(aFOperation);
    foreach(QString aAction, myActions.keys()) {
      if (!aNested.contains(aAction))
        setActionEnabled(aAction, false);
    }
  } else
    setNestedCommandsEnabled(false);

  updateByPlugins(anActiveFeature);
  updateByDocumentKind();
  updateCheckState();
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

void XGUI_ActionsMgr::updateOnViewSelection()
{
  if (!myOperationMgr->hasOperation())
    return;

  QStringList aIdList = myOperationMgr->operationList();
  //ModuleBase_Operation* anOperation = myOperationMgr->currentOperation();
  //FeaturePtr anActiveFeature = anOperation->feature();
  //if(!anActiveFeature.get())
  if (aIdList.isEmpty())
    return;

  ModuleBase_Operation* theOperation = myOperationMgr->currentOperation();
  //QString aFeatureId = QString::fromStdString(anActiveFeature->getKind());
  XGUI_Selection* aSelection = myWorkshop->selector()->selection();
  // only viewer selection is processed
  SessionPtr aMgr = ModelAPI_Session::get();
  ModelAPI_ValidatorsFactory* aFactory = aMgr->validators();
  foreach(QString aFeatureId, aIdList) {
    foreach(QString aId, nestedCommands(aFeatureId)) {
      ModelAPI_ValidatorsFactory::Validators aValidators;
      aFactory->validators(aId.toStdString(), aValidators);
      ModelAPI_ValidatorsFactory::Validators::iterator aValidatorIt = aValidators.begin();
      for (; aValidatorIt != aValidators.end(); ++aValidatorIt) {
        const ModuleBase_SelectionValidator* aSelValidator =
            dynamic_cast<const ModuleBase_SelectionValidator*>
            (aFactory->validator(aValidatorIt->first));
        if (aSelValidator)
          setActionEnabled(aId, aSelValidator->isValid(aSelection, theOperation));
      }
    }
  }
}

QKeySequence XGUI_ActionsMgr::registerShortcut(const QKeySequence& theKeySequence)
{
  if (theKeySequence.isEmpty()) {
    return QKeySequence();
  }
  if (myShortcuts.contains(theKeySequence)) {
    QString aMessage = tr("Shortcut %1 is already defined. Ignore.");
    aMessage = aMessage.arg(theKeySequence.toString());
    Events_InfoMessage("XGUI_ActionsMgr", aMessage.toStdString()).send();
    return QKeySequence();
  }
  myShortcuts.append(theKeySequence);
  return theKeySequence;
}

QKeySequence XGUI_ActionsMgr::registerShortcut(const QString& theKeySequence)
{
  if (theKeySequence.isEmpty()) {
    return QKeySequence();
  }
  QKeySequence aResult(theKeySequence);
  registerShortcut(aResult);
  return aResult;
}

void XGUI_ActionsMgr::processEvent(const std::shared_ptr<Events_Message>& theMessage)
{
  const Events_ID kResponseEvent =
      Events_Loop::loop()->eventByName(EVENT_FEATURE_STATE_RESPONSE);
  if (theMessage->eventID() == kResponseEvent) {
    std::shared_ptr<ModelAPI_FeatureStateMessage> aStateMessage =
        std::dynamic_pointer_cast<ModelAPI_FeatureStateMessage>(theMessage);
    if (!aStateMessage.get())
      return;
    std::list<std::string> aFeaturesList = aStateMessage->features();
    std::list<std::string>::iterator it = aFeaturesList.begin();
    for( ; it != aFeaturesList.end(); ++it) {
      QString anActionId = QString::fromStdString(*it);
      bool theDefaultState = false;
      if (myActions.contains(anActionId)) {
        theDefaultState = myActions[anActionId]->isEnabled();
      }
      setActionEnabled(anActionId, aStateMessage->state(*it, theDefaultState));
    }
  } else if (theMessage.get()) {
    #ifdef _DEBUG
    std::cout << "XGUI_ActionsMgr::processEvent: unhandled message caught: " << std::endl
              << theMessage->eventID().eventText() << std::endl;
    #endif
  }
}

QAction* XGUI_ActionsMgr::operationStateAction(OperationStateActionId theId)
{
  QAction* aResult = NULL;
  if (myOperationActions.contains(theId)) {
    aResult = myOperationActions.value(theId);
    //if (theParent && aResult->parent() != theParent) {
    //  aResult->setParent(theParent);
    //}
  } else {
    QWidget* aParent = myWorkshop->desktop();
    switch (theId) {
      case Accept:
      case AcceptAll: {
        aResult = ModuleBase_Tools::createAction(QIcon(":pictures/button_ok.png"),
                            tr("Apply"), aParent);
      }
      break;
      case AcceptPlus: {
        aResult = ModuleBase_Tools::createAction(QIcon(":pictures/button_ok-plus.png"),
                            tr("Apply and continue"), aParent);
      }
      break;
      case Abort:
      case AbortAll: {
        aResult = ModuleBase_Tools::createAction(QIcon(":pictures/button_cancel.png"), tr("Cancel"),
                                                 aParent);
      }
      break;
      case Help: {
        aResult = ModuleBase_Tools::createAction(QIcon(":pictures/button_help.png"), tr("Help"),
                                                 aParent);
      }
      break;
      case Preview: {
        aResult = ModuleBase_Tools::createAction(QIcon(), tr("See preview"),
                                                 aParent, 0, 0, tr("Compute preview"));
        aResult->setStatusTip(aResult->toolTip());
      }
      break;
      default:
        break;
    }
    myOperationActions.insert(theId, aResult);
  }
  return aResult;
}

QAction* XGUI_ActionsMgr::action(const QString& theId)
{
  QAction* anAction = 0;
  if(myActions.contains(theId)) {
    anAction = myActions.value(theId);
  }
  return anAction;
}

ActionInfo XGUI_ActionsMgr::actionInfoById(const QString& theId)
{
  ActionInfo aResult;
  if(myActions.contains(theId)) {
    aResult.initFrom(myActions.value(theId));
  } else {
   aResult.id = theId;
   aResult.text = theId;
  }
  return aResult;
}

void XGUI_ActionsMgr::setAllEnabled()
{
  foreach(QString eachAction, myActions.keys()) {
    if (myActions.contains(eachAction)) {
      QAction* aAction = myActions[eachAction];
      aAction->setEnabled(true);
    }
  }
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

void XGUI_ActionsMgr::setNestedStackEnabled(ModuleBase_Operation* theOperation)
{
  ModuleBase_OperationFeature* anOperation =
    dynamic_cast<ModuleBase_OperationFeature*>(theOperation);
  if(!anOperation || !anOperation->feature())
    return;
  FeaturePtr aFeature = anOperation->feature();
  QString aFeatureId = QString::fromStdString(aFeature->getKind());
  //setActionEnabled(aFeatureId, true);
  setNestedCommandsEnabled(true, aFeatureId);

  setNestedStackEnabled(myOperationMgr->previousOperation(theOperation));
}

QStringList XGUI_ActionsMgr::allNestedCommands(ModuleBase_Operation* theOperation)
{
  QStringList aFeatures;
  ModuleBase_OperationFeature* anOperation =
    dynamic_cast<ModuleBase_OperationFeature*>(theOperation);
  if(!anOperation || !anOperation->feature())
    return aFeatures;
  FeaturePtr aFeature = anOperation->feature();
  QString aFeatureId = QString::fromStdString(aFeature->getKind());

  aFeatures << myNestedActions[aFeatureId];
  aFeatures << allNestedCommands(myOperationMgr->previousOperation(theOperation));
  return aFeatures;
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

void XGUI_ActionsMgr::setActionEnabled(const QString& theId, const bool theEnabled)
{
  if (myActions.contains(theId)) {
    QAction* aAction = myActions[theId];
    // Initially all actions are enabled
    // If it was disabled for any reason then we can not enable it
    if (aAction->isEnabled())
      aAction->setEnabled(theEnabled);
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
#ifdef HAVE_SALOME
  XGUI_Workshop* aWorkshop = static_cast<XGUI_Workshop*>(parent());
#endif
  foreach(QAction* eachAction, myActions.values()) {
    QString aCmdDocKind;
#ifdef HAVE_SALOME
    QString aId = eachAction->data().toString();
    if (!aId.isEmpty()) {
      aCmdDocKind = QString::fromStdString(
                 aWorkshop->salomeConnector()->featureInfo(aId)->documentKind());
    }
#else
    AppElements_Command* aCmd = dynamic_cast<AppElements_Command*>(eachAction);
    aCmdDocKind = QString::fromStdString(aCmd->featureMessage()->documentKind());
#endif
    if(!aCmdDocKind.isEmpty() && aCmdDocKind != aDocKind) {
      eachAction->setEnabled(false);
    }
  }
}

void XGUI_ActionsMgr::updateByPlugins(FeaturePtr anActiveFeature)
{
  static Events_ID aStateRequestEventId = Events_Loop::loop()->eventByName(
      EVENT_FEATURE_STATE_REQUEST);
  std::shared_ptr<ModelAPI_FeatureStateMessage> aMsg(
      new ModelAPI_FeatureStateMessage(aStateRequestEventId, this));
  aMsg->setFeature(anActiveFeature);
  Events_Loop::loop()->send(aMsg, false);
}
