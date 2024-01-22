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

#include <ModelAPI_AttributeBoolean.h>
#include <ModelAPI_AttributeDocRef.h>
#include <ModelAPI_AttributeDouble.h>
#include <ModelAPI_AttributeImage.h>
#include <ModelAPI_AttributeIntArray.h>
#include <ModelAPI_AttributeSelectionList.h>
#include <ModelAPI_CompositeFeature.h>
#include <ModelAPI_Document.h>
#include <ModelAPI_Events.h>
#include <ModelAPI_Object.h>
#include <ModelAPI_ResultBody.h>
#include <ModelAPI_ResultConstruction.h>
#include <ModelAPI_ResultGroup.h>
#include <ModelAPI_ResultParameter.h>
#include <ModelAPI_ResultPart.h>
#include <ModelAPI_Session.h>
#include "ModelAPI_Tools.h"
#include <ModelAPI_Validator.h>

#include <Config_Translator.h>
#include <Events_Loop.h>
#include <Locale_Convert.h>

#include <GeomAlgoAPI_MakeShape.h>
#include <GeomAPI_ShapeHierarchy.h>
#include <GeomAPI_ShapeIterator.h>
#include <GeomAPI_ShapeExplorer.h>

#include <algorithm>
#include <iostream>
#include <list>
#include <map>
#include <sstream>
#include <vector>
#include <array>

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

// LCOV_EXCL_START
const char* toString(ModelAPI_ExecState theExecState)
{
  switch (theExecState) {
  case ModelAPI_StateDone: return "Done";
  case ModelAPI_StateMustBeUpdated: return "Must be updated";
  case ModelAPI_StateExecFailed: return "Execution failed";
  case ModelAPI_StateInvalidArgument: return "Invalid argument";
  case ModelAPI_StateNothing: return "Empty state";
  default: return "Unknown ExecState.";
  }
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
          bool aHasSubError = false;
          for (int i = 0, aSize = aComposite->numberOfSubs(); i < aSize; i++) {
            FeaturePtr aSubFeature = aComposite->subFeature(i);
            std::string aSubFeatureError = getFeatureError(aSubFeature);
            if (!aSubFeatureError.empty()) {
              anError = anError + " in " + aSubFeature->getKind() + ".\n" + aSubFeatureError;
              aHasSubError = true;
              break;
            }
          }
          if (!aHasSubError) { // #24260: error not in the sub-features, but in the argument
            if (aComposite->getKind() == "Sketch" &&
                aComposite->selection("External")->isInvalid()) {
              std::string aMsg = "The sketch base plane is invalid, ";
              aMsg += "please push 'Change sketch plane' button to reselect it.";
              anError = Config_Translator::translate(aComposite->getKind(), aMsg);
            }
          }
        }
      }
    }
  }

  return anError;
}
// LCOV_EXCL_STOP

ObjectPtr objectByName(const DocumentPtr& theDocument, const std::string& theGroup,
                       const std::wstring& theName)
{
  for (int anIndex = 0; anIndex < theDocument->size(theGroup); ++anIndex) {
    ObjectPtr anObject = theDocument->object(theGroup, anIndex);
    if (anObject->data()->name() == theName)
      return anObject;
  }
  // not found
  return ObjectPtr();
}

//==================================================================================================
void loadModifiedShapes(ResultBodyPtr theResultBody,
                        const ListOfShape& theBaseShapes,
                        const ListOfShape& theTools,
                        const GeomMakeShapePtr& theMakeShape,
                        const GeomShapePtr theResultShape,
                        const std::string& theNamePrefix)
{
  theResultBody->storeModified(theBaseShapes, theResultShape, theMakeShape);

  ListOfShape aShapes = theBaseShapes;
  ListOfShape::const_iterator aToolIter = theTools.cbegin();
  for (; aToolIter != theTools.cend(); aToolIter++)
    aShapes.push_back(*aToolIter);

  for (ListOfShape::const_iterator anIter = aShapes.begin(); anIter != aShapes.end(); ++anIter)
  {
    theResultBody->loadModifiedShapes(theMakeShape, *anIter, GeomAPI_Shape::VERTEX, theNamePrefix);
    theResultBody->loadModifiedShapes(theMakeShape, *anIter, GeomAPI_Shape::EDGE, theNamePrefix);
    theResultBody->loadModifiedShapes(theMakeShape, *anIter, GeomAPI_Shape::FACE, theNamePrefix);
  }
}

//==================================================================================================
void loadModifiedShapes(ResultBodyPtr theResultBody,
                        const GeomShapePtr& theBaseShape,
                        const GeomMakeShapePtr& theMakeShape,
                        const std::string theName)
{
  switch (theBaseShape->shapeType()) {
  case GeomAPI_Shape::COMPOUND: {
    for (GeomAPI_ShapeIterator anIt(theBaseShape); anIt.more(); anIt.next())
    {
      loadModifiedShapes(theResultBody,
        anIt.current(),
        theMakeShape,
        theName);
    }
    break;
  }
  case GeomAPI_Shape::COMPSOLID:
  case GeomAPI_Shape::SOLID:
  case GeomAPI_Shape::SHELL: {
    theResultBody->loadModifiedShapes(theMakeShape,
      theBaseShape,
      GeomAPI_Shape::FACE,
      theName);
  }
  case GeomAPI_Shape::FACE:
  case GeomAPI_Shape::WIRE: {
    theResultBody->loadModifiedShapes(theMakeShape,
      theBaseShape,
      GeomAPI_Shape::EDGE,
      theName);
  }
  case GeomAPI_Shape::EDGE: {
    theResultBody->loadModifiedShapes(theMakeShape,
      theBaseShape,
      GeomAPI_Shape::VERTEX,
      theName);
  }
  default: // [to avoid compilation warning]
    break;
  }
}

//==================================================================================================
void loadDeletedShapes(ResultBodyPtr theResultBody,
                      const GeomShapePtr theBaseShape,
                      const ListOfShape& theTools,
                      const GeomMakeShapePtr& theMakeShape,
                      const GeomShapePtr theResultShapesCompound)
{
  ListOfShape aShapes = theTools;
  if (theBaseShape.get())
    aShapes.push_front(theBaseShape);

  for (ListOfShape::const_iterator anIter = aShapes.begin(); anIter != aShapes.end(); anIter++)
  {
    theResultBody->loadDeletedShapes(theMakeShape,
      *anIter,
      GeomAPI_Shape::VERTEX,
      theResultShapesCompound);
    theResultBody->loadDeletedShapes(theMakeShape,
      *anIter,
      GeomAPI_Shape::EDGE,
      theResultShapesCompound);
    theResultBody->loadDeletedShapes(theMakeShape,
      *anIter,
      GeomAPI_Shape::FACE,
      theResultShapesCompound);
    // store information about deleted solids because of unittest TestBooleanCommon_SolidsHistory
    // on OCCT 7.4.0 : common produces modified compsolid, so, move to the end for removed solids
    // starts to produce whole compsolid
    theResultBody->loadDeletedShapes(theMakeShape,
      *anIter,
      GeomAPI_Shape::SOLID,
      theResultShapesCompound);
  }
}

//==================================================================================================
void loadDeletedShapes(std::vector<ResultBaseAlgo>& theResultBaseAlgoList,
                        const ListOfShape& theTools,
                        const GeomShapePtr theResultShapesCompound)
{
  for (std::vector<ResultBaseAlgo>::iterator anIt = theResultBaseAlgoList.begin();
    anIt != theResultBaseAlgoList.end();
    ++anIt)
  {
    ResultBaseAlgo& aRCA = *anIt;
    loadDeletedShapes(aRCA.resultBody,
      aRCA.baseShape,
      theTools,
      aRCA.makeShape,
      theResultShapesCompound);
  }
}

bool findVariable(const DocumentPtr& theDocument, FeaturePtr theSearcher,
                  const std::wstring& theName, double& outValue, ResultParameterPtr& theParam)
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

bool findVariable(FeaturePtr theSearcher, const std::wstring& theName, double& outValue,
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

static void cacheSubresults(const ResultBodyPtr& theTopLevelResult,
                            std::set<ResultPtr>& theCashedResults)
{
  std::list<ResultPtr> aResults;
  ModelAPI_Tools::allSubs(theTopLevelResult, aResults, false);
  for (std::list<ResultPtr>::iterator aR = aResults.begin(); aR != aResults.end(); ++aR) {
    theCashedResults.insert(*aR);
  }
}

bool isInResults(AttributeSelectionListPtr theSelection,
                 const std::list<ResultPtr>& theResults,
                 std::set<ResultPtr>& theCashedResults)
{
  // collect all results into a cashed set
  if (theCashedResults.empty()) {
    std::list<ResultPtr>::const_iterator aRes = theResults.cbegin();
    for(; aRes != theResults.cend(); aRes++) {
      if (theCashedResults.count(*aRes))
        continue;
      else
        theCashedResults.insert(*aRes);

      if ((*aRes)->groupName() == ModelAPI_ResultBody::group()) {
        ResultBodyPtr aResBody = std::dynamic_pointer_cast<ModelAPI_ResultBody>(*aRes);
        cacheSubresults(aResBody, theCashedResults);
      } else if ((*aRes)->groupName() == ModelAPI_ResultPart::group()) { // all results of the part
        ResultPartPtr aResPart = std::dynamic_pointer_cast<ModelAPI_ResultPart>(*aRes);
        DocumentPtr aPartDoc = aResPart->partDoc();
        if (!aPartDoc.get() || !aPartDoc->isOpened()) { // document is not accessible
          return false;
        }
        int aBodyCount = aPartDoc->size(ModelAPI_ResultBody::group());
        for (int aBodyIndex = 0; aBodyIndex < aBodyCount; ++aBodyIndex) {
          ResultBodyPtr aResBody =
            std::dynamic_pointer_cast<ModelAPI_ResultBody>(
              aPartDoc->object(ModelAPI_ResultBody::group(), aBodyIndex));
          if (aResBody.get()) {
            theCashedResults.insert(aResBody);
            cacheSubresults(aResBody, theCashedResults);
          }
        }
      }
    }
  }
  // if context is in results, return true
  for(int a = 0; a < theSelection->size(); a++) {
    AttributeSelectionPtr anAttr = theSelection->value(a);
    ResultPtr aContext = anAttr->context();
    // check is it group selected for groups BOP
    if (aContext.get() && aContext->groupName() == ModelAPI_ResultGroup::group()) {
      // it is impossible by used results check which result is used in this group result,
      // so check the results shapes is it in results of this document or not
      FeaturePtr aSelFeature =
        std::dynamic_pointer_cast<ModelAPI_Feature>(theSelection->owner());
      if (!aSelFeature.get() || aSelFeature->results().empty())
        continue;
      GeomShapePtr aGroupResShape = aSelFeature->firstResult()->shape();

      std::set<ResultPtr>::iterator allResultsIter = theCashedResults.begin();
      for(; allResultsIter != theCashedResults.end(); allResultsIter++) {
        GeomShapePtr aResultShape = (*allResultsIter)->shape();

        GeomAPI_Shape::ShapeType aType =
          GeomAPI_Shape::shapeTypeByStr(theSelection->selectionType());
        GeomAPI_ShapeExplorer aGroupResExp(aGroupResShape, aType);
        for(; aGroupResExp.more(); aGroupResExp.next()) {
          if (aResultShape->isSubShape(aGroupResExp.current(), false))
            return true; // at least one shape of the group is in the used results
        }
      }
    }
    ResultBodyPtr aSelected = std::dynamic_pointer_cast<ModelAPI_ResultBody>(anAttr->context());
    if (!aSelected.get()) { // try to get selected feature and all its results
      FeaturePtr aContextFeature = anAttr->contextFeature();
      if (aContextFeature.get() && !aContextFeature->results().empty()) {
        const std::list<ResultPtr>& allResluts = aContextFeature->results();
        std::list<ResultPtr>::const_iterator aResIter = allResluts.cbegin();
        for(; aResIter != allResluts.cend(); aResIter++) {
          if (aResIter->get() && theCashedResults.count(*aResIter))
            return true;
        }
      }
    } else if (aSelected.get() && theCashedResults.count(aSelected))
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
  if (theFeature.get() && theFeature->data() && theFeature->data()->isValid()) {
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

ResultBodyPtr bodyOwner(const ResultPtr& theSub, const bool theRoot)
{
  if (theSub.get()) {
    ObjectPtr aParent = theSub->document()->parent(theSub);
    if (aParent.get()) {
      if (theRoot) { // try to find parent of parent
        ResultPtr aResultParent = std::dynamic_pointer_cast<ModelAPI_Result>(aParent);
        ResultBodyPtr aGrandParent = bodyOwner(aResultParent, true);
        if (aGrandParent.get())
          aParent = aGrandParent;
      }
      return std::dynamic_pointer_cast<ModelAPI_ResultBody>(aParent);
    }
  }
  return ResultBodyPtr(); // not found
}

int bodyIndex(const ResultPtr& theSub)
{
  int anIndex = -1;
  ResultBodyPtr aParent = bodyOwner(theSub);
  if (aParent.get()) {
    ResultBodyPtr aBody = std::dynamic_pointer_cast<ModelAPI_ResultBody>(theSub);
    if (aBody.get() && aParent->isSub(aBody, anIndex))
      return anIndex;
  }
  return anIndex; // not found
}

bool hasSubResults(const ResultPtr& theResult)
{
  ResultBodyPtr aCompSolid = std::dynamic_pointer_cast<ModelAPI_ResultBody>(theResult);
  return aCompSolid.get() && aCompSolid->numberOfSubs() > 0;
}

void allSubs(const ResultBodyPtr& theResult, std::list<ResultPtr>& theResults,
             const bool theLowerOnly) {
  // iterate sub-bodies of compsolid
  ResultBodyPtr aComp = std::dynamic_pointer_cast<ModelAPI_ResultBody>(theResult);
  if (aComp.get()) {
    int aNumSub = aComp->numberOfSubs();
    for (int a = 0; a < aNumSub; a++) {
      ResultBodyPtr aSub = aComp->subResult(a);
      if (!theLowerOnly || aSub->numberOfSubs() == 0)
        theResults.push_back(aSub);
      allSubs(aSub, theResults);
    }
  }
}

void allResults(const FeaturePtr& theFeature, std::list<ResultPtr>& theResults)
{
  if (!theFeature.get()) // safety: for empty feature no results
    return;
  const std::list<std::shared_ptr<ModelAPI_Result> >& aResults = theFeature->results();
  std::list<ResultPtr>::const_iterator aRIter = aResults.begin();
  for (; aRIter != aResults.cend(); aRIter++) {
    theResults.push_back(*aRIter);
    ResultBodyPtr aResult = std::dynamic_pointer_cast<ModelAPI_ResultBody>(*aRIter);
    allSubs(aResult, theResults);
  }
}

//******************************************************************
bool allDocumentsActivated(std::wstring& theNotActivatedNames)
{
  theNotActivatedNames = L"";
  bool anAllPartActivated = true;

  DocumentPtr aRootDoc = ModelAPI_Session::get()->moduleDocument();
  int aSize = aRootDoc->size(ModelAPI_ResultPart::group());
  for (int i = 0; i < aSize; i++) {
    ObjectPtr aObject = aRootDoc->object(ModelAPI_ResultPart::group(), i);
    ResultPartPtr aPart = std::dynamic_pointer_cast<ModelAPI_ResultPart>(aObject);
    if (!aPart->isActivated()) {
      anAllPartActivated = false;
      if (!theNotActivatedNames.empty())
        theNotActivatedNames += L", ";
      theNotActivatedNames += aObject->data()->name().c_str();
    }
  }
  return anAllPartActivated;
}

bool removeFeaturesAndReferences(const std::set<FeaturePtr>& theFeatures,
                                 const bool /*theFlushRedisplay*/,
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
                      int theRecLevel,
                      std::set<FeaturePtr>& theReferences)
{
  if (theRecLevel > RECURSE_TOP_LEVEL)
    return;
  theRecLevel++;

  if (theReferencesMap.find(theFeature) == theReferencesMap.end())
    return; // this feature is not in the selection list, so exists without references to it
  std::set<FeaturePtr> aMainReferences = theReferencesMap.at(theFeature);

  std::set<FeaturePtr>::const_iterator anIt = aMainReferences.begin(),
                                       aLast = aMainReferences.end();
  for (; anIt != aLast; anIt++) {
    FeaturePtr aRefFeature = *anIt;
    if (theReferences.find(aRefFeature) == theReferences.end()) {
      addRefsToFeature(aRefFeature, theReferencesMap, theRecLevel, theReferences);
      theReferences.insert(aRefFeature);
    }
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
        std::set<FeaturePtr>::const_iterator aRefIt = aSelRefFeatures.begin(),
                                             aRefLast = aSelRefFeatures.end();
        for (; aRefIt != aRefLast; aRefIt++) {
          FeaturePtr aCFeature = *aRefIt;
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
      aRecLevel = 0;
#ifdef DEBUG_REMOVE_FEATURES_RECURSE
      std::cout << " Ref: " << getFeatureInfo(aFeature) << std::endl;
#endif
      aRecLevel++;
      addRefsToFeature(aFeature, aMainList,
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

std::pair<std::wstring, bool> getDefaultName(const std::shared_ptr<ModelAPI_Result>& theResult,
                                             const bool theInherited,
                                             const bool theRecursive)
{
  typedef std::list< std::pair < std::string, std::list<ObjectPtr> > > ListOfReferences;

  SessionPtr aSession = ModelAPI_Session::get();

  ResultBodyPtr anOwnerRes = bodyOwner(theResult);
  if (anOwnerRes) {
    // names of sub-solids in CompSolid should be default (for example,
    // result of boolean operation 'Boolean_1_1' is a CompSolid which is renamed to 'MyBOOL',
    // however, sub-elements of 'MyBOOL' should be named 'Boolean_1_1_1', 'Boolean_1_1_2' etc.)
    std::wostringstream aDefaultName;
    aDefaultName << getDefaultName(anOwnerRes).first;
    aDefaultName << "_" << (bodyIndex(theResult) + 1);
    return std::pair<std::wstring, bool>(aDefaultName.str(), false);
  }

  FeaturePtr anOwner = ModelAPI_Feature::feature(theResult->data()->owner());
  DataPtr aData = anOwner->data();

  ListOfReferences aReferences;
  // find first result with user-defined name
  ListOfReferences::const_iterator aFoundRef = aReferences.end();
  if (theInherited) {
    aData->referencesToObjects(aReferences);

    for (ListOfReferences::const_iterator aRefIt = aReferences.begin();
         aRefIt != aReferences.end(); ++aRefIt) {
      bool isConcealed = aSession->validators()->isConcealed(anOwner->getKind(), aRefIt->first);
      bool isMainArg = isConcealed &&
                       aSession->validators()->isMainArgument(anOwner->getKind(), aRefIt->first);
      if (isConcealed) {
        // check the referred object is a Body
        // (for example, ExtrusionCut has a sketch as a first attribute which is concealing)
        bool isBody = aRefIt->second.size() > 1 || (aRefIt->second.size() == 1 &&
                      aRefIt->second.front().get() &&
                      aRefIt->second.front()->groupName() == ModelAPI_ResultBody::group());
        if (isBody && (isMainArg || aFoundRef == aReferences.end() ||
            aData->isPrecedingAttribute(aRefIt->first, aFoundRef->first)))
          aFoundRef = aRefIt;

        if (isMainArg)
          break;
      }
    }
  }
  // get the result number in the feature
  int anIndexInOwner = 0;
  const std::list<ResultPtr>& anOwnerResults = anOwner->results();
  std::list<ResultPtr>::const_iterator aResIt = anOwnerResults.cbegin();
  for(; aResIt != anOwnerResults.cend(); aResIt++) {
    if(*aResIt == theResult)
      break;
    anIndexInOwner++;
  }

  // find an object which is concealed by theResult
  if (aFoundRef != aReferences.end() && !aFoundRef->second.empty()) {
    // store number of references for each object
    std::map<ResultPtr, int> aNbRefToObject;
    // search the object by result index
    std::list<ObjectPtr>::const_iterator anObjIt = aFoundRef->second.begin();
    int aResultIndex = anIndexInOwner;
    while (--aResultIndex >= 0) {
      ResultPtr aCurRes = std::dynamic_pointer_cast<ModelAPI_Result>(*anObjIt);
      ResultBodyPtr aParentBody = ModelAPI_Tools::bodyOwner(aCurRes);
      if (aParentBody)
        aCurRes = aParentBody;
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
    if (anObjIt->get() && (*anObjIt)->groupName() == ModelAPI_ResultBody::group()) {
      // check the result is part of CompSolid
      ResultPtr anObjRes = std::dynamic_pointer_cast<ModelAPI_Result>(*anObjIt);
      ResultBodyPtr aParentBody = ModelAPI_Tools::bodyOwner(anObjRes);
      if (aParentBody)
        anObjRes = aParentBody;

      // return name of reference result only if it has been renamed by the user,
      // in other case compose a default name
      if (anObjRes->data()->hasUserDefinedName() ||
          (theRecursive && anObjRes->data()->name() != getDefaultName(anObjRes).first)) {
        std::wstringstream aName;
        aName << anObjRes->data()->name();
        std::map<ResultPtr, int>::iterator aFound = aNbRefToObject.find(anObjRes);
        if (aFound != aNbRefToObject.end()) {
          // to generate unique name, add suffix if there are several results
          // referring to the same shape
          aName << "_" << aFound->second + 1;
        }
        return std::pair<std::wstring, bool>(aName.str(), true);
      }
    }
  }

  // compose default name by the name of the feature and the index of result
  std::wstringstream aDefaultName;
  aDefaultName << anOwner->name();
  // if there are several results (issue #899: any number of result),
  // add unique prefix starting from second
  if (anIndexInOwner > 0 || theResult->groupName() == ModelAPI_ResultBody::group())
    aDefaultName << "_" << anIndexInOwner + 1;
  return std::pair<std::wstring, bool>(aDefaultName.str(), false);
}

std::set<FeaturePtr> getParents(const FeaturePtr& theFeature)
{
  std::set<FeaturePtr> aParents;
  for (FeaturePtr aCurFeat = theFeature; aCurFeat; ) {
    CompositeFeaturePtr aFoundComposite;
    const std::set<AttributePtr>& aRefs = aCurFeat->data()->refsToMe();
    for (std::set<AttributePtr>::const_iterator anIt = aRefs.begin();
      anIt != aRefs.end(); ++anIt) {
      FeaturePtr aF = ModelAPI_Feature::feature((*anIt)->owner());
      aFoundComposite = std::dynamic_pointer_cast<ModelAPI_CompositeFeature>(aF);
      if (aFoundComposite && aFoundComposite->isSub(aCurFeat))
        break;
      else
        aFoundComposite = CompositeFeaturePtr();
    }

    if (aFoundComposite) {
      aParents.insert(aFoundComposite);
      aCurFeat = aFoundComposite;
    }
    else {
      // add the part containing high-level feature
      SessionPtr aSession = ModelAPI_Session::get();
      DocumentPtr aPartSetDoc = aSession->moduleDocument();
      std::list<FeaturePtr> aPartSetFeatures = aPartSetDoc->allFeatures();
      for (std::list<FeaturePtr>::const_iterator anIt = aPartSetFeatures.begin();
        anIt != aPartSetFeatures.end(); ++anIt) {
        aFoundComposite = std::dynamic_pointer_cast<ModelAPI_CompositeFeature>(*anIt);
        if (aFoundComposite && aFoundComposite->isSub(aCurFeat)) {
          aParents.insert(aFoundComposite);
          break;
        }
      }

      aCurFeat = FeaturePtr();
    }
  }
  return aParents;
}

void fillShapeHierarchy(const GeomShapePtr& theShape,
                        const ResultPtr& theContext,
                        GeomAPI_ShapeHierarchy& theHierarchy)
{
  ResultBodyPtr aResCompSolidPtr = ModelAPI_Tools::bodyOwner(theContext);
  if (aResCompSolidPtr.get()) {
    std::shared_ptr<GeomAPI_Shape> aContextShape = aResCompSolidPtr->shape();
    if (aContextShape->shapeType() <= GeomAPI_Shape::COMPSOLID) {
      theHierarchy.addParent(theShape, aContextShape);
      fillShapeHierarchy(aContextShape, aResCompSolidPtr, theHierarchy);
    }
  }
}


void removeResults(const std::list<ResultPtr>& theResults)
{
  // collect all documents where the results must be removed
  std::map<DocumentPtr, std::list<ResultPtr> > aDocs;

  std::list<ResultPtr>::const_iterator aResIter = theResults.cbegin();
  for(; aResIter != theResults.cend(); aResIter++) {
    DocumentPtr aDoc = (*aResIter)->document();
    if (!aDocs.count(aDoc))
      aDocs[aDoc] = std::list<ResultPtr>();
    aDocs[aDoc].push_back(*aResIter);
  }
  // create a "remove" feature in each doc
  std::map<DocumentPtr, std::list<ResultPtr> >::iterator aDoc = aDocs.begin();
  for(; aDoc != aDocs.end(); aDoc++) {
    FeaturePtr aRemove = aDoc->first->addFeature("RemoveResults");
    if (aRemove) {
      for(aResIter = aDoc->second.cbegin(); aResIter != aDoc->second.cend(); aResIter++)
        aRemove->selectionList("results")->append(*aResIter, GeomShapePtr());
    }
  }
}

// used by GUI only
// LCOV_EXCL_START

//**************************************************************
void setDeflection(ResultPtr theResult, const double theDeflection)
{
  if (!theResult.get())
    return;

  AttributeDoublePtr aDeflectionAttr = theResult->data()->real(ModelAPI_Result::DEFLECTION_ID());
  if (aDeflectionAttr.get() != NULL) {
    aDeflectionAttr->setValue(theDeflection);
  }
}

double getDeflection(const std::shared_ptr<ModelAPI_Result>& theResult)
{
  double aDeflection = -1;
  // get deflection from the attribute of the result
  if (theResult.get() != NULL &&
    theResult->data()->attribute(ModelAPI_Result::DEFLECTION_ID()).get() != NULL) {
    AttributeDoublePtr aDoubleAttr = theResult->data()->real(ModelAPI_Result::DEFLECTION_ID());
    if (aDoubleAttr.get() && aDoubleAttr->isInitialized()) {
      double aValue = aDoubleAttr->value();
      if (aValue > 0) /// zero value should not be used as a deflection(previous studies)
        aDeflection = aDoubleAttr->value();
    }
  }
  return aDeflection;
}

//******************************************************
void setColor(ResultPtr theResult, const std::vector<int>& theColor)
{
  if (!theResult.get())
    return;

  AttributeIntArrayPtr aColorAttr = theResult->data()->intArray(ModelAPI_Result::COLOR_ID());
  if (aColorAttr.get() != NULL) {
    if (!aColorAttr->size()) {
      aColorAttr->setSize(3);
    }
    aColorAttr->setValue(0, theColor[0]);
    aColorAttr->setValue(1, theColor[1]);
    aColorAttr->setValue(2, theColor[2]);
  }
}

void getColor(const std::shared_ptr<ModelAPI_Result>& theResult, std::vector<int>& theColor)
{
  theColor.clear();
  // get color from the attribute of the result
  if (theResult.get() != NULL &&
    theResult->data()->attribute(ModelAPI_Result::COLOR_ID()).get() != NULL) {
    AttributeIntArrayPtr aColorAttr = theResult->data()->intArray(ModelAPI_Result::COLOR_ID());
    if (aColorAttr.get() && aColorAttr->size()) {
      theColor.push_back(aColorAttr->value(0));
      theColor.push_back(aColorAttr->value(1));
      theColor.push_back(aColorAttr->value(2));
    }
  }
}

//******************************************************
void getIsoLines(const std::shared_ptr<ModelAPI_Result>& theResult,
  bool& isVisible, std::vector<int>& theNbLines)
{
  theNbLines.clear();
  isVisible = false;
  if (!theResult.get())
    return;
  if (theResult->groupName() == ModelAPI_ResultConstruction::group()) {
    theNbLines.push_back(0);
    theNbLines.push_back(0);
  }
  else {
    // get color from the attribute of the result
    AttributeIntArrayPtr aAttr = theResult->data()->intArray(ModelAPI_Result::ISO_LINES_ID());
    if (aAttr.get()) {
      if (aAttr->size()) {
        theNbLines.push_back(aAttr->value(0));
        theNbLines.push_back(aAttr->value(1));
      }
    }
    AttributeBooleanPtr aBoolAttr =
      theResult->data()->boolean(ModelAPI_Result::SHOW_ISO_LINES_ID());
    if (aBoolAttr.get()) {
      isVisible = aBoolAttr->value();
    }
  }
}

//******************************************************
void setIsoLines(ResultPtr theResult, const std::vector<int>& theIso)
{
  if (!theResult.get())
    return;

  AttributeIntArrayPtr aAttr = theResult->data()->intArray(ModelAPI_Result::ISO_LINES_ID());
  if (aAttr.get() != NULL) {
    if (!aAttr->size()) {
      aAttr->setSize(2);
    }
    aAttr->setValue(0, theIso[0]);
    aAttr->setValue(1, theIso[1]);
  }
}

//******************************************************
void showIsoLines(std::shared_ptr<ModelAPI_Result> theResult, bool theShow)
{
  if (!theResult.get())
    return;

  AttributeBooleanPtr aAttr = theResult->data()->boolean(ModelAPI_Result::SHOW_ISO_LINES_ID());
  if (aAttr.get() != NULL) {
    aAttr->setValue(theShow);
  }
}

//******************************************************
bool isShownIsoLines(std::shared_ptr<ModelAPI_Result> theResult)
{
  if (!theResult.get())
    return false;

  AttributeBooleanPtr aAttr = theResult->data()->boolean(ModelAPI_Result::SHOW_ISO_LINES_ID());
  if (aAttr.get() != NULL) {
    return aAttr->value();
  }
  return false;
}

//******************************************************
void showEdgesDirection(std::shared_ptr<ModelAPI_Result> theResult, bool theShow)
{
  if (!theResult.get())
    return;

  AttributeBooleanPtr aAttr = theResult->data()->boolean(ModelAPI_Result::SHOW_EDGES_DIRECTION_ID());
  if (aAttr.get() != NULL) {
    aAttr->setValue(theShow);
  }
}

//******************************************************
bool isShowEdgesDirection(std::shared_ptr<ModelAPI_Result> theResult)
{
  if (!theResult.get())
    return false;

  AttributeBooleanPtr aAttr = theResult->data()->boolean(ModelAPI_Result::SHOW_EDGES_DIRECTION_ID());
  if (aAttr.get() != NULL) {
    return aAttr->value();
  }
  return false;
}

//******************************************************
void bringToFront(std::shared_ptr<ModelAPI_Result> theResult, bool theFlag)
{
  if (!theResult.get())
    return;

  AttributeBooleanPtr aAttr = theResult->data()->boolean(ModelAPI_Result::BRING_TO_FRONT_ID());
  if (aAttr.get() != NULL) {
    aAttr->setValue(theFlag);
  }
}

//******************************************************
bool isBringToFront(std::shared_ptr<ModelAPI_Result> theResult)
{
  if (!theResult.get())
    return false;

  AttributeBooleanPtr aAttr = theResult->data()->boolean(ModelAPI_Result::BRING_TO_FRONT_ID());
  if (aAttr.get() != NULL) {
    return aAttr->value();
  }
  return false;
}

//**************************************************************
void setTransparency(ResultPtr theResult, double theTransparency)
{
  if (!theResult.get())
    return;

  AttributeDoublePtr anAttribute = theResult->data()->real(ModelAPI_Result::TRANSPARENCY_ID());
  if (anAttribute.get() != NULL) {
    anAttribute->setValue(theTransparency);
  }
}

double getTransparency(const std::shared_ptr<ModelAPI_Result>& theResult)
{
  double aTransparency = -1;
  // get transparency from the attribute of the result
  if (theResult.get() != NULL &&
    theResult->data()->attribute(ModelAPI_Result::TRANSPARENCY_ID()).get() != NULL) {
    AttributeDoublePtr aDoubleAttr = theResult->data()->real(ModelAPI_Result::TRANSPARENCY_ID());
    if (aDoubleAttr.get() && aDoubleAttr->isInitialized()) {
      aTransparency = aDoubleAttr->value();
    }
  }
  return aTransparency;
}

void copyVisualizationAttrs(
  std::shared_ptr<ModelAPI_Result> theSource, std::shared_ptr<ModelAPI_Result> theDest)
{
  // color
  AttributeIntArrayPtr aSourceColor = theSource->data()->intArray(ModelAPI_Result::COLOR_ID());
  if (aSourceColor.get() && aSourceColor->isInitialized() && aSourceColor->size()) {
    AttributeIntArrayPtr aDestColor = theDest->data()->intArray(ModelAPI_Result::COLOR_ID());
    if (aDestColor.get()) {
      aDestColor->setSize(aSourceColor->size());
      for(int a = 0; a < aSourceColor->size(); a++)
        aDestColor->setValue(a, aSourceColor->value(a));
    }
  }
  // Iso-lines
  AttributeIntArrayPtr aSource = theSource->data()->intArray(ModelAPI_Result::ISO_LINES_ID());
  if (aSource.get() && aSource->isInitialized() && aSource->size()) {
    AttributeIntArrayPtr aDest = theDest->data()->intArray(ModelAPI_Result::ISO_LINES_ID());
    if (aDest.get()) {
      aDest->setSize(aSource->size());
      for(int a = 0; a < aSource->size(); a++)
        aDest->setValue(a, aSource->value(a));
    }
  }
  // deflection
  AttributeDoublePtr aSourceDefl = theSource->data()->real(ModelAPI_Result::DEFLECTION_ID());
  if (aSourceDefl.get() && aSourceDefl->isInitialized()) {
    AttributeDoublePtr aDestDefl = theDest->data()->real(ModelAPI_Result::DEFLECTION_ID());
    if (aDestDefl.get()) {
      aDestDefl->setValue(aSourceDefl->value());
    }
  }
  // transparency
  AttributeDoublePtr aSourceTransp = theSource->data()->real(ModelAPI_Result::TRANSPARENCY_ID());
  if (aSourceTransp.get() && aSourceTransp->isInitialized()) {
    AttributeDoublePtr aDestTransp = theDest->data()->real(ModelAPI_Result::TRANSPARENCY_ID());
    if (aDestTransp.get()) {
      aDestTransp->setValue(aSourceTransp->value());
    }
  }
}


void copyImageAttribute (std::shared_ptr<ModelAPI_Result> theSource,
                         std::shared_ptr<ModelAPI_Result> theDest)
{
  if (!theSource.get() || !theDest.get())
    return;

  // images allowed only for ResultBody
  ResultBodyPtr aSourceBody = std::dynamic_pointer_cast<ModelAPI_ResultBody>(theSource);
  ResultBodyPtr aDestBody   = std::dynamic_pointer_cast<ModelAPI_ResultBody>(theDest);
  if (!aSourceBody.get() || !aDestBody.get())
    return;

  AttributeImagePtr aSourceImage =
    theSource->data()->image(ModelAPI_ResultBody::IMAGE_ID());
  if (aSourceImage.get() && aSourceImage->hasTexture()) {
    AttributeImagePtr aDestImage =
      theDest->data()->image(ModelAPI_ResultBody::IMAGE_ID());
    if (aDestImage.get()) {
      aSourceImage->copyTo(aDestImage);
    }
  }
}

std::list<FeaturePtr> referencedFeatures(
  std::shared_ptr<ModelAPI_Result> theTarget, const std::string& theFeatureKind,
  const bool theSortResults)
{
  std::set<FeaturePtr> aResSet; // collect in the set initially to avoid duplicates
  std::list<ResultPtr> allSubRes;
  allSubRes.push_back(theTarget);
  ResultBodyPtr aBody = std::dynamic_pointer_cast<ModelAPI_ResultBody>(theTarget);
  if (aBody.get())
    allSubs(aBody, allSubRes);
  std::list<ResultPtr>::iterator aSub = allSubRes.begin();
  for(; aSub != allSubRes.end(); aSub++) {
    const std::set<AttributePtr>& aRefs = (*aSub)->data()->refsToMe();
    std::set<AttributePtr>::const_iterator aRef = aRefs.cbegin();
    for(; aRef != aRefs.cend(); aRef++) {
      FeaturePtr aFeat = std::dynamic_pointer_cast<ModelAPI_Feature>((*aRef)->owner());
      if (aFeat.get() && (theFeatureKind.empty() || aFeat->getKind() == theFeatureKind))
        aResSet.insert(aFeat);
    }
  }
  // add also feature of the target that may be referenced as a whole
  FeaturePtr aTargetFeature = theTarget->document()->feature(theTarget);
  const std::set<AttributePtr>& aRefs = aTargetFeature->data()->refsToMe();
  std::set<AttributePtr>::const_iterator aRef = aRefs.cbegin();
  for(; aRef != aRefs.cend(); aRef++) {
    FeaturePtr aFeat = std::dynamic_pointer_cast<ModelAPI_Feature>((*aRef)->owner());
    if (aFeat.get() && (theFeatureKind.empty() || aFeat->getKind() == theFeatureKind))
      aResSet.insert(aFeat);
  }
  // check also Group-operations that may refer to groups - add them for theFeatureKind "Group"
  if (theFeatureKind == "Group") {
    std::set<FeaturePtr> aGroupOperations;
    for(bool aNeedIterate = true; aNeedIterate; ) {
      std::set<FeaturePtr>::iterator aResIter = aResSet.begin();
      for(; aResIter != aResSet.end(); aResIter++) {
        std::list<ResultPtr>::const_iterator aGroupRes = (*aResIter)->results().cbegin();
        for(; aGroupRes != (*aResIter)->results().cend(); aGroupRes++) {
          const std::set<AttributePtr>& aGroupRefs = (*aGroupRes)->data()->refsToMe();
          std::set<AttributePtr>::const_iterator aRefIt = aGroupRefs.cbegin();
          for(; aRefIt != aGroupRefs.cend(); aRefIt++) {
            FeaturePtr aFeat = std::dynamic_pointer_cast<ModelAPI_Feature>((*aRefIt)->owner());
            if (aFeat.get() && !aGroupOperations.count(aFeat) && !aFeat->results().empty() &&
                aFeat->firstResult()->groupName() == ModelAPI_ResultGroup::group()) {
              // iterate results of this group operation because it may be without theTarget shape
              GeomShapePtr aTargetShape = theTarget->shape();
              bool anIsIn = false;
              std::list<ResultPtr>::const_iterator anOpRes = aFeat->results().cbegin();
              for(; anOpRes != aFeat->results().cend() && !anIsIn; anOpRes++) {
                GeomShapePtr anOpShape = (*anOpRes)->shape();
                if (!anOpShape.get() || anOpShape->isNull())
                  continue;
                for(GeomAPI_ShapeIterator aSubIt(anOpShape); aSubIt.more(); aSubIt.next()) {
                  if (aTargetShape->isSubShape(aSubIt.current(), false)) {
                    anIsIn = true;
                    break;
                  }
                }
              }
              if (anIsIn)
                aGroupOperations.insert(aFeat);
            }
          }
        }
      }
      // insert all new group operations into result and if they are, check for next dependencies
      aNeedIterate = false;
      std::set<FeaturePtr>::iterator aGroupOpIter = aGroupOperations.begin();
      for(; aGroupOpIter != aGroupOperations.end(); aGroupOpIter++) {
        if (aResSet.find(*aGroupOpIter) == aResSet.end()) {
          aResSet.insert(*aGroupOpIter);
          aNeedIterate = true;
        }
      }
    }
  }

  std::list<FeaturePtr> aResList;
  std::set<FeaturePtr>::iterator aResIter = aResSet.begin();
  for(; aResIter != aResSet.end(); aResIter++) {
    if (theSortResults) { // sort results by creation-order
      std::list<FeaturePtr>::iterator aListIter = aResList.begin();
      for(; aListIter != aResList.end(); aListIter++) {
        if ((*aResIter)->document()->isLater(*aListIter, *aResIter))
          break;
      }
      if (aListIter == aResList.end()) // goes to the end
        aResList.push_back(*aResIter);
      else
        aResList.insert(aListIter, *aResIter);
    } else //just push to the end unsorted
      aResList.push_back(*aResIter);
  }
  return aResList;
}

void setValues(std::vector<int>& theRGB, const int theRed, const int theGreen, const int theBlue)
{
  theRGB.push_back(theRed);
  theRGB.push_back(theGreen);
  theRGB.push_back(theBlue);
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
  int aV_int    = (int)(aV*aPercentToValue);
  int aVinc_int = (int)(aVinc*aPercentToValue);
  int aVmin_int = (int)(aVmin*aPercentToValue);
  int aVdec_int = (int)(aVdec*aPercentToValue);

  switch(aHi) {
    case 0: setValues(aRGB, aV_int,    aVinc_int, aVmin_int); break;
    case 1: setValues(aRGB, aVdec_int, aV_int,    aVmin_int); break;
    case 2: setValues(aRGB, aVmin_int, aV_int,    aVinc_int); break;
    case 3: setValues(aRGB, aVmin_int, aVdec_int, aV_int); break;
    case 4: setValues(aRGB, aVinc_int, aVmin_int, aV_int); break;
    case 5: setValues(aRGB, aV_int,    aVmin_int, aVdec_int); break;
    default: break;
  }
  return aRGB;
}

std::array<std::vector<int>, 10> myColorTab = {
  std::vector<int> {255, 0, 0},
  std::vector<int> {0, 255, 0},
  std::vector<int> {0, 0, 255},
  std::vector<int> {255, 255, 0},
  std::vector<int> {0, 255, 255},
  std::vector<int> {255, 0, 255},
  std::vector<int> {255, 94, 0},
  std::vector<int> {132, 255, 0},
  std::vector<int> {132, 0, 255},
  std::vector<int> {0, 0, 0},
};

void findRandomColor(std::vector<int>& theValues, bool theReset)
{
  static size_t i = 0;
  static std::vector<std::vector<int>> usedGeneratedColor;

  // True when disabling auto-color
  if ( theReset ) {
    i = 0;
    return;
  }

  theValues.clear();
  if (i < myColorTab.size()) {
    theValues = myColorTab[i++];
  } else {
      int timeout = 0;
      std::vector<int> aHSVColor;
      std::vector<int> aRGBColor;
      do {
        aHSVColor = {rand() % 360 , rand() % (100 - 50 + 1) + 50, rand() % (100 - 50 + 1) + 50};
        aRGBColor = HSVtoRGB(aHSVColor[0], aHSVColor[1], aHSVColor[2]);
        timeout++;
      } while (
        timeout < 20 &&
        std::find(usedGeneratedColor.begin(), usedGeneratedColor.end(), aHSVColor)
        != usedGeneratedColor.end() &&
        std::find(myColorTab.begin(), myColorTab.end(), aRGBColor) != myColorTab.end());
      usedGeneratedColor.push_back(aHSVColor);
      theValues = aRGBColor;
  }
}

// LCOV_EXCL_STOP

/// Returns name of the higher level feature (Part or feature of PartSet).
static FeaturePtr topOwner(const FeaturePtr& theFeature)
{
  FeaturePtr anOwner = theFeature;
  while (anOwner.get())
  {
    FeaturePtr aNextOwner = compositeOwner(anOwner);
    if (aNextOwner.get())
      anOwner = aNextOwner;
    else
      break;
  }
  if (anOwner->document() != ModelAPI_Session::get()->moduleDocument()) // the part-owner name
    anOwner = findPartFeature(ModelAPI_Session::get()->moduleDocument(), anOwner->document());
  return anOwner;
}

std::wstring validateMovement(const FeaturePtr& theAfter, const std::list<FeaturePtr> theMoved)
{
  std::wstring aResult;
  if (theMoved.empty())
    return aResult; // nothing to move, nothing to check, ok
  DocumentPtr aDoc = theAfter.get() ? theAfter->document() : (*(theMoved.cbegin()))->document();
  std::set<FeaturePtr> aMoved(theMoved.begin(), theMoved.end()); // fast access to moved
  std::set<FeaturePtr> aPassed, aPassedMoved; // all features and all moved before the current one
  std::set<FeaturePtr> aPassedAfter; // all passed features after theAfter
  bool anAfterIsPassed = theAfter.get() == 0; // flag that iterator already passed theAfter
  std::list<FeaturePtr> allFeat = aDoc->allFeatures();
  for (std::list<FeaturePtr>::iterator aFeat = allFeat.begin(); aFeat != allFeat.end(); aFeat++)
  {
    if (!anAfterIsPassed)
    {
      if (aMoved.count(*aFeat))
        aPassedMoved.insert(*aFeat);
      else // check aPassedMoved are not referenced by the current feature
        aPassed.insert(*aFeat);

      anAfterIsPassed = *aFeat == theAfter;
      if (anAfterIsPassed && !aPassedMoved.empty())
      { // check dependencies of moved relatively to the passed
        std::map<FeaturePtr, std::set<FeaturePtr> > aReferences;
        findAllReferences(aPassedMoved, aReferences);
        std::map<FeaturePtr, std::set<FeaturePtr> >::iterator aRefIter = aReferences.begin();
        for (; aRefIter != aReferences.end(); aRefIter++)
        {
          if (aPassed.count(aRefIter->first))
          {
            aResult += topOwner(aRefIter->first)->name() + L" -> ";
            // iterate all passed moved to check is it referenced by described feature or not
            std::set<FeaturePtr>::iterator aMovedIter = aPassedMoved.begin();
            for (; aMovedIter != aPassedMoved.end(); aMovedIter++)
            {
              std::map<FeaturePtr, std::set<FeaturePtr> > aPassedRefs;
              std::set<FeaturePtr> aMovedOne;
              aMovedOne.insert(*aMovedIter);
              findAllReferences(aMovedOne, aPassedRefs);
              if (aPassedRefs.count(aRefIter->first))
                aResult += topOwner(*aMovedIter)->name() + L" ";
            }
            aResult += L"\n";
          }
        }
      }
    }
    else // iteration after theAfter
    {
      if (aMoved.count(*aFeat)) { // check dependencies of moved relatively to ones after theAfter
        std::map<FeaturePtr, std::set<FeaturePtr> > aReferences;
        findAllReferences(aPassedAfter, aReferences);
        bool aFoundRef = aReferences.find(*aFeat) != aReferences.end();
        if (!aFoundRef && !(*aFeat)->results().empty()) // reference may be a feature in moved part
        {
          ResultPartPtr aFeatPart =
            std::dynamic_pointer_cast<ModelAPI_ResultPart>((*aFeat)->firstResult());
          if (aFeatPart.get() && aFeatPart->partDoc().get())
          {
            std::map<FeaturePtr, std::set<FeaturePtr> >::iterator aRef = aReferences.begin();
            for (; aRef != aReferences.end() && !aFoundRef; aRef++)
              aFoundRef = aRef->first->document() == aFeatPart->partDoc();
          }
        }

        if (aFoundRef)
        {
          aResult += topOwner(*aFeat)->name() + L" -> ";
          std::set<FeaturePtr> aReferencedCount; // to avoid duplicates in the displayed references
          // iterate all passed after theAfter to check refers it described feature or not
          FeaturePtr aFeatTop = topOwner(*aFeat);
          std::set<FeaturePtr>::iterator aPassedIter = aPassedAfter.begin();
          for (; aPassedIter != aPassedAfter.end(); aPassedIter++)
          {
            FeaturePtr aPassedTop = topOwner(*aPassedIter);
            if (aReferencedCount.count(aPassedTop))
              continue;
            std::map<FeaturePtr, std::set<FeaturePtr> > aPassedRefs;
            std::set<FeaturePtr> aPassedOne;
            aPassedOne.insert(*aPassedIter);
            findAllReferences(aPassedOne, aPassedRefs);
            std::map<FeaturePtr, std::set<FeaturePtr> >::iterator aPRIter = aPassedRefs.begin();
            for (; aPRIter != aPassedRefs.end(); aPRIter++)
            {
              FeaturePtr aPRTop = topOwner(aPRIter->first);
              if (aPRIter->first == *aFeat || aPRIter->first == aFeatTop ||
                  aPRTop == *aFeat || aPRTop == aFeatTop)
              {
                aResult += aPassedTop->name() + L" ";
                aReferencedCount.insert(aPassedTop);
                break;
              }
            }
          }
          aResult += L"\n";
        }
      }
      else {
        aPassedAfter.insert(*aFeat);
      }
    }

  }
  return aResult;
}

} // namespace ModelAPI_Tools
