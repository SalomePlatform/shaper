// File:        GeomAPI_Circ.h
// Created:     24 Jun 2014
// Author:      Artem ZHIDKOV

#ifndef GeomAPI_Circ_HeaderFile
#define GeomAPI_Circ_HeaderFile

#include <GeomAPI_Interface.h>
#include <boost/shared_ptr.hpp>

class GeomAPI_Pnt;
class GeomAPI_Dir;

/**\class GeomAPI_Circ
 * \ingroup DataModel
 * \brief Circle in 3D
 */

class GEOMAPI_EXPORT GeomAPI_Circ: public GeomAPI_Interface
{
public:
  /// Creation of circle defined by center point, direction and circle radius
  GeomAPI_Circ(const boost::shared_ptr<GeomAPI_Pnt>& theCenter,
               const boost::shared_ptr<GeomAPI_Dir>& theDir,
               double theRadius);

  /// Return center of the circle
  const boost::shared_ptr<GeomAPI_Pnt> center() const;

  /// Return radius of the circle
  double radius() const;

  /// Project point on circle
  const boost::shared_ptr<GeomAPI_Pnt> project(const boost::shared_ptr<GeomAPI_Pnt>& thePoint) const;
};

#endif

