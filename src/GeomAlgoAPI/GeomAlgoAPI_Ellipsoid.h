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

#ifndef GEOMALGOAPI_ELLIPSOID_H_
#define GEOMALGOAPI_ELLIPSOID_H_

#include <GeomAlgoAPI_MakeShape.h>

/**\class GeomAlgoAPI_Ellipsoid
 * \ingroup DataAlgo
 * \brief Allows to create Ellipsoid GDML Primitives.
 */
class GeomAlgoAPI_Ellipsoid : public GeomAlgoAPI_MakeShape
{
 public:
  /// Creates a ellipsoid using standard GDML parameters.
  /// \param theAx X dimension of the ellipsoid.
  /// \param theBy Y dimension of the ellipsoid.
  /// \param theCz Z dimension of the ellipsoid.
  /// \param theZCut1 Lower z cut.
  /// \param theZCut2 Upper z cut.
  GEOMALGOAPI_EXPORT GeomAlgoAPI_Ellipsoid(const double theAx,
                                           const double theBy,
                                           const double theCz,
                                           const double theZCut1,
                                           const double theZCut2);

  /// Checks if the set of parameters used to define the ellipsoid are OK.
  GEOMALGOAPI_EXPORT bool check();

  /// Builds the ellipsoid based on the parameters given in the constructor.
  GEOMALGOAPI_EXPORT void build();

 private:
  double myAx; /// X dimension of the ellipsoid.
  double myBy; /// Y dimension of the ellipsoid.
  double myCz; /// Z dimension of the ellipsoid.
  double myZCut1; /// Lower z cut.
  double myZCut2; /// Upper z cut.
};

#endif // GEOMALGOAPI_ELLIPSOID_H_

