// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        ModelAPI_Feature.cpp
// Created:     17 Jul 2014
// Author:      Mikhail PONIKAROV

#include "ModelAPI_Feature.h"
#include <ModelAPI_Events.h>
#include <ModelAPI_Result.h>
#include <ModelAPI_Data.h>
#include <ModelAPI_Document.h>
#include <ModelAPI_Session.h>
#include <Events_Loop.h>

const std::list<std::shared_ptr<ModelAPI_Result> >& ModelAPI_Feature::results()
{
  return myResults;
}

std::shared_ptr<ModelAPI_Result> ModelAPI_Feature::firstResult()
{
  return myResults.empty() ? std::shared_ptr<ModelAPI_Result>() : *(myResults.begin());
}

std::shared_ptr<ModelAPI_Result> ModelAPI_Feature::lastResult()
{
  return myResults.empty() ? std::shared_ptr<ModelAPI_Result>() : *(myResults.rbegin());
}

void ModelAPI_Feature::setResult(const std::shared_ptr<ModelAPI_Result>& theResult)
{
  static Events_Loop* aLoop = Events_Loop::loop();
  static Events_ID EVENT_UPD = Events_Loop::eventByName(EVENT_OBJECT_UPDATED);
  static const ModelAPI_EventCreator* aECreator = ModelAPI_EventCreator::get();

  if (firstResult() == theResult) {
    // nothing to change
  } else if (!myResults.empty()) {  // all except first become disabled
    std::list<std::shared_ptr<ModelAPI_Result> >::iterator aResIter = myResults.begin();
    *aResIter = theResult;
    aECreator->sendUpdated(theResult, EVENT_UPD);
    for(aResIter++; aResIter != myResults.end(); aResIter++) {
      (*aResIter)->setDisabled((*aResIter), true);
    }
  } else {
    myResults.push_back(theResult);
  }
  // in any case result decomes enabled
  theResult->setDisabled(theResult, false);
}

void ModelAPI_Feature::setResult(const std::shared_ptr<ModelAPI_Result>& theResult,
                                 const int theIndex)
{
  std::list<std::shared_ptr<ModelAPI_Result> >::iterator aResIter = myResults.begin();
  for (int anIndex = 0; anIndex < theIndex; anIndex++) {
    aResIter++;
  }
  if (aResIter == myResults.end()) {  // append
    myResults.push_back(theResult);
  } else {  // update
    *aResIter = theResult;
  }
  theResult->setDisabled(theResult, false);
}

void ModelAPI_Feature::removeResult(const std::shared_ptr<ModelAPI_Result>& theResult)
{
  theResult->setDisabled(theResult, true);
}

void ModelAPI_Feature::removeResults(const int theSinceIndex)
{
  std::list<std::shared_ptr<ModelAPI_Result> >::iterator aResIter = myResults.begin();
  for(int anIndex = 0; anIndex < theSinceIndex && aResIter != myResults.end(); anIndex++)
    aResIter++;
  std::list<std::shared_ptr<ModelAPI_Result> >::iterator aNextIter = aResIter;
  for(; aNextIter != myResults.end(); aNextIter++) {
    (*aNextIter)->setDisabled(*aNextIter, true); // just disable results
  }
}

void ModelAPI_Feature::eraseResults()
{
  removeResults(0);
}

const std::string& ModelAPI_Feature::documentToAdd()
{
  // empty to use the current document
  static const std::string anEmpty;
  return anEmpty;
}

void ModelAPI_Feature::erase()
{
  static Events_Loop* aLoop = Events_Loop::loop();
  static Events_ID EVENT_DISP = aLoop->eventByName(EVENT_OBJECT_TO_REDISPLAY);
  static const ModelAPI_EventCreator* aECreator = ModelAPI_EventCreator::get();

  while (!myResults.empty()) {  // remove one by one with messages
    std::shared_ptr<ModelAPI_Result> aRes = *(myResults.begin());
    myResults.erase(myResults.begin());
    aECreator->sendDeleted(aRes->document(), aRes->groupName());
    aECreator->sendUpdated(aRes, EVENT_DISP);
  }
  ModelAPI_Object::erase();
}

ModelAPI_Feature::~ModelAPI_Feature()
{
  erase();
}

FeaturePtr ModelAPI_Feature::feature(ObjectPtr theObject)
{
  FeaturePtr aFeature = std::dynamic_pointer_cast<ModelAPI_Feature>(theObject);
  if (!aFeature) {
    ResultPtr aResult = std::dynamic_pointer_cast<ModelAPI_Result>(theObject);
    if (aResult) {
      DocumentPtr aDoc = aResult->document();
      return aDoc->feature(aResult);
    }
  }
  return aFeature;
}


bool ModelAPI_Feature::isMacro() const
{
  return false;
}

bool ModelAPI_Feature::setDisabled(const bool theFlag)
{
  if (myIsDisabled != theFlag) {
    myIsDisabled = theFlag;
    if (myIsDisabled)
      eraseResults();
    return true;
  }
  return false;
}

bool ModelAPI_Feature::isDisabled() const
{
  return myIsDisabled;
}
