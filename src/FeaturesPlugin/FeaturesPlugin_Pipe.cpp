// Copyright (C) 2014-2022  CEA/DEN, EDF R&D
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
#include <GeomAlgoAPI_Tools.h>

#include <GeomAPI_PlanarEdges.h>
#include <GeomAPI_ShapeExplorer.h>

#include <map>
#include <sstream>

static void storeSubShape(ResultBodyPtr theResultBody,
                          const GeomShapePtr theShape,
                          const GeomAPI_Shape::ShapeType theType,
                          const std::string theName,
                          int& theShapeIndex);

//==================================================================================================
FeaturesPlugin_Pipe::FeaturesPlugin_Pipe()
{
}

//==================================================================================================
void FeaturesPlugin_Pipe::initAttributes()
{
  data()->addAttribute(CREATION_METHOD(), ModelAPI_AttributeString::typeId());

  data()->addAttribute(BASE_OBJECTS_ID(), ModelAPI_AttributeSelectionList::typeId());
  data()->addAttribute(PATH_OBJECT_ID(), ModelAPI_AttributeSelection::typeId());

  data()->addAttribute(BINORMAL_ID(), ModelAPI_AttributeSelection::typeId());

  data()->addAttribute(LOCATIONS_ID(), ModelAPI_AttributeSelectionList::typeId());
  ModelAPI_Session::get()->validators()->registerNotObligatory(getKind(), LOCATIONS_ID());
}

//==================================================================================================
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
      if(aConstruction.get() && !aBaseShape->isEqual(aConstruction->shape()) &&
          aST == GeomAPI_Shape::WIRE) {
        // It is a wire on the sketch, store it to make face later.
        aSketchWiresMap[aConstruction].push_back(aBaseShape);
        continue;
      } else {
      aST == GeomAPI_Shape::FACE ? aBaseFacesList.push_back(aBaseShape) :
                                   aBaseShapesList.push_back(aBaseShape);
      }
    } else {
      // This may be the whole sketch result selected, check and get faces.
      ResultConstructionPtr aConstruction =
        std::dynamic_pointer_cast<ModelAPI_ResultConstruction>(aBaseObjectSelection->context());
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
          std::shared_ptr<GeomAPI_Shape> aBaseFace =
            std::dynamic_pointer_cast<GeomAPI_Shape>(aConstruction->face(aFaceIndex));
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
  if(aCreationMethod == CREATION_METHOD_SIMPLE()) {
    std::shared_ptr<GeomAPI_Shape> aFacesCompound =
      GeomAlgoAPI_CompoundBuilder::compound(aBaseFacesList);
    GeomAlgoAPI_ShapeTools::combineShapes(aFacesCompound, GeomAPI_Shape::SHELL, aBaseShapesList);
  } else {
    aBaseShapesList.insert(aBaseShapesList.end(), aBaseFacesList.begin(), aBaseFacesList.end());
  }

  // Getting path.
  AttributeSelectionPtr aPathSelection = selection(PATH_OBJECT_ID());
  if(!aPathSelection.get()) {
    setError("Error: Path selection is empty.");
    return;
  }
  std::shared_ptr<GeomAPI_Shape> aPathShape =
    std::dynamic_pointer_cast<GeomAPI_Shape>(aPathSelection->value());
  if(!aPathShape.get() && aPathSelection->context().get()) {
    // Probably it is a construction.
    aPathShape = aPathSelection->context()->shape();
  }
  if(!aPathShape.get() || aPathShape->isNull()) {
    setError("Error: Path shape is null.");
    return;
  }

  // Getting Bi-Normal
  std::shared_ptr<GeomAPI_Shape> aBiNormal;
  if(aCreationMethod == CREATION_METHOD_BINORMAL()) {
    AttributeSelectionPtr aBiNormalSelection = selection(BINORMAL_ID());
    if(!aBiNormalSelection.get()) {
      setError("Error: Bi-Normal selection is empty.");
      return;
    }
    aBiNormal = std::dynamic_pointer_cast<GeomAPI_Shape>(aBiNormalSelection->value());
    if(!aBiNormal.get() && aBiNormalSelection->context().get()) {
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
  if(aCreationMethod == CREATION_METHOD_LOCATIONS()) {
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
      if(!aLocationShape.get() && aLocationSelection->context().get()) {
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
  std::string anError;
  if(aCreationMethod == CREATION_METHOD_SIMPLE() ||
      aCreationMethod == CREATION_METHOD_BINORMAL()) {
    for(ListOfShape::const_iterator
        anIter = aBaseShapesList.cbegin(); anIter != aBaseShapesList.cend(); anIter++) {
      std::shared_ptr<GeomAPI_Shape> aBaseShape = *anIter;

      std::shared_ptr<GeomAlgoAPI_Pipe> aPipeAlgo(
        aCreationMethod == CREATION_METHOD_SIMPLE() ? new GeomAlgoAPI_Pipe(aBaseShape,
                                                                           aPathShape)
                                                    : new GeomAlgoAPI_Pipe(aBaseShape,
                                                                           aPathShape,
                                                                           aBiNormal));

      if (GeomAlgoAPI_Tools::AlgoError::isAlgorithmFailed(aPipeAlgo, getKind(), anError)) {
        setError(anError);
        aResultIndex = 0;
        break;
      }

      storeResult(aBaseShape, aPathShape, aPipeAlgo, aResultIndex++);
    }
  } else if(aCreationMethod == CREATION_METHOD_LOCATIONS()) {
    std::shared_ptr<GeomAlgoAPI_Pipe> aPipeAlgo(new GeomAlgoAPI_Pipe(aBaseShapesList,
                                                                     aLocations,
                                                                     aPathShape));

    if (GeomAlgoAPI_Tools::AlgoError::isAlgorithmFailed(aPipeAlgo, getKind(), anError)) {
      setError(anError);
      removeResults(0);
      return;
    }

    storeResult(aBaseShapesList, aPathShape, aPipeAlgo, aResultIndex++);
  } else {
    setError("Error: Wrong creation method.");
    return;
  }

  removeResults(aResultIndex);
}

//==================================================================================================
void FeaturesPlugin_Pipe::storeResult(const std::shared_ptr<GeomAPI_Shape> theBaseShape,
                                      const std::shared_ptr<GeomAPI_Shape> thePathShape,
                                      const std::shared_ptr<GeomAlgoAPI_Pipe> thePipeAlgo,
                                      const int theResultIndex)
{
  // Create result body.
  ResultBodyPtr aResultBody = document()->createBody(data(), theResultIndex);

  // Store generated shape.
  aResultBody->storeGenerated(theBaseShape, thePipeAlgo->shape());

  // Store generated edges/faces.
  GeomAPI_Shape::ShapeType aBaseShapeType = theBaseShape->shapeType();
  GeomAPI_Shape::ShapeType aShapeTypeToExplode = GeomAPI_Shape::SHAPE;

  switch(aBaseShapeType) {
    case GeomAPI_Shape::VERTEX: {
      aShapeTypeToExplode = GeomAPI_Shape::VERTEX;
      break;
    }
    case GeomAPI_Shape::EDGE:
    case GeomAPI_Shape::WIRE: {
      std::shared_ptr<GeomAPI_Vertex> aV1, aV2;
      GeomAlgoAPI_ShapeTools::findBounds(theBaseShape, aV1, aV2);
      ListOfShape aV1History, aV2History;
      thePipeAlgo->generated(aV1, aV1History);
      thePipeAlgo->generated(aV2, aV2History);
      if(!aV1History.empty()) {
        aResultBody->generated(aV1, aV1History.front());
      }
      if(!aV2History.empty()) {
        aResultBody->generated(aV2, aV2History.front());
      }
    }
    case GeomAPI_Shape::FACE:
    case GeomAPI_Shape::SHELL: {
      aShapeTypeToExplode = GeomAPI_Shape::EDGE;
      break;
    }
    case GeomAPI_Shape::COMPOUND: {
      aShapeTypeToExplode = GeomAPI_Shape::COMPOUND;
      break;
    }
    default:
      return; // unsupported type of shape
  }

  if(aShapeTypeToExplode == GeomAPI_Shape::VERTEX ||
      aShapeTypeToExplode == GeomAPI_Shape::COMPOUND) {
    aResultBody->loadGeneratedShapes(thePipeAlgo, theBaseShape, GeomAPI_Shape::VERTEX);
  }
  if(aShapeTypeToExplode == GeomAPI_Shape::EDGE ||
      aShapeTypeToExplode == GeomAPI_Shape::COMPOUND) {
    aResultBody->loadGeneratedShapes(thePipeAlgo, theBaseShape, GeomAPI_Shape::EDGE);
  }
  if (thePathShape.get())
    aResultBody->loadGeneratedShapes(thePipeAlgo, thePathShape, GeomAPI_Shape::EDGE);

  // Store from shapes.
  storeShapes(aResultBody, aBaseShapeType, thePipeAlgo->fromShapes(), "From_");

  // Store to shapes.
  storeShapes(aResultBody, aBaseShapeType, thePipeAlgo->toShapes(), "To_");

  setResult(aResultBody, theResultIndex);
}

//==================================================================================================
void FeaturesPlugin_Pipe::storeResult(const ListOfShape& theBaseShapes,
                                      const std::shared_ptr<GeomAPI_Shape> thePathShape,
                                      const std::shared_ptr<GeomAlgoAPI_Pipe> thePipeAlgo,
                                      const int theResultIndex)
{
  // Create result body.
  ResultBodyPtr aResultBody = document()->createBody(data(), theResultIndex);

  // Store generated shape.
  aResultBody->storeGenerated(theBaseShapes.front(), thePipeAlgo->shape());

  // Store generated edges/faces.
  for(ListOfShape::const_iterator anIter = theBaseShapes.cbegin();
      anIter != theBaseShapes.cend();
      ++anIter)
  {
    GeomShapePtr aBaseShape = *anIter;
    GeomAPI_Shape::ShapeType aBaseShapeType = aBaseShape->shapeType();
    GeomAPI_Shape::ShapeType aShapeTypeToExplode = GeomAPI_Shape::SHAPE;
    switch(aBaseShapeType) {
      case GeomAPI_Shape::VERTEX: {
        aShapeTypeToExplode = GeomAPI_Shape::VERTEX;
        break;
      }
      case GeomAPI_Shape::EDGE:
      case GeomAPI_Shape::WIRE: {
        std::shared_ptr<GeomAPI_Vertex> aV1, aV2;
        GeomAlgoAPI_ShapeTools::findBounds(aBaseShape, aV1, aV2);
        ListOfShape aV1History, aV2History;
        thePipeAlgo->generated(aV1, aV1History);
        thePipeAlgo->generated(aV2, aV2History);
        if (!aV1History.empty())
          aResultBody->generated(aV1, aV1History.front());
        if (!aV2History.empty())
          aResultBody->generated(aV2, aV2History.front());
      }
      case GeomAPI_Shape::FACE:
      case GeomAPI_Shape::SHELL: {
        aShapeTypeToExplode = GeomAPI_Shape::EDGE;
        break;
      }
      default:
        return; // unsupported type of shape
    }
    aResultBody->loadGeneratedShapes(thePipeAlgo, aBaseShape, aShapeTypeToExplode);
  }

  if (thePathShape.get())
    aResultBody->loadGeneratedShapes(thePipeAlgo, thePathShape, GeomAPI_Shape::EDGE);

  // Store from shapes.
  storeShapes(aResultBody, theBaseShapes.front()->shapeType(), thePipeAlgo->fromShapes(), "From_");

  // Store to shapes.
  storeShapes(aResultBody, theBaseShapes.back()->shapeType(), thePipeAlgo->toShapes(), "To_");


  setResult(aResultBody, theResultIndex);
}

//==================================================================================================
void FeaturesPlugin_Pipe::storeShapes(ResultBodyPtr theResultBody,
                                      const GeomAPI_Shape::ShapeType theBaseShapeType,
                                      const ListOfShape& theShapes,
                                      const std::string theName)
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
    case GeomAPI_Shape::COMPOUND: {
      aShapeTypeToExplore = GeomAPI_Shape::COMPOUND;
      break;
    }
    default:
      return; // unsupported type of shape
  }

  // Store shapes.
  int aShapeIndex = 1;
  int aFaceIndex = 1;
  for(ListOfShape::const_iterator anIt = theShapes.cbegin(); anIt != theShapes.cend(); ++anIt) {
    GeomShapePtr aShape = *anIt;

    if(aShapeTypeToExplore == GeomAPI_Shape::COMPOUND) {
      std::string aName = theName + (aShape->shapeType() == GeomAPI_Shape::EDGE ? "Edge" : "Face");
      storeSubShape(theResultBody, aShape,
                    aShape->shapeType(),
                    aName,
                    aShape->shapeType() == GeomAPI_Shape::EDGE ? aShapeIndex : aFaceIndex);
    } else {
      std::string aName = theName + aShapeTypeStr;
      storeSubShape(theResultBody, aShape, aShapeTypeToExplore, aName, aShapeIndex);
    }
  }
}

//==================================================================================================
void storeSubShape(ResultBodyPtr theResultBody,
                   const GeomShapePtr theShape,
                   const GeomAPI_Shape::ShapeType theType,
                   const std::string theName,
                   int& theShapeIndex)
{
  for(GeomAPI_ShapeExplorer anExp(theShape, theType); anExp.more(); anExp.next()) {
    GeomShapePtr aSubShape = anExp.current();
    std::ostringstream aStr;
    aStr << theName << "_" << theShapeIndex++;
    theResultBody->generated(aSubShape, aStr.str());
  }
}
