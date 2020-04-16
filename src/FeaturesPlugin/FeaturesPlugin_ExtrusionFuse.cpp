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

#include "FeaturesPlugin_ExtrusionFuse.h"

#include <ModelAPI_AttributeString.h>
#include <ModelAPI_AttributeSelectionList.h>

#include <GeomAlgoAPI_Boolean.h>
#include <GeomAlgoAPI_Prism.h>
#include <GeomAlgoAPI_ThroughAll.h>
#include <GeomAlgoAPI_CompoundBuilder.h>

//=================================================================================================
FeaturesPlugin_ExtrusionFuse::FeaturesPlugin_ExtrusionFuse()
{
  myFeature = this;
  myOperationType = BOOL_FUSE;
}

//=================================================================================================
void FeaturesPlugin_ExtrusionFuse::execute()
{
  if (string(CREATION_METHOD())->value() != CREATION_METHOD_THROUGH_ALL())
    executeCompositeBoolean();
  else {
    executeFuseThroughAll();
  }
}

//=================================================================================================
void FeaturesPlugin_ExtrusionFuse::executeFuseThroughAll()
{
  // Getting objects.
  ListOfShape anObjects;
  AttributeSelectionListPtr anObjectsSelList = myFeature->selectionList(OBJECTS_ID());
  for (int anObjectsIndex = 0; anObjectsIndex < anObjectsSelList->size(); anObjectsIndex++) {
    AttributeSelectionPtr anObjectAttr = anObjectsSelList->value(anObjectsIndex);
    GeomShapePtr anObject = anObjectAttr->value();
    if (!anObject.get()) {
      myFeature->setError("Error: Could not get object.");
      return;
    }
    anObjects.push_back(anObject);
  }

  // Make generation.
  ListOfShape aGenBaseShapes;
  ListOfMakeShape aGenMakeShapes;
  if (!makeGeneration(aGenBaseShapes, aGenMakeShapes)) {
    return;
  }

  // Getting tools.
  ListOfShape aNewTools;
  ListOfMakeShape aToolsMakeShapes;
  for (ListOfMakeShape::const_iterator
         anIt = aGenMakeShapes.cbegin(); anIt != aGenMakeShapes.cend(); ++anIt) {
    GeomMakeShapePtr anAlgo = (*anIt);
    std::shared_ptr<GeomAlgoAPI_Prism> aPrismAlgo =
        std::dynamic_pointer_cast<GeomAlgoAPI_Prism>(anAlgo);

    // Cut the prism by all objects and throw away end pieces
    std::shared_ptr<GeomAlgoAPI_ThroughAll> aToolAlgo (
        new GeomAlgoAPI_ThroughAll(aPrismAlgo, anObjects));

    // Checking that the algorithm worked properly
    if (!aToolAlgo->isDone() || aToolAlgo->shape()->isNull() || !aToolAlgo->isValid()) {
      myFeature->setError("Error: ThroughAll algorithm failed.");
    } else {
      GeomShapePtr aCuttedTool = aToolAlgo->shape();
      aNewTools.push_back(aCuttedTool);
      aToolsMakeShapes.push_back(aToolAlgo);
    }
  }

  // Perform FeaturesPlugin_CompositeBoolean::makeBoolean() with new (cutted) tools
  ListOfShape aBooleanObjects;
  ListOfMakeShape aBooleanMakeShapes;
  if (!makeBoolean(aNewTools, aBooleanObjects, aBooleanMakeShapes)) {
    return;
  }

  if (myOperationType == BOOL_FUSE) {
    aNewTools.splice(aNewTools.begin(), aBooleanObjects);
    aBooleanObjects.splice(aBooleanObjects.begin(), aNewTools, aNewTools.begin());
  }

  // 4. Store result (like in FeaturesPlugin_CompositeBoolean::executeCompositeBoolean())
  int aResultIndex = 0;
  std::vector<ResultBaseAlgo> aResultBaseAlgoList;
  ListOfShape aResultShapesList;
  ListOfShape::const_iterator aBoolObjIt = aBooleanObjects.cbegin();
  ListOfMakeShape::const_iterator aBoolMSIt = aBooleanMakeShapes.cbegin();
  for(; aBoolObjIt != aBooleanObjects.cend() && aBoolMSIt != aBooleanMakeShapes.cend();
      ++aBoolObjIt, ++aBoolMSIt) {

    ResultBodyPtr aResultBody = myFeature->document()->createBody(myFeature->data(), aResultIndex);

    if((*aBoolObjIt)->isEqual((*aBoolMSIt)->shape())) {
      aResultBody->store((*aBoolMSIt)->shape(), false);
    }
    else
    {
      aResultBody->storeModified(*aBoolObjIt, (*aBoolMSIt)->shape());

      // Store generation history.
      ListOfShape::const_iterator aGenBaseIt = aGenBaseShapes.cbegin();
      ListOfMakeShape::const_iterator aGenMSIt = aGenMakeShapes.cbegin();
      for(; aGenBaseIt != aGenBaseShapes.cend() && aGenMSIt != aGenMakeShapes.cend();
          ++aGenBaseIt, ++aGenMSIt) {

        // ???
        ListOfMakeShape::const_iterator aToolsMSIt = aToolsMakeShapes.cbegin();
        for(; aToolsMSIt != aToolsMakeShapes.cend(); ++aToolsMSIt) {
          std::shared_ptr<GeomAlgoAPI_MakeShapeList> aMSList(new GeomAlgoAPI_MakeShapeList());

          // prism generation
          aMSList->appendAlgo(*aGenMSIt);

          // tool modification (cut by objects)
          aMSList->appendAlgo(*aToolsMSIt);

          // bool fuse
          aMSList->appendAlgo(*aBoolMSIt);
          storeGenerationHistory(aResultBody, *aGenBaseIt, aMSList);
        }
      }

      storeModificationHistory(aResultBody, *aBoolObjIt, aNewTools, *aBoolMSIt);

      ResultBaseAlgo aRBA;
      aRBA.resultBody = aResultBody;
      aRBA.baseShape = *aBoolObjIt;
      aRBA.makeShape = *aBoolMSIt;
      aResultBaseAlgoList.push_back(aRBA);
      aResultShapesList.push_back((*aBoolMSIt)->shape());
    }

    myFeature->setResult(aResultBody, aResultIndex++);
  }

  // Store deleted shapes after all results has been proceeded. This is to avoid issue when in one
  // result shape has been deleted, but in another it was modified or stayed.
  GeomShapePtr aResultShapesCompound = GeomAlgoAPI_CompoundBuilder::compound(aResultShapesList);
  storeDeletedShapes(aResultBaseAlgoList, aNewTools, aResultShapesCompound);

  myFeature->removeResults(aResultIndex);
}
