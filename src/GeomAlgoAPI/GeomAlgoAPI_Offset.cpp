// Copyright (C) 2019-2020  CEA/DEN, EDF R&D
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

#include "GeomAlgoAPI_Offset.h"

#include <BRepOffsetAPI_MakeOffsetShape.hxx>


GeomAlgoAPI_Offset::GeomAlgoAPI_Offset(const GeomShapePtr& theShape,
                                       const double theOffsetValue)
{
  build(theShape, theOffsetValue);
}

void GeomAlgoAPI_Offset::build(const GeomShapePtr& theShape, const double theOffsetValue)
{
  BRepOffsetAPI_MakeOffsetShape* anOffsetAlgo = new BRepOffsetAPI_MakeOffsetShape;
  anOffsetAlgo->PerformBySimple(theShape->impl<TopoDS_Shape>(), theOffsetValue);
  setImpl(anOffsetAlgo);
  setBuilderType(OCCT_BRepBuilderAPI_MakeShape);

  if (anOffsetAlgo->IsDone()) {
    const TopoDS_Shape& aResult = anOffsetAlgo->Shape();
    std::shared_ptr<GeomAPI_Shape> aShape(new GeomAPI_Shape());
    aShape->setImpl(new TopoDS_Shape(aResult));
    setShape(aShape);
    setDone(true);
  }
}

void GeomAlgoAPI_Offset::generated(const GeomShapePtr theOldShape,
                                   ListOfShape& theNewShapes)
{
  try {
    GeomAlgoAPI_MakeShape::generated(theOldShape, theNewShapes);
  } catch(...) {
    // nothing is generated
  }
}
