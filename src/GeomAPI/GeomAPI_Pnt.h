// File:        GeomAPI_Pnt.hxx
// Created:     23 Apr 2014
// Author:      Mikhail PONIKAROV

#ifndef GeomAPI_Pnt_HeaderFile
#define GeomAPI_Pnt_HeaderFile

#include <GeomAPI_Interface.h>

/**\class GeomAPI_Pnt
 * \ingroup DataModel
 * \brief 3D point defined by three coordinates
 */

class GEOMAPI_EXPORT GeomAPI_Pnt: public GeomAPI_Interface
{
public:
  /// Creation of point by coordinates
  GeomAPI_Pnt(const double theX, const double theY, const double theZ);

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
};

#endif

