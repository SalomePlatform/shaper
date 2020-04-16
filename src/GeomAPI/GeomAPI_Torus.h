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

#ifndef GeomAPI_Torus_H_
#define GeomAPI_Torus_H_

#include <GeomAPI_Interface.h>
#include <memory>

class GeomAPI_Dir;
class GeomAPI_Pnt;

/**\class GeomAPI_Torus
 * \ingroup DataModel
 * \brief Toroidal surface in 3D
 */
class GeomAPI_Torus : public GeomAPI_Interface
{
public:
  /// Creation of torus defined by center point, direction, major and minor radii
  GEOMAPI_EXPORT GeomAPI_Torus(const std::shared_ptr<GeomAPI_Pnt>& theCenter,
                               const std::shared_ptr<GeomAPI_Dir>& theDir,
                               const double theMajorRadius,
                               const double theMinorRadius);

  /// Return center of the torus
  GEOMAPI_EXPORT std::shared_ptr<GeomAPI_Pnt> center() const;

  /// Return direction of the torus
  GEOMAPI_EXPORT std::shared_ptr<GeomAPI_Dir> direction() const;

  /// Return major radius of the torus
  GEOMAPI_EXPORT double majorRadius() const;

  /// Return minor radius of the torus
  GEOMAPI_EXPORT double minorRadius() const;
};

//! Pointer on the object
typedef std::shared_ptr<GeomAPI_Torus> GeomTorusPtr;

#endif
