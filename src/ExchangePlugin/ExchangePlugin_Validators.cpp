// File:        SketchPlugin_Validators.cpp
// Created:     01 Aug 2014
// Author:      Vitaly SMETANNIKOV

#include <ExchangePlugin_Validators.h>
#include <ModelAPI_Feature.h>
#include <ModelAPI_Object.h>
#include <ModelAPI_Session.h>

#include <list>
#include <string>


bool ExchangePlugin_ImportFormatValidator::isValid(
  const AttributePtr& theAttribute, const std::list<std::string>& theArguments) const
{
  SessionPtr aMgr = ModelAPI_Session::get();
  ModelAPI_ValidatorsFactory* aFactory = aMgr->validators();
  return false;
}

