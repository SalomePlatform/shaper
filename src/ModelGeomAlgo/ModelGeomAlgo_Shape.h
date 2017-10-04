// Copyright (C) 2014-2017  CEA/DEN, EDF R&D
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
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
//
// See http://www.salome-platform.org/ or
// email : webmaster.salome@opencascade.com<mailto:webmaster.salome@opencascade.com>
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
}

#endif