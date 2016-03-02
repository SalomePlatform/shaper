// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        PartSet_WidgetShapeSelector.h
// Created:     27 Nov 2014
// Author:      Vitaly Smetannikov


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
  /// \param theParentId is Id of a parent of the current attribute
  PartSet_WidgetShapeSelector(QWidget* theParent, ModuleBase_IWorkshop* theWorkshop,
    const Config_WidgetAPI* theData, const std::string& theParentId);

  virtual ~PartSet_WidgetShapeSelector();

  /// Set sketcher
  /// \param theSketch a sketcher object
  void setSketcher(CompositeFeaturePtr theSketch) { mySketch = theSketch; }

  /// Retrurns installed sketcher
  CompositeFeaturePtr sketch() const { return mySketch; }

  /// Activate or deactivate selection and selection filters
  /// \param toActivate boolean state whether it should be activated/deactivated
  virtual bool activateSelectionAndFilters(bool toActivate);

protected:
  /// Checks the widget validity. By default, it returns true.
  /// \param thePrs a selected presentation in the view
  /// \return a boolean value
  virtual bool isValidSelectionCustom(const ModuleBase_ViewerPrs& thePrs);

  /// Return an object and geom shape by the viewer presentation
  /// \param thePrs a selection
  /// \param theObject an output object
  /// \param theShape a shape of the selection
  virtual void getGeomSelection(const ModuleBase_ViewerPrs& thePrs,
                                ObjectPtr& theObject,
                                GeomShapePtr& theShape);

  /// Creates a backup of the current values of the attribute
  /// It should be realized in the specific widget because of different
  /// parameters of the current attribute
  /// \param theValid a boolean flag, if restore happens for valid parameters
  void restoreAttributeValue(const bool theValid);

protected:
  /// A reference to external objects manager
  PartSet_ExternalObjectsMgr* myExternalObjectMgr;

  /// Pointer to a sketch 
  CompositeFeaturePtr mySketch;

  bool myUseSketchPlane; /// state whether the sketch plane filter should be switched on
};

#endif