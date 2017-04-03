// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        GeomAlgoAPI_MultiRotation.h
// Created:     30 Mar 2017
// Author:      Clarisse Genrault (CEA)

#ifndef GEOMALGOAPI_MULTIROTATION_H_
#define GEOMALGOAPI_MULTIROTATION_H_

#include <GeomAlgoAPI.h>
#include <GeomAlgoAPI_MakeShape.h>

#include <GeomAPI_Ax1.h>
#include <GeomAPI_Shape.h>

/// \class GeomAlgoAPI_MultiRotation
/// \ingroup DataAlgo
/// \brief Creates a copy of the object by rotating it around the axis.
class GeomAlgoAPI_MultiRotation : public GeomAlgoAPI_MakeShape
{
public:

  /// \brief Creates an object which is obtained from current object by rotating it around the axis
  ///        with the angle.
  /// \param[in] theSourceShape  a shape to be rotated.
  /// \param[in] theAxis         rotation axis.
  /// \param[in] theAngle        rotation angle(in degree).
  GEOMALGOAPI_EXPORT GeomAlgoAPI_MultiRotation(std::shared_ptr<GeomAPI_Shape> theSourceShape,
                                          std::shared_ptr<GeomAPI_Ax1>   theAxis,
                                          double                         theAngle);

  /// Checks if data for the translation execution is OK.
  GEOMALGOAPI_EXPORT bool check();

  /// Execute the translation.
  GEOMALGOAPI_EXPORT void build();

private:
  std::shared_ptr<GeomAPI_Shape> mySourceShape; /// Shape to be rotated.
  std::shared_ptr<GeomAPI_Ax1> myAxis; /// Rotation axis.
  double myAngle; /// Rotation angle.
  std::shared_ptr<GeomAPI_Pnt> myCenterPoint; /// Rotation center point.
  std::shared_ptr<GeomAPI_Pnt> myStartPoint; /// Rotation start point.
  std::shared_ptr<GeomAPI_Pnt> myEndPoint; /// Rotation end point.
};

#endif // GEOMALGOAPI_MULTIROTATION_H_
