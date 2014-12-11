// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

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
class ModuleBase_FilterFactory;

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

  //! Returns the factory of selection filters : the only one instance per application
  ModuleBase_FilterFactory* selectionFilters() const;
  
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

  /// Signal which is emited after activation of property panel
  void propertyPanelActivated();

};

#endif
