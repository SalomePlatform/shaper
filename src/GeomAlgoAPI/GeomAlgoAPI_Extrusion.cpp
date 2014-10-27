// File:        GeomAlgoAPI_Extrusion.cpp
// Created:     06 Jun 2014
// Author:      Artem ZHIDKOV

#include <GeomAlgoAPI_Extrusion.h>
#include <GeomAlgoAPI_MakeShape.h>
#include <GeomAlgoAPI_DFLoader.h>
#include <GeomAlgoAPI_DFLoader.h>
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

const double tolerance = Precision::Angular();
  // Constructor
GeomAlgoAPI_Extrusion::GeomAlgoAPI_Extrusion (boost::shared_ptr<GeomAPI_Shape> theBasis, double theSize)
	  : mySize(theSize), myBuilder(NULL), myDone(false), myShape(new GeomAPI_Shape())
  {
    myBasis = theBasis->impl<TopoDS_Shape>();
	build();
  }

  //============================================================================
  void GeomAlgoAPI_Extrusion::build()
  {
    bool isFirstNorm = true;
    gp_Dir aShapeNormal;

    //const TopoDS_Shape& aShape = theShape->impl<TopoDS_Shape>(); 
    Handle(Geom_Plane) aPlane = Handle(Geom_Plane)::DownCast(BRep_Tool::Surface(TopoDS::Face(myBasis)));
    if (aPlane.IsNull())  // non-planar shapes is not supported for extrusion yet
      return;

    const gp_Dir& aNormal = aPlane->Pln().Axis().Direction();  
    gp_Vec aVec(aNormal);
	aVec = aVec * mySize;

    myBuilder = new BRepPrimAPI_MakePrism(myBasis, aVec);
	if(myBuilder != NULL) {
	  myDone = myBuilder->IsDone();
	  if(myDone) {
		if(myBuilder->Shape().ShapeType() == TopAbs_COMPOUND) 
		  myResult = GeomAlgoAPI_DFLoader::refineResult(myBuilder->Shape());
		else
		  myResult = myBuilder->Shape();		
		myShape->setImpl((void *)&myResult);  
		myFirst->setImpl((void *)&(myBuilder->FirstShape()));
		myLast->setImpl((void *)&(myBuilder-> LastShape()));
	  }
	}	
  }

  //============================================================================
const bool GeomAlgoAPI_Extrusion::isDone() const
{return myDone;}

  //============================================================================
const bool GeomAlgoAPI_Extrusion::isValid() const
{
	bool isValid(false);
	if(myDone && !myBuilder->Shape().IsNull()) {
	  BRepCheck_Analyzer aChecker(myBuilder->Shape());
	  isValid = aChecker.IsValid();
	}
	return isValid;
  }

  //============================================================================
  const bool GeomAlgoAPI_Extrusion::hasVolume() const
  {
	bool hasVolume(false);
	if(isValid()) {
	  const TopoDS_Shape& aRShape = myBuilder->Shape();
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
  const ListOfShape& GeomAlgoAPI_Extrusion::generated(const boost::shared_ptr<GeomAPI_Shape> theShape)
  {
	myHistory.clear();
    if(myDone) {
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
   /*
  void GeomAlgoAPI_Extrusion::LoadNamingDS(boost::shared_ptr<ModelAPI_ResultBody> theResultBody, 
	  boost::shared_ptr<GeomAPI_Shape> theContext)
{
  if(isValid()) {
	const TopoDS_Shape& aShape = myBuilder->Shape();
	TopoDS_Shape aResult = GeomAlgoAPI_DFLoader::refineResult(aShape);
	boost::shared_ptr<Model_Data> aData = boost::dynamic_pointer_cast<Model_Data>(theResultBody->data());
	if (aData) {
      const TDF_Label& aShapeLab = aData->shapeLab();
	  const Handle(TDF_TagSource)& Tagger = TDF_TagSource::Set(aShapeLab);
	  if (Tagger.IsNull()) return;
	  Tagger->Set(0);

      TNaming_Builder aBuilder (aShapeLab);
	  if(myBasis.IsEqual(theContext->impl<TopoDS_Shape>()))
	    aBuilder.Generated(aResult);
	  else
        aBuilder.Generated(theContext->impl<TopoDS_Shape>(), aResult);

	  TopTools_DataMapOfShapeShape aSubShapes;
	  for (TopExp_Explorer Exp(aResult,TopAbs_FACE); Exp.More(); Exp.Next()) {
	    aSubShapes.Bind(Exp.Current(),Exp.Current());
	  }

	   //Insert lateral face : Face from Edge
	  TNaming_Builder  aLateralFaceBuilder(aShapeLab.NewChild());
	  GeomAlgoAPI_DFLoader::loadAndOrientGeneratedShapes(*myBuilder, myBasis, TopAbs_EDGE, aLateralFaceBuilder, aSubShapes);

	  //Insert bottom face
	  TopoDS_Shape aBottomFace = myBuilder->FirstShape();
      if (!aBottomFace.IsNull()) {
		if (aBottomFace.ShapeType() != TopAbs_COMPOUND) {
		  TNaming_Builder aBottomBuilder(aShapeLab.NewChild());  //2
          if (aSubShapes.IsBound(aBottomFace)) {
			aBottomFace = aSubShapes(aBottomFace);
		  }
        aBottomBuilder.Generated(aBottomFace);
		} else {
          TopoDS_Iterator itr(aBottomFace);
          for (; itr.More(); itr.Next()) {
            TNaming_Builder aBottomBuilder(aShapeLab.NewChild());
		    aBottomBuilder.Generated(itr.Value());
		  }
		}

	  }

	  //Insert top face
	  TopoDS_Shape aTopFace = myBuilder->LastShape();
	  if (!aTopFace.IsNull()) {
       if (aTopFace.ShapeType() != TopAbs_COMPOUND) {
         TNaming_Builder aTopBuilder(aShapeLab.NewChild()); //3
         if (aSubShapes.IsBound(aTopFace)) {
           aTopFace = aSubShapes(aTopFace);
		 }
        aTopBuilder.Generated(aTopFace);
	   } else {
        TopoDS_Iterator itr(aTopFace);
        for (; itr.More(); itr.Next()) {
          TNaming_Builder aTopBuilder(aShapeLab.NewChild());
          aTopBuilder.Generated(itr.Value());
		}
	   }
	  }
	}
  }
  */