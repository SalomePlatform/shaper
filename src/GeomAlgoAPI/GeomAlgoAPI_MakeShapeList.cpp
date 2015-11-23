// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        GeomAlgoAPI_MakeShapeListList.cpp
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
void GeomAlgoAPI_MakeShapeList::append(const std::shared_ptr<GeomAlgoAPI_MakeShape> theMakeShape)
{
  myListOfMakeShape.push_back(theMakeShape);
}

//=================================================================================================
void GeomAlgoAPI_MakeShapeList::append(const GeomAlgoAPI_MakeShapeList& theMakeShapeList)
{
  for(ListOfMakeShape::const_iterator anIt = theMakeShapeList.myListOfMakeShape.cbegin();
    anIt != theMakeShapeList.myListOfMakeShape.cend(); anIt++) {
    myListOfMakeShape.push_back(*anIt);
  }
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
  result(theShape,  GeomAlgoAPI_MakeShapeList::Generated, theHistory);
}

//=================================================================================================
void GeomAlgoAPI_MakeShapeList::modified(const std::shared_ptr<GeomAPI_Shape> theShape,
                                         ListOfShape& theHistory)
{
  result(theShape, GeomAlgoAPI_MakeShapeList::Modified, theHistory);
}

bool GeomAlgoAPI_MakeShapeList::isDeleted(const std::shared_ptr<GeomAPI_Shape> theShape)
{
  for(ListOfMakeShape::iterator aBuilderIt = myListOfMakeShape.begin(); aBuilderIt != myListOfMakeShape.end(); aBuilderIt++) {
    std::shared_ptr<GeomAlgoAPI_MakeShape> aMakeShape = *aBuilderIt;
    if(aMakeShape->isDeleted(theShape)) {
      return true;
    }
  }

  return false;
}

void GeomAlgoAPI_MakeShapeList::result(const std::shared_ptr<GeomAPI_Shape> theShape,
                                       OperationType theOperationType,
                                       ListOfShape& theHistory)
{
  if(myListOfMakeShape.empty()) {
    return;
  }

  NCollection_Map<TopoDS_Shape> anAlgoShapes;
  NCollection_Map<TopoDS_Shape> aResultShapes;
  anAlgoShapes.Add(theShape->impl<TopoDS_Shape>());
  aResultShapes.Add(theShape->impl<TopoDS_Shape>());

  for(ListOfMakeShape::iterator aBuilderIt = myListOfMakeShape.begin(); aBuilderIt != myListOfMakeShape.end(); aBuilderIt++) {
    std::shared_ptr<GeomAlgoAPI_MakeShape> aMakeShape = *aBuilderIt;
    NCollection_Map<TopoDS_Shape> aTempShapes;
    bool hasResults = false;
    for(NCollection_Map<TopoDS_Shape>::Iterator aShapeIt(anAlgoShapes); aShapeIt.More(); aShapeIt.Next()) {
      std::shared_ptr<GeomAPI_Shape> aShape(new GeomAPI_Shape);
      aShape->setImpl(new TopoDS_Shape(aShapeIt.Value()));
      ListOfShape aGeneratedShapes;
      aMakeShape->generated(aShape, aGeneratedShapes);
      for(ListOfShape::const_iterator anIt = aGeneratedShapes.cbegin(); anIt != aGeneratedShapes.cend(); anIt++) {
        aTempShapes.Add((*anIt)->impl<TopoDS_Shape>());
        aResultShapes.Add((*anIt)->impl<TopoDS_Shape>());
        hasResults = true;
      }
      ListOfShape aModifiedShapes;
      aMakeShape->modified(aShape, aModifiedShapes);
      for(ListOfShape::const_iterator anIt = aModifiedShapes.cbegin(); anIt != aModifiedShapes.cend(); anIt++) {
        aTempShapes.Add((*anIt)->impl<TopoDS_Shape>());
        aResultShapes.Add((*anIt)->impl<TopoDS_Shape>());
        hasResults = true;
      }
      if(hasResults) {
        aResultShapes.Remove(aShape->impl<TopoDS_Shape>());
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
