// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        ModuleBase_WidgetValidator.h
// Created:     12 Mar 2015
// Author:      Natalia ERMOLAEVA


#ifndef ModuleBase_WidgetValidator_H_
#define ModuleBase_WidgetValidator_H_

#include <ModuleBase.h>

#include <QList>
#include <memory>

class ModuleBase_ModelWidget;
class ModuleBase_IWorkshop;
class ModuleBase_ViewerPrs;

/**
* \ingroup GUI
* Implementation of widget with validators and filters processing.
*/
class MODULEBASE_EXPORT ModuleBase_WidgetValidator
{
 public:
  /// Constructor
  /// \param theModelWidget the model widget to be validated
  /// \param theWorkshop the current workshop
  ModuleBase_WidgetValidator(ModuleBase_ModelWidget* theModelWidget,
                             ModuleBase_IWorkshop* theWorkshop);
  virtual ~ModuleBase_WidgetValidator();

  /// Checks all widget validator if the owner is valid. Firstly it checks custom widget validating,
  /// next, the attribute's validating. It trying on the give selection to current attribute by
  /// setting the value inside and calling validators. After this, the previous attribute value is
  /// restored.The valid/invalid value is cashed.
  /// \param theValue a selected presentation in the view
  /// \return a boolean value
  virtual bool isValidSelection(const std::shared_ptr<ModuleBase_ViewerPrs>& theValue);

  /// It obtains selection filters from the workshop and activates them in the active viewer
  /// \param toActivate a flag about activation or deactivation the filters
  /// \return true if the selection filter of the widget is activated in viewer context
  bool activateFilters(const bool toActivate);

private:
  /// Returns true if the workshop validator filter has been already activated
  /// \return boolean value
  bool isFilterActivated() const;

  //! Clear all validated cash in the widget
  void clearValidatedCash();

  /// Gets the validity state of the presentation in an internal map. Returns true if the valid state of value is stored
  /// \param theValue a viewer presentation
  /// \param theValid a valid state
  bool getValidState(const std::shared_ptr<ModuleBase_ViewerPrs>& theValue, bool& theValid);

  /// Store the validity state of the presentation in an internal map
  /// \param theValue a viewer presentation
  /// \param theValid a valid state
  void storeValidState(const std::shared_ptr<ModuleBase_ViewerPrs>& theValue, const bool theValid);

protected:
  /// Reference to workshop
  ModuleBase_ModelWidget* myModelWidget; ///< the current widget to be validated
  ModuleBase_IWorkshop* myWorkshop; ///< the active workshop

  QList<std::shared_ptr<ModuleBase_ViewerPrs>> myValidPrs; /// cash of valid selection presentations
  QList<std::shared_ptr<ModuleBase_ViewerPrs>> myInvalidPrs; /// cash of invalid selection presentations
};

#endif /* ModuleBase_WidgetValidator_H_ */
