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

std::shared_ptr<ModelAPI_Result> ModelAPI_Feature::firstResult() const
{
  return myResults.empty() ? std::shared_ptr<ModelAPI_Result>() : *(myResults.begin());
}

std::shared_ptr<ModelAPI_Result> ModelAPI_Feature::lastResult()
{
  return myResults.empty() ? std::shared_ptr<ModelAPI_Result>() : *(myResults.rbegin());
}

void ModelAPI_Feature::setResult(const std::shared_ptr<ModelAPI_Result>& theResult)
{
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
  // in any case result becomes enabled
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
  // flush visualisation changes
  static Events_Loop* aLoop = Events_Loop::loop();
  static Events_ID aRedispEvent = aLoop->eventByName(EVENT_OBJECT_TO_REDISPLAY);
  aLoop->flush(aRedispEvent);
}

void ModelAPI_Feature::eraseResultFromList(const std::shared_ptr<ModelAPI_Result>& theResult)
{
  std::list<std::shared_ptr<ModelAPI_Result> >::iterator aResIter = myResults.begin();
  for(; aResIter != myResults.end(); aResIter++) {
    ResultPtr aRes = *aResIter;
    if (aRes == theResult) {
      std::string aGroup = aRes->groupName();
      aRes->setDisabled(aRes, true); // for complex results to disable all subs
      aRes->data()->erase();
      myResults.erase(aResIter);

      static Events_Loop* aLoop = Events_Loop::loop();
      static Events_ID EVENT_DISP = aLoop->eventByName(EVENT_OBJECT_TO_REDISPLAY);
      static const ModelAPI_EventCreator* aECreator = ModelAPI_EventCreator::get();
      aECreator->sendDeleted(document(), aGroup);
      aECreator->sendUpdated(aRes, EVENT_DISP);
      break;
    }
  }
}

void ModelAPI_Feature::removeResults(const int theSinceIndex, const bool theFlush)
{
  std::list<std::shared_ptr<ModelAPI_Result> >::iterator aResIter = myResults.begin();
  for(int anIndex = 0; anIndex < theSinceIndex && aResIter != myResults.end(); anIndex++)
    aResIter++;

  std::string aGroup;
  std::list<std::shared_ptr<ModelAPI_Result> >::iterator aNextIter = aResIter;
  while( aNextIter != myResults.end()) {
    aGroup = (*aNextIter)->groupName();
    // remove previously erased results: to enable later if needed only actual (of history change)
    //if (theSinceIndex == 0 && (*aNextIter)->isDisabled()) {
    //  aNextIter = myResults.erase(aNextIter);
    //} else {
      (*aNextIter)->setDisabled(*aNextIter, true); // just disable results
      aNextIter++;
    //}
  }
  if (!aGroup.empty() && theFlush) {
    // flush visualisation changes
    static Events_Loop* aLoop = Events_Loop::loop();
    static Events_ID aRedispEvent = aLoop->eventByName(EVENT_OBJECT_TO_REDISPLAY);
    aLoop->flush(aRedispEvent);
    static Events_ID aDelEvent = aLoop->eventByName(EVENT_OBJECT_DELETED);
    aLoop->flush(aDelEvent);
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
  // if this is the current feature, make the upper feature as current before removing
  if (document().get() && document()->currentFeature(false).get() == this) {
    document()->setCurrentFeatureUp();
  }

  static Events_Loop* aLoop = Events_Loop::loop();
  static Events_ID EVENT_DISP = aLoop->eventByName(EVENT_OBJECT_TO_REDISPLAY);
  static const ModelAPI_EventCreator* aECreator = ModelAPI_EventCreator::get();

  while (!myResults.empty()) {  // remove one by one with messages
    std::shared_ptr<ModelAPI_Result> aRes = *(myResults.begin());
    aRes->setDisabled(aRes, true); // to avoid activation of the Part result
    if (!myResults.empty()) // disabling result may erase the list (on undo of Part, issue 665)
      myResults.erase(myResults.begin());
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
    if (myIsDisabled) {
      removeResults(0, false); // flush will be in setCurrentFeature
    } else {
      // enable all disabled previously results
      std::list<std::shared_ptr<ModelAPI_Result> >::iterator aResIter = myResults.begin();
      for(; aResIter != myResults.end(); aResIter++) {
        (*aResIter)->setDisabled(*aResIter, false);
      }
    }
    return true;
  }
  return false;
}

bool ModelAPI_Feature::isDisabled()
{
  return myIsDisabled;
}

bool ModelAPI_Feature::setStable(const bool theFlag)
{
  if (myIsStable != theFlag) {
    myIsStable = theFlag;
    return true;
  }
  return false;
}

bool ModelAPI_Feature::isStable()
{
  return myIsStable;
}

bool ModelAPI_Feature::isPreviewNeeded() const
{
  return true;
}

void ModelAPI_Feature::init()
{
  myIsDisabled = false;
  myIsStable = true;
}
