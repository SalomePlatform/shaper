// File:        ModelAPI_Tools.cpp
// Created:     06 Aug 2014
// Author:      Vitaly Smetannikov

#include "ModelAPI_Tools.h"
#include <ModelAPI_ResultBody.h>
#include <ModelAPI_ResultConstruction.h>

namespace ModelAPI_Tools {

boost::shared_ptr<GeomAPI_Shape> shape(const ResultPtr& theResult)
{
  ResultBodyPtr aBody = boost::dynamic_pointer_cast<ModelAPI_ResultBody>(theResult);
  if (aBody)
    return aBody->shape();
  ResultConstructionPtr aConstruct = boost::dynamic_pointer_cast<ModelAPI_ResultConstruction>(
      theResult);
  if (aConstruct)
    return aConstruct->shape();
  return boost::shared_ptr<GeomAPI_Shape>();
}

}
