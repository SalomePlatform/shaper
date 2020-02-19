// Copyright (C) 2014-2019  CEA/DEN, EDF R&D
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

#include "FeaturesPlugin_Tools.h"

#include <ModelAPI_AttributeSelectionList.h>
#include <ModelAPI_ResultBody.h>
#include <ModelAPI_ResultConstruction.h>
#include <ModelAPI_ResultPart.h>
#include <ModelAPI_Tools.h>

#include <GeomAlgoAPI_CompoundBuilder.h>
#include <GeomAlgoAPI_ShapeTools.h>

#include <GeomAPI_PlanarEdges.h>
#include <GeomAPI_ShapeIterator.h>

//==================================================================================================
void FeaturesPlugin_Tools::loadModifiedShapes(ResultBodyPtr theResultBody,
                                              const ListOfShape& theBaseShapes,
                                              const ListOfShape& theTools,
                                              const GeomMakeShapePtr& theMakeShape,
                                              const GeomShapePtr theResultShape,
                                              const std::string& theNamePrefix)
{
  theResultBody->storeModified(theBaseShapes, theResultShape, theMakeShape);

  ListOfShape aShapes = theBaseShapes;
  ListOfShape::const_iterator aToolIter = theTools.cbegin();
  for(; aToolIter != theTools.cend(); aToolIter++)
    aShapes.push_back(*aToolIter);

  for (ListOfShape::const_iterator anIter = aShapes.begin(); anIter != aShapes.end(); ++anIter)
  {
    theResultBody->loadModifiedShapes(theMakeShape, *anIter, GeomAPI_Shape::VERTEX, theNamePrefix);
    theResultBody->loadModifiedShapes(theMakeShape, *anIter, GeomAPI_Shape::EDGE, theNamePrefix);
    theResultBody->loadModifiedShapes(theMakeShape, *anIter, GeomAPI_Shape::FACE, theNamePrefix);
  }
}

//==================================================================================================
void FeaturesPlugin_Tools::loadModifiedShapes(ResultBodyPtr theResultBody,
                                              const GeomShapePtr& theBaseShape,
                                              const GeomMakeShapePtr& theMakeShape,
                                              const std::string theName)
{
  switch(theBaseShape->shapeType()) {
    case GeomAPI_Shape::COMPOUND: {
      for(GeomAPI_ShapeIterator anIt(theBaseShape); anIt.more(); anIt.next())
      {
        loadModifiedShapes(theResultBody,
                           anIt.current(),
                           theMakeShape,
                           theName);
      }
      break;
    }
    case GeomAPI_Shape::COMPSOLID:
    case GeomAPI_Shape::SOLID:
    case GeomAPI_Shape::SHELL: {
      theResultBody->loadModifiedShapes(theMakeShape,
                                        theBaseShape,
                                        GeomAPI_Shape::FACE,
                                        theName);
    }
    case GeomAPI_Shape::FACE:
    case GeomAPI_Shape::WIRE: {
      theResultBody->loadModifiedShapes(theMakeShape,
                                        theBaseShape,
                                        GeomAPI_Shape::EDGE,
                                        theName);
    }
    case GeomAPI_Shape::EDGE: {
      theResultBody->loadModifiedShapes(theMakeShape,
                                        theBaseShape,
                                        GeomAPI_Shape::VERTEX,
                                        theName);
    }
  }
}

//==================================================================================================
void FeaturesPlugin_Tools::loadDeletedShapes(ResultBodyPtr theResultBody,
  const GeomShapePtr theBaseShape,
  const ListOfShape& theTools,
  const GeomMakeShapePtr& theMakeShape,
  const GeomShapePtr theResultShapesCompound)
{
  ListOfShape aShapes = theTools;
  if (theBaseShape.get())
    aShapes.push_front(theBaseShape);

  for (ListOfShape::const_iterator anIter = aShapes.begin(); anIter != aShapes.end(); anIter++)
  {
    theResultBody->loadDeletedShapes(theMakeShape,
                                     *anIter,
                                     GeomAPI_Shape::VERTEX,
                                     theResultShapesCompound);
    theResultBody->loadDeletedShapes(theMakeShape,
                                     *anIter,
                                     GeomAPI_Shape::EDGE,
                                     theResultShapesCompound);
    theResultBody->loadDeletedShapes(theMakeShape,
                                     *anIter,
                                     GeomAPI_Shape::FACE,
                                     theResultShapesCompound);
  }
}

//==================================================================================================
void FeaturesPlugin_Tools::loadDeletedShapes(
  std::vector<ResultBaseAlgo>& theResultBaseAlgoList,
  const ListOfShape& theTools,
  const GeomShapePtr theResultShapesCompound)
{
  for (std::vector<ResultBaseAlgo>::iterator anIt = theResultBaseAlgoList.begin();
       anIt != theResultBaseAlgoList.end();
       ++anIt)
  {
    ResultBaseAlgo& aRCA = *anIt;
    loadDeletedShapes(aRCA.resultBody,
                      aRCA.baseShape,
                      theTools,
                      aRCA.makeShape,
                      theResultShapesCompound);
  }
}

//==================================================================================================
bool FeaturesPlugin_Tools::getShape(const AttributeSelectionListPtr theSelectionList,
                                    const bool theShareTopology,
                                    ListOfShape& theShapesList,
                                    std::string& theError)
{
  theShapesList.clear();

  ListOfShape aBaseFacesList;
  std::map<ResultConstructionPtr, ListOfShape> aSketchWiresMap;
  if(!theSelectionList.get()) {
    theError = "Error: Could not get base objects selection list.";
    return false;
  }
  if(theSelectionList->size() == 0) {
    theError = "Error: Base objects list is empty.";
    return false;
  }
  for(int anIndex = 0; anIndex < theSelectionList->size(); anIndex++) {
    AttributeSelectionPtr aBaseObjectSelection = theSelectionList->value(anIndex);
    if(!aBaseObjectSelection.get()) {
      theError = "Error: Selected base object is empty.";
      return false;
    }
    GeomShapePtr aBaseShape = aBaseObjectSelection->value();
    if(aBaseShape.get() && !aBaseShape->isNull()) {
      GeomAPI_Shape::ShapeType aST = aBaseShape->shapeType();
      if(aST == GeomAPI_Shape::SOLID || aST == GeomAPI_Shape::COMPSOLID) {
        theError = "Error: Selected shapes has unsupported type.";
        return false;
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
                                   theShapesList.push_back(aBaseShape);
      }
    } else {
      // This may be the whole sketch result selected, check and get faces.
      ResultConstructionPtr aConstruction =
        std::dynamic_pointer_cast<ModelAPI_ResultConstruction>(aBaseObjectSelection->context());
      if(!aConstruction.get()) {
        theError = "Error: Selected sketches does not have results.";
        return false;
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
            theError = "Error: Selected shapes has unsupported type.";
            return false;
          }
          aST == GeomAPI_Shape::FACE ? aBaseFacesList.push_back(aBaseShape) :
                                       theShapesList.push_back(aBaseShape);
        }
      } else {
        for(int aFaceIndex = 0; aFaceIndex < aFacesNum; aFaceIndex++) {
          GeomShapePtr aBaseFace = aConstruction->face(aFaceIndex);
          if(!aBaseFace.get() || aBaseFace->isNull()) {
            theError = "Error: One of the faces on selected sketch is null.";
            return false;
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
  if(theShareTopology && aBaseFacesList.size() > 1) {
    GeomShapePtr aFacesCompound = GeomAlgoAPI_CompoundBuilder::compound(aBaseFacesList);
    GeomAlgoAPI_ShapeTools::combineShapes(aFacesCompound, GeomAPI_Shape::SHELL, theShapesList);
  } else {
    theShapesList.insert(theShapesList.end(), aBaseFacesList.begin(), aBaseFacesList.end());
  }
  return true;
}

//==================================================================================================
bool FeaturesPlugin_Tools::shapesFromSelectionList(
    const std::shared_ptr<ModelAPI_AttributeSelectionList> theSelectionList,
    const bool theStoreFullHierarchy,
    GeomAPI_ShapeHierarchy& theHierarchy,
    std::list<ResultPtr>& theParts)
{
  int aSize = theSelectionList->size();
  for (int anObjectsIndex = 0; anObjectsIndex < aSize; anObjectsIndex++) {
    AttributeSelectionPtr anObjectAttr = theSelectionList->value(anObjectsIndex);
    std::shared_ptr<GeomAPI_Shape> anObject = anObjectAttr->value();
    if (!anObject.get()) { // may be for not-activated parts
      return false;
    }
    ResultPtr aContext = anObjectAttr->context();
    if (aContext && aContext->groupName() == ModelAPI_ResultPart::group())
      theParts.push_back(aContext);
    else {
      // store full shape hierarchy for the corresponding version only
      theHierarchy.addObject(anObject);
      if (theStoreFullHierarchy)
        ModelAPI_Tools::fillShapeHierarchy(anObject, aContext, theHierarchy);
    }
  }
  return true;
}
