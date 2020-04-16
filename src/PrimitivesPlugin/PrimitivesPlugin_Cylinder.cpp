// Copyright (C) 2014-2020  CEA/DEN, EDF R&D
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

#include <PrimitivesPlugin_Cylinder.h>

#include <GeomAPI_Dir.h>
#include <GeomAPI_Edge.h>
#include <GeomAPI_Lin.h>
#include <GeomAPI_ShapeIterator.h>

#include <GeomAlgoAPI_PointBuilder.h>

#include <ModelAPI_AttributeDouble.h>
#include <ModelAPI_AttributeSelection.h>
#include <ModelAPI_AttributeString.h>
#include <ModelAPI_ResultBody.h>
#include <ModelAPI_ResultConstruction.h>
#include <ModelAPI_Session.h>

//=================================================================================================
PrimitivesPlugin_Cylinder::PrimitivesPlugin_Cylinder()
{
}

//=================================================================================================
void PrimitivesPlugin_Cylinder::initAttributes()
{
  data()->addAttribute(PrimitivesPlugin_Cylinder::CREATION_METHOD(),
                       ModelAPI_AttributeString::typeId());

  data()->addAttribute(PrimitivesPlugin_Cylinder::BASE_POINT_ID(),
                       ModelAPI_AttributeSelection::typeId());
  data()->addAttribute(PrimitivesPlugin_Cylinder::AXIS_ID(),
                       ModelAPI_AttributeSelection::typeId());

  data()->addAttribute(PrimitivesPlugin_Cylinder::RADIUS_ID(),
                       ModelAPI_AttributeDouble::typeId());
  data()->addAttribute(PrimitivesPlugin_Cylinder::HEIGHT_ID(),
                       ModelAPI_AttributeDouble::typeId());
  data()->addAttribute(PrimitivesPlugin_Cylinder::ANGLE_ID(),
                       ModelAPI_AttributeDouble::typeId());

  // Initialize the base point of the cylinder at the origin if the base point is not filled.
  AttributeSelectionPtr aBasePoint = data()->selection(BASE_POINT_ID());
  if (!aBasePoint->isInitialized()) {
    ObjectPtr aPointObj = ModelAPI_Session::get()->moduleDocument()
      ->objectByName(ModelAPI_ResultConstruction::group(), "Origin");
    if (aPointObj.get()) {
      ResultPtr aPointRes = std::dynamic_pointer_cast<ModelAPI_Result>(aPointObj);
      aBasePoint->setValue(aPointRes, std::shared_ptr<GeomAPI_Shape>());
    }
  }

  // Initialize the axis at the OZ axis if the axis is not filled.
  AttributeSelectionPtr anAxis = data()->selection(AXIS_ID());
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
void PrimitivesPlugin_Cylinder::execute()
{
  AttributeStringPtr aMethodTypeAttr = string(PrimitivesPlugin_Cylinder::CREATION_METHOD());
  std::string aMethodType = aMethodTypeAttr->value();

  if (aMethodType == CREATION_METHOD_CYLINDER()) {
    createCylinder(false);
  }

  if (aMethodType == CREATION_METHOD_CYLINDER_PORTION()) {
    createCylinder(true);
  }
}

//=================================================================================================
void PrimitivesPlugin_Cylinder::createCylinder(bool withAngle)
{
  // Getting point.
  std::shared_ptr<GeomAPI_Pnt> aBasePoint;
  std::shared_ptr<ModelAPI_AttributeSelection> aPointRef =
    selection(PrimitivesPlugin_Cylinder::BASE_POINT_ID());
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
  static const std::string aSelectionError = "Error: The axis shape selection is bad.";
  std::shared_ptr<ModelAPI_AttributeSelection> anEdgeRef = selection(AXIS_ID());
  GeomShapePtr aShape = anEdgeRef->value();
  if (!aShape.get()) {
    if (anEdgeRef->context().get()) {
      aShape = anEdgeRef->context()->shape();
    }
  }
  if (!aShape.get()) {
    setError(aSelectionError);
    return;
  }
  std::shared_ptr<GeomAPI_Edge> anEdge;
  if (aShape->isEdge())
  {
    anEdge = aShape->edge();
  }
  else if (aShape->isCompound())
  {
    GeomAPI_ShapeIterator anIt(aShape);
    anEdge = anIt.current()->edge();
  }
  else
  {
    setError(aSelectionError);
    return;
  }

  if (!anEdge.get())
  {
    setError(aSelectionError);
    return;
  }

  std::shared_ptr<GeomAPI_Ax2> anAxis(new GeomAPI_Ax2(aBasePoint,
                                                      anEdge->line()->direction()));


  // Getting radius and height
  double aRadius = real(PrimitivesPlugin_Cylinder::RADIUS_ID())->value();
  double aHeight = real(PrimitivesPlugin_Cylinder::HEIGHT_ID())->value();

  std::shared_ptr<GeomAlgoAPI_Cylinder> aCylinderAlgo;
  if (withAngle) {
    // Getting angle
    double anAngle = real(PrimitivesPlugin_Cylinder::ANGLE_ID())->value();
    aCylinderAlgo =
      std::shared_ptr<GeomAlgoAPI_Cylinder>(new GeomAlgoAPI_Cylinder(anAxis,
                                                                     aRadius, aHeight,
                                                                     anAngle));
  } else {
    aCylinderAlgo =
      std::shared_ptr<GeomAlgoAPI_Cylinder>(new GeomAlgoAPI_Cylinder(anAxis,
                                                                     aRadius, aHeight));
  }

  // These checks should be made to the GUI for the feature but
  // the corresponding validator does not exist yet.
  if (!aCylinderAlgo->check()) {
    setError(aCylinderAlgo->getError());
    return;
  }

  // Build the cylinder
  aCylinderAlgo->build();

  // Check if the creation of the cylinder
  if(!aCylinderAlgo->isDone()) {
    // The error is not displayed in a popup window. It must be in the message console.
    setError(aCylinderAlgo->getError());
    return;
  }
  if(!aCylinderAlgo->checkValid("Cylinder builder")) {
    // The error is not displayed in a popup window. It must be in the message console.
    setError(aCylinderAlgo->getError());
    return;
  }

  int aResultIndex = 0;
  ResultBodyPtr aResultBox = document()->createBody(data(), aResultIndex);
  loadNamingDS(aCylinderAlgo, aResultBox);
  setResult(aResultBox, aResultIndex);
}

//=================================================================================================
void PrimitivesPlugin_Cylinder::loadNamingDS(std::shared_ptr<GeomAlgoAPI_Cylinder> theCylinderAlgo,
                                             std::shared_ptr<ModelAPI_ResultBody> theResultCylinder)
{
  // Load the result
  theResultCylinder->store(theCylinderAlgo->shape());

  // Prepare the naming
  theCylinderAlgo->prepareNamingFaces();

  // Insert to faces
  std::map< std::string, std::shared_ptr<GeomAPI_Shape> > listOfFaces =
    theCylinderAlgo->getCreatedFaces();
  for (std::map< std::string, std::shared_ptr<GeomAPI_Shape> >::iterator it = listOfFaces.begin();
       it != listOfFaces.end();
       ++it)
  {
    theResultCylinder->generated((*it).second, (*it).first);
  }
}
