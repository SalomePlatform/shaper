// Copyright (C) 2014-2020  CEA/DEN, EDF R&D
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

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

  struct SubshapeOfResult
  {
    std::shared_ptr<ModelAPI_Result> myResult;
    std::shared_ptr<GeomAPI_Shape> mySubshape;
    int myCenterType;
  };

  /// Searches a sub-shape in the results of the given features,
  /// which contains the given point.
  /// \param[in]  theFeature    feature, which results are being processed
  /// \param[in]  thePoint      selected point which identifies the shape
  /// \param[in]  theShapeType  type of the selected shape
  /// \param[out] theSelected   applicable result, sub-shape of the found result and
  ///                           type of the point if it is a center of circle or a focus of ellipse
  /// \return \c true if the result and its applicable sub-shape are found
  MODELGEOMALGO_EXPORT bool findSubshapeByPoint(
                              const std::shared_ptr<ModelAPI_Feature>& theFeature,
                              const std::shared_ptr<GeomAPI_Pnt>& thePoint,
                              const GeomAPI_Shape::ShapeType& theShapeType,
                              std::list<SubshapeOfResult>& theSelected);
}

#endif
