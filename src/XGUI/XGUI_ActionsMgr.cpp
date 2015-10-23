// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

/*
 * XGUI_ActionsMgr.cpp
 */

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
#include <Events_Error.h>

#include <ModelAPI_Session.h>
#include <ModelAPI_Events.h>
#include <ModelAPI_Validator.h>
#include <ModuleBase_Operation.h>
#include <ModuleBase_OperationFeature.h>
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
#ifdef HAVE_SALOME
    XGUI_Workshop* aWorkshop = static_cast<XGUI_Workshop*>(parent());
    myNestedActions[aId] = aWorkshop->salomeConnector()->nestedActions(aId);
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

void XGUI_ActionsMgr::update()
{
  XGUI_Selection* aSelection = myWorkshop->selector()->selection();
  if (aSelection->getSelected(ModuleBase_ISelection::Viewer).size() > 0) {
    updateOnViewSelection();
  } else {
    FeaturePtr anActiveFeature = FeaturePtr();
    ModuleBase_OperationFeature* aFOperation = dynamic_cast<ModuleBase_OperationFeature*>
                                                           (myOperationMgr->currentOperation());
    if (aFOperation) {
      anActiveFeature = aFOperation->feature();
      if(anActiveFeature.get()) {
        setAllEnabled(false);
        QString aFeatureId = QString::fromStdString(anActiveFeature->getKind());
        setActionEnabled(aFeatureId, true);
      }
      setNestedStackEnabled(aFOperation);
    } else {
      setAllEnabled(true);
      setNestedCommandsEnabled(false);
    }
    // TODO(SBH): Get defaults state of actions from XML and remove the following method
    updateByDocumentKind();
    updateCheckState();
    updateByPlugins(anActiveFeature);
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
            dynamic_cast<const ModuleBase_SelectionValidator*>(aFactory->validator(aValidatorIt->first));
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
    Events_Error::send(aMessage.toStdString());
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

QAction* XGUI_ActionsMgr::operationStateAction(OperationStateActionId theId, QObject* theParent)
{
  QAction* aResult = NULL;
  if (myOperationActions.contains(theId)) {
    aResult = myOperationActions.value(theId);
    if (theParent && aResult->parent() != theParent) {
      aResult->setParent(theParent);
    }
  } else {
    switch (theId) {
      case Accept:
      case AcceptAll:
        aResult = new QAction(QIcon(":pictures/button_ok.png"), "", theParent);
        break;
      case Abort:
      case AbortAll: {
        aResult = new QAction(QIcon(":pictures/button_cancel.png"), "", theParent);
        if(theId == Abort) {
          aResult->setShortcut(QKeySequence(Qt::Key_Escape));
        }
      }
      break;
      case Help:
        aResult = new QAction(QIcon(":pictures/button_help.png"), "", theParent);
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
  ModuleBase_OperationFeature* anOperation = dynamic_cast<ModuleBase_OperationFeature*>(theOperation);
  if(!anOperation || !anOperation->feature())
    return;
  FeaturePtr aFeature = anOperation->feature();
  QString aFeatureId = QString::fromStdString(aFeature->getKind());
  setActionEnabled(aFeatureId, true);
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
    QString aCmdDocKind;
#ifdef HAVE_SALOME
    QString aId = eachAction->data().toString();
    if (!aId.isEmpty()) {
      aCmdDocKind = aWorkshop->salomeConnector()->documentKind(aId);
    }
#else
    AppElements_Command* aCmd = dynamic_cast<AppElements_Command*>(eachAction);
    aCmdDocKind = aCmd->documentKind();
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
