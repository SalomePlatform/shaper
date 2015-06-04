// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        GeomAPI_ShapeExplorer.cpp
// Created:     5 June 2015
// Author:      Dmitry Bobylev

#include <GeomAPI_ShapeExplorer.h>

#include <Standard_NoMoreObject.hxx>
#include <TopExp_Explorer.hxx>

#define MY_EXPLORER static_cast<TopExp_Explorer*>(myImpl)

//=================================================================================================
GeomAPI_ShapeExplorer::GeomAPI_ShapeExplorer()
: GeomAPI_Interface(new TopExp_Explorer())
{
}

//=================================================================================================
GeomAPI_ShapeExplorer::GeomAPI_ShapeExplorer(const std::shared_ptr<GeomAPI_Shape>& theShape,
                                             const GeomAPI_Shape::ShapeType toFind,
                                             const GeomAPI_Shape::ShapeType toAvoid)
: GeomAPI_Interface(new TopExp_Explorer(theShape->impl<TopoDS_Shape>(),
                                       (TopAbs_ShapeEnum)toFind,
                                       (TopAbs_ShapeEnum)toAvoid))
{
}

//=================================================================================================
void GeomAPI_ShapeExplorer::init(const std::shared_ptr<GeomAPI_Shape>& theShape,
                                 const GeomAPI_Shape::ShapeType toFind,
                                 const GeomAPI_Shape::ShapeType toAvoid)
{
  MY_EXPLORER->Init(theShape->impl<TopoDS_Shape>(),
                   (TopAbs_ShapeEnum)toFind,
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
