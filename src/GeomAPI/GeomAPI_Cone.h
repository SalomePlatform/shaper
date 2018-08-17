// Copyright (C) 2018-20xx  CEA/DEN, EDF R&D
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
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
//
// See http://www.salome-platform.org/ or
// email : webmaster.salome@opencascade.com<mailto:webmaster.salome@opencascade.com>
//

#ifndef GeomAPI_Cone_H_
#define GeomAPI_Cone_H_

#include <GeomAPI_Interface.h>
#include <memory>

class GeomAPI_Dir;
class GeomAPI_Pnt;

/**\class GeomAPI_Cone
 * \ingroup DataModel
 * \brief Conical surface in 3D
 */
class GeomAPI_Cone : public GeomAPI_Interface
{
public:
  /// Creation of infinite cone defined by apex, axis and semi-angle
  GEOMAPI_EXPORT GeomAPI_Cone(const std::shared_ptr<GeomAPI_Pnt>& theApex,
                              const std::shared_ptr<GeomAPI_Dir>& theAxis,
                              const double theSemiAngle);

  /// Creation of semi-infinite cone by location and radius of reference circle, axis, semi-angle
  GEOMAPI_EXPORT GeomAPI_Cone(const std::shared_ptr<GeomAPI_Pnt>& theLocation,
                              const std::shared_ptr<GeomAPI_Dir>& theAxis,
                              const double theSemiAngle,
                              const double theRadius);

  /// Creating of cone by location of reference circle, axis, semi-angle and
  /// radii of boundary circles
  GEOMAPI_EXPORT GeomAPI_Cone(const std::shared_ptr<GeomAPI_Pnt>& theLocation,
                              const std::shared_ptr<GeomAPI_Dir>& theAxis,
                              const double theSemiAngle,
                              const double theRadius1,
                              const double theRadius2);

  /// Return apex of the cone
  GEOMAPI_EXPORT std::shared_ptr<GeomAPI_Pnt> apex() const;

  /// Return location of the cone (center of the circle nearest to the apex)
  GEOMAPI_EXPORT std::shared_ptr<GeomAPI_Pnt> location() const;

  /// Return axis of the cone
  GEOMAPI_EXPORT std::shared_ptr<GeomAPI_Dir> axis() const;

  /// Return semi-angle of the cone
  GEOMAPI_EXPORT double semiAngle() const;

  /// Return first radius of the cone or Precision::Infinite() if the cone is infinite
  GEOMAPI_EXPORT double radius1() const;

  /// Return second radius of the cone or Precision::Infinite() if the cone is infinite
  GEOMAPI_EXPORT double radius2() const;

  /// Return height of the cone
  GEOMAPI_EXPORT double height() const;

  /// Return \c true is the cone is semi-infinite
  GEOMAPI_EXPORT bool isSemiInfinite() const;

  /// Return \c true is the cone is infinite
  GEOMAPI_EXPORT bool isInfinite() const;

private:
  double myRadius1, myRadius2;
};

//! Pointer on the object
typedef std::shared_ptr<GeomAPI_Cone> GeomConePtr;

#endif
