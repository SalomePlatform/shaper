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
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com<mailto:webmaster.salome@opencascade.com>
//

#include "FeaturesPlugin_RemoveSubShapes.h"

#include <ModelAPI_AttributeSelectionList.h>
#include <ModelAPI_ResultBody.h>
#include <ModelAPI_ResultCompSolid.h>
#include <ModelAPI_ResultConstruction.h>
#include <ModelAPI_Session.h>
#include <ModelAPI_Validator.h>

#include <GeomAPI_ShapeIterator.h>

#include <GeomAlgoAPI_Copy.h>
#include <GeomAlgoAPI_ShapeBuilder.h>
#include <GeomAlgoAPI_ShapeTools.h>

//==================================================================================================
FeaturesPlugin_RemoveSubShapes::FeaturesPlugin_RemoveSubShapes()
{
}

//==================================================================================================
void FeaturesPlugin_RemoveSubShapes::initAttributes()
{
  data()->addAttribute(BASE_SHAPE_ID(), ModelAPI_AttributeSelection::typeId());

  data()->addAttribute(SUBSHAPES_ID(), ModelAPI_AttributeSelectionList::typeId());
}

void FeaturesPlugin_RemoveSubShapes::attributeChanged(const std::string& theID)
{
  ModelAPI_Feature::attributeChanged(theID);

  if(theID == BASE_SHAPE_ID()) {
    AttributeSelectionPtr aShapeAttrSelection = selection(BASE_SHAPE_ID());
    AttributeSelectionListPtr aSubShapesAttrList = selectionList(SUBSHAPES_ID());
    if(!aShapeAttrSelection.get() || !aSubShapesAttrList.get()) {
      return;
    }

    aSubShapesAttrList->clear();

    ResultPtr aContext = aShapeAttrSelection->context();
    ResultCompSolidPtr aResultCompSolid =
      std::dynamic_pointer_cast<ModelAPI_ResultCompSolid>(aContext);
    if(!aResultCompSolid.get()) {
      return;
    }

    GeomShapePtr aBaseShape = aShapeAttrSelection->value();
    if(!aBaseShape.get()) {
      aBaseShape = aContext->shape();
    }
    if(!aBaseShape.get()) {
      return;
    }
    GeomAPI_Shape::ShapeType aShapeType = aBaseShape->shapeType();
    if(aShapeType != GeomAPI_Shape::WIRE
        && aShapeType != GeomAPI_Shape::SHELL
        && aShapeType != GeomAPI_Shape::COMPSOLID
        && aShapeType != GeomAPI_Shape::COMPOUND) {
      return;
    }
    for(GeomAPI_ShapeIterator anIt(aBaseShape); anIt.more(); anIt.next()) {
      GeomShapePtr aSubShape = anIt.current();
      const int aNumOfSubs = aResultCompSolid->numberOfSubs();
      if(aNumOfSubs == 0) {
        aSubShapesAttrList->append(aContext, aSubShape);
      } else {
        for(int anIndex = 0; anIndex < aResultCompSolid->numberOfSubs(); ++anIndex) {
          ResultBodyPtr aSubResult = aResultCompSolid->subResult(anIndex);
          if(aSubResult->shape()->isEqual(aSubShape)) {
            aSubShapesAttrList->append(aSubResult, aSubShape);
            break;
          }
        }
      }
    }
  }
}

//==================================================================================================
void FeaturesPlugin_RemoveSubShapes::execute()
{
  // Get base shape and sub-shapes list.
  AttributeSelectionPtr aShapeAttrSelection = selection(BASE_SHAPE_ID());
  AttributeSelectionListPtr aSubShapesAttrList = selectionList(SUBSHAPES_ID());
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

  GeomAlgoAPI_Copy aCopy(aResultShape);
  aResultShape = aCopy.shape();

  // Store result.
  ResultBodyPtr aResultBody = document()->createBody(data());
  aResultBody->storeModified(aBaseShape, aResultShape, 1);
  aResultBody->loadAndOrientModifiedShapes(&aCopy,
                                           aBaseShape,
                                           GeomAPI_Shape::FACE,
                                           10000,
                                           "Modified_Face",
                                           *aCopy.mapOfSubShapes().get(),
                                           true);
  setResult(aResultBody);
}
