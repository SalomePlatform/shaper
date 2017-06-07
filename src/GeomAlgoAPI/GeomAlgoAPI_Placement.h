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
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com<mailto:webmaster.salome@opencascade.com>
//

#ifndef GeomAlgoAPI_Placement_H_
#define GeomAlgoAPI_Placement_H_

#include <GeomAlgoAPI.h>
#include <GeomAlgoAPI_MakeShape.h>

#include <GeomAPI_Shape.h>
#include <GeomAPI_Trsf.h>

/// \class GeomAlgoAPI_Placement
/// \ingroup DataAlgo
/// \brief Creates the copied object which sub-element is placed on the given element
class GeomAlgoAPI_Placement : public GeomAlgoAPI_MakeShape
{
public:
  /// \brief Creates an object which is obtained from current object by transformation calculated
  ///        as a movement of the source object to be coincident with the destination object
  /// \param[in] theSourceSolid  a shape to be moved
  /// \param[in] theDestSolid    invariant shape
  /// \param[in] theSourceShape  a shape on the solid to be made coincident with destination object
  /// \param[in] theDestShape    destination object
  /// \param[in] theIsReverse    indicates that the solid materials should be on the same side
  ///                            against the destination plane
  /// \param[in] theIsCentering  indicates the planes should be centered
  /// \param[in] theSimpleTransform makes just transformation of shape
  ///                               without changing of topology or geometry
  GEOMALGOAPI_EXPORT GeomAlgoAPI_Placement(const std::shared_ptr<GeomAPI_Shape> theSourceSolid,
                                           const std::shared_ptr<GeomAPI_Shape> theDestSolid,
                                           const std::shared_ptr<GeomAPI_Shape> theSourceShape,
                                           const std::shared_ptr<GeomAPI_Shape> theDestShape,
                                           const bool theIsReverse = false,
                                           const bool theIsCentering = false,
                                           const bool theSimpleTransform = false);

  /// Returns the simple transformation
  GEOMALGOAPI_EXPORT std::shared_ptr<GeomAPI_Trsf> transformation() const;

private:
  /// builds resulting shape
  void build(const std::shared_ptr<GeomAPI_Shape>& theSourceSolid,
             const std::shared_ptr<GeomAPI_Shape>& theDestSolid,
             const std::shared_ptr<GeomAPI_Shape>& theSourceShape,
             const std::shared_ptr<GeomAPI_Shape>& theDestShape,
             const bool theIsReverse,
             const bool theIsCentering,
             const bool theSimpleTransform);

private:
  std::shared_ptr<GeomAPI_Trsf> myTrsf; ///< transformation of the shape in case theSimpleTransform
};

#endif
