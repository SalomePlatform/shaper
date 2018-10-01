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

#include "FeaturesPlugin_RemoveSubShapes.h"

#include <ModelAPI_AttributeSelectionList.h>
#include <ModelAPI_AttributeString.h>
#include <ModelAPI_ResultBody.h>
#include <ModelAPI_ResultConstruction.h>
#include <ModelAPI_Session.h>
#include <ModelAPI_Tools.h>
#include <ModelAPI_Validator.h>

#include <GeomAPI_ShapeIterator.h>
#include <GeomAPI_ShapeExplorer.h>

#include <GeomAlgoAPI_Copy.h>
#include <GeomAlgoAPI_ShapeBuilder.h>
#include <GeomAlgoAPI_ShapeTools.h>
#include <GeomAlgoAPI_MakeShapeCustom.h>


//==================================================================================================
FeaturesPlugin_RemoveSubShapes::FeaturesPlugin_RemoveSubShapes()
: myChangedInCode(false)
{
}

//==================================================================================================
void FeaturesPlugin_RemoveSubShapes::initAttributes()
{
  data()->addAttribute(BASE_SHAPE_ID(), ModelAPI_AttributeSelection::typeId());

  data()->addAttribute(CREATION_METHOD(), ModelAPI_AttributeString::typeId());

  data()->addAttribute(SUBSHAPES_TO_KEEP_ID(), ModelAPI_AttributeSelectionList::typeId());

  data()->addAttribute(SUBSHAPES_TO_REMOVE_ID(), ModelAPI_AttributeSelectionList::typeId());
}

void FeaturesPlugin_RemoveSubShapes::attributeChanged(const std::string& theID)
{
  ModelAPI_Feature::attributeChanged(theID);

  if (myChangedInCode) return;

  AttributeSelectionPtr aShapeAttrSelection = selection(BASE_SHAPE_ID());
  AttributeSelectionListPtr aSubShapesToKeepAttrList = selectionList(SUBSHAPES_TO_KEEP_ID());
  AttributeSelectionListPtr aSubShapesToRemoveAttrList = selectionList(SUBSHAPES_TO_REMOVE_ID());
  if (!aShapeAttrSelection.get()
      || !aSubShapesToKeepAttrList.get()
      || !aSubShapesToRemoveAttrList.get())
  {
    return;
  }

  ResultPtr aContext = aShapeAttrSelection->context();
  ResultBodyPtr aResultBody =
    std::dynamic_pointer_cast<ModelAPI_ResultBody>(aContext);
  if (!aResultBody.get()) {
    aSubShapesToKeepAttrList->clear();
    aSubShapesToRemoveAttrList->clear();
    return;
  }
  const bool isHasSubs = ModelAPI_Tools::hasSubResults(aResultBody);

  GeomShapePtr aBaseShape = aShapeAttrSelection->value();
  if(!aBaseShape.get()) {
    aBaseShape = aContext->shape();
  }

  myChangedInCode = true;

  if(theID == BASE_SHAPE_ID()) {
    aSubShapesToKeepAttrList->clear();
    aSubShapesToRemoveAttrList->clear();

    if (!aBaseShape.get()) {
      return;
    }

    std::list<GeomShapePtr> aSubShapes = GeomAlgoAPI_ShapeTools::getLowLevelSubShapes(aBaseShape);
    for (ListOfShape::const_iterator anIt = aSubShapes.cbegin(); anIt != aSubShapes.cend(); ++anIt)
    {
      GeomShapePtr aSubShape = *anIt;
      if(!isHasSubs) {
        aSubShapesToKeepAttrList->append(aContext, aSubShape);
      } else {
        std::list<ResultPtr> anAllSubs;
        ModelAPI_Tools::allSubs(aResultBody, anAllSubs);
        std::list<ResultPtr>::iterator aSubsIt = anAllSubs.begin();
        for(; aSubsIt != anAllSubs.end(); aSubsIt++) {
          ResultBodyPtr aSub = std::dynamic_pointer_cast<ModelAPI_ResultBody>(*aSubsIt);
          if (aSub && aSub->shape().get() && aSub->shape()->isSubShape(aSubShape)) {
            aSubShapesToKeepAttrList->append(aSub, aSubShape);
            break;
          }
        }
      }
    }
  }
  else if (theID == SUBSHAPES_TO_KEEP_ID())
  {
    aSubShapesToRemoveAttrList->clear();

    if (!aBaseShape.get()) {
      return;
    }

    int anIndex;
    const int aSubsToKeepNb = aSubShapesToKeepAttrList->size();
    std::list<GeomShapePtr> aSubShapes = GeomAlgoAPI_ShapeTools::getLowLevelSubShapes(aBaseShape);
    for (ListOfShape::const_iterator anIt = aSubShapes.cbegin(); anIt != aSubShapes.cend(); ++anIt)
    {
      GeomShapePtr aSubShape = *anIt;
      for(anIndex = 0; anIndex < aSubsToKeepNb; ++anIndex) {
        AttributeSelectionPtr anAttrSelectionInList = aSubShapesToKeepAttrList->value(anIndex);
        GeomShapePtr aSubShapeToKeep = anAttrSelectionInList->value();
        if (aSubShapeToKeep.get() &&
           (aSubShapeToKeep->isEqual(aSubShape) || aSubShapeToKeep->isSubShape(aSubShape))) {
          break;
        }
      }

      if (anIndex == aSubsToKeepNb) {
        if(!isHasSubs) {
          aSubShapesToRemoveAttrList->append(aContext, aSubShape);
        } else {
          std::list<ResultPtr> anAllSubs;
          ModelAPI_Tools::allSubs(aResultBody, anAllSubs);
          std::list<ResultPtr>::iterator aSubsIt = anAllSubs.begin();
          for(; aSubsIt != anAllSubs.end(); aSubsIt++) {
            ResultBodyPtr aSub = std::dynamic_pointer_cast<ModelAPI_ResultBody>(*aSubsIt);
            if (aSub && aSub->shape().get() && aSub->shape()->isSubShape(aSubShape)) {
              aSubShapesToRemoveAttrList->append(aSub, aSubShape);
              break;
            }
          }
        }
      }
    }
  }
  else if (theID == SUBSHAPES_TO_REMOVE_ID())
  {
    aSubShapesToKeepAttrList->clear();

    if (!aBaseShape.get()) {
      return;
    }

    int anIndex;
    const int aSubsToRemoveNb = aSubShapesToRemoveAttrList->size();
    std::list<GeomShapePtr> aSubShapes = GeomAlgoAPI_ShapeTools::getLowLevelSubShapes(aBaseShape);
    for (ListOfShape::const_iterator anIt = aSubShapes.cbegin(); anIt != aSubShapes.cend(); ++anIt)
    {
      GeomShapePtr aSubShape = *anIt;
      for(anIndex = 0; anIndex < aSubsToRemoveNb; ++anIndex) {
        AttributeSelectionPtr anAttrSelectionInList = aSubShapesToRemoveAttrList->value(anIndex);
        GeomShapePtr aSubShapeToRemove = anAttrSelectionInList->value();
        if (aSubShapeToRemove.get() &&
           (aSubShapeToRemove->isEqual(aSubShape) || aSubShapeToRemove->isSubShape(aSubShape))) {
          break;
        }
      }

      if (anIndex == aSubsToRemoveNb) {
        if(!isHasSubs) {
          aSubShapesToKeepAttrList->append(aContext, aSubShape);
        } else {
          std::list<ResultPtr> anAllSubs;
          ModelAPI_Tools::allSubs(aResultBody, anAllSubs);
          std::list<ResultPtr>::iterator aSubsIt = anAllSubs.begin();
          for(; aSubsIt != anAllSubs.end(); aSubsIt++) {
            ResultBodyPtr aSub = std::dynamic_pointer_cast<ModelAPI_ResultBody>(*aSubsIt);
            if (aSub && aSub->shape().get() && aSub->shape()->isSubShape(aSubShape)) {
              aSubShapesToKeepAttrList->append(aSub, aSubShape);
              break;
            }
          }
        }
      }
    }
  }

  myChangedInCode = false;
}

//==================================================================================================
void FeaturesPlugin_RemoveSubShapes::execute()
{
  // Get base shape and sub-shapes list.
  AttributeSelectionPtr aShapeAttrSelection = selection(BASE_SHAPE_ID());
  AttributeSelectionListPtr aSubShapesAttrList = selectionList(SUBSHAPES_TO_KEEP_ID());
  if(!aShapeAttrSelection.get() || !aSubShapesAttrList.get()) {
    return;
  }

  // Get base shape.
  GeomShapePtr aBaseShape = aShapeAttrSelection->value();

  GeomShapePtr aResultShape;
  int aSubsNb = aSubShapesAttrList->size();
  if(aSubsNb > 1) {
    if(!aBaseShape.get()) {
      return;
    }
    aResultShape = aBaseShape->emptyCopied();

    // Copy sub-shapes from list to new shape.
    for(int anIndex = 0; anIndex < aSubsNb; ++anIndex) {
      AttributeSelectionPtr anAttrSelectionInList = aSubShapesAttrList->value(anIndex);
      GeomShapePtr aShapeToAdd = anAttrSelectionInList->value();
      GeomAlgoAPI_ShapeBuilder::add(aResultShape, aShapeToAdd);
    }
  } else if(aSubsNb == 1) {
    AttributeSelectionPtr anAttrSelectionInList = aSubShapesAttrList->value(0);
    aResultShape = anAttrSelectionInList->value();
  }
  // deleted and copied must be jointed to one list which keeps all the history
  GeomAlgoAPI_MakeShapeList aMakeShapeList;

  // find all removed shapes
  std::shared_ptr<GeomAlgoAPI_MakeShapeCustom> aDeletedSubs(new GeomAlgoAPI_MakeShapeCustom);
  std::set<GeomAPI_Shape::ShapeType> aTypes; // types that where removed
  aTypes.insert(GeomAPI_Shape::FACE);

  std::list<GeomShapePtr> aSubShapes = GeomAlgoAPI_ShapeTools::getLowLevelSubShapes(aBaseShape);
  for (ListOfShape::const_iterator anIt = aSubShapes.cbegin(); anIt != aSubShapes.cend(); ++anIt) {
    GeomShapePtr aSubShape = *anIt;
    if (!aSubShape.get() || aSubShape->isNull())
      continue;

    int anIndex;
    for(anIndex = 0; anIndex < aSubsNb; ++anIndex) {
      AttributeSelectionPtr anAttrSelectionInList = aSubShapesAttrList->value(anIndex);
      GeomShapePtr aLeftShape = anAttrSelectionInList->value();
      if (aSubShape->isEqual(aLeftShape)) {
        break; // found in a left-list
      }
    }
    if (anIndex == aSubsNb) { // not found in left
      aDeletedSubs->addDeleted(aSubShape);
      aTypes.insert(aSubShape->shapeType());
      if (aSubShape->shapeType() != GeomAPI_Shape::FACE) {
        GeomAPI_ShapeExplorer aFaces(aSubShape, GeomAPI_Shape::FACE);
        for(; aFaces.more(); aFaces.next())
          aDeletedSubs->addDeleted(aFaces.current());
      }
    }
  }
  aMakeShapeList.appendAlgo(aDeletedSubs);

  std::shared_ptr<GeomAlgoAPI_Copy> aCopy(new GeomAlgoAPI_Copy(aResultShape));
  aResultShape = aCopy->shape();
  aMakeShapeList.appendAlgo(aCopy);

  if (aResultShape->shapeType() == GeomAPI_Shape::COMPOUND) {
    aResultShape = GeomAlgoAPI_ShapeTools::groupSharedTopology(aResultShape);
    if (aResultShape->shapeType() == GeomAPI_Shape::COMPOUND) {
      // if the result has only one sub-shape, discard the compound
      GeomAPI_ShapeIterator aSubIt(aResultShape);
      GeomShapePtr aSub = aSubIt.current();
      aSubIt.next();
      if (!aSubIt.more())
        aResultShape = aSub;
    }
  }

  // Store result.
  ResultBodyPtr aResultBody = document()->createBody(data());
  aResultBody->storeModified(aBaseShape, aResultShape, 1);
  std::set<GeomAPI_Shape::ShapeType>::iterator aTypeIter = aTypes.begin();
  for(; aTypeIter != aTypes.end(); aTypeIter++)
    aResultBody->loadDeletedShapes(&aMakeShapeList, aBaseShape, *aTypeIter, 1);
  aResultBody->loadAndOrientModifiedShapes(&aMakeShapeList, aBaseShape, GeomAPI_Shape::FACE,
      2, "Modified_Face", *aMakeShapeList.mapOfSubShapes().get(), true, false, true);
  aResultBody->loadAndOrientModifiedShapes(&aMakeShapeList, aBaseShape, GeomAPI_Shape::EDGE,
      3, "Modified_Edge", *aMakeShapeList.mapOfSubShapes().get(), false, false, true);
  aResultBody->loadAndOrientModifiedShapes(&aMakeShapeList, aBaseShape, GeomAPI_Shape::VERTEX,
      4, "Modified_Vertex", *aMakeShapeList.mapOfSubShapes().get());
  setResult(aResultBody);
}
