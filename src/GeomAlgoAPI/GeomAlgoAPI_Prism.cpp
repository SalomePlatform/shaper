// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        GeomAlgoAPI_Prism.cpp
// Created:     5 May 2015
// Author:      Dmitry Bobylev

#include <GeomAlgoAPI_Prism.h>

#include <GeomAPI_Face.h>
#include <GeomAPI_Pln.h>
#include <GeomAPI_Pnt.h>
#include <GeomAPI_XYZ.h>
#include <GeomAlgoAPI_DFLoader.h>
#include <GeomAlgoAPI_FaceBuilder.h>

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
                                     double                         theToSize,
                                     double                         theFromSize)
: myDone(false)
{
  build(theBasis, std::shared_ptr<GeomAPI_Shape>(), theToSize, std::shared_ptr<GeomAPI_Shape>(), theFromSize);
}

//=================================================================================================
GeomAlgoAPI_Prism::GeomAlgoAPI_Prism(std::shared_ptr<GeomAPI_Shape> theBasis,
                                     std::shared_ptr<GeomAPI_Shape> theToShape,
                                     double                         theToSize,
                                     std::shared_ptr<GeomAPI_Shape> theFromShape,
                                     double                         theFromSize)
: myDone(false)
{
  build(theBasis, theToShape, theToSize, theFromShape, theFromSize);
}

//=================================================================================================
void GeomAlgoAPI_Prism::build(const std::shared_ptr<GeomAPI_Shape>& theBasis,
                              const std::shared_ptr<GeomAPI_Shape>& theToShape,
                              double                                theToSize,
                              const std::shared_ptr<GeomAPI_Shape>& theFromShape,
                              double                                theFromSize)
{
  if(!theBasis ||
    (((!theFromShape && !theToShape) || (theFromShape && theToShape && theFromShape->isEqual(theToShape)))
    && (theFromSize == 0.0 && theToSize == 0.0))) {
    return;
  }

  // If bounding faces was not set creating them.
  std::shared_ptr<GeomAPI_Face>  aBaseFace(new GeomAPI_Face(theBasis));
  std::shared_ptr<GeomAPI_Pln>   aBasePln = aBaseFace->getPlane();
  std::shared_ptr<GeomAPI_Dir>   aBaseDir = aBasePln->direction();
  std::shared_ptr<GeomAPI_Pnt>   aBaseLoc = aBasePln->location();
  std::shared_ptr<GeomAPI_Shape> aBasePlane = GeomAlgoAPI_FaceBuilder::planarFace(aBaseLoc, aBaseDir);

  std::shared_ptr<GeomAPI_Shape> aBoundingFromShape = theFromShape ? theFromShape : aBasePlane;
  std::shared_ptr<GeomAPI_Shape> aBoundingToShape   = theToShape   ? theToShape   : aBasePlane;

  // Moving bounding faces according to "from" and "to" sizes.
  std::shared_ptr<GeomAPI_Face> aFromFace(new GeomAPI_Face(aBoundingFromShape));
  std::shared_ptr<GeomAPI_Pln>  aFromPln = aFromFace->getPlane();
  std::shared_ptr<GeomAPI_Pnt>  aFromLoc = aFromPln->location();
  std::shared_ptr<GeomAPI_Dir>  aFromDir = aFromPln->direction();

  std::shared_ptr<GeomAPI_Face> aToFace(new GeomAPI_Face(aBoundingToShape));
  std::shared_ptr<GeomAPI_Pln>  aToPln = aToFace->getPlane();
  std::shared_ptr<GeomAPI_Pnt>  aToLoc = aToPln->location();
  std::shared_ptr<GeomAPI_Dir>  aToDir = aToPln->direction();

  bool aSign = aFromLoc->xyz()->dot(aBaseDir->xyz()) > aToLoc->xyz()->dot(aBaseDir->xyz());

  std::shared_ptr<GeomAPI_Pnt> aFromPnt(new GeomAPI_Pnt(aFromLoc->xyz()->added(aBaseDir->xyz()->multiplied(
                                                        aSign ? theFromSize : -theFromSize))));
  aBoundingFromShape = GeomAlgoAPI_FaceBuilder::planarFace(aFromPnt, aFromDir);

  std::shared_ptr<GeomAPI_Pnt> aToPnt(new GeomAPI_Pnt(aToLoc->xyz()->added(aBaseDir->xyz()->multiplied(
                                                      aSign ? -theToSize : theToSize))));
  aBoundingToShape = GeomAlgoAPI_FaceBuilder::planarFace(aToPnt, aToDir);

  TopoDS_Face aBasis = TopoDS::Face(theBasis->impl<TopoDS_Shape>());
  Handle(Geom_Plane) aPlane = Handle(Geom_Plane)::DownCast(BRep_Tool::Surface(aBasis));
  if(aPlane.IsNull()) { // non-planar shapes is not supported for extrusion yet
    return;
  }

  const gp_Dir& aNormal = aPlane->Pln().Axis().Direction();
  BRepFeat_MakePrism* aBuilder = new BRepFeat_MakePrism(aBasis, aBasis, aBasis, aNormal, 2, Standard_True);

  if(aBuilder) {
    const TopoDS_Shape& aFromShape = aBoundingFromShape->impl<TopoDS_Shape>();
    const TopoDS_Shape& aToShape   = aBoundingToShape->impl<TopoDS_Shape>();
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
      myMap = std::make_shared<GeomAPI_DataMapOfShapeShape>();
      for (TopExp_Explorer Exp(aResult,TopAbs_FACE); Exp.More(); Exp.Next()) {
        std::shared_ptr<GeomAPI_Shape> aCurrentShape(new GeomAPI_Shape());
        aCurrentShape->setImpl(new TopoDS_Shape(Exp.Current()));
        myMap->bind(aCurrentShape, aCurrentShape);
      }
      myShape = std::make_shared<GeomAPI_Shape>();
      myShape->setImpl(new TopoDS_Shape(aResult));
      myFirst = std::make_shared<GeomAPI_Shape>();
      myFirst->setImpl(new TopoDS_Shape(aBuilder->Modified(aFromShape).First()));
      myLast = std::make_shared<GeomAPI_Shape>();
      myLast->setImpl(new TopoDS_Shape(aBuilder->Modified(aToShape).First()));
      myMkShape = std::make_shared<GeomAlgoAPI_MakeShape>();
      myMkShape->setImpl(aBuilder);
    }
  }
}

//=================================================================================================
bool GeomAlgoAPI_Prism::isDone() const
{
  return myDone;
}

//=================================================================================================
bool GeomAlgoAPI_Prism::isValid() const
{
  BRepCheck_Analyzer aChecker(myShape->impl<TopoDS_Shape>());
  return (aChecker.IsValid() == Standard_True);
}

//=================================================================================================
bool GeomAlgoAPI_Prism::hasVolume() const
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
std::shared_ptr<GeomAPI_Shape> GeomAlgoAPI_Prism::shape() const
{
  return myShape;
}

//=================================================================================================
std::shared_ptr<GeomAPI_Shape> GeomAlgoAPI_Prism::firstShape() const
{
  return myFirst;
}

//=================================================================================================
std::shared_ptr<GeomAPI_Shape> GeomAlgoAPI_Prism::lastShape() const
{
  return myLast;
}

//=================================================================================================
std::shared_ptr<GeomAPI_DataMapOfShapeShape> GeomAlgoAPI_Prism::mapOfShapes() const
{
  return myMap;
}

//=================================================================================================
std::shared_ptr<GeomAlgoAPI_MakeShape> GeomAlgoAPI_Prism::makeShape() const
{
  return myMkShape;
}
