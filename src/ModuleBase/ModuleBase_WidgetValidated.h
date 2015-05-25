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
class ModuleBase_ISelection;
class ModelAPI_Validator;
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
  bool isValidSelection(const ModuleBase_ViewerPrs& theValue);

  /// Set the given wrapped value to the current widget
  /// This value should be processed in the widget according to the needs
  /// The method is called by the current operation to process the operation preselection.
  /// It is redefined to check the value validity and if it is, fill the attribute with by value
  /// \param theValues the wrapped selection values
  /// \param thePosition an index in the list of values, the values should be get from the index
  virtual bool setSelection(const QList<ModuleBase_ViewerPrs>& theValues, int& thePosition);

protected:
  /// Creates a backup of the current values of the attribute
  /// It should be realized in the specific widget because of different
  /// parameters of the current attribute
  virtual void storeAttributeValue() = 0;

  /// Creates a backup of the current values of the attribute
  /// It should be realized in the specific widget because of different
  /// parameters of the current attribute
  /// \param theValid a boolean flag, if restore happens for valid parameters
  virtual void restoreAttributeValue(const bool theValid) = 0;

  /// Fills the attribute with the value of the selected owner
  /// \param theOwner a selected owner
  virtual bool setSelectionCustom(const ModuleBase_ViewerPrs& thePrs) = 0;

  virtual void removePresentations() {};

  /// Checks the current attibute in all attribute validators
  // \return true if all validators return that the attribute is valid
  bool isValidAttribute() const;

  /// It obtains selection filters from the workshop and activates them in the active viewer
  /// \param theWorkshop an active workshop
  /// \param toActivate a flag about activation or deactivation the filters
  virtual void activateFilters(ModuleBase_IWorkshop* theWorkshop, const bool toActivate) const;

  /// Puts additional validators to the given list
  /// \param theValidators a list of validators
  /// \param theArguments a list of validators arguments
  virtual void customValidators(std::list<ModelAPI_Validator*>& theValidators,
                                std::list<std::list<std::string> >& theArguments) const;

  /// Returns a list of selected presentations. Firstly it is obtained from the viewer,
  /// if there are not selected objects in the viewer, it get the selection from the object browser.
  /// If the browser has selected objects, the viewer prs objects are created with only object
  /// field of the presentation initialized. The widget should accept the selection in the object
  /// browser at the same way as in the viewer.
  /// \param theSelection a selection, where the selected objects and presentations are found
  /// \return a list of presentations
  QList<ModuleBase_ViewerPrs> getSelectedEntitiesOrObjects(ModuleBase_ISelection* theSelection) const;

};

#endif /* MODULEBASE_WIDGETVALIDATED_H_ */
