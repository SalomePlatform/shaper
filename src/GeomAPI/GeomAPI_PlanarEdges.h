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

#ifndef GEOMAPI_WIRE_H_
#define GEOMAPI_WIRE_H_

#include "GeomAPI.h"
#include "GeomAPI_Edge.h"
#include "GeomAPI_Pnt.h"
#include "GeomAPI_Dir.h"
#include "GeomAPI_Ax3.h"

#include <memory>

#include <list>

/**\class GeomAPI_PlanarEdges
 * \ingroup DataModel
 * \brief Interface to the set of edges located in one plane
 *
 * Normally this interface corresponds to the edges of the sketch
 */
class GeomAPI_PlanarEdges : public GeomAPI_Shape
{
 public:
  /// Creation of empty (null) shape
  GEOMAPI_EXPORT GeomAPI_PlanarEdges();

  /// Returns whether the shape is alone vertex
  GEOMAPI_EXPORT virtual bool isVertex() const;

  /// Returns whether the shape is alone edge
  GEOMAPI_EXPORT virtual bool isEdge() const;
  /// Appends the edge to the set
  GEOMAPI_EXPORT void addEdge(std::shared_ptr<GeomAPI_Shape> theEdge);
  /// Returns the list of edges in this interface
  GEOMAPI_EXPORT std::list<std::shared_ptr<GeomAPI_Shape> > getEdges();

  /// Returns True if the wire is defined in a plane
  GEOMAPI_EXPORT bool hasPlane() const;

  /// Returns the plane origin point
  GEOMAPI_EXPORT std::shared_ptr<GeomAPI_Pnt> origin() const;

  /// Returns X direction vector
  GEOMAPI_EXPORT std::shared_ptr<GeomAPI_Dir> dirX() const;

  /// Returns Y direction vector
  GEOMAPI_EXPORT std::shared_ptr<GeomAPI_Dir> dirY() const;

  /// Returns Z direction vector
  GEOMAPI_EXPORT std::shared_ptr<GeomAPI_Dir> norm() const;

  /// Returns whether the shape is planar
  GEOMAPI_EXPORT virtual bool isPlanar() const;

  /// Set working plane
  /// \param theOrigin origin of the plane axis
  /// \param theDirX X direction of the plane axis
  /// \param theNorm normal direction of the plane axis
  GEOMAPI_EXPORT void setPlane(const std::shared_ptr<GeomAPI_Pnt>& theOrigin,
                               const std::shared_ptr<GeomAPI_Dir>& theDirX,
                               const std::shared_ptr<GeomAPI_Dir>& theNorm);

  /// Returns whether the shapes are equal
  GEOMAPI_EXPORT
  virtual bool isEqual(const std::shared_ptr<GeomAPI_Shape> theShape) const;

private:

  std::shared_ptr<GeomAPI_Ax3> myPlane;
};

#endif

