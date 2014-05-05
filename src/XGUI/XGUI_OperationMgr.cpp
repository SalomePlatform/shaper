// File:        XGUI_OperationMgr.h
// Created:     20 Apr 2014
// Author:      Natalia ERMOLAEVA

#include "XGUI_OperationMgr.h"

#include "ModuleBase_Operation.h"

#include <QMessageBox>

XGUI_OperationMgr::XGUI_OperationMgr(QObject* theParent)
: QObject(theParent)
{
}

XGUI_OperationMgr::~XGUI_OperationMgr()
{
}

ModuleBase_Operation* XGUI_OperationMgr::currentOperation() const
{
  return myOperations.count() > 0 ? myOperations.last() : 0;
}

bool XGUI_OperationMgr::startOperation(ModuleBase_Operation* theOperation)
{
  if (!canStartOperation(theOperation))
    return false;

  myOperations.append(theOperation);

  connect(theOperation, SIGNAL(stopped()), this, SLOT(onOperationStopped()));
  connect(theOperation, SIGNAL(started()), this, SIGNAL(operationStarted()));

  theOperation->start();
  return true;
}

void XGUI_OperationMgr::resumeOperation(ModuleBase_Operation* theOperation)
{
  connect(theOperation, SIGNAL(stopped()), this, SLOT(onOperationStopped()));
  connect(theOperation, SIGNAL(started()), this, SIGNAL(operationStarted()));

  theOperation->resume();
}

bool XGUI_OperationMgr::canStartOperation(ModuleBase_Operation* theOperation)
{
  bool aCanStart = true;
  ModuleBase_Operation* aCurrentOp = currentOperation();
  if (aCurrentOp && !theOperation->isGranted())
  {
    int anAnswer = QMessageBox::question(0, tr("Operation launch"),
                                tr("Previous operation is not finished and will be aborted"),
                                QMessageBox::Ok, QMessageBox::Cancel);
    if (anAnswer == QMessageBox::Ok) {
      aCurrentOp->abort();
    } else {
      aCanStart = false;
    }
  }
  return aCanStart;
}

void XGUI_OperationMgr::onOperationStopped()
{
  ModuleBase_Operation* aSenderOperation = dynamic_cast<ModuleBase_Operation*>(sender());
  ModuleBase_Operation* anOperation = currentOperation();
  if (!aSenderOperation || !anOperation || aSenderOperation != anOperation )
    return;

  emit operationStopped(anOperation);

  myOperations.removeAll(anOperation);
  anOperation->deleteLater();

  // get last operation which can be resumed
  ModuleBase_Operation* aResultOp = 0;
  QListIterator<ModuleBase_Operation*> anIt(myOperations);
  anIt.toBack();
  while(anIt.hasPrevious())
  {
    ModuleBase_Operation* anOp = anIt.previous();
    if (anOp) {
      aResultOp = anOp;
      break;
    }
  }
  if (aResultOp)
    resumeOperation(aResultOp);
}
