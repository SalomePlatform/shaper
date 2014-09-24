// File:        XGUI_OperationMgr.h
// Created:     20 Apr 2014
// Author:      Natalia ERMOLAEVA

#include "XGUI_OperationMgr.h"

#include "ModuleBase_Operation.h"

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
      onKeyReleased(aKeyEvent);
      return true;
    }
  }
  return QObject::eventFilter(theObject, theEvent);
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
  if(anOperation) {
    bool isValid = anOperation->isValid();
    emit operationValidated(isValid);
  }
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

bool XGUI_OperationMgr::canStartOperation(ModuleBase_Operation* theOperation)
{
  bool aCanStart = true;
  ModuleBase_Operation* aCurrentOp = currentOperation();
  if (aCurrentOp) {
    if (!aCurrentOp->isGranted(theOperation)) {
      if (canAbortOperation()) {
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
  if (hasOperation() && canAbortOperation()) {
    currentOperation()->abort();
  }
}

bool XGUI_OperationMgr::canAbortOperation()
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
  // Let the manager decide what to do with the given key combination.
  ModuleBase_Operation* anOperation = currentOperation();
  bool isRestart = false;
  switch (theEvent->key()) {
    case Qt::Key_Escape: {
      onAbortOperation();
    }
      break;
    case Qt::Key_Return:
    case Qt::Key_Enter: {
      if(anOperation) {
         anOperation->activateNextToCurrentWidget();
      }
      commitOperation();
    }
      break;
    default:
      break;
  }
  if(anOperation)
    anOperation->keyReleased(theEvent->key());
}

void XGUI_OperationMgr::onWidgetActivated(ModuleBase_ModelWidget* theWidget)
{
  ModuleBase_Operation* anOperation = currentOperation();
  if (anOperation)
    anOperation->onWidgetActivated(theWidget);
}
