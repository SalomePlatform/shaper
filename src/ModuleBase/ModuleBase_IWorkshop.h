// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        ModuleBase_IWorkshop.h
// Created:     2 June 2014
// Author:      Vitaly Smetannikov

#ifndef ModuleBase_IWorkshop_H
#define ModuleBase_IWorkshop_H

#include "ModuleBase.h"
#include "ModuleBase_Definitions.h"
#include <ModuleBase_FilterValidated.h>

#include <ModelAPI_Object.h>
#include <GeomAPI_AISObject.h>
#include <Config_FeatureMessage.h>

#include <QObject>

class ModuleBase_IModule;
class ModuleBase_ISelection;
class ModuleBase_IViewer;
class ModuleBase_IPropertyPanel;
class ModuleBase_Operation;
class ModuleBase_FilterFactory;
class ModuleBase_ViewerPrs;
class QMainWindow;

/**
 * \ingroup GUI
 * Class which provides access to Workshop object services
 */
class MODULEBASE_EXPORT ModuleBase_IWorkshop : public QObject
{
Q_OBJECT
 public:
   /// Constructor
   /// \param theParent parent object
  ModuleBase_IWorkshop(QObject* theParent);

  virtual ~ModuleBase_IWorkshop()
  {}

  /// Return current selection instance
  virtual ModuleBase_ISelection* selection() const = 0;

  /// Activate sub-shapes selection (opens local context)
  /// Types has to be defined according to TopAbs_ShapeEnum
  virtual void activateSubShapesSelection(const QIntList& theTypes) = 0;

  /// Activate objects in the module selection modes(opens local context)
  virtual void activateModuleSelectionModes() = 0;

  /// Deactivate sub-shapes selection (closes local context)
  virtual void deactivateSubShapesSelection() = 0;

  //! Returns instance of loaded module
  virtual ModuleBase_IModule* module() const = 0;

  //! Returns current viewer
  virtual ModuleBase_IViewer* viewer() const = 0;

  //! Returns property panel
  virtual ModuleBase_IPropertyPanel* propertyPanel() const = 0;

  /// A filter to process an attribute validators
  /// \return a filter
  Handle(ModuleBase_FilterValidated) validatorFilter();

  //! Returns the factory of selection filters : the only one instance per application
  ModuleBase_FilterFactory* selectionFilters() const;
  
  //! Returns currently active operation
  virtual ModuleBase_Operation* currentOperation() const = 0;

  //! Returns true if the operation with id theId can be started
  virtual bool canStartOperation(QString theId) = 0;

  //! Returns started operation by the operation identifier
  //! \param theId an operation id
  //! \return an operation instance or NULL
  virtual ModuleBase_Operation* findStartedOperation(const QString& theId) = 0;

  //! Returns true if the operation with id theId can be stopped
  //! \param theOperation a stopped operation
  //! \return boolean result
  virtual bool canStopOperation(ModuleBase_Operation* theOperation) = 0;

  //! Aborts the operation.
  //! \param theOperation an aborted operation
  virtual void abortOperation(ModuleBase_Operation* theOperation) = 0;

  //! Returns AIS object by data object
  //! \param theObject a data object
  virtual AISObjectPtr findPresentation(const ObjectPtr& theObject) const = 0;

  //! Returns data object by AIS
  //! \param theAIS a presentation
  virtual ObjectPtr findPresentedObject(const AISObjectPtr& theAIS) const = 0;

  //! Select features clearing previous selection. 
  //! If the list is empty then selection will be cleared
  //! \param theValues a list of presentations
  virtual void setSelected(const QList<std::shared_ptr<ModuleBase_ViewerPrs>>& theValues) = 0;

   /// Update of commands status
  virtual void updateCommandStatus() = 0;

  virtual std::shared_ptr<Config_FeatureMessage> featureInfo(const QString& theId) const = 0;

  virtual QMainWindow* desktop() const = 0;


signals:
  /// Signal selection changed.
  void selectionChanged();

  /// Signal which is emited after activation of property panel
  void propertyPanelActivated();

protected:
  /// A filter to process an attribute validators
  Handle(ModuleBase_FilterValidated) myValidatorFilter;
};

#endif
