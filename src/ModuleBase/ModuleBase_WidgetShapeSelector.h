// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        ModuleBase_WidgetShapeSelector.h
// Created:     2 June 2014
// Author:      Vitaly Smetannikov

#ifndef ModuleBase_WidgetShapeSelector_H
#define ModuleBase_WidgetShapeSelector_H

#include "ModuleBase.h"
#include "ModuleBase_WidgetSelector.h"
#include "ModuleBase_ViewerFilters.h"

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
class ModuleBase_ViewerPrs;
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
  ModuleBase_WidgetShapeSelector(QWidget* theParent, ModuleBase_IWorkshop* theWorkshop,
                                 const Config_WidgetAPI* theData);

  virtual ~ModuleBase_WidgetShapeSelector();

  /// Set the given wrapped value to the current widget
  /// This value should be processed in the widget according to the needs
  /// The method is called by the current operation to process the operation preselection.
  /// It is redefined to check the value validity and if it is, fill the attribute with by value
  /// \param theValues the wrapped selection values
  /// \param theToValidate a flag on validation of the values
  virtual bool setSelection(QList<std::shared_ptr<ModuleBase_ViewerPrs>>& theValues,
                            const bool theToValidate);

  /// Returns list of widget controls
  /// \return a control list
  virtual QList<QWidget*> getControls() const;

 protected:
  /// Saves the internal parameters to the given feature
  /// \return True in success
  virtual bool storeValueCustom() const;

  virtual bool restoreValueCustom();

  /// Computes and updates name of selected object in the widget
  virtual void updateSelectionName();

  // Update focus after the attribute value change
  virtual void updateFocus();

  /// Retunrs a list of possible shape types
  /// \return a list of shapes
  virtual QIntList getShapeTypes() const;

  /// Get the shape from the attribute if the attribute contains a shape, e.g. selection attribute
  /// \return a shape
  virtual GeomShapePtr getShape() const;

  /// Return the attribute values wrapped in a list of viewer presentations
  /// \return a list of viewer presentations, which contains an attribute result and
  /// a shape. If the attribute do not uses the shape, it is empty
  virtual QList<std::shared_ptr<ModuleBase_ViewerPrs>> getAttributeSelection() const;

  //----------- Class members -------------
  protected:
  /// Label of the widget
  QLabel* myLabel;

  /// Input control of the widget
  QLineEdit* myTextLine;

  /// List of accepting shapes types
  QStringList myShapeTypes;
};

#endif
