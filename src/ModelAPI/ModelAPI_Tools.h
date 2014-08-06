// File:        ModelAPI_Tools.h
// Created:     06 Aug 2014
// Author:      Vitaly Smetannikov

#ifndef ModelAPI_Tools_HeaderFile
#define ModelAPI_Tools_HeaderFile

#include "ModelAPI.h"
#include <ModelAPI_Result.h>
#include <GeomAPI_Shape.h>

namespace ModelAPI_Tools 
{
  /// Returns shape from the given Result object
  MODELAPI_EXPORT boost::shared_ptr<GeomAPI_Shape> shape(const ResultPtr& theResult);
};

#endif