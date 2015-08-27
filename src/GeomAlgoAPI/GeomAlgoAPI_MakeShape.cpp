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
    case MakeShape:
    case MakePipe: {
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
GeomAlgoAPI_MakeShape::GeomAlgoAPI_MakeShape(void* theMkShape,
                                             const std::shared_ptr<GeomAPI_Shape> theWire,
                                             const std::shared_ptr<GeomAPI_Shape> theBaseShape)
: GeomAPI_Interface(theMkShape),
  myAlgoType(MakePipe),
  myShape(new GeomAPI_Shape()),
  myWire(theWire),
  myBaseShape(theBaseShape)
{
  myShape->setImpl(new TopoDS_Shape(implPtr<BRepOffsetAPI_MakePipe>()->Shape()));
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
  if(myAlgoType == MakePipe) {
    BRepOffsetAPI_MakePipe* aMakePipe = implPtr<BRepOffsetAPI_MakePipe>();
    TopExp_Explorer aShapeExplorer(myWire->impl<TopoDS_Wire>(), TopAbs_EDGE);
    for (; aShapeExplorer.More(); aShapeExplorer.Next ()) {
      const TopoDS_Shape& aSpine = aShapeExplorer.Current();
      const TopoDS_Shape& aProfile = theShape->impl<TopoDS_Shape>();
      if(aProfile.ShapeType() != TopAbs_EDGE && aProfile.ShapeType() != TopAbs_VERTEX) {
          return;
      }
      const TopoDS_Shape& aBaseShape = myBaseShape->impl<TopoDS_Shape>();
      TopExp_Explorer anExp(aBaseShape, aProfile.ShapeType());
      Standard_Boolean hasShape = Standard_False;
      for(; anExp.More(); anExp.Next()) {
        if(anExp.Current().IsSame(aProfile)) {
          hasShape = Standard_True;
          break;
        }
      }
      if(!hasShape) {
        return;
      }
      const TopoDS_Shape& aGeneratedShape = aMakePipe->Generated(aSpine, aProfile);
      std::shared_ptr<GeomAPI_Shape> aShape(new GeomAPI_Shape());
      aShape->setImpl(new TopoDS_Shape(aGeneratedShape));
      theHistory.push_back(aShape);
    }
  } else {
    TopTools_ListOfShape aList;
    if(myAlgoType == MakeShape) {
      BRepBuilderAPI_MakeShape* aMakeShape = implPtr<BRepBuilderAPI_MakeShape>();
      aList = aMakeShape->Generated(theShape->impl<TopoDS_Shape>());
    } else if(myAlgoType == BOPAlgoBuilder) {
      BOPAlgo_Builder* aBOPBuilder = implPtr<BOPAlgo_Builder>();
      aList = aBOPBuilder->Generated(theShape->impl<TopoDS_Shape>());
    }
    for(TopTools_ListIteratorOfListOfShape anIt(aList); anIt.More(); anIt.Next()) {
      std::shared_ptr<GeomAPI_Shape> aShape(new GeomAPI_Shape());
      aShape->setImpl(new TopoDS_Shape(anIt.Value()));
      theHistory.push_back(aShape);
    }
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
