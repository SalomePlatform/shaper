// File:        ModuleBase_IWorkshop.h
// Created:     2 June 2014
// Author:      Vitaly Smetannikov

#ifndef ModuleBase_IWorkshop_H
#define ModuleBase_IWorkshop_H

#include "ModuleBase.h"
#include "ModuleBase_Definitions.h"

#include <ModelAPI_Object.h>
#include <GeomAPI_AISObject.h>

#include <QObject>

class ModuleBase_IModule;
class ModuleBase_ISelection;
class ModuleBase_IViewer;
class ModuleBase_Operation;

/**
 * Class which provides access to Workshop object serveces
 */
class MODULEBASE_EXPORT ModuleBase_IWorkshop : public QObject
{
Q_OBJECT
 public:
  ModuleBase_IWorkshop(QObject* theParent)
      : QObject(theParent)
  {}

  virtual ~ModuleBase_IWorkshop()
  {}

  virtual ModuleBase_ISelection* selection() const = 0;

  /// Activate sub-shapes selection (opens local context)
  /// Types has to be dined according to TopAbs_ShapeEnum
  virtual void activateSubShapesSelection(const QIntList& theTypes) = 0;

  /// Deactivate sub-shapes selection (closes local context)
  virtual void deactivateSubShapesSelection() = 0;

  //! Returns instance of loaded module
  virtual ModuleBase_IModule* module() const = 0;

  //! Returns current viewer
  virtual ModuleBase_IViewer* viewer() const = 0;

  //! Returns currently active operation
  virtual ModuleBase_Operation* currentOperation() const = 0;

  //! Returns true if the operation with id theId can be started
  virtual bool canStartOperation(QString theId) = 0;

  //! Returns AIS opbject by data object
  virtual AISObjectPtr findPresentation(const ObjectPtr& theObject) const = 0;

  //! Returns data object by AIS
  virtual ObjectPtr findPresentedObject(const AISObjectPtr& theAIS) const = 0;

  //! Select features clearing previous selection. 
  //! If the list is empty then selection will be cleared
  virtual void setSelected(const QObjectPtrList& theFeatures) = 0;

signals:
  void selectionChanged();

  /// Signal about an operation is started. It is emitted after the start() of operation is done.
  void operationStarted(ModuleBase_Operation* theOperation);

  /// Signal about an operation is stopped. It is emitted after the stop() of operation is done.
  /// \param theOperation a stopped operation
  void operationStopped(ModuleBase_Operation* theOperation);

  /// Signal about an operation is resumed. It is emitted after the resume() of operation is done.
  void operationResumed(ModuleBase_Operation* theOperation);

  /// Emitted when current operation is comitted
  void operationComitted(ModuleBase_Operation* theOperation);

  /// Emitted when current operation is aborted
  void operationAborted(ModuleBase_Operation* theOperation);

  /// Signal which is emited after activation of property panel
  void propertyPanelActivated();

};

#endif
