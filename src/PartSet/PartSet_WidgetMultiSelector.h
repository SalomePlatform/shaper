// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        PartSet_WidgetMultiSelector.h
// Created:     15 Apr 2015
// Author:      Natalia Ermolaeva


#ifndef PartSet_WidgetMultiSelector_H
#define PartSet_WidgetMultiSelector_H

#include "PartSet.h"

#include <ModuleBase_WidgetShapeSelector.h>

#include <ModelAPI_CompositeFeature.h>

class PartSet_ExternalObjectsMgr;

/**
* \ingroup Modules
* Customosation of ModuleBase_WidgetShapeSelector in order to provide 
* working with sketch specific objects.
*/
class PARTSET_EXPORT PartSet_WidgetMultiSelector: public ModuleBase_WidgetShapeSelector
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

  /// Set sketcher
  /// \param theSketch a sketcher object
  void setSketcher(CompositeFeaturePtr theSketch) { mySketch = theSketch; }

  /// Retrurns installed sketcher
  CompositeFeaturePtr sketch() const { return mySketch; }

protected:
  /// Creates a backup of the current values of the attribute
  /// It should be realized in the specific widget because of different
  /// parameters of the current attribute
  /// \param theValid a boolean flag, if restore happens for valid parameters
  void restoreAttributeValue(const bool theValid);

protected:
  PartSet_ExternalObjectsMgr* myExternalObjectMgr;
  /// Pointer to a sketch 
  CompositeFeaturePtr mySketch;
};

#endif