// File:        GeomAlgoAPI_Extrusion.cpp
// Created:     06 Jun 2014
// Author:      Artem ZHIDKOV

#include <GeomAlgoAPI_Extrusion.h>
#include <GeomAlgoAPI_MakeShape.h>
#include <GeomAlgoAPI_DFLoader.h>
#include <GeomAlgoAPI_MakeShape.h>
#include <gp_Pln.hxx>
#include <BRepPrimAPI_MakePrism.hxx>
#include <BRepBuilderAPI_MakeShape.hxx>
#include <Geom_Plane.hxx>
#include <Geom_Surface.hxx>
#include <TopExp_Explorer.hxx>
#include <BRepCheck_Analyzer.hxx>
#include <GProp_GProps.hxx>
#include <BRepGProp.hxx>
#include <TopoDS.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>
#include <Precision.hxx>
#include <TDF_TagSource.hxx>
#include <boost/shared_ptr.hpp>
#include <BRepPrimAPI_MakePrism.hxx>
#include <TopoDS_Shape.hxx>

const double tolerance = Precision::Angular();
// Constructor
GeomAlgoAPI_Extrusion::GeomAlgoAPI_Extrusion(
  boost::shared_ptr<GeomAPI_Shape> theBasis, double theSize)
: mySize(theSize), myDone(false), myShape(new GeomAPI_Shape()),
  myFirst(new GeomAPI_Shape()), myLast(new GeomAPI_Shape())
{
  build(theBasis);
}

//============================================================================
void GeomAlgoAPI_Extrusion::build(const boost::shared_ptr<GeomAPI_Shape>& theBasis)
{
  bool isFirstNorm = true;
  gp_Dir aShapeNormal;

  TopoDS_Face aBasis = TopoDS::Face(theBasis->impl<TopoDS_Shape>());
  Handle(Geom_Plane) aPlane = Handle(Geom_Plane)::DownCast(
    BRep_Tool::Surface(aBasis));
  if (aPlane.IsNull())  // non-planar shapes is not supported for extrusion yet
    return;

  const gp_Dir& aNormal = aPlane->Pln().Axis().Direction();  
  gp_Vec aVec(aNormal);
  aVec = aVec * mySize;

  BRepPrimAPI_MakePrism* aBuilder = new BRepPrimAPI_MakePrism(aBasis, aVec);
  if(aBuilder) {
    setImpl(aBuilder);
    myDone = aBuilder->IsDone() == Standard_True;
    if (myDone) {
      BRepCheck_Analyzer aChecker(aBuilder->Shape());
      myDone = aChecker.IsValid() == Standard_True;
    }
    if(myDone) {
      TopoDS_Shape aResult;
      if(aBuilder->Shape().ShapeType() == TopAbs_COMPOUND) 
        aResult = GeomAlgoAPI_DFLoader::refineResult(aBuilder->Shape());
      else
        aResult = aBuilder->Shape();
	
	for (TopExp_Explorer Exp(aResult,TopAbs_FACE); Exp.More(); Exp.Next()) {
	   boost::shared_ptr<GeomAPI_Shape> aCurrentShape(new GeomAPI_Shape());
       aCurrentShape->setImpl(new TopoDS_Shape(Exp.Current()));
	   myMap.bind(aCurrentShape, aCurrentShape);
	}   
 
      myShape->setImpl(new TopoDS_Shape(aResult));
      myFirst->setImpl(new TopoDS_Shape(aBuilder->FirstShape()));
      myLast->setImpl(new TopoDS_Shape(aBuilder-> LastShape()));
	  myMkShape = new GeomAlgoAPI_MakeShape (aBuilder);
    }
  }	
}

//============================================================================
const bool GeomAlgoAPI_Extrusion::isDone() const
{return myDone;}

//============================================================================
const bool GeomAlgoAPI_Extrusion::isValid() const
{
  return myDone;
}

//============================================================================
const bool GeomAlgoAPI_Extrusion::hasVolume() const
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

//============================================================================
const boost::shared_ptr<GeomAPI_Shape>& GeomAlgoAPI_Extrusion::shape () const 
{return myShape;}

//============================================================================
/*void GeomAlgoAPI_Extrusion::generated(
  const boost::shared_ptr<GeomAPI_Shape> theShape, ListOfShape& theHistory)
{
  theHistory.clear();
  if(myDone) {
    const TopTools_ListOfShape& aList = implPtr<BRepPrimAPI_MakePrism>()
      ->Generated(theShape->impl<TopoDS_Shape>());
    TopTools_ListIteratorOfListOfShape it(aList);
    for(;it.More();it.Next()) {
      boost::shared_ptr<GeomAPI_Shape> aShape(new GeomAPI_Shape());
      aShape->setImpl(&(it.Value()));
      theHistory.push_back(aShape);
    }
  }
}
*/
//============================================================================
const boost::shared_ptr<GeomAPI_Shape>& GeomAlgoAPI_Extrusion::firstShape()
{
  return myFirst;
}

//============================================================================
const boost::shared_ptr<GeomAPI_Shape>& GeomAlgoAPI_Extrusion::lastShape()
{
  return myLast;
}

//============================================================================
void GeomAlgoAPI_Extrusion::mapOfShapes (GeomAPI_DataMapOfShapeShape& theMap) const
{
  theMap = myMap;
}

//============================================================================
GeomAlgoAPI_MakeShape * GeomAlgoAPI_Extrusion::makeShape() const
{
  return myMkShape;
}

//============================================================================
GeomAlgoAPI_Extrusion::~GeomAlgoAPI_Extrusion()
{
  if (myImpl) {    
	myMap.clear();
	//delete myImpl;
  }
}