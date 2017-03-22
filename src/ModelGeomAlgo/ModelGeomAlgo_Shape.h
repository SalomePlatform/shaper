// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        ModelGeomAlgo_Point2D.h
// Created:     20 Jul 2016
// Author:      Natalia ERMOLAEVA

#ifndef ModelGeomAlgo_Shape_H
#define ModelGeomAlgo_Shape_H

#include "ModelGeomAlgo.h"
#include "GeomAPI_Shape.h"

#include <set>

class ModelAPI_Feature;
class ModelAPI_Result;

namespace ModelGeomAlgo_Shape {

  /// Searches Shape results of the feature satisfied the given shape type
  /// \param theFeature a feature to obtain AttributeRefAttr
  /// \param theType shape type
  /// \param theShapeResults a list of results
  MODELGEOMALGO_EXPORT void shapesOfType(
                              const std::shared_ptr<ModelAPI_Feature>& theFeature,
                              const GeomAPI_Shape::ShapeType& theType,
                              std::set<std::shared_ptr<ModelAPI_Result> >& theShapeResults);
}

#endif
