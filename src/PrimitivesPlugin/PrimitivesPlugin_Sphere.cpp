// Copyright (C) 2014-201x CEA/DEN, EDF R&D

// File:        PrimitivesPlugin_Sphere.h
// Created:     15 Mar 2017
// Author:      Clarisse Genrault (CEA)

#include <PrimitivesPlugin_Sphere.h>

#include <GeomAPI_ShapeExplorer.h>

#include <GeomAlgoAPI_PointBuilder.h>

#include <ModelAPI_AttributeDouble.h>
#include <ModelAPI_AttributeSelection.h>
#include <ModelAPI_ResultBody.h>
#include <ModelAPI_ResultConstruction.h>
#include <ModelAPI_Session.h>

#include <sstream>

//=================================================================================================
PrimitivesPlugin_Sphere::PrimitivesPlugin_Sphere()
{
}

//=================================================================================================
void PrimitivesPlugin_Sphere::initAttributes()
{
  data()->addAttribute(PrimitivesPlugin_Sphere::CENTER_POINT_ID(),
                       ModelAPI_AttributeSelection::typeId());

  data()->addAttribute(PrimitivesPlugin_Sphere::RADIUS_ID(),
                       ModelAPI_AttributeDouble::typeId());

  // Initialize the center point of the sphere at the origin if the center point is not filled.
  AttributeSelectionPtr aCenterPoint =
    data()->selection(PrimitivesPlugin_Sphere::CENTER_POINT_ID());
  if (!aCenterPoint->isInitialized()) {
    ObjectPtr aPointObj = ModelAPI_Session::get()->moduleDocument()
      ->objectByName(ModelAPI_ResultConstruction::group(), "Origin");
    if (aPointObj.get()) {
      ResultPtr aPointRes = std::dynamic_pointer_cast<ModelAPI_Result>(aPointObj);
      aCenterPoint->setValue(aPointRes, std::shared_ptr<GeomAPI_Shape>());
    }
  }
}

//=================================================================================================
void PrimitivesPlugin_Sphere::execute()
{
  // Getting point.
  std::shared_ptr<GeomAPI_Pnt> aCenterPoint;
  std::shared_ptr<ModelAPI_AttributeSelection> aPointRef =
    selection(PrimitivesPlugin_Sphere::CENTER_POINT_ID());
  if (aPointRef.get() != NULL) {
    GeomShapePtr aShape1 = aPointRef->value();
    if (!aShape1.get()) {
      aShape1 = aPointRef->context()->shape();
    }
    if (aShape1) {
    	aCenterPoint = GeomAlgoAPI_PointBuilder::point(aShape1);
    }
  }

  // Getting radius
  double aRadius = real(PrimitivesPlugin_Sphere::RADIUS_ID())->value();

  std::shared_ptr<GeomAlgoAPI_Sphere> aSphereAlgo =
    std::shared_ptr<GeomAlgoAPI_Sphere>(new GeomAlgoAPI_Sphere(aCenterPoint, aRadius));

  // These checks should be made to the GUI for the feature but
  // the corresponding validator does not exist yet.
  if (!aSphereAlgo->check()) {
    setError(aSphereAlgo->getError());
    return;
  }

  // Build the sphere
  aSphereAlgo->build();

  // Check if the creation of the cylinder
  if(!aSphereAlgo->isDone()) {
    setError(aSphereAlgo->getError());
    return;
  }
  if(!aSphereAlgo->checkValid("Sphere builder")) {
    setError(aSphereAlgo->getError());
    return;
  }

  int aResultIndex = 0;
  ResultBodyPtr aResultBox = document()->createBody(data(), aResultIndex);
  loadNamingDS(aSphereAlgo, aResultBox);
  setResult(aResultBox, aResultIndex);
}

//=================================================================================================
void PrimitivesPlugin_Sphere::loadNamingDS(std::shared_ptr<GeomAlgoAPI_Sphere> theSphereAlgo,
                                           std::shared_ptr<ModelAPI_ResultBody> theResultSphere)
{
  // Load the result
  theResultSphere->store(theSphereAlgo->shape());

  // Prepare the naming
  theSphereAlgo->prepareNamingFaces();

  // Insert to faces
  // Naming for faces and edges
  int num = 1;
  std::map< std::string, std::shared_ptr<GeomAPI_Shape> > listOfFaces =
      theSphereAlgo->getCreatedFaces();
  for (std::map< std::string, std::shared_ptr<GeomAPI_Shape> >::iterator
       it=listOfFaces.begin(); it!=listOfFaces.end(); ++it) {
    std::shared_ptr<GeomAPI_Shape> aFace = (*it).second;
    theResultSphere->generated(aFace, (*it).first, num++);
  }

  // Naming vertices
  GeomAPI_DataMapOfShapeShape aVertices;
  GeomAPI_ShapeExplorer aVertExp(theSphereAlgo->shape(), GeomAPI_Shape::VERTEX);
  for(int anIndex = 1; aVertExp.more(); aVertExp.next()) {
    if (!aVertices.isBound(aVertExp.current())) {
      std::ostringstream aStream;
      aStream<<"Vertex_"<<anIndex++;
      theResultSphere->generated(aVertExp.current(), aStream.str(), num++);
      aVertices.bind(aVertExp.current(), aVertExp.current());
    }
  }
}
