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

#ifndef GeomAlgoAPI_NExplode_H_
#define GeomAlgoAPI_NExplode_H_

#include "GeomAlgoAPI.h"

#include <GeomAPI_Shape.h>

/// \class GeomAlgoAPI_NExplode
/// \ingroup DataAlgo
/// \brief Sort shapes by their centers of mass, using formula X*999 + Y*99 + Z*0.9.
/// Algorithm is copied from GEOM module, which uses nexplode Draw command from OCCT.
/// Used for getting index of sub0shape in WeakNaming algorithm.
class GeomAlgoAPI_NExplode
{
 public:
   /// \brief Initializes the sorted list of shapes by the context shape and type of sub-shapes.
   GEOMALGOAPI_EXPORT GeomAlgoAPI_NExplode(
     const GeomShapePtr theContext, const GeomAPI_Shape::ShapeType theShapeType);

   /// Returns an index (started from one) of sub-shape in the sorted list. Returns 0 if not found.
   GEOMALGOAPI_EXPORT int index(const GeomShapePtr theSubShape);
   /// Returns a shape by an index (started from one). Returns null if not found.
   GEOMALGOAPI_EXPORT GeomShapePtr shape(const int theIndex);

protected:
  ListOfShape mySorted;
};

#endif
