// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        GeomAlgoAPI_Prism.cpp
// Created:     5 May 2015
// Author:      Dmitry Bobylev

#include <GeomAlgoAPI_Prism.h>

#include <GeomAlgoAPI_DFLoader.h>

#include <BRep_Tool.hxx>
#include <BRepCheck_Analyzer.hxx>
#include <BRepFeat_MakePrism.hxx>
#include <BRepGProp.hxx>
#include <Geom_Plane.hxx>
#include <gp_Pln.hxx>
#include <GProp_GProps.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>

//=================================================================================================
GeomAlgoAPI_Prism::GeomAlgoAPI_Prism(std::shared_ptr<GeomAPI_Shape> theBasis,
                                     std::shared_ptr<GeomAPI_Shape> theFromShape,
                                     std::shared_ptr<GeomAPI_Shape> theToShape)
: myFromShape(theFromShape),
  myToShape(theToShape),
  myDone(false),
  myShape(new GeomAPI_Shape()),
  myFirst(new GeomAPI_Shape()),myLast(new GeomAPI_Shape())
{
  build(theBasis);
}

//=================================================================================================
void GeomAlgoAPI_Prism::build(const std::shared_ptr<GeomAPI_Shape>& theBasis)
{
  if(!theBasis || !myFromShape || !myToShape ||
    (myFromShape && myToShape && myFromShape->isEqual(myToShape))) {
    return;
  }

  TopoDS_Face aBasis = TopoDS::Face(theBasis->impl<TopoDS_Shape>());
  Handle(Geom_Plane) aPlane = Handle(Geom_Plane)::DownCast(BRep_Tool::Surface(aBasis));
  if(aPlane.IsNull()) { // non-planar shapes is not supported for extrusion yet
    return;
  }

  const gp_Dir& aNormal = aPlane->Pln().Axis().Direction();
  BRepFeat_MakePrism* aBuilder = new BRepFeat_MakePrism(aBasis, aBasis, aBasis, aNormal, 2, Standard_True);

  if(aBuilder) {
    setImpl(aBuilder);
    TopoDS_Shape aFromShape = myFromShape->impl<TopoDS_Shape>();
    TopoDS_Shape aToShape   = myToShape->impl<TopoDS_Shape>();
    aBuilder->Perform(aFromShape, aToShape);
    myDone = aBuilder->IsDone() == Standard_True;
    if(myDone){
      TopoDS_Shape aResult = aBuilder->Shape();
      TopExp_Explorer anExp(aResult, TopAbs_SOLID);
      if(!anExp.More()) {
        return;
      }
      if(aResult.ShapeType() == TopAbs_COMPOUND) {
        aResult = GeomAlgoAPI_DFLoader::refineResult(aResult);
      }
      // fill data map to keep correct orientation of sub-shapes
      for (TopExp_Explorer Exp(aResult,TopAbs_FACE); Exp.More(); Exp.Next()) {
        std::shared_ptr<GeomAPI_Shape> aCurrentShape(new GeomAPI_Shape());
        aCurrentShape->setImpl(new TopoDS_Shape(Exp.Current()));
        myMap.bind(aCurrentShape, aCurrentShape);
      }
      myShape->setImpl(new TopoDS_Shape(aResult));
      myFirst->setImpl(new TopoDS_Shape(aBuilder->Modified(aFromShape).First()));
      myLast->setImpl(new TopoDS_Shape(aBuilder->Modified(aToShape).First()));
      myMkShape = new GeomAlgoAPI_MakeShape (aBuilder);
    }
  }
}

//=================================================================================================
const bool GeomAlgoAPI_Prism::isDone() const
{
  return myDone;
}

//=================================================================================================
const bool GeomAlgoAPI_Prism::isValid() const
{
  BRepCheck_Analyzer aChecker(myShape->impl<TopoDS_Shape>());
  return (aChecker.IsValid() == Standard_True);
}

//=================================================================================================
const bool GeomAlgoAPI_Prism::hasVolume() const
{
  bool hasVolume(false);
  if(isValid()) {
    const TopoDS_Shape& aRShape = myShape->impl<TopoDS_Shape>();
    GProp_GProps aGProp;
    BRepGProp::VolumeProperties(aRShape, aGProp);
    if(aGProp.Mass() > Precision::Confusion())
      hasVolume = true;
  }
  return hasVolume;
}

//=================================================================================================
const std::shared_ptr<GeomAPI_Shape>& GeomAlgoAPI_Prism::shape () const
{
  return myShape;
}

//=================================================================================================
const std::shared_ptr<GeomAPI_Shape>& GeomAlgoAPI_Prism::firstShape()
{
  return myFirst;
}

//=================================================================================================
const std::shared_ptr<GeomAPI_Shape>& GeomAlgoAPI_Prism::lastShape()
{
  return myLast;
}

//=================================================================================================
void GeomAlgoAPI_Prism::mapOfShapes (GeomAPI_DataMapOfShapeShape& theMap) const
{
  theMap = myMap;
}

//=================================================================================================
GeomAlgoAPI_MakeShape* GeomAlgoAPI_Prism::makeShape() const
{
  return myMkShape;
}

//=================================================================================================
GeomAlgoAPI_Prism::~GeomAlgoAPI_Prism()
{
  if (myImpl) {
    myMap.clear();
  }
}