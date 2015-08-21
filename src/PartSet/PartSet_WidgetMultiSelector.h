// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        PartSet_WidgetMultiSelector.h
// Created:     15 Apr 2015
// Author:      Natalia Ermolaeva


#ifndef PartSet_WidgetMultiSelector_H
#define PartSet_WidgetMultiSelector_H

#include "PartSet.h"

#include <ModuleBase_WidgetMultiSelector.h>
#include <ModuleBase_ViewerPrs.h>

#include <ModelAPI_CompositeFeature.h>

class PartSet_ExternalObjectsMgr;

/**
* \ingroup Modules
* Customosation of ModuleBase_WidgetMultiSelector in order to provide 
* working with sketch specific objects and creation of external objects.
*/
class PARTSET_EXPORT PartSet_WidgetMultiSelector: public ModuleBase_WidgetMultiSelector
{
Q_OBJECT
 public:
  /// Constructor
  /// \param theParent the parent object
  /// \param theWorkshop instance of workshop interface
  /// \param theData the widget configuation. The attribute of the model widget is obtained from
  /// \param theParentId is Id of a parent of the current attribute
  PartSet_WidgetMultiSelector(QWidget* theParent, ModuleBase_IWorkshop* theWorkshop,
                              const Config_WidgetAPI* theData, const std::string& theParentId);

  virtual ~PartSet_WidgetMultiSelector();

  /// Defines if it is supposed that the widget should interact with the viewer.
  virtual bool isViewerSelector() { return true; }

  /// Set sketcher
  /// \param theSketch a sketcher object
  void setSketcher(CompositeFeaturePtr theSketch) { mySketch = theSketch; }

  /// Retrurns installed sketcher
  CompositeFeaturePtr sketch() const { return mySketch; }

protected:
  /// Checks the widget validity. By default, it returns true.
  /// \param theValue a selected presentation in the view
  /// \return a boolean value
  virtual bool isValidSelectionCustom(const ModuleBase_ViewerPrs& thePrs);

  /// Creates a backup of the current values of the attribute
  /// It should be realized in the specific widget because of different
  /// parameters of the current attribute
  /// \param theValid a boolean flag, if restore happens for valid parameters
  void restoreAttributeValue(const bool theValid);

  /// Return an object and geom shape by the viewer presentation
  /// \param thePrs a selection
  /// \param theObject an output object
  /// \param theShape a shape of the selection
  virtual void getGeomSelection(const ModuleBase_ViewerPrs& thePrs,
                                ObjectPtr& theObject,
                                GeomShapePtr& theShape);

protected:
  PartSet_ExternalObjectsMgr* myExternalObjectMgr;
  /// Pointer to a sketch 
  CompositeFeaturePtr mySketch;
};

#endif