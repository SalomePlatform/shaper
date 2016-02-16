// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:        FeaturesPlugin_Intersection.cpp
// Created:     15 Feb 2016
// Author:      Dmitry Bobylev

#include "FeaturesPlugin_Intersection.h"

#include <ModelAPI_Data.h>
#include <ModelAPI_Document.h>
#include <ModelAPI_BodyBuilder.h>
#include <ModelAPI_ResultBody.h>
#include <ModelAPI_AttributeSelectionList.h>

#include <GeomAlgoAPI_Intersection.h>

#include <sstream>

//=================================================================================================
FeaturesPlugin_Intersection::FeaturesPlugin_Intersection()
{
}

//=================================================================================================
void FeaturesPlugin_Intersection::initAttributes()
{
  data()->addAttribute(FeaturesPlugin_Intersection::OBJECT_LIST_ID(), ModelAPI_AttributeSelectionList::typeId());
  data()->addAttribute(FeaturesPlugin_Intersection::TOOL_LIST_ID(), ModelAPI_AttributeSelectionList::typeId());
}

//=================================================================================================
void FeaturesPlugin_Intersection::execute()
{
  ListOfShape anObjects, aTools;

  // Getting objects.
  AttributeSelectionListPtr anObjectsSelList = selectionList(FeaturesPlugin_Intersection::OBJECT_LIST_ID());
  for (int anObjectsIndex = 0; anObjectsIndex < anObjectsSelList->size(); anObjectsIndex++) {
    std::shared_ptr<ModelAPI_AttributeSelection> anObjectAttr = anObjectsSelList->value(anObjectsIndex);
    std::shared_ptr<GeomAPI_Shape> anObject = anObjectAttr->value();
    if (!anObject.get()) {
      return;
    }
    anObjects.push_back(anObject);
  }

  // Getting tools.
  AttributeSelectionListPtr aToolsSelList = selectionList(FeaturesPlugin_Intersection::TOOL_LIST_ID());
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
  for (ListOfShape::iterator anObjectsIt = anObjects.begin(); anObjectsIt != anObjects.end(); anObjectsIt++) {
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
      std::string aFeatureError = "Error: resulting shape is not valid";
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
  //load result
  std::shared_ptr<GeomAPI_Shape> aResultShape = theMakeShape.shape();
  std::shared_ptr<GeomAPI_DataMapOfShapeShape> aMapOfShapes = theMakeShape.mapOfSubShapes();
  const int aDeletedTag = 1;
  const int aSubsolidsTag = 2; /// sub solids will be placed at labels 3, 4, etc. if result is compound of solids
  const int aModifyTag = 100000;
  int aModifyToolsTag = 200000;
  std::ostringstream aStream;

  theResultBody->storeModified(theBaseShape, aResultShape, aSubsolidsTag);

  std::string aModName = "Modified";
  theResultBody->loadAndOrientModifiedShapes(&theMakeShape, theBaseShape, GeomAPI_Shape::VERTEX,
                                             aModifyTag, aModName, *aMapOfShapes.get(), true);
  theResultBody->loadAndOrientModifiedShapes(&theMakeShape, theBaseShape, GeomAPI_Shape::EDGE,
                                             aModifyTag, aModName, *aMapOfShapes.get(), true);
  theResultBody->loadDeletedShapes(&theMakeShape, theBaseShape, GeomAPI_Shape::FACE, aDeletedTag);

  int anIndex = 1;
  for(ListOfShape::const_iterator anIter = theTools.begin(); anIter != theTools.end(); anIter++) {
    aStream.str(std::string());
    aStream.clear();
    aStream << aModName << "_" << anIndex++;
    theResultBody->loadAndOrientModifiedShapes(&theMakeShape, *anIter, GeomAPI_Shape::VERTEX,
                                               aModifyToolsTag, aStream.str(), *aMapOfShapes.get(), true);
    theResultBody->loadAndOrientModifiedShapes(&theMakeShape, *anIter, GeomAPI_Shape::EDGE,
                                               aModifyToolsTag, aStream.str(), *aMapOfShapes.get(), true);
    theResultBody->loadDeletedShapes(&theMakeShape, *anIter, GeomAPI_Shape::FACE, aDeletedTag);
    aModifyToolsTag += 10000;
  }
}
