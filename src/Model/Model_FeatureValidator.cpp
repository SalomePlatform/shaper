// File:        Model_FeatureValidator.cpp
// Created:     8 Jul 2014
// Author:      Vitaly SMETANNIKOV

#include <Model_FeatureValidator.h>
#include <ModelAPI_Attribute.h>
#include <ModelAPI_Data.h>
#include <ModelAPI_Feature.h>
#include <ModelAPI_Object.h>

#include <list>
#include <boost/shared_ptr.hpp>

bool Model_FeatureValidator::isValid(const boost::shared_ptr<ModelAPI_Feature>& theFeature) const
{
  boost::shared_ptr<ModelAPI_Data> aData = theFeature->data();
  if (!aData->isValid())
    return false;
  const std::string kAllTypes = "";
  std::list<AttributePtr> aLtAttributes = aData->attributes(kAllTypes);
  std::list<AttributePtr>::iterator it = aLtAttributes.begin();
  for (; it != aLtAttributes.end(); it++) {
    if (!(*it)->isInitialized())
      return false;
  }
  return true;
}
