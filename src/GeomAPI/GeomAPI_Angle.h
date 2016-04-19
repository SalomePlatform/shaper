// Copyright (C) 2016-20xx CEA/DEN, EDF R&D

// File:        GeomAPI_Angle.h
// Created:     19 April 2016
// Author:      Artem ZHIDKOV

#ifndef GeomAPI_Angle_H_
#define GeomAPI_Angle_H_

#include <GeomAPI_Interface.h>

class GeomAPI_Lin;
class GeomAPI_Pnt;

/// \class GeomAPI_Angle
/// \ingroup DataModel
/// \brief Build an angle in 3D
class GeomAPI_Angle : public GeomAPI_Interface
{
public:
  /// Creation of an angle defined by two lines' start, end points
  GEOMAPI_EXPORT
  GeomAPI_Angle(const std::shared_ptr<GeomAPI_Pnt>& theStartLine1,
                const std::shared_ptr<GeomAPI_Pnt>& theEndLine1,
                const std::shared_ptr<GeomAPI_Pnt>& theStartLine2,
                const std::shared_ptr<GeomAPI_Pnt>& theEndLine2);
  /// Creation of an angle defined by two lines taking into account their orientation
  GEOMAPI_EXPORT
  GeomAPI_Angle(const std::shared_ptr<GeomAPI_Lin>& theLine1, bool theReversed1,
                const std::shared_ptr<GeomAPI_Lin>& theLine2, bool theReversed2);
  /// Creation of an angle defined by three points
  GEOMAPI_EXPORT
  GeomAPI_Angle(const std::shared_ptr<GeomAPI_Pnt>& theCenter,
                const std::shared_ptr<GeomAPI_Pnt>& thePoint1,
                const std::shared_ptr<GeomAPI_Pnt>& thePoint2);

  /// Returns central point of the angle
  GEOMAPI_EXPORT std::shared_ptr<GeomAPI_Pnt> center();
  /// Returns point on the first edge
  GEOMAPI_EXPORT std::shared_ptr<GeomAPI_Pnt> firstPoint();
  /// Returns point on the second edge
  GEOMAPI_EXPORT std::shared_ptr<GeomAPI_Pnt> secondPoint();

  /// Returns value of the angle in degrees
  GEOMAPI_EXPORT double angleDegree();
  /// Returns value of the angle in radians
  GEOMAPI_EXPORT double angleRadian();

  /// Returns \c true if the line is reversed during angle calculation.
  /// If theIndex = 0, the result corresponds to the first line, if theIndex = 1, the to the second line
  GEOMAPI_EXPORT bool isReversed(int theIndex);
};

#endif

