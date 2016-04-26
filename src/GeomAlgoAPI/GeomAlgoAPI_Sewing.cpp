// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        GeomAlgoAPI_Sewing.cpp
// Created:     25 April 2016
// Author:      Dmitry Bobylev


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
  TopoDS_Iterator anIt(aResultCompound);
  if(anIt.More()) {
    aResult = aResultCompound;
  }

  std::shared_ptr<GeomAPI_Shape> aShape(new GeomAPI_Shape());
  aShape->setImpl(new TopoDS_Shape(aResult));
  this->setShape(aShape);
  this->setDone(true);
}

//==================================================================================================
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
}
