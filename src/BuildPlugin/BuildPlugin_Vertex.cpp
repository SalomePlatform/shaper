// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        BuildPlugin_Vertex.cpp
// Created:     18 April 2016
// Author:      Dmitry Bobylev

#include "BuildPlugin_Vertex.h"

#include <ModelAPI_AttributeSelectionList.h>
#include <ModelAPI_ResultBody.h>
#include <ModelAPI_ResultConstruction.h>

#include <Events_Error.h>

#include <GeomAPI_DataMapOfShapeShape.h>
#include <GeomAPI_PlanarEdges.h>
#include <GeomAPI_ShapeExplorer.h>

#include <GeomAlgoAPI_ShapeTools.h>
#include <GeomAlgoAPI_WireBuilder.h>

#include <algorithm>

//=================================================================================================
BuildPlugin_Vertex::BuildPlugin_Vertex()
{
}

//=================================================================================================
void BuildPlugin_Vertex::initAttributes()
{
  data()->addAttribute(BASE_OBJECTS_ID(), ModelAPI_AttributeSelectionList::typeId());
}

//=================================================================================================
void BuildPlugin_Vertex::execute()
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

    if(aShape->shapeType() != GeomAPI_Shape::VERTEX) {
      setError("Error: Selected shape has wrong type. Only vertices acceptable.");
      return;
    }

    // Store result.
    ResultBodyPtr aResultBody = document()->createBody(data(), aResultIndex);
    aResultBody->store(aShape);
    setResult(aResultBody, aResultIndex);
    ++aResultIndex;
  }

  removeResults(aResultIndex);
}
