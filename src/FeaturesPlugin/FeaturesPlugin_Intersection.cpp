// Copyright (C) 2014-2019  CEA/DEN, EDF R&D
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
#include <GeomAlgoAPI_Tools.h>
#include <GeomAPI_ShapeExplorer.h>

#include <sstream>

//=================================================================================================
FeaturesPlugin_Intersection::FeaturesPlugin_Intersection()
{
}

//=================================================================================================
void FeaturesPlugin_Intersection::initAttributes()
{
  data()->addAttribute(OBJECT_LIST_ID(),
                       ModelAPI_AttributeSelectionList::typeId());
}

//=================================================================================================
void FeaturesPlugin_Intersection::execute()
{
  ListOfShape anObjects;

  // Getting objects.
  AttributeSelectionListPtr anObjectsSelList = selectionList(OBJECT_LIST_ID());
  for (int anObjectsIndex = 0; anObjectsIndex < anObjectsSelList->size(); anObjectsIndex++) {
    std::shared_ptr<ModelAPI_AttributeSelection> anObjectAttr =
      anObjectsSelList->value(anObjectsIndex);
    std::shared_ptr<GeomAPI_Shape> anObject = anObjectAttr->value();
    if (!anObject.get()) {
      return;
    }
    anObjects.push_back(anObject);
  }

  if(anObjects.empty()) {
    setError("Error: Objects or tools are empty.");
    return;
  }

  int aResultIndex = 0;

  // Create result.
  GeomMakeShapePtr anIntersectionAlgo(new GeomAlgoAPI_Intersection(anObjects));

  // Checking that the algorithm worked properly.
  std::string anError;
  if (GeomAlgoAPI_Tools::AlgoError::isAlgorithmFailed(anIntersectionAlgo, getKind(), anError)) {
    setError(anError);
    return;
  }

  std::shared_ptr<ModelAPI_ResultBody> aResultBody = document()->createBody(data(), aResultIndex);
  loadNamingDS(aResultBody, anObjects, anIntersectionAlgo);
  setResult(aResultBody, aResultIndex);
  aResultIndex++;


  // remove the rest results if there were produced in the previous pass
  removeResults(aResultIndex);
}

//=================================================================================================
void FeaturesPlugin_Intersection::loadNamingDS(ResultBodyPtr theResultBody,
                                               const ListOfShape& theObjects,
                                               const GeomMakeShapePtr& theMakeShape)
{
  std::shared_ptr<GeomAPI_Shape> aResultShape = theMakeShape->shape();

  if(theObjects.front()->isEqual(aResultShape)) {
    theResultBody->store(aResultShape, false);
    return;
  }

  theResultBody->storeModified(theObjects.front(), aResultShape);

  const int aShapeTypesNb = 3;
  const GeomAPI_Shape::ShapeType aShapeTypes[aShapeTypesNb] = {GeomAPI_Shape::VERTEX,
                                                               GeomAPI_Shape::EDGE,
                                                               GeomAPI_Shape::FACE };
  for (ListOfShape::const_iterator anIt = theObjects.cbegin(); anIt != theObjects.cend(); ++anIt) {
    const GeomShapePtr aShape = *anIt;
    for(int anIndex = 0; anIndex < aShapeTypesNb; ++anIndex) {
      theResultBody->loadModifiedShapes(theMakeShape, aShape, aShapeTypes[anIndex]);
      theResultBody->loadGeneratedShapes(theMakeShape, aShape, aShapeTypes[anIndex]);
    }
  }
}
