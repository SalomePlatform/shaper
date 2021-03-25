// Copyright (C) 2014-2021  CEA/DEN, EDF R&D
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

#include "FeaturesPlugin_Intersection.h"

#include <ModelAPI_Data.h>
#include <ModelAPI_Document.h>
#include <ModelAPI_BodyBuilder.h>
#include <ModelAPI_ResultBody.h>
#include <ModelAPI_AttributeSelectionList.h>

#include <GeomAlgoAPI_Intersection.h>
#include <GeomAlgoAPI_MakeShapeList.h>
#include <GeomAlgoAPI_Tools.h>
#include <GeomAPI_ShapeExplorer.h>

#include <sstream>

static const std::string INTERSECTION_VERSION_1("v9.5");

//=================================================================================================
FeaturesPlugin_Intersection::FeaturesPlugin_Intersection()
{
}

//=================================================================================================
void FeaturesPlugin_Intersection::initAttributes()
{
  AttributePtr anObjectsAttr = data()->addAttribute(OBJECT_LIST_ID(),
                       ModelAPI_AttributeSelectionList::typeId());

  initVersion(INTERSECTION_VERSION_1, anObjectsAttr, AttributePtr());
}

//=================================================================================================
void FeaturesPlugin_Intersection::execute()
{
  GeomAPI_ShapeHierarchy anObjectsHierarchy;
  ListOfShape aPlanes;
  // Getting objects.
  if (!processAttribute(OBJECT_LIST_ID(), anObjectsHierarchy, aPlanes)) {
    setError("Error: Objects or tools are empty.");
    return;
  }

  int aResultIndex = 0;

  // Create result.
  const ListOfShape& anObjects = anObjectsHierarchy.objects();
  GeomMakeShapePtr anIntersectionAlgo(new GeomAlgoAPI_Intersection(anObjects));

  // Checking that the algorithm worked properly.
  std::string anError;
  if (GeomAlgoAPI_Tools::AlgoError::isAlgorithmFailed(anIntersectionAlgo, getKind(), anError)) {
    setError(anError);
    return;
  }

  std::shared_ptr<GeomAlgoAPI_MakeShapeList> aMakeShapeList(new GeomAlgoAPI_MakeShapeList);
  aMakeShapeList->appendAlgo(anIntersectionAlgo);

  GeomShapePtr aResShape = anIntersectionAlgo->shape();
  if (data()->version() == INTERSECTION_VERSION_1) {
    // merge hierarchies of compounds containing objects and tools
    // and append the result of the FUSE operation
    aResShape = keepUnusedSubsOfCompound(aResShape, anObjectsHierarchy,
                                         GeomAPI_ShapeHierarchy(), aMakeShapeList);
  }

  std::shared_ptr<ModelAPI_ResultBody> aResultBody = document()->createBody(data(), aResultIndex);
  FeaturesPlugin_Tools::loadModifiedShapes(aResultBody,
                                           anObjects,
                                           ListOfShape(),
                                           aMakeShapeList,
                                           aResShape);
  setResult(aResultBody, aResultIndex);
  aResultIndex++;

  FeaturesPlugin_Tools::loadDeletedShapes(aResultBody,
                                          GeomShapePtr(),
                                          anObjects,
                                          aMakeShapeList,
                                          aResShape);

  // remove the rest results if there were produced in the previous pass
  removeResults(aResultIndex);
}
