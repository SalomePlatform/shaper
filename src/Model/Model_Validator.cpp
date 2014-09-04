// File:        Model_Validator.cpp
// Created:     2 Jul 2014
// Author:      Mikhail PONIKAROV

#include <Model_Validator.h>
#include <Model_FeatureValidator.h>
#include <ModelAPI_Feature.h>
#include <Events_Error.h>

void Model_ValidatorsFactory::registerValidator(const std::string& theID,
                                                ModelAPI_Validator* theValidator)
{
  if (myIDs.find(theID) != myIDs.end()) {
    Events_Error::send(std::string("Validator ") + theID + " is already registered");
  } else {
    myIDs[theID] = theValidator;
  }
}

void Model_ValidatorsFactory::assignValidator(const std::string& theID,
                                              const std::string& theFeatureID)
{
  if (myFeatures.find(theFeatureID) == myFeatures.end()) {
    myFeatures[theFeatureID] = AttrValidators();
  }
  if (myFeatures[theFeatureID].find(theID) != myFeatures[theFeatureID].end()) {
    //Events_Error::send(std::string("Validator ") + theID + 
    //  " for feature " + theFeatureID + "is already registered");
  } else {
    myFeatures[theFeatureID][theID] = std::list<std::string>();
  }
}

void Model_ValidatorsFactory::assignValidator(const std::string& theID,
                                              const std::string& theFeatureID,
                                              const std::list<std::string>& theArguments)
{
  if (myFeatures.find(theFeatureID) == myFeatures.end()) {
    myFeatures[theFeatureID] = AttrValidators();
  }

  if (myFeatures[theFeatureID].find(theID) != myFeatures[theFeatureID].end()) {
    //Events_Error::send(std::string("Validator ") + theID + 
    //  " for feature " + theFeatureID + "is already registered");
  } else {
    myFeatures[theFeatureID][theID] = theArguments;
  }
}

void Model_ValidatorsFactory::assignValidator(const std::string& theID,
                                              const std::string& theFeatureID,
                                              const std::string& theAttrID,
                                              const std::list<std::string>& theArguments)
{
  // create feature-structures if not exist
  std::map<std::string, std::map<std::string, AttrValidators> >::iterator aFeature = myAttrs.find(
      theFeatureID);
  if (aFeature == myAttrs.end()) {
    myAttrs[theFeatureID] = std::map<std::string, AttrValidators>();
    aFeature = myAttrs.find(theFeatureID);
  }
  // add attr-structure if not exist, or generate error if already exist
  std::map<std::string, AttrValidators>::iterator anAttr = aFeature->second.find(theAttrID);
  if (anAttr == aFeature->second.end()) {
    aFeature->second[theAttrID] = AttrValidators();
  }
  aFeature->second[theAttrID][theID] = theArguments;
}

void Model_ValidatorsFactory::validators(const std::string& theFeatureID,
                                         std::list<ModelAPI_Validator*>& theResult,
                                         std::list<std::list<std::string> >& theArguments) const
{
  std::map<std::string, AttrValidators>::const_iterator aFeature = myFeatures.find(theFeatureID);
  if (aFeature != myFeatures.cend()) {
    AttrValidators::const_iterator aValIter = aFeature->second.cbegin();
    for (; aValIter != aFeature->second.cend(); aValIter++) {
      std::map<std::string, ModelAPI_Validator*>::const_iterator aFound = 
        myIDs.find(aValIter->first);
      if (aFound == myIDs.end()) {
        Events_Error::send(std::string("Validator ") + aValIter->first + " was not registered");
      } else {
        theResult.push_back(aFound->second);
        theArguments.push_back(aValIter->second);
      }
    }
  }
  addDefaultValidators(theResult);
}

void Model_ValidatorsFactory::validators(const std::string& theFeatureID,
                                         const std::string& theAttrID,
                                         std::list<ModelAPI_Validator*>& theValidators,
                                         std::list<std::list<std::string> >& theArguments) const
{
  std::map<std::string, std::map<std::string, AttrValidators> >::const_iterator aFeature = myAttrs
      .find(theFeatureID);
  if (aFeature != myAttrs.cend()) {
    std::map<std::string, AttrValidators>::const_iterator anAttr = aFeature->second.find(theAttrID);
    if (anAttr != aFeature->second.end()) {
      AttrValidators::const_iterator aValIter = anAttr->second.cbegin();
      for (; aValIter != anAttr->second.cend(); aValIter++) {
        std::map<std::string, ModelAPI_Validator*>::const_iterator aFound = myIDs.find(
            aValIter->first);
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

Model_ValidatorsFactory::Model_ValidatorsFactory()
    : ModelAPI_ValidatorsFactory()
{
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

void Model_ValidatorsFactory::addDefaultValidators(std::list<ModelAPI_Validator*>& theValidators) const
{
  std::string anId = "Model_FeatureValidator";
  std::map<std::string, ModelAPI_Validator*>::const_iterator it = myIDs.find(anId);
  if(it == myIDs.end())
    return;
  theValidators.push_back(it->second);
}
