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
  myListOfMakeShape = theMakeShapeList;
}

//=================================================================================================
const std::shared_ptr<GeomAPI_Shape> GeomAlgoAPI_MakeShapeList::shape() const
{
  if(myListOfMakeShape.empty()) {
    return std::shared_ptr<GeomAPI_Shape>();
  } else {
    return myListOfMakeShape.back()->shape();
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

bool GeomAlgoAPI_MakeShapeList::isDeleted(const std::shared_ptr<GeomAPI_Shape> theShape)
{
  for(ListOfMakeShape::iterator aBuilderIt = myListOfMakeShape.begin(); aBuilderIt != myListOfMakeShape.end(); aBuilderIt++) {
    BRepBuilderAPI_MakeShape* aBuilder = (*aBuilderIt)->implPtr<BRepBuilderAPI_MakeShape>();
    if(aBuilder && (aBuilder->IsDeleted(theShape->impl<TopoDS_Shape>()) == Standard_True)) {
      return true;
    }
  }

  return false;
}

void GeomAlgoAPI_MakeShapeList::result(const std::shared_ptr<GeomAPI_Shape> theShape,
                                       ListOfShape& theHistory,
                                       OperationType theOperationType)
{
  if(myListOfMakeShape.empty()) {
    return;
  }

  NCollection_Map<TopoDS_Shape> anAlgoShapes;
  NCollection_Map<TopoDS_Shape> aResultShapes;
  anAlgoShapes.Add(theShape->impl<TopoDS_Shape>());
  aResultShapes.Add(theShape->impl<TopoDS_Shape>());

  for(ListOfMakeShape::iterator aBuilderIt = myListOfMakeShape.begin(); aBuilderIt != myListOfMakeShape.end(); aBuilderIt++) {
    BRepBuilderAPI_MakeShape* aBuilder = (*aBuilderIt)->implPtr<BRepBuilderAPI_MakeShape>();
    NCollection_Map<TopoDS_Shape> aTempShapes;
    bool hasResults = false;
    for(NCollection_Map<TopoDS_Shape>::Iterator aShapeIt(anAlgoShapes); aShapeIt.More(); aShapeIt.Next()) {
      const TopoDS_Shape& aShape = aShapeIt.Value();
      const TopTools_ListOfShape& aGeneratedList = aBuilder->Generated(aShape);
      for(TopTools_ListIteratorOfListOfShape anIt(aGeneratedList); anIt.More(); anIt.Next()) {
        aTempShapes.Add(anIt.Value());
        aResultShapes.Add(anIt.Value());
        hasResults = true;
      }
      const TopTools_ListOfShape& aModifiedList = aBuilder->Modified(aShape);
      for(TopTools_ListIteratorOfListOfShape anIt(aModifiedList); anIt.More(); anIt.Next()) {
        aTempShapes.Add(anIt.Value());
        aResultShapes.Add(anIt.Value());
        hasResults = true;
      }
      if(hasResults) {
        aResultShapes.Remove(aShape);
      }
    }
    anAlgoShapes.Unite(aTempShapes);
  }

  for(NCollection_Map<TopoDS_Shape>::Iterator aShapeIt(aResultShapes); aShapeIt.More(); aShapeIt.Next()) {
    std::shared_ptr<GeomAPI_Shape> aShape(new GeomAPI_Shape());
    aShape->setImpl(new TopoDS_Shape(aShapeIt.Value()));
    theHistory.push_back(aShape);
  }
}

