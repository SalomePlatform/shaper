// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        GeomAlgoAPI_MakeShape.cpp
// Created:     20 Oct 2014
// Author:      Sergey ZARITCHNY

#include <GeomAlgoAPI_MakeShape.h>
#include <BRepBuilderAPI_MakeShape.hxx>
#include <BRepOffsetAPI_MakePipe.hxx>
#include <TopExp_Explorer.hxx>
#include <TopTools_ListOfShape.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>

GeomAlgoAPI_MakeShape::GeomAlgoAPI_MakeShape(void* theMkShape)
  : GeomAPI_Interface(theMkShape),myShape(new GeomAPI_Shape())
{
  myShape->setImpl(new TopoDS_Shape(implPtr<BRepBuilderAPI_MakeShape>()->Shape()));
}

GeomAlgoAPI_MakeShape::GeomAlgoAPI_MakeShape(void* theMkShape, const std::shared_ptr<GeomAPI_Shape> theWire)
: GeomAPI_Interface(theMkShape),
  myShape(new GeomAPI_Shape()),
  myWire(theWire)
{
  myShape->setImpl(new TopoDS_Shape(implPtr<BRepBuilderAPI_MakeShape>()->Shape()));
}

GeomAlgoAPI_MakeShape::GeomAlgoAPI_MakeShape()
  : GeomAPI_Interface(),myShape(new GeomAPI_Shape())
{}

const std::shared_ptr<GeomAPI_Shape> GeomAlgoAPI_MakeShape::shape() const
{
  return myShape;
}

void GeomAlgoAPI_MakeShape::generated(
  const std::shared_ptr<GeomAPI_Shape> theShape, ListOfShape& theHistory)
{
  if(!myWire.get()) {
    BRepBuilderAPI_MakeShape* aBuilder = implPtr<BRepBuilderAPI_MakeShape>();
    if(aBuilder) {
      const TopTools_ListOfShape& aList =  aBuilder->Generated(theShape->impl<TopoDS_Shape>());
      TopTools_ListIteratorOfListOfShape it(aList);
      for(;it.More();it.Next()) {
        std::shared_ptr<GeomAPI_Shape> aShape(new GeomAPI_Shape());
        aShape->setImpl(new TopoDS_Shape(it.Value()));
        theHistory.push_back(aShape);
      }
    }
  } else { // Pipe builder
    BRepOffsetAPI_MakePipe* aPipeBuilder = implPtr<BRepOffsetAPI_MakePipe>();
    if(aPipeBuilder) {
      TopExp_Explorer aShapeExplorer(myWire->impl<TopoDS_Wire>(), TopAbs_EDGE);
      for (; aShapeExplorer.More(); aShapeExplorer.Next ()) {
        const TopoDS_Shape& aGeneratedShape = aPipeBuilder->Generated(aShapeExplorer.Current(), theShape->impl<TopoDS_Shape>());
        std::shared_ptr<GeomAPI_Shape> aShape(new GeomAPI_Shape());
        aShape->setImpl(new TopoDS_Shape(aGeneratedShape));
        theHistory.push_back(aShape);
      }
    }
  }
}

void GeomAlgoAPI_MakeShape::modified(
  const std::shared_ptr<GeomAPI_Shape> theShape, ListOfShape& theHistory)
{
  BRepBuilderAPI_MakeShape* aBuilder = implPtr<BRepBuilderAPI_MakeShape>();
  if(aBuilder) {
    const TopTools_ListOfShape& aList =  aBuilder->Modified(theShape->impl<TopoDS_Shape>());
    TopTools_ListIteratorOfListOfShape it(aList);
    for(;it.More();it.Next()) {
      std::shared_ptr<GeomAPI_Shape> aShape(new GeomAPI_Shape());
      aShape->setImpl(new TopoDS_Shape(it.Value()));
      theHistory.push_back(aShape);
    }
  }
}

bool GeomAlgoAPI_MakeShape::isDeleted(const std::shared_ptr<GeomAPI_Shape> theShape)
{
  bool isDeleted(false);
  BRepBuilderAPI_MakeShape* aBuilder = implPtr<BRepBuilderAPI_MakeShape>();
  if(aBuilder) {
    isDeleted = aBuilder->IsDeleted(theShape->impl<TopoDS_Shape>()) == Standard_True;
  }
  return isDeleted;
}
