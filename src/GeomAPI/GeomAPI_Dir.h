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

class GEOMAPI_EXPORT GeomAPI_Dir : public GeomAPI_Interface
{
 public:
  /// Creation of direction by coordinates
  GeomAPI_Dir(const double theX, const double theY, const double theZ);
  /// Creation of direction by coordinates
  GeomAPI_Dir(const std::shared_ptr<GeomAPI_XYZ>& theCoords);

  /// returns X coordinate
  double x() const;
  /// returns Y coordinate
  double y() const;
  /// returns Z coordinate
  double z() const;

  /// returns coordinates of the direction
  const std::shared_ptr<GeomAPI_XYZ> xyz();

  /// result is a scalar product of directions
  double dot(const std::shared_ptr<GeomAPI_Dir>& theArg) const;
  /// result is a cross product of two directions
  const std::shared_ptr<GeomAPI_XYZ> cross(const std::shared_ptr<GeomAPI_Dir>& theArg) const;

  /// calculates angle between two directions
  double angle(const std::shared_ptr<GeomAPI_Dir>& theArg) const;
};

#endif

