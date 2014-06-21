// File:        GeomAPI_Circ2d.h
// Created:     29 May 2014
// Author:      Artem ZHIDKOV

#ifndef GeomAPI_Circ2d_HeaderFile
#define GeomAPI_Circ2d_HeaderFile

#include <GeomAPI_Interface.h>
#include <boost/shared_ptr.hpp>

class GeomAPI_Pnt2d;
class GeomAPI_Dir2d;

/**\class GeomAPI_Circ2d
 * \ingroup DataModel
 * \brief Line in 2D
 */

class GEOMAPI_EXPORT GeomAPI_Circ2d: public GeomAPI_Interface
{
public:
  /// Creation of circle defined by center point and circle radius
  GeomAPI_Circ2d(const boost::shared_ptr<GeomAPI_Pnt2d>& theCenter,
                 const boost::shared_ptr<GeomAPI_Pnt2d>& theCirclePoint);

  /// Creation of circle defined by center point, direction and circle radius
  GeomAPI_Circ2d(const boost::shared_ptr<GeomAPI_Pnt2d>& theCenter,
                 const boost::shared_ptr<GeomAPI_Dir2d>& theDir,
                 double theRadius);

  /// Project point on line
  const boost::shared_ptr<GeomAPI_Pnt2d> project(const boost::shared_ptr<GeomAPI_Pnt2d>& thePoint) const;
};

#endif

