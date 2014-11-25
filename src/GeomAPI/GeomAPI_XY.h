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

class GEOMAPI_EXPORT GeomAPI_XY : public GeomAPI_Interface
{
 public:
  /// Creation by coordinates
  GeomAPI_XY(const double theX, const double theY);

  /// returns X coordinate
  double x() const;
  /// returns Y coordinate
  double y() const;

  /// sets X coordinate
  void setX(const double theX);
  /// sets Y coordinate
  void setY(const double theY);

  /// result is sum of coordinates of this and the given argument
  const std::shared_ptr<GeomAPI_XY> added(const std::shared_ptr<GeomAPI_XY>& theArg);
  /// result is coordinates multiplied by the argument
  const std::shared_ptr<GeomAPI_XY> multiplied(const double theArg);

  /// result is a scalar product of two triplets
  double dot(const std::shared_ptr<GeomAPI_XY>& theArg) const;
  /// result is a cross product of two triplets
  double cross(const std::shared_ptr<GeomAPI_XY>& theArg) const;

  /// Distance between two pairs
  double distance(const std::shared_ptr<GeomAPI_XY>& theOther) const;
};

#endif

