// Copyright (C) 2014-2021  CEA/DEN, EDF R&D
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

#ifndef GeomAlgoAPI_Pipe_H_
#define GeomAlgoAPI_Pipe_H_

#include "GeomAlgoAPI.h"

#include "GeomAlgoAPI_MakeSweep.h"

#include <GeomAPI_Shape.h>

/// \class GeomAlgoAPI_Pipe
/// \ingroup DataAlgo
/// \brief Allows to create extrusion of objects along a path.
/// It produces the following results from objects:\n
/// Vertex -> Edge\n
/// Edge -> Face\n
/// Wire -> Shell\n
/// Face -> Solid
class GeomAlgoAPI_Pipe : public GeomAlgoAPI_MakeSweep
{
  GeomAPI_DataMapOfShapeShape myMovedPath; ///< map from initial path edges to the moved
public:
  /// \brief Creates extrusion for the given shape along a path.
  /// \param[in] theBaseShape base shape(vertex, edge, wire of face).
  /// \param[in] thePathShape path shape(edge or wire).
  GEOMALGOAPI_EXPORT GeomAlgoAPI_Pipe(const GeomShapePtr theBaseShape,
                                      const GeomShapePtr thePathShape);

  /// \brief Creates extrusion for the given shape along a path.
  /// \param[in] theBaseShape base shape(vertex, edge, wire of face).
  /// \param[in] thePathShape path shape(edge or wire).
  /// \param[in] theBiNormal edge or wire to preserve the constant angle between the normal vector
  /// to the base object and the BiNormal vector.
  GEOMALGOAPI_EXPORT GeomAlgoAPI_Pipe(const GeomShapePtr theBaseShape,
                                      const GeomShapePtr thePathShape,
                                      const GeomShapePtr theBiNormal);

  /// \brief Creates extrusion for the given shape along a path.
  /// \param[in] theBaseShapes base shape(vertex, edge, wire of face).
  /// \param[in] theLocations vertexes on the path. Should be empty or same size as theBaseShapes.
  /// \param[in] thePathShape path shape(edge or wire).
  /// to the base object and the BiNormal vector.
  GEOMALGOAPI_EXPORT GeomAlgoAPI_Pipe(const ListOfShape& theBaseShapes,
                                      const ListOfShape& theLocations,
                                      const GeomShapePtr thePathShape);

  /// \return the list of shapes generated from theShape.
  /// \param[in] theShape base shape.
  /// \param[out] theHistory generated shapes.
  GEOMALGOAPI_EXPORT void generated(const GeomShapePtr theShape,
                                    ListOfShape& theHistory);

private:
  void build(const GeomShapePtr theBaseShape,
             const GeomShapePtr thePathShape);

  void build(const GeomShapePtr theBaseShape,
             const GeomShapePtr thePathShape,
             const GeomShapePtr theBiNormal);

  void build(const ListOfShape& theBaseShapes,
             const ListOfShape& theLocations,
             const GeomShapePtr thePathShape);
  /// keeps the information about the moved path shapes into myMovedPath field
  void addMovedPath(GeomShapePtr thePath, GeomShapePtr theMoved);
};

#endif
