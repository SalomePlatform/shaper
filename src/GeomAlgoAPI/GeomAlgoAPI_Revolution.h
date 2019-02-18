// Copyright (C) 2014-2019  CEA/DEN, EDF R&D
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

#ifndef GeomAlgoAPI_Revolution_H_
#define GeomAlgoAPI_Revolution_H_

#include "GeomAlgoAPI.h"

#include "GeomAlgoAPI_MakeSweep.h"

#include <GeomAPI_Ax1.h>

/// \class GeomAlgoAPI_Revolution
/// \ingroup DataAlgo
/// \brief Allows to create the revolution based on a given face, angles and bounding planes.
/// \n Note that only the planar faces are allowed as bounding faces and resulting
/// revolution will be bounded by the infinite planes taken from the faces.
/// \n If the bounding plane was specified with the angle then this plane will be rotated around
/// the axis to the value of the angle.
/// \n Note that algorithm return only one solid object. So in case when after cutting with bounding
/// planes algorithm got more than one solid it will return the closest to the center of mass of
/// the base face.
class GeomAlgoAPI_Revolution : public GeomAlgoAPI_MakeSweep
{
public:
  /// \brief Creates revolution for the given shape.
  /// \param[in] theBaseShape face for revolution.
  /// \param[in] theAxis axis for revolution.
  /// \param[in] theToShape to bounding shape. Can be empty.
  ///                       In this case offset will be applied to the basis.
  /// \param[in] theToAngle to angle.
  /// \param[in] theFromShape from bounding shape. Can be empty.
  ///                       In this case offset will be applied to the basis.
  /// \param[in] theFromAngle from angle.
  GEOMALGOAPI_EXPORT GeomAlgoAPI_Revolution(const GeomShapePtr                 theBaseShape,
                                            const std::shared_ptr<GeomAPI_Ax1> theAxis,
                                            const GeomShapePtr                 theToShape,
                                            const double                       theToAngle,
                                            const GeomShapePtr                 theFromShape,
                                            const double                       theFromAngle);

private:
  /// Builds resulting shape.
  void build(const GeomShapePtr&                 theBaseShape,
             const std::shared_ptr<GeomAPI_Ax1>& theAxis,
             const GeomShapePtr&                 theToShape,
             const double                        theToAngle,
             const GeomShapePtr&                 theFromShape,
             const double                        theFromAngle);
};

#endif