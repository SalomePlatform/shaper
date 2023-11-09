// Copyright (C) 2017-2023  CEA, EDF
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

// File:        PrimitivesPlugin_Sphere.cpp
// Created:     15 Mar 2017
// Author:      Clarisse Genrault (CEA)

#include <PrimitivesPlugin_Sphere.h>

#include <GeomAPI_ShapeExplorer.h>

#include <GeomAlgoAPI_PointBuilder.h>

#include <ModelAPI_AttributeDouble.h>
#include <ModelAPI_AttributeSelection.h>
#include <ModelAPI_AttributeString.h>
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
  // Attention! A fix for 37570 Tuleap issue.
  // We could have studies with aCenterPoint at the first position (old studies)
  // and studies with CREATION_METHOD() at the first position (new studies)

  // data for the first mode : by a point and a radius
  AttributeSelectionPtr aCenterPoint = std::dynamic_pointer_cast<ModelAPI_AttributeSelection>
    (data()->addAttribute(PrimitivesPlugin_Sphere::CENTER_POINT_ID(),
                          ModelAPI_AttributeSelection::typeId())); // #1 (old studies)
  if (aCenterPoint->isInitialized()) {
    // we are opening an old study:
    // #1 Center Point
    // #2 Radius
    // #3 Creation method (initialize now)

    data()->addAttribute(PrimitivesPlugin_Sphere::RADIUS_ID(),
                         ModelAPI_AttributeDouble::typeId()); // #2 (old studies)

    AttributeStringPtr aMethod = std::dynamic_pointer_cast<ModelAPI_AttributeString>
      (data()->addAttribute(PrimitivesPlugin_Sphere::CREATION_METHOD(),
                            ModelAPI_AttributeString::typeId())); // #3
    aMethod->setValue(CREATION_METHOD_BY_PT_RADIUS());
  }
  else {
    // we are opening a new study or creating Sphere from scratch
    // #1 Creation method
    // #2 Center Point
    // #3 Radius

    data()->addAttribute(PrimitivesPlugin_Sphere::CREATION_METHOD(),
                         ModelAPI_AttributeString::typeId(),
                         1); // #1 new studies or from scratch

    aCenterPoint = std::dynamic_pointer_cast<ModelAPI_AttributeSelection>
      (data()->addAttribute(PrimitivesPlugin_Sphere::CENTER_POINT_ID(),
                            ModelAPI_AttributeSelection::typeId(),
                            2)); // #2

    // Initialize the center point of the sphere at the origin if the center point is not filled.
    if (!aCenterPoint->isInitialized()) {
      ObjectPtr aPointObj = ModelAPI_Session::get()->moduleDocument()
        ->objectByName(ModelAPI_ResultConstruction::group(), L"Origin");
      if (aPointObj.get()) {
        ResultPtr aPointRes = std::dynamic_pointer_cast<ModelAPI_Result>(aPointObj);
        aCenterPoint->setValue(aPointRes, std::shared_ptr<GeomAPI_Shape>());
      }
    }

    data()->addAttribute(PrimitivesPlugin_Sphere::RADIUS_ID(),
                         ModelAPI_AttributeDouble::typeId(),
                         3); // #3
  }

  // data for the second mode : by dimensions
  data()->addAttribute(PrimitivesPlugin_Sphere::RMIN_ID(), ModelAPI_AttributeDouble::typeId());
  data()->addAttribute(PrimitivesPlugin_Sphere::RMAX_ID(), ModelAPI_AttributeDouble::typeId());
  data()->addAttribute(PrimitivesPlugin_Sphere::PHIMIN_ID(), ModelAPI_AttributeDouble::typeId());
  data()->addAttribute(PrimitivesPlugin_Sphere::PHIMAX_ID(), ModelAPI_AttributeDouble::typeId());
  data()->addAttribute(PrimitivesPlugin_Sphere::THETAMIN_ID(), ModelAPI_AttributeDouble::typeId());
  data()->addAttribute(PrimitivesPlugin_Sphere::THETAMAX_ID(), ModelAPI_AttributeDouble::typeId());
}

//=================================================================================================
void PrimitivesPlugin_Sphere::execute()
{
  AttributeStringPtr aMethodTypeAttr = string(PrimitivesPlugin_Sphere::CREATION_METHOD());
  std::string aMethodType = aMethodTypeAttr->value();

  if (aMethodType == CREATION_METHOD_BY_PT_RADIUS())
    createSphereByPtRadius();

  if (aMethodType == CREATION_METHOD_BY_DIMENSIONS())
    createShereByDimensions();
}


//=================================================================================================
void PrimitivesPlugin_Sphere::createSphereByPtRadius()
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

  // Check if the creation of the sphere is OK
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
void PrimitivesPlugin_Sphere::createShereByDimensions()
{
  // Getting rmin, rmax, phimin, phimax, thetamin et thetamax
  double aRMin = real(PrimitivesPlugin_Sphere::RMIN_ID())->value();
  double aRMax = real(PrimitivesPlugin_Sphere::RMAX_ID())->value();
  double aPhiMin = real(PrimitivesPlugin_Sphere::PHIMIN_ID())->value();
  double aPhiMax = real(PrimitivesPlugin_Sphere::PHIMAX_ID())->value();
  double aThetaMin = real(PrimitivesPlugin_Sphere::THETAMIN_ID())->value();
  double aThetaMax = real(PrimitivesPlugin_Sphere::THETAMAX_ID())->value();

  std::shared_ptr<GeomAlgoAPI_Sphere> aSphereAlgo = std::shared_ptr<GeomAlgoAPI_Sphere>(
      new GeomAlgoAPI_Sphere(aRMin, aRMax, aPhiMin, aPhiMax, aThetaMin, aThetaMax));

  // These checks should be made to the GUI for the feature but
  // the corresponding validator does not exist yet.
  if (!aSphereAlgo->check()) {
    setError(aSphereAlgo->getError());
    return;
  }

  // Build the sphere
  aSphereAlgo->build();

  // Check if the creation of the sphere is OK
  if(!aSphereAlgo->isDone()) {
    // The error is not displayed in a popup window. It must be in the message console.
    setError(aSphereAlgo->getError());
    return;
  }
  if(!aSphereAlgo->checkValid("Sphere Builder")) {
    // The error is not displayed in a popup window. It must be in the message console.
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
  std::map< std::string, std::shared_ptr<GeomAPI_Shape> > listOfFaces =
      theSphereAlgo->getCreatedFaces();
  for (std::map< std::string, std::shared_ptr<GeomAPI_Shape> >::iterator it = listOfFaces.begin();
       it != listOfFaces.end();
       ++it)
  {
    theResultSphere->generated((*it).second, (*it).first);
  }

  // Naming vertices
  GeomAPI_DataMapOfShapeShape aVertices;
  int anIndex = 1;
  for (GeomAPI_ShapeExplorer aVertExp(theSphereAlgo->shape(), GeomAPI_Shape::VERTEX);
       aVertExp.more();
       aVertExp.next())
  {
    if (!aVertices.isBound(aVertExp.current())) {
      std::ostringstream aStream;
      aStream<<"Vertex_"<<anIndex++;
      theResultSphere->generated(aVertExp.current(), aStream.str());
      aVertices.bind(aVertExp.current(), aVertExp.current());
    }
  }
}
