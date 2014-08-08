// File:        GeomAPI_Lin.h
// Created:     29 May 2014
// Author:      Artem ZHIDKOV

#ifndef GeomAPI_Lin_H_
#define GeomAPI_Lin_H_

#include <GeomAPI_Interface.h>
#include <boost/shared_ptr.hpp>

class GeomAPI_Pnt;

/**\class GeomAPI_Lin
 * \ingroup DataModel
 * \brief Line in 3D
 */

class GEOMAPI_EXPORT GeomAPI_Lin: public GeomAPI_Interface
{
public:
  /// Creation of line defined by cordinates of start and end points
  GeomAPI_Lin(const double theStartX, const double theStartY, const double theStartZ,
              const double theEndX,   const double theEndY,   const double theEndZ);
  /// Creation of line defined by start and end points
  GeomAPI_Lin(const boost::shared_ptr<GeomAPI_Pnt>& theStart,
              const boost::shared_ptr<GeomAPI_Pnt>& theEnd);

  /// Distance between two points
  double distance(const boost::shared_ptr<GeomAPI_Pnt>& thePoint) const;
  /// Intersection of two lines
  const boost::shared_ptr<GeomAPI_Pnt> intersect(const boost::shared_ptr<GeomAPI_Lin>& theLine) const;
  /// Project point on line
  const boost::shared_ptr<GeomAPI_Pnt> project(const boost::shared_ptr<GeomAPI_Pnt>& thePoint) const;
};

#endif

