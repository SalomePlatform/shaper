// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        GeomAlgoAPI_Torus.h
// Created:     20 Mar 2017
// Author:      Clarisse Genrault (CEA)

#ifndef GEOMALGOAPI_TORUS_H_
#define GEOMALGOAPI_TORUS_H_

#include <GeomAlgoAPI_MakeShape.h>

#include <GeomAPI_Ax2.h>

/**\class GeomAlgoAPI_Torus
 * \ingroup DataAlgo
 * \brief Allows to create Torus Primitives
 */
class GeomAlgoAPI_Torus : public GeomAlgoAPI_MakeShape
{
 public:
  /// Creates a torus.
  /// \param theAxis The axis of the torus
  /// \param theRadius The radius of the torus
  /// \param theRingRadius The ring radius of the torus
  GEOMALGOAPI_EXPORT GeomAlgoAPI_Torus(std::shared_ptr<GeomAPI_Ax2> theAxis,
                                       const double theRadius,
                                       const double theRingRadius);

  /// Checks if data for the torus construction is OK.
  GEOMALGOAPI_EXPORT bool check();

  /// Builds the torus.
  GEOMALGOAPI_EXPORT void build();

 private:
  std::shared_ptr<GeomAPI_Ax2> myAxis; /// Axis of the torus.
  double myRadius; /// Radius of the torus.
  double myRingRadius; /// Ring radius of the torus.
};

#endif // GEOMALGOAPI_TORUS_H_