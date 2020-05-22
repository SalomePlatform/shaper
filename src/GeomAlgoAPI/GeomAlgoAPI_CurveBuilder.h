// Copyright (C) 2014-2020  CEA/DEN, EDF R&D
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

#ifndef GeomAlgoAPI_CurveBuilder_H_
#define GeomAlgoAPI_CurveBuilder_H_

#include "GeomAlgoAPI.h"

#include <GeomAPI_Pnt.h>
#include <GeomAPI_Edge.h>

/// \class GeomAlgoAPI_CurveBuilder
/// \ingroup DataAlgo
/// \brief Allows to create a curve by the list of point.
class GeomAlgoAPI_CurveBuilder
{
 public:
   /// \brief Creates an interpolation curve from points.
   /// \param[in] thePoints list of points.
   /// \param[in] thePeriodic defines whether the curve to be periodic.
   /// \param[in] theIsToReorder defines whether to change the order of points to construct
   ///            the shortest curve.
   /// \param[in] theStartTangent vector tangent to the start of curve.
   /// \param[in] theEndTangent vector tangent to the end of curve.
   /// \return Interpolation curve (edge). Empty in case of error or bad input.
   GEOMALGOAPI_EXPORT static GeomEdgePtr edge(const std::list<GeomPointPtr>& thePoints,
                                              const bool thePeriodic,
                                              const bool theIsToReorder,
                                              const GeomDirPtr& theStartTangent,
                                              const GeomDirPtr& theEndTangent);

   /// \brief Approximate the given points by a curve.
   /// \param[in] thePoints list of points.
   /// \param[in] thePeriodic defines whether the curve to be periodic.
   /// \param[in] thePrecision how close the curve should be to the points.
   /// \return Apporimation curve (edge). Empty in case of error or bad input.
   GEOMALGOAPI_EXPORT static GeomEdgePtr approximate(const std::list<GeomPointPtr>& thePoints,
                                                     const bool thePeriodic,
                                                     const double thePrecision);

   /// \brief Reoder the list of points to get a polyline of minimal length
   GEOMALGOAPI_EXPORT static void reorderPoints(std::list<GeomPointPtr>& thePoints);
};

#endif
