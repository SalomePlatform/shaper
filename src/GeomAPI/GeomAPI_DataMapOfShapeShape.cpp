// File:        GeomAPI_DataMapOfShapeShape.cpp
// Created:     28 Oct 2014
// Author:      Sergey Zaritchny

#include <GeomAPI_Shape.h>
#include <GeomAPI_DataMapOfShapeShape.h>
#include <TopTools_DataMapOfShapeShape.hxx>
#include <TopoDS_Shape.hxx>
using namespace std;


GeomAPI_DataMapOfShapeShape::GeomAPI_DataMapOfShapeShape()
	:GeomAPI_Interface((void *)new TopTools_DataMapOfShapeShape){}

/// Clear 
void GeomAPI_DataMapOfShapeShape::clear()
{
  implPtr<TopTools_DataMapOfShapeShape>()->Clear();
}

/// Adds the Key <K> to  the Map <me>  with  the  Item. Returns True  if the Key  was not already in the map
bool GeomAPI_DataMapOfShapeShape::bind (std::shared_ptr<GeomAPI_Shape> theKey, std::shared_ptr<GeomAPI_Shape> theItem)
{
  bool flag(false);
  if(implPtr<TopTools_DataMapOfShapeShape>()->Bind(theKey->impl<TopoDS_Shape>(), theItem->impl<TopoDS_Shape>()))
	flag = true;
  return flag;
}

/// Returns true if theKey is stored  in the map.
bool GeomAPI_DataMapOfShapeShape::isBound (std::shared_ptr<GeomAPI_Shape> theKey)
{
  bool flag(false);
  if(impl<TopTools_DataMapOfShapeShape>().IsBound(theKey->impl<TopoDS_Shape>()))
    flag = true;
  return flag;
}

/// Returns  the Item stored  with the Key in the Map. To be checked before with isBound()
const std::shared_ptr<GeomAPI_Shape> GeomAPI_DataMapOfShapeShape::find(std::shared_ptr<GeomAPI_Shape> theKey)
{
  std::shared_ptr<GeomAPI_Shape> aShape(new GeomAPI_Shape());  
  aShape->setImpl(new TopoDS_Shape(impl<TopTools_DataMapOfShapeShape>().Find(theKey->impl<TopoDS_Shape>())));
  return aShape;
}  
  
/// Removes the Key from the  map. Returns true if the Key was in the Map
bool GeomAPI_DataMapOfShapeShape::unBind(std::shared_ptr<GeomAPI_Shape> theKey)
{
  bool flag(false);
  if(implPtr<TopTools_DataMapOfShapeShape>()->UnBind(theKey->impl<TopoDS_Shape>()))
	flag = true;
  return flag;
}

 GeomAPI_DataMapOfShapeShape::~GeomAPI_DataMapOfShapeShape()
 {
  if (myImpl) {
	implPtr<TopTools_DataMapOfShapeShape>()->Clear();
    //delete myImpl;
  }
 }