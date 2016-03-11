// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:        XGUI_OperationMgr.h
// Created:     20 Apr 2014
// Author:      Natalia ERMOLAEVA

#ifndef XGUI_OperationMgr_H
#define XGUI_OperationMgr_H

#include "XGUI.h"

#include <ModuleBase_Operation.h>
#include "ModelAPI_Feature.h"

#include <QList>
#include <QObject>
#include <QStringList>

class QKeyEvent;

class ModuleBase_IWorkshop;
class XGUI_Workshop;
class XGUI_ShortCutListener;

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
  /// \param theWorkshop a reference to workshop
  XGUI_OperationMgr(QObject* theParent, ModuleBase_IWorkshop* theWorkshop);

  /// Destructor
  virtual ~XGUI_OperationMgr();

  /// Switch on short cut listener
  void activate();
  /// Switch off short cut listener
  void deactivate();

  /// Set reference to workshop
  /// \param theWorkshop reference to workshop
  void setWorkshop(ModuleBase_IWorkshop* theWorkshop)
  { myWorkshop = theWorkshop; };

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

  /// Returns true if the operation can be aborted. If the operation is modified,
  /// the warning message box is shown.
  /// \param theOperation an operation which is checked on stop
  bool canStopOperation(ModuleBase_Operation* theOperation);

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
  /// \param theObject a sender of the event
  virtual bool eventFilter(QObject *theObject, QEvent *theEvent);

  /// Start the operation and append it to the stack of operations
  /// \param theOperation the started operation
  /// \return the state whether the current operation is started
  bool startOperation(ModuleBase_Operation* theOperation);

  /// Returns whether the operation can be started. Check if there is already started operation and
  /// the granted parameter of the launched operation
  /// \param theId id of the operation which is going to start
  bool canStartOperation(const QString& theId);

  /// Aborts the parameter operation if it is current, else abort operations from the stack
  /// of operations until the operation is found. All operations upper the parameter one are
  /// not aborted.
  /// \param theOperation an aborted operation
  void abortOperation(ModuleBase_Operation* theOperation);

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

  /// Signal is emitted after the key released click.
  void keyEnterReleased();

public: // TEMPORARY, it should be protected and be performed automatically
  /// Emits nestedStateChange for operations with an information about validity of the operation
  /// \param theOperation the sent operation. If it is NULL, all operations in the stack are sent.
  void updateApplyOfOperations(ModuleBase_Operation* theOperation = 0);

  /// Commits the current operatin if it is valid
  bool commitOperation();

protected: // TEMPORARY
  /// Sets the current operation or NULL
  /// \param theOperation the started operation
  void resumeOperation(ModuleBase_Operation* theOperation);

  /// Returns whether the parameter operation is granted in relation to the previous operation
  /// in a stack of started operations. It is used in canStopOperation to avoid warning message
  /// when granted operation is aborted, e.g. SketchLine in Sketch
  /// \param theId id of the operation which is checked
  /// \return boolean result
  bool isGrantedOperation(const QString& theId);

  /// Sets the feature as a current in the document
  /// \param theFeature a feature
  void setCurrentFeature(const FeaturePtr& theFeature);

 public slots:
  /// SLOT, that is called by the key in the property panel is clicked.
  /// \param theObject a sender of the event
  /// \param theEvent the mouse event
  bool onKeyReleased(QObject *theObject, QKeyEvent* theEvent);

  /// The functionaly, that should be done by delete click
  /// Fistly the active widget processes it, then workshop. If no one does not
  /// process it, do nothing
  /// \param theObject a sender of the event
  bool onProcessDelete(QObject* theObject);

  protected slots:
  /// The functionaly, that should be done by enter click
  /// Fistly the active widget processes it, then module. If no one does not
  /// process it, the current operation is committed
  /// \param theObject a sender of the event
  bool onProcessEnter(QObject *theObject);

  /// Slot that is called by an operation stop. Removes the stopped operation form the stack.
  /// If there is a suspended operation, restart it.
  void onOperationStopped();

  /// Slot called before operation started. Stores the previous current feature, set the feature
  /// of the operation as a current in the document. The previous current feature should be restored
  /// by the operation abort/commit
  void onBeforeOperationStarted();

  /// Slot called after operation started
  void onOperationStarted();

  /// Slot called before operation aborted. Restore the previous current operation
  void onBeforeOperationAborted();

  /// Slot called after operation aborted
  void onOperationAborted();

  /// Slot called before operation committed. Restore the previous current operation
  void onBeforeOperationCommitted();

  /// Slot called after operation committed
  void onOperationCommitted();

  /// Slot called on operation resume
  void onOperationResumed();

private:
  XGUI_Workshop* workshop() const;

  /// Checks if the object is a parent or a child under
  /// \param theObject an investivated object
  /// \param theParent a candidate to be a parent
  static bool isChildObject(const QObject* theObject, const QObject* theParent);

 private:
  typedef QList<ModuleBase_Operation*> Operations;  ///< definition for a list of operations
  Operations myOperations;  ///< a stack of started operations. The active operation is on top,
                            // others are suspended and started by the active is finished

  /// Current workshop
  ModuleBase_IWorkshop* myWorkshop;

  XGUI_ShortCutListener* myShortCutListener;
};

#endif
