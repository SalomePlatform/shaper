// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        BuildPlugin_Shell.cpp
// Created:     14 April 2016
// Author:      Dmitry Bobylev

#include "BuildPlugin_Shell.h"

#include <ModelAPI_AttributeSelectionList.h>
#include <ModelAPI_ResultBody.h>
#include <ModelAPI_ResultConstruction.h>

#include <GeomAlgoAPI_Sewing.h>
#include <GeomAPI_ShapeExplorer.h>

//=================================================================================================
BuildPlugin_Shell::BuildPlugin_Shell()
{
}

//=================================================================================================
void BuildPlugin_Shell::initAttributes()
{
  data()->addAttribute(BASE_OBJECTS_ID(), ModelAPI_AttributeSelectionList::typeId());
}

//=================================================================================================
void BuildPlugin_Shell::execute()
{
  // Get base objects list.
  AttributeSelectionListPtr aSelectionList = selectionList(BASE_OBJECTS_ID());

  // Collect base shapes.
  ListOfShape aShapes;
  for(int anIndex = 0; anIndex < aSelectionList->size(); ++anIndex) {
    AttributeSelectionPtr aSelection = aSelectionList->value(anIndex);
    GeomShapePtr aShape = aSelection->value();
    if(!aShape.get()) {
      aShape = aSelection->context()->shape();
    }
    aShapes.push_back(aShape);
  }

  // Sew faces.
  GeomAlgoAPI_Sewing aSewingAlgo(aShapes);

  // Check that algo is done.
  if(!aSewingAlgo.isDone()) {
    setError("Error: " + getKind() + " algorithm failed.");
    return;
  }

  // Check if shape is not null.
  if(!aSewingAlgo.shape().get() || aSewingAlgo.shape()->isNull()) {
    setError("Error: Resulting shape is null.");
    return;
  }

  // Check that resulting shape is valid.
  if(!aSewingAlgo.isValid()) {
    setError("Error: Resulting shape is not valid.");
    return;
  }

  // Store result.
  GeomShapePtr aResult = aSewingAlgo.shape();
  std::shared_ptr<GeomAPI_DataMapOfShapeShape> aMapOfShapes = aSewingAlgo.mapOfSubShapes();

  int anIndex = 0;
  for(GeomAPI_ShapeExplorer anExp(aResult, GeomAPI_Shape::SHELL); anExp.more(); anExp.next()) {
    GeomShapePtr aShell = anExp.current();
    ResultBodyPtr aResultBody = document()->createBody(data(), anIndex);
    aResultBody->store(aShell);
    for(ListOfShape::const_iterator anIt = aShapes.cbegin(); anIt != aShapes.cend(); ++anIt) {
      for(GeomAPI_ShapeExplorer aFaceExp(*anIt, GeomAPI_Shape::FACE); aFaceExp.more(); aFaceExp.next()) {
        GeomShapePtr aFace = aFaceExp.current();
        ListOfShape aHistory;
        aSewingAlgo.modified(aFace, aHistory);
        for(ListOfShape::const_iterator aHistoryIt = aHistory.cbegin();
            aHistoryIt != aHistory.cend();
            ++aHistoryIt) {
          if(aShell->isSubShape(*aHistoryIt)) {
            aResultBody->loadAndOrientModifiedShapes(&aSewingAlgo, aFace, GeomAPI_Shape::EDGE,
                                                     1, "Modified", *aMapOfShapes.get());
          }
        }
      }
    }
    setResult(aResultBody, anIndex);
    ++anIndex;
  }

  removeResults(anIndex);
}
