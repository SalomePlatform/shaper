// Copyright (C) 2014-2020  CEA/DEN, EDF R&D
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

#include "BuildPlugin_Wire.h"

#include <ModelAPI_AttributeBoolean.h>
#include <ModelAPI_AttributeSelectionList.h>
#include <ModelAPI_ResultBody.h>
#include <ModelAPI_ResultConstruction.h>
#include <ModelAPI_Session.h>
#include <ModelAPI_Validator.h>

#include <Events_InfoMessage.h>

#include <GeomAPI_PlanarEdges.h>
#include <GeomAPI_ShapeExplorer.h>
#include <GeomAPI_Vertex.h>

#include <GeomAlgoAPI_MakeShapeList.h>
#include <GeomAlgoAPI_PaveFiller.h>
#include <GeomAlgoAPI_SketchBuilder.h>
#include <GeomAlgoAPI_Tools.h>
#include <GeomAlgoAPI_WireBuilder.h>

#include <SketchPlugin_Sketch.h>

#include <algorithm>

static bool buildSketchWires(FeaturePtr theSketchFeature, GeomShapePtr theSketchShape,
                             bool isIntersect,
                             ListOfShape& theWires, GeomMakeShapePtr& theAlgo,
                             std::string& theError);

static bool buildWire(const ListOfShape& theEdges, GeomShapePtr& theWire, std::string& theError);

//=================================================================================================
BuildPlugin_Wire::BuildPlugin_Wire()
{
}

//=================================================================================================
void BuildPlugin_Wire::initAttributes()
{
  data()->addAttribute(BASE_OBJECTS_ID(), ModelAPI_AttributeSelectionList::typeId());

  data()->addAttribute(INTERSECT_ID(), ModelAPI_AttributeBoolean::typeId());
  ModelAPI_Session::get()->validators()->registerNotObligatory(getKind(), INTERSECT_ID());
}

//=================================================================================================
void BuildPlugin_Wire::execute()
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

  AttributeBooleanPtr anIntersectAttr = boolean(INTERSECT_ID());
  bool isIntersect = anIntersectAttr->isInitialized() && anIntersectAttr->value();

  // Collect base shapes.
  ListOfShape anEdges;
  std::list< std::pair<FeaturePtr, GeomShapePtr> > aSketches;
  for(int anIndex = 0; anIndex < aSelectionList->size(); ++anIndex) {
    AttributeSelectionPtr aSelection = aSelectionList->value(anIndex);
    GeomShapePtr aShape = aSelection->value();
    if(!aShape.get()) {
      aShape = aSelection->context()->shape();

      std::shared_ptr<GeomAPI_PlanarEdges> aSketchShape =
          std::dynamic_pointer_cast<GeomAPI_PlanarEdges>(aShape);
      if (aSketchShape) {
        FeaturePtr aSketchFeature = ModelAPI_Feature::feature(aSelection->context());
        aSketches.push_back(std::pair<FeaturePtr, GeomShapePtr>(aSketchFeature, aSketchShape));
        continue;
      }
    }
    for(GeomAPI_ShapeExplorer anExp(aShape, GeomAPI_Shape::EDGE); anExp.more(); anExp.next()) {
      GeomShapePtr anEdge = anExp.current();
      anEdges.push_back(anEdge);
    }
  }

  int aResultIndex = 0;
  std::string anError;

  if (!anEdges.empty()) {
    // Create wire from the list of edges.
    GeomShapePtr aWire;
    if (!buildWire(anEdges, aWire, anError)) {
      setError(anError);
      return;
    }

    // Store result.
    ResultBodyPtr aResultBody = document()->createBody(data(), aResultIndex);
    aResultBody->store(aWire);
    for (GeomAPI_ShapeExplorer anExp(aWire, GeomAPI_Shape::EDGE); anExp.more(); anExp.next()) {
      GeomShapePtr anEdgeInResult = anExp.current();
      for (ListOfShape::const_iterator anIt = anEdges.cbegin(); anIt != anEdges.cend(); ++anIt) {
        std::shared_ptr<GeomAPI_Edge> anEdgeInList(new GeomAPI_Edge(*anIt));
        if (anEdgeInList->isEqual(anEdgeInResult)) {
          aResultBody->modified(anEdgeInList, anEdgeInResult);
          break;
        }
      }
    }
    setResult(aResultBody, aResultIndex);
    ++aResultIndex;
  }

  // create wires from sketches
  for (std::list<std::pair<FeaturePtr, GeomShapePtr> >::iterator anIt = aSketches.begin();
       anIt != aSketches.end(); ++anIt) {
    ListOfShape aWires;
    GeomMakeShapePtr aMakeShapeList;
    if (!buildSketchWires(anIt->first, anIt->second, isIntersect,
                          aWires, aMakeShapeList, anError)) {
      setError(anError);
      return;
    }

    for (ListOfShape::iterator aWIt = aWires.begin(); aWIt != aWires.end(); ++aWIt) {
      ResultBodyPtr aResultBody = document()->createBody(data(), aResultIndex);
      ListOfShape aSketch;
      aSketch.push_back(anIt->second);
      aResultBody->storeModified(aSketch, *aWIt, aMakeShapeList);
      aResultBody->loadModifiedShapes(aMakeShapeList, anIt->second, GeomAPI_Shape::EDGE);
      setResult(aResultBody, aResultIndex);
      ++aResultIndex;
    }
  }

  removeResults(aResultIndex);
}

//=================================================================================================
bool BuildPlugin_Wire::customAction(const std::string& theActionId)
{
  if(theActionId == ADD_CONTOUR_ACTION_ID()) {
    return addContour();
  } else {
    std::string aMsg = "Error: Feature \"%1\" does not support action \"%2\".";
    Events_InfoMessage("BuildPlugin_Wire", aMsg).arg(getKind()).arg(theActionId).send();
  }

  return false;
}

//=================================================================================================
bool BuildPlugin_Wire::addContour()
{
  // Get base objects list.
  AttributeSelectionListPtr aSelectionList = selectionList(BASE_OBJECTS_ID());
  if(aSelectionList->size() == 0) {
    Events_InfoMessage("BuildPlugin_Wire", "Error: Empty selection list.").send();
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
    ResultConstructionPtr aConstruction =
      std::dynamic_pointer_cast<ModelAPI_ResultConstruction>(aContext);
    if(!aConstruction.get()) {
      continue;
    }
    GeomShapePtr aContextShape = aConstruction->shape();
    std::shared_ptr<GeomAPI_PlanarEdges> aPlanarEdges =
      std::dynamic_pointer_cast<GeomAPI_PlanarEdges>(aContextShape);
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
  bool isAnyContourAdded = false;
  for(std::list<AttributeSelectionPtr>::const_iterator aListIt = anAttributesToCheck.cbegin();
      aListIt != anAttributesToCheck.cend();
      ++aListIt) {
    AttributeSelectionPtr aSelection = *aListIt;
    std::shared_ptr<GeomAPI_Edge> anEdgeInList(new GeomAPI_Edge(aSelection->value()));

    ResultConstructionPtr aConstruction =
      std::dynamic_pointer_cast<ModelAPI_ResultConstruction>(aSelection->context());

    // Iterate on wires and add wire with this edge.
    std::shared_ptr<GeomAPI_Shape> aFoundWire;
    for(int anIndex = 0; anIndex < aConstruction->facesNum(); ++anIndex) {
      std::shared_ptr<GeomAPI_Face> aFace = aConstruction->face(anIndex);
      for(GeomAPI_ShapeExplorer
          aWireExp(aFace, GeomAPI_Shape::WIRE); aWireExp.more(); aWireExp.next()) {
        GeomShapePtr aWireOnFace = aWireExp.current();
        for(GeomAPI_ShapeExplorer
            anExp(aWireOnFace, GeomAPI_Shape::EDGE); anExp.more(); anExp.next()) {
          std::shared_ptr<GeomAPI_Edge> anEdgeOnFace(new GeomAPI_Edge(anExp.current()));
          if(anEdgeInList->isEqual(anEdgeOnFace)) {
            aFoundWire = aWireOnFace;
            break;
          }
        }
      }
      if(aFoundWire.get()) {
        break;
      }
    }

    // If wire with the same edge found. Add all other edges to list.
    if(aFoundWire.get()) {
      for(GeomAPI_ShapeExplorer
          anExp(aFoundWire, GeomAPI_Shape::EDGE); anExp.more(); anExp.next()) {
        std::shared_ptr<GeomAPI_Edge> anEdgeOnFace(new GeomAPI_Edge(anExp.current()));
        ListOfShape::const_iterator anEdgesIt = anAddedEdges.cbegin();
        for(; anEdgesIt != anAddedEdges.cend(); ++anEdgesIt) {
          if(anEdgeOnFace->isEqual(*anEdgesIt)) {
            break;
          }
        }
        if(anEdgesIt == anAddedEdges.cend()) {
          isAnyContourAdded = true;
          anAddedEdges.push_back(anEdgeOnFace);
          aSelectionList->append(aConstruction, anEdgeOnFace);
        }
      }
    }
  }

  if(!isAnyContourAdded) {
    Events_InfoMessage("BuildPlugin_Wire",
      "Error: Contours already closed or no contours found for selected edges.").send();
    return false;
  }

  return true;
}



// =====================     Auxiliary functions     ==============================================

bool buildWire(const ListOfShape& theEdges, GeomShapePtr& theWire, std::string& theError)
{
  theWire = GeomAlgoAPI_WireBuilder::wire(theEdges);
  if (!theWire.get()) {
    theError = "Error: Result wire is empty. Probably it has disconnected edges or non-manifold.";
    return false;
  }
  return true;
}

bool buildSketchWires(FeaturePtr theSketchFeature, GeomShapePtr theSketchShape, bool isIntersect,
                      ListOfShape& theWires, GeomMakeShapePtr& theAlgo, std::string& theError)
{
  ListOfShape aSketchEdges =
      std::dynamic_pointer_cast<GeomAPI_PlanarEdges>(theSketchShape)->getEdges();

  std::shared_ptr<GeomAlgoAPI_MakeShapeList> anAlgoList(new GeomAlgoAPI_MakeShapeList);
  if (isIntersect) {
    std::set<GeomShapePtr, GeomAPI_Shape::Comparator> aProcessedEdges;
    // perform sketch builder first
    AttributePointPtr anOrigin = std::dynamic_pointer_cast<GeomDataAPI_Point>(
        theSketchFeature->attribute(SketchPlugin_Sketch::ORIGIN_ID()));
    AttributeDirPtr aNormal = std::dynamic_pointer_cast<GeomDataAPI_Dir>(
        theSketchFeature->attribute(SketchPlugin_Sketch::NORM_ID()));
    AttributeDirPtr aDirX = std::dynamic_pointer_cast<GeomDataAPI_Dir>(
        theSketchFeature->attribute(SketchPlugin_Sketch::DIRX_ID()));
    std::shared_ptr<GeomAlgoAPI_SketchBuilder> aSketchBuilder(new GeomAlgoAPI_SketchBuilder(
        anOrigin->pnt(), aDirX->dir(), aNormal->dir(), theSketchShape));

    anAlgoList->appendAlgo(aSketchBuilder);

    // collect wires from faces
    const ListOfShape& aFaces = aSketchBuilder->faces();
    for (ListOfShape::const_iterator anIt = aFaces.begin(); anIt != aFaces.end(); ++anIt) {
      for (GeomAPI_ShapeExplorer aWExp(*anIt, GeomAPI_Shape::WIRE); aWExp.more(); aWExp.next()) {
        GeomAPI_ShapeExplorer aEExp(aWExp.current(), GeomAPI_Shape::EDGE);
        if (aProcessedEdges.find(aEExp.current()) != aProcessedEdges.end())
          continue; // wire is already processed
        // mark edges as processed
        for (; aEExp.more(); aEExp.next())
          aProcessedEdges.insert(aEExp.current());
        // store the wire
        theWires.push_back(aWExp.current());
      }
    }

    // collect unused edges
    ListOfShape aCopy;
    for (ListOfShape::iterator anIt = aSketchEdges.begin(); anIt != aSketchEdges.end(); ++anIt) {
      ListOfShape anImages;
      aSketchBuilder->modified(*anIt, anImages);
      for (ListOfShape::iterator anEdge = anImages.begin(); anEdge != anImages.end(); ++anEdge)
        if (aProcessedEdges.find(*anEdge) == aProcessedEdges.end())
          aCopy.push_back(*anEdge);
    }

    if (aCopy.size() > 1) {
      // split these edges
      std::shared_ptr<GeomAlgoAPI_PaveFiller> aGeneralFuse(new GeomAlgoAPI_PaveFiller(aCopy));
      if (GeomAlgoAPI_Tools::AlgoError::isAlgorithmFailed(
              aGeneralFuse, BuildPlugin_Wire::ID(), theError))
        return false;
      anAlgoList->appendAlgo(aGeneralFuse);

      // collect edges after the split
      aSketchEdges.clear();
      for (GeomAPI_ShapeExplorer anExp(aGeneralFuse->shape(), GeomAPI_Shape::EDGE);
           anExp.more(); anExp.next())
        aSketchEdges.push_back(anExp.current());
    }
    else
      aSketchEdges = aCopy;
  }

  // connect least edges to wires
  typedef std::list<ListOfShape> ListOfWires;
  ListOfWires aNewWires;
  typedef std::map<GeomVertexPtr, ListOfWires::iterator,
                   GeomAPI_Vertex::GeometricComparator> MapVertexWire;
  MapVertexWire aMapVW;
  for (ListOfShape::iterator aEIt = aSketchEdges.begin(); aEIt != aSketchEdges.end(); ++aEIt) {
    GeomEdgePtr anEdge = (*aEIt)->edge();
    GeomVertexPtr aStartV, aEndV;
    anEdge->vertices(aStartV, aEndV);
    MapVertexWire::iterator aFoundStart = aMapVW.find(aStartV);
    MapVertexWire::iterator aFoundEnd = aMapVW.find(aEndV);
    if (aFoundStart == aMapVW.end()) {
      if (aFoundEnd == aMapVW.end()) {
        // new wire
        aNewWires.push_back(ListOfShape());
        ListOfWires::iterator aNewW = --aNewWires.end();
        aNewW->push_back(anEdge);
        aMapVW[aStartV] = aNewW;
        aMapVW[aEndV] = aNewW;
        continue;
      }
    }
    else {
      if (aFoundEnd == aMapVW.end()) {
        // swap found vertices for correct further processing
        aFoundEnd = aFoundStart;
        aStartV = aEndV;
      }
      else {
        // both vertices are found
        aFoundStart->second->push_back(anEdge);
        if (aFoundStart->second != aFoundEnd->second) {
          // different wires => merge segments
          aFoundStart->second->insert(aFoundStart->second->end(),
              aFoundEnd->second->begin(), aFoundEnd->second->end());
          for (MapVertexWire::iterator it = aMapVW.begin(); it != aMapVW.end(); ++it)
            if (it != aFoundEnd && it->second == aFoundEnd->second) {
              // another boundary of the wire, change link to the whole result
              it->second = aFoundStart->second;
              break;
            }
          aNewWires.erase(aFoundEnd->second);
        }
        aMapVW.erase(aFoundStart);
        aMapVW.erase(aFoundEnd);
        continue;
      }
    }
    // add edge to existing wire, substitute the connection point
    // by the other boundary point of the edge
    aFoundEnd->second->push_back(anEdge);
    aMapVW[aStartV] = aFoundEnd->second;
    aMapVW.erase(aFoundEnd);
  }

  // generate new wires from the sets of edges
  for (ListOfWires::iterator anIt = aNewWires.begin(); anIt != aNewWires.end(); ++anIt) {
    GeomShapePtr aWire;
    if (!buildWire(*anIt, aWire, theError))
      return false;
    theWires.push_back(aWire);
  }

  theAlgo = anAlgoList;
  return true;
}
