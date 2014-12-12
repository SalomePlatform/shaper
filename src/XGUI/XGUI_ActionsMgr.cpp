// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

/*
 * XGUI_ActionsMgr.cpp
 */

#include <AppElements_Command.h>

#include <XGUI_ActionsMgr.h>
#include <XGUI_Workshop.h>
#include <XGUI_OperationMgr.h>
#include <XGUI_SalomeConnector.h>
#include <XGUI_Selection.h>
#include <XGUI_SelectionMgr.h>

#include <Events_Loop.h>
#include <Events_Error.h>

#include <ModelAPI_Session.h>
#include <ModelAPI_Events.h>
#include <ModelAPI_Validator.h>
#include <ModuleBase_Operation.h>
#include <ModuleBase_SelectionValidator.h>


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

void XGUI_ActionsMgr::update()
{
  FeaturePtr anActiveFeature = FeaturePtr();
  if (myOperationMgr->hasOperation()) {
    ModuleBase_Operation* anOperation = myOperationMgr->currentOperation();
    anActiveFeature = anOperation->feature();
    if(anActiveFeature.get()) {
      setAllEnabled(false);
      QString aFeatureId = QString::fromStdString(anActiveFeature->getKind());
      setActionEnabled(aFeatureId, true);
    }
    setNestedStackEnabled(anOperation);
  } else {
    setAllEnabled(true);
    setNestedCommandsEnabled(false);
  }
  // TODO(SBH): Get defaults state of actions from XML and remove the following method
  updateByDocumentKind();
  updateCheckState();
  updateByPlugins(anActiveFeature);
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
  XGUI_Selection* aSelection = myWorkshop->selector()->selection();
  if (aSelection->getSelected().size() == 0 || !myOperationMgr->hasOperation())
    return;
  ModuleBase_Operation* anOperation = myOperationMgr->currentOperation();
  FeaturePtr anActiveFeature = anOperation->feature();
  if(!anActiveFeature.get())
    return;
  QString aFeatureId = QString::fromStdString(anActiveFeature->getKind());

  SessionPtr aMgr = ModelAPI_Session::get();
  ModelAPI_ValidatorsFactory* aFactory = aMgr->validators();
  foreach(QString aId, nestedCommands(aFeatureId)) {
    std::list<ModelAPI_Validator*> aValidators;
    std::list<std::list<std::string> > anArguments;
    if (!anArguments.empty()) {
      std::list<std::string> firstArg = anArguments.front();
    }
    aFactory->validators(aId.toStdString(), aValidators, anArguments);
    std::list<ModelAPI_Validator*>::iterator aValidator = aValidators.begin();
    std::list<std::list<std::string> >::iterator aValidatorArgs = anArguments.begin();
    for (; aValidator != aValidators.end(); aValidator++, aValidatorArgs++) {
      if (!(*aValidator))
        continue;
      const ModuleBase_SelectionValidator* aSelValidator =
          dynamic_cast<const ModuleBase_SelectionValidator*>(*aValidator);
      if (!aSelValidator)
        continue;
      setActionEnabled(aId, aSelValidator->isValid(aSelection, *aValidatorArgs));

    }
  }
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
  if(!theOperation || !theOperation->feature())
    return;
  FeaturePtr aFeature = theOperation->feature();
  QString aFeatureId = QString::fromStdString(aFeature->getKind());
  setNestedCommandsEnabled(true, aFeatureId);

  setNestedStackEnabled(myOperationMgr->previousOperation(theOperation));
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
    myActions[theId]->setEnabled(theEnabled);
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

void XGUI_ActionsMgr::updateByPlugins(FeaturePtr anActiveFeature)
{
  static Events_ID aStateRequestEventId = Events_Loop::loop()->eventByName(
      EVENT_FEATURE_STATE_REQUEST);
  std::shared_ptr<ModelAPI_FeatureStateMessage> aMsg =
      std::make_shared<ModelAPI_FeatureStateMessage>(aStateRequestEventId, this);
  aMsg->setFeature(anActiveFeature);
  Events_Loop::loop()->send(aMsg, false);
}
