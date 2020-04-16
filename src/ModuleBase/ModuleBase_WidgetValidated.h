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

#ifndef MODULEBASE_WIDGETVALIDATED_H_
#define MODULEBASE_WIDGETVALIDATED_H_

#include <ModuleBase.h>
#include <ModuleBase_ModelWidget.h>

#include <GeomAPI_Shape.h>
#include <GeomAPI_AISObject.h>
#include <ModelAPI_Object.h>
#include <ModelAPI_Attribute.h>
#include <ModelAPI_ResultBody.h>

#include <SelectMgr_ListOfFilter.hxx>
#include <NCollection_DataMap.hxx>
#include <TopoDS_Shape.hxx>
#include <AIS_NListOfEntityOwner.hxx>

#include <QList>
#include <QMap>

class QWidget;
class ModuleBase_IWorkshop;
class ModuleBase_ISelection;
class ModuleBase_WidgetSelectorStore;
class ModelAPI_Validator;
class Config_WidgetAPI;

//#define LIST_OF_VALID_PRS

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
  ModuleBase_WidgetValidated(QWidget* theParent,
                             ModuleBase_IWorkshop* theWorkshop,
                             const Config_WidgetAPI* theData);
  virtual ~ModuleBase_WidgetValidated();

  /// Checks all widget validator if the owner is valid. Firstly it checks custom widget validating,
  /// next, the attribute's validating. It trying on the give selection to current attribute by
  /// setting the value inside and calling validators. After this, the previous attribute value is
  /// restored.The valid/invalid value is cashed.
  /// \param theValue a selected presentation in the view
  /// \return a boolean value
  virtual bool isValidSelection(const std::shared_ptr<ModuleBase_ViewerPrs>& theValue);

  //! Returns data object by AIS
  ObjectPtr findPresentedObject(const AISObjectPtr& theAIS) const;

  /// The method called when widget is deactivated
  virtual void deactivate();

  //! Clear all validated cash in the widget
  void clearValidatedCash();

  /// Returns true if the workshop validator filter has been already activated
  /// \return boolean value
  bool isFilterActivated() const;

  /// Appends into container of workshop selection filters
  /// \param [out] theModuleSelectionFilters module additional modes, -1 means all default modes
  /// \param [out] selection filters
  virtual void selectionFilters(QIntList& theModuleSelectionFilters,
                                SelectMgr_ListOfFilter& theSelectionFilters);

  /// Block the model flush of update and intialization of attribute
  /// \param theAttribute an attribute of blocking
  /// \param theFeature a feature
  /// \param theToBlock flag whether the model is blocked or unblocked
  /// \param isFlushesActived out value if model is blocked, in value if model is unblocked
  /// to be used to restore flush state when unblocked
  /// \param isAttributeSetInitializedBlocked out value if model is blocked
  /// in value if model is unblocked to be used to restore previous state when unblocked
  /// \param isAttributeSendUpdatedBlocked out value if model signal is blocked
  static void blockFeatureAttribute(const AttributePtr& theAttribute,
                                    const FeaturePtr& theFeature,
                                    const bool& theToBlock,
                                    bool& isFlushesActived,
                                    bool& isAttributeSetInitializedBlocked,
                                    bool& isAttributeSendUpdatedBlocked);

  /// Checks the current attibute in all attribute validators
  /// \param theAttribute an attribute to be validated
  /// \return true if all validators return that the attribute is valid
  static bool isValidAttribute(const AttributePtr& theAttribute);

protected:
  /// Checks whether all active viewer filters validate the presentation
  /// \param thePrs a selected presentation in the view
  /// \return a boolean value
  virtual bool isValidInFilters(const std::shared_ptr<ModuleBase_ViewerPrs>& thePrs);

  /// Checks all attribute validators returns valid. It tries on the given selection
  /// to current attribute by setting the value inside and calling validators. After this,
  /// the previous attribute value is restored.The valid/invalid value is cashed.
  /// \param theValue a selected presentation in the view
  /// \param theAttribute the attribute
  /// \return a boolean value
  bool isValidSelectionForAttribute(const std::shared_ptr<ModuleBase_ViewerPrs>& theValue,
                                    const AttributePtr& theAttribute);

  /// Retunrs attribute, which should be validated. In default implementation,
  /// this is an attribute of ID
  /// \return an attribute
  virtual AttributePtr attribute() const;

  /// Creates a backup of the current values of the attribute
  /// It should be realized in the specific widget because of different
  /// parameters of the current attribute
  /// \param theAttribute an attribute to be stored
  virtual void storeAttributeValue(const AttributePtr& theAttribute);

  /// Creates a backup of the current values of the attribute
  /// It should be realized in the specific widget because of different
  /// parameters of the current attribute
  /// \param theAttribute an attribute to be restored
  /// \param theValid a boolean flag, if restore happens for valid parameters
  virtual void restoreAttributeValue(const AttributePtr& theAttribute, const bool theValid);

  /// Checks the widget validity. By default, it returns true.
  /// \param thePrs a selected presentation in the view
  /// \return a boolean value
  virtual bool isValidSelectionCustom(const std::shared_ptr<ModuleBase_ViewerPrs>& thePrs);

  /// Fills the attribute with the value of the selected owner
  /// \param thePrs a selected owner
  virtual bool setSelectionCustom(const std::shared_ptr<ModuleBase_ViewerPrs>& thePrs) = 0;

  /// Returns a list of selected presentations in the viewer and object browser
  /// The presentations from the object browser are filtered by the AIS context filters
  /// \return a list of presentations
  QList<std::shared_ptr<ModuleBase_ViewerPrs>> getFilteredSelected();

  /// Block the model flush of update and intialization of attribute
  /// \param theAttribute an attribute of blocking
  /// \param theToBlock flag whether the model is blocked or unblocked
  /// \param isFlushesActived out value if model is blocked, in value if model is unblocked
  /// to be used to restore flush state when unblocked
  /// \param isAttributeSetInitializedBlocked out value if model is blocked
  /// in value if model is unblocked to be used to restore previous state when unblocked
  /// \param isAttributeSendUpdatedBlocked out value if model signal is blocked
  virtual void blockAttribute(const AttributePtr& theAttribute, const bool& theToBlock,
                              bool& isFlushesActived, bool& isAttributeSetInitializedBlocked,
                              bool& isAttributeSendUpdatedBlocked);

protected:
  /// Gets the validity state of the presentation in an internal map.
  /// Returns true if the valid state of value is stored
  /// \param theValue a viewer presentation
  /// \param theValid a valid state
  bool getValidState(const std::shared_ptr<ModuleBase_ViewerPrs>& theValue, bool& theValid);

  /// Store the validity state of the presentation in an internal map
  /// \param theValue a viewer presentation
  /// \param theValid a valid state
  void storeValidState(const std::shared_ptr<ModuleBase_ViewerPrs>& theValue, const bool theValid);

private:
  /// Applies AIS context filters to the parameter list. The not approved presentations are
  /// removed from the parameters.
  /// \param theValues a list of presentations.
  void filterPresentations(QList<std::shared_ptr<ModuleBase_ViewerPrs>>& theValues);

  /// Remove subshapes of compsolids if whole compsolid is present.
  /// \param theValues a list of presentations.
  void filterCompSolids(QList<std::shared_ptr<ModuleBase_ViewerPrs>>& theValues);

  void collectSubBodies(const ResultBodyPtr& theBody, AIS_NListOfEntityOwner& theList);

protected:
  /// Reference to workshop
  ModuleBase_IWorkshop* myWorkshop;
  /// The widget is in validation mode: store is performed, restore is not
  bool myIsInValidate;

private:
  ObjectPtr myPresentedObject; /// back up of the filtered object
#ifdef LIST_OF_VALID_PRS
  /// cash of valid selection presentations
  QList<std::shared_ptr<ModuleBase_ViewerPrs>> myValidPrs;

  /// cash of invalid selection presentations
  QList<std::shared_ptr<ModuleBase_ViewerPrs>> myInvalidPrs;
#else
  // assume that one presentation selection presentation corresponds only one shape
  NCollection_DataMap<TopoDS_Shape, std::shared_ptr<ModuleBase_ViewerPrs> > myValidPrs;
  NCollection_DataMap<TopoDS_Shape, std::shared_ptr<ModuleBase_ViewerPrs> > myInvalidPrs;
#endif

  /// store to backup parameters of the model
  ModuleBase_WidgetSelectorStore* myAttributeStore;
};

#endif /* MODULEBASE_WIDGETVALIDATED_H_ */
