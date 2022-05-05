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

#include "BuildPlugin_Vertex.h"

#include <ModelAPI_AttributeSelectionList.h>
#include <ModelAPI_AttributeBoolean.h>
#include <ModelAPI_CompositeFeature.h>
#include <ModelAPI_ResultBody.h>
#include <ModelAPI_Validator.h>
#include <ModelAPI_Session.h>

#include <GeomAlgoAPI_CompoundBuilder.h>
#include <GeomAlgoAPI_Copy.h>
#include <GeomAlgoAPI_MakeShapeList.h>
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

void BuildPlugin_Vertex::buildVertices(const ListOfShape& theShapes, bool isIntersect)
{
  GeomShapePtr aResult;
  std::shared_ptr<GeomAlgoAPI_Partition> aPartitionAlgo;
  if (isIntersect) {
    aPartitionAlgo.reset(new GeomAlgoAPI_Partition(theShapes, ListOfShape()));

    std::string anError;
    if (GeomAlgoAPI_Tools::AlgoError::isAlgorithmFailed(aPartitionAlgo, getKind(), anError)) {
      setError(anError);
      return;
    }

    aResult = aPartitionAlgo->shape();
  }
  else
    aResult = GeomAlgoAPI_CompoundBuilder::compound(theShapes);

  int aResultIndex = 0;

  // Explode on vertices
  std::set<GeomVertexPtr, GeomAPI_Vertex::GeometricComparator> aProcessed;
  for (GeomAPI_ShapeExplorer anExp(aResult, GeomAPI_Shape::VERTEX); anExp.more(); anExp.next()) {
    GeomVertexPtr aVertex(new GeomAPI_Vertex(anExp.current()));
    if (aProcessed.find(aVertex) != aProcessed.end())
      continue; // vertex is already processed
    aProcessed.insert(aVertex);

    std::shared_ptr<GeomAlgoAPI_Copy> aCopy(new GeomAlgoAPI_Copy(aVertex));
    aVertex.reset(new GeomAPI_Vertex(aCopy->shape()));

    std::shared_ptr<GeomAlgoAPI_MakeShapeList> aMakeShapeList(new GeomAlgoAPI_MakeShapeList);
    if (aPartitionAlgo)
      aMakeShapeList->appendAlgo(aPartitionAlgo);
    aMakeShapeList->appendAlgo(aCopy);

    // Store result.
    ResultBodyPtr aResultBody = document()->createBody(data(), aResultIndex);
    aResultBody->storeModified(theShapes, aVertex, aMakeShapeList);
    setResult(aResultBody, aResultIndex);
    ++aResultIndex;
  }

  removeResults(aResultIndex);
}

static void collectEdgesAndVertices(AttributeSelectionPtr theSelection, ListOfShape& thePrimitives)
{
  FeaturePtr aFeature = theSelection->contextFeature();
  ResultPtr aContext = theSelection->context();
  GeomShapePtr aShape = theSelection->value();
  if (aShape)
    thePrimitives.push_back(aShape);
  else {
    if (aContext && !aFeature)
      aFeature = ModelAPI_Feature::feature(aContext);
    if (!aFeature)
      return;

    // process results of the feature
    const std::list<ResultPtr>& aResults = aFeature->results();
    std::list<ResultPtr>::const_iterator anIt = aResults.begin();
    for (; anIt != aResults.end(); ++anIt)
      thePrimitives.push_back((*anIt)->shape());

    CompositeFeaturePtr aComposite =
        std::dynamic_pointer_cast<ModelAPI_CompositeFeature>(aFeature);
    if (!aComposite)
      return;

    // add construction points (centers of circles, etc.)
    for (int i = 0, nbSubs = aComposite->numberOfSubs(); i < nbSubs; ++i) {
      FeaturePtr aSubFeature = aComposite->subFeature(i);
      const std::list<ResultPtr>& aSubResults = aSubFeature->results();
      // find all points
      for (anIt = aSubResults.begin(); anIt != aSubResults.cend(); ++anIt) {
        GeomShapePtr aSubResShape = (*anIt)->shape();
        if (aSubResShape->isVertex())
          thePrimitives.push_back(aSubResShape);
      }
    }
  }
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

  // Iterate arguments and collect shapes
  ListOfShape aShapes;
  for (int anIndex = 0; anIndex < aSelectionList->size(); ++anIndex) {
    AttributeSelectionPtr aSelection = aSelectionList->value(anIndex);
    collectEdgesAndVertices(aSelection, aShapes);
  }

  buildVertices(aShapes, isIntersect);
}
