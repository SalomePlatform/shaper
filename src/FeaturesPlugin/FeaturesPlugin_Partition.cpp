// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:        FeaturesPlugin_Partition.cpp
// Created:     31 Jul 2015
// Author:      Natalia ERMOLAEVA

#include "FeaturesPlugin_Partition.h"

#include <ModelAPI_Data.h>
#include <ModelAPI_Document.h>
#include <ModelAPI_AttributeBoolean.h>
#include <ModelAPI_AttributeReference.h>
#include <ModelAPI_AttributeInteger.h>
#include <ModelAPI_BodyBuilder.h>
#include <ModelAPI_ResultBody.h>
#include <ModelAPI_AttributeSelectionList.h>
#include <ModelAPI_Session.h>
#include <ModelAPI_Validator.h>

#include <GeomAlgoAPI_CompoundBuilder.h>
#include <GeomAlgoAPI_Partition.h>
#include <GeomAlgoAPI_MakeShapeCustom.h>
#include <GeomAlgoAPI_MakeShapeList.h>
#include <GeomAlgoAPI_ShapeTools.h>

#include <sstream>

//=================================================================================================
FeaturesPlugin_Partition::FeaturesPlugin_Partition()
{
}

//=================================================================================================
void FeaturesPlugin_Partition::initAttributes()
{
  AttributeSelectionListPtr aSelection =
    std::dynamic_pointer_cast<ModelAPI_AttributeSelectionList>(data()->addAttribute(
    FeaturesPlugin_Partition::OBJECT_LIST_ID(), ModelAPI_AttributeSelectionList::typeId()));
  aSelection->setSelectionType("SOLID");

  aSelection =
    std::dynamic_pointer_cast<ModelAPI_AttributeSelectionList>(data()->addAttribute(
    FeaturesPlugin_Partition::TOOL_LIST_ID(), ModelAPI_AttributeSelectionList::typeId()));
  aSelection->setSelectionType("SOLID");

  ModelAPI_Session::get()->validators()->registerNotObligatory(getKind(), TOOL_LIST_ID());

  data()->addAttribute(COMBINE_ID(), ModelAPI_AttributeBoolean::typeId());
}

//=================================================================================================
std::shared_ptr<GeomAPI_Shape> FeaturesPlugin_Partition::getShape(const std::string& theAttrName)
{
  std::shared_ptr<ModelAPI_AttributeReference> aObjRef =
      std::dynamic_pointer_cast<ModelAPI_AttributeReference>(data()->attribute(theAttrName));
  if (aObjRef) {
    std::shared_ptr<ModelAPI_ResultBody> aConstr =
        std::dynamic_pointer_cast<ModelAPI_ResultBody>(aObjRef->value());
    if (aConstr)
      return aConstr->shape();
  }
  return std::shared_ptr<GeomAPI_Shape>();
}

//=================================================================================================
void FeaturesPlugin_Partition::execute()
{
  ListOfShape anObjects, aTools, aToolsForNaming;

  // Getting objects.
  AttributeSelectionListPtr anObjectsSelList = selectionList(FeaturesPlugin_Partition::OBJECT_LIST_ID());
  for (int anObjectsIndex = 0; anObjectsIndex < anObjectsSelList->size(); anObjectsIndex++) {
    std::shared_ptr<ModelAPI_AttributeSelection> anObjectAttr = anObjectsSelList->value(anObjectsIndex);
    std::shared_ptr<GeomAPI_Shape> anObject = anObjectAttr->value();
    if (!anObject.get()) {
      return;
    }
    anObjects.push_back(anObject);
  }

  GeomAlgoAPI_MakeShapeList aMakeShapeList;
  std::list<std::shared_ptr<GeomAPI_Pnt> > aBoundingPoints = GeomAlgoAPI_ShapeTools::getBoundingBox(anObjects, 1.0);

  // Getting tools.
  AttributeSelectionListPtr aToolsSelList = selectionList(FeaturesPlugin_Partition::TOOL_LIST_ID());
  for (int aToolsIndex = 0; aToolsIndex < aToolsSelList->size(); aToolsIndex++) {
    std::shared_ptr<ModelAPI_AttributeSelection> aToolAttr = aToolsSelList->value(aToolsIndex);
    std::shared_ptr<GeomAPI_Shape> aTool = aToolAttr->value();
    if(!aTool.get()) {
      // it could be a construction plane
      ResultPtr aContext = aToolAttr->context();
      if(aContext.get()) {
        aTool = GeomAlgoAPI_ShapeTools::fitPlaneToBox(aContext->shape(), aBoundingPoints);
        std::shared_ptr<GeomAlgoAPI_MakeShapeCustom> aMkShCustom(new GeomAlgoAPI_MakeShapeCustom);
        aMkShCustom->addModified(aContext->shape(), aTool);
        aMakeShapeList.append(aMkShCustom);
        aTools.push_back(aTool);
        aToolsForNaming.push_back(aContext->shape());
      }
    } else {
      aTools.push_back(aTool);
      aToolsForNaming.push_back(aTool);
    }
  }

  // Getting combine flag.
  bool isCombine = boolean(COMBINE_ID())->value();

  if(anObjects.empty()/* || aTools.empty()*/) {
    std::string aFeatureError = "Not enough objects for partition operation";
    setError(aFeatureError);
    return;
  }

  int aResultIndex = 0;

  if(isCombine) {
    // Create single result.
    std::shared_ptr<GeomAPI_Shape> aCompoud = GeomAlgoAPI_CompoundBuilder::compound(anObjects);
    anObjects.clear();
    anObjects.push_back(aCompoud);
    GeomAlgoAPI_Partition aPartitionAlgo(anObjects, aTools);

    // Checking that the algorithm worked properly.
    if (!aPartitionAlgo.isDone()) {
      static const std::string aFeatureError = "Partition algorithm failed";
      setError(aFeatureError);
      return;
    }
    if (aPartitionAlgo.shape()->isNull()) {
      static const std::string aShapeError = "Resulting shape is Null";
      setError(aShapeError);
      return;
    }
    if (!aPartitionAlgo.isValid()) {
      std::string aFeatureError = "Warning: resulting shape is not valid";
      setError(aFeatureError);
      return;
    }

    if (GeomAlgoAPI_ShapeTools::volume(aPartitionAlgo.shape()) > 1.e-7) {
      std::shared_ptr<ModelAPI_ResultBody> aResultBody = document()->createBody(data(), aResultIndex);
      aMakeShapeList.append(aPartitionAlgo.makeShape());
      GeomAPI_DataMapOfShapeShape aMapOfShapes = *aPartitionAlgo.mapOfShapes().get();
      loadNamingDS(aResultBody, anObjects.front(), aToolsForNaming, aPartitionAlgo.shape(), aMakeShapeList, aMapOfShapes);
      setResult(aResultBody, aResultIndex);
      aResultIndex++;
    }
  } else {
    // Create result for each object.
    for (ListOfShape::iterator anObjectsIt = anObjects.begin(); anObjectsIt != anObjects.end(); anObjectsIt++) {
      std::shared_ptr<GeomAPI_Shape> anObject = *anObjectsIt;
      ListOfShape aListWithObject; aListWithObject.push_back(anObject);
      GeomAlgoAPI_Partition aPartitionAlgo(aListWithObject, aTools);

      // Checking that the algorithm worked properly.
      if (!aPartitionAlgo.isDone()) {
        static const std::string aFeatureError = "Partition algorithm failed";
        setError(aFeatureError);
        return;
      }
      if (aPartitionAlgo.shape()->isNull()) {
        static const std::string aShapeError = "Resulting shape is Null";
        setError(aShapeError);
        return;
      }
      if (!aPartitionAlgo.isValid()) {
        std::string aFeatureError = "Warning: resulting shape is not valid";
        setError(aFeatureError);
        return;
      }

      if (GeomAlgoAPI_ShapeTools::volume(aPartitionAlgo.shape()) > 1.e-7) {
        std::shared_ptr<ModelAPI_ResultBody> aResultBody = document()->createBody(data(), aResultIndex);
        aMakeShapeList.append(aPartitionAlgo.makeShape());
        GeomAPI_DataMapOfShapeShape aMapOfShapes = *aPartitionAlgo.mapOfShapes().get();
        loadNamingDS(aResultBody, anObject, aToolsForNaming, aPartitionAlgo.shape(), aMakeShapeList, aMapOfShapes);
        setResult(aResultBody, aResultIndex);
        aResultIndex++;
      }
    }
  }

  // remove the rest results if there were produced in the previous pass
  removeResults(aResultIndex);
}

//=================================================================================================
void FeaturesPlugin_Partition::loadNamingDS(std::shared_ptr<ModelAPI_ResultBody> theResultBody,
                                            const std::shared_ptr<GeomAPI_Shape> theBaseShape,
                                            const ListOfShape& theTools,
                                            const std::shared_ptr<GeomAPI_Shape> theResultShape,
                                            GeomAlgoAPI_MakeShape& theMakeShape,
                                            GeomAPI_DataMapOfShapeShape& theMapOfShapes)
{
  //load result
  if(theBaseShape->isEqual(theResultShape)) {
    theResultBody->store(theResultShape);
  } else {
    const int aDeletedTag = 1;
    const int aSubsolidsTag = 2; /// sub solids will be placed at labels 3, 4, etc. if result is compound of solids
    const int aModifyTag = 100000;
    int aModifyToolsTag = 200000;
    std::ostringstream aStream;

    theResultBody->storeModified(theBaseShape, theResultShape, aSubsolidsTag);

    std::string aModName = "Modified";
    theResultBody->loadAndOrientModifiedShapes(&theMakeShape, theBaseShape, GeomAPI_Shape::FACE,
                                               aModifyTag, aModName, theMapOfShapes, true);
    theResultBody->loadDeletedShapes(&theMakeShape, theBaseShape, GeomAPI_Shape::FACE, aDeletedTag);

    int anIndex = 1;
    for(ListOfShape::const_iterator anIter = theTools.begin(); anIter != theTools.end(); anIter++) {
      aStream.str(std::string());
      aStream.clear();
      aStream << aModName << "_" << anIndex++;
      theResultBody->loadAndOrientModifiedShapes(&theMakeShape, *anIter, GeomAPI_Shape::FACE,
                                                 aModifyToolsTag, aStream.str(), theMapOfShapes, true);
      theResultBody->loadDeletedShapes(&theMakeShape, *anIter, GeomAPI_Shape::FACE, aDeletedTag);
      aModifyToolsTag += 10000;
    }
  }
}
