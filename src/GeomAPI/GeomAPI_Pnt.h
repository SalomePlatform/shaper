// File:        GeomAPI_Pnt.hxx
// Created:     23 Apr 2014
// Author:      Mikhail PONIKAROV

#ifndef GeomAPI_Pnt_H_
#define GeomAPI_Pnt_H_

#include <GeomAPI_Interface.h>
#include <boost/shared_ptr.hpp>

class GeomAPI_XYZ;

/**\class GeomAPI_Pnt
 * \ingroup DataModel
 * \brief 3D point defined by three coordinates
 */

class GEOMAPI_EXPORT GeomAPI_Pnt : public GeomAPI_Interface
{
 public:
  /// Creation of point by coordinates
  GeomAPI_Pnt(const double theX, const double theY, const double theZ);
  /// Creation of point by coordinates
  GeomAPI_Pnt(const boost::shared_ptr<GeomAPI_XYZ>& theCoords);

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

  /// returns coordinates of the point
  const boost::shared_ptr<GeomAPI_XYZ> xyz();

  /// Distance between two points
  double distance(const boost::shared_ptr<GeomAPI_Pnt>& theOther) const;
};

#endif

