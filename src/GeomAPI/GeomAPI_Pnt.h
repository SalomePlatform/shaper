// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        GeomAPI_Pnt.hxx
// Created:     23 Apr 2014
// Author:      Mikhail PONIKAROV

#ifndef GeomAPI_Pnt_H_
#define GeomAPI_Pnt_H_

#include <GeomAPI_Interface.h>
#include <memory>

class GeomAPI_XYZ;
class GeomAPI_Pnt2d;
class GeomAPI_Dir;
class GeomAPI_Pln;

/**\class GeomAPI_Pnt
 * \ingroup DataModel
 * \brief 3D point defined by three coordinates
 */

class GeomAPI_Pnt : public GeomAPI_Interface
{
 public:
  /// Creation of point by coordinates
  GEOMAPI_EXPORT 
  GeomAPI_Pnt(const double theX, const double theY, const double theZ);
  /// Creation of point by coordinates
  GEOMAPI_EXPORT 
  GeomAPI_Pnt(const std::shared_ptr<GeomAPI_XYZ>& theCoords);

  /// returns X coordinate
  GEOMAPI_EXPORT 
  double x() const;
  /// returns Y coordinate
  GEOMAPI_EXPORT 
  double y() const;
  /// returns Z coordinate
  GEOMAPI_EXPORT 
  double z() const;

  /// sets X coordinate
  GEOMAPI_EXPORT 
  void setX(const double theX);
  /// sets Y coordinate
  GEOMAPI_EXPORT 
  void setY(const double theY);
  /// sets Z coordinate
  GEOMAPI_EXPORT 
  void setZ(const double theZ);

  /// returns coordinates of the point
  GEOMAPI_EXPORT 
  const std::shared_ptr<GeomAPI_XYZ> xyz();

  /// Distance between two points
  GEOMAPI_EXPORT 
  double distance(const std::shared_ptr<GeomAPI_Pnt>& theOther) const;

  /// Projects a point to the plane defined by the origin and 2 axes vectors in this plane
  GEOMAPI_EXPORT 
  std::shared_ptr<GeomAPI_Pnt2d> to2D(const std::shared_ptr<GeomAPI_Pnt>& theOrigin,
                                        const std::shared_ptr<GeomAPI_Dir>& theDirX,
                                        const std::shared_ptr<GeomAPI_Dir>& theDirY);

  /// Projects a point to the plane defined by the origin and 2 axes vectors in this plane
  GEOMAPI_EXPORT 
  std::shared_ptr<GeomAPI_Pnt2d> to2D(const std::shared_ptr<GeomAPI_Pln>& thePln) const;

  /// Translates the point along direction theDir on distance theDist
  GEOMAPI_EXPORT 
  void translate(const std::shared_ptr<GeomAPI_Dir>& theDir, double theDist);
};

#endif
