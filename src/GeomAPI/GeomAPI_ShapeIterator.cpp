// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        GeomAPI_ShapeIterator.cpp
// Created:     27 April 2016
// Author:      Dmitry Bobylev

#include "GeomAPI_ShapeIterator.h"

#include <Standard_NoMoreObject.hxx>
#include <TopoDS_Iterator.hxx>

#define MY_ITERATOR implPtr<TopoDS_Iterator>()

//==================================================================================================
GeomAPI_ShapeIterator::GeomAPI_ShapeIterator()
: GeomAPI_Interface(new TopoDS_Iterator())
{
}

//==================================================================================================
GeomAPI_ShapeIterator::GeomAPI_ShapeIterator(const std::shared_ptr<GeomAPI_Shape> theShape)
: GeomAPI_Interface(new TopoDS_Iterator())
{
  init(theShape);
}

//==================================================================================================
void GeomAPI_ShapeIterator::init(const std::shared_ptr<GeomAPI_Shape> theShape)
{
  if(!theShape.get()) {
    return;
  }
  MY_ITERATOR->Initialize(theShape->impl<TopoDS_Shape>());
}

//==================================================================================================
bool GeomAPI_ShapeIterator::more() const
{
  return MY_ITERATOR->More() == Standard_True;
}

//==================================================================================================
void GeomAPI_ShapeIterator::next()
{
  try {
    MY_ITERATOR->Next();
  } catch (Standard_NoMoreObject) {
  }
}

//==================================================================================================
std::shared_ptr<GeomAPI_Shape> GeomAPI_ShapeIterator::current()
{
  try {
    const TopoDS_Shape& aShape = MY_ITERATOR->Value();
    std::shared_ptr<GeomAPI_Shape> aGeomShape(new GeomAPI_Shape());
    aGeomShape->setImpl(new TopoDS_Shape(aShape));
    return aGeomShape;
  } catch (Standard_NoMoreObject) {
    return std::shared_ptr<GeomAPI_Shape>();
  }
}
