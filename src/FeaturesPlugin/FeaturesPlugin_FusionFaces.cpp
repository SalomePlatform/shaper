// Copyright (C) 2014-2017  CEA/DEN, EDF R&D
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
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
//
// See http://www.salome-platform.org/ or
// email : webmaster.salome@opencascade.com<mailto:webmaster.salome@opencascade.com>
//

#include "FeaturesPlugin_FusionFaces.h"

#include <ModelAPI_AttributeSelectionList.h>
#include <ModelAPI_AttributeString.h>
#include <ModelAPI_ResultBody.h>
#include <ModelAPI_ResultCompSolid.h>
#include <ModelAPI_ResultConstruction.h>
#include <ModelAPI_Session.h>
#include <ModelAPI_Validator.h>

#include <GeomAPI_ShapeIterator.h>
#include <GeomAPI_ShapeExplorer.h>

#include <GeomAlgoAPI_UnifySameDomain.h>


//==================================================================================================
FeaturesPlugin_FusionFaces::FeaturesPlugin_FusionFaces()
{
}

//==================================================================================================
void FeaturesPlugin_FusionFaces::initAttributes()
{
  data()->addAttribute(BASE_SHAPE_ID(), ModelAPI_AttributeSelection::typeId());
}

//==================================================================================================
void FeaturesPlugin_FusionFaces::execute()
{
  // Get selection.
  AttributeSelectionPtr aShapeAttrSelection = selection(BASE_SHAPE_ID());
  if (!aShapeAttrSelection.get()) {
    return;
  }

  // Get shape.
  GeomShapePtr aBaseShape = aShapeAttrSelection->value();

  // Make fusion
  std::shared_ptr<GeomAlgoAPI_UnifySameDomain> anAlgo(new GeomAlgoAPI_UnifySameDomain(aBaseShape));

  // Check algo status
  if (!anAlgo->isDone()) {
    setError("Error: Fusion algorithm failed.");
    return;
  }
  if (anAlgo->shape()->isNull()) {
    setError("Error: Resulting shape is Null.");
    return;
  }
  if (!anAlgo->isValid()) {
    setError("Error: Resulting shape is not valid.");
    return;
  }

  // Store result
  GeomShapePtr aResultShape = anAlgo->shape();
  ResultBodyPtr aResultBody = document()->createBody(data());
  if (aResultShape->isEqual(aBaseShape)) {
    aResultBody->store(aResultShape);
  } else {
    aResultBody->storeModified(aBaseShape, aResultShape);

    const int aModifyEdgeTag = 1;
    const int aModifyFaceTag = 2;
    const std::string aModEName = "Modified_Edge";
    const std::string aModFName = "Modified_Face";
    std::shared_ptr<GeomAPI_DataMapOfShapeShape> aMapOfShapes = anAlgo->mapOfSubShapes();
    aResultBody->loadAndOrientModifiedShapes(anAlgo.get(), aBaseShape, GeomAPI_Shape::EDGE,
      aModifyEdgeTag, aModEName, *aMapOfShapes.get(), true);
    aResultBody->loadAndOrientModifiedShapes(anAlgo.get(), aBaseShape, GeomAPI_Shape::FACE,
      aModifyFaceTag, aModFName, *aMapOfShapes.get(), true);
  }
  setResult(aResultBody);
}
