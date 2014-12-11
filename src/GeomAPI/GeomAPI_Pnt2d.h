// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        GeomAPI_Pnt2d.h
// Created:     29 May 2014
// Author:      Artem ZHIDKOV

#ifndef GeomAPI_Pnt2d_H_
#define GeomAPI_Pnt2d_H_

#include <GeomAPI_Interface.h>
#include <memory>

class GeomAPI_XY;
class GeomAPI_Pnt;
class GeomAPI_Dir;

/**\class GeomAPI_Pnt2d
 * \ingroup DataModel
 * \brief 2D point defined by two coordinates
 */

class GEOMAPI_EXPORT GeomAPI_Pnt2d : public GeomAPI_Interface
{
 public:
  /// Creation of point by coordinates
  GeomAPI_Pnt2d(const double theX, const double theY);
  /// Creation of point by coordinates
  GeomAPI_Pnt2d(const std::shared_ptr<GeomAPI_XY>& theCoords);

  /// returns X coordinate
  double x() const;
  /// returns Y coordinate
  double y() const;

  /// sets X coordinate
  void setX(const double theX);
  /// sets Y coordinate
  void setY(const double theY);

  /// Returns the 3D point
  std::shared_ptr<GeomAPI_Pnt> to3D(const std::shared_ptr<GeomAPI_Pnt>& theOrigin,
                                      const std::shared_ptr<GeomAPI_Dir>& theDirX,
                                      const std::shared_ptr<GeomAPI_Dir>& theDirY);

  /// returns coordinates of the point
  const std::shared_ptr<GeomAPI_XY> xy();

  /// Distance between two points
  double distance(const std::shared_ptr<GeomAPI_Pnt2d>& theOther) const;

  /// Returns whether the distance between two points is less then precision confusion
  bool isEqual(const std::shared_ptr<GeomAPI_Pnt2d>& theOther) const;
};

#endif

