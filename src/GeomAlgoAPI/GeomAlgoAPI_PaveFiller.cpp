// Copyright (C) 2014-2020  CEA/DEN, EDF R&D
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

#include "GeomAlgoAPI_PaveFiller.h"

#include <GeomAlgoAPI_DFLoader.h>
#include <GeomAlgoAPI_ShapeTools.h>

#include <BOPAlgo_Builder.hxx>
#include <BOPAlgo_PaveFiller.hxx>
#include <TopoDS_Builder.hxx>
#include <TopoDS_Iterator.hxx>

//=================================================================================================
GeomAlgoAPI_PaveFiller::GeomAlgoAPI_PaveFiller(const ListOfShape& theListOfShape,
                                               const bool theIsMakeCompSolids)
{
  build(theListOfShape, theIsMakeCompSolids);
}


//=================================================================================================
void GeomAlgoAPI_PaveFiller::build(const ListOfShape& theListOfShape,
                                   const bool theIsMakeCompSolids)
{
  BOPAlgo_PaveFiller* aPaveFiller = new BOPAlgo_PaveFiller;
  TopTools_ListOfShape aListOfShape;
  for(ListOfShape::const_iterator
    anIt = theListOfShape.cbegin(); anIt != theListOfShape.cend(); anIt++) {
    const TopoDS_Shape& aShape = (*anIt)->impl<TopoDS_Shape>();
    if(aShape.ShapeType() == TopAbs_COMPOUND) {
      for(TopoDS_Iterator anIter(aShape); anIter.More(); anIter.Next()) {
        aListOfShape.Append(anIter.Value());
      }
    } else {
      aListOfShape.Append(aShape);
    }
  }
  aPaveFiller->SetArguments(aListOfShape);
  aPaveFiller->Perform();
  if (aPaveFiller->HasErrors())
    return;

  BOPAlgo_Builder* aBuilder = new BOPAlgo_Builder();
  this->setImpl(aBuilder);
  this->setBuilderType(OCCT_BOPAlgo_Builder);
  aBuilder->SetArguments(aListOfShape);
  aBuilder->PerformWithFiller(*aPaveFiller);
  if (aBuilder->HasErrors())
    return;

  TopoDS_Shape aResult = aBuilder->Shape();
  if(aResult.ShapeType() == TopAbs_COMPOUND) {
    aResult = GeomAlgoAPI_DFLoader::refineResult(aResult);
  }
  if(theIsMakeCompSolids && aResult.ShapeType() == TopAbs_COMPOUND) {
    std::shared_ptr<GeomAPI_Shape> aGeomShape(new GeomAPI_Shape);
    aGeomShape->setImpl(new TopoDS_Shape(aResult));
    ListOfShape aResults;
    aGeomShape = GeomAlgoAPI_ShapeTools::combineShapes(aGeomShape,
                                                       GeomAPI_Shape::COMPSOLID,
                                                       aResults);
    aResult = aGeomShape->impl<TopoDS_Shape>();
  }

  std::shared_ptr<GeomAPI_Shape> aShape(new GeomAPI_Shape());
  aShape->setImpl(new TopoDS_Shape(aResult));
  this->setShape(aShape);
  this->setDone(true);
}
