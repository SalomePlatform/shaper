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

QStringList XGUI_OperationMgr::operationList()
{
  QStringList result;
  foreach(ModuleBase_Operation* eachOperation, myOperations)
  {
    result << eachOperation->id();
  }
  return result;
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
  onValidateOperation();
  return true;
}

bool XGUI_OperationMgr::abortAllOperations()
{
  if (operationsCount() == 1) {
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

bool XGUI_OperationMgr::validateOperation(ModuleBase_Operation* theOperation)
{
  //Get operation feature to validate
  FeaturePtr aFeature = theOperation->feature();
  //Get validators for the Id
  SessionPtr aMgr = ModelAPI_Session::get();
  ModelAPI_ValidatorsFactory* aFactory = aMgr->validators();

  bool isValid = aFactory->validate(aFeature);
  emit operationValidated(isValid);
  return isValid;
}

void XGUI_OperationMgr::onValidateOperation()
{
  if (!hasOperation())
    return;
  ModuleBase_Operation* anOperation = currentOperation();
  validateOperation(currentOperation());
}

bool XGUI_OperationMgr::eventFilter(QObject *theObject, QEvent *theEvent)
{
  if (theEvent->type() == QEvent::KeyRelease) {
    QKeyEvent* aKeyEvent = dynamic_cast<QKeyEvent*>(theEvent);
    // TODO: this is Escape button processing when the property panel has empty content,
    // but the operation should be stopped by the Enter has been clicked
    if(aKeyEvent) {
      onKeyReleased(aKeyEvent);
      return true;
    }
  }
  return QObject::eventFilter(theObject, theEvent);
}

void XGUI_OperationMgr::commitOperation()
{
  if (validateOperation(currentOperation())) {
    onCommitOperation();
  }
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
        if (canAbortOperation()) {
          aCurrentOp->abort();
        } else {
          aCanStart = false;
        }
      }
    }
  }
  return aCanStart;
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
  if (hasOperation() && canAbortOperation()) {
    currentOperation()->abort();
  }
}

bool XGUI_OperationMgr::canAbortOperation()
{
  ModuleBase_Operation* anOperation = currentOperation();
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

void XGUI_OperationMgr::onKeyReleased(QKeyEvent* theEvent)
{
  ModuleBase_Operation* anOperation = currentOperation();
  if (anOperation) {
    bool isFinished = anOperation->keyReleased(theEvent->key());
    if(isFinished)
      return;
  }
  // Let the manager decide what to do with the given key combination.
  switch (theEvent->key()) {
    case Qt::Key_Escape:
      onAbortOperation();
      break;
    case Qt::Key_Return:
    case Qt::Key_Enter:
      commitOperation();
      break;
    default:
      break;
  }
}

void XGUI_OperationMgr::onWidgetActivated(ModuleBase_ModelWidget* theWidget)
{
  ModuleBase_Operation* anOperation = currentOperation();
  if (anOperation)
    anOperation->onWidgetActivated(theWidget);
}
