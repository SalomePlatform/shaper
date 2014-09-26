/*
 * ModuleBase_Operation.h
 *
 *  Created on: Apr 2, 2014
 *      Author: sbh
 */

#ifndef ModuleBase_Operation_H
#define ModuleBase_Operation_H

#include <ModuleBase.h>
#include <ModuleBase_IOperation.h>

#include <ModelAPI_Feature.h>

#include <QObject>
#include <QString>

#include <boost/shared_ptr.hpp>

class ModelAPI_Document;
class ModuleBase_ModelWidget;

class QKeyEvent;

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

class MODULEBASE_EXPORT ModuleBase_Operation : public ModuleBase_IOperation
{
Q_OBJECT

 public:
  /// Constructor
  /// \param theId the operation identifier
  /// \param theParent the QObject parent
  ModuleBase_Operation(const QString& theId = "", QObject* theParent = 0);
  /// Destructor
  virtual ~ModuleBase_Operation();

  // Returns operations Id from it's description
  QString id() const;
  /// Returns the operation feature
  /// \return the feature
  FeaturePtr feature() const;

  /// Returns true is feature of operation is valid.
  virtual bool isValid() const;

  /// Returns whether the nested operations are enabled.
  /// The state can depend on the operation current state.
  /// \return enabled state
  virtual bool isNestedOperationsEnabled() const;

  // Data model methods.
  /// Stores a custom value in model.
  void storeCustomValue();

  /// Sets the operation feature
  void setEditingFeature(FeaturePtr theFeature);

  /// Returns True if the current operation works with the given object (feature or result)
  virtual bool hasObject(ObjectPtr theObj) const;

  virtual void keyReleased(const int theKey) {};

  virtual void activateNextToCurrentWidget() {};

  /// If operation needs to redisplay its result during operation
  /// then this method has to return True
  virtual bool hasPreview() const { return false; }

 public slots:
  /// Slots which listen the mode widget activation
  /// \param theWidget the model widget
  virtual void onWidgetActivated(ModuleBase_ModelWidget* theWidget);

signals:
  /// Signals about the activating of the next widget
  /// \param theWidget the previous active widget
  void activateNextWidget(ModuleBase_ModelWidget* theWidget);

 protected:
  /// Virtual method called when operation started (see start() method for more description)
  /// Default impl calls corresponding slot and commits immediately.
  virtual void startOperation();
  /// Virtual method called when operation stopped - committed or aborted.
  virtual void stopOperation();
  /// Virtual method called when operation aborted (see abort() method for more description)
  virtual void abortOperation();
  /// Virtual method called when operation committed (see commit() method for more description)
  virtual void commitOperation();
  /// Virtual method called after operation committed (see commit() method for more description)
  virtual void afterCommitOperation();

  /// Send update message by loop
  void flushUpdated();
  /// Send created message by loop
  void flushCreated();

  /// Creates an operation new feature
  /// \param theFlushMessage the flag whether the create message should be flushed
  /// \returns the created feature
  virtual FeaturePtr createFeature(const bool theFlushMessage = true);

 protected:
  /// Sets the operation feature
  void setFeature(FeaturePtr theFeature);

  /// Verifies whether this operator can be commited.
  /// \return Returns TRUE if current operation can be committed, e.g. all parameters are filled
  virtual bool canBeCommitted() const;

 protected:
  FeaturePtr myFeature;  /// the operation feature to be handled
};

#endif
