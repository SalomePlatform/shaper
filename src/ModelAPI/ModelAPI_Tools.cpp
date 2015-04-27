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
#include <map>

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

static std::map<int, std::vector<int> > myColorMap;

std::vector<int> vectorOfValues(const int theRed, const int theGreen, const int theBlue)
{
  std::vector<int> aValues;
  aValues.push_back(theRed);
  aValues.push_back(theGreen);
  aValues.push_back(theBlue);

  return aValues;
}

void fillColorMap()
{
  if (!myColorMap.empty())
    return;
  myColorMap[0] = vectorOfValues(127, 51, 0);
  myColorMap[1] = vectorOfValues(0, 38, 225);
  myColorMap[2] = vectorOfValues(255, 0, 0);
}

void findRandomColor(std::vector<int>& theValues)
{
  theValues.clear();
  if (myColorMap.empty()) {
    fillColorMap();
  }

  int aSize = myColorMap.size();
  int anIndex = rand() % aSize;
  if (myColorMap.find(anIndex) != myColorMap.end()) {
    theValues = myColorMap.at(anIndex);
  }
}

} // namespace ModelAPI_Tools
