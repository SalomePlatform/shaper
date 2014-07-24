// File:        Model_Validator.cpp
// Created:     2 Jul 2014
// Author:      Mikhail PONIKAROV

#include <Model_Validator.h>
#include <ModelAPI_Feature.h>
#include <Events_Error.h>

using namespace std;

void Model_ValidatorsFactory::registerValidator(
  const string& theID, ModelAPI_Validator* theValidator)
{
  if (myIDs.find(theID) != myIDs.end()) {
    Events_Error::send(string("Validator ") + theID + " is already registered");
  } else {
    myIDs[theID] = theValidator;
  }
}

void Model_ValidatorsFactory::assignValidator(const string& theID, const string& theFeatureID)
{
  bool isError = false;
  map<string, ModelAPI_Validator*>::iterator aVal = myIDs.find(theID);
  if (aVal == myIDs.end()) {
    Events_Error::send(
      string("Validator ") + theID + " for feature " + theFeatureID + " was not registered");
    isError = true;
  }
  if (myFeatures.find(theFeatureID) != myFeatures.end()) {
    // TODO: it is called many times now because of not-optimized XML reader
      //Events_Error::send(
      //  string("Validator for feature ") + theFeatureID + " is already registered");
    isError = true;
  }
  if (!isError)
    myFeatures[theFeatureID] = aVal->second;
}

void Model_ValidatorsFactory::assignValidator(const string& theID, 
  const string& theFeatureID, const string& theAttrID, const list<string>& theArguments)
{
  bool isError = false;
  map<string, ModelAPI_Validator*>::iterator aVal = myIDs.find(theID);
  if (aVal == myIDs.end()) {
    Events_Error::send(
      string("Validator ") + theID + " for feature " + theFeatureID + " was not registered");
    isError = true;
  }
  // create feature-structures if not exist
  map<string, map<string, pair<ModelAPI_Validator*, list<string> > > >::iterator
    aFeature = myAttrs.find(theFeatureID);
  if (aFeature == myAttrs.end()) {
    myAttrs[theFeatureID] = map<string, pair<ModelAPI_Validator*, list<string> > >();
    aFeature = myAttrs.find(theFeatureID);
  }
  // add attr-structure if not exist, or generate error if already exist
  map<string, pair<ModelAPI_Validator*, list<string> > >::iterator 
    anAttr = aFeature->second.find(theAttrID);
  if (anAttr == aFeature->second.end()) {
    if (!isError) {
      aFeature->second[theAttrID] = 
        pair<ModelAPI_Validator*, list<string> >(aVal->second, theArguments);
    }
  } else {
    // TODO: it is called many times now because of not-optimized XML reader
    //Events_Error::send(
    //  string("Validator ") + theID + " for feature " + theFeatureID +
    //          "attribute " + theAttrID + " is already registered");
    isError = true;
  }
}

const ModelAPI_Validator* Model_ValidatorsFactory::validator(const string& theFeatureID) const
{
  map<string, ModelAPI_Validator*>::const_iterator aFeature = myFeatures.find(theFeatureID);
  if (aFeature != myFeatures.cend())
    return aFeature->second;
  return NULL; // not found
}

const ModelAPI_Validator* Model_ValidatorsFactory::validator(
  const std::string& theFeatureID, const std::string& theAttrID) const
{
  map<string, map<string, pair<ModelAPI_Validator*, list<string> > > >::const_iterator
    aFeature = myAttrs.find(theFeatureID);
  if (aFeature == myAttrs.cend()) return NULL; // feature is not found
  map<string, pair<ModelAPI_Validator*, list<string> > >::const_iterator 
    anAttr = aFeature->second.find(theAttrID);
  if (anAttr == aFeature->second.cend()) return NULL; // attribute is not found
  return anAttr->second.first;
}

/*bool Model_ValidatorsFactory::validate(
  const boost::shared_ptr<ModelAPI_Feature>& theFeature, const string& theAttrID ) const
{
  map<string, map<string, pair<ModelAPI_Validator*, list<string> > > >::const_iterator
    aFeature = myAttrs.find(theFeature->getKind());
  if (aFeature == myAttrs.cend()) return true; // feature is not found
  map<string, pair<ModelAPI_Validator*, list<string> > >::const_iterator 
    anAttr = aFeature->second.find(theAttrID);
  if (anAttr == aFeature->second.cend()) return true; // attribute is not found
  return anAttr->second.first->validate(theFeature, theAttrID, anAttr->second.second);
}*/

Model_ValidatorsFactory::Model_ValidatorsFactory() : ModelAPI_ValidatorsFactory()
{
}
