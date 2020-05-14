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

#include "BuildPlugin_SubShapes.h"

#include <ModelAPI_AttributeSelectionList.h>
#include <ModelAPI_ResultBody.h>
#include <ModelAPI_ResultConstruction.h>
#include <ModelAPI_Session.h>
#include <ModelAPI_Validator.h>

#include <GeomAPI_Edge.h>
#include <GeomAPI_ShapeIterator.h>
#include <GeomAPI_Wire.h>

#include <GeomAlgoAPI_ShapeBuilder.h>

//==================================================================================================
BuildPlugin_SubShapes::BuildPlugin_SubShapes()
{
}

//==================================================================================================
void BuildPlugin_SubShapes::initAttributes()
{
  data()->addAttribute(BASE_SHAPE_ID(), ModelAPI_AttributeSelection::typeId());

  data()->addAttribute(SUBSHAPES_ID(), ModelAPI_AttributeSelectionList::typeId());
  ModelAPI_Session::get()->validators()->registerNotObligatory(getKind(), SUBSHAPES_ID());
}

void BuildPlugin_SubShapes::attributeChanged(const std::string& theID)
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

    GeomShapePtr aBaseShape = aShapeAttrSelection->value();
    if(!aBaseShape.get()) {
      return;
    }
    GeomAPI_Shape::ShapeType aBaseShapeType = aBaseShape->shapeType();
    for(GeomAPI_ShapeIterator anIt(aBaseShape); anIt.more(); anIt.next()) {
      GeomShapePtr aSubShape = anIt.current();
      if(aBaseShapeType == GeomAPI_Shape::WIRE) {
        for(GeomAPI_ShapeIterator aSubIt(aSubShape); aSubIt.more(); aSubIt.next()) {
          GeomShapePtr aSubOfSubShape = aSubIt.current();
          if(aSubOfSubShape->orientation() == GeomAPI_Shape::INTERNAL) {
            aSubShapesAttrList->append(aContext, aSubOfSubShape);
          }
        }
      } else if(aBaseShapeType == GeomAPI_Shape::FACE) {
        if(aSubShape->shapeType() != GeomAPI_Shape::WIRE) {
          aSubShapesAttrList->append(aContext, aSubShape);
        } else if(aSubShape->orientation() == GeomAPI_Shape::INTERNAL) {
          if(aSubShape->shapeType() == GeomAPI_Shape::WIRE) {
            for(GeomAPI_ShapeIterator aWireIt(aSubShape); aWireIt.more(); aWireIt.next()) {
              aSubShapesAttrList->append(aContext, aWireIt.current());
            }
          }
        }
      }
    }
  }
}

//==================================================================================================
void BuildPlugin_SubShapes::execute()
{
  // Get base shape and sub-shapes list.
  AttributeSelectionPtr aShapeAttrSelection = selection(BASE_SHAPE_ID());
  AttributeSelectionListPtr aSubShapesAttrList = selectionList(SUBSHAPES_ID());
  if(!aShapeAttrSelection.get() || !aSubShapesAttrList.get()) {
    return;
  }

  // Get base shape without internal shapes.
  GeomShapePtr aBaseShape = aShapeAttrSelection->value();
  if(!aBaseShape.get()) {
    return;
  }
  std::shared_ptr<GeomAlgoAPI_ShapeBuilder> aBuilder(new GeomAlgoAPI_ShapeBuilder());
  aBuilder->removeInternal(aBaseShape);
  GeomShapePtr aResultShape = aBuilder->shape();

  // Get list of shapes.
  ListOfShape aShapesToAdd;
  for(int anIndex = 0; anIndex < aSubShapesAttrList->size(); ++anIndex) {
    AttributeSelectionPtr anAttrSelectionInList = aSubShapesAttrList->value(anIndex);
    aShapesToAdd.push_back(anAttrSelectionInList->value());
  }

  // Copy sub-shapes from list to new shape.
  if(!aShapesToAdd.empty()) {
    aBuilder->addInternal(aResultShape, aShapesToAdd);
    aResultShape = aBuilder->shape();
  }

  // Store result.
  ResultBodyPtr aResultBody = document()->createBody(data());
  aResultBody->storeModified(aBaseShape, aResultShape);
  aResultBody->loadModifiedShapes(aBuilder, aBaseShape, GeomAPI_Shape::EDGE);
  for (ListOfShape::const_iterator anIt = aShapesToAdd.cbegin();
       anIt != aShapesToAdd.cend();
       ++anIt)
  {
    GeomAPI_Shape::ShapeType aShType = (*anIt)->shapeType();
    aResultBody->loadModifiedShapes(aBuilder,
                                    *anIt,
                                    aShType);
  }
  setResult(aResultBody);
}
