// Copyright (C) 2017-2019  CEA/DEN, EDF R&D
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
