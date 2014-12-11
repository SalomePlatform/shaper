// File:        ModuleBase_FilterFactory.h
// Created:     10 Dec 2014
// Author:      Natalia ERMOLAEVA

#include "ModuleBase_FilterFactory.h"
#include <ModuleBase_Filter.h>

#include <Model_FeatureValidator.h>
#include <ModelAPI_Feature.h>
#include <ModelAPI_Attribute.h>
#include <ModelAPI_Data.h>
#include <ModelAPI_AttributeValidator.h>
#include <Events_Error.h>


void ModuleBase_FilterFactory::registerFilter(const std::string& theID,
                                              Handle(ModuleBase_Filter) theFilter)
{
  /*if (myIDs.find(theID) != myIDs.end()) {
    Events_Error::send(std::string("Validator ") + theID + " is already registered");
  } else {
    myIDs[theID] = theValidator;
  }*/
}

void ModuleBase_FilterFactory::assignFilter(const std::string& theID,
  const std::string& theFeatureID,
  const std::string& theAttrID)
{
  /*
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
  //aFeature->second[theAttrID][theID] = theArguments;
  */
}

const SelectMgr_ListOfFilter& ModuleBase_FilterFactory::filters(const std::string& theFeatureID,
  const std::string& theAttrID/*,
  std::list<Handle(ModuleBase_Filter)>& theFilters/*,
  std::list<std::list<std::string> >& theArguments*/) const
{
  SelectMgr_ListOfFilter aFilters;
  return aFilters;
  /*  std::map<std::string, std::map<std::string, AttrValidators> >::const_iterator aFeature = 
    myAttrs.find(theFeatureID);
  if (aFeature != myAttrs.cend()) {
    std::map<std::string, AttrValidators>::const_iterator anAttr = aFeature->second.find(theAttrID);
    if (anAttr != aFeature->second.end()) {
      AttrValidators::const_iterator aValIter = anAttr->second.cbegin();
      for (; aValIter != anAttr->second.cend(); aValIter++) {
        std::map<std::string, Handle(ModuleBase_Filter)>::const_iterator aFound = myIDs.find(
          aValIter->first);
        if (aFound == myIDs.end()) {
          Events_Error::send(std::string("Validator ") + aValIter->first + " was not registered");
        } else {
          theValidators.push_back(aFound->second);
          theArguments.push_back(aValIter->second);
        }
      }
    }
  }*/
}

ModuleBase_FilterFactory::ModuleBase_FilterFactory()
{
  //const static std::string kDefaultId = "Model_FeatureValidator";
  //registerValidator(kDefaultId, new Model_FeatureValidator);
}

const Handle(ModuleBase_Filter) ModuleBase_FilterFactory::validator(const std::string& theID) const
{
/*  std::map<std::string, Handle(ModuleBase_Filter)>::const_iterator aIt = myIDs.find(theID);
  if (aIt != myIDs.end()) {
    return aIt->second;
  }*/
  return NULL;
}

void ModuleBase_FilterFactory::addDefaultValidators(std::list<Handle(ModuleBase_Filter)>& theValidators) const
{
/*  const static std::string kDefaultId = "Model_FeatureValidator";
  std::map<std::string, Handle(ModuleBase_Filter)>::const_iterator it = myIDs.find(kDefaultId);
  if(it == myIDs.end())
    return;
  theValidators.push_back(it->second);*/
}

bool ModuleBase_FilterFactory::validate(const std::shared_ptr<ModelAPI_Feature>& theFeature) const
{
/*  const static std::string kDefaultId = "Model_FeatureValidator";
  // check feature validators first
  std::map<std::string, AttrValidators>::const_iterator aFeature = 
    myFeatures.find(theFeature->getKind());
  if (aFeature != myFeatures.end()) {
    AttrValidators::const_iterator aValidator = aFeature->second.begin();
    for(; aValidator != aFeature->second.end(); aValidator++) {
      std::map<std::string, Handle(ModuleBase_Filter)>::const_iterator aValFind = 
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
  std::map<std::string, Handle(ModuleBase_Filter)>::const_iterator aDefaultVal = myIDs.find(kDefaultId);
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
  std::shared_ptr<ModelAPI_Data> aData = theFeature->data();
  // Validity of data is checked by "Model_FeatureValidator" (kDefaultId)
  // if (!aData || !aData->isValid())
  //   return false;
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
          std::map<std::string, Handle(ModuleBase_Filter)>::const_iterator aFound = myIDs.find(
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
  */
 return true;
}

void ModuleBase_FilterFactory::registerNotObligatory(std::string theFeature, std::string theAttribute)
{
/*  const static std::string kDefaultId = "Model_FeatureValidator";
  std::map<std::string, Handle(ModuleBase_Filter)>::const_iterator it = myIDs.find(kDefaultId);
  if (it != myIDs.end()) {
    Model_FeatureValidator* aValidator = dynamic_cast<Model_FeatureValidator*>(it->second);
    if (aValidator) {
      aValidator->registerNotObligatory(theFeature, theAttribute);
    }
  }*/
}

bool ModuleBase_FilterFactory::isNotObligatory(std::string theFeature, std::string theAttribute)
{
/*  const static std::string kDefaultId = "Model_FeatureValidator";
  std::map<std::string, Handle(ModuleBase_Filter)>::const_iterator it = myIDs.find(kDefaultId);
  if (it != myIDs.end()) {
    Model_FeatureValidator* aValidator = dynamic_cast<Model_FeatureValidator*>(it->second);
    if (aValidator) {
      return aValidator->isNotObligatory(theFeature, theAttribute);
    }
  }*/
  return false; // default
}

void ModuleBase_FilterFactory::registerConcealment(std::string theFeature, std::string theAttribute)
{
/*  std::map<std::string, std::set<std::string> >::iterator aFind = myConcealed.find(theFeature);
  if (aFind == myConcealed.end()) {
    std::set<std::string> aNewSet;
    aNewSet.insert(theAttribute);
    myConcealed[theFeature] = aNewSet;
  } else {
    aFind->second.insert(theAttribute);
  }*/
}

bool ModuleBase_FilterFactory::isConcealed(std::string theFeature, std::string theAttribute)
{
  /*std::map<std::string, std::set<std::string> >::iterator aFind = myConcealed.find(theFeature);
  return aFind != myConcealed.end() && aFind->second.find(theAttribute) != aFind->second.end();*/
  return true;
}
