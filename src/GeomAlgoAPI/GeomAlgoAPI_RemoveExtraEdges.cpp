// Copyright (C) 2014-2026  CEA, EDF
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

#include "GeomAlgoAPI_RemoveExtraEdges.h"

#include "GEOMAlgo_GetInPlace.hxx"

#include <TopoDS_Iterator.hxx>
#include <TopoDS_Shape.hxx>

#include <BlockFix_BlockFixAPI.hxx>

//==================================================================================================
GeomAlgoAPI_RemoveExtraEdges::GeomAlgoAPI_RemoveExtraEdges(const GeomShapePtr& theShape,
                                                           const bool theDoUnionFaces)
{
  build(theShape,theDoUnionFaces );
}

//==================================================================================================
void GeomAlgoAPI_RemoveExtraEdges::build(const GeomShapePtr& theShape,
                                         const bool theDoUnionFaces)
{
  BlockFix_BlockFixAPI* aUnifyAlgo = new BlockFix_BlockFixAPI();
  //this->setImpl(aUnifyAlgo);

  const TopoDS_Shape& aShape = theShape->impl<TopoDS_Shape>();
  aUnifyAlgo->OptimumNbFaces() = theDoUnionFaces ? 0 : -1;
  aUnifyAlgo->SetShape(aShape);
  aUnifyAlgo->Perform();

  TopoDS_Shape aResult = aUnifyAlgo->Shape();
  if (aResult.IsNull()) {
    return;
  }

  // store modified shapes
  TopTools_MapOfShape aMapOfUnique;
  addModifiedRecursively(aShape, aResult, aMapOfUnique);

  std::shared_ptr<GeomAPI_Shape> aResShape(new GeomAPI_Shape());
  aResShape->setImpl(new TopoDS_Shape(aResult));
  this->setShape(aResShape);
  this->setDone(true);
}

//==================================================================================================
void GeomAlgoAPI_RemoveExtraEdges::addModifiedRecursively(const TopoDS_Shape& theShape,
                                                          const TopoDS_Shape& theResult,
                                                          TopTools_MapOfShape& theMapOfUnique)
{
  if (theMapOfUnique.Contains(theResult)) return;

  // Store modified shapes (SOLIDS, FACES, EDGES and VERTICES)
  if (theResult.ShapeType() == TopAbs_VERTEX ||
      theResult.ShapeType() == TopAbs_EDGE ||
      theResult.ShapeType() == TopAbs_FACE ||
      theResult.ShapeType() == TopAbs_SOLID) {
    // Here we search in the initial shape for the new shape, because in case of united faces/edges
    // GetInPlace will not find bigger new shape in place of one of its parts, but OK vice versa
    GEOMAlgo_GetInPlace aGIP (theShape, theResult);
    if (!aGIP.ErrorStatus()) {
      TopoDS_Shape aModifiedFrom = aGIP.Result();
      if (!aModifiedFrom.IsNull()) {
        if (aModifiedFrom.ShapeType() == TopAbs_COMPOUND ||
            aModifiedFrom.ShapeType() == TopAbs_COMPSOLID) {
          // COMPOUND or COMPSOLID, new shape was made from several old
          TopoDS_Iterator itComp (aModifiedFrom);
          for (; itComp.More(); itComp.Next()) {
            addModifiedSub(itComp.Value(), theResult, theMapOfUnique);
          }
        }
        else if (aModifiedFrom.ShapeType() == theResult.ShapeType()) {
          addModifiedSub(aModifiedFrom, theResult, theMapOfUnique);
        }
      }
    }
  }
  else {
    // Store modified sub-shapes of containers
    TopoDS_Iterator itRes (theResult);
    for (; itRes.More(); itRes.Next()) {
      addModifiedRecursively(theShape, itRes.Value(), theMapOfUnique);
    }
  }

  theMapOfUnique.Add(theResult);
}

void GeomAlgoAPI_RemoveExtraEdges::addModifiedSub(const TopoDS_Shape& theFrom,
                                                  const TopoDS_Shape& theTo,
                                                  TopTools_MapOfShape& theMapOfUnique)
{
  GeomShapePtr aBaseShape (new GeomAPI_Shape());
  aBaseShape->setImpl(new TopoDS_Shape(theFrom));

  GeomShapePtr aNewShape (new GeomAPI_Shape());
  aNewShape->setImpl(new TopoDS_Shape(theTo));

  addModified(aBaseShape, aNewShape);

  // store sub-shapes
  if (theTo.ShapeType() != TopAbs_VERTEX) {
    for (TopoDS_Iterator itTo (theTo); itTo.More(); itTo.Next()) {
      addModifiedRecursively(theFrom, itTo.Value(), theMapOfUnique);
    }
  }
}
