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

#include "BuildPlugin_Edge.h"

#include <ModelAPI_AttributeBoolean.h>
#include <ModelAPI_AttributeSelectionList.h>
#include <ModelAPI_AttributeString.h>
#include <ModelAPI_ResultBody.h>
#include <ModelAPI_Session.h>
#include <ModelAPI_Validator.h>

#include <GeomAlgoAPI_CompoundBuilder.h>
#include <GeomAlgoAPI_Copy.h>
#include <GeomAlgoAPI_EdgeBuilder.h>
#include <GeomAlgoAPI_MakeShapeList.h>
#include <GeomAlgoAPI_Partition.h>
#include <GeomAlgoAPI_Tools.h>

#include <GeomAPI_Edge.h>
#include <GeomAPI_ShapeExplorer.h>
#include <GeomAPI_Vertex.h>

static bool getShape(const AttributeSelectionPtr theAttribute,
                     GeomShapePtr& theShape,
                     std::string& theError)
{
  theShape = theAttribute->value();
  if (!theShape.get()) {
    ResultPtr aContext = theAttribute->context();
    if (!aContext.get()) {
      theError = "Error: Attribute has empty context.";
      return false;
    }
    theShape = aContext->shape();
  }
  return true;
}

//=================================================================================================
BuildPlugin_Edge::BuildPlugin_Edge()
{
}

//=================================================================================================
void BuildPlugin_Edge::initAttributes()
{
  data()->addAttribute(BASE_OBJECTS_ID(), ModelAPI_AttributeSelectionList::typeId());

  data()->addAttribute(CREATION_METHOD(), ModelAPI_AttributeString::typeId());
  ModelAPI_Session::get()->validators()->registerNotObligatory(getKind(), CREATION_METHOD());

  data()->addAttribute(FIRST_POINT(), ModelAPI_AttributeSelection::typeId());
  data()->addAttribute(SECOND_POINT(), ModelAPI_AttributeSelection::typeId());

  data()->addAttribute(INTERSECT_ID(), ModelAPI_AttributeBoolean::typeId());
  ModelAPI_Session::get()->validators()->registerNotObligatory(getKind(), INTERSECT_ID());
}

//=================================================================================================
void BuildPlugin_Edge::execute()
{
  AttributeStringPtr aCreationMethod = string(CREATION_METHOD());
  if (aCreationMethod && aCreationMethod->isInitialized() &&
      aCreationMethod->value() == CREATION_BY_POINTS())
    edgeByPoints();
  else
    edgesBySegments();
}

//=================================================================================================
void BuildPlugin_Edge::edgesBySegments()
{
  string(CREATION_METHOD())->setValue(CREATION_BY_SEGMENTS());

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
  std::string aError;
  for(int anIndex = 0; anIndex < aSelectionList->size(); ++anIndex) {
    AttributeSelectionPtr aSelection = aSelectionList->value(anIndex);
    GeomShapePtr aShape;
    if (!getShape(aSelection, aShape, aError)) {
      setError(aError);
      return;
    }
    if(!aShape.get()) {
      setError("Error: Empty shape selected.");
      return;
    }
    aListOfShapes.push_back(aShape);
  }

  AttributeBooleanPtr isIntersect = boolean(INTERSECT_ID());

  GeomShapePtr aResult;
  std::shared_ptr<GeomAlgoAPI_Partition> aPartitionAlgo;
  if (isIntersect->isInitialized() && isIntersect->value()) {
    aPartitionAlgo.reset(new GeomAlgoAPI_Partition(aListOfShapes, ListOfShape()));

    std::string anError;
    if (GeomAlgoAPI_Tools::AlgoError::isAlgorithmFailed(aPartitionAlgo, getKind(), anError)) {
      setError(anError);
      return;
    }

    aResult = aPartitionAlgo->shape();
  }
  else
    aResult = GeomAlgoAPI_CompoundBuilder::compound(aListOfShapes);

  int aResultIndex = 0;

  // Explode on edges
  std::set<GeomShapePtr, GeomAPI_Shape::Comparator> aProcessed;
  for (GeomAPI_ShapeExplorer anExp(aResult, GeomAPI_Shape::EDGE); anExp.more(); anExp.next()) {
    GeomShapePtr anEdge = anExp.current();
    if (aProcessed.find(anEdge) != aProcessed.end())
      continue; // vertex is already processed
    aProcessed.insert(anEdge);

    std::shared_ptr<GeomAlgoAPI_Copy> aCopyAlgo(new GeomAlgoAPI_Copy(anEdge));

    std::shared_ptr<GeomAlgoAPI_MakeShapeList> aMakeShapeList(new GeomAlgoAPI_MakeShapeList);
    if (aPartitionAlgo)
      aMakeShapeList->appendAlgo(aPartitionAlgo);
    aMakeShapeList->appendAlgo(aCopyAlgo);

    // Store result.
    ResultBodyPtr aResultBody = document()->createBody(data(), aResultIndex);
    aResultBody->storeModified(aListOfShapes, aCopyAlgo->shape(), aMakeShapeList);
    aResultBody->loadModifiedShapes(aMakeShapeList, anEdge, GeomAPI_Shape::VERTEX);
    setResult(aResultBody, aResultIndex);
    ++aResultIndex;
  }

  removeResults(aResultIndex);
}

void BuildPlugin_Edge::edgeByPoints()
{
  // Get base points.
  AttributeSelectionPtr aFirstPointAttr = selection(FIRST_POINT());
  AttributeSelectionPtr aSecondPointAttr = selection(SECOND_POINT());
  if (!aFirstPointAttr.get() || !aSecondPointAttr.get()) {
    setError("Error: Not enough points selected.");
    return;
  }

  std::string aError;
  GeomShapePtr aFirstShape, aSecondShape;
  if (!getShape(aFirstPointAttr, aFirstShape, aError) ||
      !getShape(aSecondPointAttr, aSecondShape, aError)) {
    setError(aError);
    return;
  }
  if (!aFirstShape.get() || !aSecondShape.get()) {
    setError("Error: Empty shape selected.");
    return;
  }

  if (!aFirstShape->isVertex() || !aSecondShape->isVertex()) {
    setError("Error: Selected shape has wrong type. Only vertices acceptable.");
    return;
  }

  int aResultIndex = 0;

  GeomEdgePtr anEdge = GeomAlgoAPI_EdgeBuilder::line(aFirstShape->vertex()->point(),
                                                     aSecondShape->vertex()->point());
  if (!anEdge.get()) {
    setError("Error: Algorithm failed.");
    return;
  }

  // Store result.
  ResultBodyPtr aResultBody = document()->createBody(data(), aResultIndex);
  aResultBody->store(anEdge);
  // History of vertices
  GeomAPI_ShapeExplorer anExp(anEdge, GeomAPI_Shape::VERTEX);
  GeomShapePtr aStartVertex = anExp.current();
  anExp.next();
  GeomShapePtr anEndVertex = anExp.current();
  aResultBody->modified(aFirstShape, aStartVertex);
  aResultBody->modified(aSecondShape, anEndVertex);

  setResult(aResultBody, aResultIndex++);
  removeResults(aResultIndex);
}
