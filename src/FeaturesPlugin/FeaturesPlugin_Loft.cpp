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

#include "FeaturesPlugin_Loft.h"
#include "FeaturesPlugin_Tools.h"

#include <ModelAPI_AttributeSelection.h>

#include <GeomAlgoAPI_Copy.h>
#include <GeomAlgoAPI_Filling.h>
#include <GeomAlgoAPI_ShapeTools.h>
#include <GeomAlgoAPI_Tools.h>
#include <GeomAPI_Pnt.h>
#include <GeomAPI_ShapeExplorer.h>

#include <cmath>

//==================================================================================================
FeaturesPlugin_Loft::FeaturesPlugin_Loft()
{
}

//==================================================================================================
void FeaturesPlugin_Loft::initAttributes()
{
  data()->addAttribute(FIRST_OBJECT_ID(), ModelAPI_AttributeSelection::typeId());
  data()->addAttribute(SECOND_OBJECT_ID(), ModelAPI_AttributeSelection::typeId());
}

//==================================================================================================
void FeaturesPlugin_Loft::execute()
{
  AttributeSelectionPtr aFirstSelection = selection(FIRST_OBJECT_ID());
  AttributeSelectionPtr aSecondelection = selection(SECOND_OBJECT_ID());

  if (aFirstSelection->isInitialized() && aSecondelection->isInitialized()) {

    GeomShapePtr aFirstShape, aSecondShape;

    if (aFirstSelection && aSecondelection) {
      aFirstShape = aFirstSelection->value();
      if (!aFirstShape && aFirstSelection->context())
        aFirstShape = aFirstSelection->context()->shape();

      aSecondShape = aSecondelection->value();
      if (!aSecondShape && aSecondelection->context())
        aSecondShape = aSecondelection->context()->shape();
    }

    std::string anError;
    std::shared_ptr<GeomAlgoAPI_Loft> aLoftAlgo(new GeomAlgoAPI_Loft(aFirstShape, aSecondShape));

    if (GeomAlgoAPI_Tools::AlgoError::isAlgorithmFailed(aLoftAlgo, getKind(), anError)) {
      setError(anError);
      return;
    }
    ListOfShape  theBoundaryShapes;
    theBoundaryShapes.push_back(aFirstShape);
    theBoundaryShapes.push_back(aSecondShape);

    // Create result body.
    ResultBodyPtr aResultBody = document()->createBody(data());

    aResultBody->store(aLoftAlgo->shape());
    // store Faces
    for(GeomAPI_ShapeExplorer anExp(aLoftAlgo->shape(), GeomAPI_Shape::FACE);
                              anExp.more(); anExp.next()) {
      GeomShapePtr anEdge = anExp.current();
      aResultBody->generated(anEdge, "Loft_Face");
    }
    setResult(aResultBody, 0);
  }
}
