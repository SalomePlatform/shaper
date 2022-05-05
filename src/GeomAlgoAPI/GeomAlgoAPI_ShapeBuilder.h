// Copyright (C) 2014-2022  CEA/DEN, EDF R&D
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

#ifndef GeomAlgoAPI_ShapeBuilder_H_
#define GeomAlgoAPI_ShapeBuilder_H_

#include "GeomAlgoAPI.h"
#include "GeomAlgoAPI_MakeShapeList.h"

#include <GeomAPI_Shape.h>

/// \class GeomAlgoAPI_ShapeBuilder
/// \ingroup DataAlgo
/// \brief Allows to add or remove subshapes from a shape.
class GeomAlgoAPI_ShapeBuilder: public GeomAlgoAPI_MakeShapeList
{
 public:
   /// \brief Adds theShapeToAdd in theShape.
   /// \param[in] theShape shape to modify.
   /// \param[in] theShapeToAdd shape which will be added.
   GEOMALGOAPI_EXPORT static void add(std::shared_ptr<GeomAPI_Shape> theShape,
                                      const std::shared_ptr<GeomAPI_Shape> theShapeToAdd);

   /// \brief Removes theShapeToRemove in theShape.
   /// \param[in] theShape shape to modify.
   /// \param[in] theShapeToRemove shape which will be removed.
   GEOMALGOAPI_EXPORT static void remove(std::shared_ptr<GeomAPI_Shape> theShape,
                                         const std::shared_ptr<GeomAPI_Shape> theShapeToRemove);

   /// Creates empty builder.
   GEOMALGOAPI_EXPORT GeomAlgoAPI_ShapeBuilder();

   /// \brief Store new shape as result of removing internal shapes from theShape.
   /// \param[in] theShape base shape.
   GEOMALGOAPI_EXPORT void removeInternal(const std::shared_ptr<GeomAPI_Shape> theShape);

   /// \brief Store new shape as result of adding theShapesToAdd to theShape as internal shapes.
   /// \param[in] theShape base shape.
   /// \param[in] theShapesToAdd shapes which will be added.
   GEOMALGOAPI_EXPORT void addInternal(const std::shared_ptr<GeomAPI_Shape> theShape,
                                       const ListOfShape& theShapesToAdd);
};

#endif
