// File:        GeomAPI_Dir.hxx
// Created:     23 Apr 2014
// Author:      Mikhail PONIKAROV

#ifndef GeomAPI_Dir_HeaderFile
#define GeomAPI_Dir_HeaderFile

#include <GeomAPI_Interface.h>
#include <boost/shared_ptr.hpp>

class GeomAPI_XYZ;

/**\class GeomAPI_Dir
 * \ingroup DataModel
 * \brief 3D direction defined by three normalized coordinates
 */

class GEOMAPI_EXPORT GeomAPI_Dir: public GeomAPI_Interface
{
public:
  /// Creation of direction by coordinates
  GeomAPI_Dir(const double theX, const double theY, const double theZ);
  /// Creation of direction by coordinates
  GeomAPI_Dir(const boost::shared_ptr<GeomAPI_XYZ>& theCoords);

  /// returns X coordinate
  double x() const;
  /// returns Y coordinate
  double y() const;
  /// returns Z coordinate
  double z() const;

  /// returns coordinates of the direction
  const boost::shared_ptr<GeomAPI_XYZ> xyz();

  /// result is a scalar product of directions
  double dot(const boost::shared_ptr<GeomAPI_Dir>& theArg) const;
  /// result is a cross product of two directions
  const boost::shared_ptr<GeomAPI_XYZ> cross(const boost::shared_ptr<GeomAPI_Dir>& theArg) const;
};

#endif

