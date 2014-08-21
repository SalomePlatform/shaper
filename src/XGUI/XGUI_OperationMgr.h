// File:        XGUI_OperationMgr.h
// Created:     20 Apr 2014
// Author:      Natalia ERMOLAEVA

#ifndef XGUI_OperationMgr_H
#define XGUI_OperationMgr_H

#include "XGUI.h"

#include <ModuleBase_Operation.h>

#include <QList>
#include <QObject>
#include <QStringList>

class QKeyEvent;

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
  /// Returns true is operation manager has at least one non-null operation.
  bool hasOperation() const;
  /// Returns number of operations in the stack
  int operationsCount() const;
  /// Start the operation and append it to the stack of operations
  /// \param theOperation the started operation
  /// \return the state whether the current operation is started
  bool startOperation(ModuleBase_Operation* theOperation);

  /// Abort the operation and append it to the stack of operations
  /// \return the state whether the current operation is aborted
  bool abortOperation();
  ///Returns list of all operations IDs
  QStringList operationList();

  virtual bool eventFilter(QObject *theObject, QEvent *theEvent);

 public slots:
  void validateCurrentOperation();

signals:
  /// Signal about an operation is started. It is emitted after the start() of operation is done.
  void operationStarted();
  /// Signal about an operation is stopped. It is emitted after the stop() of operation is done.
  /// \param theOperation a stopped operation
  void operationStopped(ModuleBase_Operation* theOperation);
  /// Signal about an operation is resumed. It is emitted after the resume() of operation is done.
  void operationResumed();
  /// Signal is emitted after the validate methods calls.
  void operationValidated(bool);
  /// Signal about the necessety of the next widget activating
  /// \param theWidget the model widget
  void activateNextWidget(ModuleBase_ModelWidget* theWidget);

 protected:
  /// Sets the current operation or NULL
  /// \param theOperation the started operation
  /// \param isCheckBeforeStart the flag whether to check whether the operation can be started
  /// \return the state whether the operation is resumed
  void resumeOperation(ModuleBase_Operation* theOperation);

  /// Returns whether the operation can be started. Check if there is already started operation and
  /// the granted parameter of the launched operation
  /// \param theOperation an operation to check
  bool canStartOperation(ModuleBase_Operation* theOperation);

  /// Returns whether the operation can be stopped.
  bool canStopOperation();

  /// Returns true if the operation can be aborted
  bool canAbortOperation();

  void validateOperation(ModuleBase_Operation* theOperation);

 protected slots:
  /// Slot that commits the current operation.
  void onCommitOperation();
  /// Slot that aborts the current operation.
  void onAbortOperation();

  /// Slot that is called by an operation stop. Removes the stopped operation form the stack.
  /// If there is a suspended operation, restart it.
  void onOperationStopped();

  /// SLOT, that is called by the key in the property panel is clicked.
  /// \param theName the attribute name
  /// \param theEvent the mouse event
  void onKeyReleased(const std::string& theName, QKeyEvent* theEvent);

  /// SLOT, that reacts to the widget activation
  /// \param theWidget an activated widget
  void onWidgetActivated(ModuleBase_ModelWidget* theWidget);

 private:
  typedef QList<ModuleBase_Operation*> Operations;  ///< definition for a list of operations
  Operations myOperations;  ///< a stack of started operations. The active operation is on top,
                            // others are suspended and started by the active is finished
};

#endif
