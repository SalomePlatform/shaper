// File:        GeomAlgoAPI_MakeShape.cpp
// Created:     20 Oct 2014
// Author:      Sergey ZARITCHNY

#include <GeomAlgoAPI_MakeShape.h>
#include <TopTools_ListOfShape.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>
GeomAlgoAPI_MakeShape::GeomAlgoAPI_MakeShape(void* theMkShape)
  : GeomAPI_Interface(theMkShape)
{}

const boost::shared_ptr<GeomAPI_Shape> GeomAlgoAPI_MakeShape::shape() const
{
  return myShape;
}

/// Returns the  list   of shapes generated   from the shape <theShape>
void GeomAlgoAPI_MakeShape::generated(
  const boost::shared_ptr<GeomAPI_Shape> theShape, ListOfShape& theHistory)
{
  BRepBuilderAPI_MakeShape* aBuilder = implPtr<BRepBuilderAPI_MakeShape>();
  if(aBuilder) {
    const TopTools_ListOfShape& aList =  aBuilder->Generated(theShape->impl<TopoDS_Shape>());
    TopTools_ListIteratorOfListOfShape it(aList);
    for(;it.More();it.Next()) {
      boost::shared_ptr<GeomAPI_Shape> aShape(new GeomAPI_Shape());
      aShape->setImpl(&(it.Value()));
      theHistory.push_back(aShape);
    }
  }
}

/// Returns the  list   of shapes modified   from the shape <theShape>
void GeomAlgoAPI_MakeShape::modified(
  const boost::shared_ptr<GeomAPI_Shape> theShape, ListOfShape& theHistory)
{
  BRepBuilderAPI_MakeShape* aBuilder = implPtr<BRepBuilderAPI_MakeShape>();
  if(aBuilder) {
    const TopTools_ListOfShape& aList =  aBuilder->Modified(theShape->impl<TopoDS_Shape>());
    TopTools_ListIteratorOfListOfShape it(aList);
    for(;it.More();it.Next()) {
      boost::shared_ptr<GeomAPI_Shape> aShape(new GeomAPI_Shape());
      aShape->setImpl(&(it.Value()));
      theHistory.push_back(aShape);
    }
  }
}

/// Returns whether the shape is an edge
bool GeomAlgoAPI_MakeShape::isDeleted(const boost::shared_ptr<GeomAPI_Shape> theShape)
{
  bool isDeleted(false);
  BRepBuilderAPI_MakeShape* aBuilder = implPtr<BRepBuilderAPI_MakeShape>();
  if(aBuilder) {
    isDeleted = aBuilder->IsDeleted(theShape->impl<TopoDS_Shape>()) == Standard_True;
  }
  return isDeleted;
}
