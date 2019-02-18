// Copyright (C) 2014-2019  CEA/DEN, EDF R&D
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

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

