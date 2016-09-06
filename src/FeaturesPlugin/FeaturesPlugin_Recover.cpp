// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:        FeaturesPlugin_Recover.cpp
// Created:     29 Jul 2016
// Author:      Natalia ERMOLAEVA

#include "FeaturesPlugin_Recover.h"

#include <ModelAPI_Data.h>
#include <ModelAPI_Document.h>
#include <ModelAPI_AttributeReference.h>
#include <ModelAPI_AttributeRefList.h>
#include <ModelAPI_AttributeBoolean.h>
#include <ModelAPI_Session.h>
#include <ModelAPI_Validator.h>
#include <ModelAPI_Result.h>
#include <ModelAPI_Tools.h>

using namespace std;

FeaturesPlugin_Recover::FeaturesPlugin_Recover()
{
}

void FeaturesPlugin_Recover::initAttributes()
{
  data()->addAttribute(BASE_FEATURE(), ModelAPI_AttributeReference::typeId());
  data()->addAttribute(RECOVERED_ENTITIES(), ModelAPI_AttributeRefList::typeId());
  data()->addAttribute(PERSISTENT(), ModelAPI_AttributeBoolean::typeId());

  myPersistent = boolean(PERSISTENT())->value();
  synchronizeRegistered();
}

void FeaturesPlugin_Recover::execute()
{
  synchronizeRegistered();
}

void FeaturesPlugin_Recover::attributeChanged(const std::string& theID)
{
  synchronizeRegistered();
}

void FeaturesPlugin_Recover::synchronizeRegistered()
{
  FeaturePtr aBase = baseFeature();
  bool aNewPersistent = boolean(PERSISTENT())->value();
  if (aNewPersistent != myPersistent || myCurrentBase != aBase)
    clearRegistered();

  std::set<ObjectPtr> aRecoveredInList;
  // add unconcealed which are not in the myRegistered map
  if (isStable() && !isDisabled()) { // if unstable, clear any unconcealment effect
    AttributeRefListPtr aRecovered = reflist(RECOVERED_ENTITIES());
    for(int anIndex = aRecovered->size() - 1; anIndex >= 0; anIndex--) {
      ObjectPtr anObj = aRecovered->object(anIndex);
      aRecoveredInList.insert(anObj);
      if (myRegistered.find(anObj) == myRegistered.end()) {
        // not found, so register a new
        ResultPtr aRes = std::dynamic_pointer_cast<ModelAPI_Result>(anObj);
        if (aRes.get()) { // this may be on first update after "open"
          ModelAPI_Session::get()->validators()->registerUnconcealment(
            aRes, aNewPersistent ? FeaturePtr() : aBase);
          myRegistered.insert(anObj);
        }
      }
    }
  }
  // remove unconcealed which are not in the stored list, but in the map
  std::set<std::shared_ptr<ModelAPI_Object> >::iterator aMyReg = myRegistered.begin();
  while(aMyReg != myRegistered.end()) {
    if (aRecoveredInList.find(*aMyReg) == aRecoveredInList.end()) {
      ResultPtr aRes = std::dynamic_pointer_cast<ModelAPI_Result>(*aMyReg);
      ModelAPI_Session::get()->validators()->disableUnconcealment(
        aRes, aNewPersistent ? FeaturePtr() : myCurrentBase);
      myRegistered.erase(aMyReg);
      aMyReg = myRegistered.begin(); // restart iteration because after erase iterator may be bad
    } else {
      aMyReg++;
    }
  }
  myCurrentBase = aBase;
  myPersistent = aNewPersistent;
}

void FeaturesPlugin_Recover::erase()
{
  // clears myRegistered before all information is destroyed
  clearRegistered();
  ModelAPI_Feature::erase();
}

bool FeaturesPlugin_Recover::setStable(const bool theFlag)
{
  bool aRes = ModelAPI_Feature::setStable(theFlag);
  synchronizeRegistered();
  return aRes;
}

bool FeaturesPlugin_Recover::setDisabled(const bool theFlag)
{
  bool aRes = ModelAPI_Feature::setDisabled(theFlag);
  synchronizeRegistered();
  return aRes;
}

FeaturePtr FeaturesPlugin_Recover::baseFeature()
{
  // for the current moment it can be result of feature of feature: GUI is not debugged
  ObjectPtr aBaseObj = reference(BASE_FEATURE())->value();
  FeaturePtr aResult;
  if (aBaseObj.get() == NULL)
    return aResult;
  aResult = std::dynamic_pointer_cast<ModelAPI_Feature>(aBaseObj);
  if (aResult.get() == NULL)
    aResult = aBaseObj->document()->feature(std::dynamic_pointer_cast<ModelAPI_Result>(aBaseObj));
  return aResult;
}

void FeaturesPlugin_Recover::clearRegistered()
{
  std::set<std::shared_ptr<ModelAPI_Object> >::iterator aMyReg = myRegistered.begin();
  for(; aMyReg != myRegistered.end(); aMyReg++) {
    ResultPtr aRes = std::dynamic_pointer_cast<ModelAPI_Result>(*aMyReg);
    ModelAPI_Session::get()->validators()->disableUnconcealment(
      aRes, myPersistent ? FeaturePtr() : myCurrentBase);
  }
  myRegistered.clear();
}
