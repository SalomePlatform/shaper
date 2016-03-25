// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        GeomAlgoAPI_MakeShape.cpp
// Created:     20 Oct 2014
// Author:      Sergey ZARITCHNY

#include "GeomAlgoAPI_MakeShape.h"

#include <BOPAlgo_Builder.hxx>
#include <BRepBuilderAPI_MakeShape.hxx>
#include <BRepCheck_Analyzer.hxx>
#include <BRepGProp.hxx>
#include <GProp_GProps.hxx>
#include <Precision.hxx>
#include <TopExp_Explorer.hxx>
#include <TopTools_ListOfShape.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>

//=================================================================================================
GeomAlgoAPI_MakeShape::GeomAlgoAPI_MakeShape()
: myBuilderType(Unknown),
  myDone(false)
{
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
bool GeomAlgoAPI_MakeShape::isValid() const
{
  BRepCheck_Analyzer aChecker(myShape->impl<TopoDS_Shape>());
  return (aChecker.IsValid() == Standard_True);
}

//=================================================================================================
bool GeomAlgoAPI_MakeShape::hasVolume() const
{
  bool hasVolume = false;
  if(isValid()) {
    const TopoDS_Shape& aRShape = myShape->impl<TopoDS_Shape>();
    GProp_GProps aGProp;
    BRepGProp::VolumeProperties(aRShape, aGProp);
    if(aGProp.Mass() > Precision::Confusion())
      hasVolume = true;
  }
  return hasVolume;
}

//=================================================================================================
std::shared_ptr<GeomAPI_DataMapOfShapeShape> GeomAlgoAPI_MakeShape::mapOfSubShapes() const
{
  return myMap;
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
  if(myShape.get() && myShape->isEqual(theShape)) {
    return;
  }

  myShape = theShape;

  // Filling data map to keep correct orientation of sub-shapes.
  if(myShape.get()) {
    if(myMap.get()) {
      myMap->clear();
    } else {
      myMap.reset(new GeomAPI_DataMapOfShapeShape);
    }

    const TopoDS_Shape& aTopoDSShape = myShape->impl<TopoDS_Shape>();
    for(TopExp_Explorer anExp(aTopoDSShape,TopAbs_FACE); anExp.More(); anExp.Next()) {
      std::shared_ptr<GeomAPI_Shape> aCurrentShape(new GeomAPI_Shape());
      aCurrentShape->setImpl(new TopoDS_Shape(anExp.Current()));
      myMap->bind(aCurrentShape, aCurrentShape);
    }
  } else {
    if(myMap.get()) {
      myMap->clear();
    }
  }
}

//=================================================================================================
void GeomAlgoAPI_MakeShape::initialize() {
  switch (myBuilderType) {
    case OCCT_BRepBuilderAPI_MakeShape: {
      myDone = implPtr<BRepBuilderAPI_MakeShape>()->IsDone() == Standard_True;
      myShape.reset(new GeomAPI_Shape());
      myShape->setImpl(new TopoDS_Shape(implPtr<BRepBuilderAPI_MakeShape>()->Shape()));
      break;
    }
    case OCCT_BOPAlgo_Builder: {
      myDone = true;
      myShape.reset(new GeomAPI_Shape());
      myShape->setImpl(new TopoDS_Shape(implPtr<BOPAlgo_Builder>()->Shape()));
      break;
    }
  }

  if(myMap.get()) {
    myMap->clear();
  } else {
    myMap.reset(new GeomAPI_DataMapOfShapeShape);
  }

  const TopoDS_Shape& aTopoDSShape = myShape->impl<TopoDS_Shape>();
  for(TopExp_Explorer anExp(aTopoDSShape,TopAbs_FACE); anExp.More(); anExp.Next()) {
    std::shared_ptr<GeomAPI_Shape> aCurrentShape(new GeomAPI_Shape());
    aCurrentShape->setImpl(new TopoDS_Shape(anExp.Current()));
    myMap->bind(aCurrentShape, aCurrentShape);
  }
}
