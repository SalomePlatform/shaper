// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:        FeaturesPlugin_Pipe.cpp
// Created:     16 March 2016
// Author:      Dmitry Bobylev

#include "FeaturesPlugin_Pipe.h"

#include <ModelAPI_AttributeSelection.h>
#include <ModelAPI_AttributeSelectionList.h>
#include <ModelAPI_AttributeString.h>
#include <ModelAPI_ResultConstruction.h>
#include <ModelAPI_Session.h>
#include <ModelAPI_Validator.h>

#include <GeomAlgoAPI_CompoundBuilder.h>
#include <GeomAlgoAPI_Pipe.h>
#include <GeomAlgoAPI_ShapeTools.h>
#include <GeomAPI_PlanarEdges.h>
#include <GeomAPI_ShapeExplorer.h>

#include <map>
#include <sstream>

//=================================================================================================
FeaturesPlugin_Pipe::FeaturesPlugin_Pipe()
{
}

//=================================================================================================
void FeaturesPlugin_Pipe::initAttributes()
{
  data()->addAttribute(CREATION_METHOD(), ModelAPI_AttributeString::typeId());

  data()->addAttribute(BASE_OBJECTS_ID(), ModelAPI_AttributeSelectionList::typeId());
  data()->addAttribute(PATH_OBJECT_ID(), ModelAPI_AttributeSelection::typeId());

  data()->addAttribute(BINORMAL_ID(), ModelAPI_AttributeSelection::typeId());

  data()->addAttribute(LOCATIONS_ID(), ModelAPI_AttributeSelectionList::typeId());
  ModelAPI_Session::get()->validators()->registerNotObligatory(getKind(), LOCATIONS_ID());
}

//=================================================================================================
void FeaturesPlugin_Pipe::execute()
{
  // Getting creation method.
  std::string aCreationMethod = string(CREATION_METHOD())->value();

  // Getting base objects.
  ListOfShape aBaseShapesList, aBaseFacesList;
  std::map<ResultConstructionPtr, ListOfShape> aSketchWiresMap;
  AttributeSelectionListPtr aBaseObjectsSelectionList = selectionList(BASE_OBJECTS_ID());
  if(!aBaseObjectsSelectionList.get()) {
    setError("Error: Could not get base objects selection list.");
    return;
  }
  if(aBaseObjectsSelectionList->size() == 0) {
    setError("Error: Base objects list is empty.");
    return;
  }
  for(int anIndex = 0; anIndex < aBaseObjectsSelectionList->size(); anIndex++) {
    AttributeSelectionPtr aBaseObjectSelection = aBaseObjectsSelectionList->value(anIndex);
    if(!aBaseObjectSelection.get()) {
      setError("Error: One of the selected base objects is empty.");
      return;
    }
    std::shared_ptr<GeomAPI_Shape> aBaseShape = aBaseObjectSelection->value();
    if(aBaseShape.get() && !aBaseShape->isNull()) {
      GeomAPI_Shape::ShapeType aST = aBaseShape->shapeType();
      ResultConstructionPtr aConstruction =
        std::dynamic_pointer_cast<ModelAPI_ResultConstruction>(aBaseObjectSelection->context());
      if(aConstruction.get() && !aBaseShape->isEqual(aConstruction->shape()) && aST == GeomAPI_Shape::WIRE) {
        // It is a wire on the sketch, store it to make face later.
        aSketchWiresMap[aConstruction].push_back(aBaseShape);
        continue;
      } else {
      aST == GeomAPI_Shape::FACE ? aBaseFacesList.push_back(aBaseShape) :
                                   aBaseShapesList.push_back(aBaseShape);
      }
    } else {
      // This may be the whole sketch result selected, check and get faces.
      ResultConstructionPtr aConstruction = std::dynamic_pointer_cast<ModelAPI_ResultConstruction>(aBaseObjectSelection->context());
      if(!aConstruction.get()) {
        setError("Error: One of selected sketches does not have results.");
        return;
      }
      int aFacesNum = aConstruction->facesNum();
      if(aFacesNum == 0) {
        // Probably it can be construction.
        aBaseShape = aConstruction->shape();
        if(aBaseShape.get() && !aBaseShape->isNull()) {
          aBaseShape->shapeType() == GeomAPI_Shape::FACE ? aBaseFacesList.push_back(aBaseShape) :
                                                           aBaseShapesList.push_back(aBaseShape);
        }
      } else {
        for(int aFaceIndex = 0; aFaceIndex < aFacesNum; aFaceIndex++) {
          std::shared_ptr<GeomAPI_Shape> aBaseFace = std::dynamic_pointer_cast<GeomAPI_Shape>(aConstruction->face(aFaceIndex));
          if(!aBaseFace.get() || aBaseFace->isNull()) {
            setError("Error: One of the faces on selected sketch is Null.");
            return;
          }
          aBaseFacesList.push_back(aBaseFace);
        }
      }
    }
  }

  // Make faces from sketch wires.
  for(std::map<ResultConstructionPtr, ListOfShape>::const_iterator anIt = aSketchWiresMap.cbegin();
      anIt != aSketchWiresMap.cend(); ++anIt) {
    const std::shared_ptr<GeomAPI_PlanarEdges> aSketchPlanarEdges =
      std::dynamic_pointer_cast<GeomAPI_PlanarEdges>((*anIt).first->shape());
    const ListOfShape& aWiresList = (*anIt).second;
    ListOfShape aFaces;
    GeomAlgoAPI_ShapeTools::makeFacesWithHoles(aSketchPlanarEdges->origin(),
                                               aSketchPlanarEdges->norm(),
                                               aWiresList,
                                               aFaces);
    aBaseFacesList.insert(aBaseFacesList.end(), aFaces.begin(), aFaces.end());
  }

  // Searching faces with common edges.
  if(aCreationMethod == "simple") {
    ListOfShape aShells;
    ListOfShape aFreeFaces;
    std::shared_ptr<GeomAPI_Shape> aFacesCompound = GeomAlgoAPI_CompoundBuilder::compound(aBaseFacesList);
    GeomAlgoAPI_ShapeTools::combineShapes(aFacesCompound, GeomAPI_Shape::SHELL, aShells, aFreeFaces);
    aBaseShapesList.insert(aBaseShapesList.end(), aFreeFaces.begin(), aFreeFaces.end());
    aBaseShapesList.insert(aBaseShapesList.end(), aShells.begin(), aShells.end());
  } else {
    aBaseShapesList.insert(aBaseShapesList.end(), aBaseFacesList.begin(), aBaseFacesList.end());
  }

  // Getting path.
  AttributeSelectionPtr aPathSelection = selection(PATH_OBJECT_ID());
  if(!aPathSelection.get()) {
    setError("Error: Path selection is empty.");
    return;
  }
  std::shared_ptr<GeomAPI_Shape> aPathShape = std::dynamic_pointer_cast<GeomAPI_Shape>(aPathSelection->value());
  if(!aPathShape.get()) {
    // Probaply it is a construction.
    aPathShape = aPathSelection->context()->shape();
  }
  if(!aPathShape.get() || aPathShape->isNull()) {
    setError("Error: Path shape is null.");
    return;
  }

  // Getting Bi-Normal
  std::shared_ptr<GeomAPI_Shape> aBiNormal;
  if(aCreationMethod == "binormal") {
    AttributeSelectionPtr aBiNormalSelection = selection(BINORMAL_ID());
    if(!aBiNormalSelection.get()) {
      setError("Error: Bi-Normal selection is empty.");
      return;
    }
    aBiNormal = std::dynamic_pointer_cast<GeomAPI_Shape>(aBiNormalSelection->value());
    if(!aBiNormal.get()) {
      // Probably it is a construction.
      aBiNormal = aBiNormalSelection->context()->shape();
    }
    if(!aBiNormal.get() || aBiNormal->isNull()) {
      setError("Error: Bi-Normal shape is null.");
      return;
    }
  }

  // Getting locations.
  ListOfShape aLocations;
  if(aCreationMethod == "locations") {
    AttributeSelectionListPtr aLocationsSelectionList = selectionList(LOCATIONS_ID());
    if(!aLocationsSelectionList.get()) {
      setError("Error: Could not get locations selection list.");
      return;
    }
    for(int anIndex = 0; anIndex < aLocationsSelectionList->size(); anIndex++) {
      AttributeSelectionPtr aLocationSelection = aLocationsSelectionList->value(anIndex);
      if(!aLocationSelection.get()) {
        setError("Error: One of the selected location is empty.");
        return;
      }
      std::shared_ptr<GeomAPI_Shape> aLocationShape = aLocationSelection->value();
      if(!aLocationShape.get()) {
        // Probably it is a construction.
        aLocationShape = aLocationSelection->context()->shape();
      }
      if(!aLocationShape.get() || aLocationShape->isNull()) {
        setError("Error: One of the selected location shape is null.");
        return;
      }
      aLocations.push_back(aLocationShape);
    }
  }

  // Generating result for each object.
  int aResultIndex = 0;
  if(aCreationMethod == "simple" || aCreationMethod == "binormal") {
    for(ListOfShape::const_iterator anIter = aBaseShapesList.cbegin(); anIter != aBaseShapesList.cend(); anIter++) {
      std::shared_ptr<GeomAPI_Shape> aBaseShape = *anIter;

      GeomAlgoAPI_Pipe aPipeAlgo = aCreationMethod == "simple" ? GeomAlgoAPI_Pipe(aBaseShape, aPathShape) :
                                                                 GeomAlgoAPI_Pipe(aBaseShape, aPathShape, aBiNormal);

      if(!aPipeAlgo.isDone()) {
        setError("Error: Pipe algorithm failed.");
        aResultIndex = 0;
        break;
      }

      // Check if shape is valid
      if(!aPipeAlgo.shape().get() || aPipeAlgo.shape()->isNull()) {
        setError("Error: Resulting shape is Null.");
        aResultIndex = 0;
        break;
      }
      if(!aPipeAlgo.isValid()) {
        setError("Error: Resulting shape is not valid.");
        aResultIndex = 0;
        break;
      }

      storeResult(aBaseShape, aPipeAlgo, aResultIndex++);
    }
  } else if(aCreationMethod == "locations") {
    GeomAlgoAPI_Pipe aPipeAlgo = GeomAlgoAPI_Pipe(aBaseShapesList, aLocations, aPathShape);

    if(!aPipeAlgo.isDone()) {
      setError("Error: Pipe algorithm failed.");
      removeResults(0);
      return;
    }

    // Check if shape is valid
    if(!aPipeAlgo.shape().get() || aPipeAlgo.shape()->isNull()) {
      setError("Error: Resulting shape is Null.");
      removeResults(0);
      return;
    }
    if(!aPipeAlgo.isValid()) {
      setError("Error: Resulting shape is not valid.");
      removeResults(0);
      return;
    }

    storeResult(aBaseShapesList, aPipeAlgo, aResultIndex++);
  } else {
    setError("Error: Wrong creation method.");
    return;
  }

  removeResults(aResultIndex);
}

//=================================================================================================
void FeaturesPlugin_Pipe::storeResult(const std::shared_ptr<GeomAPI_Shape> theBaseShape,
                                      GeomAlgoAPI_Pipe& thePipeAlgo,
                                      const int theResultIndex)
{
  // Create result body.
  ResultBodyPtr aResultBody = document()->createBody(data(), theResultIndex);

  // Store generated shape.
  aResultBody->storeGenerated(theBaseShape, thePipeAlgo.shape());

  // Store generated edges/faces.
  GeomAPI_Shape::ShapeType aBaseShapeType = theBaseShape->shapeType();
  GeomAPI_Shape::ShapeType aShapeTypeToExplode;
  int aGenTag = 1;
  std::string aGenName = "Generated_";

  std::shared_ptr<GeomAPI_DataMapOfShapeShape> aMapOfSubShapes = thePipeAlgo.mapOfSubShapes();
  switch(aBaseShapeType) {
    case GeomAPI_Shape::VERTEX: {
      aShapeTypeToExplode = GeomAPI_Shape::VERTEX;
      aGenName += "Edge";
      break;
    }
    case GeomAPI_Shape::EDGE:
    case GeomAPI_Shape::WIRE: {
      std::shared_ptr<GeomAPI_Vertex> aV1, aV2;
      GeomAlgoAPI_ShapeTools::findBounds(theBaseShape, aV1, aV2);
      ListOfShape aV1History, aV2History;
      thePipeAlgo.generated(aV1, aV1History);
      thePipeAlgo.generated(aV2, aV2History);
      if(!aV1History.empty()) {
        aResultBody->generated(aV1, aV1History.front(), aGenName + "Edge_1", aGenTag++);
      }
      if(!aV2History.empty()) {
        aResultBody->generated(aV2, aV2History.front(), aGenName + "Edge_2", aGenTag++);
      }
    }
    case GeomAPI_Shape::FACE:
    case GeomAPI_Shape::SHELL: {
      aShapeTypeToExplode = GeomAPI_Shape::EDGE;
      aGenName += "Face";
      break;
    }
  }
  aResultBody->loadAndOrientGeneratedShapes(&thePipeAlgo, theBaseShape, aShapeTypeToExplode, aGenTag++, aGenName, *aMapOfSubShapes.get());

  // Store from shapes.
  int aFromTag = aGenTag;
  storeShapes(aResultBody, aBaseShapeType, aMapOfSubShapes, thePipeAlgo.fromShapes(), "From_", aFromTag);

  // Store to shapes.
  int aToTag = aFromTag;
  storeShapes(aResultBody, aBaseShapeType, aMapOfSubShapes, thePipeAlgo.toShapes(), "To_", aToTag);

  setResult(aResultBody, theResultIndex);
}

//=================================================================================================
void FeaturesPlugin_Pipe::storeResult(const ListOfShape& theBaseShapes,
                                      GeomAlgoAPI_Pipe& thePipeAlgo,
                                      const int theResultIndex)
{
  // Create result body.
  ResultBodyPtr aResultBody = document()->createBody(data(), theResultIndex);

  // Store generated shape.
  aResultBody->storeGenerated(theBaseShapes.front(), thePipeAlgo.shape());

  // Store generated edges/faces.
  int aGenTag = 1;
  std::shared_ptr<GeomAPI_DataMapOfShapeShape> aMapOfSubShapes = thePipeAlgo.mapOfSubShapes();

  for(ListOfShape::const_iterator anIter = theBaseShapes.cbegin(); anIter != theBaseShapes.cend(); anIter++) {
    GeomShapePtr aBaseShape = *anIter;
    GeomAPI_Shape::ShapeType aBaseShapeType = aBaseShape->shapeType();
    GeomAPI_Shape::ShapeType aShapeTypeToExplode;
    std::string aGenName = "Generated_";
    switch(aBaseShapeType) {
      case GeomAPI_Shape::VERTEX: {
        aShapeTypeToExplode = GeomAPI_Shape::VERTEX;
        aGenName += "Edge";
        break;
      }
      case GeomAPI_Shape::EDGE:
      case GeomAPI_Shape::WIRE: {
        std::shared_ptr<GeomAPI_Vertex> aV1, aV2;
        GeomAlgoAPI_ShapeTools::findBounds(aBaseShape, aV1, aV2);
        ListOfShape aV1History, aV2History;
        thePipeAlgo.generated(aV1, aV1History);
        thePipeAlgo.generated(aV2, aV2History);
        aResultBody->generated(aV1, aV1History.front(), aGenName + "Edge_1", aGenTag++);
        aResultBody->generated(aV2, aV2History.front(), aGenName + "Edge_2", aGenTag++);
      }
      case GeomAPI_Shape::FACE:
      case GeomAPI_Shape::SHELL: {
        aShapeTypeToExplode = GeomAPI_Shape::EDGE;
        aGenName += "Face";
        break;
      }
    }
    aResultBody->loadAndOrientGeneratedShapes(&thePipeAlgo, aBaseShape, aShapeTypeToExplode, aGenTag++, aGenName, *aMapOfSubShapes.get());
  }

  // Store from shapes.
  int aFromTag = aGenTag;
  storeShapes(aResultBody, theBaseShapes.front()->shapeType(), aMapOfSubShapes, thePipeAlgo.fromShapes(), "From", aFromTag);

  // Store to shapes.
  int aToTag = aFromTag;
  storeShapes(aResultBody, theBaseShapes.back()->shapeType(), aMapOfSubShapes, thePipeAlgo.toShapes(), "To", aToTag);


  setResult(aResultBody, theResultIndex);
}

//=================================================================================================
void FeaturesPlugin_Pipe::storeShapes(ResultBodyPtr theResultBody,
                                      const GeomAPI_Shape::ShapeType theBaseShapeType,
                                      const std::shared_ptr<GeomAPI_DataMapOfShapeShape> theMapOfSubShapes,
                                      const ListOfShape& theShapes,
                                      const std::string theName,
                                      int& theTag)
{
  GeomAPI_Shape::ShapeType aShapeTypeToExplore = GeomAPI_Shape::FACE;
  std::string aShapeTypeStr = "Face";
  switch(theBaseShapeType) {
    case GeomAPI_Shape::VERTEX: {
      aShapeTypeToExplore = GeomAPI_Shape::VERTEX;
      aShapeTypeStr = "Vertex";
      break;
    }
    case GeomAPI_Shape::EDGE:
    case GeomAPI_Shape::WIRE: {
      aShapeTypeToExplore = GeomAPI_Shape::EDGE;
      aShapeTypeStr = "Edge";
      break;
    }
    case GeomAPI_Shape::FACE:
    case GeomAPI_Shape::SHELL: {
      aShapeTypeToExplore = GeomAPI_Shape::FACE;
      aShapeTypeStr = "Face";
      break;
    }
  }

  // Store shapes.
  int aShapeIndex = 1;
  std::string aName = theName + aShapeTypeStr;
  for(ListOfShape::const_iterator anIt = theShapes.cbegin(); anIt != theShapes.cend(); ++anIt) {
    std::shared_ptr<GeomAPI_Shape> aShape = *anIt;
    for(GeomAPI_ShapeExplorer anExp(aShape, aShapeTypeToExplore); anExp.more(); anExp.next()) {
      std::shared_ptr<GeomAPI_Shape> aSubShape = anExp.current();
      if(theMapOfSubShapes->isBound(aSubShape)) {
        aSubShape = theMapOfSubShapes->find(aSubShape);
      }
      std::ostringstream aStr;
      aStr << aName << "_" << aShapeIndex++;
      theResultBody->generated(aSubShape, aStr.str(), theTag++);
    }
  }
}
