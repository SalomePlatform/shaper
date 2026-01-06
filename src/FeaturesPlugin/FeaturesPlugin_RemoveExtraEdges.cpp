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

#include "FeaturesPlugin_RemoveExtraEdges.h"

#include <ModelAPI_AttributeBoolean.h>
#include <ModelAPI_AttributeSelection.h>
#include <ModelAPI_ResultBody.h>
#include <ModelAPI_ResultConstruction.h>
#include <ModelAPI_Session.h>
#include <ModelAPI_Validator.h>
#include <ModelAPI_Tools.h>

#include <GeomAPI_ShapeIterator.h>
#include <GeomAPI_ShapeExplorer.h>

#include <GeomAlgoAPI_Tools.h>
#include <GeomAlgoAPI_RemoveExtraEdges.h>


//==================================================================================================
FeaturesPlugin_RemoveExtraEdges::FeaturesPlugin_RemoveExtraEdges()
{
}

//==================================================================================================
void FeaturesPlugin_RemoveExtraEdges::initAttributes()
{
  data()->addAttribute(BASE_SHAPE_ID(), ModelAPI_AttributeSelection::typeId());
  data()->addAttribute(UNITE_FACES_ID(), ModelAPI_AttributeBoolean::typeId());
}

//==================================================================================================
void FeaturesPlugin_RemoveExtraEdges::execute()
{
  // Get selection
  AttributeSelectionPtr aShapeAttrSelection = selection(BASE_SHAPE_ID());
  if (!aShapeAttrSelection.get()) {
    return;
  }

  // Get arguments
  GeomShapePtr aBaseShape = aShapeAttrSelection->value();
  bool doUniteFaces = boolean(UNITE_FACES_ID())->value();

  // Make fusion
  std::shared_ptr<GeomAlgoAPI_RemoveExtraEdges> anAlgo
    (new GeomAlgoAPI_RemoveExtraEdges(aBaseShape, doUniteFaces));

  // Check algo status
  std::string anError;
  if (GeomAlgoAPI_Tools::AlgoError::isAlgorithmFailed(anAlgo, getKind(), anError)) {
    setError(anError);
    return;
  }

  // Store result
  GeomShapePtr aResultShape = anAlgo->shape();
  ResultBodyPtr aResultBody = document()->createBody(data());

  ListOfShape aBaseShapesList;
  aBaseShapesList.push_back(aBaseShape);
  ModelAPI_Tools::loadModifiedShapes(aResultBody, aBaseShapesList, ListOfShape(),
                                     anAlgo, aResultShape);
  setResult(aResultBody);
}
