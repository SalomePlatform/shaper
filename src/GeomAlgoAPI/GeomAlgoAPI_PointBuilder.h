// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        GeomAlgoAPI_PointBuilder.h
// Created:     02 Jun 2014
// Author:      Mikhail PONIKAROV

#ifndef GeomAlgoAPI_PointBuilder_H_
#define GeomAlgoAPI_PointBuilder_H_

#include <GeomAlgoAPI.h>
#include <memory>

class GeomAPI_Edge;
class GeomAPI_Face;
class GeomAPI_Pnt;
class GeomAPI_Shape;
class GeomAPI_Vertex;

/// \class GeomAlgoAPI_PointBuilder
/// \ingroup DataAlgo
/// \brief Allows to create vertex-shapes by different parameters
class GEOMALGOAPI_EXPORT GeomAlgoAPI_PointBuilder
{
public:
  /// Creates a vertex by point
  static std::shared_ptr<GeomAPI_Vertex> vertex(const std::shared_ptr<GeomAPI_Pnt> thePoint);

  /// Creates a vertex by point coordinates
  static std::shared_ptr<GeomAPI_Vertex> vertex(const double theX,
                                                const double theY,
                                                const double theZ);

  /// \brief Creates vertex by edge and distance on it.
  /// \param[in] theEdge edge.
  /// \param[in] theValue distance value.
  /// \param[in] theIsPercent if true theValue will be treated as a percentage of theEdge total length.
  /// \param[in] theIsReverse if true the distance will be measured from the edge end point.
  /// \return created vertex.
  static std::shared_ptr<GeomAPI_Vertex> vertexOnEdge(const std::shared_ptr<GeomAPI_Edge> theEdge,
                                                      const double theValue,
                                                      const bool theIsPercent = false,
                                                      const bool theIsReverse = false);

  /// \brief Creates vertex by projection another vertex on plane.
  /// \param[in] theVertex vertex to project.
  /// \param[in] thePlane face for projection. Should be planar.
  /// \return created vertex.
  static std::shared_ptr<GeomAPI_Vertex> vertexByProjection(const std::shared_ptr<GeomAPI_Vertex> theVertex,
                                                            const std::shared_ptr<GeomAPI_Face> thePlane);

  /// \brief Creates vertex by intersection two coplanar lines.
  /// \param[in] theEdge1 first linear edge.
  /// \param[in] theEdge2 second linear edge.
  /// \return created vertex.
  static std::shared_ptr<GeomAPI_Vertex> vertexByIntersection(const std::shared_ptr<GeomAPI_Edge> theEdge1,
                                                              const std::shared_ptr<GeomAPI_Edge> theEdge2);

  /// Return point by shape vertex
  static std::shared_ptr<GeomAPI_Pnt> point(const std::shared_ptr<GeomAPI_Shape> theVertex);
};

#endif
