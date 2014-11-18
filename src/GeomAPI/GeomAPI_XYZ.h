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

class GEOMAPI_EXPORT GeomAPI_XYZ : public GeomAPI_Interface
{
 public:
  /// Creation by coordinates
  GeomAPI_XYZ(const double theX, const double theY, const double theZ);

  /// returns X coordinate
  double x() const;
  /// returns Y coordinate
  double y() const;
  /// returns Z coordinate
  double z() const;

  /// sets X coordinate
  void setX(const double theX);
  /// sets Y coordinate
  void setY(const double theY);
  /// sets Z coordinate
  void setZ(const double theZ);

  /// result is sum of coordinates of this and the given argument
  const std::shared_ptr<GeomAPI_XYZ> added(const std::shared_ptr<GeomAPI_XYZ>& theArg);
  /// result is difference between coordinates of this and the given argument
  const std::shared_ptr<GeomAPI_XYZ> decreased(const std::shared_ptr<GeomAPI_XYZ>& theArg);
  /// result is coordinates multiplied by the argument
  const std::shared_ptr<GeomAPI_XYZ> multiplied(const double theArg);

  /// result is a scalar product of two triplets
  double dot(const std::shared_ptr<GeomAPI_XYZ>& theArg) const;
  /// result is a cross product of two triplets
  const std::shared_ptr<GeomAPI_XYZ> cross(const std::shared_ptr<GeomAPI_XYZ>& theArg) const;

  /// Distance between two triplets
  double distance(const std::shared_ptr<GeomAPI_XYZ>& theOther) const;
};

#endif

