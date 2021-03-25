// Copyright (C) 2014-2021  CEA/DEN, EDF R&D
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

#include "GeomAPI_Tools.h"

#include "GeomAPI_Vertex.h"
#include "GeomAPI_Edge.h"
#include "GeomAPI_Face.h"
#include "GeomAPI_Wire.h"

//==================================================================================================
GeomShapePtr GeomAPI_Tools::getTypedShape(const GeomShapePtr& theShape)
{
  if (!theShape.get())
    return GeomShapePtr();

  GeomShapePtr aTypedShape = theShape;

  switch (theShape->shapeType()) {
  case GeomAPI_Shape::VERTEX:
    aTypedShape = GeomShapePtr(new GeomAPI_Vertex(theShape));
    break;
  case GeomAPI_Shape::EDGE:
    aTypedShape = GeomShapePtr(new GeomAPI_Edge(theShape));
    break;
  case GeomAPI_Shape::FACE:
    aTypedShape = GeomShapePtr(new GeomAPI_Face(theShape));
    break;
  case GeomAPI_Shape::WIRE:
    aTypedShape = GeomShapePtr(new GeomAPI_Wire(theShape));
    break;
  default:
    break;
  }

  return aTypedShape;
}
