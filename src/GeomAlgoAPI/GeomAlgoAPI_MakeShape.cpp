// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        GeomAlgoAPI_MakeShape.cpp
// Created:     20 Oct 2014
// Author:      Sergey ZARITCHNY

#include <GeomAlgoAPI_MakeShape.h>
#include <BRepBuilderAPI_MakeShape.hxx>
#include <TopTools_ListOfShape.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>
GeomAlgoAPI_MakeShape::GeomAlgoAPI_MakeShape(void* theMkShape)
  : GeomAPI_Interface(theMkShape),myShape(new GeomAPI_Shape())
{
  myShape->setImpl((void *)&implPtr<BRepBuilderAPI_MakeShape>()->Shape());
}

GeomAlgoAPI_MakeShape::GeomAlgoAPI_MakeShape()
  : GeomAPI_Interface(),myShape(new GeomAPI_Shape())
{}
void GeomAlgoAPI_MakeShape::init(void* theMkShape)
{
  setImpl((void *)implPtr<BRepBuilderAPI_MakeShape>());
}

const std::shared_ptr<GeomAPI_Shape> GeomAlgoAPI_MakeShape::shape() const
{
  return myShape;
}

void GeomAlgoAPI_MakeShape::generated(
  const std::shared_ptr<GeomAPI_Shape> theShape, ListOfShape& theHistory)
{
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
