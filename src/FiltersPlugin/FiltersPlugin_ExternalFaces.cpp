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

#include "FiltersPlugin_ExternalFaces.h"

#include <ModelAPI_Tools.h>

#include <TopExp.hxx>
#include <TopTools_IndexedDataMapOfShapeListOfShape.hxx>

bool FiltersPlugin_ExternalFaces::isSupported(GeomAPI_Shape::ShapeType theType) const
{
  return theType == GeomAPI_Shape::FACE;
}

bool FiltersPlugin_ExternalFaces::isOk(const GeomShapePtr& theShape,
                                       const ResultPtr& theResult,
                                       const ModelAPI_FiltersArgs& theArgs) const
{
  if (!theShape->isFace())
    return false;

  // verify INTERNAL flag
  TopoDS_Shape aShape = theShape->impl<TopoDS_Shape>();
  if (aShape.Orientation() == TopAbs_INTERNAL)
    return false;

  // check number of solids containing the face
  ResultBodyPtr anOwner = ModelAPI_Tools::bodyOwner(theResult, true);
  if (!anOwner) {
    anOwner = std::dynamic_pointer_cast<ModelAPI_ResultBody>(theResult);
    if (!anOwner)
      return false;
  }
  GeomShapePtr anOwnerShape = anOwner->shape();

  TopTools_IndexedDataMapOfShapeListOfShape aMapFS;
  TopExp::MapShapesAndUniqueAncestors(anOwnerShape->impl<TopoDS_Shape>(),
                                      TopAbs_FACE, TopAbs_SOLID, aMapFS);
  const TopTools_ListOfShape& aSolids = aMapFS.FindFromKey(aShape);
  return aSolids.Extent() <= 1;
}
