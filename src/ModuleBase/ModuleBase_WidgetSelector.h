// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        ModuleBase_WidgetSelector.h
// Created:     19 June 2015
// Author:      Natalia ERMOLAEVA

#ifndef ModuleBase_WidgetSelector_H
#define ModuleBase_WidgetSelector_H

#include "ModuleBase.h"
#include "ModuleBase_WidgetValidated.h"
#include <ModuleBase_ViewerPrs.h>
#include "ModuleBase_Definitions.h"

#include <ModelAPI_Object.h>
#include <ModelAPI_Attribute.h>
#include <GeomAPI_Shape.h>

class Config_WidgetAPI;
class QWidget;
class ModuleBase_IWorkshop;

/**
* \ingroup GUI
* Implementation of widget for selection.
* This abstract interface is designed to be used for shape/multi shape selector widgets.
*/
class MODULEBASE_EXPORT ModuleBase_WidgetSelector : public ModuleBase_WidgetValidated
{
Q_OBJECT
 public:

  /// Constructor
  /// \param theParent the parent object
  /// \param theWorkshop instance of workshop interface
  /// \param theData the widget configuation. The attribute of the model widget is obtained from
  ModuleBase_WidgetSelector(QWidget* theParent, ModuleBase_IWorkshop* theWorkshop,
                            const Config_WidgetAPI* theData);

  virtual ~ModuleBase_WidgetSelector();

  /// Defines if it is supposed that the widget should interact with the viewer.
  virtual bool isViewerSelector() { return true; }

  /// Activate or deactivate selection and selection filters
  /// \return true if the selection filter of the widget is activated in viewer context
  virtual bool activateSelectionAndFilters(bool toActivate);

  /// Checks the widget validity. By default, it returns true.
  /// \param thePrs a selected presentation in the view
  /// \return a boolean value
  virtual bool isValidSelectionCustom(const ModuleBase_ViewerPrs& thePrs);

  /// Fills the attribute with the value of the selected owner
  /// \param thePrs a selected owner
  virtual bool setSelectionCustom(const ModuleBase_ViewerPrs& thePrs);

  /// The methiod called when widget is deactivated
  virtual void deactivate();

 private slots:
   /// Slot which is called on selection event
  void onSelectionChanged();

 protected:
  /// Update focus after the attribute value change
  virtual void updateFocus() = 0;

  /// Return the attribute values wrapped in a list of viewer presentations
  /// \return a list of viewer presentations, which contains an attribute result and
  /// a shape. If the attribute do not uses the shape, it is empty
  virtual QList<ModuleBase_ViewerPrs> getAttributeSelection() const = 0;

  /// Retunrs a list of possible shape types
  /// \return a list of shapes
  virtual QIntList getShapeTypes() const = 0;

  /// Computes and updates name of selected object in the widget
  virtual void updateSelectionName() = 0;

  /// Store the values to the model attribute of the widget. It casts this attribute to
  /// the specific type and set the given values
  /// \param theSelectedObject an object
  /// \param theShape a selected shape, which is used in the selection attribute
  /// \return true if it is succeed
  virtual void setObject(ObjectPtr theSelectedObject, GeomShapePtr theShape) = 0;

  /// The methiod called when widget is activated
  virtual void activateCustom();

  /// Returns true if selected shape corresponds to requested shape types.
  /// If the widget type of shapes contains the faces item, the result is converted
  /// to construction result and the method returns true if there is at least one face
  /// in the construction.
  /// \param theShape a shape
  /// \param theResult a selected result
  bool acceptSubShape(const GeomShapePtr& theShape, const ResultPtr& theResult) const;

  /// Return an object and geom shape by the viewer presentation
  /// \param thePrs a selection
  /// \param theObject an output object
  /// \param theShape a shape of the selection
  virtual void getGeomSelection(const ModuleBase_ViewerPrs& thePrs,
                                ObjectPtr& theObject,
                                GeomShapePtr& theShape);

  //----------- Class members -------------
  /// Returns a name in the next form: attribute_feature_name/attribute_id
  /// \param theAttribute a model attribute
  /// \return string value
  static std::string generateName(const AttributePtr& theAttribite,
                                  ModuleBase_IWorkshop* theWorkshop);
};

#endif
