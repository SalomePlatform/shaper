// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        PartSet_WidgetShapeSelector.h
// Created:     27 Nov 2014
// Author:      Vitaly Smetannikov


#ifndef PartSet_WidgetShapeSelector_H
#define PartSet_WidgetShapeSelector_H

#include "PartSet.h"

#include <ModuleBase_WidgetShapeSelector.h>

#include <ModelAPI_CompositeFeature.h>

class PARTSET_EXPORT PartSet_WidgetShapeSelector: public ModuleBase_WidgetShapeSelector
{
Q_OBJECT
 public:
  PartSet_WidgetShapeSelector(QWidget* theParent, ModuleBase_IWorkshop* theWorkshop,
    const Config_WidgetAPI* theData, const std::string& theParentId)
    : ModuleBase_WidgetShapeSelector(theParent, theWorkshop, theData, theParentId) {}

  virtual ~PartSet_WidgetShapeSelector() {}

  /// Saves the internal parameters to the given feature
  virtual bool storeValue() const;

  void setSketcher(CompositeFeaturePtr theSketch) { mySketch = theSketch; }
  CompositeFeaturePtr sketch() const { return mySketch; }

private:
  CompositeFeaturePtr mySketch;
};


class PARTSET_EXPORT PartSet_WidgetConstraintShapeSelector: public ModuleBase_WidgetShapeSelector
{
Q_OBJECT
 public:
  PartSet_WidgetConstraintShapeSelector(QWidget* theParent, ModuleBase_IWorkshop* theWorkshop,
    const Config_WidgetAPI* theData, const std::string& theParentId)
    : ModuleBase_WidgetShapeSelector(theParent, theWorkshop, theData, theParentId) {}

  virtual ~PartSet_WidgetConstraintShapeSelector() {}

  /// Saves the internal parameters to the given feature
  virtual bool storeValue() const;

  void setSketcher(CompositeFeaturePtr theSketch) { mySketch = theSketch; }
  CompositeFeaturePtr sketch() const { return mySketch; }

private:
  CompositeFeaturePtr mySketch;
};

#endif