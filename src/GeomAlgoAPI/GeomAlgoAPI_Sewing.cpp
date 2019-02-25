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

void GeomAlgoAPI_Sewing::build(const ListOfShape& theShapes)
{
  if(theShapes.empty()) {
    return;
  }

  BRepBuilderAPI_Sewing* aSewingBuilder = new BRepBuilderAPI_Sewing();
  this->setImpl(aSewingBuilder);

  for(ListOfShape::const_iterator anIt = theShapes.cbegin(); anIt != theShapes.cend(); ++anIt) {
    const TopoDS_Shape& aShape = (*anIt)->impl<TopoDS_Shape>();
    aSewingBuilder->Add(aShape);
  }

  aSewingBuilder->Perform();

  TopoDS_Shape aResult = aSewingBuilder->SewedShape();
  BRep_Builder aBuilder;
  if(aResult.ShapeType() == TopAbs_COMPOUND) {
    TopoDS_Compound aResultCompound;
    aBuilder.MakeCompound(aResultCompound);
    for(TopoDS_Iterator anIt(aResult); anIt.More(); anIt.Next()) {
      const TopoDS_Shape aSubShape = anIt.Value();
      if(aSubShape.ShapeType() == TopAbs_SHELL) {
        aBuilder.Add(aResultCompound, aSubShape);
      } else if (aSubShape.ShapeType() == TopAbs_FACE) {
        TopoDS_Shell aShell;
        aBuilder.MakeShell(aShell);
        aBuilder.Add(aShell, aSubShape);
        aBuilder.Add(aResultCompound, aShell);
      }
    }
    aResult = aResultCompound;
  } else if(aResult.ShapeType() == TopAbs_FACE) {
    TopoDS_Shell aShell;
    aBuilder.MakeShell(aShell);
    aBuilder.Add(aShell, aResult);
    aResult = aShell;
  }

  std::shared_ptr<GeomAPI_Shape> aShape(new GeomAPI_Shape());
  aShape->setImpl(new TopoDS_Shape(aResult));
  this->setShape(aShape);
  this->setDone(true);
}

//==================================================================================================
#include <GeomAPI_ShapeExplorer.h>
#include <GeomAPI_ShapeIterator.h>

typedef std::map<GeomShapePtr, ListOfShape, GeomAPI_Shape::Comparator> MapFaceSolid;
static void facesBelongingToSolids(const GeomShapePtr& theShape,
                                   MapFaceSolid& theShapeRelations)
{
  for (GeomAPI_ShapeExplorer aSolidExp(theShape, GeomAPI_Shape::SHELL);
       aSolidExp.more(); aSolidExp.next()) {
    GeomShapePtr aSolid = aSolidExp.current();
    for (GeomAPI_ShapeExplorer aFaceExp(aSolid, GeomAPI_Shape::FACE);
         aFaceExp.more(); aFaceExp.next())
      theShapeRelations[aFaceExp.current()].push_back(aSolid);
  }
}

static bool isShapeInList(const GeomShapePtr& theShape, const ListOfShape& theList)
{
  for (ListOfShape::const_iterator anIt = theList.begin(); anIt != theList.end(); ++anIt)
    if (theShape->isEqual(*anIt))
      return true;
  return false;
}

void GeomAlgoAPI_Sewing::modified(const std::shared_ptr<GeomAPI_Shape> theShape,
                                  ListOfShape& theHistory)
{
  static int anIndex = 0;
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
