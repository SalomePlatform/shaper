// File:        Model_Validator.cpp
// Created:     2 Jul 2014
// Author:      Mikhail PONIKAROV

#include <Model_Validator.h>
#include <Model_ResultValidators.h>
#include <Model_FeatureValidator.h>
#include <ModelAPI_Feature.h>
#include <Events_Error.h>

void Model_ValidatorsFactory::registerValidator(
  const std::string& theID, ModelAPI_Validator* theValidator)
{
  if (myIDs.find(theID) != myIDs.end()) {
    Events_Error::send(std::string("Validator ") + theID + " is already registered");
  } else {
    myIDs[theID] = theValidator;
  }
}

void Model_ValidatorsFactory::assignValidator(
  const std::string& theID, const std::string& theFeatureID)
{
  if (myFeatures.find(theFeatureID) == myFeatures.end()) {
    myFeatures[theFeatureID] = std::set<std::string>();
  }
  myFeatures[theFeatureID].insert(theID);
}

void Model_ValidatorsFactory::assignValidator(const std::string& theID, 
  const std::string& theFeatureID, const std::string& theAttrID, 
  const std::list<std::string>& theArguments)
{
  // create feature-structures if not exist
  std::map<std::string, std::map<std::string, AttrValidators> >::iterator aFeature = 
    myAttrs.find(theFeatureID);
  if (aFeature == myAttrs.end()) {
    myAttrs[theFeatureID] = std::map<std::string, AttrValidators>();
    aFeature = myAttrs.find(theFeatureID);
  }
  // add attr-structure if not exist, or generate error if already exist
  std::map<std::string, AttrValidators>::iterator anAttr = aFeature->second.find(theAttrID);
  if (anAttr == aFeature->second.end()) {
    aFeature->second[theAttrID] = AttrValidators();
  }
  aFeature->second[theAttrID].insert(
    std::pair<std::string, std::list<std::string> >(theID, theArguments));
}

void Model_ValidatorsFactory::validators(
  const std::string& theFeatureID, std::list<ModelAPI_Validator*>& theResult ) const
{
  std::map<std::string, std::set<std::string> >::const_iterator aFeature =
    myFeatures.find(theFeatureID);
  if (aFeature != myFeatures.cend()) {
    std::set<std::string>::const_iterator aValIter = aFeature->second.cbegin();
    for(; aValIter != aFeature->second.cend(); aValIter++) {
      std::map<std::string, ModelAPI_Validator*>::const_iterator aFound = myIDs.find(*aValIter);
      if (aFound == myIDs.end()) {
        Events_Error::send(std::string("Validator ") + *aValIter + " was not registered");
      } else {
        theResult.push_back(aFound->second);
      }
    }
  }
}

void Model_ValidatorsFactory::validators(const std::string& theFeatureID, 
  const std::string& theAttrID, std::list<ModelAPI_Validator*>& theValidators, 
  std::list<std::list<std::string> >& theArguments) const
{
  std::map<std::string, std::map<std::string, AttrValidators> >::const_iterator aFeature =
    myAttrs.find(theFeatureID);
  if (aFeature != myAttrs.cend()) {
    std::map<std::string, AttrValidators>::const_iterator anAttr =
      aFeature->second.find(theAttrID);
    if (anAttr != aFeature->second.end()) {
      AttrValidators::const_iterator aValIter = anAttr->second.cbegin();
      for(; aValIter != anAttr->second.cend(); aValIter++) {
        std::map<std::string, ModelAPI_Validator*>::const_iterator aFound = 
          myIDs.find(aValIter->first);
        if (aFound == myIDs.end()) {
          Events_Error::send(std::string("Validator ") + aValIter->first + " was not registered");
        } else {
          theValidators.push_back(aFound->second);
          theArguments.push_back(aValIter->second);
        }
      }
    }
  }
}

Model_ValidatorsFactory::Model_ValidatorsFactory() : ModelAPI_ValidatorsFactory()
{
  registerValidator("Model_ResultPointValidator", new Model_ResultPointValidator);
  registerValidator("Model_ResultLineValidator", new Model_ResultLineValidator);
  registerValidator("Model_ResultArcValidator", new Model_ResultArcValidator);
  registerValidator("Model_FeatureValidator", new Model_FeatureValidator);
}


const ModelAPI_Validator* Model_ValidatorsFactory::validator(const std::string& theID) const
{
  std::map<std::string, ModelAPI_Validator*>::const_iterator aIt = myIDs.find(theID);
  if (aIt != myIDs.end()) {
    return aIt->second;
  }
  return NULL;
}
