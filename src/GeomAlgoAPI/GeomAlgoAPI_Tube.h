// Copyright (C) 2017-2021  CEA/DEN, EDF R&D
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

#ifndef GEOMALGOAPI_TUBE_H_
#define GEOMALGOAPI_TUBE_H_

#include <GeomAlgoAPI_MakeShape.h>

/**\class GeomAlgoAPI_Tube
 * \ingroup DataAlgo
 * \brief Allows to create Tube Primitives
 */
class GeomAlgoAPI_Tube : public GeomAlgoAPI_MakeShape
{
 public:
  GEOMALGOAPI_EXPORT GeomAlgoAPI_Tube();

  /// Creates a tube.
  /// \param theRMin The inside radius
  /// \param theRMax The outside radius
  /// \param theZ The heigth
  GEOMALGOAPI_EXPORT GeomAlgoAPI_Tube(const double theRMin,
                                      const double theRMax,
                                      const double theZ);

  /// Checks if data for the torus construction is OK.
  GEOMALGOAPI_EXPORT bool check();

  /// Builds the torus.
  GEOMALGOAPI_EXPORT void build();

private:
  /// Builds the the tube with the inside radius, the outside radius and the height.
  void buildTube();

  double myRMin; /// inside radius to create a tube.
  double myRMax; /// outside radius to create a tube.
  double myZ; /// height to create a tube.
};

#endif // GEOMALGOAPI_TUBE_H_
