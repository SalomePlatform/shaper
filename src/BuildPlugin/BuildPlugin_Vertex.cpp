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

#include "BuildPlugin_Vertex.h"

#include <ModelAPI_AttributeSelectionList.h>
#include <ModelAPI_AttributeBoolean.h>
#include <ModelAPI_CompositeFeature.h>
#include <ModelAPI_ResultBody.h>
#include <ModelAPI_Validator.h>
#include <ModelAPI_Session.h>

#include <GeomAlgoAPI_Copy.h>
#include <GeomAlgoAPI_Tools.h>
#include <GeomAlgoAPI_Partition.h>
#include <GeomAlgoAPI_ShapeTools.h>

#include <GeomAPI_ShapeExplorer.h>

//=================================================================================================
BuildPlugin_Vertex::BuildPlugin_Vertex()
{
}

//=================================================================================================
void BuildPlugin_Vertex::initAttributes()
{
  data()->addAttribute(BASE_OBJECTS_ID(), ModelAPI_AttributeSelectionList::typeId());

  data()->addAttribute(INTERSECT_ID(), ModelAPI_AttributeBoolean::typeId());
  ModelAPI_Session::get()->validators()->registerNotObligatory(getKind(), INTERSECT_ID());
}

//=================================================================================================
bool BuildPlugin_Vertex::buildVertices(GeomShapePtr theShape,
                                       bool isIntersect,
                                       int& theResultIndex)
{
  if (!theShape.get()) {
    setError("Error: Empty shape selected.");
    return false;
  }

  if (theShape->shapeType() == GeomAPI_Shape::VERTEX) {
    // Copy shape.
    std::shared_ptr<GeomAlgoAPI_Copy> aCopyAlgo(new GeomAlgoAPI_Copy(theShape));

    std::string anError;
    if (GeomAlgoAPI_Tools::AlgoError::isAlgorithmFailed(aCopyAlgo, getKind(), anError)) {
      setError(anError);
      return false;
    }

    // Store result.
    ResultBodyPtr aResultBody = document()->createBody(data(), theResultIndex);
    aResultBody->storeModified(theShape, aCopyAlgo->shape());
    setResult(aResultBody, theResultIndex);
    ++theResultIndex;
  } else {
    // Sketch
    GeomAPI_DataMapOfShapeShape alreadyProcessed;

    // 1. Explode on Vertices
    for (GeomAPI_ShapeExplorer anExp (theShape, GeomAPI_Shape::VERTEX); anExp.more(); anExp.next()) {
      GeomShapePtr aSubShape = anExp.current();

      if (alreadyProcessed.bind(aSubShape, aSubShape)) {
        // Store result.
        ResultBodyPtr aResultBody = document()->createBody(data(), theResultIndex);
        aResultBody->storeModified(theShape, aSubShape);
        setResult(aResultBody, theResultIndex);
        ++theResultIndex;
      }
    }

    // 2. If need intersection points, perform Partition
    if (isIntersect) {
      // Partition
      ListOfShape anObjList, aTools;
      anObjList.push_back(theShape);
      std::shared_ptr<GeomAlgoAPI_Partition> aPartitionAlgo (new GeomAlgoAPI_Partition(anObjList, aTools));

      std::string anError;
      if (GeomAlgoAPI_Tools::AlgoError::isAlgorithmFailed(aPartitionAlgo, getKind(), anError)) {
        setError(anError);
        return false;
      }
      GeomShapePtr aSplittedSketch = aPartitionAlgo->shape();

      // Explode on Vertices, skip vertices of initial sketch
      for (GeomAPI_ShapeExplorer anExp (aSplittedSketch, GeomAPI_Shape::VERTEX); anExp.more(); anExp.next()) {
        GeomShapePtr aSubShape = anExp.current();

        //if (!theShape->isSubShape(aSubShape)) { // skip vertices of initial sketch
        if (alreadyProcessed.bind(aSubShape, aSubShape)) {
          // Store result.
          ResultBodyPtr aResultBody = document()->createBody(data(), theResultIndex);
          aResultBody->storeGenerated(anObjList, aSubShape, aPartitionAlgo);
          setResult(aResultBody, theResultIndex);
          ++theResultIndex;
        }
      }
    }
  }

  return true;
}

//=================================================================================================
bool BuildPlugin_Vertex::buildVertices(FeaturePtr theFeature,
                                       bool isIntersect,
                                       int& theResultIndex)
{
  if (theFeature->getKind() != "Sketch") return false;

  // Sub-features
  CompositeFeaturePtr aComposite =
    std::dynamic_pointer_cast<ModelAPI_CompositeFeature>(theFeature);
  if (!aComposite) return false;
  int nbSubs = aComposite->numberOfSubs();
  if (nbSubs < 1) return false;

  // The whole sketch shape
  ResultPtr aContext = theFeature->firstResult();
  GeomShapePtr theShape = aContext->shape();

  GeomAPI_DataMapOfShapeShape alreadyProcessed;

  // 1. Explode on Vertices
  for (GeomAPI_ShapeExplorer anExp (theShape, GeomAPI_Shape::VERTEX); anExp.more(); anExp.next()) {
    GeomShapePtr aSubShape = anExp.current();

    if (alreadyProcessed.bind(aSubShape, aSubShape)) {
      // Store result.
      ResultBodyPtr aResultBody = document()->createBody(data(), theResultIndex);
      aResultBody->storeModified(theShape, aSubShape);
      setResult(aResultBody, theResultIndex);
      ++theResultIndex;
    }
  }

  // 2. If need intersection points, perform Partition
  if (isIntersect) {
    // Partition
    ListOfShape anObjList, aTools;
    anObjList.push_back(theShape);
    std::shared_ptr<GeomAlgoAPI_Partition> aPartitionAlgo (new GeomAlgoAPI_Partition(anObjList, aTools));

    std::string anError;
    if (GeomAlgoAPI_Tools::AlgoError::isAlgorithmFailed(aPartitionAlgo, getKind(), anError)) {
      setError(anError);
      return false;
    }
    GeomShapePtr aSplittedSketch = aPartitionAlgo->shape();

    // Explode on Vertices, skip vertices of initial sketch
    for (GeomAPI_ShapeExplorer anExp (aSplittedSketch, GeomAPI_Shape::VERTEX); anExp.more(); anExp.next()) {
      GeomShapePtr aSubShape = anExp.current();

      //if (!theShape->isSubShape(aSubShape)) { // skip vertices of initial sketch
      if (alreadyProcessed.bind(aSubShape, aSubShape)) {
        // Store result.
        ResultBodyPtr aResultBody = document()->createBody(data(), theResultIndex);
        aResultBody->storeGenerated(anObjList, aSubShape, aPartitionAlgo);
        setResult(aResultBody, theResultIndex);
        ++theResultIndex;
      }
    }
  }

  // 3. Add construction points (centers of circles, etc.)
  for (int i = 0; i < nbSubs; i++) {
    FeaturePtr aSubFeature = aComposite->subFeature(i);
    const std::list<ResultPtr>& aSubResults = aSubFeature->results();
    std::list<ResultPtr>::const_iterator anItRes = aSubResults.cbegin();
    // Iterate on all sub-results
    for (; anItRes != aSubResults.cend(); anItRes++) {
      ResultPtr aRes = *anItRes;
      if (aRes.get()) {
        // Sub-result i
        GeomShapePtr aSubResShape = aRes->shape();

        for (GeomAPI_ShapeExplorer anExp (aSubResShape, GeomAPI_Shape::VERTEX); anExp.more(); anExp.next()) {
          GeomShapePtr aSubShape = anExp.current();

          if (alreadyProcessed.bind(aSubShape, aSubShape)) {
            // Store result.
            ResultBodyPtr aResultBody = document()->createBody(data(), theResultIndex);
            aResultBody->storeModified(theShape, aSubShape);
            setResult(aResultBody, theResultIndex);
            ++theResultIndex;
          }
        }
      }
    }
  }

  return true;
}

void BuildPlugin_Vertex::execute()
{
  // Get base objects list.
  AttributeSelectionListPtr aSelectionList = selectionList(BASE_OBJECTS_ID());
  if (!aSelectionList.get()) {
    setError("Error: Could not get selection list.");
    return;
  }
  if (aSelectionList->size() == 0) {
    setError("Error: Empty selection list.");
    return;
  }

  // Get "Compute intersections" flag value
  bool isIntersect = false;
  if (boolean(INTERSECT_ID()).get() && boolean(INTERSECT_ID())->isInitialized()) {
    isIntersect = boolean(INTERSECT_ID())->value();
  }

  // Iterate arguments and build results
  int aResultIndex = 0;
  for (int anIndex = 0; anIndex < aSelectionList->size(); ++anIndex) {
    AttributeSelectionPtr aSelection = aSelectionList->value(anIndex);
    GeomShapePtr aShape = aSelection->value();
    if (aShape.get()) {
      // A shape selected
      if (!buildVertices(aShape, isIntersect, aResultIndex)) return;
    } else {
      ResultPtr aContext = aSelection->context();
      if (aContext.get()) { // Result selected
        FeaturePtr aFeature = ModelAPI_Feature::feature(aContext);
        if (aFeature.get()) {
          if (aFeature->getKind() == "Sketch") {
            // Special processing for sketch to build center vertices etc.
            if (!buildVertices(aFeature, isIntersect, aResultIndex)) return;
          } else {
            aShape = aContext->shape();
            if (!buildVertices(aShape, isIntersect, aResultIndex)) return;
          }
        }
      } else {
        FeaturePtr aFeature = aSelection->contextFeature();
        if (aFeature.get()) { // Feature selected
          if (aFeature->getKind() == "Sketch") {
            // Special processing for sketch to build center vertices etc.
            if (!buildVertices(aFeature, isIntersect, aResultIndex)) return;
          } else {
            const std::list<ResultPtr>& anArgResults = aFeature->results();
            std::list<ResultPtr>::const_iterator anItRes = anArgResults.cbegin();
            // Iterate on all its results
            for (; anItRes != anArgResults.cend(); anItRes++) {
              ResultPtr aRes = *anItRes;
              if (aRes.get()) {
                // Result i
                aShape = aRes->shape();
                if (!buildVertices(aShape, isIntersect, aResultIndex)) return;
              }
            }
          }
        }
      }
    }
  }

  // Remove extra results from previous execution
  removeResults(aResultIndex);
}
