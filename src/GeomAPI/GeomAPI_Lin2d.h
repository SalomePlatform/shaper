// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        GeomAPI_Lin2d.h
// Created:     29 May 2014
// Author:      Artem ZHIDKOV

#ifndef GeomAPI_Lin2d_H_
#define GeomAPI_Lin2d_H_

#include <GeomAPI_Interface.h>
#include <memory>

class GeomAPI_Dir2d;
class GeomAPI_Pnt2d;

/**\class GeomAPI_Lin2d
 * \ingroup DataModel
 * \brief Line in 2D
 */

class GEOMAPI_EXPORT GeomAPI_Lin2d : public GeomAPI_Interface
{
 public:
  /// Creation of line defined by cordinates of start and end points
  GeomAPI_Lin2d(const double theStartX, const double theStartY, const double theEndX,
                const double theEndY);
  /// Creation of line defined by start and end points
  GeomAPI_Lin2d(const std::shared_ptr<GeomAPI_Pnt2d>& theStart,
                const std::shared_ptr<GeomAPI_Pnt2d>& theEnd);

  /// Returns point on the line (first point)
  std::shared_ptr<GeomAPI_Pnt2d> location();

  /// Returns a line direction
  std::shared_ptr<GeomAPI_Dir2d> direction();

  /// Distance between two points
  double distance(const std::shared_ptr<GeomAPI_Pnt2d>& theOther) const;
  /// Intersection of two lines
  const std::shared_ptr<GeomAPI_Pnt2d> intersect(
      const std::shared_ptr<GeomAPI_Lin2d>& theLine) const;
  /// Project point on line
  const std::shared_ptr<GeomAPI_Pnt2d> project(
      const std::shared_ptr<GeomAPI_Pnt2d>& thePoint) const;
  /// Computes the cross product of the line direction and a vector from the line start point to the point
  bool isRight(const std::shared_ptr<GeomAPI_Pnt2d>& thePoint) const;

  /// Returns a location point shifted on theShift in perpendicular direction
  std::shared_ptr<GeomAPI_Pnt2d> shiftedLocation(double theShift) const;
};

#endif

