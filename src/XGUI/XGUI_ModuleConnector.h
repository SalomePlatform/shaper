// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:        XGUI_ModuleConnector.h
// Created:     3 June 2014
// Author:      Vitaly Smetannikov

#ifndef XGUI_ModuleConnector_H
#define XGUI_ModuleConnector_H

#include "XGUI.h"
#include <ModuleBase_Definitions.h>
#include <ModuleBase_IWorkshop.h>
#include <ModuleBase_ViewerFilters.h>

class Handle_AIS_InteractiveContext;
class XGUI_Workshop;
class XGUI_Displayer;

/**
 * \ingroup GUI
 * Implementation of IWorkshop interface which provides access to Workshop sevices at module level
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

  //! Returns AIS opbject by data object
  virtual AISObjectPtr findPresentation(const ObjectPtr& theObject) const;

  //! Returns data object by AIS
  virtual ObjectPtr findPresentedObject(const AISObjectPtr& theAIS) const;

  //! Select features clearing previous selection. 
  //! If the list is empty then selection will be cleared
  virtual void setSelected(const QObjectPtrList& theFeatures);

  //! Returns workshop
  XGUI_Workshop* workshop() const { return myWorkshop; }

private:
  /// Reference to workshop
  XGUI_Workshop* myWorkshop;

  /// A filter which provides selection within a current document or whole PartSet
  Handle(ModuleBase_ShapeDocumentFilter) myDocumentShapeFilter;
};

#endif
