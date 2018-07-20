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
// See http://www.salome-platform.org/ or
// email : webmaster.salome@opencascade.com<mailto:webmaster.salome@opencascade.com>
//

#include "GeomAlgoAPI_Boolean.h"

#include <GeomAlgoAPI_DFLoader.h>
#include <GeomAlgoAPI_ShapeTools.h>

#include <BOPAlgo_BOP.hxx>
#include <TopTools_ListOfShape.hxx>

//=================================================================================================
GeomAlgoAPI_Boolean::GeomAlgoAPI_Boolean(const GeomShapePtr theObject,
                                         const ListOfShape& theTools,
                                         const OperationType theOperationType)
{
  ListOfShape aListWithObject;
  aListWithObject.push_back(theObject);
  build(aListWithObject, theTools, theOperationType);
}

//=================================================================================================
GeomAlgoAPI_Boolean::GeomAlgoAPI_Boolean(const ListOfShape& theObjects,
                                         const ListOfShape& theTools,
                                         const OperationType theOperationType)
{
  build(theObjects, theTools, theOperationType);
}


//=================================================================================================
void GeomAlgoAPI_Boolean::build(const ListOfShape& theObjects,
                                const ListOfShape& theTools,
                                const OperationType theOperationType)
{
  if(theObjects.empty() || theTools.empty()) {
    return;
  }

  // Getting objects.
  TopTools_ListOfShape anObjects;
  for(ListOfShape::const_iterator
    anObjectsIt = theObjects.begin(); anObjectsIt != theObjects.end(); anObjectsIt++)
  {
    anObjects.Append((*anObjectsIt)->impl<TopoDS_Shape>());
  }

  // Getting tools.
  TopTools_ListOfShape aTools;
  for(ListOfShape::const_iterator
    aToolsIt = theTools.begin(); aToolsIt != theTools.end(); aToolsIt++)
  {
    aTools.Append((*aToolsIt)->impl<TopoDS_Shape>());
  }

  // Creating boolean operation.
  BOPAlgo_BOP* aBuilder = new BOPAlgo_BOP();
  switch (theOperationType) {
    case BOOL_CUT: {
      aBuilder->SetOperation(BOPAlgo_CUT);
      break;
    }
    case BOOL_FUSE: {
      aBuilder->SetOperation(BOPAlgo_FUSE);
      break;
    }
    case BOOL_COMMON: {
      aBuilder->SetOperation(BOPAlgo_COMMON);
      break;
    }
    default: {
      return;
    }
  }
  this->setImpl(aBuilder);
  this->setBuilderType(OCCT_BOPAlgo_Builder);
  aBuilder->SetArguments(anObjects);
  aBuilder->SetTools(aTools);

  // Building and getting result.
  aBuilder->Perform();
  if (aBuilder->HasErrors())
    return;
  TopoDS_Shape aResult = aBuilder->Shape();

  if(aResult.ShapeType() == TopAbs_COMPOUND) {
    aResult = GeomAlgoAPI_DFLoader::refineResult(aResult);
  }
  if(aResult.ShapeType() == TopAbs_COMPOUND) {
    std::shared_ptr<GeomAPI_Shape> aGeomShape(new GeomAPI_Shape);
    aGeomShape->setImpl(new TopoDS_Shape(aResult));
    ListOfShape aCompSolids, aFreeSolids;
    aGeomShape = GeomAlgoAPI_ShapeTools::combineShapes(aGeomShape,
                                                       GeomAPI_Shape::COMPSOLID,
                                                       aCompSolids,
                                                       aFreeSolids);
    aResult = aGeomShape->impl<TopoDS_Shape>();
  }

  std::shared_ptr<GeomAPI_Shape> aShape(new GeomAPI_Shape());
  aShape->setImpl(new TopoDS_Shape(aResult));
  this->setShape(aShape);
  this->setDone(true);
}
