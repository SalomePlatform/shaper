// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        GeomAPI_Dir2d.hxx
// Created:     23 Apr 2014
// Author:      Mikhail PONIKAROV

#ifndef GeomAPI_Dir2d_H_
#define GeomAPI_Dir2d_H_

#include <GeomAPI_Interface.h>
#include <memory>

class GeomAPI_XY;

/**\class GeomAPI_Dir2d
 * \ingroup DataModel
 * \brief 2D direction defined by three normalized coordinates
 */

class GeomAPI_Dir2d : public GeomAPI_Interface
{
 public:
  /// Creation of direction by coordinates
  GEOMAPI_EXPORT 
  GeomAPI_Dir2d(const double theX, const double theY);
  /// Creation of direction by coordinates
  GEOMAPI_EXPORT 
  GeomAPI_Dir2d(const std::shared_ptr<GeomAPI_XY>& theCoords);

  /// returns X coordinate
  GEOMAPI_EXPORT 
  double x() const;
  /// returns Y coordinate
  GEOMAPI_EXPORT 
  double y() const;

  /// returns coordinates of the direction
  GEOMAPI_EXPORT 
  const std::shared_ptr<GeomAPI_XY> xy();

  /// inverses the direction
  GEOMAPI_EXPORT void reverse();

  /// result is a scalar product of directions
  GEOMAPI_EXPORT 
  double dot(const std::shared_ptr<GeomAPI_Dir2d>& theArg) const;
  /// result is a cross product of two directions
  GEOMAPI_EXPORT 
  double cross(const std::shared_ptr<GeomAPI_Dir2d>& theArg) const;

  /// calculates angle between two directions
  GEOMAPI_EXPORT 
  double angle(const std::shared_ptr<GeomAPI_Dir2d>& theArg) const;
};

#endif

