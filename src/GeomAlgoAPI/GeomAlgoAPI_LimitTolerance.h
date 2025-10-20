// Copyright (C) 2014-2025  CEA, EDF
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

#ifndef GeomAlgoAPI_LimitTolerance_H_
#define GeomAlgoAPI_LimitTolerance_H_

#include "GeomAlgoAPI.h"
#include "GeomAlgoAPI_MakeShape.h"

#include <GeomAPI_Shape.h>

/// \class GeomAlgoAPI_LimitTolerance
/// \ingroup DataAlgo
/// \brief Allows changing the tolerance in the shape and all of its sub shapes.
class GeomAlgoAPI_LimitTolerance : public GeomAlgoAPI_MakeShape
{
public:
  /// @brief Constructor.
  /// @param[in] theShape for which tolerance should be changed
  /// @param[in] theTolerance the new tolerance
  /// @param[in] theCheckGeometry if True, also check geometry
  GEOMALGOAPI_EXPORT GeomAlgoAPI_LimitTolerance(const GeomShapePtr theShape,
                                                const double theTolerance,
                                                const bool theCheckGeometry = false,
                                                const bool theExactAdjust = false);

private:
  /// Builds resulting shape.
  void build(const GeomShapePtr theShape,
             const double theTolerance,
             const bool theCheckGeometry,
             const bool theExactAdjust);
};

#endif
