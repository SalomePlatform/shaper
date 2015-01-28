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
#include <ModuleBase_ModelWidget.h>

#include <GeomAPI_Shape.h>
#include <ModelAPI_Result.h>
#include <ModelAPI_AttributeSelectionList.h>

#include <NCollection_List.hxx>
#include <TopoDS_Shape.hxx>

#include <QList>
#include <QString>
#include <QStringList>
#include <QPair>

class QWidget;
class QListWidget;
class QComboBox;
class ModuleBase_IWorkshop;
class QAction;


/**
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
class MODULEBASE_EXPORT ModuleBase_WidgetMultiSelector : public ModuleBase_ModelWidget
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

  /// Saves the internal parameters to the given feature
  virtual bool storeValue() const;

  virtual bool restoreValue();

  /// Returns the internal parent wiget control, that can be shown anywhere
  /// \returns the widget
  QWidget* getControl() const;

  /// Returns list of widget controls
  /// \return a control list
  virtual QList<QWidget*> getControls() const;

  virtual bool eventFilter(QObject* , QEvent* );

 public slots:
  /// Activate or deactivate selection
  void activateSelection(bool toActivate);

  /// Slot is called on selection type changed
  void onSelectionTypeChanged();

  /// Slot is called on selection changed
  void onSelectionChanged();

protected slots:
  /// Slot for copy command in a list pop-up menu
  void onCopyItem();

  /// Slot is called on selection of list of selected items
  void onListSelection();

 protected:
   /// Provide filtering of selected shapes
   /// \param theShapesToFilter source list of shapes
   /// \param theResult result list of shapes
  void filterShapes(const NCollection_List<TopoDS_Shape>& theShapesToFilter,
                    NCollection_List<TopoDS_Shape>& theResult);

  /// Set current shape type for selection
  void setCurrentShapeType(const TopAbs_ShapeEnum theShapeType);

  /// Start shape selection
  void activateShapeSelection();

 private:
   /// Update selection list
   void updateSelectionList(AttributeSelectionListPtr);

   /// List control
  QListWidget* myListControl;

  /// Combobox of types
  QComboBox* myTypeCombo;

  /// Container
  QWidget* myMainWidget;

  //TODO: Move into the base of selectors
  ModuleBase_IWorkshop* myWorkshop;

  /// If true then local selector has to be activated in context
  bool myIsActive;

  /// Provides correspondance between Result object and its shape
  typedef QPair<ResultPtr, GeomShapePtr> GeomSelection;

  /// Variable of GeomSelection type
  QList<GeomSelection> mySelection;

  /// An action for pop-up menu in a list control
  QAction* myCopyAction;
};

#endif /* MODULEBASE_WIDGETFILESELECTOR_H_ */
