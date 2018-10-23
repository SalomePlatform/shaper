// Copyright (C) 2017  CEA/DEN, EDF R&D
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

#ifndef Selector_NExplode_H_
#define Selector_NExplode_H_

#include "Selector.h"

#include <TopoDS_Shape.hxx>
#include <TopoDS_ListOfShape.hxx>

/// \class Selector_NExplode
/// \ingroup DataModel
/// \brief Sort shapes by their centers of mass, using formula X*999 + Y*99 + Z*0.9.
/// Algorithm is copied from GEOM module, which uses nexplode Draw command from OCCT.
/// Used for getting index of sub0shape in WeakNaming algorithm.
class Selector_NExplode
{
 public:
   /// \brief Initializes the sorted list of shapes by the shapes list.
   SELECTOR_EXPORT Selector_NExplode(const TopoDS_ListOfShape& theShapes);
   /// \brief Initializes the sorted list of shapes by the context shape and type of sub-shapes.
   SELECTOR_EXPORT Selector_NExplode(const TopoDS_Shape& theShape, const TopAbs_ShapeEnum theType);

   /// Returns an index (started from one) of sub-shape in the sorted list. Returns 0 if not found.
   SELECTOR_EXPORT int index(const TopoDS_Shape& theSubShape);
   /// Returns a shape by an index (started from one). Returns null if not found.
   SELECTOR_EXPORT TopoDS_Shape shape(const int theIndex);

protected:
  TopoDS_ListOfShape mySorted; ///< keepthe the ordered list of shapes
};

#endif
