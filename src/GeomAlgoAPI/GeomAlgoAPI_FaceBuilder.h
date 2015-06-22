// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        GeomAlgoAPI_FaceBuilder.h
// Created:     23 Apr 2014
// Author:      Mikhail PONIKAROV

#ifndef GeomAlgoAPI_FaceBuilder_H_
#define GeomAlgoAPI_FaceBuilder_H_

#include <GeomAlgoAPI.h>
#include <GeomAPI_Shape.h>
#include <GeomAPI_Pnt.h>
#include <GeomAPI_Pln.h>
#include <GeomAPI_Dir.h>
#include <memory>

/**\class GeomAlgoAPI_FaceBuilder
 * \ingroup DataAlgo
 * \brief Allows to create face-shapes by different parameters
 */

class GEOMALGOAPI_EXPORT GeomAlgoAPI_FaceBuilder
{
 public:
  /// Creates square planar face by given point of the center,
  /// normal to the plane and size of square
  static std::shared_ptr<GeomAPI_Shape> square(std::shared_ptr<GeomAPI_Pnt> theCenter,
                                                 std::shared_ptr<GeomAPI_Dir> theNormal,
                                                 const double theSize);
  /// Creates square planar face by given point of the center,
  /// normal to the plane and size of square
  static std::shared_ptr<GeomAPI_Shape> square(std::shared_ptr<GeomAPI_Pln> thePlane,
                                               const double theSize,
                                               const bool theInfinite = false);

  /// Returns the plane of the planar face. If it is not planar, returns empty ptr.
  static std::shared_ptr<GeomAPI_Pln> plane(std::shared_ptr<GeomAPI_Shape> theFace);

  /// Creates the planar face by given point of the center and normal to the plane.
  static std::shared_ptr<GeomAPI_Shape> planarFace(std::shared_ptr<GeomAPI_Pnt> theCenter,
                                                   std::shared_ptr<GeomAPI_Dir> theNormal);

  /// Creates a planar face by given plane, left lower point and size.
  static std::shared_ptr<GeomAPI_Shape> planarFace(std::shared_ptr<GeomAPI_Pln> thePlane,
                                                   double theX, double theY,
                                                   double theWidth, double theHeight);
};

#endif
