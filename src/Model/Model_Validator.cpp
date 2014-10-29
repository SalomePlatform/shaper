// File:        Model_Validator.cpp
// Created:     2 Jul 2014
// Author:      Mikhail PONIKAROV

#include <Model_Validator.h>
#include <Model_FeatureValidator.h>
#include <ModelAPI_Feature.h>
#include <ModelAPI_Attribute.h>
#include <ModelAPI_Data.h>
#include <ModelAPI_AttributeValidator.h>
#include <Events_Error.h>

const static std::string kDefaultId = "Model_FeatureValidator";

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
  std::map<std::string, std::map<std::string, AttrValidators> >::const_iterator aFeature = 
    myAttrs.find(theFeatureID);
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
  registerValidator(kDefaultId, new Model_FeatureValidator);
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
  std::map<std::string, ModelAPI_Validator*>::const_iterator it = myIDs.find(kDefaultId);
  if(it == myIDs.end())
    return;
  theValidators.push_back(it->second);
}

bool Model_ValidatorsFactory::validate(const boost::shared_ptr<ModelAPI_Feature>& theFeature) const
{
  // check feature validators first
  std::map<std::string, AttrValidators>::const_iterator aFeature = 
    myFeatures.find(theFeature->getKind());
  if (aFeature != myFeatures.end()) {
    AttrValidators::const_iterator aValidator = aFeature->second.begin();
    for(; aValidator != aFeature->second.end(); aValidator++) {
      std::map<std::string, ModelAPI_Validator*>::const_iterator aValFind = 
        myIDs.find(aValidator->first);
      if (aValFind == myIDs.end()) {
        Events_Error::send(std::string("Validator ") + aValidator->first + " was not registered");
        continue;
      }
      const ModelAPI_FeatureValidator* aFValidator = 
        dynamic_cast<const ModelAPI_FeatureValidator*>(aValFind->second);
      if (aFValidator) {
        if (!aFValidator->isValid(theFeature, aValidator->second))
          return false;
      }
    }
  }
  // check default validator
  std::map<std::string, ModelAPI_Validator*>::const_iterator aDefaultVal = myIDs.find(kDefaultId);
  if(aDefaultVal != myIDs.end()) {
    static const std::list<std::string> anEmptyArgList;
    const ModelAPI_FeatureValidator* aFValidator = 
      dynamic_cast<const ModelAPI_FeatureValidator*>(aDefaultVal->second);
    if (aFValidator) {
      if (!aFValidator->isValid(theFeature, anEmptyArgList))
        return false;
    }
  }
  // check all attributes for validity
  boost::shared_ptr<ModelAPI_Data> aData = theFeature->data();
  if (!aData->isValid())
    return false;
  static const std::string kAllTypes = "";
  std::map<std::string, std::map<std::string, AttrValidators> >::const_iterator aFeatureIter = 
    myAttrs.find(theFeature->getKind());
  if (aFeatureIter != myAttrs.cend()) {
    std::list<std::string> aLtAttributes = aData->attributesIDs(kAllTypes);
    std::list<std::string>::iterator anAttrIter = aLtAttributes.begin();
    for (; anAttrIter != aLtAttributes.end(); anAttrIter++) {
      std::map<std::string, AttrValidators>::const_iterator anAttr = 
          aFeatureIter->second.find(*anAttrIter);
      if (anAttr != aFeatureIter->second.end()) {
        AttrValidators::const_iterator aValIter = anAttr->second.cbegin();
        for (; aValIter != anAttr->second.cend(); aValIter++) {
          std::map<std::string, ModelAPI_Validator*>::const_iterator aFound = myIDs.find(
            aValIter->first);
          if (aFound == myIDs.end()) {
            Events_Error::send(std::string("Validator ") + aValIter->first + " was not registered");
          } else {
            const ModelAPI_AttributeValidator* anAttrValidator = 
              dynamic_cast<const ModelAPI_AttributeValidator*>(aFound->second);
            if (anAttrValidator) {
              AttributePtr anAttribute = theFeature->data()->attribute(*anAttrIter);
              if (!anAttrValidator->isValid(anAttribute, aValIter->second)) {
                  return false;
              }
            }
          }
        }
      }
    }
  }
  return true;
}

void Model_ValidatorsFactory::registerNotObligatory(std::string theFeature, std::string theAttribute)
{
  std::map<std::string, ModelAPI_Validator*>::const_iterator it = myIDs.find(kDefaultId);
  if (it != myIDs.end()) {
    Model_FeatureValidator* aValidator = dynamic_cast<Model_FeatureValidator*>(it->second);
    if (aValidator) {
      aValidator->registerNotObligatory(theFeature, theAttribute);
    }
  }
}

void Model_ValidatorsFactory::registerConcealment(std::string theFeature, std::string theAttribute)
{
  std::map<std::string, std::set<std::string> >::iterator aFind = myConcealed.find(theFeature);
  if (aFind == myConcealed.end()) {
    std::set<std::string> aNewSet;
    aNewSet.insert(theAttribute);
    myConcealed[theFeature] = aNewSet;
  } else {
    aFind->second.insert(theAttribute);
  }
}

bool Model_ValidatorsFactory::isConcealed(std::string theFeature, std::string theAttribute)
{
  std::map<std::string, std::set<std::string> >::iterator aFind = myConcealed.find(theFeature);
  return aFind != myConcealed.end() && aFind->second.find(theAttribute) != aFind->second.end();
}
