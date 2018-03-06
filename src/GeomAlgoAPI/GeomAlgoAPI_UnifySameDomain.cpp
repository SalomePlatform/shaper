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

#include "GeomAlgoAPI_UnifySameDomain.h"

#include <GeomAlgoAPI_CompoundBuilder.h>
#include <GeomAlgoAPI_ShapeTools.h>

#include <ShapeUpgrade_UnifySameDomain.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS_Shape.hxx>

//==================================================================================================
GeomAlgoAPI_UnifySameDomain::GeomAlgoAPI_UnifySameDomain(const ListOfShape& theShapes)
{
  build(theShapes);
}

void GeomAlgoAPI_UnifySameDomain::build(const ListOfShape& theShapes)
{
  if(theShapes.empty()) {
    return;
  }

  // Make compound.
  GeomShapePtr aCompound = GeomAlgoAPI_CompoundBuilder::compound(theShapes);
  ListOfShape aCombined, aFree;
  GeomAlgoAPI_ShapeTools::combineShapes(
    aCompound,
    GeomAPI_Shape::SHELL,
    aCombined,
    aFree);

  if(aFree.size() > 0 || aCombined.size() > 1) {
    return;
  }

  const TopoDS_Shape& aShell = aCombined.front()->impl<TopoDS_Shape>();

  ShapeUpgrade_UnifySameDomain* aUnifyAlgo = new ShapeUpgrade_UnifySameDomain();
  this->setImpl(aUnifyAlgo);

  aUnifyAlgo->Initialize(aShell);
  aUnifyAlgo->UnifyFacesAndEdges();
  aUnifyAlgo->Build();

  TopoDS_Shape aResult = aUnifyAlgo->Shape();
  if (aResult.IsNull()) {
    return;
  }

  if (aResult.ShapeType() == TopAbs_SHELL) {
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

  std::shared_ptr<GeomAPI_Shape> aShape(new GeomAPI_Shape());
  aShape->setImpl(new TopoDS_Shape(aResult));
  this->setShape(aShape);
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

  const TopTools_ListOfShape& aMList = aUnifyAlgo.History()->Generated(aShape);
  for(TopTools_ListIteratorOfListOfShape aModified(aMList); aModified.More(); aModified.Next()) {

    for(TopExp_Explorer anExp(aModified.Value(), aShape.ShapeType()); anExp.More(); anExp.Next()) {
      GeomShapePtr aGeomShape(new GeomAPI_Shape());
      aGeomShape->setImpl(new TopoDS_Shape(anExp.Current()));
      theHistory.push_back(aGeomShape);
    }
  }
}
