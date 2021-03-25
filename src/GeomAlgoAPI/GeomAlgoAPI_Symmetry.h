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

#ifndef GEOMALGOAPI_SYMMETRY_H_
#define GEOMALGOAPI_SYMMETRY_H_

#include <GeomAlgoAPI.h>
#include <GeomAlgoAPI_Transform.h>

class GeomAPI_Ax1;
class GeomAPI_Ax2;
class GeomAPI_Pnt;

/// \class GeomAlgoAPI_Symmetry
/// \ingroup DataAlgo
/// \brief Creates a copy of the object by performing a symmetry operation by a point,
///        by an axis or by a plane.
class GeomAlgoAPI_Symmetry : public GeomAlgoAPI_Transform
{
public:
  /// \brief Creates an object which is obtained from current object by performing
  ///        a symmetry operation by a point.
  /// \param[in] theSourceShape  the shape to be moved.
  /// \param[in] thePoint        the symmetry point.
  GEOMALGOAPI_EXPORT GeomAlgoAPI_Symmetry(std::shared_ptr<GeomAPI_Shape> theSourceShape,
                                          std::shared_ptr<GeomAPI_Pnt>   thePoint);

  /// \brief Creates an object which is obtained from current object by performing
  ///        a symmetry operation by a point.
  /// \param[in] theSourceShape  the shape to be moved.
  /// \param[in] theAxis         the symmetry axis.
  GEOMALGOAPI_EXPORT GeomAlgoAPI_Symmetry(std::shared_ptr<GeomAPI_Shape> theSourceShape,
                                          std::shared_ptr<GeomAPI_Ax1>   theAxis);

  /// \brief Creates an object which is obtained from current object by performing
  ///        a symmetry operation by a point.
  /// \param[in] theSourceShape  the shape to be moved.
  /// \param[in] thePlane        the symmetry plane.
  GEOMALGOAPI_EXPORT GeomAlgoAPI_Symmetry(std::shared_ptr<GeomAPI_Shape> theSourceShape,
                                          std::shared_ptr<GeomAPI_Ax2>   thePlane);
};

#endif // GEOMALGOAPI_SYMMETRY_H_
