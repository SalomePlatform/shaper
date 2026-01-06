// Copyright (C) 2014-2026  CEA, EDF
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

#ifndef GeomAlgoAPI_RemoveExtraEdges_H_
#define GeomAlgoAPI_RemoveExtraEdges_H_

#include "GeomAlgoAPI.h"
#include "GeomAlgoAPI_MakeShapeCustom.h"

#include <GeomAPI_Shape.h>

#include <TopoDS_Shape.hxx>
#include <TopTools_MapOfShape.hxx>

/// \class GeomAlgoAPI_RemoveExtraEdges
/// \ingroup DataAlgo
/// \brief This tool tries to unify edges and, optionally, faces of the shape
///        that lie on the same geometry.
///        It works similar to GeomAlgoAPI_UnifySameDomain, but in addition
///        it tries to remove seam and degenerated edges on spherical surfaces.
class GeomAlgoAPI_RemoveExtraEdges: public GeomAlgoAPI_MakeShapeCustom
{
public:
  /// Constructor.
  /// \param[in] theShape base shape.
  /// \param[in] theDoUnionFaces boolean flag to enable/disable faces merging.
  GEOMALGOAPI_EXPORT GeomAlgoAPI_RemoveExtraEdges(const GeomShapePtr& theShape,
                                                  const bool theDoUnionFaces = false);

private:
  /// Builds resulting shape from the shape.
  void build(const GeomShapePtr& theShape, const bool theDoUnionFaces);

  /// Fill modification history.
  void addModifiedRecursively(const TopoDS_Shape& theShape,
                              const TopoDS_Shape& theResult,
                              TopTools_MapOfShape& theMapOfUnique);
  void addModifiedSub        (const TopoDS_Shape& theShape,
                              const TopoDS_Shape& theResult,
                              TopTools_MapOfShape& theMapOfUnique);
};

#endif
