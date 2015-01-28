// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        PartSet_WidgetShapeSelector.h
// Created:     27 Nov 2014
// Author:      Vitaly Smetannikov


#ifndef PartSet_WidgetShapeSelector_H
#define PartSet_WidgetShapeSelector_H

#include "PartSet.h"

#include <ModuleBase_WidgetShapeSelector.h>

#include <ModelAPI_CompositeFeature.h>


/**
* \ingroup Module
* Customosation of ModuleBase_WidgetShapeSelector in order to provide 
* working with sketch specific objects.
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
    const Config_WidgetAPI* theData, const std::string& theParentId)
    : ModuleBase_WidgetShapeSelector(theParent, theWorkshop, theData, theParentId) {}

  virtual ~PartSet_WidgetShapeSelector() {}

  virtual bool storeValue() const;

  /// Set sketcher
  /// \param theSketch a sketcher object
  void setSketcher(CompositeFeaturePtr theSketch) { mySketch = theSketch; }

  /// Retrurns installed sketcher
  CompositeFeaturePtr sketch() const { return mySketch; }

protected:
  /// Check the selected with validators if installed
  virtual bool isValid(ObjectPtr theObj, std::shared_ptr<GeomAPI_Shape> theShape);

private:
  /// Pointer to a sketch 
  CompositeFeaturePtr mySketch;
};

/**
* \ingroup Module
* Customosation of ModuleBase_WidgetShapeSelector in order to provide 
* working with constraints.
*/
class PARTSET_EXPORT PartSet_WidgetConstraintShapeSelector: public ModuleBase_WidgetShapeSelector
{
Q_OBJECT
 public:
  /// Constructor
  /// \param theParent the parent object
  /// \param theWorkshop instance of workshop interface
  /// \param theData the widget configuation. The attribute of the model widget is obtained from
  /// \param theParentId is Id of a parent of the current attribute
  PartSet_WidgetConstraintShapeSelector(QWidget* theParent, ModuleBase_IWorkshop* theWorkshop,
    const Config_WidgetAPI* theData, const std::string& theParentId)
    : ModuleBase_WidgetShapeSelector(theParent, theWorkshop, theData, theParentId) {}

  virtual ~PartSet_WidgetConstraintShapeSelector() {}

  /// Saves the internal parameters to the given feature
  virtual bool storeValue() const;

  /// Set sketcher
  /// \param theSketch a sketcher object
  void setSketcher(CompositeFeaturePtr theSketch) { mySketch = theSketch; }

  /// Retrurns installed sketcher
  CompositeFeaturePtr sketch() const { return mySketch; }

private:
  /// Pointer to a sketch 
  CompositeFeaturePtr mySketch;
};

#endif