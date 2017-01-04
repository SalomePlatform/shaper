// Copyright (C) 2014-2016 CEA/DEN, EDF R&D

// File:        GeomAlgoAPI_Symmetry.h
// Created:     30 Nov 2016
// Author:      Clarisse Genrault (CEA)

#ifndef GEOMALGOAPI_SYMMETRY_H_
#define GEOMALGOAPI_SYMMETRY_H_

#include <GeomAlgoAPI.h>
#include <GeomAlgoAPI_MakeShape.h>

#include <GeomAPI_Pnt.h>
#include <GeomAPI_Ax1.h>
#include <GeomAPI_Ax2.h>

/// \class GeomAlgoAPI_Symmetry
/// \ingroup DataAlgo
/// \brief Creates a copy of the object by performing a symmetry operation by a point,
///        by an axis or by a plane.
class GeomAlgoAPI_Symmetry : public GeomAlgoAPI_MakeShape
{
public:
  /// Type of mirror operation
  enum MethodType {
    BY_POINT, ///< Mirror by point.
    BY_AXIS,  ///< Mirror by axis.
    BY_PLANE  ///< Mirror by plane.
  };

  /// \brief Creates an object which is obtained from current object by performing
  ///        a symmetry operation by a point.
  /// \param[in] theSourceShape  a shape to be moved.
  /// \param[in] thePoint         symmetry point.
  GEOMALGOAPI_EXPORT GeomAlgoAPI_Symmetry(std::shared_ptr<GeomAPI_Shape> theSourceShape,
                                          std::shared_ptr<GeomAPI_Pnt>   thePoint);

  /// \brief Creates an object which is obtained from current object by performing
  ///        a symmetry operation by a point.
  /// \param[in] theSourceShape  a shape to be moved.
  /// \param[in] theAxis         symmetry axis.
  GEOMALGOAPI_EXPORT GeomAlgoAPI_Symmetry(std::shared_ptr<GeomAPI_Shape> theSourceShape,
                                          std::shared_ptr<GeomAPI_Ax1>   theAxis);

  /// \brief Creates an object which is obtained from current object by performing
  ///        a symmetry operation by a point.
  /// \param[in] theSourceShape  a shape to be moved.
  /// \param[in] thePlane        symmetry plane.
  GEOMALGOAPI_EXPORT GeomAlgoAPI_Symmetry(std::shared_ptr<GeomAPI_Shape> theSourceShape,
                                          std::shared_ptr<GeomAPI_Ax2>   thePlane);

  /// Checks if data for the translation execution is OK.
  GEOMALGOAPI_EXPORT bool check();

  /// Execute the translation.
  GEOMALGOAPI_EXPORT void build();

private:
  MethodType myMethodType; /// Type of method used.
  std::shared_ptr<GeomAPI_Shape> mySourceShape; /// Shape to be moved.
  std::shared_ptr<GeomAPI_Pnt> myPoint; /// Reflection point.
  std::shared_ptr<GeomAPI_Ax1> myAxis; /// Reflection axis.
  std::shared_ptr<GeomAPI_Ax2> myPlane; /// Reflection plane.
};

#endif // GEOMALGOAPI_SYMMETRY_H_
