// File:        GeomAPI_Lin2d.h
// Created:     29 May 2014
// Author:      Artem ZHIDKOV

#ifndef GeomAPI_Lin2d_H_
#define GeomAPI_Lin2d_H_

#include <GeomAPI_Interface.h>
#include <boost/shared_ptr.hpp>

class GeomAPI_Pnt2d;

/**\class GeomAPI_Lin2d
 * \ingroup DataModel
 * \brief Line in 2D
 */

class GEOMAPI_EXPORT GeomAPI_Lin2d : public GeomAPI_Interface
{
 public:
  /// Creation of line defined by cordinates of start and end points
  GeomAPI_Lin2d(const double theStartX, const double theStartY, const double theEndX,
                const double theEndY);
  /// Creation of line defined by start and end points
  GeomAPI_Lin2d(const boost::shared_ptr<GeomAPI_Pnt2d>& theStart,
                const boost::shared_ptr<GeomAPI_Pnt2d>& theEnd);

  /// Distance between two points
  double distance(const boost::shared_ptr<GeomAPI_Pnt2d>& theOther) const;
  /// Intersection of two lines
  const boost::shared_ptr<GeomAPI_Pnt2d> intersect(
      const boost::shared_ptr<GeomAPI_Lin2d>& theLine) const;
  /// Project point on line
  const boost::shared_ptr<GeomAPI_Pnt2d> project(
      const boost::shared_ptr<GeomAPI_Pnt2d>& thePoint) const;
  /// Computes the cross product of the line direction and a vector from the line start point to the point
  bool isRight(const boost::shared_ptr<GeomAPI_Pnt2d>& thePoint) const;
};

#endif

