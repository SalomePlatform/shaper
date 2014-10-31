// File:        FeaturesPlugin_Boolean.cpp
// Created:     02 Sept 2014
// Author:      Vitaly SMETANNIKOV

#include "FeaturesPlugin_Boolean.h"

#include <ModelAPI_Data.h>
#include <ModelAPI_Document.h>
#include <ModelAPI_AttributeReference.h>
#include <ModelAPI_AttributeInteger.h>
#include <ModelAPI_ResultBody.h>
#include <GeomAlgoAPI_Boolean.h>
#include <Events_Error.h>
using namespace std;
#ifdef _DEBUG
#include <iostream>
#include <ostream>
#endif

#define FACE 4
#define _MODIFY_TAG 1
#define _DELETED_TAG 2
FeaturesPlugin_Boolean::FeaturesPlugin_Boolean()
{
}

void FeaturesPlugin_Boolean::initAttributes()
{
  data()->addAttribute(FeaturesPlugin_Boolean::TYPE_ID(), ModelAPI_AttributeInteger::type());
  data()->addAttribute(FeaturesPlugin_Boolean::OBJECT_ID(), ModelAPI_AttributeReference::type());
  data()->addAttribute(FeaturesPlugin_Boolean::TOOL_ID(), ModelAPI_AttributeReference::type());
}

boost::shared_ptr<GeomAPI_Shape> FeaturesPlugin_Boolean::getShape(const std::string& theAttrName)
{
  boost::shared_ptr<ModelAPI_AttributeReference> aObjRef = boost::dynamic_pointer_cast<
      ModelAPI_AttributeReference>(data()->attribute(theAttrName));
  if (aObjRef) {
    boost::shared_ptr<ModelAPI_ResultBody> aConstr = boost::dynamic_pointer_cast<
        ModelAPI_ResultBody>(aObjRef->value());
    if (aConstr)
      return aConstr->shape();
  }
  return boost::shared_ptr<GeomAPI_Shape>();
}


void FeaturesPlugin_Boolean::execute()
{
  boost::shared_ptr<ModelAPI_AttributeInteger> aTypeAttr = boost::dynamic_pointer_cast<
      ModelAPI_AttributeInteger>(data()->attribute(FeaturesPlugin_Boolean::TYPE_ID()));
  if (!aTypeAttr)
    return;
  int aType = aTypeAttr->value();

  boost::shared_ptr<GeomAPI_Shape> anObject = this->getShape(FeaturesPlugin_Boolean::OBJECT_ID());
  if (!anObject)
    return;

  boost::shared_ptr<GeomAPI_Shape> aTool = this->getShape(FeaturesPlugin_Boolean::TOOL_ID());
  if (!aTool)
    return;

  boost::shared_ptr<ModelAPI_ResultBody> aResultBody = document()->createBody(data());

  GeomAlgoAPI_Boolean* aFeature = new GeomAlgoAPI_Boolean(anObject, aTool, aType);
  if(aFeature && !aFeature->isDone()) {
    std::string aFeatureError = "Boolean feature: algorithm failed";  
    Events_Error::send(aFeatureError, this);
    return;
  }
   // Check if shape is valid
  if (aFeature->shape()->isNull()) {
    std::string aShapeError = "Boolean feature: resulting shape is Null";     
    Events_Error::send(aShapeError, this);
#ifdef _DEBUG
    std::cerr << aShapeError << std::endl;
#endif
    return;
  }
  if(!aFeature->isValid()) {
    std::string aFeatureError = "Boolean feature: resulting shape is not valid";  
    Events_Error::send(aFeatureError, this);
    return;
  }  
  //LoadNamingDS
  LoadNamingDS(aFeature, aResultBody, anObject, aTool, aType);

  setResult(aResultBody);
}

//============================================================================
void FeaturesPlugin_Boolean::LoadNamingDS(GeomAlgoAPI_Boolean* theFeature, 
						boost::shared_ptr<ModelAPI_ResultBody> theResultBody, 
						boost::shared_ptr<GeomAPI_Shape> theObject,
						boost::shared_ptr<GeomAPI_Shape> theTool,
						int theType)
{  

  //load result
  theResultBody->storeModified(theObject, theFeature->shape()); 

  GeomAPI_DataMapOfShapeShape* aSubShapes = new GeomAPI_DataMapOfShapeShape();
  theFeature->mapOfShapes(*aSubShapes);

  // Put in DF modified faces
  theResultBody->loadAndOrientModifiedShapes(theFeature->makeShape(), theObject, FACE, _MODIFY_TAG, *aSubShapes);
  theResultBody->loadAndOrientModifiedShapes(theFeature->makeShape(), theTool,   FACE, _MODIFY_TAG, *aSubShapes);

  //Put in DF deleted faces
  theResultBody->loadDeletedShapes(theFeature->makeShape(), theObject, FACE, _DELETED_TAG);
  theResultBody->loadDeletedShapes(theFeature->makeShape(), theTool,   FACE, _DELETED_TAG);  
}
