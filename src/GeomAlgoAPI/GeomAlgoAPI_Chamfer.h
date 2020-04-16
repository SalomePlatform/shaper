// Copyright (C) 2017-2020  CEA/DEN, EDF R&D
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

#ifndef GeomAlgoAPI_Chamfer_H_
#define GeomAlgoAPI_Chamfer_H_

#include <GeomAlgoAPI.h>
#include <GeomAlgoAPI_MakeShape.h>

#include <GeomAPI_Shape.h>
//#include <GeomAPI_Edge.h>
//#include <GeomAPI_Face.h>

/// \class GeomAlgoAPI_Chamfer
/// \ingroup DataAlgo
/// \brief Perform chamfer
class GeomAlgoAPI_Chamfer : public GeomAlgoAPI_MakeShape
{
public:
  /// Run chamfer operation with two distances or with a distance and an angle .
  /// \param theBaseSolid      a changing solid
  /// \param theChamferShapes  list of edges the chamfer is performed on
  /// \param theMapEdgeFace    map that associates an edge to a face when the chamfer is applied
  ///                          to a face
  /// \param performDistances  boolean that indicates whether the operation is performed with
  ///                          two distances or not
  /// \param theVal1           double D1 if performDistances is true or D
  /// \param theVal2           double D2 if performDistances is true or Angle
  GEOMALGOAPI_EXPORT GeomAlgoAPI_Chamfer(const GeomShapePtr& theBaseSolid,
                                         const ListOfShape&  theChamferShapes,
                                         const std::map<GeomShapePtr, GeomShapePtr> theMapEdgeFace,
                                         const bool performDistances,
                                         const double theVal1,
                                         const double theVal2);

private:
  /// Perform chamfer operation.
  /// \param theBaseSolid     a changing solid
  /// \param theChamferShapes list of edges the chamfer is performed on
  /// \param theMapEdgeFace   map that associates an edge to a face when the chamfer is applied
  ///                         to a face
  /// \param performDistances boolean that indicates whether the operation is performed with two
  ///                         distances or not
  /// \param theVal1          double D1 if performDistances is true or D
  /// \param theVal2          double D2 if performDistances is true or Angle
  GEOMALGOAPI_EXPORT void build(const GeomShapePtr& theBaseSolid,
                                const ListOfShape&  theChamferShapes,
                                const std::map<GeomShapePtr, GeomShapePtr> theMapEdgeFace,
                                const bool performDistances,
                                const double theVal1,
                                const double theVal2);
};

#endif
