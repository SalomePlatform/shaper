// Copyright (C) 2014-2017  CEA/DEN, EDF R&D
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

#ifndef GeomAPI_Dir_H_
#define GeomAPI_Dir_H_

#include <GeomAPI_Interface.h>
#include <memory>

class GeomAPI_XYZ;

/**\class GeomAPI_Dir
 * \ingroup DataModel
 * \brief 3D direction defined by three normalized coordinates
 */

class GeomAPI_Dir : public GeomAPI_Interface
{
 public:
  /// Creation of direction by coordinates
  GEOMAPI_EXPORT
  GeomAPI_Dir(const double theX, const double theY, const double theZ);
  /// Creation of direction by coordinates
  GEOMAPI_EXPORT
  GeomAPI_Dir(const std::shared_ptr<GeomAPI_XYZ>& theCoords);

  /// returns X coordinate
  GEOMAPI_EXPORT
  double x() const;
  /// returns Y coordinate
  GEOMAPI_EXPORT
  double y() const;
  /// returns Z coordinate
  GEOMAPI_EXPORT
  double z() const;

  /// returns coordinates of the direction
  GEOMAPI_EXPORT
  const std::shared_ptr<GeomAPI_XYZ> xyz();

  /// inverses the direction
  GEOMAPI_EXPORT void reverse();

  /// result is a scalar product of directions
  GEOMAPI_EXPORT
  double dot(const std::shared_ptr<GeomAPI_Dir>& theArg) const;
  /// result is a cross product of two directions
  GEOMAPI_EXPORT
  const std::shared_ptr<GeomAPI_XYZ> cross(const std::shared_ptr<GeomAPI_Dir>& theArg) const;

  /// calculates angle between two directions
  GEOMAPI_EXPORT
  double angle(const std::shared_ptr<GeomAPI_Dir>& theArg) const;

  /// \return true if the angle between this unit vector and
  /// theDir unit vector is equal to 0 or to Pi.
  GEOMAPI_EXPORT
  bool isParallel(const std::shared_ptr<GeomAPI_Dir> theDir,
                  const double theTolerance = 1.e-7) const;


};

//! Pointer on the object
typedef std::shared_ptr<GeomAPI_Dir> GeomDirPtr;

#endif

