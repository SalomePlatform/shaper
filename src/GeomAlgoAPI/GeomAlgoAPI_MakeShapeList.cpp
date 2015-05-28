// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        GeomAlgoAPI_MakeShapeListList.h
// Created:     27 May 2015
// Author:      Dmitry Bobylev

#include <GeomAlgoAPI_MakeShapeList.h>

#include <BRepBuilderAPI_MakeShape.hxx>
#include <NCollection_Map.hxx>
#include <TopTools_ListOfShape.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>

//=================================================================================================
GeomAlgoAPI_MakeShapeList::GeomAlgoAPI_MakeShapeList()
: GeomAlgoAPI_MakeShape()
{}

//=================================================================================================
GeomAlgoAPI_MakeShapeList::GeomAlgoAPI_MakeShapeList(const ListOfMakeShape& theMakeShapeList)
: GeomAlgoAPI_MakeShape()
{
  init(theMakeShapeList);
}

//=================================================================================================
void GeomAlgoAPI_MakeShapeList::init(const ListOfMakeShape& theMakeShapeList)
{
  myMakeShapeList = theMakeShapeList;
}

//=================================================================================================
const std::shared_ptr<GeomAPI_Shape> GeomAlgoAPI_MakeShapeList::shape() const
{
  if(myMakeShapeList.empty()) {
    return std::shared_ptr<GeomAPI_Shape>();
  } else {
    return myMakeShapeList.back()->shape();
  }
}

//=================================================================================================
void GeomAlgoAPI_MakeShapeList::generated(const std::shared_ptr<GeomAPI_Shape> theShape,
                                          ListOfShape& theHistory)
{
  result(theShape, theHistory, GeomAlgoAPI_MakeShapeList::Generated);
}

//=================================================================================================
void GeomAlgoAPI_MakeShapeList::modified(const std::shared_ptr<GeomAPI_Shape> theShape,
                                         ListOfShape& theHistory)
{
  result(theShape, theHistory, GeomAlgoAPI_MakeShapeList::Modified);
}

void GeomAlgoAPI_MakeShapeList::result(const std::shared_ptr<GeomAPI_Shape> theShape,
                                       ListOfShape& theHistory,
                                       OperationType theOperationType)
{
  if(myMakeShapeList.empty()) {
    return;
  }

  NCollection_Map<TopoDS_Shape> aTempShapes;
  NCollection_Map<TopoDS_Shape> aResultShapes;
  aTempShapes.Add(theShape->impl<TopoDS_Shape>());

  for(ListOfMakeShape::iterator aBuilderIt = myMakeShapeList.begin(); aBuilderIt != myMakeShapeList.end(); aBuilderIt++) {
    BRepBuilderAPI_MakeShape* aBuilder = (*aBuilderIt)->implPtr<BRepBuilderAPI_MakeShape>();
    for(NCollection_Map<TopoDS_Shape>::Iterator aShapeIt(aTempShapes); aShapeIt.More(); aShapeIt.Next()) {
      const TopoDS_Shape& aShape = aShapeIt.Value();
      const TopTools_ListOfShape& aList = theOperationType == GeomAlgoAPI_MakeShapeList::Generated ?
                                          aBuilder->Generated(aShape) : aBuilder->Modified(aShape);
      bool prevResRemoved = false;
      for(TopTools_ListIteratorOfListOfShape anIt(aList); anIt.More(); anIt.Next()) {
        aTempShapes.Add(anIt.Value());
        aResultShapes.Add(anIt.Value());
        if(!prevResRemoved) {
          aResultShapes.Remove(aShape);
          prevResRemoved = true;
        }
      }
    }
  }

  for(NCollection_Map<TopoDS_Shape>::Iterator aShapeIt(aResultShapes); aShapeIt.More(); aShapeIt.Next()) {
    std::shared_ptr<GeomAPI_Shape> aShape(new GeomAPI_Shape());
    aShape->setImpl(new TopoDS_Shape(aShapeIt.Value()));
    theHistory.push_back(aShape);
  }
}

