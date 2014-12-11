// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        GeomAlgoAPI_Boolean.cpp
// Created:     02 Sept 2014
// Author:      Vitaly Smetannikov

#include "GeomAlgoAPI_Boolean.h"

#include <BRepAlgoAPI_Cut.hxx>
#include <BRepAlgoAPI_Common.hxx>
#include <BRepAlgoAPI_Fuse.hxx>
#include <BRepCheck_Analyzer.hxx>
#include <TopExp_Explorer.hxx>
#include <GeomAlgoAPI_DFLoader.h>

std::shared_ptr<GeomAPI_Shape> GeomAlgoAPI_Boolean::makeCut(
  std::shared_ptr<GeomAPI_Shape> theShape,
  std::shared_ptr<GeomAPI_Shape> theTool)
{
  const TopoDS_Shape& aShape = theShape->impl<TopoDS_Shape>();
  const TopoDS_Shape& aTool = theTool->impl<TopoDS_Shape>();

  BRepAlgoAPI_Cut aCut(aShape, aTool);
  if (aCut.IsDone()) {
    std::shared_ptr<GeomAPI_Shape> aResult(new GeomAPI_Shape());
    aResult->setImpl(new TopoDS_Shape(aCut.Shape()));
    return aResult;
  }
  return std::shared_ptr<GeomAPI_Shape>();
}


std::shared_ptr<GeomAPI_Shape> GeomAlgoAPI_Boolean::makeFuse(
  std::shared_ptr<GeomAPI_Shape> theShape,
  std::shared_ptr<GeomAPI_Shape> theTool)
{
  const TopoDS_Shape& aShape = theShape->impl<TopoDS_Shape>();
  const TopoDS_Shape& aTool = theTool->impl<TopoDS_Shape>();

  BRepAlgoAPI_Fuse aFuse(aShape, aTool);
  if (aFuse.IsDone()) {
    std::shared_ptr<GeomAPI_Shape> aResult(new GeomAPI_Shape());
    aResult->setImpl(new TopoDS_Shape(aFuse.Shape()));
    return aResult;
  }
  return std::shared_ptr<GeomAPI_Shape>();
}


std::shared_ptr<GeomAPI_Shape> GeomAlgoAPI_Boolean::makeCommon(
  std::shared_ptr<GeomAPI_Shape> theShape,
  std::shared_ptr<GeomAPI_Shape> theTool)
{
  const TopoDS_Shape& aShape = theShape->impl<TopoDS_Shape>();
  const TopoDS_Shape& aTool = theTool->impl<TopoDS_Shape>();

  BRepAlgoAPI_Common aCommon(aShape, aTool);
  if (aCommon.IsDone()) {
    std::shared_ptr<GeomAPI_Shape> aResult(new GeomAPI_Shape());
    aResult->setImpl(new TopoDS_Shape(aCommon.Shape()));
    return aResult;
  }
  return std::shared_ptr<GeomAPI_Shape>();
}

//============================================================================
GeomAlgoAPI_Boolean::GeomAlgoAPI_Boolean(std::shared_ptr<GeomAPI_Shape> theObject,
                                         std::shared_ptr<GeomAPI_Shape> theTool,
                                         int theType)
: myOperation(theType), myDone(false), myShape(new GeomAPI_Shape())
{
  build(theObject, theTool);
}


//============================================================================
void GeomAlgoAPI_Boolean::build(std::shared_ptr<GeomAPI_Shape> theObject,
                                std::shared_ptr<GeomAPI_Shape> theTool)
{
  const TopoDS_Shape& anObject = theObject->impl<TopoDS_Shape>();
  const TopoDS_Shape& aTool    = theTool->impl<TopoDS_Shape>();
  TopoDS_Shape aResult;
  switch (myOperation) {
  case BOOL_FUSE: 
	{
	  BRepAlgoAPI_Fuse* mkFuse = new BRepAlgoAPI_Fuse(anObject, aTool);
      if (mkFuse && mkFuse->IsDone()) {
		setImpl(mkFuse);
		myDone = mkFuse->IsDone() == Standard_True;
		myMkShape = new GeomAlgoAPI_MakeShape (mkFuse);
		aResult = mkFuse->Shape();//GeomAlgoAPI_DFLoader::refineResult(aFuse->Shape());      
	  }
	  break;
	}
  case BOOL_CUT:
	{
      BRepAlgoAPI_Cut* mkCut = new BRepAlgoAPI_Cut(anObject, aTool);
      if (mkCut && mkCut->IsDone()) {
		setImpl(mkCut);
		myDone = mkCut->IsDone() == Standard_True;
		myMkShape = new GeomAlgoAPI_MakeShape (mkCut);
		aResult = mkCut->Shape();    
	  }
	  break;
	}
  case BOOL_COMMON:
	{
      BRepAlgoAPI_Common* mkCom = new BRepAlgoAPI_Common(anObject, aTool);
      if (mkCom && mkCom->IsDone()) {
		setImpl(mkCom);
		myDone = mkCom->IsDone() == Standard_True;
		myMkShape = new GeomAlgoAPI_MakeShape (mkCom);
		aResult = mkCom->Shape(); 
	  }
	  break;
	}	
  }
  if(myDone) {
	if(aResult.ShapeType() == TopAbs_COMPOUND) 
      aResult = GeomAlgoAPI_DFLoader::refineResult(aResult);
	myShape->setImpl(new TopoDS_Shape(aResult));
	std::shared_ptr<GeomAPI_Shape> aGeomResult(new GeomAPI_Shape());
	aGeomResult->setImpl(new TopoDS_Shape(aResult)); 

	// fill data map to keep correct orientation of sub-shapes 
	for (TopExp_Explorer Exp(aResult,TopAbs_FACE); Exp.More(); Exp.Next()) {
	  std::shared_ptr<GeomAPI_Shape> aCurrentShape(new GeomAPI_Shape());
      aCurrentShape->setImpl(new TopoDS_Shape(Exp.Current()));
	  myMap.bind(aCurrentShape, aCurrentShape);
	}
  }  
}


//============================================================================
const bool GeomAlgoAPI_Boolean::isDone() const
{return myDone;}

//============================================================================
const bool GeomAlgoAPI_Boolean::isValid() const
{
  BRepCheck_Analyzer aChecker(myShape->impl<TopoDS_Shape>());
  return (aChecker.IsValid() == Standard_True);
}

//============================================================================
const std::shared_ptr<GeomAPI_Shape>& GeomAlgoAPI_Boolean::shape () const 
{
  return myShape;
}

//============================================================================
void GeomAlgoAPI_Boolean::mapOfShapes (GeomAPI_DataMapOfShapeShape& theMap) const
{
  theMap = myMap;
}

//============================================================================
GeomAlgoAPI_MakeShape * GeomAlgoAPI_Boolean::makeShape() const
{
  return myMkShape;
}

//============================================================================
GeomAlgoAPI_Boolean::~GeomAlgoAPI_Boolean()
{
  if (myImpl) {    
	myMap.clear();
  }
}