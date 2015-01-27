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

  /// Sets origin point
  GEOMAPI_EXPORT void setOrigin(const std::shared_ptr<GeomAPI_Pnt>& theOrigin);

  /// Returns the plane origin point
  GEOMAPI_EXPORT std::shared_ptr<GeomAPI_Pnt> origin() const;

  /// Sets X direction vector
  GEOMAPI_EXPORT void setDirX(const std::shared_ptr<GeomAPI_Dir>& theDirX);
  /// Returns X direction vector
  GEOMAPI_EXPORT std::shared_ptr<GeomAPI_Dir> dirX() const;

  /// Sets Y direction vector
  GEOMAPI_EXPORT void setDirY(const std::shared_ptr<GeomAPI_Dir>& theDirY);
  /// Returns Y direction vector
  GEOMAPI_EXPORT std::shared_ptr<GeomAPI_Dir> dirY() const;

  /// Sets Z direction vector
  GEOMAPI_EXPORT void setNorm(const std::shared_ptr<GeomAPI_Dir>& theNorm);
  /// Returns Z direction vector
  GEOMAPI_EXPORT std::shared_ptr<GeomAPI_Dir> norm() const;

private:
  /// Origin point of the plane
  std::shared_ptr<GeomAPI_Pnt> myOrigin;
  /// The X direction inside of the plane
  std::shared_ptr<GeomAPI_Dir> myDirX;
  /// The Y direction inside of the plane
  std::shared_ptr<GeomAPI_Dir> myDirY;
  /// The normal direction to the plane
  std::shared_ptr<GeomAPI_Dir> myNorm;
};

#endif

