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
  const int aNumOfSubs = aResultBody->numberOfSubs();

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

    for(GeomAPI_ShapeIterator anIt(aBaseShape); anIt.more(); anIt.next()) {
      GeomShapePtr aSubShape = anIt.current();
      if(aNumOfSubs == 0) {
        aSubShapesToKeepAttrList->append(aContext, aSubShape);
      } else {
        for (int anIndex = 0; anIndex < aResultBody->numberOfSubs(); ++anIndex) {
          ResultBodyPtr aSubResult = aResultBody->subResult(anIndex);
          if(aSubResult->shape()->isEqual(aSubShape)) {
            aSubShapesToKeepAttrList->append(aSubResult, aSubShape);
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
    for(GeomAPI_ShapeIterator anIt(aBaseShape); anIt.more(); anIt.next()) {
      GeomShapePtr aSubShape = anIt.current();
      for(anIndex = 0; anIndex < aSubsToKeepNb; ++anIndex) {
        AttributeSelectionPtr anAttrSelectionInList = aSubShapesToKeepAttrList->value(anIndex);
        GeomShapePtr aSubShapeToKeep = anAttrSelectionInList->value();
        if (aSubShapeToKeep.get() && aSubShapeToKeep->isEqual(aSubShape)) {
          break;
        }
      }

      if (anIndex == aSubsToKeepNb) {
        if(aNumOfSubs == 0) {
          aSubShapesToRemoveAttrList->append(aContext, aSubShape);
        } else {
          for (int anIndex = 0; anIndex < aResultBody->numberOfSubs(); ++anIndex) {
            ResultBodyPtr aSubResult = aResultBody->subResult(anIndex);
            if(aSubResult->shape()->isEqual(aSubShape)) {
              aSubShapesToRemoveAttrList->append(aSubResult, aSubShape);
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
    for(GeomAPI_ShapeIterator anIt(aBaseShape); anIt.more(); anIt.next()) {
      GeomShapePtr aSubShape = anIt.current();
      for(anIndex = 0; anIndex < aSubsToRemoveNb; ++anIndex) {
        AttributeSelectionPtr anAttrSelectionInList = aSubShapesToRemoveAttrList->value(anIndex);
        GeomShapePtr aSubShapeToRemove = anAttrSelectionInList->value();
        if (aSubShapeToRemove.get() && aSubShapeToRemove->isEqual(aSubShape)) {
          break;
        }
      }

      if (anIndex == aSubsToRemoveNb) {
        if(aNumOfSubs == 0) {
          aSubShapesToKeepAttrList->append(aContext, aSubShape);
        } else {
          for (int anIndex = 0; anIndex < aResultBody->numberOfSubs(); ++anIndex) {
            ResultBodyPtr aSubResult = aResultBody->subResult(anIndex);
            if(aSubResult->shape()->isEqual(aSubShape)) {
              aSubShapesToKeepAttrList->append(aSubResult, aSubShape);
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
  for(GeomAPI_ShapeIterator anIt(aBaseShape); anIt.more(); anIt.next()) {
    if (!anIt.current().get() || anIt.current()->isNull())
      continue;
    int anIndex;
    for(anIndex = 0; anIndex < aSubsNb; ++anIndex) {
      AttributeSelectionPtr anAttrSelectionInList = aSubShapesAttrList->value(anIndex);
      GeomShapePtr aLeftShape = anAttrSelectionInList->value();
      if (anIt.current()->isEqual(aLeftShape)) {
        break; // found in a left-list
      }
    }
    if (anIndex == aSubsNb) { // not found in left
      aDeletedSubs->addDeleted(anIt.current());
      aTypes.insert(anIt.current()->shapeType());
      if (anIt.current()->shapeType() != GeomAPI_Shape::FACE) {
        GeomAPI_ShapeExplorer aFaces(anIt.current(), GeomAPI_Shape::FACE);
        for(; aFaces.more(); aFaces.next())
          aDeletedSubs->addDeleted(aFaces.current());
      }
    }
  }
  aMakeShapeList.appendAlgo(aDeletedSubs);

  std::shared_ptr<GeomAlgoAPI_Copy> aCopy(new GeomAlgoAPI_Copy(aResultShape));
  aResultShape = aCopy->shape();
  aMakeShapeList.appendAlgo(aCopy);

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
