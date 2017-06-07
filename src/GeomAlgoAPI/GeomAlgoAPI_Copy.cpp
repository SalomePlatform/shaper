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

#include "GeomAlgoAPI_Copy.h"


#include <BRepBuilderAPI_Copy.hxx>

//=================================================================================================
GeomAlgoAPI_Copy::GeomAlgoAPI_Copy(const std::shared_ptr<GeomAPI_Shape> theShape,
                                   const bool theCopyGeom,
                                   const bool theCopyMesh)
{
  build(theShape, theCopyGeom, theCopyMesh);
}


//=================================================================================================
void GeomAlgoAPI_Copy::build(const std::shared_ptr<GeomAPI_Shape> theShape,
                             const bool theCopyGeom,
                             const bool theCopyMesh)
{
  if(!theShape.get()) {
    return;
  }

  // Getting shape.
  const TopoDS_Shape& aBaseShape = theShape->impl<TopoDS_Shape>();

  // Creating copy.
  BRepBuilderAPI_Copy* aBuilder = new BRepBuilderAPI_Copy(aBaseShape, theCopyGeom, theCopyMesh);
  this->setImpl(aBuilder);
  this->setBuilderType(OCCT_BRepBuilderAPI_MakeShape);

  TopoDS_Shape aResult = aBuilder->Shape();

  std::shared_ptr<GeomAPI_Shape> aShape(new GeomAPI_Shape());
  aShape->setImpl(new TopoDS_Shape(aResult));
  this->setShape(aShape);
  this->setDone(true);
}
