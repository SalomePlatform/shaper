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

#include "FeaturesPlugin_Union.h"

#include <GeomAlgoAPI_Boolean.h>
#include <GeomAlgoAPI_MakeShapeList.h>
#include <GeomAlgoAPI_PaveFiller.h>

#include <GeomAPI_ShapeExplorer.h>

#include <ModelAPI_AttributeSelectionList.h>
#include <ModelAPI_ResultCompSolid.h>
#include <ModelAPI_Tools.h>

//=================================================================================================
FeaturesPlugin_Union::FeaturesPlugin_Union()
{
}

//=================================================================================================
void FeaturesPlugin_Union::initAttributes()
{
  data()->addAttribute(BASE_OBJECTS_ID(), ModelAPI_AttributeSelectionList::typeId());
}

//=================================================================================================
void FeaturesPlugin_Union::execute()
{
  ListOfShape anObjects;
  std::map<std::shared_ptr<GeomAPI_Shape>, ListOfShape> aCompSolidsObjects;

  // Getting objects.
  AttributeSelectionListPtr anObjectsSelList =
    selectionList(FeaturesPlugin_Union::BASE_OBJECTS_ID());
  for(int anObjectsIndex = 0; anObjectsIndex < anObjectsSelList->size(); anObjectsIndex++) {
    AttributeSelectionPtr anObjectAttr = anObjectsSelList->value(anObjectsIndex);
    std::shared_ptr<GeomAPI_Shape> anObject = anObjectAttr->value();
    if(!anObject.get()) {
      return;
    }
    ResultPtr aContext = anObjectAttr->context();
    ResultCompSolidPtr aResCompSolidPtr = ModelAPI_Tools::compSolidOwner(aContext);
    if(aResCompSolidPtr.get()) {
      std::shared_ptr<GeomAPI_Shape> aContextShape = aResCompSolidPtr->shape();
      std::map<std::shared_ptr<GeomAPI_Shape>, ListOfShape>::iterator
        anIt = aCompSolidsObjects.begin();
      for(; anIt != aCompSolidsObjects.end(); anIt++) {
        if(anIt->first->isEqual(aContextShape)) {
          aCompSolidsObjects[anIt->first].push_back(anObject);
          break;
        }
      }
      if(anIt == aCompSolidsObjects.end()) {
        aCompSolidsObjects[aContextShape].push_back(anObject);
      }
    } else {
      anObjects.push_back(anObject);
    }
  }

  // Collecting solids from compsolids which will not be modified in
  // boolean operation and will be added to result.
  ListOfShape aShapesToAdd;
  for(std::map<std::shared_ptr<GeomAPI_Shape>, ListOfShape>::iterator
    anIt = aCompSolidsObjects.begin();
    anIt != aCompSolidsObjects.end(); anIt++) {
    std::shared_ptr<GeomAPI_Shape> aCompSolid = anIt->first;
    ListOfShape& aUsedInOperationSolids = anIt->second;
    anObjects.insert(anObjects.end(), aUsedInOperationSolids.begin(), aUsedInOperationSolids.end());

    // Collect solids from compsolid which will not be modified in boolean operation.
    for(GeomAPI_ShapeExplorer anExp(aCompSolid, GeomAPI_Shape::SOLID); anExp.more(); anExp.next()) {
      std::shared_ptr<GeomAPI_Shape> aSolidInCompSolid = anExp.current();
      ListOfShape::iterator anIt = aUsedInOperationSolids.begin();
      for(; anIt != aUsedInOperationSolids.end(); anIt++) {
        if(aSolidInCompSolid->isEqual(*anIt)) {
          break;
        }
      }
      if(anIt == aUsedInOperationSolids.end()) {
        aShapesToAdd.push_back(aSolidInCompSolid);
      }
    }
  }

  if(anObjects.size() < 2) {
    setError("Error: Not enough objects for operation. Should be at least 2.");
    return;
  }

  // Fuse objects.
  ListOfShape aTools;
  aTools.splice(aTools.begin(), anObjects, anObjects.begin());
  std::shared_ptr<GeomAlgoAPI_Boolean> aFuseAlgo(new GeomAlgoAPI_Boolean(anObjects,
                                                            aTools,
                                                            GeomAlgoAPI_Boolean::BOOL_FUSE));

  // Checking that the algorithm worked properly.
  GeomAlgoAPI_MakeShapeList aMakeShapeList;
  GeomAPI_DataMapOfShapeShape aMapOfShapes;
  if(!aFuseAlgo->isDone()) {
    setError("Error: Boolean algorithm failed.");
    return;
  }
  if(aFuseAlgo->shape()->isNull()) {
    setError("Error: Resulting shape is Null.");
    return;
  }
  if(!aFuseAlgo->isValid()) {
    setError("Error: Resulting shape is not valid.");
    return;
  }

  GeomShapePtr aShape = aFuseAlgo->shape();
  aMakeShapeList.appendAlgo(aFuseAlgo);
  aMapOfShapes.merge(aFuseAlgo->mapOfSubShapes());

  // Store original shapes for naming.
  anObjects.splice(anObjects.begin(), aTools);
  anObjects.insert(anObjects.end(), aShapesToAdd.begin(), aShapesToAdd.end());

  // Combine result with not used solids from compsolid.
  if(aShapesToAdd.size() > 0) {
    aShapesToAdd.push_back(aShape);
    std::shared_ptr<GeomAlgoAPI_PaveFiller> aFillerAlgo(
      new GeomAlgoAPI_PaveFiller(aShapesToAdd, true));
    if(!aFillerAlgo->isDone()) {
      setError("Error: PaveFiller algorithm failed.");
      return;
    }
    if(aFillerAlgo->shape()->isNull()) {
      setError("Error: Resulting shape is Null.");
      return;
    }
    if(!aFillerAlgo->isValid()) {
      setError("Error: Resulting shape is not valid.");
      return;
    }

    aShape = aFillerAlgo->shape();
    aMakeShapeList.appendAlgo(aFillerAlgo);
    aMapOfShapes.merge(aFillerAlgo->mapOfSubShapes());
  }

  // Store result and naming.
  const int aModifyTag = 1;
  const int aDeletedTag = 2;
  /// sub solids will be placed at labels 3, 4, etc. if result is compound of solids
  const int aSubsolidsTag = 3;
  const std::string aModName = "Modified";

  std::shared_ptr<ModelAPI_ResultBody> aResultBody = document()->createBody(data());
  aResultBody->storeModified(anObjects.front(), aShape, aSubsolidsTag);

  for(ListOfShape::const_iterator anIter = anObjects.begin(); anIter != anObjects.end(); ++anIter) {
    aResultBody->loadAndOrientModifiedShapes(&aMakeShapeList, *anIter, GeomAPI_Shape::FACE,
                                             aModifyTag, aModName, aMapOfShapes);
    aResultBody->loadDeletedShapes(&aMakeShapeList, *anIter, GeomAPI_Shape::FACE, aDeletedTag);
  }

  setResult(aResultBody);
}
