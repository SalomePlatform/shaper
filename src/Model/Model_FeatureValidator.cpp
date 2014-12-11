// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        Model_FeatureValidator.cpp
// Created:     8 Jul 2014
// Author:      Vitaly SMETANNIKOV

#include <Model_FeatureValidator.h>
#include <ModelAPI_Attribute.h>
#include <ModelAPI_Data.h>
#include <ModelAPI_Feature.h>
#include <ModelAPI_Object.h>

#include <list>
#include <memory>

bool Model_FeatureValidator::isValid(const std::shared_ptr<ModelAPI_Feature>& theFeature,
  const std::list<std::string>& theArguments) const
{
  std::shared_ptr<ModelAPI_Data> aData = theFeature->data();
  // "Action" features has no data, but still valid. e.g "Remove Part"  
  if (!aData) {
    return theFeature->isAction();
  }
  if (!aData->isValid())
    return false;
  const std::string kAllTypes = "";
  std::list<std::string> aLtAttributes = aData->attributesIDs(kAllTypes);
  std::list<std::string>::iterator it = aLtAttributes.begin();
  for (; it != aLtAttributes.end(); it++) {
    AttributePtr anAttr = aData->attribute(*it);
    if (!anAttr->isInitialized()) { // attribute is not initialized
      std::map<std::string, std::set<std::string> >::const_iterator aFeatureFind = 
        myNotObligatory.find(theFeature->getKind());
      if (aFeatureFind == myNotObligatory.end() || // and it is obligatory for filling
          aFeatureFind->second.find(*it) == aFeatureFind->second.end()) {
        return false;
      }
    }
  }
  return true;
}

void Model_FeatureValidator::registerNotObligatory(std::string theFeature, std::string theAttribute)
{
  std::set<std::string>& anAttrs = myNotObligatory[theFeature];
  anAttrs.insert(theAttribute);
}

bool Model_FeatureValidator::isNotObligatory(std::string theFeature, std::string theAttribute)
{
  std::set<std::string>& anAttrs = myNotObligatory[theFeature];
  return anAttrs.find(theAttribute) != anAttrs.end();
}
