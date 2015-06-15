// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

/*
 * ModuleBase_WidgetMultiSelector.h
 *
 *  Created on: Oct 8, 2014
 *      Author: sbh
 */

#ifndef MODULEBASE_WIDGETMULTISELECTOR_H_
#define MODULEBASE_WIDGETMULTISELECTOR_H_

#include <ModuleBase.h>
#include <ModuleBase_WidgetValidated.h>

#include <GeomAPI_Shape.h>
#include <ModelAPI_Result.h>
#include <ModelAPI_AttributeSelectionList.h>

#include <NCollection_List.hxx>
#include <TopoDS_Shape.hxx>

#include <QList>
#include <QString>
#include <QStringList>
#include <QPair>
#include <QMap>

class QWidget;
class QListWidget;
class QComboBox;
class ModuleBase_IWorkshop;
class GeomValidators_ShapeType;
class QAction;


/**
* \ingroup GUI
* Implementation of widget for shapes selection. This widget provides selection of several shapes.
* It can be defined in XML file as following:
* \code
* <multi_selector id="group_list" 
*    tooltip="Select a set of objects" 
*    type_choice="Vertices Edges Faces Solids" /> 
* \endcode
* It uses folloing parameters:
* - id - is a name of corresponded attribute
* - tooltip - a tooltip for the widget
* - type_choice - list of expected shape types.
*/
class MODULEBASE_EXPORT ModuleBase_WidgetMultiSelector : public ModuleBase_WidgetValidated
{
  Q_OBJECT
 public:
  /// Constructor
  /// \param theParent the parent object
  /// \param theWorkshop instance of workshop interface
  /// \param theData the widget configuation. The attribute of the model widget is obtained from
  /// \param theParentId is Id of a parent of the current attribute
  ModuleBase_WidgetMultiSelector(QWidget* theParent,
                                 ModuleBase_IWorkshop* theWorkshop,
                                 const Config_WidgetAPI* theData,
                                 const std::string& theParentId);
  virtual ~ModuleBase_WidgetMultiSelector();

  //TODO: nds stabilization hotfix
  virtual void disconnectSignals();

  virtual bool restoreValue();

  /// Returns list of widget controls
  /// \return a control list
  virtual QList<QWidget*> getControls() const;

  /// The methiod called when widget is deactivated
  virtual void deactivate();

  /// Set the given wrapped value to the current widget
  /// This value should be processed in the widget according to the needs
  /// \param theValues the wrapped selection values
  /// \param thePosition an index in the list of values, the values should be get from the index
  virtual bool setSelection(const QList<ModuleBase_ViewerPrs>& theValues, int& thePosition);

  /// Checks the widget validity. By default, it returns true.
  /// \param theValue a selected presentation in the view
  /// \return a boolean value
  virtual bool isValidSelectionCustom(const ModuleBase_ViewerPrs& thePrs);

  /// Fills the attribute with the value of the selected owner
  /// \param theOwner a selected owner
  virtual bool setSelectionCustom(const ModuleBase_ViewerPrs& thePrs);

 public slots:
  /// Slot is called on selection type changed
  void onSelectionTypeChanged();

  /// Slot is called on selection changed
  virtual void onSelectionChanged();

protected slots:
  /// Slot for copy command in a list pop-up menu
  void onCopyItem();

  /// Slot is called on selection of list of selected items
  void onListSelection();

 protected:
  /// The methiod called when widget is activated
  virtual void activateCustom();

  /// Saves the internal parameters to the given feature
  /// \return True in success
  virtual bool storeValueCustom() const;

  /// Creates a backup of the current values of the attribute
  /// It should be realized in the specific widget because of different
  /// parameters of the current attribute
  virtual void storeAttributeValue();

  /// Creates a backup of the current values of the attribute
  /// It should be realized in the specific widget because of different
  /// parameters of the current attribute
  /// \param theValid a boolean flag, if restore happens for valid parameters
  virtual void restoreAttributeValue(const bool theValid);

  /// Puts additional validators to the given list. A separate validator is created for each of the
  /// "type_choice" value
  /// \param theValidators a list of validators
  /// \param theArguments a list of validators arguments
  virtual void customValidators(std::list<ModelAPI_Validator*>& theValidators,
                                std::list<std::list<std::string> >& theArguments) const;

  /// Returns true if selected shape corresponds to requested shape types
  /// \param theShape a shape
  bool acceptSubShape(const TopoDS_Shape& theShape) const;

  /// Set current shape type for selection
  void setCurrentShapeType(const TopAbs_ShapeEnum theShapeType);

  /// Start shape selection
  /// \param isActivated a state whether the shape is activated or deactivated in selection
  void activateShapeSelection(const bool isActivated);

  /// Return the attribute values wrapped in a list of viewer presentations
  /// \return a list of viewer presentations, which contains an attribute result and
  /// a shape. If the attribute do not uses the shape, it is empty
  QList<ModuleBase_ViewerPrs> getAttributeSelection() const;

 protected:
  /// Update selection list
  void updateSelectionList(AttributeSelectionListPtr);

  /// Converts the XML defined type choice to the validator type
  /// For example, the "Edges" is converted to "edge"
  std::string validatorType(const QString& theType) const;

   /// List control
  QListWidget* myListControl;

  /// Combobox of types
  QComboBox* myTypeCombo;

  //TODO: Move into the base of selectors
  ModuleBase_IWorkshop* myWorkshop;

  /// Provides correspondance between Result object and its shape
  typedef QPair<ResultPtr, GeomShapePtr> GeomSelection;

  /// An action for pop-up menu in a list control
  QAction* myCopyAction;

  /// backup parameters of the model attribute. The class processes three types of attribute:
  /// Reference, RefAttr and Selection. Depending on the attribute type, only the attribute parameter
  /// values are reserved in the backup
  /// Variable of selection type
  std::string mySelectionType;

  /// Variable of GeomSelection
  QList<GeomSelection> mySelection;

  /// An instance of the "type_choice" validator. It is returns on validating in customValidator()
  GeomValidators_ShapeType* myShapeValidator;

  //bool myIsUseChoice;
};

#endif /* MODULEBASE_WIDGETFILESELECTOR_H_ */
