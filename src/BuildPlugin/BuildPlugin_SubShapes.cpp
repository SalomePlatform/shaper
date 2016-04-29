// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        BuildPlugin_SubShapes.cpp
// Created:     14 April 2016
// Author:      Dmitry Bobylev

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

  data()->addAttribute(SUB_SHAPES_ID(), ModelAPI_AttributeSelectionList::typeId());
  ModelAPI_Session::get()->validators()->registerNotObligatory(getKind(), SUB_SHAPES_ID());
}

void BuildPlugin_SubShapes::attributeChanged(const std::string& theID)
{
  ModelAPI_Feature::attributeChanged(theID);

  if(theID == BASE_SHAPE_ID()) {
    AttributeSelectionPtr aShapeAttrSelection = selection(BASE_SHAPE_ID());
    AttributeSelectionListPtr aSubShapesAttrList = selectionList(SUB_SHAPES_ID());
    if(!aShapeAttrSelection.get() || !aSubShapesAttrList.get()) {
      return;
    }
    ResultPtr aContext = aShapeAttrSelection->context();

    aSubShapesAttrList->clear();

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
  AttributeSelectionListPtr aSubShapesAttrList = selectionList(SUB_SHAPES_ID());
  if(!aShapeAttrSelection.get() || !aSubShapesAttrList.get()) {
    return;
  }

  // Get base shape without internal shapes.
  GeomShapePtr aBaseShape = aShapeAttrSelection->value();
  if(!aBaseShape.get()) {
    return;
  }
  GeomAlgoAPI_ShapeBuilder aBuilder;
  aBuilder.removeInternal(aBaseShape);
  GeomShapePtr aResultShape = aBuilder.shape();

  // Get list of shapes.
  ListOfShape aShapesToAdd;
  for(int anIndex = 0; anIndex < aSubShapesAttrList->size(); ++anIndex) {
    AttributeSelectionPtr anAttrSelectionInList = aSubShapesAttrList->value(anIndex);
    aShapesToAdd.push_back(anAttrSelectionInList->value());
  }

  // Copy sub-shapes from list to new shape.
  if(!aShapesToAdd.empty()) {
    aBuilder.add(aResultShape, aShapesToAdd);
    aResultShape = aBuilder.shape();
  }

  // Store result.
  ResultBodyPtr aResultBody = document()->createBody(data());
  aResultBody->storeModified(aBaseShape, aResultShape);
  aResultBody->loadAndOrientModifiedShapes(&aBuilder, aBaseShape, GeomAPI_Shape::EDGE, 1,
                                          "Modified_Edge", *aBuilder.mapOfSubShapes().get());
  for(ListOfShape::const_iterator anIt = aShapesToAdd.cbegin(); anIt != aShapesToAdd.cend(); ++anIt) {
    GeomAPI_Shape::ShapeType aShType = (*anIt)->shapeType();
    aResultBody->loadAndOrientModifiedShapes(&aBuilder, *anIt, aShType, 1,
                                             aShType == GeomAPI_Shape::VERTEX ? "Modified_Vertex" : "Modified_Edge",
                                             *aBuilder.mapOfSubShapes().get());
  }
  setResult(aResultBody);
}
