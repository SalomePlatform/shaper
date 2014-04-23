#ifndef XGUI_OperationMgr_H
#define XGUI_OperationMgr_H

#include "XGUI.h"

#include <ModuleBase_Operation.h>

#include <QList>
#include <QObject>

/**\class XGUI_OperationMgr
 * \ingroup GUI
 * \brief Operation manager. Servers to manupulate to the workshop operations. Contains a stack
 *   of started operations and uses the upper one as a current.
 */
class XGUI_EXPORT XGUI_OperationMgr : public QObject
{
  Q_OBJECT
public:
  XGUI_OperationMgr(QObject* theParent);
  virtual ~XGUI_OperationMgr();

  ModuleBase_Operation* currentOperation() const;
  bool startOperation(ModuleBase_Operation* theOperation);

  void commitCurrentOperation();

signals:
  void beforeOperationStart();
  void afterOperationStart();

protected:
  bool canStartOperation(ModuleBase_Operation* theOperation);

protected slots:
  void onOperationStopped();

private:
  typedef QList<ModuleBase_Operation*> Operations;
  Operations myOperations;
};

#endif
