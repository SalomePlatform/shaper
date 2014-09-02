// File:        SketchPlugin_Validators.cpp
// Created:     01 Aug 2014
// Author:      Vitaly SMETANNIKOV

#include <ExchangePlugin_Validators.h>
#include <ModelAPI_Feature.h>
#include <ModelAPI_Object.h>
#include <ModelAPI_PluginManager.h>

#include <list>
#include <string>


bool ExchangePlugin_ImportFormatValidator::isValid(const FeaturePtr& theFeature,
                                                 const std::list<std::string>& theArguments,
                                                 const ObjectPtr& theObject) const
{
  PluginManagerPtr aMgr = ModelAPI_PluginManager::get();
  ModelAPI_ValidatorsFactory* aFactory = aMgr->validators();
  return false;
}

