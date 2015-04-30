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

void appendValues(std::vector<int>& theRGB, const int theRed, const int theGreen, const int theBlue)
{
  theRGB.push_back(theRed);
  theRGB.push_back(theGreen);
  theRGB.push_back(theBlue);
}

bool containsValues(std::map<int, std::vector<int> >& theColorMap, std::vector<int>& theValues)
{
  std::map<int, std::vector<int> >::const_iterator anIt = theColorMap.begin(), aLast = theColorMap.end();
  bool isFound = false;
  for (; anIt != aLast && !isFound; anIt++) {
    std::vector<int> aValues = anIt->second;
    isFound = aValues[0] == theValues[0] &&
              aValues[1] == theValues[1] &&
              aValues[2] == theValues[2];
  }
  return isFound;
}

std::vector<int> HSVtoRGB(int theH, int theS, int theV)
{
  std::vector<int> aRGB;
  if (theH < 0 || theH > 360 ||
      theS < 0 || theS > 100 ||
      theV < 0 || theV > 100)
    return aRGB;

  int aHi = (int)theH/60;

  double aV = theV;
  double aVmin = (100 - theS)*theV/100;

  double anA = (theV - aVmin)* (theH % 60) / 60;

  double aVinc = aVmin + anA;
  double aVdec = theV - anA;

  double aPercentToValue = 255./100;
  aV    *= aPercentToValue;
  aVinc *= aPercentToValue;
  aVmin *= aPercentToValue;
  aVdec *= aPercentToValue;

  switch(aHi) {
    case 0: appendValues(aRGB, aV,    aVinc, aVmin); break;
    case 1: appendValues(aRGB, aVdec, aV,    aVmin); break;
    case 2: appendValues(aRGB, aVmin, aV,    aVinc); break;
    case 3: appendValues(aRGB, aVmin, aVdec, aV); break;
    case 4: appendValues(aRGB, aVinc, aVmin, aV); break;
    case 5: appendValues(aRGB, aV,    aVmin, aVdec); break;
    default: break;
  }
  return aRGB;
}


void fillColorMap()
{
  if (!myColorMap.empty())
    return;

  int i = 0;
  for (int s = 100; s > 0; s = s - 50)
  {
    for (int v = 100; v >= 40; v = v - 20)
    {
      for (int h = 0; h < 359 ; h = h + 60)
      {
        std::vector<int> aColor = HSVtoRGB(h, s, v);
        if (containsValues(myColorMap, aColor))
          continue;
        myColorMap[i] = aColor;
        i++;
      }
    }
  }
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
