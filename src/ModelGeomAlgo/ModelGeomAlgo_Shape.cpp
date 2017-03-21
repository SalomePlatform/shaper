// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        ModelAPI_Tools.cpp
// Created:     19 Mar 2017
// Author:      Natalia ERMOLAEVA

#include <GeomAPI_Shape.h>

#include "ModelGeomAlgo_Shape.h"

#include <ModelAPI_Feature.h>
#include <ModelAPI_Result.h>


#ifdef WIN32
#pragma warning(disable : 4996) // for sprintf
#endif

namespace ModelGeomAlgo_Shape
{
  void shapesOfType(const FeaturePtr& theFeature,
                    const GeomAPI_Shape::ShapeType& theType,
                    std::set<ResultPtr>& theShapeResults)
  {
    theShapeResults.clear();
    std::list<ResultPtr> aResults = theFeature->results();
    std::list<ResultPtr>::const_iterator aRIter = aResults.cbegin();
    for (; aRIter != aResults.cend(); aRIter++) {
      ResultPtr aResult = *aRIter;
      GeomShapePtr aShape = aResult->shape();
      if (aShape.get() && aShape->shapeType() == theType)
        theShapeResults.insert(aResult);
    }
  }
} // namespace ModelGeomAlgo_Shape
