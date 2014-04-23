#include "XGUI_OperationMgr.h"

#include "ModuleBase_Operation.h"

#include <QMessageBox>

/*!
 \brief Constructor
 */
XGUI_OperationMgr::XGUI_OperationMgr(QObject* theParent)
: QObject(theParent)
{
}

/*!
 \brief Destructor
 */
XGUI_OperationMgr::~XGUI_OperationMgr()
{
}

/*!
 \brief Returns the current operation or NULL
 * \return the current operation
 */
ModuleBase_Operation* XGUI_OperationMgr::currentOperation() const
{
  return myOperations.count() > 0 ? myOperations.last() : 0;
}

/*!
 \brief Sets the current operation or NULL
 * \return the current operation
 */
bool XGUI_OperationMgr::startOperation(ModuleBase_Operation* theOperation)
{
  if (!canStartOperation(theOperation))
    return false;

  myOperations.append(theOperation);
  emit beforeOperationStart();

  connect(theOperation, SIGNAL(stopped()), this, SLOT(onOperationStopped()));
  theOperation->start();

  emit afterOperationStart();
  return true;
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
    if (anAnswer == QMessageBox::Ok)
      aCurrentOp->abort();
    else
      aCanStart = false;
  }
  return aCanStart;
}

void XGUI_OperationMgr::commitCurrentOperation()
{
  ModuleBase_Operation* anOperation = currentOperation();
  if (!anOperation)
    return;

  anOperation->commit();
}

void XGUI_OperationMgr::onOperationStopped()
{
  ModuleBase_Operation* aSenderOperation = dynamic_cast<ModuleBase_Operation*>(sender());
  ModuleBase_Operation* anOperation = currentOperation();
  if (!aSenderOperation || !anOperation || aSenderOperation != anOperation )
    return;

  myOperations.removeAll(anOperation);

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
    startOperation(aResultOp);
}
