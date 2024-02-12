// Copyright (C) 2014-2024  CEA, EDF
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

#ifndef GeomAPI_IndexedMapOfShape_H_
#define GeomAPI_IndexedMapOfShape_H_

#include <GeomAPI_Interface.h>
#include <GeomAPI_Shape.h>

/**\class GeomAPI_IndexedMapOfShape
 * \ingroup DataModel
 * \brief IndexedMap of Shape defined by TopoDS_Shape
 */
class GeomAPI_IndexedMapOfShape : public GeomAPI_Interface
{
 public:
  /// Constructor.
  GEOMAPI_EXPORT
  GeomAPI_IndexedMapOfShape() {};

  /// Constructor.
  GEOMAPI_EXPORT
  GeomAPI_IndexedMapOfShape(const std::shared_ptr<GeomAPI_Shape> theMainShape);

  /// Initialize the Map.
  GEOMAPI_EXPORT
  void MapShapes(const std::shared_ptr<GeomAPI_Shape> theMainShape);

  /// \return index of the Key in the Map.
  GEOMAPI_EXPORT
  int FindIndex(const std::shared_ptr<GeomAPI_Shape> theKey);

  /// \return index of the Key in the Map.
  /// It works also for the case of equal locations
  GEOMAPI_EXPORT
  int FindIndexEqualLocations(const std::shared_ptr<GeomAPI_Shape> theKey);

  /// Destructor
  GEOMAPI_EXPORT
  ~GeomAPI_IndexedMapOfShape();
};

#endif

