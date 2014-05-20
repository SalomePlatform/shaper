/*
 * ModuleBase_IOperation.h
 *
 *  Created on: May 5, 2014
 *      Author: nds
 */


#ifndef ModuleBase_IOperation_H
#define ModuleBase_IOperation_H

#include <ModuleBase.h>

#include <QObject>
#include <QString>

#include <boost/shared_ptr.hpp>

class ModelAPI_Document;
class ModuleBase_OperationDescription;

/*!
 \class ModuleBase_IOperation
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

class MODULEBASE_EXPORT ModuleBase_IOperation: public QObject
{
Q_OBJECT

public:
  /// Constructor
  /// Constructs an empty operation. Constructor should work very fast because many
  /// operators may be created after starting workshop but only several from them
  /// may be used. As result this constructor stores given workshop in myApp field
  /// and set Waiting status.
  /// \param theId the operation identifier
  /// \param theParent the QObject parent
  ModuleBase_IOperation(const QString& theId = "", QObject* theParent = 0);
  /// Destructor
  virtual ~ModuleBase_IOperation();

  /// Returns the operation description
  /// /returns the instance of the description class
  ModuleBase_OperationDescription* getDescription() const;

  /// Verifies whether this operator can be always started above any already running one
  /// \return Returns TRUE if current operation must not be checked for ActiveOperation->IsValid( this )
  /// This method must be redefined in derived operation if operation of derived class
  /// must be always can start above any launched one. Default impl returns FALSE,
  /// so it is being checked for IsValid, but some operations may overload IsGranted()
  /// In this case they will always start, no matter what operation is running.
  /// \param theOperation the previous running operation
  virtual bool isGranted(ModuleBase_IOperation* theOperation) const;

signals:
  void started(); /// the operation is started
  void aborted(); /// the operation is aborted
  void committed(); /// the operation is committed
  void stopped(); /// the operation is aborted or committed

public slots:
  /// Starts operation
  /// Public slot. Verifies whether operation can be started and starts operation.
  /// This slot is not virtual and cannot be redefined. Redefine startOperation method
  /// to change behavior of operation. There is no point in using this method. It would
  /// be better to inherit own operator from base one and redefine startOperation method
  /// instead.
  void start();
  /// Resumes operation
  /// Public slot. Verifies whether operation can be started and starts operation.
  /// This slot is not virtual and cannot be redefined. Redefine startOperation method
  /// to change behavior of operation. There is no point in using this method. It would
  /// be better to inherit own operator from base one and redefine startOperation method
  /// instead.
  void resume();
  /// Aborts operation
  /// Public slot. Aborts operation. This slot is not virtual and cannot be redefined.
  /// Redefine abortOperation method to change behavior of operation instead
  void abort();
  /// Commits operation
  /// Public slot. Commits operation. This slot is not virtual and cannot be redefined.
  /// Redefine commitOperation method to change behavior of operation instead
  void commit();

  /// Alias for start/abort slots
  /// Public slot. Aborts operation if false, else does nothing.
  /// Provided for S/S compatibility with QAction's toggle(bool)
  /// \param theState th flag to abort, if it is true, do nothing, overwise abort
  void setRunning(bool theState);

  // Data model methods.
  /// Stores a real value in model.
  /// \param theValue - to store
  virtual void storeReal(double theValue) = 0;
  /// Stores a custom value in model.
  virtual void storeCustomValue() = 0;

protected:
  /// Virtual method called when operation started (see start() method for more description)
  /// Default impl calls corresponding slot and commits immediately.
  virtual void startOperation() = 0;
  /// Virtual method called when operation stopped - committed or aborted.
  virtual void stopOperation() = 0;
  /// Virtual method called when operation aborted (see abort() method for more description)
  virtual void abortOperation() = 0;
  /// Virtual method called when operation committed (see commit() method for more description)
  virtual void commitOperation() = 0;

  /// Returns pointer to the root document.
  boost::shared_ptr<ModelAPI_Document> document() const;

private:
  ModuleBase_OperationDescription* myDescription; /// the container to have the operation description
};

#endif
