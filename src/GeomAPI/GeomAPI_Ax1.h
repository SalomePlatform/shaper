// Copyright (C) 2014-2021  CEA/DEN, EDF R&D
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

#ifndef GEOMAPI_AX1_H_
#define GEOMAPI_AX1_H_

#include <GeomAPI.h>
#include <GeomAPI_Pnt.h>
#include <GeomAPI_Dir.h>

/** \ingroup DataModel
 *  \brief The class represents an axis in 3D space.
 */
class GeomAPI_Ax1 : public GeomAPI_Interface
{
public:
  /// Default constructor.
  GEOMAPI_EXPORT
  GeomAPI_Ax1();

  /** \brief Constructor.
   *  \param[in] theOrigin point of origin.
   *  \param[in] theDir direction of axis.
   */
  GEOMAPI_EXPORT
  GeomAPI_Ax1(std::shared_ptr<GeomAPI_Pnt> theOrigin,
              std::shared_ptr<GeomAPI_Dir> theDir);

  /// Sets origin point.
  GEOMAPI_EXPORT
  void setOrigin(const std::shared_ptr<GeomAPI_Pnt>& theOrigin);

  /// \return the plane origin point.
  GEOMAPI_EXPORT
  std::shared_ptr<GeomAPI_Pnt> origin() const;

  /// Sets direction vector.
  GEOMAPI_EXPORT
  void setDir(const std::shared_ptr<GeomAPI_Dir>& theDir);

  /// \return direction vector.
  GEOMAPI_EXPORT
  std::shared_ptr<GeomAPI_Dir> dir() const;

  /// Reverses the unit vector of this axis and assigns the result to this axis.
  GEOMAPI_EXPORT
  void reverse();

  /// \return reversed unit vector of this axis.
  GEOMAPI_EXPORT
  std::shared_ptr<GeomAPI_Ax1> reversed();
};

#endif
