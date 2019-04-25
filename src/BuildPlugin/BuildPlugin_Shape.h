// Copyright (C) 2014-2019  CEA/DEN, EDF R&D
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

#ifndef BuildPlugin_Shape_H_
#define BuildPlugin_Shape_H_

#include "BuildPlugin.h"

#include <GeomAPI_Shape.h>

#include <ModelAPI_Feature.h>

class GeomAlgoAPI_MakeShape;

/// \class BuildPlugin_Shape
/// \ingroup Plugins
/// \brief Base class containing common methods for shape creating operations.
class BuildPlugin_Shape: public ModelAPI_Feature
{
protected:
  /// Obtain list of selected shapes and their contexts
  void getOriginalShapesAndContexts(const std::string& theSelectionListID,
                                    ListOfShape& theShapes,
                                    ListOfShape& theContexts);

  /// Store result of algorithm
  void storeResult(const std::shared_ptr<GeomAlgoAPI_MakeShape>& theAlgorithm,
                   const ListOfShape& theOriginalShapes,
                   const ListOfShape& theOriginalContexts,
                   const GeomShapePtr& theResultShape,
                   const int theResultIndex = 0);
};

#endif
