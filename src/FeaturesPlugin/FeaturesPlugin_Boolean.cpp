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
using namespace std;

#define DEBUG_ONE_OBJECT

#define FACE 4
#define _MODIFY_TAG 1
#define _DELETED_TAG 2
FeaturesPlugin_Boolean::FeaturesPlugin_Boolean()
{
}

void FeaturesPlugin_Boolean::initAttributes()
{
  data()->addAttribute(FeaturesPlugin_Boolean::TYPE_ID(), ModelAPI_AttributeInteger::typeId());

#ifdef DEBUG_ONE_OBJECT
  data()->addAttribute(FeaturesPlugin_Boolean::OBJECT_ID(), ModelAPI_AttributeReference::typeId());
#else
  AttributeSelectionListPtr aSelection = 
    std::dynamic_pointer_cast<ModelAPI_AttributeSelectionList>(data()->addAttribute(
    FeaturesPlugin_Boolean::OBJECT_LIST_ID(), ModelAPI_AttributeSelectionList::typeId()));
  // extrusion works with faces always
  aSelection->setSelectionType("SOLID");
#endif

#ifdef DEBUG_ONE_OBJECT
  data()->addAttribute(FeaturesPlugin_Boolean::TOOL_ID(), ModelAPI_AttributeReference::typeId());
#else
  aSelection = std::dynamic_pointer_cast<ModelAPI_AttributeSelectionList>(data()->addAttribute(
    FeaturesPlugin_Boolean::TOOL_LIST_ID(), ModelAPI_AttributeSelectionList::typeId()));
  // extrusion works with faces always
  aSelection->setSelectionType("SOLID");
#endif
}

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


void FeaturesPlugin_Boolean::execute()
{
  std::shared_ptr<ModelAPI_AttributeInteger> aTypeAttr = std::dynamic_pointer_cast<
      ModelAPI_AttributeInteger>(data()->attribute(FeaturesPlugin_Boolean::TYPE_ID()));
  if (!aTypeAttr)
    return;
  int aType = aTypeAttr->value();
#ifdef DEBUG_ONE_OBJECT
  std::shared_ptr<GeomAPI_Shape> anObject = this->getShape(FeaturesPlugin_Boolean::OBJECT_ID());
#else
  std::shared_ptr<GeomAPI_Shape> anObject;
  {
    AttributeSelectionListPtr anObjects = selectionList(FeaturesPlugin_Boolean::OBJECT_LIST_ID());
    if (anObjects->size() == 0)
      return;

    // Getting bounding planes.
    std::shared_ptr<ModelAPI_AttributeSelection> anObjRef = anObjects->value(0);
    if (!anObjRef.get())
      return;
    anObject = std::dynamic_pointer_cast<GeomAPI_Shape>(anObjRef->value());
  }
#endif
  if (!anObject)
    return;

#ifdef DEBUG_ONE_OBJECT
  std::shared_ptr<GeomAPI_Shape> aTool = this->getShape(FeaturesPlugin_Boolean::TOOL_ID());
#else
  std::shared_ptr<GeomAPI_Shape> aTool;
  {
    AttributeSelectionListPtr anObjects = selectionList(FeaturesPlugin_Boolean::TOOL_LIST_ID());
    if (anObjects->size() == 0)
      return;

    // Getting bounding planes.
    std::shared_ptr<ModelAPI_AttributeSelection> anObjRef = anObjects->value(0);
    if (!anObjRef.get())
      return;
    aTool = std::dynamic_pointer_cast<GeomAPI_Shape>(anObjRef->value());
  }
#endif
  if (!aTool)
    return;

  std::shared_ptr<ModelAPI_ResultBody> aResultBody = document()->createBody(data());

  GeomAlgoAPI_Boolean* aFeature = new GeomAlgoAPI_Boolean(anObject, aTool, aType);
  if(aFeature && !aFeature->isDone()) {
    static const std::string aFeatureError = "Boolean feature: algorithm failed";  
    setError(aFeatureError);
    return;
  }
   // Check if shape is valid
  if (aFeature->shape()->isNull()) {
    static const std::string aShapeError = "Boolean feature: resulting shape is Null";     
    setError(aShapeError);
    return;
  }
  if(!aFeature->isValid()) {
    static const std::string aFeatureError = "Boolean feature: resulting shape is not valid";  
    setError(aFeatureError);
    return;
  }  
  // if result of Boolean operation is same as was before it means that Boolean operation has no sence
  // and naming provides no result, so, generate an error in this case
  if (anObject->isEqual(aFeature->shape())) {
    static const std::string aFeatureError = "Boolean feature: operation was not performed";  
    setError(aFeatureError);
    return;
  }
  //LoadNamingDS
  LoadNamingDS(aFeature, aResultBody, anObject, aTool, aType);

  setResult(aResultBody);
}

//============================================================================
void FeaturesPlugin_Boolean::LoadNamingDS(GeomAlgoAPI_Boolean* theFeature, 
						std::shared_ptr<ModelAPI_ResultBody> theResultBody, 
						std::shared_ptr<GeomAPI_Shape> theObject,
						std::shared_ptr<GeomAPI_Shape> theTool,
						int theType)
{  

  //load result
  theResultBody->storeModified(theObject, theFeature->shape()); 

  GeomAPI_DataMapOfShapeShape* aSubShapes = new GeomAPI_DataMapOfShapeShape();
  theFeature->mapOfShapes(*aSubShapes);

  // Put in DF modified faces
  std::string aModName = "Modified";
  theResultBody->loadAndOrientModifiedShapes(theFeature->makeShape(), theObject, FACE, _MODIFY_TAG, aModName, *aSubShapes);
  theResultBody->loadAndOrientModifiedShapes(theFeature->makeShape(), theTool,   FACE, _MODIFY_TAG, aModName, *aSubShapes);

  //Put in DF deleted faces
  theResultBody->loadDeletedShapes(theFeature->makeShape(), theObject, FACE, _DELETED_TAG);
  theResultBody->loadDeletedShapes(theFeature->makeShape(), theTool,   FACE, _DELETED_TAG);  
}
