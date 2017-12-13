// Copyright (C) 2014-2017  CEA/DEN, EDF R&D
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
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
//
// See http://www.salome-platform.org/ or
// email : webmaster.salome@opencascade.com<mailto:webmaster.salome@opencascade.com>
//

#ifndef ModuleBase_WidgetSelector_H
#define ModuleBase_WidgetSelector_H

#include "ModuleBase.h"
#include "ModuleBase_WidgetValidated.h"
#include "ModuleBase_Definitions.h"

#include <ModelAPI_Object.h>
#include <ModelAPI_Attribute.h>
#include <GeomAPI_Shape.h>

class Config_WidgetAPI;
class QWidget;
class ModuleBase_IWorkshop;
class ModuleBase_ViewerPrs;
class ModelAPI_Result;

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

  /// Fills given container with selection modes if the widget has it
  /// \param theModes [out] a container of modes
  /// \param isAdditional if true, the modes are combinated with the module ones
  virtual void selectionModes(QIntList& theModes, bool& isAdditional);

  /// Defines if it is supposed that the widget should interact with the viewer.
  virtual bool isViewerSelector() { return true; }

  /// Activate or deactivate selection and selection filters
  virtual void activateSelectionAndFilters(bool toActivate);

  /// Checks the widget validity. By default, it returns true.
  /// \param thePrs a selected presentation in the view
  /// \return a boolean value
  virtual bool isValidSelectionCustom(const std::shared_ptr<ModuleBase_ViewerPrs>& thePrs);

  /// Fills the attribute with the value of the selected owner
  /// \param thePrs a selected owner
  virtual bool setSelectionCustom(const std::shared_ptr<ModuleBase_ViewerPrs>& thePrs);

  /// The methiod called when widget is deactivated
  virtual void deactivate();

  /// Return the attribute values wrapped in a list of viewer presentations
  /// \return a list of viewer presentations, which contains an attribute result and
  /// a shape. If the attribute do not uses the shape, it is empty
  virtual QList<std::shared_ptr<ModuleBase_ViewerPrs>> getAttributeSelection() const;

protected:
  /// Returns true if envent is processed. The default implementation is empty, returns false.
  virtual bool processSelection();

  /// Emits model changed info, updates the current control by selection change
  /// \param theDone a state whether the selection is set
  virtual void updateOnSelectionChanged(const bool theDone);

  /// Update focus after the attribute value change
  // NDS: has body is temporary
   virtual void updateFocus() {};

  /// Retunrs a list of possible shape types
  /// \return a list of shapes
  QIntList getShapeTypes() const;

  /// Retunrs a list of possible shape types
  /// \return a list of shapes
  virtual QIntList shapeTypes() const = 0;

  /// Computes and updates name of selected object in the widget
  // NDS: has body is temporary
  virtual void updateSelectionName() {};

  /// The methiod called when widget is activated
  virtual void activateCustom();

  /// Returns true if selected shape corresponds to requested shape types.
  /// If the widget type of shapes contains the faces item, the result is converted
  /// to construction result and the method returns true if there is at least one face
  /// in the construction.
  /// \param theShape a shape
  /// \param theResult a selected result
  bool acceptSubShape(const GeomShapePtr& theShape,
                      const std::shared_ptr<ModelAPI_Result>& theResult) const;

  /// Return an object and geom shape by the viewer presentation
  /// \param thePrs a selection
  /// \param theObject an output object
  /// \param theShape a shape of the selection
  virtual void getGeomSelection(const std::shared_ptr<ModuleBase_ViewerPrs>& thePrs,
                                ObjectPtr& theObject,
                                GeomShapePtr& theShape);

  //----------- Class members -------------
  /// Returns a name in the next form: attribute_feature_name/attribute_id
  /// \param theAttribute a model attribute
  /// \param theWorkshop a workshop class instance
  /// \return string value
  static std::string generateName(const AttributePtr& theAttribute,
                                  ModuleBase_IWorkshop* theWorkshop);
};

#endif
