// File:        GeomAPI_Circ2d.h
// Created:     29 May 2014
// Author:      Artem ZHIDKOV

#ifndef GeomAPI_Circ2d_H_
#define GeomAPI_Circ2d_H_

#include <GeomAPI_Interface.h>
#include <boost/shared_ptr.hpp>

class GeomAPI_Pnt2d;
class GeomAPI_Dir2d;

/**\class GeomAPI_Circ2d
 * \ingroup DataModel
 * \brief Circle in 2D
 */

class GEOMAPI_EXPORT GeomAPI_Circ2d : public GeomAPI_Interface
{
 public:
  /// Creation of circle defined by center point and circle radius
  GeomAPI_Circ2d(const boost::shared_ptr<GeomAPI_Pnt2d>& theCenter,
                 const boost::shared_ptr<GeomAPI_Pnt2d>& theCirclePoint);

  /// Creation of circle defined by center point, direction and circle radius
  GeomAPI_Circ2d(const boost::shared_ptr<GeomAPI_Pnt2d>& theCenter,
                 const boost::shared_ptr<GeomAPI_Dir2d>& theDir, double theRadius);

  /// Return center of the circle
  const boost::shared_ptr<GeomAPI_Pnt2d> center() const;

  /// Return radius of the circle
  double radius() const;

  /// Project point on line
  const boost::shared_ptr<GeomAPI_Pnt2d> project(
      const boost::shared_ptr<GeomAPI_Pnt2d>& thePoint) const;
};

#endif

