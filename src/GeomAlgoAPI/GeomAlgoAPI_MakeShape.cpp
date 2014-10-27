// File:        GeomAlgoAPI_MakeShape.cpp
// Created:     20 Oct 2014
// Author:      Sergey ZARITCHNY

#include <GeomAlgoAPI_MakeShape.h>
#include <TopTools_ListOfShape.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>
GeomAlgoAPI_MakeShape::GeomAlgoAPI_MakeShape(BRepBuilderAPI_MakeShape *  theMkShape)
{ myBuilder = theMkShape;}

const boost::shared_ptr<GeomAPI_Shape>  GeomAlgoAPI_MakeShape::shape() const
{
  boost::shared_ptr<GeomAPI_Shape> aShape(new GeomAPI_Shape());
  if(myBuilder != NULL) 
    aShape->setImpl(new TopoDS_Shape(myBuilder->Shape()));
  return aShape;
}

  /// Returns the  list   of shapes generated   from the shape <theShape>
const ListOfShape& GeomAlgoAPI_MakeShape::generated(const boost::shared_ptr<GeomAPI_Shape> theShape)
{
  myHistory.clear();
  if(myBuilder != NULL) {
	const TopTools_ListOfShape& aList =  myBuilder->Generated(theShape->impl<TopoDS_Shape>());
    TopTools_ListIteratorOfListOfShape it(aList);
	for(;it.More();it.Next()) {
		boost::shared_ptr<GeomAPI_Shape> aShape(new GeomAPI_Shape());
		aShape->setImpl(&(it.Value()));
		myHistory.push_back(aShape);
	}
  }
  return myHistory;
}

  /// Returns the  list   of shapes modified   from the shape <theShape>
const ListOfShape& GeomAlgoAPI_MakeShape::modified(const boost::shared_ptr<GeomAPI_Shape> theShape)
{
  myHistory.clear();
  if(myBuilder != NULL) {
	const TopTools_ListOfShape& aList =  myBuilder->Modified(theShape->impl<TopoDS_Shape>());
	TopTools_ListIteratorOfListOfShape it(aList);
	for(;it.More();it.Next()) {
		boost::shared_ptr<GeomAPI_Shape> aShape(new GeomAPI_Shape());
		aShape->setImpl(&(it.Value()));
		myHistory.push_back(aShape);
	}
  }
  return myHistory;
}

  /// Returns whether the shape is an edge
bool GeomAlgoAPI_MakeShape::isDeleted(const boost::shared_ptr<GeomAPI_Shape> theShape)
{
  bool isDeleted(false);
  if (myBuilder != NULL) 
	isDeleted = (bool) myBuilder->IsDeleted(theShape->impl<TopoDS_Shape>());	
  return isDeleted;
}