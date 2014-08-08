// File:        SketchPlugin_Validators.h
// Created:     01 Aug 2014
// Author:      Vitaly SMETANNIKOV

#ifndef SketchPlugin_Validators_H
#define SketchPlugin_Validators_H

#include "SketchPlugin.h"
//#include <ModuleBase_FeatureValidator.h>
#include <ModelAPI_AttributeValidator.h>


class SketchPlugin_DistanceAttrValidator: public ModelAPI_AttributeValidator
{
public:
  virtual bool isValid(const FeaturePtr& theFeature, 
                       const std::list<std::string>& theArguments,
                       const ObjectPtr& theObject) const;

};

#endif