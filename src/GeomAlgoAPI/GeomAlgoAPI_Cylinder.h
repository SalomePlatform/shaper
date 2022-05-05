// Copyright (C) 2014-2022  CEA/DEN, EDF R&D
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

#ifndef GEOMALGOAPI_CYLINDER_H_
#define GEOMALGOAPI_CYLINDER_H_

#include <GeomAlgoAPI_MakeShape.h>

#include <GeomAPI_Ax2.h>
#include <GeomAPI_Pnt.h>

/**\class GeomAlgoAPI_Cylinder
 * \ingroup DataAlgo
 * \brief Allows to create Cylinder Primitives
 */
class GeomAlgoAPI_Cylinder : public GeomAlgoAPI_MakeShape
{
 public:
  /// Creates a cylinder.
  /// \param theAxis The axis of the cylinder
  /// \param theRadius The radius of the cylinder
  /// \param theHeight The height of the cylinder
  GEOMALGOAPI_EXPORT GeomAlgoAPI_Cylinder(std::shared_ptr<GeomAPI_Ax2> theAxis,
                                          const double theRadius,
                                          const double theHeight);

  /// Creates a cylinder.
  /// \param theAxis The axis of the cylinder
  /// \param theRadius The radius of the cylinder
  /// \param theHeight The height of the cylinder
  /// \param theAngle The covering angle of the cylinder
  GEOMALGOAPI_EXPORT GeomAlgoAPI_Cylinder(std::shared_ptr<GeomAPI_Ax2> theAxis,
                                          const double theRadius,
                                          const double theHeight,
                                          const double theAngle);

  /// Checks if data for the cylinder construction is OK.
  GEOMALGOAPI_EXPORT bool check();

  /// Builds the cylinder.
  GEOMALGOAPI_EXPORT void build();

 private:
  bool withAngle; /// Boolean indicating if the type of cylinder (full or portion).
  std::shared_ptr<GeomAPI_Pnt> myBasePoint; /// Center of the lower base of the cylinder.
  std::shared_ptr<GeomAPI_Ax2> myAxis; /// Axis of the cylinder.
  double myRadius; /// Radius of the cylinder.
  double myHeight; /// Height of the cylinder.
  double myAngle; /// Covering polar angle of the cylinder;
};


#endif // GEOMALGOAPI_CYLINDER_H_
