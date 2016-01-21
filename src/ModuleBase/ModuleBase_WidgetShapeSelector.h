// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        ModuleBase_WidgetShapeSelector.h
// Created:     2 June 2014
// Author:      Vitaly Smetannikov

#ifndef ModuleBase_WidgetShapeSelector_H
#define ModuleBase_WidgetShapeSelector_H

#include "ModuleBase.h"
#include "ModuleBase_WidgetSelector.h"
#include "ModuleBase_ViewerFilters.h"
#include <ModuleBase_ViewerPrs.h>

#include <ModelAPI_Object.h>
#include <ModelAPI_Attribute.h>
#include <GeomAPI_Shape.h>

#include <TopAbs_ShapeEnum.hxx>

#include <QStringList>

class Config_WidgetAPI;
class QWidget;
class QLabel;
class QLineEdit;
class QToolButton;
class ModuleBase_IWorkshop;
class ModelAPI_Validator;

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
class MODULEBASE_EXPORT ModuleBase_WidgetShapeSelector : public ModuleBase_WidgetSelector
{
Q_OBJECT
 public:

  /// Constructor
  /// \param theParent the parent object
  /// \param theWorkshop instance of workshop interface
  /// \param theData the widget configuation. The attribute of the model widget is obtained from
  /// \param theParentId is Id of a parent of the current attribute
  ModuleBase_WidgetShapeSelector(QWidget* theParent, ModuleBase_IWorkshop* theWorkshop,
                                 const Config_WidgetAPI* theData, const std::string& theParentId);

  virtual ~ModuleBase_WidgetShapeSelector();

  /// Returns list of widget controls
  /// \return a control list
  virtual QList<QWidget*> getControls() const;

 protected:
  /// Saves the internal parameters to the given feature
  /// \return True in success
  virtual bool storeValueCustom() const;

  virtual bool restoreValueCustom();

  /// Creates a backup of the current values of the attribute
  /// It should be realized in the specific widget because of different
  /// parameters of the current attribute
  virtual void storeAttributeValue();

  /// Creates a backup of the current values of the attribute
  /// It should be realized in the specific widget because of different
  /// parameters of the current attribute
  /// \param theValid a boolean flag, if restore happens for valid parameters
  virtual void restoreAttributeValue(const bool theValid);

  /// Computes and updates name of selected object in the widget
  virtual void updateSelectionName();

  /// Clear attribute
  virtual void clearAttribute();

  // Update focus after the attribute value change
  virtual void updateFocus();

  /// Retunrs a list of possible shape types
  /// \return a list of shapes
  virtual QIntList getShapeTypes() const;

  /// Store the values to the model attribute of the widget. It casts this attribute to
  /// the specific type and set the given values
  /// \param theSelectedObject an object
  /// \param theShape a selected shape, which is used in the selection attribute
  /// \return true if it is succeed
  virtual void setObject(ObjectPtr theSelectedObject, GeomShapePtr theShape);

  /// Get the shape from the attribute if the attribute contains a shape, e.g. selection attribute
  /// \return a shape
  virtual GeomShapePtr getShape() const;

  /// Return the attribute values wrapped in a list of viewer presentations
  /// \return a list of viewer presentations, which contains an attribute result and
  /// a shape. If the attribute do not uses the shape, it is empty
  virtual QList<ModuleBase_ViewerPrs> getAttributeSelection() const;

  //----------- Class members -------------
  protected:
  /// Label of the widget
  QLabel* myLabel;

  /// Input control of the widget
  QLineEdit* myTextLine;

  /// List of accepting shapes types
  QStringList myShapeTypes;

  /// backup parameters of the model attribute. The class processes three types of attribute:
  /// Reference, RefAttr and Selection. Depending on the attribute type, only the attribute parameter
  /// values are reserved in the backup
  /// An attribute object
  ObjectPtr myObject;
  /// An attribute shape
  GeomShapePtr myShape;
  /// A reference of the attribute
  AttributePtr myRefAttribute;
  /// A boolean value whether refAttr uses reference of object
  bool myIsObject;
};

#endif
