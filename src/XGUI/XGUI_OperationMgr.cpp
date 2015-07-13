// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:        XGUI_OperationMgr.cpp
// Created:     20 Apr 2014
// Author:      Natalia ERMOLAEVA

#include "XGUI_OperationMgr.h"

#include "ModuleBase_Operation.h"
#include "ModuleBase_IWorkshop.h"
#include "ModuleBase_IModule.h"

#include "ModelAPI_CompositeFeature.h"
#include "ModelAPI_Session.h"

#include <QMessageBox>
#include <QApplication>
#include <QKeyEvent>

XGUI_OperationMgr::XGUI_OperationMgr(QObject* theParent,
                                     ModuleBase_IWorkshop* theWorkshop)
: QObject(theParent), myIsValidationLock(false), myIsApplyEnabled(false),
  myWorkshop(theWorkshop)
{
}

XGUI_OperationMgr::~XGUI_OperationMgr()
{
}

ModuleBase_Operation* XGUI_OperationMgr::currentOperation() const
{
  return myOperations.count() > 0 ? myOperations.last() : 0;
}

bool XGUI_OperationMgr::isCurrentOperation(ModuleBase_Operation* theOperation)
{
  if(!hasOperation())
    return false;
  return currentOperation() == theOperation;
}

bool XGUI_OperationMgr::hasOperation() const
{
  return !myOperations.isEmpty() && (myOperations.last() != NULL);
}

bool XGUI_OperationMgr::hasOperation(const QString& theId) const
{
  foreach(ModuleBase_Operation* aOp, myOperations) {
    if (aOp->id() == theId)
      return true;
  }
  return false;
}

ModuleBase_Operation* XGUI_OperationMgr::findOperation(const QString& theId) const
{
  foreach(ModuleBase_Operation* aOp, myOperations) {
    if (aOp->id() == theId)
      return aOp;
  }
  return 0;
}


int XGUI_OperationMgr::operationsCount() const
{
  return myOperations.count();
}

QStringList XGUI_OperationMgr::operationList() const
{
  QStringList result;
  foreach(ModuleBase_Operation* eachOperation, myOperations) {
    FeaturePtr aFeature = eachOperation->feature();
    if(aFeature) {
      result << QString::fromStdString(aFeature->getKind());
    }
  }
  return result;
}

ModuleBase_Operation* XGUI_OperationMgr::previousOperation(ModuleBase_Operation* theOperation) const
{
  int idx = myOperations.lastIndexOf(theOperation);
  if(idx == -1 || idx == 0) {
    return NULL;
  }
  return myOperations.at(idx - 1);
}

bool XGUI_OperationMgr::eventFilter(QObject *theObject, QEvent *theEvent)
{
  if (theEvent->type() == QEvent::KeyRelease) {
    QKeyEvent* aKeyEvent = dynamic_cast<QKeyEvent*>(theEvent);
    if(aKeyEvent) {
      return onKeyReleased(aKeyEvent);
    }
  }
  return QObject::eventFilter(theObject, theEvent);
}

bool XGUI_OperationMgr::startOperation(ModuleBase_Operation* theOperation)
{
  if (hasOperation())
    currentOperation()->postpone();
  myOperations.append(theOperation);

  connect(theOperation, SIGNAL(started()), SLOT(onOperationStarted()));
  connect(theOperation, SIGNAL(aborted()), SLOT(onOperationAborted()));
  connect(theOperation, SIGNAL(committed()), SLOT(onOperationCommitted()));
  connect(theOperation, SIGNAL(stopped()), SLOT(onOperationStopped()));
  connect(theOperation, SIGNAL(resumed()), SLOT(onOperationResumed()));
  connect(theOperation, SIGNAL(triggered(bool)), SLOT(onOperationTriggered(bool)));
  connect(theOperation, SIGNAL(activatedByPreselection()),
          SIGNAL(operationActivatedByPreselection()));

  theOperation->start();
  onValidateOperation();
  return true;
}

bool XGUI_OperationMgr::abortAllOperations()
{
  if(!hasOperation()) {
    return true;
  } else if (operationsCount() == 1) {
    onAbortOperation();
    return true;
  }
  QString aMessage = tr("All active operations will be aborted.");
  int anAnswer = QMessageBox::question(qApp->activeWindow(),
                                       tr("Abort operation"),
                                       aMessage,
                                       QMessageBox::Ok | QMessageBox::Cancel,
                                       QMessageBox::Cancel);
  bool result = anAnswer == QMessageBox::Ok;
  while(result && hasOperation()) {
    currentOperation()->abort();
  }
  return result;
}

bool XGUI_OperationMgr::commitAllOperations()
{
  bool isCompositeCommitted = false;
  while (hasOperation()) {
    ModuleBase_Operation* anOperation = currentOperation();
    if (isApplyEnabled()) {
      onCommitOperation();
    } else {
      anOperation->abort();
    }
    FeaturePtr aFeature = anOperation->feature();
    CompositeFeaturePtr aComposite = 
        std::dynamic_pointer_cast<ModelAPI_CompositeFeature>(aFeature);
    isCompositeCommitted = aComposite.get();
    if (isCompositeCommitted)
      break;
  }
  return true;
}

void XGUI_OperationMgr::onValidateOperation()
{
  if (!hasOperation())
    return;
  ModuleBase_Operation* anOperation = currentOperation();
  if(anOperation) {
    bool aCanCommit = myWorkshop->module()->canCommitOperation();
    setApplyEnabled(!myIsValidationLock && aCanCommit && anOperation->isValid());
  }
}

void XGUI_OperationMgr::setLockValidating(bool toLock)
{
  myIsValidationLock = toLock;
  onValidateOperation();
}

void XGUI_OperationMgr::setApplyEnabled(const bool theEnabled)
{
  myIsApplyEnabled = theEnabled;
  emit validationStateChanged(theEnabled);
}

bool XGUI_OperationMgr::isApplyEnabled() const
{
  return myIsApplyEnabled;
}

bool XGUI_OperationMgr::isParentOperationValid() const
{
  bool isValid = false;
  // the enable state of the parent operation of the nested one is defined by the rules that
  // firstly there are nested operations and secondly the parent operation is valid
  ModuleBase_Operation* aPrevOp = 0;
  Operations::const_iterator anIt = myOperations.end();
  if (anIt != myOperations.begin()) { // there are items in the operations list
    --anIt;
    aPrevOp = *anIt; // the last top operation, the operation which is started
    if (anIt != myOperations.begin()) { // find the operation where the started operation is nested
      --anIt;
      aPrevOp = *anIt;
    }
  }
  return aPrevOp && aPrevOp->isValid();
}

bool XGUI_OperationMgr::canStopOperation()
{
  ModuleBase_Operation* anOperation = currentOperation();
  if(operationsCount() > 1) //in case of nested (sketch) operation no confirmation needed
    return true;
  if (anOperation && anOperation->isModified()) {
    QString aMessage = tr("%1 operation will be aborted.").arg(anOperation->id());
    int anAnswer = QMessageBox::question(qApp->activeWindow(),
                                         tr("Abort operation"),
                                         aMessage,
                                         QMessageBox::Ok | QMessageBox::Cancel,
                                         QMessageBox::Cancel);
    return anAnswer == QMessageBox::Ok;
  }
  return true;
}

bool XGUI_OperationMgr::commitOperation()
{
  if (hasOperation() && currentOperation()->isValid()) {
    onCommitOperation();
    return true;
  }
  return false;
}

void XGUI_OperationMgr::resumeOperation(ModuleBase_Operation* theOperation)
{
  theOperation->resume();
}

bool XGUI_OperationMgr::canStartOperation(QString theId)
{
  bool aCanStart = true;
  ModuleBase_Operation* aCurrentOp = currentOperation();
  if (aCurrentOp) {
    if (!aCurrentOp->isGranted(theId)) {
      if (canStopOperation()) {
        if (myIsApplyEnabled)
          aCurrentOp->commit();
        else
          aCurrentOp->abort();
      } else {
        aCanStart = false;
      }
    }
  }
  return aCanStart;
}


void XGUI_OperationMgr::onCommitOperation()
{
  ModuleBase_Operation* anOperation = currentOperation();
  if (anOperation)
    anOperation->commit();
}

void XGUI_OperationMgr::onAbortOperation()
{
  if (hasOperation() && canStopOperation()) {
    currentOperation()->abort();
  }
}

void XGUI_OperationMgr::onOperationStarted()
{
  ModuleBase_Operation* aSenderOperation = dynamic_cast<ModuleBase_Operation*>(sender());
  
  bool aParentValid = isParentOperationValid();
  // in order to apply is enabled only if there are modifications in the model
  // e.g. sketch can be applyed only if at least one nested element modification is finished
  bool aCanUndo = ModelAPI_Session::get()->canUndo();
  emit nestedStateChanged(aParentValid && aCanUndo);

  emit operationStarted(aSenderOperation);
}

void XGUI_OperationMgr::onOperationAborted()
{
  ModuleBase_Operation* aSenderOperation = dynamic_cast<ModuleBase_Operation*>(sender());
  emit operationAborted(aSenderOperation);
}

void XGUI_OperationMgr::onOperationCommitted()
{
  ModuleBase_Operation* aSenderOperation = dynamic_cast<ModuleBase_Operation*>(sender());
  // in order to apply is enabled only if there are modifications in the model
  // e.g. sketch can be applyed only if at least one nested element create is finished
  bool aCanUndo = ModelAPI_Session::get()->canUndo();
  emit nestedStateChanged(myOperations.count() >= 1 && aCanUndo);
  emit operationCommitted(aSenderOperation);
}

void XGUI_OperationMgr::onOperationResumed()
{
  ModuleBase_Operation* aSenderOperation = dynamic_cast<ModuleBase_Operation*>(sender());
  emit operationResumed(aSenderOperation);
}

void XGUI_OperationMgr::onOperationStopped()
{
  ModuleBase_Operation* aSenderOperation = dynamic_cast<ModuleBase_Operation*>(sender());
  ModuleBase_Operation* aCurrentOperation = currentOperation();
  if (!aSenderOperation || !aCurrentOperation || aSenderOperation != aCurrentOperation)
    return;

  myOperations.removeAll(aCurrentOperation);
  aCurrentOperation->deleteLater();

  emit operationStopped(aCurrentOperation);

  // get last operation which can be resumed
  ModuleBase_Operation* aResultOp = 0;
  QListIterator<ModuleBase_Operation*> anIt(myOperations);
  anIt.toBack();
  while (anIt.hasPrevious()) {
    ModuleBase_Operation* anOp = anIt.previous();
    if (anOp) {
      aResultOp = anOp;
      break;
    }
  }
  if (aResultOp) {
    bool isModified = aCurrentOperation->isModified();
    aResultOp->setIsModified(aResultOp->isModified() || isModified);
    resumeOperation(aResultOp);
    onValidateOperation();
  }
}

void XGUI_OperationMgr::onOperationTriggered(bool theState)
{
  ModuleBase_Operation* aSenderOperation = dynamic_cast<ModuleBase_Operation*>(sender());
  if (aSenderOperation && !theState) {
    ModuleBase_Operation* aCurrentOperation = currentOperation();
    if (aSenderOperation == aCurrentOperation)
      aCurrentOperation->abort();
    else {
      // it is possible to trigger upper operation(e.g. sketch, current is sketch line)
      // all operation from the current to triggered should also be aborted
      while(hasOperation()) {
        ModuleBase_Operation* aCurrentOperation = currentOperation();
        aCurrentOperation->abort();
        if(aSenderOperation == aCurrentOperation)
          break;
      }
    }
  }
}

bool XGUI_OperationMgr::onKeyReleased(QKeyEvent* theEvent)
{
  // Let the manager decide what to do with the given key combination.
  ModuleBase_Operation* anOperation = currentOperation();
  bool isAccepted = true;
  switch (theEvent->key()) {
    case Qt::Key_Return:
    case Qt::Key_Enter: {
      emit keyEnterReleased();
      commitOperation();
    }
    break;
    default:
      isAccepted = false;
      break;
  }
  //if(anOperation) {
  //  anOperation->keyReleased(theEvent->key());
  //}
  return isAccepted;
}

