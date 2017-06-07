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

#ifndef GeomAlgoAPI_Prism_H_
#define GeomAlgoAPI_Prism_H_

#include "GeomAlgoAPI.h"

#include "GeomAlgoAPI_MakeSweep.h"

#include <GeomAPI_Dir.h>
#include <GeomAPI_Shape.h>

#include <memory>

/// \class GeomAlgoAPI_Prism
/// \ingroup DataAlgo
/// \brief Allows to create the prism based on a given face and bounding planes.
/// \n Note that only planar faces are allowed as bounding faces and resulting
/// extrusion will be bounded by the infinite planes taken from the faces.
class GeomAlgoAPI_Prism : public GeomAlgoAPI_MakeSweep
{
public:
  /// \brief Creates extrusion for the given shape along the normal for this shape.
  /// \param[in] theBaseShape planar face or wire to be extruded.
  /// \param[in] theToSize offset for "to" plane.
  /// \param[in] theFromSize offset for "from" plane.
  GEOMALGOAPI_EXPORT GeomAlgoAPI_Prism(const GeomShapePtr theBaseShape,
                                       const double       theToSize,
                                       const double       theFromSize);

  /// \brief Creates extrusion for the given shape along the normal for this shape.
  /// \param[in] theBaseShape vertex, edge, wire, face or shell.
  /// \param[in] theDirection direction of extrusion.
  ///                         Can be empty if theBaseShape is planar wire or face.
  /// \param[in] theToSize offset for "to" plane.
  /// \param[in] theFromSize offset for "from" plane.
  GEOMALGOAPI_EXPORT GeomAlgoAPI_Prism(const GeomShapePtr                 theBaseShape,
                                       const std::shared_ptr<GeomAPI_Dir> theDirection,
                                       const double                       theToSize,
                                       const double                       theFromSize);

  /// \brief Creates extrusion for the given shape along the normal for this shape.
  /// \param[in] theBaseShape planar face or wire to be extruded.
  /// \param[in] theToShape top bounding shape. Can be empty.
  ///                       In this case offset will be applied to the basis.
  /// \param[in] theToSize offset for "to" plane.
  /// \param[in] theFromShape bottom bounding shape. Can be empty.
  ///                         In this case offset will be applied to the basis.
  /// \param[in] theFromSize offset for "from" plane.
  GEOMALGOAPI_EXPORT GeomAlgoAPI_Prism(const GeomShapePtr theBaseShape,
                                       const GeomShapePtr theToShape,
                                       const double       theToSize,
                                       const GeomShapePtr theFromShape,
                                       const double       theFromSize);

  /// \brief Creates extrusion for the given shape along the normal for this shape.
  /// \param[in] theBaseShape planar face or wire to be extruded.
  /// \param[in] theDirection direction of extrusion.
  ///                         Can be empty if theBaseShape is planar wire or face.
  /// \param[in] theToShape top bounding shape. Can be empty.
  ///                       In this case offset will be applied to the basis.
  /// \param[in] theToSize offset for "to" plane.
  /// \param[in] theFromShape bottom bounding shape. Can be empty.
  ///                         In this case offset will be applied to the basis.
  /// \param[in] theFromSize offset for "from" plane.
  GEOMALGOAPI_EXPORT GeomAlgoAPI_Prism(const GeomShapePtr                 theBaseShape,
                                       const std::shared_ptr<GeomAPI_Dir> theDirection,
                                       const GeomShapePtr                 theToShape,
                                       const double                       theToSize,
                                       const GeomShapePtr                 theFromShape,
                                       const double                       theFromSize);

private:
  /// Builds resulting shape.
  void build(const GeomShapePtr&                theBaseShape,
             const std::shared_ptr<GeomAPI_Dir> theDirection,
             const GeomShapePtr&                theToShape,
             const double                       theToSize,
             const GeomShapePtr&                theFromShape,
             const double                       theFromSize);
};

#endif
