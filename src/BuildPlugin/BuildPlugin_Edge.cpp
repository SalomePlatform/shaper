// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        BuildPlugin_Edge.cpp
// Created:     18 April 2016
// Author:      Dmitry Bobylev

#include "BuildPlugin_Edge.h"

#include <ModelAPI_AttributeSelectionList.h>
#include <ModelAPI_ResultBody.h>

#include <GeomAlgoAPI_Copy.h>

//=================================================================================================
BuildPlugin_Edge::BuildPlugin_Edge()
{
}

//=================================================================================================
void BuildPlugin_Edge::initAttributes()
{
  data()->addAttribute(BASE_OBJECTS_ID(), ModelAPI_AttributeSelectionList::typeId());
}

//=================================================================================================
void BuildPlugin_Edge::execute()
{
  // Get base objects list.
  AttributeSelectionListPtr aSelectionList = selectionList(BASE_OBJECTS_ID());
  if(!aSelectionList.get()) {
    setError("Error: Could not get selection list.");
    return;
  }
  if(aSelectionList->size() == 0) {
    setError("Error: Empty selection list.");
    return;
  }

  // Collect base shapes.
  ListOfShape aListOfShapes;
  int aResultIndex = 0;
  for(int anIndex = 0; anIndex < aSelectionList->size(); ++anIndex) {
    AttributeSelectionPtr aSelection = aSelectionList->value(anIndex);
    GeomShapePtr aShape = aSelection->value();
    if(!aShape.get()) {
      ResultPtr aContext = aSelection->context();
      if(!aContext.get()) {
        setError("Error: Attribute has empty context.");
        return;
      }

      aShape = aContext->shape();
    }
    if(!aShape.get()) {
      setError("Error: Empty shape selected.");
      return;
    }

    if(aShape->shapeType() != GeomAPI_Shape::EDGE) {
      setError("Error: Selected shape has wrong type. Only edges acceptable.");
      return;
    }

    // Copy shape.
    GeomAlgoAPI_Copy aCopyAlgo(aShape);

    // Check that algo is done.
    if(!aCopyAlgo.isDone()) {
      setError("Error: " + getKind() + " algorithm failed.");
      return;
    }

    // Check if shape is not null.
    if(!aCopyAlgo.shape().get() || aCopyAlgo.shape()->isNull()) {
      setError("Error: Resulting shape is null.");
      return;
    }

    // Check that resulting shape is valid.
    if(!aCopyAlgo.isValid()) {
      setError("Error: Resulting shape is not valid.");
      return;
    }

    // Store result.
    ResultBodyPtr aResultBody = document()->createBody(data(), aResultIndex);
    aResultBody->storeModified(aShape, aCopyAlgo.shape());
    std::shared_ptr<GeomAPI_DataMapOfShapeShape> aSubShapes = aCopyAlgo.mapOfSubShapes();
    int aModVertexTag = 1;
    std::string aModVertexName = "Modified_Vertex";
    aResultBody->loadAndOrientModifiedShapes(&aCopyAlgo, aShape, GeomAPI_Shape::VERTEX,
                                             aModVertexTag, aModVertexName, *aSubShapes.get());

    setResult(aResultBody, aResultIndex);
    ++aResultIndex;
  }

  removeResults(aResultIndex);
}
