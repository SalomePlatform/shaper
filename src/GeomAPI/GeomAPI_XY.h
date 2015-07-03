// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        GeomAPI_XY.hxx
// Created:     30 May 2014
// Author:      Artem ZHIDKOV

#ifndef GeomAPI_XY_H_
#define GeomAPI_XY_H_

#include <GeomAPI_Interface.h>
#include <memory>

/**\class GeomAPI_XY
 * \ingroup DataModel
 * \brief 2 coordinates: they may represent vector or point or something else
 */

class GeomAPI_XY : public GeomAPI_Interface
{
 public:
  /// Creation by coordinates
  GEOMAPI_EXPORT 
  GeomAPI_XY(const double theX, const double theY);

  /// returns X coordinate
  GEOMAPI_EXPORT 
  double x() const;
  /// returns Y coordinate
  GEOMAPI_EXPORT 
  double y() const;

  /// sets X coordinate
  GEOMAPI_EXPORT 
  void setX(const double theX);
  /// sets Y coordinate
  GEOMAPI_EXPORT 
  void setY(const double theY);

  /// result is sum of coordinates of this and the given argument
  GEOMAPI_EXPORT 
  const std::shared_ptr<GeomAPI_XY> added(const std::shared_ptr<GeomAPI_XY>& theArg);
  /// result is difference between coordinates of this and the given argument
  GEOMAPI_EXPORT 
  const std::shared_ptr<GeomAPI_XY> decreased(const std::shared_ptr<GeomAPI_XY>& theArg);
  /// result is coordinates multiplied by the argument
  GEOMAPI_EXPORT 
  const std::shared_ptr<GeomAPI_XY> multiplied(const double theArg);

  /// result is a scalar product of two triplets
  GEOMAPI_EXPORT 
  double dot(const std::shared_ptr<GeomAPI_XY>& theArg) const;
  /// result is a cross product of two triplets
  GEOMAPI_EXPORT 
  double cross(const std::shared_ptr<GeomAPI_XY>& theArg) const;

  /// Distance between two pairs
  GEOMAPI_EXPORT 
  double distance(const std::shared_ptr<GeomAPI_XY>& theOther) const;
};

#endif

