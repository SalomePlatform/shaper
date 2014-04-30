/*
 * ModuleBase_Operation.h
 *
 *  Created on: Apr 2, 2014
 *      Author: sbh
 */


#ifndef MODULEBASE_OPERATION_H
#define MODULEBASE_OPERATION_H

#include <ModuleBase.h>

#include <QObject>
#include <QString>

#include <boost/shared_ptr.hpp>

class SUIT_Study;
class XGUI_Workshop;
class ModelAPI_Feature;
class ModelAPI_Document;

/*!
 \class ModuleBase_Operation
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

class MODULEBASE_EXPORT ModuleBase_Operation: public QObject
{
Q_OBJECT

public:
  /*! Enum describes state of operation */
  enum OperationState
  {
    Waiting,  //!< Operation is not used (it is not run or suspended)
    Running  //!< Operation is started
  };

  /*!
   * Enum describes execution status of operation. Execution status often used after
   * ending work of operation which was started from this one. In this case this
   * operation can ask previously started operation whether it finished successfully.
   */
  enum ExecStatus
  {
    Rejected, //!< Operation has not performed any action (modification of data model for example)
    Accepted  //!< Operation has performed an actions and must be stopped
  };

  /*!
   * Enum describes setting of the operation.
   */
  enum Flags
  {
    None = 0x00, //!< None options
    Transaction = 0x01 //!< Automatically open (commit/abort) transaction during start (commit/abort).
  };

public:
  ModuleBase_Operation(const QString& theId = "", QObject* parent = 0);
  virtual ~ModuleBase_Operation();

  // Operation processing.
  virtual QString operationId() const;

  boost::shared_ptr<ModelAPI_Feature> feature() const;

  OperationState state() const;
  bool isRunning() const;
  virtual bool isValid(ModuleBase_Operation* theOtherOp) const;
  virtual bool isGranted() const;

  bool setSlot(const QObject* theReceiver, const char* theSlot);

  void setFlags(const int);
  void clearFlags(const int);
  bool testFlags(const int) const;

  int execStatus() const;

signals:
  void started();
  void aborted();
  void committed();
  void stopped(); //!< operation aborted or committed

  void callSlot();

public slots:
  void start();
  void resume();
  void abort();
  void commit();

  //true = do nothing, false = abort()
  //Provided for S/S compatibility with QAction's toggle(bool)
  void setRunning(bool);

  // Data model operations.
  void storeReal(double);

  // Data model operations.
  void storeCustomValue();

protected:
  virtual bool isReadyToStart() const;

  virtual void startOperation();
  virtual void stopOperation();
  virtual void abortOperation();
  virtual void commitOperation();

  void setExecStatus(const int);
  void setState(const OperationState);

  boost::shared_ptr<ModelAPI_Document> document() const;

private:
  int myFlags;               //!< Operation flags
  OperationState myState;    //!< Operation state
  ExecStatus myExecStatus;   //!< Execution status

  //!< Next fields could be extracted into a subclass;
  QString myOperationId;
  boost::shared_ptr<ModelAPI_Feature> myFeature;
};

#endif
