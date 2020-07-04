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

#ifndef GeomAlgoAPI_WireBuilder_H_
#define GeomAlgoAPI_WireBuilder_H_

#include "GeomAlgoAPI.h"
#include "GeomAlgoAPI_MakeShapeCustom.h"

#include <GeomAPI_Shape.h>

/// \class GeomAlgoAPI_WireBuilder
/// \ingroup DataAlgo
/// \brief Allows to create wire-shapes by different parameters.
class GeomAlgoAPI_WireBuilder : public GeomAlgoAPI_MakeShapeCustom
{
 public:
   /// \brief Creates a wire from edges and wires.
   /// \param[in] theShapes list of shapes. Only edges and wires allowed.
   /// \param[in] theForceOpenWire indicates the necessity to split wire
   ///                             in the first vertex if it becomes closed.
   /// The edges are not to be consecutive.
   /// But they are to be all connected geometrically or topologically.
   GEOMALGOAPI_EXPORT GeomAlgoAPI_WireBuilder(const ListOfShape& theShapes,
                                              const bool theForceOpenWire = false);

   /// \brief Creates a wire from edges and wires.
   /// \param[in] theShapes list of shapes. Only edges and wires allowed.
   /// The edges are not to be consecutive.
   /// But they are to be all connected geometrically or topologically.
   /// \return wire created from theShapes. Empty in case of error or bad input.
   GEOMALGOAPI_EXPORT static GeomShapePtr wire(const ListOfShape& theShapes);

   /// \brief Checks if the wire is self-intersected.
   /// \param[in] theWire the wire to be checked
   /// \return true if the wire is self-intersected, otherwise - false.
   GEOMALGOAPI_EXPORT static bool isSelfIntersected(const GeomShapePtr& theWire);
};

#endif
