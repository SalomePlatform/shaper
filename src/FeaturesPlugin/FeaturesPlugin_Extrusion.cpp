// File:        FeaturesPlugin_Extrusion.cpp
// Created:     30 May 2014
// Author:      Vitaly SMETANNIKOV

#include "FeaturesPlugin_Extrusion.h"
#include <ModelAPI_Session.h>
#include <ModelAPI_Document.h>
#include <ModelAPI_Data.h>
#include <ModelAPI_ResultConstruction.h>
#include <ModelAPI_ResultBody.h>
#include <ModelAPI_AttributeDouble.h>
#include <ModelAPI_AttributeSelection.h>
#include <ModelAPI_AttributeBoolean.h>
#include <Events_Error.h>
#include <GeomAlgoAPI_Extrusion.h>
#include <TopoDS_Shape.hxx>
#include <TopTools_DataMapOfShapeShape.hxx>
#include <TopExp_Explorer.hxx>
#include <TopTools_MapOfShape.hxx>
using namespace std;
#define _LATERAL_TAG 1
#define _FIRST_TAG 2
#define _LAST_TAG 3
#ifdef _DEBUG
#include <iostream>
#include <ostream>
#endif
FeaturesPlugin_Extrusion::FeaturesPlugin_Extrusion()
{
}

void FeaturesPlugin_Extrusion::initAttributes()
{
  data()->addAttribute(FeaturesPlugin_Extrusion::FACE_ID(), ModelAPI_AttributeSelection::type());
  data()->addAttribute(FeaturesPlugin_Extrusion::SIZE_ID(), ModelAPI_AttributeDouble::type());
  data()->addAttribute(FeaturesPlugin_Extrusion::REVERSE_ID(), ModelAPI_AttributeBoolean::type());
}

void FeaturesPlugin_Extrusion::execute()
{
  boost::shared_ptr<ModelAPI_AttributeSelection> aFaceRef = boost::dynamic_pointer_cast<
      ModelAPI_AttributeSelection>(data()->attribute(FeaturesPlugin_Extrusion::FACE_ID()));
  if (!aFaceRef)
    return;

  boost::shared_ptr<GeomAPI_Shape> aFace = 
    boost::dynamic_pointer_cast<GeomAPI_Shape>(aFaceRef->value());
  if (!aFace)
    return;

  boost::shared_ptr<GeomAPI_Shape> aContext = 
    boost::dynamic_pointer_cast<GeomAPI_Shape>(aFaceRef->context());

  double aSize = data()->real(FeaturesPlugin_Extrusion::SIZE_ID())->value();
  if (data()->boolean(FeaturesPlugin_Extrusion::REVERSE_ID())->value())
    aSize = -aSize;

  boost::shared_ptr<ModelAPI_ResultBody> aResultBody = document()->createBody(data());
  //TCollection_AsciiString anError;
  GeomAlgoAPI_Extrusion aFeature(aFace, aSize);
  if(!aFeature.isDone()) {
	std::string aFeatureError = "Extrusion algorithm failed";  
    Events_Error::send(aFeatureError, this);
	return;
  }

   // Check if shape is valid
  if (!aFeature.shape()->impl<TopoDS_Shape>().IsNull()) {
    std::string aShapeError = "Resulting shape is Null";     
    Events_Error::send(aShapeError, this);
 #ifdef _DEBUG
    std::cerr << aShapeError << std::endl;
 #endif
    return;
  }
   if(!aFeature.isValid()) {
	std::string aFeatureError = "Warning: resulting shape is not valid";  
    Events_Error::send(aFeatureError, this);
	return;
  }  
  //LoadNamingDS
  LoadNamingDS(aFeature, aResultBody, aFace, aContext);

  setResult(aResultBody);
}

  //============================================================================
void FeaturesPlugin_Extrusion::LoadNamingDS(GeomAlgoAPI_Extrusion& theFeature, 
	                             boost::shared_ptr<ModelAPI_ResultBody> theResultBody, 
								 boost::shared_ptr<GeomAPI_Shape> theBasis,
	                             boost::shared_ptr<GeomAPI_Shape> theContext)
{  

	
    //load result
	if(theBasis->impl<TopoDS_Shape>().IsEqual(theContext->impl<TopoDS_Shape>()))
	  theResultBody->store(theFeature.shape());
	else
	  theResultBody->storeGenerated(theContext, theFeature.shape());

	TopTools_DataMapOfShapeShape aSubShapes;
	for (TopExp_Explorer Exp(theFeature.shape()->impl<TopoDS_Shape>(),TopAbs_FACE); Exp.More(); Exp.Next()) {
	  aSubShapes.Bind(Exp.Current(),Exp.Current());
	}

	//Insert lateral face : Face from Edge
	//GeomAlgoAPI_DFLoader::loadAndOrientGeneratedShapes(*myBuilder, myBasis, TopAbs_EDGE, aLateralFaceBuilder, aSubShapes);


  TopTools_MapOfShape aView;
  TopExp_Explorer aShapeExplorer (theFeature.shape()->impl<TopoDS_Shape>(), TopAbs_EDGE);
  for (; aShapeExplorer.More(); aShapeExplorer.Next ()) {
    const TopoDS_Shape& aRoot = aShapeExplorer.Current ();
    if (!aView.Add(aRoot)) continue;
	boost::shared_ptr<GeomAPI_Shape> aRootG(new GeomAPI_Shape());
	aRootG->setImpl((void *)&aRoot);
	const ListOfShape& aShapes = theFeature.generated(aRootG);
	std::list<boost::shared_ptr<GeomAPI_Shape> >::const_iterator anIt = aShapes.begin(), aLast = aShapes.end();	  
	for (; anIt != aLast; anIt++) {
      TopoDS_Shape aNewShape = (*anIt)->impl<TopoDS_Shape>(); 
      if (aSubShapes.IsBound(aNewShape)) {
        aNewShape.Orientation((aSubShapes(aNewShape)).Orientation());
      }

	  if (!aRoot.IsSame (aNewShape)) {
		boost::shared_ptr<GeomAPI_Shape> aNew(new GeomAPI_Shape());
	    aNew->setImpl((void *)&aNewShape);
		  theResultBody->generated(aRootG, aNew,_LATERAL_TAG); 
	  }
	}
  }

  //Insert bottom face
  const boost::shared_ptr<GeomAPI_Shape>& aBottomFace = theFeature.firstShape();
  if (!aBottomFace->impl<TopoDS_Shape>().IsNull()) {
	  if (aSubShapes.IsBound(aBottomFace->impl<TopoDS_Shape>())) {
		aBottomFace->setImpl((void *)&aSubShapes(aBottomFace->impl<TopoDS_Shape>()));
	  }    
  theResultBody->generated(aBottomFace, _FIRST_TAG);
  }

  

 //Insert top face
  boost::shared_ptr<GeomAPI_Shape> aTopFace = theFeature.lastShape();
  if (!aTopFace->impl<TopoDS_Shape>().IsNull()) {
	if (aSubShapes.IsBound(aTopFace->impl<TopoDS_Shape>())) {
		aTopFace->setImpl((void *)&aSubShapes(aTopFace->impl<TopoDS_Shape>()));
	}
    theResultBody->generated(aTopFace, _FIRST_TAG);
  }

}
