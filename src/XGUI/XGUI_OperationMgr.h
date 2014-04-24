#ifndef XGUI_OperationMgr_H
#define XGUI_OperationMgr_H

#include "XGUI.h"

#include <ModuleBase_Operation.h>

#include <QList>
#include <QObject>

/**\class XGUI_OperationMgr
 * \ingroup GUI
 * \brief Operation manager. Servers to manupulate to the workshop operations. Contains a stack
 * of started operations. In simple case, if only one operration is started, the stack contains
 * one operation. It is possible for some kind of operations to start them above already
 * started one. In that case, the previous active operation becames suspended, a new one - active.
 * The new operation is added to the top of the stack. Then it is finished, it is removed from
 * the stack and the previous operation is activated.
 */
class XGUI_EXPORT XGUI_OperationMgr : public QObject
{
  Q_OBJECT
public:
  /// Constructor
  /// \param theParent the parent
  XGUI_OperationMgr(QObject* theParent);
  /// Destructor
  virtual ~XGUI_OperationMgr();

  /// Returns the current operation or NULL
  /// \return the current operation
  ModuleBase_Operation* currentOperation() const;
  /// Sets the current operation or NULL
  /// \return the current operation
  bool startOperation(ModuleBase_Operation* theOperation);

  void commitCurrentOperation();

signals:
  void operationStarted();
  void operationStopped(ModuleBase_Operation* theOperation);

protected:
  bool canStartOperation(ModuleBase_Operation* theOperation);

protected slots:
  void onOperationStopped();

private:
  typedef QList<ModuleBase_Operation*> Operations; ///< definition for a list of operations
  Operations myOperations; ///< a stack of started operations. The active operation is on top,
                           // others are suspended and started by the active is finished
};

#endif
