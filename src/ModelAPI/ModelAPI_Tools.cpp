// Copyright (C) 2014-2017  CEA/DEN, EDF R&D
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
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
//
// See http://www.salome-platform.org/ or
// email : webmaster.salome@opencascade.com<mailto:webmaster.salome@opencascade.com>
//

#include "ModelAPI_Tools.h"
#include <ModelAPI_Session.h>
#include <ModelAPI_CompositeFeature.h>
#include <ModelAPI_Document.h>
#include <ModelAPI_Object.h>
#include <ModelAPI_AttributeDouble.h>
#include <ModelAPI_ResultCompSolid.h>
#include <ModelAPI_ResultParameter.h>
#include <ModelAPI_ResultPart.h>
#include <ModelAPI_AttributeDocRef.h>
#include <ModelAPI_Validator.h>
#include <list>
#include <map>
#include <iostream>
#include <sstream>

#include <Events_Loop.h>
#include <ModelAPI_Events.h>

#define RECURSE_TOP_LEVEL 50

//#define DEBUG_REMOVE_FEATURES
//#define DEBUG_REMOVE_FEATURES_RECURSE
//#define DEBUG_CYCLING_1550

#ifdef DEBUG_REMOVE_FEATURES_RECURSE
#include <sstream>
std::string getFeatureInfo(FeaturePtr theFeature)
{
  if (!theFeature.get())
    return "";
  //std::ostringstream aPtrStr;
  //aPtrStr << "[" << theFeature.get() << "] ";
  std::string aFeatureInfo = /*aPtrStr.str() + */theFeature->name();
  CompositeFeaturePtr aComposite = ModelAPI_Tools::compositeOwner(theFeature);
  if (aComposite.get()) {
      aFeatureInfo = aFeatureInfo + "[in " + aComposite->name() + "]";
  }
  return aFeatureInfo;
}
#endif

#ifdef DEBUG_REMOVE_FEATURES
void printMapInfo(const std::map<FeaturePtr, std::set<FeaturePtr> >& theMainList,
                  const std::string& thePrefix)
{
  std::map<FeaturePtr, std::set<FeaturePtr> >::const_iterator aMainIt = theMainList.begin(),
                                                              aMainLast = theMainList.end();
  std::string anInfo;
  for (; aMainIt != aMainLast; aMainIt++) {
    FeaturePtr aMainListFeature = aMainIt->first;
    std::set<FeaturePtr> aMainRefList = aMainIt->second;
    std::set<FeaturePtr>::const_iterator anIt = aMainRefList.begin(), aLast = aMainRefList.end();
    std::string aRefsInfo;
    for (; anIt != aLast; anIt++) {
      aRefsInfo += (*anIt)->name().c_str();
      if (anIt != aLast)
        aRefsInfo += ", ";
    }
    if (!aRefsInfo.empty()) {
      anInfo = anInfo + aMainListFeature->name().c_str() + ": " + aRefsInfo + "\n";
    }
  }
  std::cout << thePrefix.c_str() << " [feature: references to]: \n" << anInfo.c_str() << std::endl;
}

void printListInfo(const std::set<FeaturePtr>& theMainList,
                  const std::string& thePrefix)
{
  std::set<FeaturePtr>::const_iterator aMainIt = theMainList.begin(),
                                       aMainLast = theMainList.end();
  std::string anInfo;
  for (; aMainIt != aMainLast; aMainIt++) {
    FeaturePtr aRefFeature = *aMainIt;
    anInfo += aRefFeature->name().c_str();
    if (aMainIt != aMainLast)
      anInfo += ", ";
  }
  std::cout << thePrefix.c_str() << ": " << anInfo.c_str() << std::endl;
}
#endif

namespace ModelAPI_Tools {

std::shared_ptr<GeomAPI_Shape> shape(const ResultPtr& theResult)
{
  return theResult->shape();
}

const char* toString(ModelAPI_ExecState theExecState)
{
#define TO_STRING(__NAME__) case __NAME__: return #__NAME__;
  switch (theExecState) {
  TO_STRING(ModelAPI_StateDone)
  TO_STRING(ModelAPI_StateMustBeUpdated)
  TO_STRING(ModelAPI_StateExecFailed)
  TO_STRING(ModelAPI_StateInvalidArgument)
  TO_STRING(ModelAPI_StateNothing)
  default: return "Unknown ExecState.";
  }
#undef TO_STRING
}

std::string getFeatureError(const FeaturePtr& theFeature)
{
  std::string anError;
  if (!theFeature.get() || !theFeature->data()->isValid() || theFeature->isAction())
    return anError;

  // to be removed later, this error should be got from the feature
  if (theFeature->data()->execState() == ModelAPI_StateDone ||
      theFeature->data()->execState() == ModelAPI_StateMustBeUpdated)
    return anError;

  // set error indication
  anError = theFeature->error();
  if (anError.empty()) {
    bool isDone = ( theFeature->data()->execState() == ModelAPI_StateDone
                 || theFeature->data()->execState() == ModelAPI_StateMustBeUpdated );
    if (!isDone) {
      anError = toString(theFeature->data()->execState());
      // If the feature is Composite and error is StateInvalidArgument,
      // error text should include error of first invalid sub-feature. Otherwise
      // it is not clear what is the reason of the invalid argument.
      if (theFeature->data()->execState() == ModelAPI_StateInvalidArgument) {
        CompositeFeaturePtr aComposite =
                    std::dynamic_pointer_cast<ModelAPI_CompositeFeature>(theFeature);
        if (aComposite) {
          for (int i = 0, aSize = aComposite->numberOfSubs(); i < aSize; i++) {
            FeaturePtr aSubFeature = aComposite->subFeature(i);
            std::string aSubFeatureError = getFeatureError(aSubFeature);
            if (!aSubFeatureError.empty()) {
              anError = anError + " in " + aSubFeature->getKind() + ".\n" + aSubFeatureError;
              break;
            }
          }
        }
      }
    }
  }

  return anError;
}

ObjectPtr objectByName(const DocumentPtr& theDocument, const std::string& theGroup,
                       const std::string& theName)
{
  for (int anIndex = 0; anIndex < theDocument->size(theGroup); ++anIndex) {
    ObjectPtr anObject = theDocument->object(theGroup, anIndex);
    if (anObject->data()->name() == theName)
      return anObject;
  }
  // not found
  return ObjectPtr();
}

bool findVariable(const DocumentPtr& theDocument, FeaturePtr theSearcher,
                  const std::string& theName, double& outValue, ResultParameterPtr& theParam)
{
  ObjectPtr aParamObj = objectByName(theDocument, ModelAPI_ResultParameter::group(), theName);
  theParam = std::dynamic_pointer_cast<ModelAPI_ResultParameter>(aParamObj);
  if (!theParam.get())
    return false;
  // avoid usage of parameters created later than the initial parameter

  if (theSearcher.get()) {
    FeaturePtr aParamFeat = theDocument->feature(theParam);
    if (aParamFeat == theSearcher || theDocument->isLater(aParamFeat, theSearcher))
      return false;
  }
  AttributeDoublePtr aValueAttribute = theParam->data()->real(ModelAPI_ResultParameter::VALUE());
  outValue = aValueAttribute->value();
  return true;
}

bool findVariable(FeaturePtr theSearcher, const std::string& theName, double& outValue,
                  ResultParameterPtr& theParam, const DocumentPtr& theDocument)
{
  SessionPtr aSession = ModelAPI_Session::get();
  std::list<DocumentPtr> aDocList;
  DocumentPtr aDocument = theDocument.get() ? theDocument : aSession->activeDocument();
  if (findVariable(aDocument, theSearcher, theName, outValue, theParam))
    return true;
  DocumentPtr aRootDocument = aSession->moduleDocument();
  if (aDocument != aRootDocument) {
    // any parameters in PartSet is okindependently on the Part position (issu #1504)
    if (findVariable(aRootDocument, FeaturePtr(), theName, outValue, theParam))
      return true;
  }
  return false;
}

ResultPtr findPartResult(const DocumentPtr& theMain, const DocumentPtr& theSub)
{
  // to optimize and avoid of crash on partset document close
  // (don't touch the sub-document structure)
  if (theMain != theSub) {
    for (int a = theMain->size(ModelAPI_ResultPart::group()) - 1; a >= 0; a--) {
      ResultPartPtr aPart = std::dynamic_pointer_cast<ModelAPI_ResultPart>(
          theMain->object(ModelAPI_ResultPart::group(), a));
      if (aPart && aPart->data()->document(ModelAPI_ResultPart::DOC_REF())->value() == theSub) {
        return aPart;
      }
    }
  }
  return ResultPtr();
}

FeaturePtr findPartFeature(const DocumentPtr& theMain, const DocumentPtr& theSub)
{
  // to optimize and avoid of crash on partset document close
  // (don't touch the sub-document structure)
  if (theMain != theSub) {
    // iteration from top to bottom to avoid finding the movement documents before the original
    int aSize = theMain->size(ModelAPI_Feature::group());
    for (int a = 0; a < aSize; a++) {
      FeaturePtr aPartFeat = std::dynamic_pointer_cast<ModelAPI_Feature>(
          theMain->object(ModelAPI_Feature::group(), a));
      if (aPartFeat.get()) {
        const std::list<std::shared_ptr<ModelAPI_Result> >& aResList = aPartFeat->results();
        std::list<std::shared_ptr<ModelAPI_Result> >::const_iterator aRes = aResList.begin();
        for(; aRes != aResList.end(); aRes++) {
          ResultPartPtr aPart = std::dynamic_pointer_cast<ModelAPI_ResultPart>(*aRes);
          if (aPart.get()) {
            if (aPart->isActivated() && aPart->partDoc() == theSub)
              return aPartFeat;
          } else break; // if the first is not Part, others are also not
        }
      }
    }
  }
  return FeaturePtr();
}

CompositeFeaturePtr compositeOwner(const FeaturePtr& theFeature)
{
  if (theFeature.get() && theFeature->data()->isValid()) {
    const std::set<std::shared_ptr<ModelAPI_Attribute> >& aRefs = theFeature->data()->refsToMe();
    std::set<std::shared_ptr<ModelAPI_Attribute> >::const_iterator aRefIter = aRefs.begin();
    for(; aRefIter != aRefs.end(); aRefIter++) {
      CompositeFeaturePtr aComp = std::dynamic_pointer_cast<ModelAPI_CompositeFeature>
        ((*aRefIter)->owner());
      if (aComp.get() && aComp->data()->isValid() && aComp->isSub(theFeature))
        return aComp;
    }
  }
  return CompositeFeaturePtr(); // not found
}

ResultCompSolidPtr compSolidOwner(const ResultPtr& theSub)
{
  int anIndex;
  ResultBodyPtr aBody = std::dynamic_pointer_cast<ModelAPI_ResultBody>(theSub);
  if (aBody.get()) {
    FeaturePtr aFeatureOwner = aBody->document()->feature(aBody);
    if (aFeatureOwner.get()) {
      std::list<std::shared_ptr<ModelAPI_Result> >::const_iterator aResIter =
        aFeatureOwner->results().cbegin();
      for(; aResIter != aFeatureOwner->results().cend(); aResIter++) {
        ResultCompSolidPtr aComp = std::dynamic_pointer_cast<ModelAPI_ResultCompSolid>(*aResIter);
        if (aComp && aComp->isSub(aBody, anIndex))
          return aComp;
      }
    }
  }
  return ResultCompSolidPtr(); // not found
}

int compSolidIndex(const ResultPtr& theSub)
{
  int anIndex = -1;
  ResultBodyPtr aBody = std::dynamic_pointer_cast<ModelAPI_ResultBody>(theSub);
  if (aBody.get()) {
    FeaturePtr aFeatureOwner = aBody->document()->feature(aBody);
    if (aFeatureOwner.get()) {
      std::list<std::shared_ptr<ModelAPI_Result> >::const_iterator aResIter =
        aFeatureOwner->results().cbegin();
      for(; aResIter != aFeatureOwner->results().cend(); aResIter++) {
        ResultCompSolidPtr aComp = std::dynamic_pointer_cast<ModelAPI_ResultCompSolid>(*aResIter);
        if (aComp && aComp->isSub(aBody, anIndex))
          return anIndex;
      }
    }
  }
  return anIndex; // not found
}

bool hasSubResults(const ResultPtr& theResult)
{
  ResultCompSolidPtr aCompSolid = std::dynamic_pointer_cast<ModelAPI_ResultCompSolid>(theResult);
  return aCompSolid.get() && aCompSolid->numberOfSubs() > 0;
}

void allResults(const FeaturePtr& theFeature, std::list<ResultPtr>& theResults)
{
  if (!theFeature.get()) // safety: for empty feature no results
    return;
  const std::list<std::shared_ptr<ModelAPI_Result> >& aResults = theFeature->results();
  std::list<std::shared_ptr<ModelAPI_Result> >::const_iterator aRIter = aResults.begin();
  for (; aRIter != aResults.cend(); aRIter++) {
    theResults.push_back(*aRIter);
    // iterate sub-bodies of compsolid
    ResultCompSolidPtr aComp = std::dynamic_pointer_cast<ModelAPI_ResultCompSolid>(*aRIter);
    if (aComp.get()) {
      int aNumSub = aComp->numberOfSubs();
      for(int a = 0; a < aNumSub; a++) {
        theResults.push_back(aComp->subResult(a));
      }
    }
  }
}

//******************************************************************
bool allDocumentsActivated(std::string& theNotActivatedNames)
{
  theNotActivatedNames = "";
  bool anAllPartActivated = true;

  DocumentPtr aRootDoc = ModelAPI_Session::get()->moduleDocument();
  int aSize = aRootDoc->size(ModelAPI_ResultPart::group());
  for (int i = 0; i < aSize; i++) {
    ObjectPtr aObject = aRootDoc->object(ModelAPI_ResultPart::group(), i);
    ResultPartPtr aPart = std::dynamic_pointer_cast<ModelAPI_ResultPart>(aObject);
    if (!aPart->isActivated()) {
      anAllPartActivated = false;
      if (!theNotActivatedNames.empty())
        theNotActivatedNames += ", ";
      theNotActivatedNames += aObject->data()->name().c_str();
    }
  }
  return anAllPartActivated;
}

bool removeFeaturesAndReferences(const std::set<FeaturePtr>& theFeatures,
                                 const bool theFlushRedisplay,
                                 const bool theUseComposite,
                                 const bool theUseRecursion)
{
#ifdef DEBUG_REMOVE_FEATURES
  printListInfo(theFeatures, "selection: ");
#endif

  std::map<FeaturePtr, std::set<FeaturePtr> > aReferences;
  ModelAPI_Tools::findAllReferences(theFeatures, aReferences, theUseComposite, theUseRecursion);
#ifdef DEBUG_REMOVE_FEATURES
  printMapInfo(aReferences, "allDependencies: ");
#endif

  std::set<FeaturePtr> aFeaturesRefsTo;
  ModelAPI_Tools::findRefsToFeatures(theFeatures, aReferences, aFeaturesRefsTo);
#ifdef DEBUG_REMOVE_FEATURES
  printListInfo(aFeaturesRefsTo, "references: ");
#endif

  std::set<FeaturePtr> aFeatures = theFeatures;
  if (!aFeaturesRefsTo.empty())
    aFeatures.insert(aFeaturesRefsTo.begin(), aFeaturesRefsTo.end());
#ifdef DEBUG_REMOVE_FEATURES
  printListInfo(aFeatures, "removeFeatures: ");
#endif

  return ModelAPI_Tools::removeFeatures(aFeatures, false);
}

//***********************************************************************
bool removeFeatures(const std::set<FeaturePtr>& theFeatures,
                    const bool theFlushRedisplay)
{
  bool isDone = false;
  std::set<FeaturePtr>::const_iterator anIt = theFeatures.begin(),
                                       aLast = theFeatures.end();
  for (; anIt != aLast; anIt++) {
    FeaturePtr aFeature = *anIt;
    if (aFeature.get()) {
      DocumentPtr aDoc = aFeature->document();
      // flush REDISPLAY signal after remove feature
      aDoc->removeFeature(aFeature);
      isDone = true;
    }
  }
  if (isDone && theFlushRedisplay) {
    // the redisplay signal should be flushed in order to erase
    // the feature presentation in the viewer
    // if should be done after removeFeature() of document
    Events_Loop::loop()->flush(Events_Loop::loop()->eventByName(EVENT_OBJECT_TO_REDISPLAY));
  }
  return true;
}

//***********************************************************************
// Fills the references list by all references of the feature from the references map.
// This is a recusive method to find references by next found feature in the map of references.
// \param theFeature a feature to find references
// \param theReferencesMap a map of references
// \param theReferences an out container of references
void addRefsToFeature(const FeaturePtr& theFeature,
                      const std::map<FeaturePtr, std::set<FeaturePtr> >& theReferencesMap,
                      std::map<FeaturePtr, std::set<FeaturePtr> >& theProcessedReferences,
                      int theRecLevel,
                      std::set<FeaturePtr>& theReferences)
{
  if (theRecLevel > RECURSE_TOP_LEVEL)
    return;
  theRecLevel++;

  // if the feature is already processed, get the ready references from the map
  if (theProcessedReferences.find(theFeature) != theProcessedReferences.end()) {
    std::set<FeaturePtr> aReferences = theProcessedReferences.at(theFeature);
    theReferences.insert(aReferences.begin(), aReferences.end());
    return;
  }

  if (theReferencesMap.find(theFeature) == theReferencesMap.end())
    return; // this feature is not in the selection list, so exists without references to it
  std::set<FeaturePtr> aMainReferences = theReferencesMap.at(theFeature);

  std::set<FeaturePtr>::const_iterator anIt = aMainReferences.begin(),
                                       aLast = aMainReferences.end();
#ifdef DEBUG_REMOVE_FEATURES_RECURSE
  std::string aSpacing;
  for (int i = 0; i < theRecLevel; i++)
    aSpacing.append(" ");
#endif

  for (; anIt != aLast; anIt++) {
    FeaturePtr aRefFeature = *anIt;
#ifdef DEBUG_REMOVE_FEATURES_RECURSE
  std::cout << aSpacing << " Ref: " << getFeatureInfo(aRefFeature) << std::endl;
#endif
    if (theReferences.find(aRefFeature) == theReferences.end())
      theReferences.insert(aRefFeature);
    addRefsToFeature(aRefFeature, theReferencesMap, theProcessedReferences,
                     theRecLevel, theReferences);
  }
}

// For each feature from the feature list it searches references to the feature and append them
// to the references map. This is a recusive method.
// \param theFeature a feature to find references
// \param theReferencesMap a map of references
// \param theReferences an out container of references
void findReferences(const std::set<FeaturePtr>& theFeatures,
                    std::map<FeaturePtr, std::set<FeaturePtr> >& theReferences,
                    const bool theUseComposite, const bool theUseRecursion, int theRecLevel)
{
  if (theRecLevel > RECURSE_TOP_LEVEL)
    return;
  theRecLevel++;
  std::set<FeaturePtr>::const_iterator anIt = theFeatures.begin(),
                                        aLast = theFeatures.end();
  for (; anIt != aLast; anIt++) {
    FeaturePtr aFeature = *anIt;
    if (aFeature.get() && theReferences.find(aFeature) == theReferences.end()) {
      DocumentPtr aSelFeatureDoc = aFeature->document();
      std::set<FeaturePtr> aSelRefFeatures;
      aSelFeatureDoc->refsToFeature(aFeature, aSelRefFeatures, false/*do not emit signals*/);
      if (theUseComposite) { // do not filter selection
        theReferences[aFeature] = aSelRefFeatures;
      }
      else { // filter references to skip composition features of the current feature
        std::set<FeaturePtr> aFilteredFeatures;
        std::set<FeaturePtr>::const_iterator anIt = aSelRefFeatures.begin(),
                                             aLast = aSelRefFeatures.end();
        for (; anIt != aLast; anIt++) {
          FeaturePtr aCFeature = *anIt;
          CompositeFeaturePtr aComposite =
            std::dynamic_pointer_cast<ModelAPI_CompositeFeature>(aCFeature);
          if (aComposite.get() && aComposite->isSub(aFeature))
            continue; /// composite of the current feature should be skipped
          aFilteredFeatures.insert(aCFeature);
        }
        theReferences[aFeature] = aFilteredFeatures;
      }
      if (theUseRecursion) {
#ifdef DEBUG_CYCLING_1550
        findReferences(aSelRefFeatures, theReferences, theUseComposite,
                       theUseRecursion, theRecLevel);
#else
        findReferences(theReferences[aFeature], theReferences, theUseComposite, theUseRecursion,
                       theRecLevel);
#endif
      }
    }
  }
}

void findAllReferences(const std::set<FeaturePtr>& theFeatures,
                       std::map<FeaturePtr, std::set<FeaturePtr> >& theReferences,
                       const bool theUseComposite,
                       const bool theUseRecursion)
{
  // For dependencies, find main_list:
  // sk_1(ext_1, vertex_1)
  // ext_1(bool_1, sk_3)
  // vertex_1()
  // sk_2(ext_2)
  // ext_2(bool_2)
  // sk_3()
  // Information: bool_1 is not selected, ext_2(bool_2) exists
  // find all referenced features
  std::map<FeaturePtr, std::set<FeaturePtr> > aMainList;
  int aRecLevel = 0;
  findReferences(theFeatures, aMainList, theUseComposite, theUseRecursion, aRecLevel);

#ifdef DEBUG_REMOVE_FEATURES
  printMapInfo(aMainList, "firstDependencies");
#endif
  // find all dependencies for each object:
  // sk_1(ext_1, vertex_1) + (sk_3, bool_1)
  // ext_1(bool_1, sk_3)
  // vertex_1()
  // sk_2(ext_2) + (bool_1)
  // ext_2(bool_1)
  // sk_3()
  std::map<FeaturePtr, std::set<FeaturePtr> >::const_iterator aMainIt = aMainList.begin(),
                                                              aMainLast = aMainList.end();
  for (; aMainIt != aMainLast; aMainIt++) {
    FeaturePtr aMainListFeature = aMainIt->first;

    //std::string aName = aMainListFeature->name();
    std::set<FeaturePtr> aMainRefList = aMainIt->second;

#ifdef DEBUG_REMOVE_FEATURES_RECURSE
    char aBuf[50];
    int n = sprintf(aBuf, "%d", aMainRefList.size());
    std::string aSize(aBuf);
    std::cout << "_findAllReferences for the Feature: " << getFeatureInfo(aMainListFeature)
              << ", references size = " << aSize << std::endl;
#endif
    std::set<FeaturePtr>::const_iterator anIt = aMainRefList.begin(),
                                         aLast = aMainRefList.end();
    std::set<FeaturePtr> aResultRefList;
    aResultRefList.insert(aMainRefList.begin(), aMainRefList.end());
    for (; anIt != aLast; anIt++) {
      FeaturePtr aFeature = *anIt;
      int aRecLevel = 0;
#ifdef DEBUG_REMOVE_FEATURES_RECURSE
      std::cout << " Ref: " << getFeatureInfo(aFeature) << std::endl;
#endif
      aRecLevel++;
      addRefsToFeature(aFeature, aMainList, theReferences,
                       aRecLevel, aResultRefList/*aMainRefList*/);
    }
    theReferences[aMainListFeature] = aResultRefList;
  }
#ifdef DEBUG_REMOVE_FEATURES_RECURSE
    std::cout << std::endl;
#endif

#ifdef DEBUG_REMOVE_FEATURES
  printMapInfo(theReferences, "allDependencies");
#endif
}

void findRefsToFeatures(const std::set<FeaturePtr>& theFeatures,
                        const std::map<FeaturePtr, std::set<FeaturePtr> >& theReferences,
                        std::set<FeaturePtr>& theFeaturesRefsTo)
{
  std::set<FeaturePtr>::const_iterator anIt = theFeatures.begin(),
                                       aLast = theFeatures.end();
  for (; anIt != aLast; anIt++) {
    FeaturePtr aFeature = *anIt;
    if (theReferences.find(aFeature) == theReferences.end())
      continue;
    std::set<FeaturePtr> aRefList = theReferences.at(aFeature);
    std::set<FeaturePtr>::const_iterator aRefIt = aRefList.begin(), aRefLast = aRefList.end();
    for (; aRefIt != aRefLast; aRefIt++) {
      FeaturePtr aRefFeature = *aRefIt;
      CompositeFeaturePtr aComposite =
        std::dynamic_pointer_cast<ModelAPI_CompositeFeature>(aRefFeature);
      if (aComposite.get() && aComposite->isSub(aFeature))
        continue; /// composite of the current feature should not be removed

      if (theFeatures.find(aRefFeature) == theFeatures.end() && // it is not selected
          theFeaturesRefsTo.find(aRefFeature) == theFeaturesRefsTo.end()) // it is not added
        theFeaturesRefsTo.insert(aRefFeature);
    }
  }
}

void getConcealedResults(const FeaturePtr& theFeature,
                         std::list<std::shared_ptr<ModelAPI_Result> >& theResults)
{
  SessionPtr aSession = ModelAPI_Session::get();

  std::list<std::pair<std::string, std::list<std::shared_ptr<ModelAPI_Object> > > > aRefs;
  theFeature->data()->referencesToObjects(aRefs);
  std::list<std::pair<std::string, std::list<ObjectPtr> > >::const_iterator
                                                  anIt = aRefs.begin(), aLast = aRefs.end();
  std::set<ResultPtr> alreadyThere; // to avoid duplications
  for (; anIt != aLast; anIt++) {
    if (!aSession->validators()->isConcealed(theFeature->getKind(), anIt->first))
      continue; // use only concealed attributes
    std::list<ObjectPtr> anObjects = (*anIt).second;
    std::list<ObjectPtr>::const_iterator anOIt = anObjects.begin(), anOLast = anObjects.end();
    for (; anOIt != anOLast; anOIt++) {
      ResultPtr aResult = std::dynamic_pointer_cast<ModelAPI_Result>(*anOIt);
      if (aResult && aResult->isConcealed()) {
        if (alreadyThere.find(aResult) == alreadyThere.end()) // issue 1712, avoid duplicates
          alreadyThere.insert(aResult);
        else continue;
        theResults.push_back(aResult);
      }
    }
  }
}

std::pair<std::string, bool> getDefaultName(
    const std::shared_ptr<ModelAPI_Result>& theResult,
    const int theResultIndex)
{
  typedef std::list< std::pair < std::string, std::list<ObjectPtr> > > ListOfReferences;

  SessionPtr aSession = ModelAPI_Session::get();
  FeaturePtr anOwner = ModelAPI_Feature::feature(theResult->data()->owner());

  ResultCompSolidPtr aCompSolidRes = compSolidOwner(theResult);
  if (aCompSolidRes) {
    // names of sub-solids in CompSolid should be default (for example,
    // result of boolean operation 'Boolean_1_1' is a CompSolid which is renamed to 'MyBOOL',
    // however, sub-elements of 'MyBOOL' should be named 'Boolean_1_1_1', 'Boolean_1_1_2' etc.)
    std::ostringstream aDefaultName;
    aDefaultName << anOwner->name();
    // compute default name of CompSolid (name of feature + index of CompSolid's result)
    int aCompSolidResultIndex = 0;
    const std::list<ResultPtr>& aResults = anOwner->results();
    for (std::list<ResultPtr>::const_iterator anIt = aResults.begin();
         anIt != aResults.end(); ++anIt, ++aCompSolidResultIndex)
      if (aCompSolidRes == *anIt)
        break;
    aDefaultName << "_" << (aCompSolidResultIndex + 1) << "_" << (theResultIndex + 1);
    return std::pair<std::string, bool>(aDefaultName.str(), false);
  }

  DataPtr aData = anOwner->data();

  ListOfReferences aReferences;
  aData->referencesToObjects(aReferences);

  // find first result with user-defined name
  ListOfReferences::const_iterator aFoundRef = aReferences.end();
  for (ListOfReferences::const_iterator aRefIt = aReferences.begin();
       aRefIt != aReferences.end(); ++aRefIt) {
    bool isConcealed = aSession->validators()->isConcealed(anOwner->getKind(), aRefIt->first);
    bool isMainArg = isConcealed &&
                     aSession->validators()->isMainArgument(anOwner->getKind(), aRefIt->first);
    if (isConcealed) {
      // check the referred object is a Body
      // (for example, ExtrusionCut has a sketch as a first attribute which is concealing)
      bool isBody = aRefIt->second.size() > 1 || (aRefIt->second.size() == 1 &&
                    aRefIt->second.front()->groupName() == ModelAPI_ResultBody::group());
      if (isBody && (isMainArg || aFoundRef == aReferences.end() ||
          aData->isPrecedingAttribute(aRefIt->first, aFoundRef->first)))
        aFoundRef = aRefIt;

      if (isMainArg)
        break;
    }
  }

  // find an object which is concealed by theResult
  if (aFoundRef != aReferences.end() && !aFoundRef->second.empty()) {
    // store number of references for each object
    std::map<ResultPtr, int> aNbRefToObject;
    // search the object by result index
    std::list<ObjectPtr>::const_iterator anObjIt = aFoundRef->second.begin();
    int aResultIndex = theResultIndex;
    while (--aResultIndex >= 0) {
      ResultPtr aCurRes = std::dynamic_pointer_cast<ModelAPI_Result>(*anObjIt);
      ResultCompSolidPtr aParentCompSolid = ModelAPI_Tools::compSolidOwner(aCurRes);
      if (aParentCompSolid)
        aCurRes = aParentCompSolid;
      if (aNbRefToObject.find(aCurRes) == aNbRefToObject.end())
        aNbRefToObject[aCurRes] = 1;
      else
        aNbRefToObject[aCurRes] += 1;

      ++anObjIt;
      if (anObjIt == aFoundRef->second.end()) {
        anObjIt = aFoundRef->second.begin();
        break;
      }
    }
    // check the result is a Body
    if ((*anObjIt)->groupName() == ModelAPI_ResultBody::group()) {
      // check the result is part of CompSolid
      ResultPtr anObjRes = std::dynamic_pointer_cast<ModelAPI_Result>(*anObjIt);
      ResultCompSolidPtr aParentCompSolid = ModelAPI_Tools::compSolidOwner(anObjRes);
      if (aParentCompSolid)
        anObjRes = aParentCompSolid;

      // return name of reference result only if it has been renamed by the user,
      // in other case compose a default name
      if (anObjRes->data()->hasUserDefinedName()) {
        std::stringstream aName;
        aName << anObjRes->data()->name();
        std::map<ResultPtr, int>::iterator aFound = aNbRefToObject.find(anObjRes);
        if (aFound != aNbRefToObject.end()) {
          // to generate unique name, add suffix if there are several results
          // referring to the same shape
          aName << "_" << aFound->second + 1;
        }
        return std::pair<std::string, bool>(aName.str(), true);
      }
    }
  }

  // compose default name by the name of the feature and the index of result
  std::stringstream aDefaultName;
  aDefaultName << anOwner->name();
  // if there are several results (issue #899: any number of result),
  // add unique prefix starting from second
  if (theResultIndex > 0 || theResult->groupName() == ModelAPI_ResultBody::group())
    aDefaultName << "_" << theResultIndex + 1;
  return std::pair<std::string, bool>(aDefaultName.str(), false);
}

} // namespace ModelAPI_Tools
