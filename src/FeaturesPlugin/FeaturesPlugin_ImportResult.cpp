// Copyright (C) 2017-2021  CEA/DEN, EDF R&D
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

#include "FeaturesPlugin_ImportResult.h"

#include <ModelAPI_AttributeSelectionList.h>
#include <ModelAPI_AttributeRefList.h>
#include <ModelAPI_ResultBody.h>
#include <ModelAPI_ResultGroup.h>
#include <ModelAPI_Session.h>
#include <ModelAPI_ResultPart.h>
#include <ModelAPI_Tools.h>
#include <Events_InfoMessage.h>
#include <GeomAPI_ShapeExplorer.h>

void FeaturesPlugin_ImportResult::initAttributes()
{
  data()->addAttribute(OBJECTS(), ModelAPI_AttributeSelectionList::typeId());

  AttributePtr aFeaturesAttribute =
    data()->addAttribute(FEATURES_ID(),
                         ModelAPI_AttributeRefList::typeId());
  aFeaturesAttribute->setIsArgument(false);

  ModelAPI_Session::get()->validators()->registerNotObligatory(
      getKind(), FEATURES_ID());
}

void FeaturesPlugin_ImportResult::execute()
{
  // Process groups/fields
  std::shared_ptr<ModelAPI_AttributeRefList> aRefListOfGroups =
      std::dynamic_pointer_cast<ModelAPI_AttributeRefList>(data()->attribute(FEATURES_ID()));

  // Remove previous groups/fields stored in RefList
  //std::list<ObjectPtr> anGroupList = aRefListOfGroups->list();
  //std::list<ObjectPtr>::iterator anGroupIt = anGroupList.begin();
  const std::list<ObjectPtr>& anGroupList = aRefListOfGroups->list();
  std::list<ObjectPtr>::const_iterator anGroupIt = anGroupList.begin();
  for (; anGroupIt != anGroupList.end(); ++anGroupIt) {
    std::shared_ptr<ModelAPI_Feature> aFeature = ModelAPI_Feature::feature(*anGroupIt);
    if (aFeature)
      document()->removeFeature(aFeature);
  }

  aRefListOfGroups->clear();

  std::set<ResultPtr> aGlobalResultsCashed; // cash to speed up searching in all results selected
  std::list<ResultGroupPtr> aGroups;

  std::list<ResultPtr> aResults;

  AttributeSelectionListPtr aList = selectionList(OBJECTS());
  int aResultIndex = 0;
  for (int aSelIndex = 0; aSelIndex < aList->size(); aSelIndex++)
  {
    AttributeSelectionPtr aSel = aList->value(aSelIndex);

    ResultPtr aContext = aSel->context();
    if (aContext.get())
      aResults.push_back (aContext);
    else
    {
      FeaturePtr aFeature = aSel->contextFeature();
      if (aFeature.get())
      {
        const std::list<ResultPtr>& aResList = aFeature->results();
        aResults.assign (aResList.begin(), aResList.end());
      }
    }
  }

  std::list<ResultPtr>::iterator aResIter = aResults.begin();
  for(; aResIter != aResults.end(); aResIter++)
  {
    GeomShapePtr aShape = (*aResIter)->shape();
    if (!aShape.get() || aShape->isNull())
      continue;

    std::shared_ptr<ModelAPI_ResultBody> aResultBody = document()->createBody(data(), aResultIndex);
    aResultBody->store(aShape);
    aResultBody->loadFirstLevel(aShape, "ImportResult");
    setResult(aResultBody, aResultIndex++);

    std::set<ResultPtr> allResultsCashed; // cash to speed up searching in all results selected
    std::list<ResultPtr> aLocalResults;
    aLocalResults.push_back (*aResIter);

    std::list<DocumentPtr> aDocuments; /// documents of Parts
    aDocuments.push_back ((*aResIter)->document());
    std::list<DocumentPtr>::iterator aDoc = aDocuments.begin();
    for(; aDoc != aDocuments.end(); aDoc++)
    {
      // groups
      int aGroupCount = (*aDoc)->size(ModelAPI_ResultGroup::group());
      for (int aGroupIndex = 0; aGroupIndex < aGroupCount; ++aGroupIndex)
      {
        ResultGroupPtr aResultGroup =
          std::dynamic_pointer_cast<ModelAPI_ResultGroup>((*aDoc)->object(ModelAPI_ResultGroup::group(), aGroupIndex));

        if (!aResultGroup.get() || !aResultGroup->shape().get())
          continue;

        FeaturePtr aGroupFeature = (*aDoc)->feature(aResultGroup);

        AttributeSelectionListPtr aSelectionList =
          aGroupFeature->selectionList("group_list");

        if (!ModelAPI_Tools::isInResults(aSelectionList,
                                         aLocalResults,
                                         allResultsCashed))// skip group not used in result
          continue;

        aGlobalResultsCashed.insert (allResultsCashed.begin(), allResultsCashed.end());

        //Check: may be this group already exists in the list
        bool anIsFound = false;
        std::list<ResultGroupPtr>::iterator anIter = aGroups.begin();
        for (; anIter != aGroups.end(); anIter++)
        {
          if (*anIter == aResultGroup)
          {
            anIsFound = true;
            break;
          }
        }
        if (!anIsFound)
          aGroups.push_back (aResultGroup);
      }
    }
  }

  std::list<ResultGroupPtr>::iterator anIter = aGroups.begin();
  for (; anIter != aGroups.end(); anIter++)
  {
    DocumentPtr aDoc = (*anIter)->document();

    FeaturePtr aGroupFeature = aDoc->feature(*anIter);

    AttributeSelectionListPtr aSelectionList =
      aGroupFeature->selectionList("group_list");

    std::shared_ptr<ModelAPI_Feature> aNewGroupFeature = addFeature("Group");
    aNewGroupFeature->data()->setName(aGroupFeature->name());

    AttributeSelectionListPtr aNewSelectionList = aNewGroupFeature->selectionList("group_list");
    aNewSelectionList->setSelectionType (aSelectionList->selectionType());
    GeomAPI_Shape::ShapeType aTypeOfShape = GeomAPI_Shape::shapeTypeByStr (aSelectionList->selectionType());

    for (int aLocalSelIndex = 0; aLocalSelIndex < aSelectionList->size(); aLocalSelIndex++) {

      AttributeSelectionPtr aLocalSel = aSelectionList->value(aLocalSelIndex);
      ResultPtr aLocalContext = aLocalSel->context();
      ResultGroupPtr aLocalGroup = std::dynamic_pointer_cast<ModelAPI_ResultGroup> (aLocalContext);
      if (aLocalGroup.get())
      {
        GeomShapePtr aLocalShape = aGroupFeature->firstResult()->shape();
        GeomAPI_ShapeExplorer anExplo (aLocalShape, aTypeOfShape);
        for (; anExplo.more(); anExplo.next())
        {
          GeomShapePtr anExploredShape = anExplo.current();
          std::set<ResultPtr>::iterator aResultIter = aGlobalResultsCashed.begin();
          for (; aResultIter != aGlobalResultsCashed.end(); aResultIter++)
          {
            GeomShapePtr aCashedShape = (*aResultIter)->shape();
            if (aCashedShape->isSubShape(anExploredShape))
              aNewSelectionList->append((*aResultIter), anExploredShape);
          }
        }
        break;
      }
      else
      {
        GeomShapePtr aLocalShape = aLocalSel->value();

        if (aLocalContext.get() && aGlobalResultsCashed.count(aLocalContext))
          aNewSelectionList->append(aLocalContext, aLocalShape);
      }
    }
  }

  removeResults(aResultIndex);
}

bool FeaturesPlugin_ValidatorImportResults::isValid(const AttributePtr& theAttribute,
  const std::list<std::string>&, Events_InfoMessage& theError) const
{
  AttributeSelectionListPtr aList =
    std::dynamic_pointer_cast<ModelAPI_AttributeSelectionList>(theAttribute);
  if (aList->size() == 0) {
    // LCOV_EXCL_START
    theError = "Please select sources.";
    return false;
    // LCOV_EXCL_STOP
  }
  // store documents in the Part-fesatures in order to check
  // the selection is located in the previous documents
  std::map<DocumentPtr, int> aDocIndices;
  DocumentPtr aRoot = ModelAPI_Session::get()->moduleDocument();
  int aNum = aRoot->size(ModelAPI_Feature::group());
  for (int a = 0; a < aNum; a++) {
    FeaturePtr aFeat = std::dynamic_pointer_cast<ModelAPI_Feature>(
      aRoot->object(ModelAPI_Feature::group(), a));
    if (aFeat.get() && aFeat->data() && aFeat->data()->isValid() && aFeat->getKind() == "Part" &&
      aFeat->results().size()) {
      ResultPartPtr aPart = std::dynamic_pointer_cast<ModelAPI_ResultPart>(aFeat->firstResult());
      if (aPart.get() && aPart->partDoc()) {
        aDocIndices[aPart->partDoc()] = a;
      }
    }
  }
  int aThisIndex = aDocIndices[aList->owner()->document()];
  for (int aSelIndex = 0; aSelIndex < aList->size(); aSelIndex++) {
    AttributeSelectionPtr aSel = aList->value(aSelIndex);
    ResultPtr aContext = aSel->context();
    if (!aContext.get()) {
      // LCOV_EXCL_START
      theError = "Empty context.";
      return false;
      // LCOV_EXCL_STOP
    }
    GeomShapePtr aShape = aSel->value();
    if (aShape.get() && !aShape->isNull() && !aShape->isSame(aContext->shape())) {
      // LCOV_EXCL_START
      theError = "Import results does not support selection of sub-shapes";
      return false;
      // LCOV_EXCL_STOP
    }
    ResultBodyPtr aBody = std::dynamic_pointer_cast<ModelAPI_ResultBody>(aContext);
    if (!aBody.get()) {
      // LCOV_EXCL_START
      theError = "Only results may be selected.";
      return false;
      // LCOV_EXCL_STOP
    }
    if (!aBody->shape()) {
      // LCOV_EXCL_START
      theError = "Result is empty.";
      return false;
      // LCOV_EXCL_STOP
    }
    int aBodyIndex = aDocIndices[aBody->document()];
    if (aBodyIndex >= aThisIndex) {
      theError = "The selected result must be located in earlier part.";
      return false;
    }
  }
  return true;
}

//============================================================================
std::shared_ptr<ModelAPI_Feature> FeaturesPlugin_ImportResult::addFeature(
    std::string theID)
{
  std::shared_ptr<ModelAPI_Feature> aNew = document()->addFeature(theID, false);
  if (aNew)
    data()->reflist(FEATURES_ID())->append(aNew);
  // set as current also after it becomes sub to set correctly enabled for other subs
  //document()->setCurrentFeature(aNew, false);
  return aNew;
}

//=================================================================================================
int FeaturesPlugin_ImportResult::numberOfSubs(bool /*forTree*/) const
{
  return data()->reflist(FEATURES_ID())->size(true);
}

//=================================================================================================
std::shared_ptr<ModelAPI_Feature> FeaturesPlugin_ImportResult::subFeature(const int theIndex,
                                                                          bool /*forTree*/)
{
  ObjectPtr anObj = data()->reflist(FEATURES_ID())->object(theIndex, false);
  FeaturePtr aRes = std::dynamic_pointer_cast<ModelAPI_Feature>(anObj);
  return aRes;
}

//=================================================================================================
int FeaturesPlugin_ImportResult::subFeatureId(const int theIndex) const
{
  std::shared_ptr<ModelAPI_AttributeRefList> aRefList = std::dynamic_pointer_cast<
      ModelAPI_AttributeRefList>(data()->attribute(FEATURES_ID()));
  std::list<ObjectPtr> aFeatures = aRefList->list();
  std::list<ObjectPtr>::const_iterator anIt = aFeatures.begin();
  int aResultIndex = 1; // number of the counted (created) features, started from 1
  int aFeatureIndex = -1; // number of the not-empty features in the list
  for (; anIt != aFeatures.end(); anIt++) {
    if (anIt->get())
      aFeatureIndex++;
    if (aFeatureIndex == theIndex)
      break;
    aResultIndex++;
  }
  return aResultIndex;
}

//=================================================================================================
bool FeaturesPlugin_ImportResult::isSub(ObjectPtr theObject) const
{
  // check is this feature of result
  FeaturePtr aFeature = ModelAPI_Feature::feature(theObject);
  if (aFeature)
    return data()->reflist(FEATURES_ID())->isInList(aFeature);
  return false;
}

//=================================================================================================
void FeaturesPlugin_ImportResult::removeFeature(
    std::shared_ptr<ModelAPI_Feature> theFeature)
{
  if (!data()->isValid())
    return;
  AttributeRefListPtr aList = reflist(FEATURES_ID());
  aList->remove(theFeature);
}
