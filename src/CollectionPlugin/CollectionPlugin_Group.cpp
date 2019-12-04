// Copyright (C) 2014-2019  CEA/DEN, EDF R&D
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

#include "CollectionPlugin_Group.h"

#include <ModelAPI_Data.h>
#include <ModelAPI_Document.h>
#include <ModelAPI_AttributeInteger.h>
#include <ModelAPI_AttributeString.h>
#include <ModelAPI_AttributeSelectionList.h>
#include <ModelAPI_AttributeIntArray.h>
#include <ModelAPI_ResultGroup.h>
#include <sstream>

CollectionPlugin_Group::CollectionPlugin_Group()
{
}

void CollectionPlugin_Group::initAttributes()
{
  AttributeSelectionListPtr aList = std::dynamic_pointer_cast<ModelAPI_AttributeSelectionList>(
    data()->addAttribute(LIST_ID(), ModelAPI_AttributeSelectionList::typeId()));
  aList->setWholeResultAllowed(true); // allow to select the whole result
}

void CollectionPlugin_Group::execute()
{
  if (results().empty() || firstResult()->isDisabled()) { // just create result if not exists
    ResultPtr aGroup = document()->createGroup(data());
    setResult(aGroup);
  }
}

// returns name with suffix, not existing in the existing set
static std::string findName(
  const std::string theOrigin, int& theSuffix, std::set<std::string>& theExisting)
{
  std::string aRes;
  do {
    std::ostringstream aName;
    aName<<theOrigin<<"_"<<theSuffix;
    aRes = aName.str();
    theSuffix++;
  } while(theExisting.count(aRes));
  theExisting.insert(aRes);
  return aRes;

}

bool CollectionPlugin_Group::customAction(const std::string& theActionId)
{
  if (theActionId == "split") {
    DocumentPtr aDoc = document();
    // collect all existing names of features to give unique names
    std::set<std::string> aFeatNames, aResNames;
    std::list<FeaturePtr> allFeat = aDoc->allFeatures();
    std::list<FeaturePtr>::iterator allFeatIter = allFeat.begin();
    for(; allFeatIter != allFeat.end(); allFeatIter++) {
      FeaturePtr aFeat = *allFeatIter;
      if (aFeat->data().get() && aFeat->data()->isValid()) {
        aFeatNames.insert(aFeat->name());
        if (aFeat->getKind() == ID() && aFeat->data().get() && aFeat->data()->isValid()) {
           std::list<ResultPtr>::const_iterator aRess = aFeat->results().cbegin();
           for(; aRess != aFeat->results().cend(); aRess++) {
             ResultPtr aRes = *aRess;
             if (aRes->data().get() && aRes->data()->isValid()) {
               aResNames.insert(aRes->data()->name());
             }
           }
        }
      }
    }

    AttributeSelectionListPtr aList = selectionList(LIST_ID());
    std::set<int> aRemoved;
    bool aStay = false; // to indicate that the good attribute found stays in the list
    int anIndex = 1; // index of the name assigned to group-feature and result
    // added in the order: 3 2 1 orig=0, so, keep the results to give names later
    std::list<ObjectPtr> aResults;
    for(int aNext = aList->size() - 1; aNext >= 0; aNext--) {
      AttributeSelectionPtr anOldAttr = aList->value(aNext);
      if (anOldAttr->isInvalid() || !anOldAttr->context().get()) {// remove invalids
        aRemoved.insert(aNext);
        continue;
      }
      if (!aStay) {
        aStay = true;
        continue;
      }
      aRemoved.insert(aNext);
      FeaturePtr aNew = aDoc->addFeature(ID(), false);
      AttributeSelectionListPtr aNewList = aNew->selectionList(LIST_ID());
      aNewList->setSelectionType(aList->selectionType());
      aNewList->append(anOldAttr->contextObject(), anOldAttr->value());
      aResults.push_front(aNew); // to keep the order
    }
    aResults.push_back(data()->owner());
    // remove all selections except the first
    aList->remove(aRemoved);
    // set names
    if (aResults.size() > 1) { // rename if there are new groups appeared only
      std::list<ObjectPtr>::iterator aResIter = aResults.begin();
      for(int aSuffix = 1; aResIter != aResults.end(); aResIter++) {
        FeaturePtr aFeat = std::dynamic_pointer_cast<ModelAPI_Feature>(*aResIter);
        aFeat->data()->setName(findName(name(), aSuffix, aFeatNames));
        if (!aFeat->results().empty() && !results().empty()) {
          int aResSuf = aSuffix - 1;
          std::string aResName = findName(firstResult()->data()->name(), aResSuf, aResNames);
          aFeat->firstResult()->data()->setName(aResName);
          // set the same color of result as in origin
          if (firstResult()->data()->attribute(ModelAPI_Result::COLOR_ID()).get()) {
            AttributeIntArrayPtr aSourceColor =
              firstResult()->data()->intArray(ModelAPI_Result::COLOR_ID());
            if (aSourceColor.get() && aSourceColor->size()) {
              AttributeIntArrayPtr aDestColor =
                aFeat->firstResult()->data()->intArray(ModelAPI_Result::COLOR_ID());
              aDestColor->setSize(aSourceColor->size());
              for(int a = 0; a < aSourceColor->size(); a++)
                aDestColor->setValue(a, aSourceColor->value(a));
            }
          }
        }
      }
      // remove also filters if split performed
      FiltersFeaturePtr aFilters = aList->filters();
      if (aFilters.get()) {
        std::list<std::string> aFiltersList = aFilters->filters();
        std::list<std::string>::iterator aFilterName = aFiltersList.begin();
        for(; aFilterName != aFiltersList.end(); aFilterName++) {
          aFilters->removeFilter(*aFilterName);
        }
      }
    }
  }
  return true;
}
