// Copyright (C) 2016-20xx CEA/DEN, EDF R&D

// File:        GeomAPI_Angle2d.h
// Created:     19 April 2016
// Author:      Artem ZHIDKOV

#ifndef GeomAPI_Angle_H_
#define GeomAPI_Angle_H_

#include <GeomAPI_Interface.h>

class GeomAPI_Lin2d;
class GeomAPI_Pnt2d;

/// \class GeomAPI_Angle2d
/// \ingroup DataModel
/// \brief Build an angle in plane
class GeomAPI_Angle2d : public GeomAPI_Interface
{
public:
  /// Creation of an angle defined by two lines' start, end points
  GEOMAPI_EXPORT
  GeomAPI_Angle2d(const std::shared_ptr<GeomAPI_Pnt2d>& theStartLine1,
                  const std::shared_ptr<GeomAPI_Pnt2d>& theEndLine1,
                  const std::shared_ptr<GeomAPI_Pnt2d>& theStartLine2,
                  const std::shared_ptr<GeomAPI_Pnt2d>& theEndLine2);
  /// Creation of an angle defined by two lines taking into account their orientation
  GEOMAPI_EXPORT
  GeomAPI_Angle2d(const std::shared_ptr<GeomAPI_Lin2d>& theLine1, bool theReversed1,
                  const std::shared_ptr<GeomAPI_Lin2d>& theLine2, bool theReversed2);
  /// Creation of an angle defined by three points
  GEOMAPI_EXPORT
  GeomAPI_Angle2d(const std::shared_ptr<GeomAPI_Pnt2d>& theCenter,
                  const std::shared_ptr<GeomAPI_Pnt2d>& thePoint1,
                  const std::shared_ptr<GeomAPI_Pnt2d>& thePoint2);

  /// Returns central point of the angle
  GEOMAPI_EXPORT std::shared_ptr<GeomAPI_Pnt2d> center();
  /// Returns point on the first edge
  GEOMAPI_EXPORT std::shared_ptr<GeomAPI_Pnt2d> firstPoint();
  /// Returns point on the second edge
  GEOMAPI_EXPORT std::shared_ptr<GeomAPI_Pnt2d> secondPoint();

  /// Returns value of the angle in degrees
  GEOMAPI_EXPORT double angleDegree();
  /// Returns value of the angle in radians
  GEOMAPI_EXPORT double angleRadian();

  /// Returns \c true if the line is reversed during angle calculation.
  /// If theIndex = 0, the result corresponds to the first line, if theIndex = 1, the to the second line
  GEOMAPI_EXPORT bool isReversed(int theIndex);
};

#endif

