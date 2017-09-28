// Copyright (C) 2014-201x CEA/DEN, EDF R&D

// File:        PrimitivesPlugin_Torus.cpp
// Created:     17 Mar 2017
// Author:      Clarisse Genrault (CEA)

#include <PrimitivesPlugin_Torus.h>

#include <GeomAPI_Edge.h>
#include <GeomAPI_Lin.h>
#include <GeomAPI_ShapeExplorer.h>

#include <GeomAlgoAPI_PointBuilder.h>

#include <ModelAPI_AttributeDouble.h>
#include <ModelAPI_AttributeSelection.h>
#include <ModelAPI_ResultBody.h>
#include <ModelAPI_ResultConstruction.h>
#include <ModelAPI_Session.h>

#include <sstream>

//=================================================================================================
PrimitivesPlugin_Torus::PrimitivesPlugin_Torus()
{
}

//=================================================================================================
void PrimitivesPlugin_Torus::initAttributes()
{
  data()->addAttribute(PrimitivesPlugin_Torus::BASE_POINT_ID(),
                       ModelAPI_AttributeSelection::typeId());
  data()->addAttribute(PrimitivesPlugin_Torus::AXIS_ID(),
                       ModelAPI_AttributeSelection::typeId());

  data()->addAttribute(PrimitivesPlugin_Torus::RADIUS_ID(),
                       ModelAPI_AttributeDouble::typeId());
  data()->addAttribute(PrimitivesPlugin_Torus::RING_RADIUS_ID(),
                       ModelAPI_AttributeDouble::typeId());

  // Initialize the base point of the torus at the origin if the base point is not filled.
  AttributeSelectionPtr aCenterPoint =
    data()->selection(PrimitivesPlugin_Torus::BASE_POINT_ID());
  if (!aCenterPoint->isInitialized()) {
    ObjectPtr aPointObj = ModelAPI_Session::get()->moduleDocument()
      ->objectByName(ModelAPI_ResultConstruction::group(), "Origin");
    if (aPointObj.get()) {
      ResultPtr aPointRes = std::dynamic_pointer_cast<ModelAPI_Result>(aPointObj);
      aCenterPoint->setValue(aPointRes, std::shared_ptr<GeomAPI_Shape>());
    }
  }

  // Initialize the axis at the OZ axis if the axis is not filled.
  AttributeSelectionPtr anAxis = data()->selection(PrimitivesPlugin_Torus::AXIS_ID());
  if (!anAxis->isInitialized()) {
    ObjectPtr anAxisObj = ModelAPI_Session::get()->moduleDocument()
      ->objectByName(ModelAPI_ResultConstruction::group(), "OZ");
    if (anAxisObj.get()) {
      ResultPtr anAxisRes = std::dynamic_pointer_cast<ModelAPI_Result>(anAxisObj);
      anAxis->setValue(anAxisRes, std::shared_ptr<GeomAPI_Shape>());
    }
  }
}

//=================================================================================================
void PrimitivesPlugin_Torus::execute()
{
  // Getting base point.
  std::shared_ptr<GeomAPI_Pnt> aBasePoint;
  std::shared_ptr<ModelAPI_AttributeSelection> aPointRef =
    selection(PrimitivesPlugin_Torus::BASE_POINT_ID());
  if (aPointRef.get() != NULL) {
    GeomShapePtr aShape1 = aPointRef->value();
    if (!aShape1.get()) {
      aShape1 = aPointRef->context()->shape();
    }
    if (aShape1) {
    	aBasePoint = GeomAlgoAPI_PointBuilder::point(aShape1);
    }
  }

  // Getting axis.
  std::shared_ptr<GeomAPI_Ax2> anAxis;
  std::shared_ptr<GeomAPI_Edge> anEdge;
  std::shared_ptr<ModelAPI_AttributeSelection> anEdgeRef =
    selection(PrimitivesPlugin_Torus::AXIS_ID());
  if(anEdgeRef && anEdgeRef->value() && anEdgeRef->value()->isEdge()) {
    anEdge = std::shared_ptr<GeomAPI_Edge>(new GeomAPI_Edge(anEdgeRef->value()));
  } else if (anEdgeRef && !anEdgeRef->value() && anEdgeRef->context() &&
             anEdgeRef->context()->shape() && anEdgeRef->context()->shape()->isEdge()) {
    anEdge = std::shared_ptr<GeomAPI_Edge>(new GeomAPI_Edge(anEdgeRef->context()->shape()));
  }
  if(anEdge) {
    anAxis = std::shared_ptr<GeomAPI_Ax2>(new GeomAPI_Ax2(aBasePoint,
                                                          anEdge->line()->direction()));
  }

  // Getting radius and ring radius
  double aRadius = real(PrimitivesPlugin_Torus::RADIUS_ID())->value();
  double aRingRadius = real(PrimitivesPlugin_Torus::RING_RADIUS_ID())->value();

  std::shared_ptr<GeomAlgoAPI_Torus> aTorusAlgo =
    std::shared_ptr<GeomAlgoAPI_Torus>(new GeomAlgoAPI_Torus(anAxis,
                                                             aRadius,
                                                             aRingRadius));

  // These checks should be made to the GUI for the feature but
  // the corresponding validator does not exist yet.
  if (!aTorusAlgo->check()) {
    setError(aTorusAlgo->getError());
    return;
  }

  // Build the sphere
  aTorusAlgo->build();

  // Check if the creation of the cylinder
  if(!aTorusAlgo->isDone()) {
    setError(aTorusAlgo->getError());
    return;
  }
  if(!aTorusAlgo->checkValid("Torus builder")) {
    setError(aTorusAlgo->getError());
    return;
  }

  int aResultIndex = 0;
  ResultBodyPtr aResultBox = document()->createBody(data(), aResultIndex);
  loadNamingDS(aTorusAlgo, aResultBox);
  setResult(aResultBox, aResultIndex);
}

//=================================================================================================
void PrimitivesPlugin_Torus::loadNamingDS(std::shared_ptr<GeomAlgoAPI_Torus> theTorusAlgo,
                                          std::shared_ptr<ModelAPI_ResultBody> theResultTorus)
{
  // Load the result
  theResultTorus->store(theTorusAlgo->shape());

  // Prepare the naming
  theTorusAlgo->prepareNamingFaces();

  // Insert to faces
  // Naming for faces
  int num = 1;
  std::map< std::string, std::shared_ptr<GeomAPI_Shape> > listOfFaces =
      theTorusAlgo->getCreatedFaces();
  for (std::map< std::string, std::shared_ptr<GeomAPI_Shape> >::iterator
       it=listOfFaces.begin(); it!=listOfFaces.end(); ++it) {
    std::shared_ptr<GeomAPI_Shape> aFace = (*it).second;
    theResultTorus->generated(aFace, (*it).first, num++);
  }
  
  // Naming of edges
  GeomAPI_DataMapOfShapeShape anEdges;
  GeomAPI_ShapeExplorer anEdgeExp(theTorusAlgo->shape(), GeomAPI_Shape::EDGE);
  for(int anIndex = 1; anEdgeExp.more(); anEdgeExp.next()) {
    if (!anEdges.isBound(anEdgeExp.current())) {
      std::ostringstream aStream;
      aStream<<"Edge_"<<anIndex++;
      theResultTorus->generated(anEdgeExp.current(), aStream.str(), num++);
      anEdges.bind(anEdgeExp.current(), anEdgeExp.current());
    }
  }
}
