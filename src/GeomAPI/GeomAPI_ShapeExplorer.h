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

#ifndef GeomAPI_ShapeExplorer_H_
#define GeomAPI_ShapeExplorer_H_

#include <GeomAPI.h>
#include <GeomAPI_Shape.h>

/** \class GeomAPI_ShapeExplorer
 *  \ingroup DataModel
 *  \brief This class is used to explore subshapes on shape.
 */
class GeomAPI_ShapeExplorer : public GeomAPI_Interface
{
public:
  /// Default constructor. Creates an empty explorer, becomes usefull after Init.
  GEOMAPI_EXPORT
  GeomAPI_ShapeExplorer();

  /** \brief Constructs an explorer to search on theShape, for shapes of type toFind,
   *  that are not part of a shape toAvoid. If the shape toAvoid is equal to GeomAPI_SHape::SHAPE,
   *  or if it is the same as, or less complex than the shape toFind it has no effect on the search.
   *  \param[in] theShape shape to explore.
   *  \param[in] toFind shape type to find.
   *  \param[in] toAvoid shape type to avoid.
   */
  GEOMAPI_EXPORT
  GeomAPI_ShapeExplorer(const std::shared_ptr<GeomAPI_Shape>& theShape,
                        const GeomAPI_Shape::ShapeType toFind,
                        const GeomAPI_Shape::ShapeType toAvoid = GeomAPI_Shape::SHAPE);

  /** \brief Resets this explorer. It is initialized to search on theShape, for shapes of type toFind,
   *  that are not part of a shape toAvoid. If the shape toAvoid is equal to GeomAPI_SHape::SHAPE,
   *  or if it is the same as, or less complex than the shape toFind it has no effect on the search.
   *  \param[in] theShape shape to explore.
   *  \param[in] toFind shape type to find.
   *  \param[in] toAvoid shape type to avoid.
   */
  GEOMAPI_EXPORT
  void init(const std::shared_ptr<GeomAPI_Shape>& theShape,
            const GeomAPI_Shape::ShapeType toFind,
            const GeomAPI_Shape::ShapeType toAvoid = GeomAPI_Shape::SHAPE);

  /// \return true if there are more shapes in the exploration.
  GEOMAPI_EXPORT
  bool more() const;

  /// Moves to the next Shape in the exploration or do nothing
  /// if there are no more shapes to explore.
  GEOMAPI_EXPORT
  void next();

  /// \return the current shape in the exploration or empty pointer
  /// if this explorer has no more shapes to explore.
  GEOMAPI_EXPORT
  std::shared_ptr<GeomAPI_Shape> current();

  /// Reinitialize the exploration with the original arguments.
  GEOMAPI_EXPORT
  void reinit();

  /// \return the current depth of the exploration. 0 is the shape to explore itself.
  GEOMAPI_EXPORT
  int depth() const;

  /// Clears the content of the explorer. It will return False on more().
  GEOMAPI_EXPORT
  void clear();

};

#endif