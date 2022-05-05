// Copyright (C) 2014-2022  CEA/DEN, EDF R&D
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

#include "GeomAlgoAPI_UnifySameDomain.h"

#include <GeomAlgoAPI_CompoundBuilder.h>
#include <GeomAlgoAPI_DFLoader.h>
#include <GeomAlgoAPI_ShapeTools.h>

#include <BRep_Tool.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS_Shape.hxx>
#include <Precision.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <Bnd_Box.hxx>
#include <BRepBndLib.hxx>
#include <ShapeUpgrade_UnifySameDomain.hxx>

//==================================================================================================
GeomAlgoAPI_UnifySameDomain::GeomAlgoAPI_UnifySameDomain(const ListOfShape& theShapes)
{
  build(theShapes);
}

//==================================================================================================
GeomAlgoAPI_UnifySameDomain::GeomAlgoAPI_UnifySameDomain(const GeomShapePtr& theShape)
{
  build(theShape, false);
}

//==================================================================================================
void GeomAlgoAPI_UnifySameDomain::build(const ListOfShape& theShapes)
{
  if(theShapes.empty()) {
    return;
  }

  // Make compound.
  GeomShapePtr aCompound = GeomAlgoAPI_CompoundBuilder::compound(theShapes);
  ListOfShape aResults;
  GeomAlgoAPI_ShapeTools::combineShapes(
    aCompound,
    GeomAPI_Shape::SHELL,
    aResults);

  if(aResults.size() > 1 ||
     (aResults.size() == 1 && aResults.front()->shapeType() > GeomAPI_Shape::SHELL)) {
    return;
  }

  const TopoDS_Shape& aShell = aResults.front()->impl<TopoDS_Shape>();

  std::shared_ptr<GeomAPI_Shape> aShape(new GeomAPI_Shape());
  aShape->setImpl(new TopoDS_Shape(aShell));
  build(aShape, true);
}

// calculates maximum possible tolerance on edges of shape
static Standard_Real defineLinearTolerance(const TopoDS_Shape& theShape)
{
  Standard_Real aMaxTol = Precision::Confusion();

  TopExp_Explorer Explo(theShape, TopAbs_EDGE);
  for (; Explo.More(); Explo.Next())
  {
    const TopoDS_Edge& anEdge = TopoDS::Edge(Explo.Current());
    aMaxTol = Max(aMaxTol, BRep_Tool::Tolerance(anEdge));
  }

  return aMaxTol;
}

//==================================================================================================
void GeomAlgoAPI_UnifySameDomain::build(const GeomShapePtr& theShape,
                                        const bool theIsToSimplifyShell)
{
  ShapeUpgrade_UnifySameDomain* aUnifyAlgo = new ShapeUpgrade_UnifySameDomain();
  this->setImpl(aUnifyAlgo);

  const TopoDS_Shape& aShape = theShape->impl<TopoDS_Shape>();
  aUnifyAlgo->Initialize(aShape);
  aUnifyAlgo->SetLinearTolerance(defineLinearTolerance(aShape));
  aUnifyAlgo->SetAngularTolerance(1.e-6); // for #2697
  aUnifyAlgo->Build();

  TopoDS_Shape aResult = aUnifyAlgo->Shape();
  if (aResult.IsNull()) {
    return;
  }
  // take off the compound if it consists of single sub-shape
  if (aResult.ShapeType() == TopAbs_COMPOUND) {
    aResult = GeomAlgoAPI_DFLoader::refineResult(aResult);
  }

  if (theIsToSimplifyShell && aResult.ShapeType() == TopAbs_SHELL) {
    int aNb = 0;
    TopoDS_Iterator anIt(aResult);
    for (; anIt.More(); anIt.Next()) {
      ++aNb;
    }

    if (aNb == 1) {
      anIt.Initialize(aResult);
      aResult = anIt.Value();
    }
  }

  std::shared_ptr<GeomAPI_Shape> aResShape(new GeomAPI_Shape());
  aResShape->setImpl(new TopoDS_Shape(aResult));
  this->setShape(aResShape);
  this->setDone(true);
}

//==================================================================================================
void GeomAlgoAPI_UnifySameDomain::modified(const std::shared_ptr<GeomAPI_Shape> theShape,
                                  ListOfShape& theHistory)
{
  if(!theShape.get()) {
    return;
  }

  const TopoDS_Shape& aShape = theShape->impl<TopoDS_Shape>();
  const ShapeUpgrade_UnifySameDomain& aUnifyAlgo = this->impl<ShapeUpgrade_UnifySameDomain>();

  for (int aIsModified = 0; aIsModified <= 1; aIsModified++) {
    if (!aUnifyAlgo.History()->IsSupportedType(aShape)) // to avoid crash in BRepTools_History
      continue;
    const TopTools_ListOfShape& aMList = aIsModified ?
      aUnifyAlgo.History()->Modified(aShape) : aUnifyAlgo.History()->Generated(aShape);
    for (TopTools_ListIteratorOfListOfShape aModified(aMList); aModified.More(); aModified.Next()) {
      for (TopExp_Explorer anExp(aModified.Value(), aShape.ShapeType());
           anExp.More(); anExp.Next()) {
        GeomShapePtr aGeomShape(new GeomAPI_Shape());
        aGeomShape->setImpl(new TopoDS_Shape(anExp.Current()));
        theHistory.push_back(aGeomShape);
      }
    }
  }
}
