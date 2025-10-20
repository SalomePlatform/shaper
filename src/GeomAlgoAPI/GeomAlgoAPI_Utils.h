// Copyright (C) 2007-2025  CEA, EDF, OPEN CASCADE
//
// Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
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

#ifndef GeomAlgoAPI_Utils_H_
#define GeomAlgoAPI_Utils_H_

#include "GeomAlgoAPI.h"

#include <Precision.hxx>
#include <TopoDS_Shape.hxx>


namespace GeomAlgoAPI_Utils
{
  ///
  /// \brief Limit shape tolerance to the given value
  ///
  /// \param theShape shape being fixed
  /// \param theType topology type which tolerance is to be limited; TopAbs_SHAPE means
  ///                all types of topology
  /// \param theTolerance expected tolerance value (1e-7 by default)
  /// \param theCheckGeometry check geometry validity of result
  /// \return \c true if resulting shape is valid
  ///
  /// \note Resulting tolerance of the shape is not mandatory equal to requested value
  ///       as it might be changed by fixshape operation in order to get valid shape where possible
  /// \note By default, the result is only checked for topology validity; check of geometry can be done by
  ///       passing \c true to \a theCheckGeometry parameter
  ///
  GEOMALGOAPI_EXPORT bool FixShapeTolerance(TopoDS_Shape& theShape,
                                            const TopAbs_ShapeEnum theType = TopAbs_SHAPE,
                                            const double theTolerance = Precision::Confusion(),
                                            const bool theCheckGeometry = false,
                                            const bool theExactAdjust = false);

  ///
  /// \brief Check shape
  ///
  /// \param theShape input shape object
  /// \param theCheckGeometry when set to \c true, causes check of underlying geometry
  ///        in addition to the topology
  /// \return \c true if shape is valid or \c false otherwise
  ///
  GEOMALGOAPI_EXPORT bool CheckShape(TopoDS_Shape& theShape,
                                     const bool theCheckGeometry = false,
                                     const bool theExact = false);
}

#endif
