// Copyright (C) 2014-2024  CEA, EDF
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

#include "ModelAPI_Feature.h"
#include <ModelAPI_Events.h>
#include <ModelAPI_Result.h>
#include <ModelAPI_Data.h>
#include <ModelAPI_Document.h>
#include <ModelAPI_Session.h>
#include <ModelAPI_Tools.h>
#include <ModelAPI_ResultGroup.h>
#include <Events_Loop.h>
#include <Config_Translator.h>
#include <Config_PropManager.h>

#include <Locale_Convert.h>

#include <PyInterp_Interp.h>
#include <Python.h>

void ModelAPI_Feature::setError(const std::string& theError,
                                bool isSend,
                                bool isTranslate)
{
  std::string anError = isTranslate ? Config_Translator::translate(getKind(), theError)
                                    : theError;
  data()->setError(anError, isSend);
}

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
  if (!isDisabled()) // disabled feature may be executed when it is added as not enabled (#2078)
    theResult->setDisabled(theResult, false);

    if (Config_PropManager::getAutoColorStatus()
                            && theResult->groupName() == ModelAPI_ResultGroup::group()) {
      std::vector<int> aColor;
      ModelAPI_Tools::findRandomColor(aColor);
      ModelAPI_Tools::setColor(theResult,  aColor);
    }
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

void ModelAPI_Feature::removeResults(
  const int theSinceIndex, const bool theForever, const bool theFlush)
{
  std::list<std::shared_ptr<ModelAPI_Result> >::iterator aResIter = myResults.begin();
  for(int anIndex = 0; anIndex < theSinceIndex && aResIter != myResults.end(); anIndex++)
    aResIter++;

  std::string aGroup;
  std::list<std::shared_ptr<ModelAPI_Result> >::iterator aNextIter = aResIter;
  while( aNextIter != myResults.end()) {
    aGroup = (*aNextIter)->groupName();
    // remove previously erased results: to enable later if needed only actual (of history change)
    (*aNextIter)->setDisabled(*aNextIter, true); // just disable results
    if (theForever) {
      aNextIter = myResults.erase(aNextIter);
    } else {
      aNextIter++;
    }
  }
  if (!aGroup.empty() && theFlush) {
    // flush visualization changes
    static Events_Loop* aLoop = Events_Loop::loop();
    static Events_ID aRedispEvent = aLoop->eventByName(EVENT_OBJECT_TO_REDISPLAY);
    aLoop->flush(aRedispEvent);
    static Events_ID aDelEvent = aLoop->eventByName(EVENT_OBJECT_DELETED);
    aLoop->flush(aDelEvent);
  }
}

void ModelAPI_Feature::eraseResults(const bool theForever)
{
  removeResults(0, theForever, true);
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

  while (!myResults.empty()) {  // remove one by one with messages
    std::shared_ptr<ModelAPI_Result> aRes = *(myResults.begin());
    aRes->setDisabled(aRes, true); // to avoid activation of the Part result
    if (!myResults.empty()) {// disabling result may erase the list (on undo of Part, issue 665)
      myResults.erase(myResults.begin());
      aRes->erase();
    }
  }
  ModelAPI_Object::erase();
}

ModelAPI_Feature::~ModelAPI_Feature()
{
  if (data() && data()->isValid())
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
      removeResults(0, false, false); // flush will be in setCurrentFeature
    } else {
      // enable all disabled previously results
      std::list<std::shared_ptr<ModelAPI_Result> >::iterator aResIter = myResults.begin();
      for(; aResIter != myResults.end(); aResIter++) {
        (*aResIter)->setDisabled(*aResIter, false);
      }
      // update selection for the case something was updated higher in the history
      // while this feature was disabled, but avoid flushing it immediately and
      // wait while all the previous features update myIsDisabled flag
      // (flush will be called by the document)
      static Events_Loop* aLoop = Events_Loop::loop();
      static Events_ID kUpdatedSel = aLoop->eventByName(EVENT_UPDATE_SELECTION);
      static const ModelAPI_EventCreator* aECreator = ModelAPI_EventCreator::get();
      aECreator->sendUpdated(data()->owner(), kUpdatedSel, true);
    }
    return true;
  }
  return false;
}

bool ModelAPI_Feature::isDisabled()
{
  return myIsDisabled;
}

// LCOV_EXCL_START
bool ModelAPI_Feature::setStable(const bool theFlag)
{
  if (myIsStable != theFlag) {
    myIsStable = theFlag;
    // send an event about the stability change (editing is started/finished)
    static Events_Loop* aLoop = Events_Loop::loop();
    static Events_ID EVENT_STAB = aLoop->eventByName(EVENT_STABILITY_CHANGED);
    std::shared_ptr<Events_Message> aMessage(new Events_Message(EVENT_STAB, this));
    aLoop->send(aMessage, false);
    return true;
  }
  return false;
}
// LCOV_EXCL_STOP

bool ModelAPI_Feature::isStable()
{
  return myIsStable;
}

bool ModelAPI_Feature::isEditable()
{
  return true;
}

bool ModelAPI_Feature::customAction(const std::string& /*theActionId*/)
{
  return false;
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

void ModelAPI_Feature::showErrorMessage()
{
  PyLockWrapper lck;
  std::string aResName{};
  if(data().get())
  {
    aResName = Locale::Convert::toString(data()->name());
  }
  std::string aMessage = "WARNING! The "+ aResName +" feature does not have any results.";
  PySys_WriteStdout("%s\n", aMessage.c_str());
}

ListOfShape ModelAPI_Feature::vertices(const bool theOnlyUnique)
{
  if(myResults.empty())
  {
    showErrorMessage();
    return ListOfShape();
  }
  return lastResult()->vertices(theOnlyUnique);
}

ListOfShape ModelAPI_Feature::edges(const bool theOnlyUnique)
{
  if(myResults.empty())
  {
    showErrorMessage();
    return ListOfShape();
  }
  return lastResult()->edges(theOnlyUnique);
}

ListOfShape ModelAPI_Feature::wires(const bool theOnlyUnique)
{
  if(myResults.empty())
  {
    showErrorMessage();
    return ListOfShape();
  }
  return lastResult()->wires(theOnlyUnique);
}

ListOfShape ModelAPI_Feature::faces(const bool theOnlyUnique)
{
  if(myResults.empty())
  {
    showErrorMessage();
    return ListOfShape();
  }
  return lastResult()->faces(theOnlyUnique);
}

ListOfShape ModelAPI_Feature::shells(const bool theOnlyUnique)
{
  if(myResults.empty())
  {
    showErrorMessage();
    return ListOfShape();
  }
  return lastResult()->shells(theOnlyUnique);
}

ListOfShape ModelAPI_Feature::solids(const bool theOnlyUnique)
{
  if(myResults.empty())
  {
    showErrorMessage();
    return ListOfShape();
  }
  return lastResult()->solids(theOnlyUnique);
}
