// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        GeomAlgoAPI_Cone.h
// Created:     20 Mar 2017
// Author:      Clarisse Genrault (CEA)

#ifndef GEOMALGOAPI_CONE_H_
#define GEOMALGOAPI_CONE_H_

#include <GeomAlgoAPI_MakeShape.h>

#include <GeomAPI_Ax2.h>

/**\class GeomAlgoAPI_Cone
 * \ingroup DataAlgo
 * \brief Allows to create Cone Primitives
 */
class GeomAlgoAPI_Cone : public GeomAlgoAPI_MakeShape
{
 public:
  GEOMALGOAPI_EXPORT GeomAlgoAPI_Cone();

  /// Creates a cone.
  /// \param theAxis The axis of the cone
  /// \param theBaseRadius The base radius of the cone
  /// \param theTopRadius The top radius of the cone
  /// \param theHeight The height of the cone
  GEOMALGOAPI_EXPORT GeomAlgoAPI_Cone(std::shared_ptr<GeomAPI_Ax2> theAxis,
                                      const double theBaseRadius,
                                      const double theTopRadius,
                                      const double theHeight);

  /// Checks if data for the cone construction is OK.
  GEOMALGOAPI_EXPORT bool check();

  /// Builds the cone.
  GEOMALGOAPI_EXPORT void build();

 private:
  std::shared_ptr<GeomAPI_Ax2> myAxis; /// Axis of the cone.
  double myBaseRadius; /// Base radius of the cone.
  double myTopRadius; /// Top radius of the cone.
  double myHeight; /// Height of the cone.
};

#endif // GEOMALGOAPI_CONE_H_
