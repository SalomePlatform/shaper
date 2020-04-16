// Copyright (C) 2018-2020  CEA/DEN, EDF R&D
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

#ifndef GeomAPI_Cylinder_H_
#define GeomAPI_Cylinder_H_

#include <GeomAPI_Interface.h>
#include <memory>

class GeomAPI_Dir;
class GeomAPI_Pnt;

/**\class GeomAPI_Cylinder
 * \ingroup DataModel
 * \brief Cylindrical surface in 3D
 */
class GeomAPI_Cylinder : public GeomAPI_Interface
{
public:
  /// Creation of infinite cylinder defined by location, axis and radius
  GEOMAPI_EXPORT GeomAPI_Cylinder(const std::shared_ptr<GeomAPI_Pnt>& theLocation,
                                  const std::shared_ptr<GeomAPI_Dir>& theAxis,
                                  const double theRadius);

  /// Creation of cylinder defined by location, axis, radius and height
  GEOMAPI_EXPORT GeomAPI_Cylinder(const std::shared_ptr<GeomAPI_Pnt>& theLocation,
                                  const std::shared_ptr<GeomAPI_Dir>& theAxis,
                                  const double theRadius,
                                  const double theHeight);

  /// Return location of the cylinder
  GEOMAPI_EXPORT std::shared_ptr<GeomAPI_Pnt> location() const;

  /// Return axis of the cylinder
  GEOMAPI_EXPORT std::shared_ptr<GeomAPI_Dir> axis() const;

  /// Return radius of the cylinder
  GEOMAPI_EXPORT double radius() const;

  /// Return height of the cylinder or Precision::Infinite() if the cylinder is infinite
  GEOMAPI_EXPORT double height() const;

  /// Return \c true is the cylinder is infinite
  GEOMAPI_EXPORT bool isInfinite() const;

  /// Returns true if cylinders have same axis and radii.
  GEOMAPI_EXPORT bool isCoincident(const std::shared_ptr<GeomAPI_Cylinder> theCylinder,
                                   const double theTolerance = 1.e-7);

private:
  double myHeight;
};

//! Pointer on the object
typedef std::shared_ptr<GeomAPI_Cylinder> GeomCylinderPtr;

#endif
