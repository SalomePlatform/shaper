// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        ModuleBase_WidgetValidated.h
// Created:     12 Mar 2015
// Author:      Natalia ERMOLAEVA


#ifndef MODULEBASE_WIDGETVALIDATED_H_
#define MODULEBASE_WIDGETVALIDATED_H_

#include <ModuleBase.h>
#include <ModuleBase_ModelWidget.h>

#include <GeomAPI_Shape.h>
#include <ModelAPI_Object.h>

#include <SelectMgr_ListOfFilter.hxx>

class QWidget;
class ModuleBase_IWorkshop;
class Config_WidgetAPI;
class Handle_SelectMgr_EntityOwner;

/**
* \ingroup GUI
* Implementation of widget with validators and filters processing.
*/
class MODULEBASE_EXPORT ModuleBase_WidgetValidated : public ModuleBase_ModelWidget
{
  Q_OBJECT
 public:
  /// Constructor
  /// \param theParent the parent object
  /// \param theData the widget configuation. The attribute of the model widget is obtained from
  /// \param theParentId is Id of a parent of the current attribute
  ModuleBase_WidgetValidated(QWidget* theParent,
                             const Config_WidgetAPI* theData,
                             const std::string& theParentId);
  virtual ~ModuleBase_WidgetValidated();

  /// Checks all widget validator if the owner is valid
  /// \param theOwner a selected owner in the view
  /// \return a boolean value
  bool isValid(const Handle_SelectMgr_EntityOwner& theOwner);

protected:
  /// Creates a backup of the current values of the attribute
  /// It should be realized in the specific widget because of different
  /// parameters of the current attribute
  /// \param isBackup a boolean flag, if true, store values from the attribute
  /// to backup, otherwise set the backed up values to the attribute
  virtual void backupAttributeValue(const bool isBackup) = 0;

  /// Fills the attribute with the value of the selected owner
  /// \param theOwner a selected owner
  virtual void setSelection(const Handle_SelectMgr_EntityOwner& theOwner) = 0;

  /// Checks the current attibute in all attribute validators
  // \return true if all validators return that the attribute is valid
  bool isValidAttribute() const;

  /// Check the selected with validators if installed
  /// \param theObj the object for checking
  /// \param theShape the shape for checking
  virtual bool isValid(ObjectPtr theObj, GeomShapePtr theShape) const;

  /// It obtains selection filters from the workshop and activates them in the active viewer
  /// \param theWorkshop an active workshop
  /// \param toActivate a flag about activation or deactivation the filters
  virtual void activateFilters(ModuleBase_IWorkshop* theWorkshop, const bool toActivate) const;

  /// Fills the given list with all widget filters.
  /// \param theWorkshop an active workshop
  /// \param theFilters a list of filters
  void selectionFilters(ModuleBase_IWorkshop* theWorkshop,
                        SelectMgr_ListOfFilter& theFilters) const;
};

#endif /* MODULEBASE_WIDGETVALIDATED_H_ */
