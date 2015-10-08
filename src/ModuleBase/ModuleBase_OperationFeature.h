// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

/*
 * ModuleBase_OperationFeature.h
 *
 *  Created on: Apr 2, 2014
 *      Author: sbh
 */

#ifndef ModuleBase_OperationFeature_H
#define ModuleBase_OperationFeature_H

#include <ModuleBase.h>
#include <ModuleBase_Operation.h>
#include <ModuleBase_ViewerPrs.h>

#include <ModelAPI_Object.h>
#include <ModelAPI_CompositeFeature.h>

#include <QObject>
#include <QString>
#include <QStringList>

#include <set>

class ModuleBase_ModelWidget;
class ModuleBase_ISelection;
class ModuleBase_IViewer;

class QKeyEvent;

/*!
 * \class ModuleBase_OperationFeature
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

class MODULEBASE_EXPORT ModuleBase_OperationFeature : public ModuleBase_Operation
{
Q_OBJECT

 public:

  /// Appends to operation's history id, if it is an "edit" operation (myIsEditing == true)
  static QString EditSuffix() { return "_E"; }
  /// Constructor
  /// \param theId the operation identifier
  /// \param theParent the QObject parent
  ModuleBase_OperationFeature(const QString& theId = "", QObject* theParent = 0);
  /// Destructor
  virtual ~ModuleBase_OperationFeature();

  /// Returns True id the current operation is launched in editing mode
  bool isEditOperation() const { return myIsEditing; }

  /// Returns the operation feature
  /// \return the feature
  FeaturePtr feature() const;

  /// Must return True if the operation's feature is valid.
  /// Since IOperation does not have any feature returns false.
  virtual bool isValid() const;

  /// Sets the operation feature
  void setFeature(FeaturePtr theFeature);

  /// Returns True if the current operation works with the given object (feature or result)
  virtual bool hasObject(ObjectPtr theObj) const;

  /// Returns true if the object is displayed when the operation was started
  /// \param theObject a feature or result of the operation feature
  /// \return boolean value whether the object display state was changed
  virtual bool isDisplayedOnStart(ObjectPtr theObject);

  /// Initialisation of operation with preliminary selection
  /// \param theSelection an instance of Selection class
  /// \param theViewer a viewer to have the viewer the eye position
  virtual void initSelection(ModuleBase_ISelection* theSelection,
                             ModuleBase_IViewer* theViewer);

  /// \brief Set property pane to the operation
  /// \param theProp a property panel instance
  virtual void setPropertyPanel(ModuleBase_IPropertyPanel* theProp);

  /// \return Currently installed property panel
  //ModuleBase_IPropertyPanel* propertyPanel() const { return myPropertyPanel; }

  void setCurrentFeature(const FeaturePtr& theFeature);

  /// Activates widgets by preselection if it is accepted. Emits signal if the activation is correct
  virtual void activateByPreselection();

  /// If the operation works with feature which is sub-feature of another one
  /// then this variable has to be initialised by parent feature 
  /// before operation feature creating
  void setParentFeature(CompositeFeaturePtr theParent);

  /// \return Installed parent feature (can be NULL)
  CompositeFeaturePtr parentFeature() const;

signals:
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

  /// Aborts operation
  /// Public slot. Aborts operation. This slot is not virtual and cannot be redefined.
  /// Redefine abortOperation method to change behavior of operation instead
  void abort();

  /// Commits operation
  /// Public slot. Commits operation. This slot is not virtual and cannot be redefined.
  /// Redefine commitOperation method to change behavior of operation instead
  bool commit();

 protected:
  /// Displays the feature/results if it is hidden. It will be hided in stopOperation
  virtual void startOperation();

  /// Hide feature/results if they were hided on start
  virtual void stopOperation();

  /// Virtual method called after operation resume (see resume() method for more description)
  virtual void resumeOperation();

  /// Creates an operation new feature
  /// \param theFlushMessage the flag whether the create message should be flushed
  /// \returns the created feature
  virtual FeaturePtr createFeature(const bool theFlushMessage = true);

  /// Removes the preselection information and clears the map of preselection
  void clearPreselection();

 protected:
   /// The operation feature to be handled
  FeaturePtr myFeature;

  /// a list of hidden objects, whic are diplayed by operation start
  /// and should be hidden by operation stop
  std::set<ObjectPtr> myVisualizedObjects;

  /// Editing feature flag
  bool myIsEditing;

  /// List of pre-selected object 
  QList<ModuleBase_ViewerPrs> myPreSelection;

  /// If the operation works with feature which is sub-feature of another one
  /// then this variable has to be initialised by parent feature 
  /// before operation feature creating
  CompositeFeaturePtr myParentFeature;

  /// Last current feature before editing operation. It is cashed when Edit operation is started
  /// in order to restore the document current feature on commit/abort this operation.
  FeaturePtr myPreviousCurrentFeature;
};

#endif
