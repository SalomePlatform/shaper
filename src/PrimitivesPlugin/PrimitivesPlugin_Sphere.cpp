// Copyright (C) 2017-2020  CEA/DEN, EDF R&D
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
      ->objectByName(ModelAPI_ResultConstruction::group(), L"Origin");
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
