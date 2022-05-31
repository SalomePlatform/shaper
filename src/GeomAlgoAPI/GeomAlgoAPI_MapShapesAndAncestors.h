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

#ifndef GeomAlgoAPI_MapShapesAndAncestors_H_
#define GeomAlgoAPI_MapShapesAndAncestors_H_

#include <GeomAlgoAPI.h>

#include <GeomAPI_Shape.h>

#include <map>
#include <set>

typedef std::set<GeomShapePtr, GeomAPI_Shape::ComparatorWithOri> SetOfShapes;
typedef std::map<GeomShapePtr, SetOfShapes, GeomAPI_Shape::Comparator> MapShapeToShapes;

/// \class GeomAlgoAPI_MapShapesAndAncestors
/// \ingroup DataAlgo
/// \brief Perform mapping specified types of sub-shapes of given shape
class GeomAlgoAPI_MapShapesAndAncestors
{
public:
  /// \brief Perform mapping.
  GEOMALGOAPI_EXPORT
  GeomAlgoAPI_MapShapesAndAncestors(const std::shared_ptr<GeomAPI_Shape> theShape,
                                    const GeomAPI_Shape::ShapeType theShapeType,
                                    const GeomAPI_Shape::ShapeType theAncestorType);
  /// \brief Perform mapping.
  GEOMALGOAPI_EXPORT
  void perform(const std::shared_ptr<GeomAPI_Shape> theShape,
               const GeomAPI_Shape::ShapeType theShapeType,
               const GeomAPI_Shape::ShapeType theAncestorType);

  /// \return Map of sub-shapes.
  const MapShapeToShapes& map() const { return myMap; }

private:
  MapShapeToShapes myMap;
};

#endif
