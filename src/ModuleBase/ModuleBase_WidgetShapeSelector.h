// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        ModuleBase_WidgetShapeSelector.h
// Created:     2 June 2014
// Author:      Vitaly Smetannikov

#ifndef ModuleBase_WidgetShapeSelector_H
#define ModuleBase_WidgetShapeSelector_H

#include "ModuleBase.h"
#include "ModuleBase_ModelWidget.h"
#include "ModuleBase_ViewerFilters.h"

#include <ModelAPI_Object.h>
#include <GeomAPI_Shape.h>

#include <TopAbs_ShapeEnum.hxx>

#include <QStringList>

class Config_WidgetAPI;
class QWidget;
class QLabel;
class QLineEdit;
class QToolButton;
class ModuleBase_IWorkshop;

/**
* \ingroup GUI
* Implementation of widget for shapes selection.
* This type of widget can be defined in XML file with 'shape_selector' keyword.
* For example:
* \code
*   <shape_selector id="main_object" 
*    label="Main object" 
*    icon=":icons/cut_shape.png" 
*    tooltip="Select an object solid"
*    shape_types="solid shell"
*    concealment="true"
*  />
* \endcode
* It can use following parameters:
* - id - name of object attribute
* - label - content of widget's label
* - icon - icon which can be used instead label
* - tooltip - the witget's tooltip text
* - shape_types - list of shape types for selection. 
*       Possible values: face, vertex, wire, edge, shell, solid
* - object_types - types of selectable objects. 
*       For today it supports only one type "construction" 
*        which corresponds to ModelAPI_ResultConstruction object type
* - concealment - hide or not hide selected object after operation
*/
class MODULEBASE_EXPORT ModuleBase_WidgetShapeSelector : public ModuleBase_ModelWidget
{
Q_OBJECT
 public:

   /// Converts string value (name of shape type) to shape enum value
   /// \param theType - string with shape type name
   /// \return TopAbs_ShapeEnum value
  static TopAbs_ShapeEnum shapeType(const QString& theType);

  /// Constructor
  /// \param theParent the parent object
  /// \param theWorkshop instance of workshop interface
  /// \param theData the widget configuation. The attribute of the model widget is obtained from
  /// \param theParentId is Id of a parent of the current attribute
  ModuleBase_WidgetShapeSelector(QWidget* theParent, ModuleBase_IWorkshop* theWorkshop,
                            const Config_WidgetAPI* theData, const std::string& theParentId);

  virtual ~ModuleBase_WidgetShapeSelector();

  /// Saves the internal parameters to the given feature
  virtual bool storeValue() const;

  virtual bool restoreValue();

  /// Defines if it is supposed that the widget should interact with the viewer.
  virtual bool isViewerSelector() { return true; }

  /// Returns the internal parent wiget control, that can be shown anywhere
  /// \returns the widget
  QWidget* getControl() const
  {
    return myContainer;
  }

  /// Returns list of widget controls
  /// \return a control list
  virtual QList<QWidget*> getControls() const;

  /// Returns currently selected data object
  ObjectPtr selectedFeature() const
  {
    return mySelectedObject;
  }

  /// Set the given wrapped value to the current widget
  /// This value should be processed in the widget according to the needs
  /// \param theValue the wrapped widget value
  virtual bool setSelection(ModuleBase_ViewerPrs theValue);


  /// The methiod called when widget is activated
  virtual void activate();

  /// The methiod called when widget is deactivated
  virtual void deactivate();


 public slots:

  /// Activate or deactivate selection
  void activateSelection(bool toActivate);

 private slots:
   /// Slot which is called on selection event
  void onSelectionChanged();

 protected:
   /// Computes and updates name of selected object in the widget
  void updateSelectionName();

  /// Raise panel which contains this widget
  void raisePanel() const;

  /// Returns true if selected shape corresponds to requested shape types
  /// \param theShape a shape
  virtual bool acceptSubShape(std::shared_ptr<GeomAPI_Shape> theShape) const;

  /// Returns true if selected object corresponds to requested Object type
  /// Thid method is used in any selection mode
  /// \param theObject an object 
  virtual bool acceptObjectType(const ObjectPtr theObject) const;


  // Set the given object as a value of the widget
  /// \param theObj an object 
  /// \param theShape a shape
  void setObject(ObjectPtr theObj, std::shared_ptr<GeomAPI_Shape> theShape = std::shared_ptr<GeomAPI_Shape>());

  /// Check the selected with validators if installed
  /// \param theObj the object for checking
  /// \param theShape the shape for checking
  virtual bool isValid(ObjectPtr theObj, std::shared_ptr<GeomAPI_Shape> theShape);

  /// Clear attribute
  void clearAttribute();

  //----------- Class members -------------
 protected:

   /// Container of the widget's control
  QWidget* myContainer;

  /// Label of the widget
  QLabel* myLabel;

  /// Input control of the widget
  QLineEdit* myTextLine;

  /// Reference to workshop
  ModuleBase_IWorkshop* myWorkshop;

  /// Pointer to selected object
  ObjectPtr mySelectedObject;

  /// Pointer to selected shape
  GeomShapePtr myShape;

  /// List of accepting shapes types
  QStringList myShapeTypes;

  /// List of accepting object types
  QStringList myObjectTypes;

  /// Active/inactive flag
  bool myIsActive;

  /// Filter by objects types
  Handle(ModuleBase_ObjectTypesFilter) myObjTypeFilter;
};

#endif
