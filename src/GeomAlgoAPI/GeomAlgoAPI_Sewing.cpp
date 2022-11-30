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

#include "GeomAlgoAPI_Sewing.h"

#include <BRep_Builder.hxx>
#include <BRepBuilderAPI_MakeShell.hxx>
#include <BRepBuilderAPI_Sewing.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopoDS_Shape.hxx>

//==================================================================================================
GeomAlgoAPI_Sewing::GeomAlgoAPI_Sewing(const ListOfShape& theShapes)
{
  build(theShapes);
}

GeomAlgoAPI_Sewing::GeomAlgoAPI_Sewing(const ListOfShape& theShapes, const bool theAllowNonManifold, const double theTolerance)
{
  build(theShapes, theAllowNonManifold, theTolerance);
}

void GeomAlgoAPI_Sewing::build(const ListOfShape& theShapes, const bool theAllowNonManifold, const double theTolerance)
{
  if(theShapes.empty()) {
    return;
  }

  BRepBuilderAPI_Sewing* aSewingBuilder = new BRepBuilderAPI_Sewing();
  this->setImpl(aSewingBuilder);

  aSewingBuilder->SetTolerance(theTolerance);
  aSewingBuilder->SetFaceMode(Standard_True);
  aSewingBuilder->SetFloatingEdgesMode(Standard_False);
  aSewingBuilder->SetNonManifoldMode(theAllowNonManifold);

  for(ListOfShape::const_iterator anIt = theShapes.cbegin(); anIt != theShapes.cend(); ++anIt) {
    const TopoDS_Shape& aShape = (*anIt)->impl<TopoDS_Shape>();
    aSewingBuilder->Add(aShape);
  }

  aSewingBuilder->Perform();

  TopoDS_Shape aResult = aSewingBuilder->SewedShape();

  std::shared_ptr<GeomAPI_Shape> aShape(new GeomAPI_Shape());
  aShape->setImpl(new TopoDS_Shape(aResult));
  this->setShape(aShape);
  this->setDone(true);
}

//==================================================================================================
#include <GeomAPI_ShapeIterator.h>

void GeomAlgoAPI_Sewing::modified(const std::shared_ptr<GeomAPI_Shape> theShape,
                                  ListOfShape& theHistory)
{
  if(!theShape.get()) {
    return;
  }

  const TopoDS_Shape& aShape = theShape->impl<TopoDS_Shape>();
  const BRepBuilderAPI_Sewing& aSewingBuilder = this->impl<BRepBuilderAPI_Sewing>();

  TopoDS_Shape aModifiedShape = aSewingBuilder.Modified(aShape);
  if(aModifiedShape.IsEqual(aShape)) {
    aModifiedShape = aSewingBuilder.ModifiedSubShape(aShape);
  }

  for(TopExp_Explorer anExp(aModifiedShape, aShape.ShapeType()); anExp.More(); anExp.Next()) {
    GeomShapePtr aGeomShape(new GeomAPI_Shape());
    aGeomShape->setImpl(new TopoDS_Shape(anExp.Current()));
    theHistory.push_back(aGeomShape);
  }

  if (theShape->shapeType() < GeomAPI_Shape::FACE) {
    ListOfShape aNewShapes;
    // collect faces and parent shapes, if it is not done yet
    if (!isNewShapesCollected(theShape, GeomAPI_Shape::FACE))
      collectNewShapes(theShape, GeomAPI_Shape::FACE);

    for (GeomAPI_ShapeIterator anIt(shape()); anIt.more(); anIt.next()) {
      GeomShapePtr anOldShapesCompound =
          oldShapesForNew(theShape, anIt.current(), GeomAPI_Shape::FACE);
      if (!anOldShapesCompound->isNull())
        aNewShapes.push_back(anIt.current());
    }

    if (!aNewShapes.empty())
      theHistory = aNewShapes;
  }
}
