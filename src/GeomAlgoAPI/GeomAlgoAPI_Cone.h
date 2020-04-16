// Copyright (C) 2017-2020  CEA/DEN, EDF R&D
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
