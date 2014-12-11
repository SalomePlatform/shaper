// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        GeomAPI_Lin.h
// Created:     29 May 2014
// Author:      Artem ZHIDKOV

#ifndef GeomAPI_Lin_H_
#define GeomAPI_Lin_H_

#include <GeomAPI_Interface.h>
#include <memory>

class GeomAPI_Pnt;

/**\class GeomAPI_Lin
 * \ingroup DataModel
 * \brief Line in 3D
 */

class GEOMAPI_EXPORT GeomAPI_Lin : public GeomAPI_Interface
{
 public:
  /// Creation of line defined by cordinates of start and end points
  GeomAPI_Lin(const double theStartX, const double theStartY, const double theStartZ,
              const double theEndX, const double theEndY, const double theEndZ);
  /// Creation of line defined by start and end points
  GeomAPI_Lin(const std::shared_ptr<GeomAPI_Pnt>& theStart,
              const std::shared_ptr<GeomAPI_Pnt>& theEnd);

  /// Distance between two points
  double distance(const std::shared_ptr<GeomAPI_Pnt>& thePoint) const;
  /// Intersection of two lines
  const std::shared_ptr<GeomAPI_Pnt> intersect(
      const std::shared_ptr<GeomAPI_Lin>& theLine) const;
  /// Project point on line
  const std::shared_ptr<GeomAPI_Pnt> project(
      const std::shared_ptr<GeomAPI_Pnt>& thePoint) const;
};

#endif

