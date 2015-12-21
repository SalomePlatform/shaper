// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        GeomAPI_ShapeExplorer.cpp
// Created:     5 June 2015
// Author:      Dmitry Bobylev

#include <GeomAPI_ShapeExplorer.h>

#include <Standard_NoMoreObject.hxx>
#include <TopExp_Explorer.hxx>

#define MY_EXPLORER implPtr<TopExp_Explorer>()

//=================================================================================================
GeomAPI_ShapeExplorer::GeomAPI_ShapeExplorer()
: GeomAPI_Interface(new TopExp_Explorer())
{
}

// returns a type of shape to expolode, but if toFind==SHAPE, it will return the type
// of the first sub-element of compoud if theSHape is compound
static TopAbs_ShapeEnum ShapeType(const std::shared_ptr<GeomAPI_Shape>& theShape,
                                  const GeomAPI_Shape::ShapeType toFind)
{
  if (toFind == GeomAPI_Shape::SHAPE) {
    TopoDS_Shape aShape = theShape->impl<TopoDS_Shape>();
    if (!aShape.IsNull() && aShape.ShapeType() == TopAbs_COMPOUND) {
      TopoDS_Iterator anIter(aShape);
      if (anIter.More()) {
        return anIter.Value().ShapeType();
      }
    }
  }
  return (TopAbs_ShapeEnum)toFind;
}

//=================================================================================================
GeomAPI_ShapeExplorer::GeomAPI_ShapeExplorer(const std::shared_ptr<GeomAPI_Shape>& theShape,
                                             const GeomAPI_Shape::ShapeType toFind,
                                             const GeomAPI_Shape::ShapeType toAvoid)
: GeomAPI_Interface(new TopExp_Explorer(theShape->impl<TopoDS_Shape>(),
                                       ShapeType(theShape, toFind),
                                       (TopAbs_ShapeEnum)toAvoid))
{
}

//=================================================================================================
void GeomAPI_ShapeExplorer::init(const std::shared_ptr<GeomAPI_Shape>& theShape,
                                 const GeomAPI_Shape::ShapeType toFind,
                                 const GeomAPI_Shape::ShapeType toAvoid)
{
  MY_EXPLORER->Init(theShape->impl<TopoDS_Shape>(),
                   ShapeType(theShape, toFind),
                   (TopAbs_ShapeEnum)toAvoid);
}

//=================================================================================================
bool GeomAPI_ShapeExplorer::more() const
{
  return MY_EXPLORER->More() == Standard_True;
}

//=================================================================================================
void GeomAPI_ShapeExplorer::next()
{
  try {
    MY_EXPLORER->Next();
  } catch (Standard_NoMoreObject) {
  }
}

//=================================================================================================
std::shared_ptr<GeomAPI_Shape> GeomAPI_ShapeExplorer::current()
{
  try {
    const TopoDS_Shape& aShape = MY_EXPLORER->Current();
    std::shared_ptr<GeomAPI_Shape> aGeomShape(new GeomAPI_Shape());
    aGeomShape->setImpl(new TopoDS_Shape(aShape));
    return aGeomShape;
  } catch (Standard_NoMoreObject) {
    return std::shared_ptr<GeomAPI_Shape>();
  }
}

//=================================================================================================
void GeomAPI_ShapeExplorer::reinit()
{
  MY_EXPLORER->ReInit();
}

//=================================================================================================
int GeomAPI_ShapeExplorer::depth() const
{
  return MY_EXPLORER->Depth();
}

//=================================================================================================
void GeomAPI_ShapeExplorer::clear()
{
  MY_EXPLORER->Clear();
}
