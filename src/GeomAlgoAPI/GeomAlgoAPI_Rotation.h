// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        GeomAlgoAPI_Rotation.h
// Created:     12 May 2015
// Author:      Dmitry Bobylev
//
// Modified by Clarisse Genrault (CEA) : 03 Mar 2017

#ifndef GeomAlgoAPI_Rotation_H_
#define GeomAlgoAPI_Rotation_H_

#include <GeomAlgoAPI.h>
#include <GeomAlgoAPI_MakeShape.h>

#include <GeomAPI_Ax1.h>
#include <GeomAPI_Shape.h>

/// \class GeomAlgoAPI_Rotation
/// \ingroup DataAlgo
/// \brief Creates a copy of the object by rotating it around the axis.
class GeomAlgoAPI_Rotation : public GeomAlgoAPI_MakeShape
{
public:
  /// Type of rotation operation
  enum MethodType {
    BY_ANGLE, ///< Rotation by axis and an angle
    BY_POINTS ///< Rotation by a center and two points
  };

  /// \brief Creates an object which is obtained from current object by rotating it around the axis
  ///        with the angle.
  /// \param[in] theSourceShape  a shape to be rotated.
  /// \param[in] theAxis         rotation axis.
  /// \param[in] theAngle        rotation angle(in degree).
  GEOMALGOAPI_EXPORT GeomAlgoAPI_Rotation(std::shared_ptr<GeomAPI_Shape> theSourceShape,
                                          std::shared_ptr<GeomAPI_Ax1>   theAxis,
                                          double                         theAngle);

  /// \brief Creates an object which is obtained from current object by rotating it around the axis
  ///        withe angle using the center and two points.
  /// \param[in] theSourceShape  a shape to be rotated.
  /// \param[in] theCenterPoint  center point.
  /// \param[in] theStartPoint   start point.
  /// \param[in] theEndPoint     end point.
  GEOMALGOAPI_EXPORT GeomAlgoAPI_Rotation(std::shared_ptr<GeomAPI_Shape> theSourceShape,
                                          std::shared_ptr<GeomAPI_Pnt>   theCenterPoint,
                                          std::shared_ptr<GeomAPI_Pnt>   theStartPoint,
                                          std::shared_ptr<GeomAPI_Pnt>   theEndPoint);

  /// Checks if data for the translation execution is OK.
  GEOMALGOAPI_EXPORT bool check();

  /// Execute the translation.
  GEOMALGOAPI_EXPORT void build();

private:
  MethodType myMethodType; /// Type of method used.
  std::shared_ptr<GeomAPI_Shape> mySourceShape; /// Shape to be rotated.
  std::shared_ptr<GeomAPI_Ax1> myAxis; /// Rotation axis.
  double myAngle; /// Rotation angle.
  std::shared_ptr<GeomAPI_Pnt> myCenterPoint; /// Rotation center point.
  std::shared_ptr<GeomAPI_Pnt> myStartPoint; /// Rotation start point.
  std::shared_ptr<GeomAPI_Pnt> myEndPoint; /// Rotation end point.
};

#endif
