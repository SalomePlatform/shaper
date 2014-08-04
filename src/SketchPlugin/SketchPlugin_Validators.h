// File:        SketchPlugin_Validators.h
// Created:     01 Aug 2014
// Author:      Vitaly SMETANNIKOV

#ifndef SketchPlugin_Validators_H
#define SketchPlugin_Validators_H

#include "SketchPlugin.h"
#include <ModuleBase_FeatureValidator.h>


//! A class to validate a selection for Perpendicular constraint operation
class SketchPlugin_DistanceFeatureValidator: public ModuleBase_FeatureValidator
{
public:
  SKETCHPLUGIN_EXPORT virtual bool isValid(const FeaturePtr theFeature) const;
};

#endif