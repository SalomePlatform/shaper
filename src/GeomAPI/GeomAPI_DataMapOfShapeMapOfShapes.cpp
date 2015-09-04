// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        GeomAPI_DataMapOfShapeMapOfShapes.cpp
// Created:     4 September 2015
// Author:      Dmitry Bobylev

#include <GeomAPI_DataMapOfShapeMapOfShapes.h>

#include <NCollection_DataMap.hxx>
#include <NCollection_Map.hxx>
#include <TopoDS_Shape.hxx>

#define MY_MAP implPtr<NCollection_DataMap<TopoDS_Shape, NCollection_Map<TopoDS_Shape> > >()

//=================================================================================================
GeomAPI_DataMapOfShapeMapOfShapes::GeomAPI_DataMapOfShapeMapOfShapes()
: GeomAPI_Interface(new NCollection_DataMap<TopoDS_Shape, NCollection_Map<TopoDS_Shape> >)
{}

//=================================================================================================
bool GeomAPI_DataMapOfShapeMapOfShapes::bind(const std::shared_ptr<GeomAPI_Shape> theKey,
                                             const ListOfShape& theItems)
{
  const TopoDS_Shape& aKey = theKey->impl<TopoDS_Shape>();
  for(ListOfShape::const_iterator anIt = theItems.cbegin(); anIt != theItems.cend(); anIt++) {
    const TopoDS_Shape& anItem = (*anIt)->impl<TopoDS_Shape>();
    if(MY_MAP->IsBound(aKey)) {
      MY_MAP->ChangeFind(aKey).Add(anItem);
    } else {
      NCollection_Map<TopoDS_Shape> anItems;
      anItems.Add(anItem);
      MY_MAP->Bind(aKey, anItems);
    }
  }

  return true;
}

//=================================================================================================
bool GeomAPI_DataMapOfShapeMapOfShapes::add(const std::shared_ptr<GeomAPI_Shape> theKey,
                                            const std::shared_ptr<GeomAPI_Shape> theItem)
{
  const TopoDS_Shape& aKey = theKey->impl<TopoDS_Shape>();
  const TopoDS_Shape& anItem = theItem->impl<TopoDS_Shape>();
  if(MY_MAP->IsBound(aKey)) {
    return MY_MAP->ChangeFind(aKey).Add(anItem) == Standard_True;
  } else {
    NCollection_Map<TopoDS_Shape> anItems;
    anItems.Add(anItem);
    return MY_MAP->Bind(aKey, anItems) == Standard_True;
  }
}

//=================================================================================================
bool GeomAPI_DataMapOfShapeMapOfShapes::isBound(const std::shared_ptr<GeomAPI_Shape> theKey) const
{
  const TopoDS_Shape& aKey = theKey->impl<TopoDS_Shape>();
  return MY_MAP->IsBound(aKey) == Standard_True;
}

//=================================================================================================
bool GeomAPI_DataMapOfShapeMapOfShapes::find(const std::shared_ptr<GeomAPI_Shape> theKey,
                                             ListOfShape& theItems) const
{
  const TopoDS_Shape& aKey = theKey->impl<TopoDS_Shape>();

  if(MY_MAP->IsBound(aKey) == Standard_False) {
    return false;
  }

  const NCollection_Map<TopoDS_Shape>& aMap = MY_MAP->Find(aKey);
  for(NCollection_Map<TopoDS_Shape>::Iterator anIt(aMap); anIt.More(); anIt.Next()) {
    std::shared_ptr<GeomAPI_Shape> aShape(new GeomAPI_Shape);
    aShape->setImpl(new TopoDS_Shape(anIt.Value()));
    theItems.push_back(aShape);
  }
}

//=================================================================================================
bool GeomAPI_DataMapOfShapeMapOfShapes::unBind(const std::shared_ptr<GeomAPI_Shape> theKey)
{
  const TopoDS_Shape& aKey = theKey->impl<TopoDS_Shape>();
  return MY_MAP->UnBind(aKey) == Standard_True;
}

//=================================================================================================
int GeomAPI_DataMapOfShapeMapOfShapes::size() const
{
  return MY_MAP->Size();
}
