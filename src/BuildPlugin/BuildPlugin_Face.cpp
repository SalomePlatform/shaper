// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        BuildPlugin_Face.cpp
// Created:     14 April 2016
// Author:      Dmitry Bobylev

#include "BuildPlugin_Face.h"

#include <ModelAPI_AttributeSelectionList.h>
#include <ModelAPI_ResultBody.h>

#include <GeomAPI_Edge.h>
#include <GeomAPI_Pln.h>
#include <GeomAPI_ShapeExplorer.h>

#include <GeomAlgoAPI_ShapeTools.h>
#include <GeomAlgoAPI_SketchBuilder.h>

//=================================================================================================
BuildPlugin_Face::BuildPlugin_Face()
{
}

//=================================================================================================
void BuildPlugin_Face::initAttributes()
{
  data()->addAttribute(BASE_OBJECTS_ID(), ModelAPI_AttributeSelectionList::typeId());
}

//=================================================================================================
void BuildPlugin_Face::execute()
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
  ListOfShape anEdges;
  for(int anIndex = 0; anIndex < aSelectionList->size(); ++anIndex) {
    AttributeSelectionPtr aSelection = aSelectionList->value(anIndex);
    GeomShapePtr aShape = aSelection->value();
    if(!aShape.get()) {
      aShape = aSelection->context()->shape();
    }
    for(GeomAPI_ShapeExplorer anExp(aShape, GeomAPI_Shape::EDGE); anExp.more(); anExp.next()) {
      GeomShapePtr anEdge = anExp.current();
      anEdges.push_back(anEdge);
    }
  }

  // Get plane.
  std::shared_ptr<GeomAPI_Pln> aPln = GeomAlgoAPI_ShapeTools::findPlane(anEdges);

  // Get faces.
  ListOfShape aFaces;
  GeomAlgoAPI_SketchBuilder::createFaces(aPln->location(), aPln->xDirection(), aPln->direction(), anEdges, aFaces);

  // Get wires from faces.
  ListOfShape aWires;
  for(ListOfShape::const_iterator anIt = aFaces.cbegin(); anIt != aFaces.cend(); ++anIt) {
    for(GeomAPI_ShapeExplorer anExp(*anIt, GeomAPI_Shape::WIRE); anExp.more(); anExp.next()) {
      aWires.push_back(anExp.current());
    }
  }

  // Make faces with holes.
  aFaces.clear();
  GeomAlgoAPI_ShapeTools::makeFacesWithHoles(aPln->location(), aPln->direction(), aWires, aFaces);

  // Store result.
  int anIndex = 0;
  for(ListOfShape::const_iterator anIt = aFaces.cbegin(); anIt != aFaces.cend(); ++anIt) {
    ResultBodyPtr aResultBody = document()->createBody(data(), anIndex);
    GeomShapePtr aShape = *anIt;
    aResultBody->store(aShape);

    for(GeomAPI_ShapeExplorer anExp(aShape, GeomAPI_Shape::EDGE); anExp.more(); anExp.next()) {
      GeomShapePtr anEdgeInResult = anExp.current();
      for(ListOfShape::const_iterator anIt = anEdges.cbegin(); anIt != anEdges.cend(); ++anIt) {
        std::shared_ptr<GeomAPI_Edge> anEdgeInList(new GeomAPI_Edge(*anIt));
        if(anEdgeInList->isEqual(anEdgeInResult)) {
          aResultBody->modified(anEdgeInList, anEdgeInResult, "Edge");
          break;
        }
      }
    }

    setResult(aResultBody, anIndex);
    ++anIndex;
  }

  removeResults(anIndex);
}
