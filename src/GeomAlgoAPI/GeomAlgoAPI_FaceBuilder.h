// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        GeomAlgoAPI_FaceBuilder.h
// Created:     23 Apr 2014
// Author:      Mikhail PONIKAROV

#ifndef GeomAlgoAPI_FaceBuilder_H_
#define GeomAlgoAPI_FaceBuilder_H_

#include "GeomAlgoAPI.h"

#include <memory>

class GeomAPI_Dir;
class GeomAPI_Face;
class GeomAPI_Pln;
class GeomAPI_Pnt;
class GeomAPI_Vertex;

/// \class GeomAlgoAPI_FaceBuilder
/// \ingroup DataAlgo
/// \brief Allows to create face-shapes by different parameters
class GEOMALGOAPI_EXPORT GeomAlgoAPI_FaceBuilder
{
 public:
  /// Creates square planar face by given point of the center,
  /// normal to the plane and size of square
  static std::shared_ptr<GeomAPI_Face> squareFace(const std::shared_ptr<GeomAPI_Pnt> theCenter,
                                                  const std::shared_ptr<GeomAPI_Dir> theNormal,
                                                  const double theSize);
  /// Creates square planar face by given point of the center,
  /// normal to the plane and size of square
  static std::shared_ptr<GeomAPI_Face> squareFace(const std::shared_ptr<GeomAPI_Pln> thePlane,
                                                  const double theSize);

  /// Creates the planar face by given point of the center and normal to the plane.
  static std::shared_ptr<GeomAPI_Face> planarFace(const std::shared_ptr<GeomAPI_Pnt> theCenter,
                                                  const std::shared_ptr<GeomAPI_Dir> theNormal);

  /// Creates a planar face by given plane, left lower point and size.
  static std::shared_ptr<GeomAPI_Face> planarFace(const std::shared_ptr<GeomAPI_Pln> thePlane,
                                                  const double theX, const double theY,
                                                  const double theWidth, const double theHeight);

  /// Creates a planar face by three vertices.
  static std::shared_ptr<GeomAPI_Face> planarFaceByThreeVertices(const std::shared_ptr<GeomAPI_Vertex> theVertex1,
                                                                 const std::shared_ptr<GeomAPI_Vertex> theVertex2,
                                                                 const std::shared_ptr<GeomAPI_Vertex> theVertex3);

  /// Creates a planar face parallel to theFace and passing through theVertex.
  static std::shared_ptr<GeomAPI_Face> planarFaceByFaceAndVertex(const std::shared_ptr<GeomAPI_Face> theFace,
                                                                 const std::shared_ptr<GeomAPI_Vertex> theVertex);
};

#endif
