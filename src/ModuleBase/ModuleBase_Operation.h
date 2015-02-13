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
#include <ModuleBase_ViewerPrs.h>

#include <ModelAPI_CompositeFeature.h>
#include <ModelAPI_Document.h>

#include <QObject>
#include <QString>
#include <QStringList>

class ModuleBase_ModelWidget;
class ModuleBase_OperationDescription;
class ModuleBase_IPropertyPanel;
class ModuleBase_ISelection;
class ModuleBase_IViewer;

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

  /// Appends to operation's history id, if it is an "edit" operation (myIsEditing == true)
  static QString EditSuffix() { return "_E"; }
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
  virtual bool isGranted(QString theId) const;


  /// Returns True if data of its feature was modified during operation
  virtual bool isModified() const { return myIsModified; }

  /// Returns True id the current operation is launched in editing mode
  bool isEditOperation() const { return myIsEditing; }

  /// Returns list of nested features
  QStringList nestedFeatures() const { return myNestedFeatures; }

  /// Sets list of nested features
  void setNestedFeatures(const QStringList& theList) { myNestedFeatures = theList; }


  /// Returns operations Id from it's description
  QString id() const;

  /// Returns the operation feature
  /// \return the feature
  FeaturePtr feature() const;

  /**
  * Must return True if the operation's feature is valid.
  * Since IOperation does not have any feature returns false.
  */
  virtual bool isValid() const;

  /// Sets the operation feature
  void setFeature(FeaturePtr theFeature);

  /// Returns True if the current operation works with the given object (feature or result)
  virtual bool hasObject(ObjectPtr theObj) const;

  /// Initialisation of operation with preliminary selection
  /// \param theSelection an instance of Selection class
  /// \param theViewer a viewer to have the viewer the eye position
  virtual void initSelection(ModuleBase_ISelection* theSelection,
                             ModuleBase_IViewer* theViewer);

  /// \brief Set property pane to the operation
  /// \param theProp a property panel instance
  virtual void setPropertyPanel(ModuleBase_IPropertyPanel* theProp);

  /// \return Currently installed property panel
  ModuleBase_IPropertyPanel* propertyPanel() const { return myPropertyPanel; }

  /// Activates widgets by preselection if it is accepted. Emits signal if the activation is correct
  virtual void activateByPreselection();

  /// If the operation works with feature which is sub-feature of another one
  /// then this variable has to be initialised by parent feature 
  /// before operation feature creating
  void setParentFeature(CompositeFeaturePtr theParent);

  /// \return Installed parent feature (can be NULL)
  CompositeFeaturePtr parentFeature() const;

signals:
  /// The operation is started
  void started();  

  /// The operation is aborted
  void aborted();  

  /// The operation is committed
  void committed();  

  /// The operation is aborted or committed
  void stopped();  

  /// The operation is resumed
  void resumed();  

  /// The operation is postponed
  void postponed();  

  /// The operation is filled with existing preselection
  void activatedByPreselection(); 

 public slots:
  /// Starts operation
  /// Public slot. Verifies whether operation can be started and starts operation.
  /// This slot is not virtual and cannot be redefined. Redefine startOperation method
  /// to change behavior of operation. There is no point in using this method. It would
  /// be better to inherit own operator from base one and redefine startOperation method
  /// instead.
  void start();

  /// Deactivates current operation which can be resumed later.
  void postpone();

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
  virtual void commitOperation() {}

  /// Virtual method called after operation committed (see commit() method for more description)
  virtual void afterCommitOperation() {}

  /// Virtual method called after operation resume (see resume() method for more description)
  virtual void resumeOperation() {}

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
  std::shared_ptr<ModelAPI_Document> document() const;

  /// Return a widget value point by the selection and the viewer position
  /// The default realization returns false
  /// \param thePrs the presentation
  /// \param theViewer a viewer to have the viewer the eye position
  /// \param theX the horizontal coordinate
  /// \param theY the vertical coordinate
  /// \return true if the point exits in the selection
  virtual bool getViewerPoint(ModuleBase_ViewerPrs thePrs,
                                   ModuleBase_IViewer* theViewer,
                                   double& theX, double& theY);

  /// Removes the preselection information and clears the map of preselection
  void clearPreselection();

 protected:
   /// The operation feature to be handled
  FeaturePtr myFeature;  

  /// the container to have the operation description
  ModuleBase_OperationDescription* myDescription;  

  /// Editing feature flag
  bool myIsEditing;

  /// Modified feature flag
  bool myIsModified;

  /// List of nested operations IDs
  QStringList myNestedFeatures;

  /// List of pre-selected object 
  QList<ModuleBase_ViewerPrs> myPreSelection;

  /// Access to property panel
  ModuleBase_IPropertyPanel* myPropertyPanel;

  /// If the operation works with feature which is sub-feature of another one
  /// then this variable has to be initialised by parent feature 
  /// before operation feature creating
  CompositeFeaturePtr myParentFeature;  

};

#endif
