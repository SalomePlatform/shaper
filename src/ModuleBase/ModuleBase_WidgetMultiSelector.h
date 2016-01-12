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
#include <ModuleBase_WidgetSelector.h>

#include <GeomAPI_Shape.h>
#include <ModelAPI_Result.h>

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
* It uses following parameters:
* - id - is a name of corresponded attribute
* - tooltip - a tooltip for the widget
* - type_choice - list of expected shape types.
*/
class MODULEBASE_EXPORT ModuleBase_WidgetMultiSelector : public ModuleBase_WidgetSelector
{
  Q_OBJECT
 public:
  /// Constructor
  /// \param theParent the parent object
  /// \param theWorkshop instance of workshop interface
  /// \param theData the widget configuration. The attribute of the model widget is obtained from
  /// \param theParentId is Id of a parent of the current attribute
  ModuleBase_WidgetMultiSelector(QWidget* theParent,
                                 ModuleBase_IWorkshop* theWorkshop,
                                 const Config_WidgetAPI* theData,
                                 const std::string& theParentId);
  virtual ~ModuleBase_WidgetMultiSelector();

  /// Returns list of widget controls
  /// \return a control list
  virtual QList<QWidget*> getControls() const;

  /// Set the given wrapped value to the current widget
  /// This value should be processed in the widget according to the needs
  /// \param theValues the wrapped selection values
  /// \param theToValidate a validation of the values flag
  virtual bool setSelection(QList<ModuleBase_ViewerPrs>& theValues,
                            const bool theToValidate);

  /// Returns values which should be highlighted when the whidget is active
  /// \param theValues a list of presentations
  virtual void getHighlighted(QList<ModuleBase_ViewerPrs>& theValues);

  /// Checks the widget validity. By default, it returns true.
  /// \param thePrs a selected presentation in the view
  /// \return a boolean value
  virtual bool isValidSelectionCustom(const ModuleBase_ViewerPrs& thePrs);

 public slots:
  /// Slot is called on selection type changed
  void onSelectionTypeChanged();

protected slots:
  /// Slot for copy command in a list pop-up menu
  void onCopyItem();

  /// Slot for delete command in a list pop-up menu
  void onDeleteItem();

  /// Slot is called on selection of list of selected items
  void onListSelection();

protected:
  /// Saves the internal parameters to the given feature
  /// \return True in success
  virtual bool storeValueCustom() const;

  virtual bool restoreValueCustom();

  /// Creates a backup of the current values of the attribute
  /// It should be realized in the specific widget because of different
  /// parameters of the current attribute
  virtual void storeAttributeValue();

  /// Clear attribute
  virtual void clearAttribute();

  /// Set the focus on the last item in  the list
  virtual void updateFocus();

  /// Computes and updates name of selected object in the widget
  virtual void updateSelectionName();

  /// Retunrs a list of possible shape types
  /// \return a list of shapes
  virtual QIntList getShapeTypes() const;

  /// Append the values to the model attribute of the widget. It casts this attribute to
  /// the specific type and set the given values
  /// \param theSelectedObject an object
  /// \param theShape a selected shape, which is used in the selection attribute
  /// \return true if it is succeed
  void setObject(ObjectPtr theSelectedObject, GeomShapePtr theShape);

  /// Creates a backup of the current values of the attribute
  /// It should be realized in the specific widget because of different
  /// parameters of the current attribute
  /// \param theValid a boolean flag, if restore happens for valid parameters
  virtual void restoreAttributeValue(const bool theValid);

  /// Set current shape type for selection
  void setCurrentShapeType(const TopAbs_ShapeEnum theShapeType);

  /// Return the attribute values wrapped in a list of viewer presentations
  /// \return a list of viewer presentations, which contains an attribute result and
  /// a shape. If the attribute do not uses the shape, it is empty
  virtual QList<ModuleBase_ViewerPrs> getAttributeSelection() const;

  /// Fills the list control by the attribute values
  void updateSelectionList();

  /// Converts the XML defined type choice to the validator type
  /// For example, the "Edges" is converted to "edge"
  std::string validatorType(const QString& theType) const;

protected:
  /// Returns attribute indices selected in the widget selection list 
  /// \param theIndices a list of indices
  void getSelectedAttributeIndices(std::set<int>& theIndices);

  /// Gets the feature attribute and fill a list of viewer presentation for the attribute
  /// indices. If the the container of indices is empty, it returns all objects.
  /// \param theAttributeIds indices in attribute list to be returned
  /// \param theValues the result presentations, filled with object and shape of an attribute item
  void convertIndicesToViewerSelection(std::set<int> theAttributeIds,
                                       QList<ModuleBase_ViewerPrs>& theValues) const;
protected:
  /// List control
  QListWidget* myListControl;

  /// Combobox of types
  QComboBox* myTypeCombo;

  /// Provides correspondance between Result object and its shape
  typedef QPair<ResultPtr, GeomShapePtr> GeomSelection;

  /// An action for pop-up menu in a list control
  QAction* myCopyAction;
  QAction* myDeleteAction;

  /// backup parameters of the model attribute. The class processes three types of attribute:
  /// Reference, RefAttr and Selection. Depending on the attribute type, only the attribute parameter
  /// values are reserved in the backup
  /// Variable of selection type
  std::string mySelectionType;

  /// Variable of GeomSelection
  int mySelectionCount; // number of elements in the attribute selection list when store

  /// A flag to store use_choice parameter state
  bool myIsUseChoice;
};

#endif /* MODULEBASE_WIDGETFILESELECTOR_H_ */
