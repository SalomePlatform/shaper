// Copyright (C) 2014-2022  CEA/DEN, EDF R&D
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

#include "BuildPlugin_Shell.h"

#include <ModelAPI_AttributeSelectionList.h>
#include <ModelAPI_ResultBody.h>
#include <ModelAPI_ResultConstruction.h>

#include <GeomAlgoAPI_Sewing.h>
#include <GeomAlgoAPI_Tools.h>
#include <GeomAPI_ShapeExplorer.h>

//=================================================================================================
BuildPlugin_Shell::BuildPlugin_Shell()
{
}

//=================================================================================================
void BuildPlugin_Shell::initAttributes()
{
  data()->addAttribute(BASE_OBJECTS_ID(), ModelAPI_AttributeSelectionList::typeId());
}

//=================================================================================================
void BuildPlugin_Shell::execute()
{
  // Get base objects list.
  AttributeSelectionListPtr aSelectionList = selectionList(BASE_OBJECTS_ID());

  // Collect base shapes.
  ListOfShape aShapes, aContexts;
  getOriginalShapesAndContexts(BASE_OBJECTS_ID(), aShapes, aContexts);

  // Collect sketch faces.
  for (int anIndex = 0; anIndex < aSelectionList->size(); ++anIndex) {
    AttributeSelectionPtr aSelection = aSelectionList->value(anIndex);
    GeomShapePtr aShape = aSelection->value();
    ResultConstructionPtr aContext =
        std::dynamic_pointer_cast<ModelAPI_ResultConstruction>(aSelection->context());
    if (!aShape && aContext) {
      for (int i = 0; i < aContext->facesNum(); ++i)
        aShapes.push_back(aContext->face(i));
    }
  }

  // Sew faces.
  GeomMakeShapePtr aSewingAlgo(new GeomAlgoAPI_Sewing(aShapes));

  std::string anError;
  if (GeomAlgoAPI_Tools::AlgoError::isAlgorithmFailed(aSewingAlgo, getKind(), anError)) {
    setError(anError);
    return;
  }

  // Store result.
  GeomShapePtr aResult = aSewingAlgo->shape();

  int anIndex = 0;
  for(GeomAPI_ShapeExplorer anExp(aResult, GeomAPI_Shape::SHELL); anExp.more(); anExp.next()) {
    GeomShapePtr aShell = anExp.current();
    storeResult(aSewingAlgo, aShapes, aContexts, aShell, anIndex++);
  }

  removeResults(anIndex);
}
