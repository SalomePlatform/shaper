// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:        FeaturesPlugin_Boolean.cpp
// Created:     02 Sept 2014
// Author:      Vitaly SMETANNIKOV

#include "FeaturesPlugin_Boolean.h"

#include <ModelAPI_Data.h>
#include <ModelAPI_Document.h>
#include <ModelAPI_AttributeReference.h>
#include <ModelAPI_AttributeInteger.h>
#include <ModelAPI_ResultBody.h>
#include <ModelAPI_AttributeSelectionList.h>

#include <GeomAlgoAPI_Boolean.h>
#include <GeomAlgoAPI_MakeShapeList.h>
#include <GeomAlgoAPI_ShapeProps.h>

#define FACE 4
#define _MODIFY_TAG 1
#define _DELETED_TAG 2

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
  int aType = aTypeAttr->value();

  ListOfShape anObjects, aTools;

  // Getting objects.
  AttributeSelectionListPtr anObjectsSelList = selectionList(FeaturesPlugin_Boolean::OBJECT_LIST_ID());
  if (anObjectsSelList->size() == 0) {
    return;
  }
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
  if (aToolsSelList->size() == 0) {
    return;
  }
  for(int aToolsIndex = 0; aToolsIndex < aToolsSelList->size(); aToolsIndex++) {
    std::shared_ptr<ModelAPI_AttributeSelection> aToolAttr = aToolsSelList->value(aToolsIndex);
    std::shared_ptr<GeomAPI_Shape> aTool = aToolAttr->value();
    if(!aTool.get()) {
      return;
    }
    aTools.push_back(aTool);
  }

  int aResultIndex = 0;
  ListOfMakeShape aListOfMakeShape;
  std::shared_ptr<GeomAPI_Shape> aResShape;
  std::shared_ptr<GeomAPI_DataMapOfShapeShape> aDataMapOfShapes;

  switch(aType) {
    case GeomAlgoAPI_Boolean::BOOL_CUT: {
      // Cut each object with all tools
      for(ListOfShape::iterator anObjectsIt = anObjects.begin(); anObjectsIt != anObjects.end(); anObjectsIt++) {
        std::shared_ptr<GeomAPI_Shape> anObject = *anObjectsIt;
        aResShape = anObject;
        ListOfShape aSingleObjectList; aSingleObjectList.push_back(anObject);
        for(ListOfShape::iterator aToolsIt = aTools.begin(); aToolsIt != aTools.end(); aToolsIt++) {
          std::shared_ptr<GeomAPI_Shape> aTool = *aToolsIt;
          GeomAlgoAPI_Boolean* aBoolAlgo = new GeomAlgoAPI_Boolean(aResShape, aTool, aType);
          if(!aBoolAlgo->isDone() || aBoolAlgo->shape()->isNull() || !aBoolAlgo->isValid()) {
            static const std::string aBoolAlgoError = "Boolean feature: algorithm failed";
            setError(aBoolAlgoError);
            return;
          }
          aListOfMakeShape.push_back(aBoolAlgo->makeShape());
          aResShape = aBoolAlgo->shape();
          aBoolAlgo->mapOfShapes(aDataMapOfShapes);
        }

        if(GeomAlgoAPI_ShapeProps::volume(aResShape) > 10.e-5) {
          std::shared_ptr<ModelAPI_ResultBody> aResultBody = document()->createBody(data(), aResultIndex);
          std::shared_ptr<GeomAlgoAPI_MakeShapeList> aMakeShapeList = std::shared_ptr<GeomAlgoAPI_MakeShapeList>(
            new GeomAlgoAPI_MakeShapeList(aListOfMakeShape));

          LoadNamingDS(aMakeShapeList, aResultBody, aResShape, aDataMapOfShapes, aSingleObjectList, aTools);
          setResult(aResultBody, aResultIndex);
          aResultIndex++;
        }
      }
      break;
    }
    case GeomAlgoAPI_Boolean::BOOL_FUSE: {
      // Fuse all objects.
      std::shared_ptr<GeomAPI_Shape> aTempRes = anObjects.front();
      for(ListOfShape::iterator anObjectsIt = ++anObjects.begin(); anObjectsIt != anObjects.end(); anObjectsIt++) {
        std::shared_ptr<GeomAPI_Shape> anObject = *anObjectsIt;
        GeomAlgoAPI_Boolean* aBoolAlgo = new GeomAlgoAPI_Boolean(aTempRes, anObject, aType);
        if(!aBoolAlgo->isDone() || aBoolAlgo->shape()->isNull() || !aBoolAlgo->isValid()) {
          static const std::string aBoolAlgoError = "Boolean feature: algorithm failed";
          setError(aBoolAlgoError);
          return;
        }
        aListOfMakeShape.push_back(aBoolAlgo->makeShape());
        aTempRes = aBoolAlgo->shape();
      }

      // Fuse all tools with result of objects fuse.
      for(ListOfShape::iterator aToolsIt = aTools.begin(); aToolsIt != aTools.end(); aToolsIt++) {
        std::shared_ptr<GeomAPI_Shape> aTool = *aToolsIt;
        GeomAlgoAPI_Boolean* aBoolAlgo = new GeomAlgoAPI_Boolean(aTempRes, aTool, aType);
        if(!aBoolAlgo->isDone() || aBoolAlgo->shape()->isNull() || !aBoolAlgo->isValid()) {
          static const std::string aBoolAlgoError = "Boolean feature: algorithm failed";
          setError(aBoolAlgoError);
          return;
        }
        aListOfMakeShape.push_back(aBoolAlgo->makeShape());
        aTempRes = aBoolAlgo->shape();
        aBoolAlgo->mapOfShapes(aDataMapOfShapes);
      }
      aResShape = aTempRes;

      std::shared_ptr<ModelAPI_ResultBody> aResultBody = document()->createBody(data());
      std::shared_ptr<GeomAlgoAPI_MakeShapeList> aMakeShapeList = std::shared_ptr<GeomAlgoAPI_MakeShapeList>(
        new GeomAlgoAPI_MakeShapeList(aListOfMakeShape));

      LoadNamingDS(aMakeShapeList, aResultBody, aResShape, aDataMapOfShapes, anObjects, aTools);
      setResult(aResultBody);
      aResultIndex++;
      break;
    }
    case GeomAlgoAPI_Boolean::BOOL_COMMON: {
      // Search common between object and tool and fuse them.
      for(ListOfShape::iterator anObjectsIt = anObjects.begin(); anObjectsIt != anObjects.end(); anObjectsIt++) {
        std::shared_ptr<GeomAPI_Shape> anObject = *anObjectsIt;
        std::shared_ptr<GeomAPI_Shape> aTempRes;
        ListOfShape aSingleObjectList; aSingleObjectList.push_back(anObject);
        for(ListOfShape::iterator aToolsIt = aTools.begin(); aToolsIt != aTools.end(); aToolsIt++) {
          std::shared_ptr<GeomAPI_Shape> aTool = *aToolsIt;
          GeomAlgoAPI_Boolean* aBoolAlgo1 = new GeomAlgoAPI_Boolean(anObject, aTool, aType);
          if(!aBoolAlgo1->isDone() || aBoolAlgo1->shape()->isNull() || !aBoolAlgo1->isValid()) {
            static const std::string aBoolAlgoError = "Boolean feature: algorithm failed";
            setError(aBoolAlgoError);
            return;
          }
          aListOfMakeShape.push_back(aBoolAlgo1->makeShape());

          if(!aTempRes) {
            aTempRes = aBoolAlgo1->shape();
            aBoolAlgo1->mapOfShapes(aDataMapOfShapes);
          } else {
            // Fuse common result with previous common results.
            GeomAlgoAPI_Boolean* aBoolAlgo2 = new GeomAlgoAPI_Boolean(aTempRes,
                                                                      aBoolAlgo1->shape(),
                                                                      GeomAlgoAPI_Boolean::BOOL_FUSE);
            if(!aBoolAlgo1->isDone() || aBoolAlgo1->shape()->isNull() || !aBoolAlgo1->isValid()) {
              static const std::string aBoolAlgoError = "Boolean feature: algorithm failed";
              setError(aBoolAlgoError);
              return;
            }
            aListOfMakeShape.push_back(aBoolAlgo2->makeShape());
            aTempRes = aBoolAlgo2->shape();
            aBoolAlgo2->mapOfShapes(aDataMapOfShapes);
          }
        }
        aResShape = aTempRes;

        if(GeomAlgoAPI_ShapeProps::volume(aResShape) > 10.e-5) {
          std::shared_ptr<ModelAPI_ResultBody> aResultBody = document()->createBody(data(), aResultIndex);
          std::shared_ptr<GeomAlgoAPI_MakeShapeList> aMakeShapeList = std::shared_ptr<GeomAlgoAPI_MakeShapeList>(
            new GeomAlgoAPI_MakeShapeList(aListOfMakeShape));

          LoadNamingDS(aMakeShapeList, aResultBody, aResShape, aDataMapOfShapes, aSingleObjectList, aTools);
          setResult(aResultBody, aResultIndex);
          aResultIndex++;
        }
      }
      break;
    }
  }
  // remove the rest results if there were produced in the previous pass
  removeResults(aResultIndex);
}

//=================================================================================================
void FeaturesPlugin_Boolean::LoadNamingDS(std::shared_ptr<GeomAlgoAPI_MakeShapeList> theMakeShapeList,
                                          std::shared_ptr<ModelAPI_ResultBody> theResultBody,
                                          std::shared_ptr<GeomAPI_Shape> theResult,
                                          std::shared_ptr<GeomAPI_DataMapOfShapeShape> theDataMapOfShapes,
                                          const ListOfShape& theObjects,
                                          const ListOfShape& theTools)
{
  //load result
  if(theObjects.front()->isEqual(theResult)) {
    theResultBody->store(theObjects.front());
  } else {
    theResultBody->storeModified(theObjects.front(), theResult);

    GeomAPI_DataMapOfShapeShape* aSubShapes = new GeomAPI_DataMapOfShapeShape();

    std::string aModName = "Modified";
    for(ListOfShape::const_iterator anIter = theObjects.begin(); anIter != theObjects.end(); anIter++) {
      theResultBody->loadAndOrientModifiedShapes(theMakeShapeList.get(), *anIter, FACE, _MODIFY_TAG, aModName, *theDataMapOfShapes.get());
      theResultBody->loadDeletedShapes(theMakeShapeList.get(), *anIter, FACE, _DELETED_TAG);
    }

    for(ListOfShape::const_iterator anIter = theTools.begin(); anIter != theTools.end(); anIter++) {
      theResultBody->loadAndOrientModifiedShapes(theMakeShapeList.get(), *anIter, FACE, _MODIFY_TAG, aModName, *theDataMapOfShapes.get());
      theResultBody->loadDeletedShapes(theMakeShapeList.get(), *anIter, FACE, _DELETED_TAG);
    }
  }
}
