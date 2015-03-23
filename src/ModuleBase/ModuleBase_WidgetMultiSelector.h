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
#include <ModuleBase_FilterNoDegeneratedEdge.h>

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

  virtual bool restoreValue();

  /// Returns list of widget controls
  /// \return a control list
  virtual QList<QWidget*> getControls() const;

  virtual bool eventFilter(QObject* , QEvent* );

  /// The methiod called when widget is deactivated
  virtual void deactivate();

  /// Fills the attribute with the value of the selected owner
  /// \param theOwner a selected owner
  virtual bool setSelection(const Handle_SelectMgr_EntityOwner& theOwner);

 public slots:
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
  /// The methiod called when widget is activated
  virtual void activateCustom();

  /// Saves the internal parameters to the given feature
  /// \return True in success
  virtual bool storeValueCustom() const;

  /// Creates a backup of the current values of the attribute
  /// It should be realized in the specific widget because of different
  /// parameters of the current attribute
  /// \param isBackup a boolean flag, if true, store values from the attribute
  /// to backup, otherwise set the backed up values to the attribute
  virtual void backupAttributeValue(const bool isBackup);

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

  //TODO: Move into the base of selectors
  ModuleBase_IWorkshop* myWorkshop;

  /// If true then local selector has to be activated in context
  bool myIsActive;

  /// Provides correspondance between Result object and its shape
  typedef QPair<ResultPtr, GeomShapePtr> GeomSelection;

  /// An action for pop-up menu in a list control
  QAction* myCopyAction;

  /// A filter for the Edges type, which avoid the generated edges selection
  Handle(ModuleBase_FilterNoDegeneratedEdge) myEdgesTypeFilter;

  /// backup parameters of the model attribute. The class processes three types of attribute:
  /// Reference, RefAttr and Selection. Depending on the attribute type, only the attribute parameter
  /// values are reserved in the backup
  /// Variable of selection type
  std::string mySelectionType;

  /// Variable of GeomSelection
  QList<GeomSelection> mySelection;
};

#endif /* MODULEBASE_WIDGETFILESELECTOR_H_ */
