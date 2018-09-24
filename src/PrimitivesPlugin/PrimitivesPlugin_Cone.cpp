// Copyright (C) 2014-201x CEA/DEN, EDF R&D

// File:        PrimitivesPlugin_Cone.cpp
// Created:     17 Mar 2017
// Author:      Clarisse Genrault (CEA)

#include <PrimitivesPlugin_Cone.h>

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
PrimitivesPlugin_Cone::PrimitivesPlugin_Cone()
{
}

//=================================================================================================
void PrimitivesPlugin_Cone::initAttributes()
{
  data()->addAttribute(PrimitivesPlugin_Cone::BASE_POINT_ID(),
                       ModelAPI_AttributeSelection::typeId());
  data()->addAttribute(PrimitivesPlugin_Cone::AXIS_ID(),
                       ModelAPI_AttributeSelection::typeId());

  data()->addAttribute(PrimitivesPlugin_Cone::BASE_RADIUS_ID(),
                       ModelAPI_AttributeDouble::typeId());
  data()->addAttribute(PrimitivesPlugin_Cone::TOP_RADIUS_ID(),
                       ModelAPI_AttributeDouble::typeId());
  data()->addAttribute(PrimitivesPlugin_Cone::HEIGHT_ID(),
                       ModelAPI_AttributeDouble::typeId());

  // Initialize the base point of the cone at the origin if the base point is not filled.
  AttributeSelectionPtr aCenterPoint =
    data()->selection(PrimitivesPlugin_Cone::BASE_POINT_ID());
  if (!aCenterPoint->isInitialized()) {
    ObjectPtr aPointObj = ModelAPI_Session::get()->moduleDocument()
      ->objectByName(ModelAPI_ResultConstruction::group(), "Origin");
    if (aPointObj.get()) {
      ResultPtr aPointRes = std::dynamic_pointer_cast<ModelAPI_Result>(aPointObj);
      aCenterPoint->setValue(aPointRes, std::shared_ptr<GeomAPI_Shape>());
    }
  }

  // Initialize the axis at the OZ axis if the axis is not filled.
  AttributeSelectionPtr anAxis = data()->selection(PrimitivesPlugin_Cone::AXIS_ID());
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
void PrimitivesPlugin_Cone::execute()
{
  // Getting base point.
  std::shared_ptr<GeomAPI_Pnt> aBasePoint;
  std::shared_ptr<ModelAPI_AttributeSelection> aPointRef =
    selection(PrimitivesPlugin_Cone::BASE_POINT_ID());
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
    selection(PrimitivesPlugin_Cone::AXIS_ID());
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

  // Getting base radius, top radius and height
  double aBaseRadius = real(PrimitivesPlugin_Cone::BASE_RADIUS_ID())->value();
  double aTopRadius = real(PrimitivesPlugin_Cone::TOP_RADIUS_ID())->value();
  double aHeight = real(PrimitivesPlugin_Cone::HEIGHT_ID())->value();

  std::shared_ptr<GeomAlgoAPI_Cone> aConeAlgo =
    std::shared_ptr<GeomAlgoAPI_Cone>(new GeomAlgoAPI_Cone(anAxis,
                                                           aBaseRadius,
                                                           aTopRadius,
                                                           aHeight));

  // These checks should be made to the GUI for the feature but
  // the corresponding validator does not exist yet.
  if (!aConeAlgo->check()) {
    setError(aConeAlgo->getError());
    return;
  }

  // Build the sphere
  aConeAlgo->build();

  // Check if the creation of the cylinder
  if(!aConeAlgo->isDone()) {
    setError(aConeAlgo->getError());
    return;
  }
  if(!aConeAlgo->checkValid("Cone builder")) {
    setError(aConeAlgo->getError());
    return;
  }

  int aResultIndex = 0;
  ResultBodyPtr aResultBox = document()->createBody(data(), aResultIndex);
  loadNamingDS(aConeAlgo, aResultBox);
  setResult(aResultBox, aResultIndex);
}

//=================================================================================================
void PrimitivesPlugin_Cone::loadNamingDS(std::shared_ptr<GeomAlgoAPI_Cone> theConeAlgo,
                                         std::shared_ptr<ModelAPI_ResultBody> theResultCone)
{
  // Load the result
  theResultCone->store(theConeAlgo->shape());

  // Prepare the naming
  theConeAlgo->prepareNamingFaces();

  // Insert to faces
  int num = 1;
  std::map< std::string, std::shared_ptr<GeomAPI_Shape> > listOfFaces =
      theConeAlgo->getCreatedFaces();
  int nbFaces = 0;
  for (std::map< std::string, std::shared_ptr<GeomAPI_Shape> >::iterator
       it=listOfFaces.begin(); it!=listOfFaces.end(); ++it) {
    std::shared_ptr<GeomAPI_Shape> aFace = (*it).second;
    theResultCone->generated(aFace, (*it).first, num++);
    nbFaces++;
  }

  if (nbFaces == 2) {
    // Naming vertices
    GeomAPI_DataMapOfShapeShape aVertices;
    GeomAPI_ShapeExplorer aVertExp(theConeAlgo->shape(), GeomAPI_Shape::VERTEX);
    for(int anIndex = 1; aVertExp.more(); aVertExp.next()) {
      if (!aVertices.isBound(aVertExp.current())) {
        std::ostringstream aStream;
        aStream<<"Vertex_"<<anIndex++;
        theResultCone->generated(aVertExp.current(), aStream.str(), num++);
        aVertices.bind(aVertExp.current(), aVertExp.current());
      }
    }
  }
}
