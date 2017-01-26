// Copyright (C) 2014-201x CEA/DEN, EDF R&D

// File:        GeomAlgoAPI_Scale.h
// Created:     23 Jan 2017
// Author:      Clarisse Genrault (CEA)

#ifndef GEOMALGOAPI_SCALE_H_
#define GEOMALGOAPI_SCALE_H_

#include <GeomAlgoAPI_MakeShape.h>

#include <GeomAPI_Pnt.h>

/// \class GeomAlgoAPI_Scale
/// \ingroup DataAlgo
/// \brief Creates a copy of the object by performing a scale operation by a factor or
///        by dimensions.
class GeomAlgoAPI_Scale : public GeomAlgoAPI_MakeShape
{
public:
  /// Type of scale operation
  enum MethodType {
    BY_FACTOR,     ///< Scale by factor.
    BY_DIMENSIONS, ///< Scale by dimensions.
  };

  /// \brief Creates an object which is obtained from current object by performing
  ///        a scale operation by a factor.
  /// \param[in] theSourceShape  the shape to be moved.
  /// \param[in] theCenterPoint  the center point.
  /// \param[in] theFactor       the scale factor.
  GEOMALGOAPI_EXPORT GeomAlgoAPI_Scale(std::shared_ptr<GeomAPI_Shape> theSourceShape,
                                       std::shared_ptr<GeomAPI_Pnt>   theCenterPoint,
                                       double                         theScaleFactor);

  /// \brief Creates an object which is obtained from current object by performing
  ///        a scale operation by dimensions.
  /// \param[in] theSourceShape  the shape to be moved.
  /// \param[in] theCenterPoint  the center point.
  /// \param[in] theFactorX      the scale factor in X.
  /// \param[in] theFactorY      the scale factor in Y.
  /// \param[in] theFactorZ      the scale factor in Z.
  GEOMALGOAPI_EXPORT GeomAlgoAPI_Scale(std::shared_ptr<GeomAPI_Shape> theSourceShape,
                                       std::shared_ptr<GeomAPI_Pnt>   theCenterPoint,
                                       double                         theScaleFactorX,
                                       double                         theScaleFactorY,
                                       double                         theScaleFactorZ);

  /// Checks if data for the scale transform is OK.
  GEOMALGOAPI_EXPORT bool check();

  /// Execute the scale transform.
  GEOMALGOAPI_EXPORT void build();

private:
  MethodType myMethodType; /// Type of method used.
  std::shared_ptr<GeomAPI_Shape> mySourceShape; /// Shape to be moved.
  std::shared_ptr<GeomAPI_Pnt> myCenterPoint; /// Center point.
  double myScaleFactor; /// Scale factor.
  double myScaleFactorX; /// Scale factor in X.
  double myScaleFactorY; /// Scale factor in Y.
  double myScaleFactorZ; /// Scale factor in Z.

  void buildByFactor();
  void buildByDimensions();
};

#endif // GEOMALGOAPI_SCALE_H_