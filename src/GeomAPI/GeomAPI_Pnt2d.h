// File:        GeomAPI_Pnt2d.h
// Created:     29 May 2014
// Author:      Artem ZHIDKOV

#ifndef GeomAPI_Pnt2d_HeaderFile
#define GeomAPI_Pnt2d_HeaderFile

#include <GeomAPI_Interface.h>
#include <boost/shared_ptr.hpp>

class GeomAPI_XY;

/**\class GeomAPI_Pnt2d
 * \ingroup DataModel
 * \brief 2D point defined by two coordinates
 */

class GEOMAPI_EXPORT GeomAPI_Pnt2d: public GeomAPI_Interface
{
public:
  /// Creation of point by coordinates
  GeomAPI_Pnt2d(const double theX, const double theY);
  /// Creation of point by coordinates
  GeomAPI_Pnt2d(const boost::shared_ptr<GeomAPI_XY>& theCoords);

  /// returns X coordinate
  double x() const;
  /// returns Y coordinate
  double y() const;

  /// sets X coordinate
  void setX(const double theX);
  /// sets Y coordinate
  void setY(const double theY);

  /// returns coordinates of the point
  const boost::shared_ptr<GeomAPI_XY> xy();

  /// Distance between two points
  double distance(const boost::shared_ptr<GeomAPI_Pnt2d>& theOther) const;
};

#endif

