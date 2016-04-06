// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:        XGUI_ModuleConnector.h
// Created:     3 June 2014
// Author:      Vitaly Smetannikov

#ifndef XGUI_ModuleConnector_H
#define XGUI_ModuleConnector_H

#include "XGUI.h"
#include <ModuleBase_Definitions.h>
#include <ModuleBase_IWorkshop.h>

class Handle_AIS_InteractiveContext;
class XGUI_Workshop;
class XGUI_Displayer;

/**
 * \ingroup GUI
 * Implementation of IWorkshop interface which provides access to Workshop services at module level
 */
class XGUI_EXPORT XGUI_ModuleConnector : public ModuleBase_IWorkshop
{
Q_OBJECT
 public:
   /// Constructor
   /// \param theWorkshop a workshop
  XGUI_ModuleConnector(XGUI_Workshop* theWorkshop);

  virtual ~XGUI_ModuleConnector();

  //! Returns list of currently selected data objects
  virtual ModuleBase_ISelection* selection() const;

  /// Activate sub-shapes selection (opens local context if it was not opened)
  /// Types has to be dined according to TopAbs_ShapeEnum
  virtual void activateSubShapesSelection(const QIntList& theTypes);

  /// Activate objects in the module selection modes(opens local context)
  virtual void activateModuleSelectionModes();

  /// Deactivate sub-shapes selection (closes local context)
  virtual void deactivateSubShapesSelection();

  //! Returns instance of loaded module
  virtual ModuleBase_IModule* module() const;

  //! Returns current viewer
  virtual ModuleBase_IViewer* viewer() const;

  //! Returns property panel
  virtual ModuleBase_IPropertyPanel* propertyPanel() const;

  //! Returns currently active operation
  virtual ModuleBase_Operation* currentOperation() const;

  //! Returns true if the operation with id theId can be started
  virtual bool canStartOperation(QString theId);

  //! Returns started operation by the operation identifier. The operation manager is called.
  //! \param theId an operation id
  //! \return an operation instance or NULL
  virtual ModuleBase_Operation* findStartedOperation(const QString& theId);

  //! Returns true if the operation with id theId can be stopped. The operation manager is called.
  //! \param theOperation a stopped operation
  //! \return boolean result
  virtual bool canStopOperation(ModuleBase_Operation* theOperation);

  //! Aborts the operation. The operation manager is called.
  //! \param theOperation an aborted operation
  void abortOperation(ModuleBase_Operation* theOperation);

  //! Returns AIS object by data object
  virtual AISObjectPtr findPresentation(const ObjectPtr& theObject) const;

  //! Returns data object by AIS
  virtual ObjectPtr findPresentedObject(const AISObjectPtr& theAIS) const;

  //! Select features clearing previous selection. 
  //! If the list is empty then selection will be cleared
  virtual void setSelected(const QList<std::shared_ptr<ModuleBase_ViewerPrs>>& theValues);

   /// Update of commands status
  virtual void updateCommandStatus();

  //! Returns workshop
  XGUI_Workshop* workshop() const { return myWorkshop; }

private:
  QObjectPtrList activeObjects(const QObjectPtrList& theObjList) const;

  /// Reference to workshop
  XGUI_Workshop* myWorkshop;
};

#endif
