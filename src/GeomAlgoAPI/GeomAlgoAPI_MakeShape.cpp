// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        GeomAlgoAPI_MakeShape.cpp
// Created:     20 Oct 2014
// Author:      Sergey ZARITCHNY

#include <GeomAlgoAPI_MakeShape.h>

#include <BOPAlgo_Builder.hxx>
#include <BRepBuilderAPI_MakeShape.hxx>
#include <BRepOffsetAPI_MakePipe.hxx>
#include <TopExp_Explorer.hxx>
#include <TopTools_ListOfShape.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>

//=================================================================================================
GeomAlgoAPI_MakeShape::GeomAlgoAPI_MakeShape(void* theMkShape, const AlgoType theAlgoType)
: GeomAPI_Interface(theMkShape),
  myAlgoType(theAlgoType),
  myShape(new GeomAPI_Shape())
{
  switch (myAlgoType) {
    case MakeShape: {
      myShape->setImpl(new TopoDS_Shape(implPtr<BRepBuilderAPI_MakeShape>()->Shape()));
      break;
    }
    case BOPAlgoBuilder: {
      myShape->setImpl(new TopoDS_Shape(implPtr<BOPAlgo_Builder>()->Shape()));
      break;
    }
  }
}

//=================================================================================================
const std::shared_ptr<GeomAPI_Shape> GeomAlgoAPI_MakeShape::shape() const
{
  return myShape;
}

//=================================================================================================
void GeomAlgoAPI_MakeShape::generated(const std::shared_ptr<GeomAPI_Shape> theShape,
                                      ListOfShape& theHistory)
{
  TopTools_ListOfShape aList;
  if(myAlgoType == MakeShape) {
    BRepBuilderAPI_MakeShape* aMakeShape = implPtr<BRepBuilderAPI_MakeShape>();
    aList = aMakeShape->Generated(theShape->impl<TopoDS_Shape>());
  } else if(myAlgoType == BOPAlgoBuilder) {
    BOPAlgo_Builder* aBOPBuilder = implPtr<BOPAlgo_Builder>();
    aList = aBOPBuilder->Generated(theShape->impl<TopoDS_Shape>());
  }
  for(TopTools_ListIteratorOfListOfShape anIt(aList); anIt.More(); anIt.Next()) {
    if(anIt.Value().IsNull()) {
      continue;
    }
    std::shared_ptr<GeomAPI_Shape> aShape(new GeomAPI_Shape());
    aShape->setImpl(new TopoDS_Shape(anIt.Value()));
    theHistory.push_back(aShape);
  }
}

//=================================================================================================
void GeomAlgoAPI_MakeShape::modified(const std::shared_ptr<GeomAPI_Shape> theShape,
                                     ListOfShape& theHistory)
{
  TopTools_ListOfShape aList;
  if(myAlgoType == MakeShape) {
    BRepBuilderAPI_MakeShape* aMakeShape = implPtr<BRepBuilderAPI_MakeShape>();
    aList = aMakeShape->Modified(theShape->impl<TopoDS_Shape>());
  } else if(myAlgoType == BOPAlgoBuilder) {
    BOPAlgo_Builder* aBOPBuilder = implPtr<BOPAlgo_Builder>();
    aList = aBOPBuilder->Modified(theShape->impl<TopoDS_Shape>());
  }
  for(TopTools_ListIteratorOfListOfShape anIt(aList); anIt.More(); anIt.Next()) {
    if(anIt.Value().IsNull()) {
      continue;
    }
    std::shared_ptr<GeomAPI_Shape> aShape(new GeomAPI_Shape());
    aShape->setImpl(new TopoDS_Shape(anIt.Value()));
    theHistory.push_back(aShape);
  }
}

//=================================================================================================
bool GeomAlgoAPI_MakeShape::isDeleted(const std::shared_ptr<GeomAPI_Shape> theShape)
{
  bool isDeleted = false;
  if(myAlgoType == MakeShape) {
    BRepBuilderAPI_MakeShape* aMakeShape = implPtr<BRepBuilderAPI_MakeShape>();
    isDeleted = aMakeShape->IsDeleted(theShape->impl<TopoDS_Shape>()) == Standard_True;
  } else if(myAlgoType == BOPAlgoBuilder) {
    BOPAlgo_Builder* aBOPBuilder = implPtr<BOPAlgo_Builder>();
    isDeleted = aBOPBuilder->IsDeleted(theShape->impl<TopoDS_Shape>()) == Standard_True;
  }

  return isDeleted;
}
