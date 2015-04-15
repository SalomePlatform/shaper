// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        ModelAPI_Tools.cpp
// Created:     06 Aug 2014
// Author:      Vitaly Smetannikov

#include "ModelAPI_Tools.h"
#include <ModelAPI_Session.h>
#include <ModelAPI_Document.h>
#include <ModelAPI_Object.h>
#include <ModelAPI_AttributeDouble.h>
#include <ModelAPI_ResultParameter.h>

#include <list>

namespace ModelAPI_Tools {

std::shared_ptr<GeomAPI_Shape> shape(const ResultPtr& theResult)
{
/*
  ResultBodyPtr aBody = std::dynamic_pointer_cast<ModelAPI_ResultBody>(theResult);
  if (aBody)
    return aBody->shape();

  ResultConstructionPtr aConstruct = std::dynamic_pointer_cast<ModelAPI_ResultConstruction>(
    theResult);
  if (aConstruct)
    return aConstruct->shape();

  ResultGroupPtr aGroup = std::dynamic_pointer_cast<ModelAPI_ResultGroup>(theResult);
  if (aGroup)
    return aGroup->shape();
  return std::shared_ptr<GeomAPI_Shape>();
  */
  return theResult->shape();
}

bool findVariable(const std::string& theName, double& outValue)
{
  SessionPtr aSession = ModelAPI_Session::get();
  std::list<DocumentPtr> aDocList;
  DocumentPtr aDocument = aSession->activeDocument();
  DocumentPtr aRootDocument = aSession->moduleDocument();
  aDocList.push_back(aDocument);
  if (aDocument != aRootDocument) {
    aDocList.push_back(aRootDocument);
  }
  for(std::list<DocumentPtr>::const_iterator it = aDocList.begin(); it != aDocList.end(); ++it) {
    ObjectPtr aParamObj = (*it)->objectByName(ModelAPI_ResultParameter::group(), theName);
    ResultParameterPtr aParam = std::dynamic_pointer_cast<ModelAPI_ResultParameter>(aParamObj);
    if(!aParam.get())
      continue;
    AttributeDoublePtr aValueAttribute = aParam->data()->real(ModelAPI_ResultParameter::VALUE());
    outValue = aValueAttribute->value();
    return true;
  }
  return false;
}

} // namespace ModelAPI_Tools
