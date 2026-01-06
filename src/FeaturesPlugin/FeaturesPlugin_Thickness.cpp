// Copyright (C) 2014-2026  CEA, EDF
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

#include "FeaturesPlugin_Thickness.h"

#include <ModelAPI_AttributeSelectionList.h>
#include <ModelAPI_AttributeString.h>
#include <ModelAPI_AttributeDouble.h>
#include <ModelAPI_AttributeBoolean.h>
#include <ModelAPI_ResultBody.h>
#include <ModelAPI_ResultConstruction.h>
#include <ModelAPI_Session.h>
#include <ModelAPI_Tools.h>
#include <ModelAPI_Validator.h>

#include <GeomAlgoAPI_Thickness.h>
#include <GeomAlgoAPI_Tools.h>

//==================================================================================================
FeaturesPlugin_Thickness::FeaturesPlugin_Thickness()
{
}

//==================================================================================================
void FeaturesPlugin_Thickness::initAttributes()
{
  data()->addAttribute(CREATION_METHOD_ID(), ModelAPI_AttributeString::typeId());
  data()->addAttribute(BASE_SHAPE_ID(), ModelAPI_AttributeSelection::typeId());
  data()->addAttribute(FACES_ID(), ModelAPI_AttributeSelectionList::typeId());
  data()->addAttribute(THICKNESS_VALUE_ID(), ModelAPI_AttributeDouble::typeId());
  data()->addAttribute(INSIDE_ID(), ModelAPI_AttributeBoolean::typeId());

  ModelAPI_Session::get()->validators()->registerNotObligatory(getKind(), FACES_ID());
}

//==================================================================================================
void FeaturesPlugin_Thickness::execute()
{
  // Getting creation method.
  std::string aCreationMethod = string(CREATION_METHOD_ID())->value();

  // Get base shape
  AttributeSelectionPtr aShapeAttrSelection = selection(BASE_SHAPE_ID());
  if (!aShapeAttrSelection.get()) {
    return;
  }
  GeomShapePtr aBaseShape = aShapeAttrSelection->value();
  if (!aBaseShape.get()) {
    return;
  }

  // Get thickness value.
  double aThickness = real(THICKNESS_VALUE_ID())->value();

  // Get thickening direction
  bool isInside = boolean(INSIDE_ID())->value();

  // Perform thickness
  std::shared_ptr<GeomAlgoAPI_Thickness> aThicknessAlgo;

  if (aCreationMethod == CREATION_METHOD_THICK()) {
    aThicknessAlgo = std::shared_ptr<GeomAlgoAPI_Thickness>
      (new GeomAlgoAPI_Thickness (aBaseShape, aThickness, isInside));
  }
  else {
    // hollowed solid algorithm has faces argument
    AttributeSelectionListPtr aFacesAttrList = selectionList(FACES_ID());
    if (!aShapeAttrSelection.get() || !aFacesAttrList.get()) {
      return;
    }

    ListOfShape aFaces;
    for (int anIndex = 0; anIndex < aFacesAttrList->size(); ++anIndex) {
      AttributeSelectionPtr aFaceAttrInList = aFacesAttrList->value(anIndex);
      GeomShapePtr aFace = aFaceAttrInList->value();
      if (!aFace.get()) {
        return;
      }
      aFaces.push_back(aFace);
    }

    aThicknessAlgo = std::shared_ptr<GeomAlgoAPI_Thickness>
      (new GeomAlgoAPI_Thickness (aBaseShape, aFaces, aThickness, isInside));
  }

  std::string anError;
  if (GeomAlgoAPI_Tools::AlgoError::isAlgorithmFailed(aThicknessAlgo, getKind(), anError)) {
    setError(anError);
    return;
  }

  GeomShapePtr aResult = aThicknessAlgo->shape();

  // Store result.
  int anIndex = 0;
  ResultBodyPtr aResultBody = document()->createBody(data(), anIndex);
  if (aCreationMethod == CREATION_METHOD_THICK()) {
    aResultBody->storeGenerated(aBaseShape, aResult);
  }
  else {
    aResultBody->storeModified(aBaseShape, aResult);
  }
  aResultBody->loadModifiedShapes(aThicknessAlgo, aBaseShape, GeomAPI_Shape::FACE);
  aResultBody->loadGeneratedShapes(aThicknessAlgo, aBaseShape, GeomAPI_Shape::FACE);
  aResultBody->loadGeneratedShapes(aThicknessAlgo, aBaseShape, GeomAPI_Shape::EDGE);
  aResultBody->loadGeneratedShapes(aThicknessAlgo, aBaseShape, GeomAPI_Shape::VERTEX);
  setResult(aResultBody, anIndex);
}

//==================================================================================================
void FeaturesPlugin_Thickness::attributeChanged(const std::string& theID)
{
  if (theID == CREATION_METHOD_ID()) {
    // clean base shape, as it has different type in different modes,
    // so, it cannot be the same
    AttributeSelectionPtr aBaseAttr = selection(BASE_SHAPE_ID());
    AttributeSelectionListPtr aFaceAttr = selectionList(FACES_ID());
    data()->blockSendAttributeUpdated(true, false);
    aBaseAttr->reset();
    aFaceAttr->clear(); // reset doesn't work for a list
    data()->blockSendAttributeUpdated(false, false);
  }
  ModelAPI_Feature::attributeChanged(theID);
}
