// Copyright (C) 2014-2020  CEA/DEN, EDF R&D
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

#ifndef ModuleBase_WidgetValidator_H_
#define ModuleBase_WidgetValidator_H_

#include "ModuleBase.h"

#include "ModuleBase_Definitions.h"

#include <SelectMgr_ListOfFilter.hxx>

#include <QList>
#include <memory>

class ModelAPI_Attribute;

class ModuleBase_ModelWidget;
class ModuleBase_IWorkshop;
class ModuleBase_ViewerPrs;
class ModuleBase_WidgetSelectorStore;

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

  /// Fills given container with selection modes if the widget has it
  /// \param [out] theModuleSelectionModes module additional modes, -1 means all default modes
  /// \param theModes [out] a container of modes
  virtual void selectionModes(int& theModuleSelectionModes, QIntList& theModes) {}

  /// Appends into container of workshop selection filters
  /// \param [out] theModuleSelectionFilters module additional modes, -1 means all default modes
  /// \param [out] selection filters
  virtual void selectionFilters(QIntList& theModuleSelectionFilters,
                                SelectMgr_ListOfFilter& theSelectionFilters);

  /// Returns true if the validation is activated
  bool isInValidate() const { return myIsInValidate; }

  /// Checks all widget validator if the owner is valid. Firstly it checks custom widget validating,
  /// next, the attribute's validating. It trying on the give selection to current attribute by
  /// setting the value inside and calling validators. After this, the previous attribute value is
  /// restored.The valid/invalid value is cashed.
  /// \param theValue a selected presentation in the view
  /// \return a boolean value
  virtual bool isValidSelection(const std::shared_ptr<ModuleBase_ViewerPrs>& theValue);

    /// Creates a backup of the current values of the attribute
  /// It should be realized in the specific widget because of different
  /// parameters of the current attribute
  /// \param theAttribute an attribute to be stored
  virtual void storeAttributeValue(const std::shared_ptr<ModelAPI_Attribute>& theAttribute);

  /// Creates a backup of the current values of the attribute
  /// It should be realized in the specific widget because of different
  /// parameters of the current attribute
  /// \param theAttribute an attribute to be restored
  /// \param theValid a boolean flag, if restore happens for valid parameters
  virtual void restoreAttributeValue(const std::shared_ptr<ModelAPI_Attribute>& theAttribute,
                                     const bool theValid);

  /// Checks the current attibute in all attribute validators
  /// \param theAttribute an attribute to be validated
  /// \return true if all validators return that the attribute is valid
  bool isValidAttribute(const std::shared_ptr<ModelAPI_Attribute>& theAttribute) const;

  //! Clear all validated cash in the widget
  void clearValidatedCash();

private:
  /// Returns true if the workshop validator filter has been already activated
  /// \return boolean value
  bool isFilterActivated() const;

  /// Gets the validity state of the presentation in an internal map.
  /// Returns true if the valid state of value is stored
  /// \param theValue a viewer presentation
  /// \param theValid a valid state
  bool getValidState(const std::shared_ptr<ModuleBase_ViewerPrs>& theValue, bool& theValid);

  /// Store the validity state of the presentation in an internal map
  /// \param theValue a viewer presentation
  /// \param theValid a valid state
  void storeValidState(const std::shared_ptr<ModuleBase_ViewerPrs>& theValue, const bool theValid);

protected:
  bool myIsInValidate; ///< cashed if the value is processed in validation
  /// Reference to workshop
  ModuleBase_ModelWidget* myModelWidget; ///< the current widget to be validated
  ModuleBase_IWorkshop* myWorkshop; ///< the active workshop
  ModuleBase_WidgetSelectorStore* myAttributeStore; ///< store/restore attribute values

  /// cash of valid selection presentations
  QList<std::shared_ptr<ModuleBase_ViewerPrs>> myValidPrs;

  /// cash of invalid selection presentations
  QList<std::shared_ptr<ModuleBase_ViewerPrs>> myInvalidPrs;
};

#endif /* ModuleBase_WidgetValidator_H_ */
