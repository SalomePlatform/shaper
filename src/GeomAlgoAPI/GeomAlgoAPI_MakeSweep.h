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

#ifndef GeomAlgoAPI_MakeSweep_H_
#define GeomAlgoAPI_MakeSweep_H_

#include <GeomAlgoAPI.h>

#include <GeomAlgoAPI_MakeShapeList.h>

/**\class GeomAlgoAPI_MakeSweep
 * \ingroup DataAlgo
 * \brief The abstract class MakeSweep is the root class of swept primitives.
 * Sweeps are objects you obtain by sweeping a profile along a path.
 * The profile can be any topology and the path is usually a curve or
 * a wire. The profile generates objects according to the following rules:
 * - Vertices generate Edges.
 * - Edges generate Faces.
 * - Wires generate Shells.
 * - Faces generate Solids.
 * - Shells generate Composite Solids.
 * You are not allowed to sweep Solids and Composite Solids.
 */
class GeomAlgoAPI_MakeSweep : public GeomAlgoAPI_MakeShapeList
{
public:
  /// \returns the list of from shapes.
  GEOMALGOAPI_EXPORT const ListOfShape& fromShapes() const;

  /// \return the list of to shapes.
  GEOMALGOAPI_EXPORT const ListOfShape& toShapes() const;

  /// \brief Adds a shape to list of from shape.
  /// \param[in] theShape a shape to add.
  GEOMALGOAPI_EXPORT void addFromShape(const std::shared_ptr<GeomAPI_Shape> theShape);

  /// \brief Sets from shapes
  /// \param[in] theListOfShapes list of from shapes.
  GEOMALGOAPI_EXPORT void setFromShapes(const ListOfShape& theListOfShapes);

  /// \brief Adds a face to list of to shape.
  /// \param[in] theShape a face to add.
  GEOMALGOAPI_EXPORT void addToShape(const std::shared_ptr<GeomAPI_Shape> theShape);

  /// \brief Sets to shapes
  /// \param[in] theListOfShapes list of to shapes.
  GEOMALGOAPI_EXPORT void setToShapes(const ListOfShape& theListOfShapes);

protected:
  /// Empty constructor.
  GeomAlgoAPI_MakeSweep() : GeomAlgoAPI_MakeShapeList() {};

private:
  ListOfShape myFromShapes;
  ListOfShape myToShapes;
};

#endif
