// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        GeomAPI_Dir.hxx
// Created:     23 Apr 2014
// Author:      Mikhail PONIKAROV

#ifndef GeomAPI_Dir_H_
#define GeomAPI_Dir_H_

#include <GeomAPI_Interface.h>
#include <memory>

class GeomAPI_XYZ;

/**\class GeomAPI_Dir
 * \ingroup DataModel
 * \brief 3D direction defined by three normalized coordinates
 */

class GeomAPI_Dir : public GeomAPI_Interface
{
 public:
  /// Creation of direction by coordinates
  GEOMAPI_EXPORT 
  GeomAPI_Dir(const double theX, const double theY, const double theZ);
  /// Creation of direction by coordinates
  GEOMAPI_EXPORT 
  GeomAPI_Dir(const std::shared_ptr<GeomAPI_XYZ>& theCoords);

  /// returns X coordinate
  GEOMAPI_EXPORT 
  double x() const;
  /// returns Y coordinate
  GEOMAPI_EXPORT 
  double y() const;
  /// returns Z coordinate
  GEOMAPI_EXPORT 
  double z() const;

  /// returns coordinates of the direction
  GEOMAPI_EXPORT 
  const std::shared_ptr<GeomAPI_XYZ> xyz();

  /// inverses the direction
  GEOMAPI_EXPORT void reverse();

  /// result is a scalar product of directions
  GEOMAPI_EXPORT 
  double dot(const std::shared_ptr<GeomAPI_Dir>& theArg) const;
  /// result is a cross product of two directions
  GEOMAPI_EXPORT 
  const std::shared_ptr<GeomAPI_XYZ> cross(const std::shared_ptr<GeomAPI_Dir>& theArg) const;

  /// calculates angle between two directions
  GEOMAPI_EXPORT 
  double angle(const std::shared_ptr<GeomAPI_Dir>& theArg) const;
};

#endif

