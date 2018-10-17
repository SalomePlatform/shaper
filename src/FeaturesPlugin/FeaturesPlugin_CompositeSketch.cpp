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
// See http://www.salome-platform.org/ or
// email : webmaster.salome@opencascade.com<mailto:webmaster.salome@opencascade.com>
//

#include <FeaturesPlugin_CompositeSketch.h>

#include <ModelAPI_AttributeSelectionList.h>
#include <ModelAPI_AttributeReference.h>
#include <ModelAPI_BodyBuilder.h>
#include <ModelAPI_ResultConstruction.h>
#include <ModelAPI_Session.h>
#include <ModelAPI_Validator.h>

#include <GeomAlgoAPI_CompoundBuilder.h>
#include <GeomAlgoAPI_Prism.h>
#include <GeomAlgoAPI_Revolution.h>
#include <GeomAlgoAPI_ShapeTools.h>
#include <GeomAlgoAPI_SketchBuilder.h>

#include <GeomAPI_PlanarEdges.h>
#include <GeomAPI_ShapeExplorer.h>

#include <map>
#include <sstream>

static void storeSubShape(ResultBodyPtr theResultBody,
                          const GeomShapePtr theShape,
                          const GeomAPI_Shape::ShapeType theType,
                          const std::string theName,
                          int& theShapeIndex);

//=================================================================================================
void FeaturesPlugin_CompositeSketch::initCompositeSketchAttribtues(const int theInitFlags)
{
  // Initialize sketch launcher.
  if(theInitFlags & InitSketchLauncher) {
    data()->addAttribute(SKETCH_ID(), ModelAPI_AttributeReference::typeId());
    ModelAPI_Session::get()->validators()->registerNotObligatory(getKind(), SKETCH_ID());
  }

  // Initialize selection list.
  if(theInitFlags & InitBaseObjectsList) {
    data()->addAttribute(BASE_OBJECTS_ID(), ModelAPI_AttributeSelectionList::typeId());
  }
}

//=================================================================================================
std::shared_ptr<ModelAPI_Feature> FeaturesPlugin_CompositeSketch::addFeature(std::string theID)
{
  FeaturePtr aNew = document()->addFeature(theID, false);
  if(aNew) {
    data()->reference(SKETCH_ID())->setValue(aNew);
  }

  // Set as current also after it becomes sub to set correctly enabled for other sketch subs.
  document()->setCurrentFeature(aNew, false);
  return aNew;
}

//=================================================================================================
int FeaturesPlugin_CompositeSketch::numberOfSubs(bool forTree) const
{
  ObjectPtr aObj = data()->reference(SKETCH_ID())->value();
  return aObj.get() ? 1 : 0;
}

//=================================================================================================
std::shared_ptr<ModelAPI_Feature> FeaturesPlugin_CompositeSketch::subFeature(const int theIndex,
                                                                             bool forTree)
{
  if(theIndex == 0) {
    return std::dynamic_pointer_cast<ModelAPI_Feature>(data()->reference(SKETCH_ID())->value());
  }

  return std::shared_ptr<ModelAPI_Feature>();
}

//=================================================================================================
int FeaturesPlugin_CompositeSketch::subFeatureId(const int theIndex) const
{
  if(theIndex == 0) {
    FeaturePtr aFeature =
      std::dynamic_pointer_cast<ModelAPI_Feature>(data()->reference(SKETCH_ID())->value());
    if(aFeature.get()) {
      return aFeature->data()->featureId();
    }
  }

  return -1;
}

//=================================================================================================
bool FeaturesPlugin_CompositeSketch::isSub(ObjectPtr theObject) const
{
  // Check is this feature of result
  FeaturePtr aFeature = std::dynamic_pointer_cast<ModelAPI_Feature>(theObject);
  if(!aFeature.get()) {
    return false;
  }

  ObjectPtr aSub = data()->reference(SKETCH_ID())->value();
  return aSub == theObject;
}

//=================================================================================================
void FeaturesPlugin_CompositeSketch::removeFeature(std::shared_ptr<ModelAPI_Feature> theFeature)
{
  AttributeSelectionListPtr aBaseObjectsSelectionList = selectionList(BASE_OBJECTS_ID());
  if(aBaseObjectsSelectionList.get() && aBaseObjectsSelectionList->size() > 0) {
    aBaseObjectsSelectionList->clear();
  }

  reference(SKETCH_ID())->setValue(ObjectPtr());
}

//=================================================================================================
void FeaturesPlugin_CompositeSketch::getBaseShapes(ListOfShape& theBaseShapesList,
                                                   const bool theIsMakeShells)
{
  theBaseShapesList.clear();

  ListOfShape aBaseFacesList;
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
      setError("Error: Selected base object is empty.");
      return;
    }
    GeomShapePtr aBaseShape = aBaseObjectSelection->value();
    if(aBaseShape.get() && !aBaseShape->isNull()) {
      GeomAPI_Shape::ShapeType aST = aBaseShape->shapeType();
      if(aST == GeomAPI_Shape::SOLID || aST == GeomAPI_Shape::COMPSOLID) {
        setError("Error: Selected shapes has unsupported type.");
        return;
      }
      ResultConstructionPtr aConstruction =
        std::dynamic_pointer_cast<ModelAPI_ResultConstruction>(aBaseObjectSelection->context());
      if(aConstruction.get() && !aBaseShape->isEqual(aConstruction->shape()) &&
          aST == GeomAPI_Shape::WIRE) {
        // It is a wire on the sketch, store it to make face later.
        aSketchWiresMap[aConstruction].push_back(aBaseShape);
        continue;
      } else {
      aST == GeomAPI_Shape::FACE ? aBaseFacesList.push_back(aBaseShape) :
                                   theBaseShapesList.push_back(aBaseShape);
      }
    } else {
      // This may be the whole sketch result selected, check and get faces.
      ResultConstructionPtr aConstruction =
        std::dynamic_pointer_cast<ModelAPI_ResultConstruction>(aBaseObjectSelection->context());
      if(!aConstruction.get()) {
        setError("Error: Selected sketches does not have results.");
        return;
      }
      int aFacesNum = aConstruction->facesNum();
      if(aFacesNum == 0) {
        // Probably it can be construction.
        aBaseShape = aConstruction->shape();
        if(aBaseShape.get() && !aBaseShape->isNull()) {
          GeomAPI_Shape::ShapeType aST = aBaseShape->shapeType();
          if(aST != GeomAPI_Shape::VERTEX && aST != GeomAPI_Shape::EDGE &&
             aST != GeomAPI_Shape::WIRE &&
             aST != GeomAPI_Shape::FACE && aST != GeomAPI_Shape::SHELL) {
            setError("Error: Selected shapes has unsupported type.");
            return;
          }
          aST == GeomAPI_Shape::FACE ? aBaseFacesList.push_back(aBaseShape) :
                                       theBaseShapesList.push_back(aBaseShape);
        }
      } else {
        for(int aFaceIndex = 0; aFaceIndex < aFacesNum; aFaceIndex++) {
          GeomShapePtr aBaseFace = aConstruction->face(aFaceIndex);
          if(!aBaseFace.get() || aBaseFace->isNull()) {
            setError("Error: One of the faces on selected sketch is null.");
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
  if(theIsMakeShells && aBaseFacesList.size() > 1) {
    ListOfShape aShells;
    ListOfShape aFreeFaces;
    GeomShapePtr aFacesCompound = GeomAlgoAPI_CompoundBuilder::compound(aBaseFacesList);
    GeomAlgoAPI_ShapeTools::combineShapes(aFacesCompound, GeomAPI_Shape::SHELL,
                                          aShells, aFreeFaces);
    theBaseShapesList.insert(theBaseShapesList.end(), aFreeFaces.begin(), aFreeFaces.end());
    theBaseShapesList.insert(theBaseShapesList.end(), aShells.begin(), aShells.end());
  } else {
    theBaseShapesList.insert(theBaseShapesList.end(), aBaseFacesList.begin(),
                             aBaseFacesList.end());
  }
}

//=================================================================================================
bool FeaturesPlugin_CompositeSketch::isMakeShapeValid(
  const std::shared_ptr<GeomAlgoAPI_MakeShape> theMakeShape)
{
  // Check that algo is done.
  if(!theMakeShape->isDone()) {
    setError("Error: " + getKind() + " algorithm failed.");
    return false;
  }

  // Check if shape is not null.
  if(!theMakeShape->shape().get() || theMakeShape->shape()->isNull()) {
    setError("Error: Resulting shape is null.");
    return false;
  }

  // Check that resulting shape is valid.
  if(!theMakeShape->isValid()) {
    setError("Error: Resulting shape is not valid.");
    return false;
  }

  return true;
}

//=================================================================================================
void FeaturesPlugin_CompositeSketch::storeResult(const GeomShapePtr theBaseShape,
                                        const std::shared_ptr<GeomAlgoAPI_MakeShape> theMakeShape,
                                        const int theIndex)
{
  // Create result body.
  ResultBodyPtr aResultBody = document()->createBody(data(), theIndex);

  // Store generated shape.
  aResultBody->storeGenerated(theBaseShape, theMakeShape->shape());

  // Store generated edges/faces.
  storeGenerationHistory(aResultBody, theBaseShape, theMakeShape);

  setResult(aResultBody, theIndex);
}

//=================================================================================================
void FeaturesPlugin_CompositeSketch::storeGenerationHistory(ResultBodyPtr theResultBody,
                                        const GeomShapePtr theBaseShape,
                                        const std::shared_ptr<GeomAlgoAPI_MakeShape> theMakeShape)
{
  GeomAPI_Shape::ShapeType aBaseShapeType = theBaseShape->shapeType();
  GeomAPI_Shape::ShapeType aShapeTypeToExplode = GeomAPI_Shape::SHAPE;

  switch(aBaseShapeType) {
    case GeomAPI_Shape::EDGE: {
            aShapeTypeToExplode = GeomAPI_Shape::VERTEX;
      break;
    }
    case GeomAPI_Shape::WIRE: {
      aShapeTypeToExplode = GeomAPI_Shape::COMPOUND;
      break;
    }
    case GeomAPI_Shape::FACE:
    case GeomAPI_Shape::SHELL: {
      aShapeTypeToExplode = GeomAPI_Shape::EDGE;
      break;
    }
    case GeomAPI_Shape::COMPOUND: {
      aShapeTypeToExplode = GeomAPI_Shape::COMPOUND;
    }
  }

  int aLateralIndex = 1;
  int aBaseEdgeIndex = 1;
  int aVertexIndex = 1;
  int aBaseVertexIndex = 1;

  if(aShapeTypeToExplode == GeomAPI_Shape::VERTEX ||
      aShapeTypeToExplode == GeomAPI_Shape::COMPOUND) {
    theResultBody->loadGeneratedShapes(theMakeShape, theBaseShape, GeomAPI_Shape::VERTEX);
  }
  if(aShapeTypeToExplode == GeomAPI_Shape::EDGE ||
      aShapeTypeToExplode == GeomAPI_Shape::COMPOUND) {
    theResultBody->loadGeneratedShapes(theMakeShape, theBaseShape, GeomAPI_Shape::EDGE);
  }
  // issue #2197: make naming of edges generated from vertices
  if (aShapeTypeToExplode == GeomAPI_Shape::EDGE) {
    GeomAPI_DataMapOfShapeShape aFacesFromFromEdges;
    GeomAPI_ShapeExplorer anEdgeExp(theBaseShape, GeomAPI_Shape::EDGE);
    for(; anEdgeExp.more(); anEdgeExp.next()) {
      ListOfShape aGenerated;
      theMakeShape->generated(anEdgeExp.current(), aGenerated);
      ListOfShape::iterator aGenIter = aGenerated.begin();
      for(; aGenIter != aGenerated.end(); aGenIter++) {
        GeomShapePtr aGen = *aGenIter;
        if (aGen.get() && !aGen->isNull()) {
          if ((*aGenIter)->shapeType() == GeomAPI_Shape::FACE) { // normal case
            aFacesFromFromEdges.bind(aGen, anEdgeExp.current());
          }
        }
      }
    }

    // closed revolution of 1-3 faces can not distinguish lateral and base edges
    if (aFacesFromFromEdges.size() <= 3) {
      bool isClosed = false; // lateral edges are closed (in full revolution)
      GeomAPI_DataMapOfShapeShape anEdgesFromVertices;
      GeomAPI_ShapeExplorer aVertExp(theBaseShape, GeomAPI_Shape::VERTEX);
      for(; aVertExp.more(); aVertExp.next()) {
        ListOfShape aGenerated;
        theMakeShape->generated(aVertExp.current(), aGenerated);
        ListOfShape::iterator aGenIter = aGenerated.begin();
        for(; aGenIter != aGenerated.end(); aGenIter++) {
          std::shared_ptr<GeomAPI_Shape> aGenerated = *aGenIter;
          if (anEdgesFromVertices.isBound(aGenerated)) // already here
            continue;
          std::ostringstream aStream;
          aStream << "Lateral_" << aLateralIndex++;
          theResultBody->generated(aGenerated, aStream.str());

          anEdgesFromVertices.bind(aGenerated, aVertExp.current());
          std::shared_ptr<GeomAPI_Edge> anEdge(new GeomAPI_Edge(aGenerated));
          isClosed = isClosed || anEdge->isClosed();
        }
      }
      if (isClosed) {
        GeomAPI_ShapeExplorer anEdgesExp(theMakeShape->shape(), GeomAPI_Shape::EDGE);
        for(; anEdgesExp.more(); anEdgesExp.next()) {
          if (!anEdgesFromVertices.isBound(anEdgesExp.current())) {
            // found a base edge
            std::ostringstream aStream;
            aStream << "Base_Edge_" << aBaseEdgeIndex++;
            theResultBody->generated(anEdgesExp.current(), aStream.str());
            // only one orientation is needed
            anEdgesFromVertices.bind(anEdgesExp.current(), anEdgesExp.current());
          }
        }
      } else if (aFacesFromFromEdges.size() == 1) { // 2233: sphere created by the revolution:
        // vertices at degenerated edges will have the same name
        GeomAPI_DataMapOfShapeShape aVertices;
        GeomAPI_ShapeExplorer aVertExp(theMakeShape->shape(), GeomAPI_Shape::VERTEX);
        for(int anIndex = 1; aVertExp.more(); aVertExp.next()) {
          if (!aVertices.isBound(aVertExp.current())) {
            // found a base edge
            std::ostringstream aStream;
            aStream << "Vertex_" << aVertexIndex++;
            theResultBody->generated(aVertExp.current(), aStream.str());
            // only one orientation is needed
            aVertices.bind(aVertExp.current(), aVertExp.current());
          }
        }
      }
    } else { // issue #2197, test case 4 : edges that produce fully-revolved face,
      // but contain only 2 edges (on apex of revolution)
      GeomAPI_ShapeExplorer anEdgeExp(theBaseShape, GeomAPI_Shape::EDGE);
      for(; anEdgeExp.more(); anEdgeExp.next()) {
        ListOfShape aGenerated;
        theMakeShape->generated(anEdgeExp.current(), aGenerated);
        ListOfShape::iterator aGenIter = aGenerated.begin();
        for(; aGenIter != aGenerated.end(); aGenIter++) {
          GeomShapePtr aGen = (*aGenIter);
          if (aGen.get() && !aGen->isNull()) {
            GeomAPI_ShapeExplorer aFaceEdgeExp(aGen, GeomAPI_Shape::EDGE);
            int aNumEdges = 0;
            int aNumClosed = 0;
            GeomShapePtr aNotClosedEdge;
            GeomEdgePtr aDegenerateEdge;
            GeomAPI_DataMapOfShapeShape alreadyIterated;
            for(; aFaceEdgeExp.more(); aFaceEdgeExp.next()) {
              std::shared_ptr<GeomAPI_Edge> anEdge(new GeomAPI_Edge(aFaceEdgeExp.current()));
              if (anEdge->isDegenerated()) {
                aDegenerateEdge = anEdge;
                continue;
              }
              if (alreadyIterated.isBound(anEdge))
                continue;
              alreadyIterated.bind(anEdge, anEdge);
              aNumEdges++;
              if (anEdge->isClosed()) {
                aNumClosed++;
              } else {
                aNotClosedEdge = anEdge;
              }
            }
            if (aNumEdges == 2 && aNumClosed == 1) {
              std::ostringstream aStream;
              aStream << "Base_Edge_" << aBaseEdgeIndex++;
              theResultBody->generated(aNotClosedEdge, aStream.str());
              if (aDegenerateEdge.get()) { // export vertex of the degenerated edge (apex) #2520
                GeomAPI_ShapeExplorer anEdgeExp(aDegenerateEdge, GeomAPI_Shape::VERTEX);
                if (anEdgeExp.more()) {
                  std::ostringstream aStream;
                  aStream << "Base_Vertex_" << aBaseVertexIndex++;
                  theResultBody->generated(anEdgeExp.current(), aStream.str());
                }
              }
            }
          }
        }
      }
    }
  }

  std::shared_ptr<GeomAlgoAPI_MakeSweep> aMakeSweep =
    std::dynamic_pointer_cast<GeomAlgoAPI_MakeSweep>(theMakeShape);
  if(aMakeSweep.get()) {
    // Store from shapes.
    storeShapes(theResultBody, aBaseShapeType, aMakeSweep->fromShapes(), "From_");

    // Store to shapes.
    storeShapes(theResultBody, aBaseShapeType, aMakeSweep->toShapes(), "To_");
  }
}

//=================================================================================================
void FeaturesPlugin_CompositeSketch::storeShapes(ResultBodyPtr theResultBody,
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
  }

  // Store shapes.
  int aShapeIndex = 1;
  int aFaceIndex = 1;
  for(ListOfShape::const_iterator anIt = theShapes.cbegin(); anIt != theShapes.cend(); ++anIt) {
    GeomShapePtr aShape = *anIt;

    if(aShapeTypeToExplore == GeomAPI_Shape::COMPOUND) {
      std::string aName = theName + (aShape->shapeType() == GeomAPI_Shape::EDGE ? "Edge" : "Face");
      storeSubShape(theResultBody,
                    aShape,
                    aShape->shapeType(),
                    aName,
                    aShape->shapeType() == GeomAPI_Shape::EDGE ? aShapeIndex : aFaceIndex);
    } else {
      std::string aName = theName + aShapeTypeStr;
      storeSubShape(theResultBody,
                    aShape,
                    aShapeTypeToExplore,
                    aName,
                    aShapeIndex);
      if (theBaseShapeType == GeomAPI_Shape::WIRE) { // issue 2289: special names also for vertices
        aName = theName + "Vertex";
        storeSubShape(theResultBody,
                      aShape,
                      GeomAPI_Shape::VERTEX,
                      aName,
                      aShapeIndex);
      }
    }
  }
}

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
