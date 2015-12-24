// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        GeomAlgoAPI_Rotation.h
// Created:     12 May 2015
// Author:      Dmitry Bobylev

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
  /// \brief Creates an object which is obtained from current object by rotating it around the axis.
  /// \param[in] theSourceShape  a shape to be rotated.
  /// \param[in] theAxis         rotation axis.
  /// \param[in] theAngle        rotation angle(in degree).
  GEOMALGOAPI_EXPORT GeomAlgoAPI_Rotation(std::shared_ptr<GeomAPI_Shape> theSourceShape,
                                          std::shared_ptr<GeomAPI_Ax1>   theAxis,
                                          double                         theAngle);

private:
  /// Builds resulting shape.
  void build(std::shared_ptr<GeomAPI_Shape> theSourceShape,
             std::shared_ptr<GeomAPI_Ax1>   theAxis,
             double                         theAngle);
};

#endif
