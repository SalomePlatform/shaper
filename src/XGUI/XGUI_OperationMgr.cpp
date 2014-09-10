// File:        XGUI_OperationMgr.h
// Created:     20 Apr 2014
// Author:      Natalia ERMOLAEVA

#include "XGUI_OperationMgr.h"

#include "ModuleBase_Operation.h"
#include <ModelAPI_Validator.h>
#include <ModelAPI_FeatureValidator.h>

#include <QMessageBox>
#include <QApplication>
#include <QKeyEvent>

XGUI_OperationMgr::XGUI_OperationMgr(QObject* theParent)
    : QObject(theParent)
{
  // listen to Escape signal to stop the current operation
  qApp->installEventFilter(this);
}

XGUI_OperationMgr::~XGUI_OperationMgr()
{
}

ModuleBase_Operation* XGUI_OperationMgr::currentOperation() const
{
  return myOperations.count() > 0 ? myOperations.last() : 0;
}

bool XGUI_OperationMgr::hasOperation() const
{
  return (myOperations.count() > 0) && (myOperations.last() != NULL);
}

int XGUI_OperationMgr::operationsCount() const
{
  return myOperations.count();
}

bool XGUI_OperationMgr::startOperation(ModuleBase_Operation* theOperation)
{
  if (!canStartOperation(theOperation))
    return false;

  myOperations.append(theOperation);

  connect(theOperation, SIGNAL(stopped()), this, SLOT(onOperationStopped()));
  connect(theOperation, SIGNAL(started()), this, SIGNAL(operationStarted()));
  connect(theOperation, SIGNAL(resumed()), this, SIGNAL(operationResumed()));
  connect(theOperation, SIGNAL(activateNextWidget(ModuleBase_ModelWidget*)), this,
          SIGNAL(activateNextWidget(ModuleBase_ModelWidget*)));

  theOperation->start();
  validateCurrentOperation();
  return true;
}

bool XGUI_OperationMgr::abortOperation()
{
  ModuleBase_Operation* aCurrentOp = currentOperation();
  if (!aCurrentOp || !canStopOperation())
    return false;

  aCurrentOp->abort();
  return true;
}

QStringList XGUI_OperationMgr::operationList()
{
  QStringList result;
  foreach(ModuleBase_Operation* eachOperation, myOperations)
  {
    result << eachOperation->id();
  }
  return result;
}

void XGUI_OperationMgr::validateOperation(ModuleBase_Operation* theOperation)
{
  //Get operation Id and feature to validate
  QString anOperationId = theOperation->id();
  FeaturePtr aFeature = theOperation->feature();
  //Get validators for the Id
  PluginManagerPtr aMgr = ModelAPI_PluginManager::get();
  ModelAPI_ValidatorsFactory* aFactory = aMgr->validators();

  bool isValid = aFactory->validate(aFeature);
  emit operationValidated(isValid);
}

void XGUI_OperationMgr::validateCurrentOperation()
{
  if (!hasOperation())
    return;
  ModuleBase_Operation* anOperation = currentOperation();
  validateOperation(currentOperation());
}

bool XGUI_OperationMgr::eventFilter(QObject *theObject, QEvent *theEvent)
{
  if (theEvent->type() == QEvent::KeyRelease) {
    QKeyEvent* aKeyEvent = (QKeyEvent*) theEvent;
    if (aKeyEvent && aKeyEvent->key() == Qt::Key_Escape) {
      // TODO: this is Escape button processing when the property panel has empty content,
      // but the operation should be stopped by the Enter has been clicked
      onKeyReleased(aKeyEvent);
      return true;
    }
  }
  return QObject::eventFilter(theObject, theEvent);
}

void XGUI_OperationMgr::resumeOperation(ModuleBase_Operation* theOperation)
{
  theOperation->resume();
}

bool XGUI_OperationMgr::canStartOperation(ModuleBase_Operation* theOperation)
{
  bool aCanStart = true;
  ModuleBase_Operation* aCurrentOp = currentOperation();
  if (aCurrentOp) {
    if (!theOperation->isGranted()) {
      if (!aCurrentOp->isValid(theOperation)) {
        if (canStopOperation()) {
          aCurrentOp->abort();
        } else {
          aCanStart = false;
        }
      }
    }
  }
  return aCanStart;
}

bool XGUI_OperationMgr::canStopOperation()
{
  ModuleBase_Operation* anOperation = currentOperation();
  if (anOperation) {
    if (anOperation->isModified()) {
      int anAnswer = QMessageBox::question(
          qApp->activeWindow(), tr("Operation launch"),
          tr("Previous operation is not finished and will be aborted"), QMessageBox::Ok,
          QMessageBox::Cancel);
      return anAnswer == QMessageBox::Ok;
    }
  }
  return true;
}

void XGUI_OperationMgr::onCommitOperation()
{
  ModuleBase_Operation* anOperation = currentOperation();
  anOperation->onWidgetActivated(NULL);
  if (anOperation)
    anOperation->commit();
}

void XGUI_OperationMgr::onAbortOperation()
{
  ModuleBase_Operation* anOperation = currentOperation();
  if (anOperation && canAbortOperation())
    anOperation->abort();
}

bool XGUI_OperationMgr::canAbortOperation()
{
  ModuleBase_Operation* anOperation = currentOperation();
  if (anOperation && anOperation->isModified()) {
    int anAnswer = QMessageBox::question(
        qApp->activeWindow(), tr("Cancel operation"),
        tr("Operation %1 will be cancelled. Continue?").arg(anOperation->id()), QMessageBox::Yes,
        QMessageBox::No);
    return anAnswer == QMessageBox::Yes;
  }
  return true;
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
    validateCurrentOperation();
  }
}

void XGUI_OperationMgr::onKeyReleased(QKeyEvent* theEvent)
{
  ModuleBase_Operation* anOperation = currentOperation();
  if (anOperation)
    anOperation->keyReleased(theEvent->key());
}

void XGUI_OperationMgr::onWidgetActivated(ModuleBase_ModelWidget* theWidget)
{
  ModuleBase_Operation* anOperation = currentOperation();
  if (anOperation)
    anOperation->onWidgetActivated(theWidget);
}
