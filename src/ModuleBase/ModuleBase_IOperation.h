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
#include <QList>
#include <QStringList>

#include <boost/shared_ptr.hpp>

class ModelAPI_Document;
class ModuleBase_OperationDescription;
//class ModuleBase_ModelWidget;

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

class MODULEBASE_EXPORT ModuleBase_IOperation : public QObject
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

  /**
  * Must return true if this operation can be launched as nested for any current operation
  * and it is not necessary to check this operation on validity. By default 
  * the operation is not granted.
  * The method has to be redefined for granted operations.
  */
  virtual bool isGranted() const  { return false; }

  /**
  * Must return True if the operation's feature is valid.
  * Since IOperation does not have any feature returns false.
  */
  virtual bool isValid() const { return false; }

  /// Sets a list of model widgets, according to the operation feature xml definition
  /// \param theXmlRepresentation an xml feature definition
  /// \param theWidgets a list of widgets
  //void setModelWidgets(const std::string& theXmlRepresentation,
  //                     QList<ModuleBase_ModelWidget*> theWidgets);

  /// Returns True if data of its feature was modified during operation
  virtual bool isModified() const
  {
    return myIsModified;
  }

  /// Returns True id the current operation is launched in editing mode
  bool isEditOperation() const
  {
    return myIsEditing;
  }

  /// Returns list of nested features
  QStringList nestedFeatures() const { return myNestedFeatures; }

  /// Sets list of nested features
  void setNestedFeatures(const QStringList& theList) { myNestedFeatures = theList; }

signals:
  void started();  /// the operation is started
  void aborted();  /// the operation is aborted
  void committed();  /// the operation is committed
  void stopped();  /// the operation is aborted or committed
  void resumed();  /// the operation is resumed

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
  bool commit();

  /// Alias for start/abort slots
  /// Public slot. Aborts operation if false, else does nothing.
  /// Provided for S/S compatibility with QAction's toggle(bool)
  /// \param theState th flag to abort, if it is true, do nothing, overwise abort
  void setRunning(bool theState);

  // Data model methods.
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

  /// Virtual method called after operation committed (see commit() method for more description)
  /// it is important that the method is called after the stop() signal is emitted
  virtual void afterCommitOperation() = 0;

  /// Verifies whether this operator can be commited.
  /// \return Returns TRUE if current operation can be committed, e.g. all parameters are filled
  virtual bool canBeCommitted() const;

  /// Returns pointer to the root document.
  boost::shared_ptr<ModelAPI_Document> document() const;

  /// Editing feature flag
  bool myIsEditing;

  /// Modified feature flag
  bool myIsModified;

 private:
  ModuleBase_OperationDescription* myDescription;  /// the container to have the operation description

  QStringList myNestedFeatures;
};

#endif
