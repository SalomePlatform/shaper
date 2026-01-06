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

#include "FeaturesPlugin_Offset.h"

#include <ModelAPI_AttributeSelectionList.h>
#include <ModelAPI_AttributeString.h>
#include <ModelAPI_AttributeDouble.h>
#include <ModelAPI_AttributeBoolean.h>
#include <ModelAPI_ResultBody.h>
#include <ModelAPI_ResultConstruction.h>
#include <ModelAPI_Session.h>
#include <ModelAPI_Tools.h>
#include <ModelAPI_Validator.h>

#include <GeomAlgoAPI_Offset.h>
#include <GeomAlgoAPI_Tools.h>

//==================================================================================================
FeaturesPlugin_Offset::FeaturesPlugin_Offset()
{
}

//==================================================================================================
void FeaturesPlugin_Offset::initAttributes()
{
  data()->addAttribute(CREATION_METHOD_ID(), ModelAPI_AttributeString::typeId());
  data()->addAttribute(BASE_SHAPE_ID(), ModelAPI_AttributeSelection::typeId());
  data()->addAttribute(OFFSET_VALUE_ID(), ModelAPI_AttributeDouble::typeId());
  data()->addAttribute(PIPE_JOINT_ID(), ModelAPI_AttributeBoolean::typeId());
  data()->addAttribute(FACES_ID(), ModelAPI_AttributeSelectionList::typeId());

  ModelAPI_Session::get()->validators()->registerNotObligatory(getKind(), PIPE_JOINT_ID());
  ModelAPI_Session::get()->validators()->registerNotObligatory(getKind(), FACES_ID());
}

//==================================================================================================
void FeaturesPlugin_Offset::execute()
{
  // Get base shape and sub-shapes list.
  AttributeSelectionPtr aShapeAttrSelection = selection(BASE_SHAPE_ID());
  if (!aShapeAttrSelection.get()) {
    return;
  }

  // Get base shape.
  GeomShapePtr aBaseShape = aShapeAttrSelection->value();
  if (!aBaseShape.get()) {
    return;
  }

  // Get offset value.
  double anOffset = real(OFFSET_VALUE_ID())->value();

  // Getting creation method.
  std::string aCreationMethod = string(CREATION_METHOD_ID())->value();

  // Perform total or partial offset
  std::shared_ptr<GeomAlgoAPI_Offset> anOffsetAlgo;

  if (aCreationMethod == CREATION_METHOD_EQUAL()) {
    // total offset can have pipe or intersection joints
    bool isPipeJoint = boolean(PIPE_JOINT_ID())->value();

    anOffsetAlgo = std::shared_ptr<GeomAlgoAPI_Offset>
      (new GeomAlgoAPI_Offset (aBaseShape, anOffset, isPipeJoint));
  }
  else {
    // partial offset has faces argument
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

    anOffsetAlgo = std::shared_ptr<GeomAlgoAPI_Offset>
      (new GeomAlgoAPI_Offset (aBaseShape, aFaces, anOffset));
  }

  std::string anError;
  if (GeomAlgoAPI_Tools::AlgoError::isAlgorithmFailed(anOffsetAlgo, getKind(), anError)) {
    setError(anError);
    return;
  }

  GeomShapePtr aResult = anOffsetAlgo->shape();

  // Store result.
  int anIndex = 0;
  ResultBodyPtr aResultBody = document()->createBody(data(), anIndex);
  aResultBody->storeModified(aBaseShape, aResult);
  aResultBody->loadModifiedShapes(anOffsetAlgo, aBaseShape, GeomAPI_Shape::FACE);
  aResultBody->loadGeneratedShapes(anOffsetAlgo, aBaseShape, GeomAPI_Shape::FACE);
  setResult(aResultBody, anIndex);
}
