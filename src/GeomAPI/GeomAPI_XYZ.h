// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        GeomAPI_XYZ.hxx
// Created:     23 Apr 2014
// Author:      Mikhail PONIKAROV

#ifndef GeomAPI_XYZ_H_
#define GeomAPI_XYZ_H_

#include <GeomAPI_Interface.h>
#include <memory>

/**\class GeomAPI_XYZ
 * \ingroup DataModel
 * \brief 3 coordinates: they may represent vector or point or something else
 */

class GeomAPI_XYZ : public GeomAPI_Interface
{
 public:
  /// Creation by coordinates
  GEOMAPI_EXPORT 
  GeomAPI_XYZ(const double theX, const double theY, const double theZ);

  /// returns X coordinate
  GEOMAPI_EXPORT 
  double x() const;
  /// returns Y coordinate
  GEOMAPI_EXPORT 
  double y() const;
  /// returns Z coordinate
  GEOMAPI_EXPORT 
  double z() const;

  /// sets X coordinate
  GEOMAPI_EXPORT 
  void setX(const double theX);
  /// sets Y coordinate
  GEOMAPI_EXPORT 
  void setY(const double theY);
  /// sets Z coordinate
  GEOMAPI_EXPORT 
  void setZ(const double theZ);

  /// result is sum of coordinates of this and the given argument
  GEOMAPI_EXPORT 
  const std::shared_ptr<GeomAPI_XYZ> added(const std::shared_ptr<GeomAPI_XYZ>& theArg);
  /// result is difference between coordinates of this and the given argument
  GEOMAPI_EXPORT 
  const std::shared_ptr<GeomAPI_XYZ> decreased(const std::shared_ptr<GeomAPI_XYZ>& theArg);
  /// result is coordinates multiplied by the argument
  GEOMAPI_EXPORT 
  const std::shared_ptr<GeomAPI_XYZ> multiplied(const double theArg);

  /// result is a scalar product of two triplets
  GEOMAPI_EXPORT 
  double dot(const std::shared_ptr<GeomAPI_XYZ>& theArg) const;
  /// result is a cross product of two triplets
  GEOMAPI_EXPORT 
  const std::shared_ptr<GeomAPI_XYZ> cross(const std::shared_ptr<GeomAPI_XYZ>& theArg) const;

  /// Distance between two triplets
  GEOMAPI_EXPORT 
  double distance(const std::shared_ptr<GeomAPI_XYZ>& theOther) const;
};

#endif

