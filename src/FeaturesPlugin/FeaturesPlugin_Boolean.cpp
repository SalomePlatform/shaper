// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:        FeaturesPlugin_Boolean.cpp
// Created:     02 Sept 2014
// Author:      Vitaly SMETANNIKOV

#include "FeaturesPlugin_Boolean.h"

#include <ModelAPI_Data.h>
#include <ModelAPI_Document.h>
#include <ModelAPI_AttributeReference.h>
#include <ModelAPI_AttributeInteger.h>
#include <ModelAPI_BodyBuilder.h>
#include <ModelAPI_ResultBody.h>
#include <ModelAPI_AttributeSelectionList.h>
#include <ModelAPI_Session.h>
#include <ModelAPI_Validator.h>

#include <GeomAlgoAPI_Boolean.h>
#include <GeomAlgoAPI_MakeShapeList.h>
#include <GeomAlgoAPI_ShapeProps.h>

#define FACE 4
#define _MODIFY_TAG 1
#define _DELETED_TAG 2
#define _SUBSOLIDS_TAG 3 /// sub solids will be placed at labels 3, 4, etc. if result is compound of solids

//=================================================================================================
FeaturesPlugin_Boolean::FeaturesPlugin_Boolean()
{
}

//=================================================================================================
void FeaturesPlugin_Boolean::initAttributes()
{
  data()->addAttribute(FeaturesPlugin_Boolean::TYPE_ID(), ModelAPI_AttributeInteger::typeId());

  AttributeSelectionListPtr aSelection = 
    std::dynamic_pointer_cast<ModelAPI_AttributeSelectionList>(data()->addAttribute(
    FeaturesPlugin_Boolean::OBJECT_LIST_ID(), ModelAPI_AttributeSelectionList::typeId()));
  // extrusion works with faces always
  aSelection->setSelectionType("SOLID");

  aSelection = std::dynamic_pointer_cast<ModelAPI_AttributeSelectionList>(data()->addAttribute(
    FeaturesPlugin_Boolean::TOOL_LIST_ID(), ModelAPI_AttributeSelectionList::typeId()));
  // extrusion works with faces always
  aSelection->setSelectionType("SOLID");

  ModelAPI_Session::get()->validators()->registerNotObligatory(getKind(), OBJECT_LIST_ID());
  ModelAPI_Session::get()->validators()->registerNotObligatory(getKind(), TOOL_LIST_ID());
}

//=================================================================================================
std::shared_ptr<GeomAPI_Shape> FeaturesPlugin_Boolean::getShape(const std::string& theAttrName)
{
  std::shared_ptr<ModelAPI_AttributeReference> aObjRef = std::dynamic_pointer_cast<
      ModelAPI_AttributeReference>(data()->attribute(theAttrName));
  if (aObjRef) {
    std::shared_ptr<ModelAPI_ResultBody> aConstr = std::dynamic_pointer_cast<
        ModelAPI_ResultBody>(aObjRef->value());
    if (aConstr)
      return aConstr->shape();
  }
  return std::shared_ptr<GeomAPI_Shape>();
}

//=================================================================================================
void FeaturesPlugin_Boolean::execute()
{
  // Getting operation type.
  std::shared_ptr<ModelAPI_AttributeInteger> aTypeAttr = std::dynamic_pointer_cast<
      ModelAPI_AttributeInteger>(data()->attribute(FeaturesPlugin_Boolean::TYPE_ID()));
  if (!aTypeAttr)
    return;
  GeomAlgoAPI_Boolean::OperationType aType = (GeomAlgoAPI_Boolean::OperationType)aTypeAttr->value();

  ListOfShape anObjects, aTools;

  // Getting objects.
  AttributeSelectionListPtr anObjectsSelList = selectionList(FeaturesPlugin_Boolean::OBJECT_LIST_ID());
  for(int anObjectsIndex = 0; anObjectsIndex < anObjectsSelList->size(); anObjectsIndex++) {
    std::shared_ptr<ModelAPI_AttributeSelection> anObjectAttr = anObjectsSelList->value(anObjectsIndex);
    std::shared_ptr<GeomAPI_Shape> anObject = anObjectAttr->value();
    if(!anObject.get()) {
      return;
    }
    anObjects.push_back(anObject);
  }

  // Getting tools.
  AttributeSelectionListPtr aToolsSelList = selectionList(FeaturesPlugin_Boolean::TOOL_LIST_ID());
  for(int aToolsIndex = 0; aToolsIndex < aToolsSelList->size(); aToolsIndex++) {
    std::shared_ptr<ModelAPI_AttributeSelection> aToolAttr = aToolsSelList->value(aToolsIndex);
    std::shared_ptr<GeomAPI_Shape> aTool = aToolAttr->value();
    if(!aTool.get()) {
      return;
    }
    aTools.push_back(aTool);
  }

  int aResultIndex = 0;

  switch(aType) {
    case GeomAlgoAPI_Boolean::BOOL_CUT:
    case GeomAlgoAPI_Boolean::BOOL_COMMON:{
      if(anObjects.empty() || aTools.empty()) {
        std::string aFeatureError = "Not enough objects for boolean operation";
        setError(aFeatureError);
        return;
      }

      // Cut each object with all tools
      for(ListOfShape::iterator anObjectsIt = anObjects.begin(); anObjectsIt != anObjects.end(); anObjectsIt++) {
        std::shared_ptr<GeomAPI_Shape> anObject = *anObjectsIt;
        ListOfShape aListWithObject;
        aListWithObject.push_back(anObject);
        GeomAlgoAPI_Boolean aBoolAlgo(aListWithObject, aTools, aType);

        // Checking that the algorithm worked properly.
        if(!aBoolAlgo.isDone()) {
          static const std::string aFeatureError = "Boolean algorithm failed";
          setError(aFeatureError);
          return;
        }
        if(aBoolAlgo.shape()->isNull()) {
          static const std::string aShapeError = "Resulting shape is Null";
          setError(aShapeError);
          return;
        }
        if(!aBoolAlgo.isValid()) {
          std::string aFeatureError = "Warning: resulting shape is not valid";
          setError(aFeatureError);
          return;
        }

        if(GeomAlgoAPI_ShapeProps::volume(aBoolAlgo.shape()) > 1.e-7) {
          std::shared_ptr<ModelAPI_ResultBody> aResultBody = document()->createBody(data(), aResultIndex);
          LoadNamingDS(aResultBody, anObject, aTools, aBoolAlgo);
          setResult(aResultBody, aResultIndex);
          aResultIndex++;
        }
      }
      break;
    }
    case GeomAlgoAPI_Boolean::BOOL_FUSE: {
      if(anObjects.empty() && aTools.size() > 1) {
        anObjects.push_back(aTools.back());
        aTools.pop_back();
      }else if(aTools.empty() && anObjects.size() > 1) {
        aTools.push_back(anObjects.back());
        anObjects.pop_back();
      }

      if(anObjects.empty() || aTools.empty()) {
        std::string aFeatureError = "Not enough objects for boolean operation";
        setError(aFeatureError);
        return;
      }

      // Fuse all objects and all tools.
      GeomAlgoAPI_Boolean aBoolAlgo(anObjects, aTools, aType);

      // Checking that the algorithm worked properly.
      if(!aBoolAlgo.isDone()) {
        static const std::string aFeatureError = "Boolean algorithm failed";
        setError(aFeatureError);
        return;
      }
      if(aBoolAlgo.shape()->isNull()) {
        static const std::string aShapeError = "Resulting shape is Null";
        setError(aShapeError);
        return;
      }
      if(!aBoolAlgo.isValid()) {
        std::string aFeatureError = "Warning: resulting shape is not valid";
        setError(aFeatureError);
        return;
      }

      std::shared_ptr<ModelAPI_ResultBody> aResultBody = document()->createBody(data(), aResultIndex);
      LoadNamingDS(aResultBody, anObjects.front(), aTools, aBoolAlgo);
      setResult(aResultBody, aResultIndex);
      aResultIndex++;
      break;
    }
    default: {
      std::string anOperationError = "Error: wrong type of operation";
      setError(anOperationError);
      return;
    }
  }
  // remove the rest results if there were produced in the previous pass
  removeResults(aResultIndex);
}

//=================================================================================================
void FeaturesPlugin_Boolean::LoadNamingDS(std::shared_ptr<ModelAPI_ResultBody> theResultBody,
                                          const std::shared_ptr<GeomAPI_Shape>& theBaseShape,
                                          const ListOfShape& theTools,
                                          const GeomAlgoAPI_Boolean& theAlgo)
{
  ModelAPI_BodyBuilder* aResultBuilder = theResultBody->getBodyBuilder();
  //load result
  if(theBaseShape->isEqual(theAlgo.shape())) {
    aResultBuilder->store(theAlgo.shape());
  } else {
    aResultBuilder->storeModified(theBaseShape, theAlgo.shape(), _SUBSOLIDS_TAG);

    GeomAPI_DataMapOfShapeShape* aSubShapes = new GeomAPI_DataMapOfShapeShape();

    std::string aModName = "Modified";
    aResultBuilder->loadAndOrientModifiedShapes(theAlgo.makeShape().get(), theBaseShape, FACE,
                                               _MODIFY_TAG, aModName, *theAlgo.mapOfShapes().get());
    aResultBuilder->loadDeletedShapes(theAlgo.makeShape().get(), theBaseShape, FACE, _DELETED_TAG);

    for(ListOfShape::const_iterator anIter = theTools.begin(); anIter != theTools.end(); anIter++) {
      aResultBuilder->loadAndOrientModifiedShapes(theAlgo.makeShape().get(), *anIter, FACE,
                                                 _MODIFY_TAG, aModName, *theAlgo.mapOfShapes().get());
      aResultBuilder->loadDeletedShapes(theAlgo.makeShape().get(), *anIter, FACE, _DELETED_TAG);
    }
  }
}
