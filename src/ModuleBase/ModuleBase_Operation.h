/*
 * ModuleBase_Operation.h
 *
 *  Created on: Apr 2, 2014
 *      Author: sbh
 */

#ifndef ModuleBase_Operation_H
#define ModuleBase_Operation_H

#include <ModuleBase.h>
#include <ModuleBase_ViewerPrs.h>

#include <ModelAPI_Feature.h>
#include <ModelAPI_Document.h>

#include <QObject>
#include <QString>
#include <QStringList>

class ModuleBase_ModelWidget;
class ModuleBase_OperationDescription;
class ModuleBase_IPropertyPanel;

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

  /**
  * Must return true if this operation can be launched as nested for any current operation
  * and it is not necessary to check this operation on validity. By default 
  * the operation is not granted.
  * The method has to be redefined for granted operations.
  */
  virtual bool isGranted(ModuleBase_Operation* theOperation) const  { return false; }

  /// Sets a list of model widgets, according to the operation feature xml definition
  /// \param theXmlRepresentation an xml feature definition
  /// \param theWidgets a list of widgets
  //void setModelWidgets(const std::string& theXmlRepresentation,
  //                     QList<ModuleBase_ModelWidget*> theWidgets);

  /// Returns True if data of its feature was modified during operation
  virtual bool isModified() const { return myIsModified; }

  /// Returns True id the current operation is launched in editing mode
  bool isEditOperation() const { return myIsEditing; }

  /// Returns list of nested features
  QStringList nestedFeatures() const { return myNestedFeatures; }

  /// Sets list of nested features
  void setNestedFeatures(const QStringList& theList) { myNestedFeatures = theList; }


  // Returns operations Id from it's description
  QString id() const;

  /// Returns the operation feature
  /// \return the feature
  FeaturePtr feature() const;

  /**
  * Must return True if the operation's feature is valid.
  * Since IOperation does not have any feature returns false.
  */
  virtual bool isValid() const;

  /// Returns whether the nested operations are enabled.
  /// The state can depend on the operation current state.
  /// \return enabled state
  virtual bool isNestedOperationsEnabled() const;

  /// Sets the operation feature
  void setFeature(FeaturePtr theFeature);

  /// Returns True if the current operation works with the given object (feature or result)
  virtual bool hasObject(ObjectPtr theObj) const;

  virtual void keyReleased(const int theKey) {};

  /// If operation needs to redisplay its result during operation
  /// then this method has to return True
  virtual bool hasPreview() const { return false; }

  /// Initialisation of operation with preliminary selection
  /// \param theSelected the list of selected presentations
  /// \param theHighlighted the list of highlighted presentations
  virtual void initSelection(const std::list<ModuleBase_ViewerPrs>& theSelected,
                             const std::list<ModuleBase_ViewerPrs>& theHighlighted);

  virtual void setPropertyPanel(ModuleBase_IPropertyPanel* theProp);

  ModuleBase_IPropertyPanel* propertyPanel() const { return myPropertyPanel; }

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
  virtual void storeCustomValue();

  /// Slots which listen the mode widget activation
  /// \param theWidget the model widget
  virtual void onWidgetActivated(ModuleBase_ModelWidget* theWidget);

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

  /// Verifies whether this operator can be commited.
  /// \return Returns TRUE if current operation can be committed, e.g. all parameters are filled
  virtual bool canBeCommitted() const;

  /// Returns pointer to the root document.
  boost::shared_ptr<ModelAPI_Document> document() const;

  ///
  virtual void activateByPreselection();

  /// Set value to the active widget
  /// \param theFeature the feature
  /// \param theX the horizontal coordinate
  /// \param theY the vertical coordinate
  /// \return true if the point is set
  virtual bool setWidgetValue(ObjectPtr theFeature, double theX, double theY);

 protected:
  FeaturePtr myFeature;  /// the operation feature to be handled

  /// the container to have the operation description
  ModuleBase_OperationDescription* myDescription;  

  /// Editing feature flag
  bool myIsEditing;

  /// Modified feature flag
  bool myIsModified;

  /// List of nested operations IDs
  QStringList myNestedFeatures;

  /// List of pre-selected object 
  std::list<ModuleBase_ViewerPrs> myPreSelection;

  /// Access to property panel
  ModuleBase_IPropertyPanel* myPropertyPanel;
};

#endif
