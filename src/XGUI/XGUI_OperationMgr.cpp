// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:        XGUI_OperationMgr.h
// Created:     20 Apr 2014
// Author:      Natalia ERMOLAEVA

#include "XGUI_OperationMgr.h"

#include "ModuleBase_Operation.h"

#include <QMessageBox>
#include <QApplication>
#include <QKeyEvent>

XGUI_OperationMgr::XGUI_OperationMgr(QObject* theParent)
    : QObject(theParent), myIsValidationLock(false)
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

void XGUI_OperationMgr::onValidateOperation()
{
  if (!hasOperation())
    return;
  ModuleBase_Operation* anOperation = currentOperation();
  if(anOperation && (!myIsValidationLock)) {
    setApplyEnabled(anOperation->isValid());
  }
}

void XGUI_OperationMgr::setApplyEnabled(const bool theEnabled)
{
  myIsApplyEnabled = theEnabled;
  emit applyEnableChanged(theEnabled);
}

bool XGUI_OperationMgr::isApplyEnabled() const
{
  return myIsApplyEnabled;
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
  ModuleBase_Operation* anOperation = currentOperation();
  if (!aSenderOperation || !anOperation || aSenderOperation != anOperation)
    return;

  myOperations.removeAll(anOperation);
  anOperation->deleteLater();

  emit operationStopped(anOperation);

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
    resumeOperation(aResultOp);
    onValidateOperation();
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

