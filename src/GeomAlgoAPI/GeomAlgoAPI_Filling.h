// Copyright (C) 2017-20xx  CEA/DEN, EDF R&D
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

#ifndef GeomAlgoAPI_Filling_H_
#define GeomAlgoAPI_Filling_H_

#include <GeomAlgoAPI.h>
#include <GeomAlgoAPI_MakeShape.h>

#include <GeomAPI_Edge.h>

/// \class GeomAlgoAPI_Filling
/// \ingroup DataAlgo
/// \brief Perform building face by the set of edges/wires
class GeomAlgoAPI_Filling : public GeomAlgoAPI_MakeShape
{
public:
  /// \brief Construct filling operation by the parameters.
  GEOMALGOAPI_EXPORT GeomAlgoAPI_Filling(const int theMinDegree = 2,
                                         const int theMaxDegree = 5,
                                         const int theNbIter    = 0,
                                         const double theTol2D = 1.e-4,
                                         const double theTol3D = 1.e-4);

  /// \brief Add an edge to constrain filling (the result face should pass through the edge)
  GEOMALGOAPI_EXPORT void add(const GeomEdgePtr theEdge);

  /// \brief Perform filling operation
  /// \param isApproximate approximate curves before building a face
  GEOMALGOAPI_EXPORT void build(bool isApproximate = false);

private:
  /// \brief Perform filling using the given edges
  void buildByEdges();

  /// \brief Perform filling by a set of points calculated on each edge
  void buildByControlPoints();

private:
  int myMinDegree;
  int myMaxDegree;
  int myNbIter;
  double myTol2D;
  double myTol3D;
  std::list<GeomEdgePtr> myConstraints;
};

#endif
