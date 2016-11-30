// Copyright (C) 2014-2016 CEA/DEN, EDF R&D

// File:        GeomAlgoAPI_ConeSegment.h
// Created:     24 Nov 2016
// Author:      Clarisse Genrault (CEA)

#ifndef GEOMALGOAPI_CONESEGMENT_H_
#define GEOMALGOAPI_CONESEGMENT_H_

#include <GeomAlgoAPI_MakeShape.h>

/**\class GeomAlgoAPI_ConeSegment
 * \ingroup DataAlgo
 * \brief Allows to create Cone Segment GDML Primitives.
 */
class GeomAlgoAPI_ConeSegment : public GeomAlgoAPI_MakeShape
{
 public:
  GEOMALGOAPI_EXPORT GeomAlgoAPI_ConeSegment();

  /// Creates a cone segment using standard GDML parameters.
  /// \param theRMin1 Cone base inner radius.
  /// \param theRMax1 Cone base outer radius.
  /// \param theRMin2 Cone top inner radius.
  /// \param theRMax2 Cone top outer radius.
  /// \param theZ Cone height.
  /// \param theStartPhi Start angle of the cone segment.
  /// \param theDeltaPhi Delta angle of the cone segment.
  GEOMALGOAPI_EXPORT GeomAlgoAPI_ConeSegment(const double theRMin1,
                                             const double theRMax1,
                                             const double theRMin2,
                                             const double theRMax2,
                                             const double theZ,
                                             const double theStartPhi,
                                             const double theDeltaPhi);

  /// Checks if the set of parameters used to define the cone segment are OK.
  GEOMALGOAPI_EXPORT bool check();

  /// Builds the cone segment based on the parameters given in the constructor.
  GEOMALGOAPI_EXPORT void build();

 private:
  double myRMin1; /// Cone base inner radius.
  double myRMax1; /// Cone base outer radius.
  double myRMin2; /// Cone top inner radius.
  double myRMax2; /// Cone top outer radius.
  double myZ; /// Cone height.
  double myStartPhi; /// Start angle of the cone segment.
  double myDeltaPhi; /// Delta angle of the cone segment.
};

#endif // GEOMALGOAPI_CONESEGMENT_H_

