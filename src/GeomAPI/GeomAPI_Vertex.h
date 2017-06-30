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

#ifndef GeomAPI_Vertex_H_
#define GeomAPI_Vertex_H_

#include <GeomAPI_Shape.h>

class GeomAPI_Pnt;

/// \class GeomAPI_Vertex
/// \ingroup DataModel
/// \brief Interface to the vertex object.
class GeomAPI_Vertex: public GeomAPI_Shape
{
public:
  /// Creation of empty (null) shape.
  GEOMAPI_EXPORT
  GeomAPI_Vertex();

   /// Creation of vertex by the vertex-shape.
  GEOMAPI_EXPORT
  GeomAPI_Vertex(const std::shared_ptr<GeomAPI_Shape>& theShape);

   /// Creation of vertex by 3d coordinates.
  GEOMAPI_EXPORT
  GeomAPI_Vertex(double theX, double theY, double theZ);

  /// Returns the first vertex coordinates of the edge.
  GEOMAPI_EXPORT
  std::shared_ptr<GeomAPI_Pnt> point();

  /// Returns true if the current edge is geometrically equal to the given edge.
  GEOMAPI_EXPORT
  bool isEqual(const std::shared_ptr<GeomAPI_Shape> theVert) const;
};

//! Pointer on the object
typedef std::shared_ptr<GeomAPI_Vertex> GeomVertexPtr;

#endif

