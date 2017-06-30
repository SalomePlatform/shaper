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

#include "FeaturesPlugin_Intersection.h"

#include <ModelAPI_Data.h>
#include <ModelAPI_Document.h>
#include <ModelAPI_BodyBuilder.h>
#include <ModelAPI_ResultBody.h>
#include <ModelAPI_AttributeSelectionList.h>

#include <GeomAlgoAPI_Intersection.h>
#include <GeomAPI_ShapeExplorer.h>

#include <sstream>

//=================================================================================================
FeaturesPlugin_Intersection::FeaturesPlugin_Intersection()
{
}

//=================================================================================================
void FeaturesPlugin_Intersection::initAttributes()
{
  data()->addAttribute(FeaturesPlugin_Intersection::OBJECT_LIST_ID(),
                       ModelAPI_AttributeSelectionList::typeId());
  data()->addAttribute(FeaturesPlugin_Intersection::TOOL_LIST_ID(),
                       ModelAPI_AttributeSelectionList::typeId());
}

//=================================================================================================
void FeaturesPlugin_Intersection::execute()
{
  ListOfShape anObjects, aTools;

  // Getting objects.
  AttributeSelectionListPtr anObjectsSelList =
    selectionList(FeaturesPlugin_Intersection::OBJECT_LIST_ID());
  for (int anObjectsIndex = 0; anObjectsIndex < anObjectsSelList->size(); anObjectsIndex++) {
    std::shared_ptr<ModelAPI_AttributeSelection> anObjectAttr =
      anObjectsSelList->value(anObjectsIndex);
    std::shared_ptr<GeomAPI_Shape> anObject = anObjectAttr->value();
    if (!anObject.get()) {
      return;
    }
    anObjects.push_back(anObject);
  }

  // Getting tools.
  AttributeSelectionListPtr aToolsSelList =
    selectionList(FeaturesPlugin_Intersection::TOOL_LIST_ID());
  for (int aToolsIndex = 0; aToolsIndex < aToolsSelList->size(); aToolsIndex++) {
    std::shared_ptr<ModelAPI_AttributeSelection> aToolAttr = aToolsSelList->value(aToolsIndex);
    std::shared_ptr<GeomAPI_Shape> aTool = aToolAttr->value();
    if (!aTool.get()) {
      return;
    }
    aTools.push_back(aTool);
  }

  if(anObjects.empty() || aTools.empty()) {
    setError("Error: Objects or tools are empty.");
    return;
  }

  int aResultIndex = 0;

  // Create result for each object.
  for (ListOfShape::iterator
       anObjectsIt = anObjects.begin(); anObjectsIt != anObjects.end(); anObjectsIt++) {
    std::shared_ptr<GeomAPI_Shape> anObject = *anObjectsIt;
    ListOfShape aListWithObject; aListWithObject.push_back(anObject);
    GeomAlgoAPI_Intersection anIntersectionAlgo(aListWithObject, aTools);

    // Checking that the algorithm worked properly.
    if (!anIntersectionAlgo.isDone()) {
      static const std::string aFeatureError = "Error: Intersection algorithm failed.";
      setError(aFeatureError);
      return;
    }
    if (anIntersectionAlgo.shape()->isNull()) {
      static const std::string aShapeError = "Error: Resulting shape is Null.";
      setError(aShapeError);
      return;
    }
    if (!anIntersectionAlgo.isValid()) {
      std::string aFeatureError = "Error: Resulting shape is not valid.";
      setError(aFeatureError);
      return;
    }

    std::shared_ptr<ModelAPI_ResultBody> aResultBody = document()->createBody(data(), aResultIndex);
    loadNamingDS(aResultBody, anObject, aTools, anIntersectionAlgo);
    setResult(aResultBody, aResultIndex);
    aResultIndex++;
  }

  // remove the rest results if there were produced in the previous pass
  removeResults(aResultIndex);
}

//=================================================================================================
void FeaturesPlugin_Intersection::loadNamingDS(std::shared_ptr<ModelAPI_ResultBody> theResultBody,
                                               const std::shared_ptr<GeomAPI_Shape> theBaseShape,
                                               const ListOfShape& theTools,
                                               GeomAlgoAPI_MakeShape& theMakeShape)
{
  std::shared_ptr<GeomAPI_Shape> aResultShape = theMakeShape.shape();
  theResultBody->storeModified(theBaseShape, aResultShape);

  const int aDeletedVertexTag = 1;
  const int aDeletedEdgeTag   = 2;
  const int aDeletedFaceTag   = 3;

  theResultBody->loadDeletedShapes(&theMakeShape,
                                   theBaseShape,
                                   GeomAPI_Shape::VERTEX,
                                   aDeletedVertexTag);
  theResultBody->loadDeletedShapes(&theMakeShape,
                                   theBaseShape,
                                   GeomAPI_Shape::EDGE,
                                   aDeletedEdgeTag);
  theResultBody->loadDeletedShapes(&theMakeShape,
                                   theBaseShape,
                                   GeomAPI_Shape::FACE,
                                   aDeletedFaceTag);

  ListOfShape aShapes = theTools;
  aShapes.push_back(theBaseShape);
  GeomAPI_DataMapOfShapeShape aShapesMap; // Map to store {result_shape, original_shape}
  const int aShapeTypesNb = 2;
  const GeomAPI_Shape::ShapeType aShapeTypes[aShapeTypesNb] =
    {GeomAPI_Shape::VERTEX, GeomAPI_Shape::EDGE};
  for(ListOfShape::const_iterator anIt = aShapes.cbegin(); anIt != aShapes.cend(); ++anIt) {
    const GeomShapePtr aShape = *anIt;
    for(int anIndex = 0; anIndex < aShapeTypesNb; ++anIndex) {
      for(GeomAPI_ShapeExplorer anOrigShapeExp(aShape, aShapeTypes[anIndex]);
          anOrigShapeExp.more();
          anOrigShapeExp.next()) {
        ListOfShape aHistory;
        const GeomShapePtr aSubShape = anOrigShapeExp.current();
        theMakeShape.modified(aSubShape, aHistory);
        for(ListOfShape::const_iterator aHistoryIt = aHistory.cbegin();
            aHistoryIt != aHistory.cend();
            ++aHistoryIt) {
          aShapesMap.bind(*aHistoryIt, aSubShape);
        }
      }
    }
  }

  int aModifiedVertexIndex(1),
      aGeneratedVertexIndex(1),
      aModifiedEdgeIndex(1),
      aGeneratedEdgeIndex(1);
  int aTag = 4;
  GeomAPI_DataMapOfShapeShape aStoredShapes;
  for(int anIndex = 0; anIndex < aShapeTypesNb; ++anIndex) {
    for(GeomAPI_ShapeExplorer aShapeExp(aResultShape, aShapeTypes[anIndex]);
        aShapeExp.more();
        aShapeExp.next()) {
      const GeomShapePtr aSubShape = aShapeExp.current();
      if(aStoredShapes.isBound(aSubShape)) {
        continue;
      }
      if(aShapesMap.isBound(aSubShape)) {
        theResultBody->modified(aShapesMap.find(aSubShape),
          aSubShape,
          std::string("Modified_")
            + (anIndex == 0 ? "Vertex_" : "Edge_")
            + std::to_string((long long)(anIndex == 0 ? aModifiedVertexIndex++
                                                      : aModifiedEdgeIndex++)),
          aTag++);
      } else {
        theResultBody->generated(aSubShape,
          std::string("Generated_")
            + (anIndex == 0 ? "Vertex_" : "Edge_")
            + std::to_string((long long)(anIndex == 0 ? aGeneratedVertexIndex++
                                                      : aGeneratedEdgeIndex++)),
          aTag++);
      }
      aStoredShapes.bind(aSubShape, aSubShape);
    }
  }
}
