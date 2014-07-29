// File:        ModuleBase_Tools.cpp
// Created:     11 July 2014
// Author:      Vitaly Smetannikov

#include "ModuleBase_Tools.h"
#include <ModelAPI_ResultBody.h>
#include <ModelAPI_ResultConstruction.h>
#include <ModelAPI_Document.h>

namespace ModuleBase_Tools
{

//******************************************************************
boost::shared_ptr<GeomAPI_Shape> shape(ResultPtr theResult)
{
  ResultBodyPtr aBody = boost::dynamic_pointer_cast<ModelAPI_ResultBody>(theResult);
  if (aBody) return aBody->shape();
  ResultConstructionPtr aConstruct = boost::dynamic_pointer_cast<ModelAPI_ResultConstruction>(theResult);
  if (aConstruct) return aConstruct->shape();
  return boost::shared_ptr<GeomAPI_Shape>();
}

//******************************************************************
FeaturePtr feature(ObjectPtr theObject)
{
  FeaturePtr aFeature = boost::dynamic_pointer_cast<ModelAPI_Feature>(theObject);
  if (!aFeature) {
    ResultPtr aResult = boost::dynamic_pointer_cast<ModelAPI_Result>(theObject);
    if (aResult) {
      DocumentPtr aDoc = aResult->document();
      return aDoc->feature(aResult);
    }
  }
  return aFeature;
}

}
