// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        GeomAPI_PlanarEdges.hxx
// Created:     24 Jul 2014
// Author:      Artem ZHIDKOV

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
 * Normally this interface corresponds to theedges of the sketch
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

  /// Set working plane
  /// \param theOrigin origin of the plane axis
  /// \param theDirX X direction of the plane axis
  /// \param theNorm normal direction of the plane axis
  GEOMAPI_EXPORT void setPlane(const std::shared_ptr<GeomAPI_Pnt>& theOrigin,
                               const std::shared_ptr<GeomAPI_Dir>& theDirX,
                               const std::shared_ptr<GeomAPI_Dir>& theNorm);

private:

  std::shared_ptr<GeomAPI_Ax3> myPlane;
};

#endif

