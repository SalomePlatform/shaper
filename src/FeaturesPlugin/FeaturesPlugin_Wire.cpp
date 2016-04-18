// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        FeaturesPlugin_Wire.cpp
// Created:     14 April 2016
// Author:      Dmitry Bobylev

#include "FeaturesPlugin_Wire.h"

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
FeaturesPlugin_Wire::FeaturesPlugin_Wire()
{
}

//=================================================================================================
void FeaturesPlugin_Wire::initAttributes()
{
  data()->addAttribute(BASE_OBJECTS_ID(), ModelAPI_AttributeSelectionList::typeId());
}

//=================================================================================================
void FeaturesPlugin_Wire::execute()
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
  for(int anIndex = 0; anIndex < aSelectionList->size(); ++anIndex) {
    AttributeSelectionPtr aSelection = aSelectionList->value(anIndex);
    GeomShapePtr aShape = aSelection->value();
    if(!aShape.get()) {
      setError("Error: Empty shape selected.");
      return;
    }

    if(aShape->shapeType() != GeomAPI_Shape::EDGE && aShape->shapeType() != GeomAPI_Shape::WIRE) {
      setError("Error: Selected shape has wrong type. Only edges and wires acceptable.");
      return;
    }

    aListOfShapes.push_back(aShape);
  }

  // Create wire.
  GeomShapePtr aWire = GeomAlgoAPI_WireBuilder::wire(aListOfShapes);
  if(!aWire.get()) {
    setError("Error: Result wire empty. Probably it has disconnected edges or non-manifold.");
    return;
  }

  // Store result.
  ResultBodyPtr aResultBody = document()->createBody(data());
  aResultBody->store(aWire);
  setResult(aResultBody);
}

//=================================================================================================
bool FeaturesPlugin_Wire::customAction(const std::string& theActionId)
{
  if(theActionId == "add_contour") {
    return addContour();
  } else {
    Events_Error::send("Error: Feature \"" + getKind() + "\" does not support action \"" + theActionId + "\".");
  }

  return false;
}

//=================================================================================================
bool FeaturesPlugin_Wire::addContour()
{
  // Get base objects list.
  AttributeSelectionListPtr aSelectionList = selectionList(BASE_OBJECTS_ID());
  if(aSelectionList->size() == 0) {
    Events_Error::send("Error: Empty selection list.");
    return false;
  }

  // Collect attributes to check.
  ListOfShape anAddedEdges;
  std::list<AttributeSelectionPtr> anAttributesToCheck;
  for(int anIndex = 0; anIndex < aSelectionList->size(); ++anIndex) {
    AttributeSelectionPtr aSelection = aSelectionList->value(anIndex);
    GeomShapePtr anEdgeInList = aSelection->value();
    if(!anEdgeInList.get()) {
      continue;
    }

    // Check that it is edge.
    if(anEdgeInList->shapeType() != GeomAPI_Shape::EDGE) {
      continue;
    }

    // Check that it is edge on sketch.
    ResultPtr aContext = aSelection->context();
    ResultConstructionPtr aConstruction = std::dynamic_pointer_cast<ModelAPI_ResultConstruction>(aContext);
    if(!aConstruction.get()) {
      continue;
    }
    GeomShapePtr aContextShape = aConstruction->shape();
    std::shared_ptr<GeomAPI_PlanarEdges> aPlanarEdges = std::dynamic_pointer_cast<GeomAPI_PlanarEdges>(aContextShape);
    if(!aPlanarEdges.get()) {
      continue;
    }

    // Check that sketch have faces.
    if(aConstruction->facesNum() == 0) {
      continue;
    }

    anAddedEdges.push_back(anEdgeInList);
    anAttributesToCheck.push_back(aSelection);
  }

  // Check if edges have contours.
  bool isAnyContourFound = false;
  for(std::list<AttributeSelectionPtr>::const_iterator aListIt = anAttributesToCheck.cbegin();
      aListIt != anAttributesToCheck.cend();
      ++aListIt) {
    AttributeSelectionPtr aSelection = *aListIt;
    std::shared_ptr<GeomAPI_Edge> anEdgeInList(new GeomAPI_Edge(aSelection->value()));
    ResultConstructionPtr aConstruction = std::dynamic_pointer_cast<ModelAPI_ResultConstruction>(aSelection->context());
    std::shared_ptr<GeomAPI_PlanarEdges> aPlanarEdges = std::dynamic_pointer_cast<GeomAPI_PlanarEdges>(aConstruction->shape());

    ListOfShape aClosedWires;
    GeomAlgoAPI_ShapeTools::getClosedWires(aPlanarEdges->getEdges(), aClosedWires);

    //// Iterate on wires and add wire with this edge.
    //std::shared_ptr<GeomAPI_Shape> aFoundWire;
    //for(ListOfShape::const_iterator aWireIt = aClosedWires.cbegin();
    //    aWireIt != aClosedWires.cend();
    //    ++aWireIt) {
    //  GeomShapePtr aWire = *aWireIt;
    //  for(GeomAPI_ShapeExplorer anExp(aWire, GeomAPI_Shape::EDGE); anExp.more(); anExp.next()) {
    //    GeomShapePtr anEdgeOnWire = anExp.current();
    //    if(anEdgeInList->isSame(anEdgeOnWire)) {
    //      aFoundWire = aWire;
    //      break;
    //    }
    //  }

    //  if(aFoundWire.get()) {
    //    break;
    //  }
    //}

    //// If wire with the same edge found add all other edges to list.
    //if(aFoundWire.get()) {
    //  isAnyContourFound = true;
    //  anAddedEdges.bind(anEdgeInList, anEdgeInList);
    //  for(GeomAPI_ShapeExplorer anExp(aFoundWire, GeomAPI_Shape::EDGE); anExp.more(); anExp.next()) {
    //    GeomShapePtr anEdgeOnFace = anExp.current();
    //    if(!anAddedEdges.isBound(anEdgeOnFace)) {
    //      anAddedEdges.bind(anEdgeOnFace, anEdgeOnFace);
    //      aSelectionList->append(aConstruction, anEdgeOnFace);
    //    }
    //  }
    //}

    // Iterate on faces and add face with this edge.
    std::shared_ptr<GeomAPI_Face> aFoundFace;
    for(int anIndex = 0; anIndex < aConstruction->facesNum(); ++anIndex) {
      std::shared_ptr<GeomAPI_Face> aFace = aConstruction->face(anIndex);
      for(GeomAPI_ShapeExplorer anExp(aFace, GeomAPI_Shape::EDGE); anExp.more(); anExp.next()) {
        std::shared_ptr<GeomAPI_Edge> anEdgeOnFace(new GeomAPI_Edge(anExp.current()));
        if(anEdgeInList->isEqual(anEdgeOnFace)) {
          aFoundFace = aFace;
          break;
        }
      }

      if(aFoundFace.get()) {
        break;
      }
    }

    // If face with the same edge found. Add all other edges to list.
    if(aFoundFace.get()) {
      isAnyContourFound = true;
      for(GeomAPI_ShapeExplorer anExp(aFoundFace, GeomAPI_Shape::EDGE); anExp.more(); anExp.next()) {
        std::shared_ptr<GeomAPI_Edge> anEdgeOnFace(new GeomAPI_Edge(anExp.current()));
        ListOfShape::const_iterator anEdgesIt = anAddedEdges.cbegin();
        for(; anEdgesIt != anAddedEdges.cend(); ++anEdgesIt) {
          if(anEdgeOnFace->isEqual(*anEdgesIt)) {
            break;
          }
        }
        if(anEdgesIt == anAddedEdges.cend()) {
          anAddedEdges.push_back(anEdgeOnFace);
          aSelectionList->append(aConstruction, anEdgeOnFace);
        }
      }
    }
  }

  if(!isAnyContourFound) {
    Events_Error::send("Error: No contours found for selected edges.");
    return false;
  }

  return false;
}
