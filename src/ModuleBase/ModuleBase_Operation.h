// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

/*
 * ModuleBase_Operation.h
 *
 *  Created on: Apr 2, 2014
 *      Author: sbh
 */

#ifndef ModuleBase_Operation_H
#define ModuleBase_Operation_H

#include <ModuleBase.h>

#include <QObject>
#include <QString>
#include <QStringList>

class ModuleBase_ModelWidget;
class ModuleBase_OperationDescription;
class ModuleBase_IPropertyPanel;

class QKeyEvent;

/*!
 * \class ModuleBase_Operation
 * \ingroup GUI
 * \brief Base class for all operations
 *
 *  Base class for all operations. If you perform an action it is reasonable to create
 *  operation intended for this. This is a base class for all operations which provides
 *  mechanism for correct starting operations, starting operations above already started
 *  ones, committing operations and so on. To create own operation it is reasonable to
 *  inherit it from this class and redefines virtual methods to provide own behavior
 *  Main virtual methods are
 *  - virtual bool      isReadyToStart();
 *  - virtual void      startOperation();
 *  - virtual void      abortOperation();
 *  - virtual void      commitOperation();
 */

class MODULEBASE_EXPORT ModuleBase_Operation : public QObject
{
Q_OBJECT

 public:
  /// Constructor
  /// \param theId the operation identifier
  /// \param theParent the QObject parent
  ModuleBase_Operation(const QString& theId = "", QObject* theParent = 0);

  /// Destructor
  virtual ~ModuleBase_Operation();

  /// Returns the operation description
  /// /returns the instance of the description class
  ModuleBase_OperationDescription* getDescription() const { return myDescription; }

  /// Returns list of granted operation indices
  const QStringList& grantedOperationIds() const;

  /// Sets list of operation indices, which can be started without the current operation stop
  /// \param theList an ids
  void setGrantedOperationIds(const QStringList& theList);

  /// Must return true if this operation can be launched as nested for any current operation
  /// and it is not necessary to check this operation on validity. By default 
  /// the operation is not granted.
  /// The method has to be redefined for granted operations.
  virtual bool isGranted(QString theId) const;

  /// Returns True if data of its feature was modified during operation
  virtual bool isModified() const { return myIsModified; }

  /// Change the modified state of the operation
  void setIsModified(const bool theIsModified) { myIsModified = theIsModified;  }

  /// Returns operations Id from it's description
  QString id() const;

  /// Must return True if the operation's feature is valid.
  /// Since IOperation does not have any feature returns false.
  virtual bool isValid() const;

  /// \brief Set property pane to the operation
  /// \param theProp a property panel instance
  virtual void setPropertyPanel(ModuleBase_IPropertyPanel* theProp);

  /// \return Currently installed property panel
  ModuleBase_IPropertyPanel* propertyPanel() const { return myPropertyPanel; }

signals:
  /// The operation is started
  void beforeStarted();
  /// The operation is started
  void started();

  /// The operation is aborted
  void beforeAborted();
  /// The operation is aborted
  void aborted();

  /// The operation is committed
  void beforeCommitted();
  /// The operation is committed
  void committed();

  /// The operation is aborted or committed
  void stopped();

  /// The operation is resumed
  void resumed();

  /// The operation is postponed
  void postponed();

 public slots:
  /// Starts operation
  /// Public slot. Verifies whether operation can be started and starts operation.
  /// This slot is not virtual and cannot be redefined. Redefine startOperation method
  /// to change behavior of operation. There is no point in using this method. It would
  /// be better to inherit own operator from base one and redefine startOperation method
  /// instead.
  /// \return true if the start is successful
  virtual bool start();

  /// Deactivates current operation which can be resumed later.
  virtual void postpone();

  /// Resumes operation
  /// Public slot. Verifies whether operation can be started and starts operation.
  /// This slot is not virtual and cannot be redefined. Redefine startOperation method
  /// to change behavior of operation. There is no point in using this method. It would
  /// be better to inherit own operator from base one and redefine startOperation method
  /// instead.
  virtual void resume();

  /// Aborts operation
  /// Public slot. Aborts operation. This slot is not virtual and cannot be redefined.
  /// Redefine abortOperation method to change behavior of operation instead
  virtual void abort();

  /// Commits operation
  /// Public slot. Commits operation. This slot is not virtual and cannot be redefined.
  /// Redefine commitOperation method to change behavior of operation instead
  virtual bool commit();

  /// Changes the modified flag of the operation
  void onValuesChanged();

 protected:
  /// Virtual method called when operation started (see start() method for more description)
  /// Default impl calls corresponding slot and commits immediately.
   virtual void startOperation() {}

  /// Implementation of specific steps on postpone operation
  virtual void postponeOperation() {}

  /// Virtual method called when operation stopped - committed or aborted.
  virtual void stopOperation() {}

  /// Virtual method called when operation aborted (see abort() method for more description)
  virtual void abortOperation() {}

  /// Virtual method called when operation committed (see commit() method for more description)
  virtual void commitOperation() {};

  /// Virtual method called after operation committed (see commit() method for more description)
  virtual void afterCommitOperation() {}

  /// Virtual method called after operation resume (see resume() method for more description)
  virtual void resumeOperation() {}

  /// Verifies whether this operator can be commited.
  /// \return Returns TRUE if current operation can be committed, e.g. all parameters are filled
  virtual bool canBeCommitted() const;

private:
  /// the container to have the operation description
  ModuleBase_OperationDescription* myDescription;

  /// Modified feature flag
  bool myIsModified;

  /// List of operations IDs which are granted of the current operation
  QStringList myGrantedIds;

  /// Access to property panel
  ModuleBase_IPropertyPanel* myPropertyPanel;
};

#endif
