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

using namespace std;

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

  boost::shared_ptr<GeomAPI_Shape> aObject = this->getShape(FeaturesPlugin_Boolean::OBJECT_ID());
  if (!aObject)
    return;

  boost::shared_ptr<GeomAPI_Shape> aTool = this->getShape(FeaturesPlugin_Boolean::TOOL_ID());
  if (!aTool)
    return;

  boost::shared_ptr<ModelAPI_ResultBody> aResult = document()->createBody(data());
  switch (aType) {
  case BOOL_CUT:
    aResult->store(GeomAlgoAPI_Boolean::makeCut(aObject, aTool));
    break;
  case BOOL_FUSE:
    aResult->store(GeomAlgoAPI_Boolean::makeFuse(aObject, aTool));
    break;
  case BOOL_COMMON:
    aResult->store(GeomAlgoAPI_Boolean::makeCommon(aObject, aTool));
    break;
  }
  setResult(aResult);
}