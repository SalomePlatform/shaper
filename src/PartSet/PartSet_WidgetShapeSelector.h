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

#ifndef PartSet_WidgetShapeSelector_H
#define PartSet_WidgetShapeSelector_H

#include "PartSet.h"

#include <ModuleBase_WidgetShapeSelector.h>

#include <ModelAPI_CompositeFeature.h>

class PartSet_ExternalObjectsMgr;

/**
* \ingroup Modules
* Customosation of ModuleBase_WidgetShapeSelector in order to provide 
* working with sketch specific objects and external objects.
*/
class PARTSET_EXPORT PartSet_WidgetShapeSelector: public ModuleBase_WidgetShapeSelector
{
Q_OBJECT
 public:
  /// Constructor
  /// \param theParent the parent object
  /// \param theWorkshop instance of workshop interface
  /// \param theData the widget configuation. The attribute of the model widget is obtained from
  PartSet_WidgetShapeSelector(QWidget* theParent, ModuleBase_IWorkshop* theWorkshop,
                              const Config_WidgetAPI* theData);

  virtual ~PartSet_WidgetShapeSelector();

  /// Set sketcher
  /// \param theSketch a sketcher object
  void setSketcher(CompositeFeaturePtr theSketch) { mySketch = theSketch; }

  /// Retrurns installed sketcher
  CompositeFeaturePtr sketch() const { return mySketch; }

  /// Appends into container of workshop selection filters
  /// \param [out] theModuleSelectionFilters module additional modes, -1 means all default modes
  /// \param [out] theSelectionFilters selection filters
  virtual void selectionFilters(int& theModuleSelectionFilters,
                                SelectMgr_ListOfFilter& theSelectionFilters);

protected:
  /// Checks the widget validity. By default, it returns true.
  /// \param thePrs a selected presentation in the view
  /// \return a boolean value
  virtual bool isValidSelectionCustom(const std::shared_ptr<ModuleBase_ViewerPrs>& thePrs);

  /// Return an object and geom shape by the viewer presentation
  /// \param thePrs a selection
  /// \param theObject an output object
  /// \param theShape a shape of the selection
  virtual void getGeomSelection(const std::shared_ptr<ModuleBase_ViewerPrs>& thePrs,
                                ObjectPtr& theObject,
                                GeomShapePtr& theShape);

  /// Creates a backup of the current values of the attribute
  /// It should be realized in the specific widget because of different
  /// parameters of the current attribute
  /// \param theAttribute an attribute
  /// \param theValid a boolean flag, if restore happens for valid parameters
  void restoreAttributeValue(const AttributePtr& theAttribute, const bool theValid);

protected:
  /// A reference to external objects manager
  PartSet_ExternalObjectsMgr* myExternalObjectMgr;

  /// Pointer to a sketch
  CompositeFeaturePtr mySketch;

  bool myUseSketchPlane; ///< state whether the sketch plane filter should be switched on
};

#endif