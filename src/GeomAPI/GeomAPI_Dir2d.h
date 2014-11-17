// File:        GeomAPI_Dir2d.hxx
// Created:     23 Apr 2014
// Author:      Mikhail PONIKAROV

#ifndef GeomAPI_Dir2d_H_
#define GeomAPI_Dir2d_H_

#include <GeomAPI_Interface.h>
#include <memory>

class GeomAPI_XY;

/**\class GeomAPI_Dir2d
 * \ingroup DataModel
 * \brief 2D direction defined by three normalized coordinates
 */

class GEOMAPI_EXPORT GeomAPI_Dir2d : public GeomAPI_Interface
{
 public:
  /// Creation of direction by coordinates
  GeomAPI_Dir2d(const double theX, const double theY);
  /// Creation of direction by coordinates
  GeomAPI_Dir2d(const std::shared_ptr<GeomAPI_XY>& theCoords);

  /// returns X coordinate
  double x() const;
  /// returns Y coordinate
  double y() const;

  /// returns coordinates of the direction
  const std::shared_ptr<GeomAPI_XY> xy();

  /// result is a scalar product of directions
  double dot(const std::shared_ptr<GeomAPI_Dir2d>& theArg) const;
  /// result is a cross product of two directions
  double cross(const std::shared_ptr<GeomAPI_Dir2d>& theArg) const;
};

#endif

