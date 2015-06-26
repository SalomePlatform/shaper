// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

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
 * \brief Operation manager. Servers to manipulate to the workshop operations. Contains a stack
 * of started operations. In simple case, if only one operation is started, the stack contains
 * one operation. It is possible for some kind of operations to start them above already
 * started one. In that case, the previous active operation becomes suspended, a new one - active.
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

  /// Check if the given operation is active operation.
  /// Also, returns false is ther is no active operation.
  bool isCurrentOperation(ModuleBase_Operation* theOperation);

  /// Returns true is operation manager has at least one non-null operation.
  bool hasOperation() const;

  /// Returns true is operation manager has an operation with given Id.
  bool hasOperation(const QString& theId) const;

  /// Returns true if the operation can be aborted
  bool canStopOperation();

  /// Find and return operation by its Id.
  ModuleBase_Operation* findOperation(const QString& theId) const;

  /// Returns number of operations in the stack
  int operationsCount() const;

  /// Returns list of all operations IDs
  QStringList operationList() const;

  /// Returns previous (parent) operation if given operation started.
  /// else, or if there is no parent - returns NULL
  ModuleBase_Operation* previousOperation(ModuleBase_Operation* theOperation) const;

  /// Redefinition of virtual function
  virtual bool eventFilter(QObject *theObject, QEvent *theEvent);

  /// Start the operation and append it to the stack of operations
  /// \param theOperation the started operation
  /// \return the state whether the current operation is started
  bool startOperation(ModuleBase_Operation* theOperation);

  /// Returns whether the operation can be started. Check if there is already started operation and
  /// the granted parameter of the launched operation
  /// \param theId id of the operation which is going to start
  bool canStartOperation(QString theId);

  /// Blocking/unblocking enabling of Ok button in property panel.
  /// It is used when operation can not be validated even all attributes are valid
  void setLockValidating(bool toLock);

  /// Returns state of validation locking
  bool isValidationLocked() const { return myIsValidationLock; }

  /// Returns enable apply state 
  /// \return theEnabled a boolean value
  bool isApplyEnabled() const;

  public slots:
  /// Slot that commits the current operation.
  void onCommitOperation();
  /// Slot that aborts the current operation.
  void onAbortOperation();
  /// Slot that validates the current operation using the validateOperation method.
  void onValidateOperation();
  /// Commit all operations
  bool commitAllOperations();
  /// Abort all operations
  bool abortAllOperations();

signals:
  /// Signal about an operation is started. It is emitted after the start() of operation is done.
  void operationStarted(ModuleBase_Operation* theOperation);

  /// Signal about an operation is stopped. It is emitted after the stop() of operation is done.
  /// \param theOperation a stopped operation
  void operationStopped(ModuleBase_Operation* theOperation);

  /// Signal about an operation is resumed. It is emitted after the resume() of operation is done.
  void operationResumed(ModuleBase_Operation* theOperation);

  /// Emitted when current operation is Committed
  void operationCommitted(ModuleBase_Operation* theOperation);

  /// Emitted when current operation is aborted
  void operationAborted(ModuleBase_Operation* theOperation);

  /// Signal is emitted after the apply enable state changed.
  void validationStateChanged(bool);

  /// Signal is emitted after the apply enable state changed.
  void nestedStateChanged(bool);

  /// Signal is emitted after the current operation is filled with existing preselection.
  void operationActivatedByPreselection();

  /// Signal is emitted after the key released click.
  void keyEnterReleased();

 protected:
  /// Sets apply state to the value and emit signal about this state is changed
  /// \param theEnabled the state value
  void setApplyEnabled(const bool theEnabled);

  /// Commits the current operatin if it is valid
  bool commitOperation();

  /// Sets the current operation or NULL
  /// \param theOperation the started operation
  void resumeOperation(ModuleBase_Operation* theOperation);

 public slots:
  /// SLOT, that is called by the key in the property panel is clicked.
  /// \param theEvent the mouse event
  bool onKeyReleased(QKeyEvent* theEvent);

  protected slots:
  /// Slot that is called by an operation stop. Removes the stopped operation form the stack.
  /// If there is a suspended operation, restart it.
  void onOperationStopped();

  /// Slot called on operation start
  void onOperationStarted();

  /// Slot called on operation abort
  void onOperationAborted();

  /// Slot called on operation commit
  void onOperationCommitted();

  /// Slot called on operation resume
  void onOperationResumed();

  /// Slot called on operation triggered
  void onOperationTriggered(bool theState);

 private:
  typedef QList<ModuleBase_Operation*> Operations;  ///< definition for a list of operations
  Operations myOperations;  ///< a stack of started operations. The active operation is on top,
                            // others are suspended and started by the active is finished

  /// Lock/Unlock access to Ok button in property panel
  bool myIsValidationLock;
  /// Lock/Unlock access to Ok button in property panel
  bool myIsApplyEnabled;
};

#endif
