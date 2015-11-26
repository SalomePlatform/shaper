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
void GeomAlgoAPI_MakeShape::initialize() {
  switch (myBuilderType) {
    case OCCT_BRepBuilderAPI_MakeShape: {
      myDone = implPtr<BRepBuilderAPI_MakeShape>()->IsDone() == Standard_True;
      myShape->setImpl(new TopoDS_Shape(implPtr<BRepBuilderAPI_MakeShape>()->Shape()));
      break;
    }
    case OCCT_BOPAlgo_Builder: {
      myDone = true;
      myShape->setImpl(new TopoDS_Shape(implPtr<BOPAlgo_Builder>()->Shape()));
      break;
    }
  }
}

//=================================================================================================
bool GeomAlgoAPI_MakeShape::isDone() const
{
  return myDone;
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
  if(myBuilderType == OCCT_BRepBuilderAPI_MakeShape) {
    BRepBuilderAPI_MakeShape* aMakeShape = implPtr<BRepBuilderAPI_MakeShape>();
    aList = aMakeShape->Generated(theShape->impl<TopoDS_Shape>());
  } else if(myBuilderType == OCCT_BOPAlgo_Builder) {
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
  if(myBuilderType == OCCT_BRepBuilderAPI_MakeShape) {
    BRepBuilderAPI_MakeShape* aMakeShape = implPtr<BRepBuilderAPI_MakeShape>();
    aList = aMakeShape->Modified(theShape->impl<TopoDS_Shape>());
  } else if(myBuilderType == OCCT_BOPAlgo_Builder) {
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
  if(myBuilderType == OCCT_BRepBuilderAPI_MakeShape) {
    BRepBuilderAPI_MakeShape* aMakeShape = implPtr<BRepBuilderAPI_MakeShape>();
    isDeleted = aMakeShape->IsDeleted(theShape->impl<TopoDS_Shape>()) == Standard_True;
  } else if(myBuilderType == OCCT_BOPAlgo_Builder) {
    BOPAlgo_Builder* aBOPBuilder = implPtr<BOPAlgo_Builder>();
    isDeleted = aBOPBuilder->IsDeleted(theShape->impl<TopoDS_Shape>()) == Standard_True;
  }

  return isDeleted;
}


//=================================================================================================
GeomAlgoAPI_MakeShape::GeomAlgoAPI_MakeShape()
: myBuilderType(OCCT_BRepBuilderAPI_MakeShape),
  myDone(false),
  myShape(new GeomAPI_Shape())
{
}

//=================================================================================================
void GeomAlgoAPI_MakeShape::setBuilderType(const BuilderType theBuilderType)
{
  myBuilderType = theBuilderType;
}

//=================================================================================================
void GeomAlgoAPI_MakeShape::setDone(const bool theFlag)
{
  myDone = theFlag;
}

//=================================================================================================
void GeomAlgoAPI_MakeShape::setShape(const std::shared_ptr<GeomAPI_Shape> theShape)
{
  myShape = theShape;
}
