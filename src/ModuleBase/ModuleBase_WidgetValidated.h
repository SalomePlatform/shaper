// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        ModuleBase_WidgetValidated.h
// Created:     12 Mar 2015
// Author:      Natalia ERMOLAEVA


#ifndef MODULEBASE_WIDGETVALIDATED_H_
#define MODULEBASE_WIDGETVALIDATED_H_

#include <ModuleBase.h>
#include <ModuleBase_ModelWidget.h>

#include <GeomAPI_Shape.h>
#include <GeomAPI_AISObject.h>
#include <ModelAPI_Object.h>

#include <SelectMgr_ListOfFilter.hxx>

#include <QList>

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
  /// \param theWorkshop a reference to workshop
  /// \param theData the widget configuation. The attribute of the model widget is obtained from
  /// \param theParentId is Id of a parent of the current attribute
  ModuleBase_WidgetValidated(QWidget* theParent,
                             ModuleBase_IWorkshop* theWorkshop,
                             const Config_WidgetAPI* theData,
                             const std::string& theParentId);
  virtual ~ModuleBase_WidgetValidated();

  /// Checks whether all active viewer filters validate the presentation
  /// \param thePrs a selected presentation in the view
  /// \return a boolean value
  bool isValidInFilters(const ModuleBase_ViewerPrs& thePrs);

  /// Checks all widget validator if the owner is valid
  /// \param theValue a selected presentation in the view
  /// \return a boolean value
  bool isValidSelection(const ModuleBase_ViewerPrs& theValue);

  /// Set the given wrapped value to the current widget
  /// This value should be processed in the widget according to the needs
  /// The method is called by the current operation to process the operation preselection.
  /// It is redefined to check the value validity and if it is, fill the attribute with by value
  /// \param theValues the wrapped selection values
  /// \param theToValidate a flag on validation of the values
  virtual bool setSelection(QList<ModuleBase_ViewerPrs>& theValues,
                            const bool theToValidate);

  //! Returns data object by AIS
  ObjectPtr findPresentedObject(const AISObjectPtr& theAIS) const;

protected:
  /// Creates a backup of the current values of the attribute
  /// It should be realized in the specific widget because of different
  /// parameters of the current attribute
  virtual void storeAttributeValue();

  /// Creates a backup of the current values of the attribute
  /// It should be realized in the specific widget because of different
  /// parameters of the current attribute
  /// \param theValid a boolean flag, if restore happens for valid parameters
  virtual void restoreAttributeValue(const bool theValid);

  /// Checks the widget validity. By default, it returns true.
  /// \param thePrs a selected presentation in the view
  /// \return a boolean value
  virtual bool isValidSelectionCustom(const ModuleBase_ViewerPrs& thePrs);

  /// Fills the attribute with the value of the selected owner
  /// \param thePrs a selected owner
  virtual bool setSelectionCustom(const ModuleBase_ViewerPrs& thePrs) = 0;

  /// Checks the current attibute in all attribute validators
  // \return true if all validators return that the attribute is valid
  bool isValidAttribute() const;

  /// Returns true if the workshop validator filter has been already activated
  /// \return boolean value
  bool isFilterActivated() const;

  /// Gets the validity state of the presentation in an internal map. Returns true if the valid state of value is stored
  /// \param theValue a viewer presentation
  /// \param theValid a valid state
  bool getValidState(const ModuleBase_ViewerPrs& theValue, bool& theValid);

  /// Store the validity state of the presentation in an internal map
  /// \param theValue a viewer presentation
  /// \param theValid a valid state
  void storeValidState(const ModuleBase_ViewerPrs& theValue, const bool theValid);

  /// Removes all presentations from internal maps.
  void clearValidState();

  /// Returns a list of selected presentations in the viewer and object browser
  /// The presentations from the object browser are filtered by the AIS context filters
  /// \return a list of presentations
  QList<ModuleBase_ViewerPrs> getFilteredSelected();

  /// Applies AIS context filters to the parameter list. The not approved presentations are
  /// removed from the parameters.
  /// \param theValues a list of presentations.
  void filterPresentations(QList<ModuleBase_ViewerPrs>& theValues);

  /// It obtains selection filters from the workshop and activates them in the active viewer
  /// \param toActivate a flag about activation or deactivation the filters
  void activateFilters(const bool toActivate);

protected:
  /// Reference to workshop
  ModuleBase_IWorkshop* myWorkshop; 

  /// The widget is in validation mode: store is performed, restore is not
  bool myIsInValidate; 

private:
  ObjectPtr myPresentedObject; /// back up of the filtered object
  QList<ModuleBase_ViewerPrs> myValidPrs;
  QList<ModuleBase_ViewerPrs> myInvalidPrs;
};

#endif /* MODULEBASE_WIDGETVALIDATED_H_ */
