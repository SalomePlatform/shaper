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

#ifndef GEOMAPI_AX3_H_
#define GEOMAPI_AX3_H_

#include "GeomAPI.h"
#include "GeomAPI_Pnt.h"
#include "GeomAPI_Dir.h"

/**
 * \ingroup DataModel
 * \brief The class represents a coordinate plane which is 2d plane with X and Y directions
 * and origin
 */ 
class GeomAPI_Ax3 : public GeomAPI_Interface
{
public:
  /// Default constructor
  GEOMAPI_EXPORT
  GeomAPI_Ax3();

  /// Constructor
  /// \param theOrigin point of origin
  /// \param theDirX direction of X axis
  /// \param theNorm direction of normal vector
  GEOMAPI_EXPORT
  GeomAPI_Ax3(std::shared_ptr<GeomAPI_Pnt> theOrigin,
              std::shared_ptr<GeomAPI_Dir> theDirX,
              std::shared_ptr<GeomAPI_Dir> theNorm);

  /// Sets origin point
  GEOMAPI_EXPORT
  void setOrigin(const std::shared_ptr<GeomAPI_Pnt>& theOrigin);

  /// Returns the plane origin point
  GEOMAPI_EXPORT
  std::shared_ptr<GeomAPI_Pnt> origin() const;

  /// Sets X direction vector
  GEOMAPI_EXPORT
  void setDirX(const std::shared_ptr<GeomAPI_Dir>& theDirX);

  /// Returns X direction vector
  GEOMAPI_EXPORT
  std::shared_ptr<GeomAPI_Dir> dirX() const;

  /// Sets Y direction vector
  GEOMAPI_EXPORT
  void setDirY(const std::shared_ptr<GeomAPI_Dir>& theDirY);

  /// Returns Y direction vector
  GEOMAPI_EXPORT
  std::shared_ptr<GeomAPI_Dir> dirY() const;

  /// Sets Z direction vector
  GEOMAPI_EXPORT
  void setNormal(const std::shared_ptr<GeomAPI_Dir>& theNorm);

  /// Returns Z direction vector
  GEOMAPI_EXPORT
  std::shared_ptr<GeomAPI_Dir> normal() const;

  /// Converts 2d coordinates from the plane to 3d space point
  /// \param theX X coordinate
  /// \param theY Y coordinate
  GEOMAPI_EXPORT
  std::shared_ptr<GeomAPI_Pnt> to3D(double theX, double theY) const;

  /// Converts 3d  to 2d coordinates of the plane
  /// \param theX X coordinate
  /// \param theY Y coordinate
  /// \param theZ Z coordinate
  GEOMAPI_EXPORT
  std::shared_ptr<GeomAPI_Pnt2d> to2D(double theX, double theY, double theZ) const;

};


#endif