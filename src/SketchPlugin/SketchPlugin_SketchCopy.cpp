// Copyright (C) 2020-2022  CEA/DEN, EDF R&D
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

#include <SketchPlugin_SketchCopy.h>
#include <SketchPlugin_Sketch.h>
#include <SketchPlugin_SketchEntity.h>
#include <SketchPlugin_Tools.h>

#include <ModelAPI_AttributeRefAttr.h>
#include <ModelAPI_AttributeRefAttrList.h>
#include <ModelAPI_AttributeReference.h>
#include <ModelAPI_AttributeRefList.h>
#include <ModelAPI_CompositeFeature.h>
#include <ModelAPI_ResultConstruction.h>

#include <GeomAlgoAPI_Copy.h>

#include <list>
#include <map>
#include <string>
#include <sstream>

class MapEntities;
static void copyAttribute(AttributePtr theOld, AttributePtr theNew, MapEntities& theMapOldNew);

/// Internal structure to keep relation between sub-elements of old and new sketch.
class MapEntities
{
public:
  void bind(FeaturePtr theOld, FeaturePtr theNew)
  {
    myObjects[theOld] = theNew;
    checkPostponed(theOld);

    std::list<ResultPtr> anOldResults = theOld->results();
    std::list<ResultPtr> aNewResults = theNew->results();
    for (std::list<ResultPtr>::iterator it1 = anOldResults.begin(), it2 = aNewResults.begin();
         it1 != anOldResults.end() && it2 != aNewResults.end(); ++it1, ++it2) {
      myObjects[*it1] = *it2;
      checkPostponed(*it1);
    }
  }

  void postpone(ObjectPtr theOldObject, AttributePtr theOldAttr, AttributePtr theNewAttr)
  {
    if (theOldObject)
      myPostponed[theOldObject][theOldAttr] = theNewAttr;
  }

  ObjectPtr find(ObjectPtr theOld)
  {
    auto aFound = myObjects.find(theOld);
    return aFound == myObjects.end() ? ObjectPtr() : aFound->second;
  }

  AttributePtr find(AttributePtr theOld)
  {
    FeaturePtr anOldOwner = ModelAPI_Feature::feature(theOld->owner());
    FeaturePtr aNewOwner = ModelAPI_Feature::feature(find(anOldOwner));
    return aNewOwner ? aNewOwner->attribute(theOld->id()) : AttributePtr();
  }

protected:
  void checkPostponed(ObjectPtr theOld)
  {
    auto aFound = myPostponed.find(theOld);
    if (aFound == myPostponed.end())
      return;
    for (auto it = aFound->second.begin(); it != aFound->second.end(); ++it)
      copyAttribute(it->first, it->second, *this);
    myPostponed.erase(aFound);
  }

private:
  std::map<ObjectPtr, ObjectPtr> myObjects;
  std::map<ObjectPtr, std::map<AttributePtr, AttributePtr> > myPostponed;
};


SketchPlugin_SketchCopy::SketchPlugin_SketchCopy() : ModelAPI_Feature()
{}

void SketchPlugin_SketchCopy::initAttributes()
{
  data()->addAttribute(BASE_ID(), ModelAPI_AttributeReference::typeId());
}

static void copyRefAttr(AttributeRefAttrPtr theOld,
                        AttributeRefAttrPtr theNew,
                        MapEntities& theMapOldNew)
{
  if (theOld->isObject()) {
    ObjectPtr aNew = theMapOldNew.find(theOld->object());
    if (aNew)
      theNew->setObject(aNew);
    else
      theMapOldNew.postpone(theOld->object(), theOld, theNew);
  }
  else {
    AttributePtr aNewAttr = theMapOldNew.find(theOld->attr());
    if (aNewAttr)
      theNew->setAttr(aNewAttr);
    else
      theMapOldNew.postpone(theOld->attr()->owner(), theOld, theNew);
  }
}

static void copyRefAttrList(AttributeRefAttrListPtr theOld,
                            AttributeRefAttrListPtr theNew,
                            MapEntities& theMapOldNew)
{
  // copy only if all referred objects/attribute are already transfered
  std::list<std::pair<ObjectPtr, AttributePtr> > aRefs = theOld->list();
  for (std::list<std::pair<ObjectPtr, AttributePtr> >::iterator anIt = aRefs.begin();
       anIt != aRefs.end(); ++anIt) {
    ObjectPtr aNewObj = anIt->first ? theMapOldNew.find(anIt->first) : ObjectPtr();
    AttributePtr aNewAttr = anIt->second ? theMapOldNew.find(anIt->second) : AttributePtr();
    if (aNewObj || aNewAttr)
      *anIt = std::pair<ObjectPtr, AttributePtr>(aNewObj, aNewAttr);
    else {
      if (anIt->first)
        theMapOldNew.postpone(anIt->first, theOld, theNew);
      else
        theMapOldNew.postpone(anIt->second->owner(), theOld, theNew);
      return;
    }
  }
  // update the RefAttrList
  theNew->clear();
  for (std::list<std::pair<ObjectPtr, AttributePtr> >::iterator anIt = aRefs.begin();
       anIt != aRefs.end(); ++anIt) {
    if (anIt->first)
      theNew->append(anIt->first);
    else
      theNew->append(anIt->second);
  }
}

static void copyReference(AttributeReferencePtr theOld,
                          AttributeReferencePtr theNew,
                          MapEntities& theMapOldNew)
{
  ObjectPtr aNew = theMapOldNew.find(theOld->value());
  if (aNew)
    theNew->setValue(aNew);
  else
    theMapOldNew.postpone(theOld->value(), theOld, theNew);
}

static void copyRefList(AttributeRefListPtr theOld,
                        AttributeRefListPtr theNew,
                        MapEntities& theMapOldNew)
{
  // copy only if all referred objects are already transfered
  std::list<ObjectPtr> aRefs = theOld->list();
  for (std::list<ObjectPtr>::iterator anIt = aRefs.begin(); anIt != aRefs.end(); ++anIt) {
    ObjectPtr aNew = theMapOldNew.find(*anIt);
    if (aNew)
      *anIt = aNew;
    else {
      theMapOldNew.postpone(*anIt, theOld, theNew);
      return;
    }
  }
  // update theRefList
  theNew->clear();
  for (std::list<ObjectPtr>::iterator anIt = aRefs.begin(); anIt != aRefs.end(); ++anIt)
    theNew->append(*anIt);
}

static void copySelection(AttributeSelectionPtr theOld,
                          AttributeSelectionPtr theNew)
{
  theNew->selectValue(theOld);
}

void copyAttribute(AttributePtr theOld, AttributePtr theNew, MapEntities& theMapOldNew)
{
  if (theNew->attributeType() == ModelAPI_AttributeRefAttr::typeId()) {
    copyRefAttr(std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(theOld),
                std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(theNew),
                theMapOldNew);
  }
  else if (theNew->attributeType() == ModelAPI_AttributeReference::typeId()) {
    copyReference(std::dynamic_pointer_cast<ModelAPI_AttributeReference>(theOld),
                  std::dynamic_pointer_cast<ModelAPI_AttributeReference>(theNew),
                  theMapOldNew);
  }
  else if (theNew->attributeType() == ModelAPI_AttributeRefAttrList::typeId()) {
    copyRefAttrList(std::dynamic_pointer_cast<ModelAPI_AttributeRefAttrList>(theOld),
                    std::dynamic_pointer_cast<ModelAPI_AttributeRefAttrList>(theNew),
                    theMapOldNew);
  }
  else if (theNew->attributeType() == ModelAPI_AttributeRefList::typeId()) {
    copyRefList(std::dynamic_pointer_cast<ModelAPI_AttributeRefList>(theOld),
                std::dynamic_pointer_cast<ModelAPI_AttributeRefList>(theNew),
                theMapOldNew);
  }
  else if (theNew->attributeType() == ModelAPI_AttributeSelection::typeId()) {
    copySelection(std::dynamic_pointer_cast<ModelAPI_AttributeSelection>(theOld),
                  std::dynamic_pointer_cast<ModelAPI_AttributeSelection>(theNew));
  }
}

static void renameByParent(FeaturePtr theOld, FeaturePtr theNew)
{
  AttributeReferencePtr anOldParentRef = theOld->reference(SketchPlugin_SketchEntity::PARENT_ID());
  if (!anOldParentRef || !anOldParentRef->isInitialized())
    return;

  AttributeReferencePtr aNewParentRef = theNew->reference(SketchPlugin_SketchEntity::PARENT_ID());

  std::wstring anOldName = anOldParentRef->value()->data()->name();
  std::wstring aNewName = aNewParentRef->value()->data()->name();

  // substitute name of old parent by the new one
  theNew->data()->setName(theOld->name());
  SketchPlugin_Tools::replaceInName(theNew, anOldName, aNewName);

  const std::list<ResultPtr>& anOldResults = theOld->results();
  const std::list<ResultPtr>& aNewResults = theNew->results();
  for (std::list<ResultPtr>::const_iterator it0 = anOldResults.begin(), it1 = aNewResults.begin();
       it0 != anOldResults.end() && it1 != aNewResults.end(); ++it0, ++it1) {
    (*it1)->data()->setName((*it0)->data()->name());
    SketchPlugin_Tools::replaceInName(*it1, anOldName, aNewName);
  }
}

static void copyFeature(const FeaturePtr theFeature,
                        std::shared_ptr<SketchPlugin_Sketch> theSketch,
                        MapEntities& theMapOldNew)
{
  if (!theFeature)
    return;

  // copy feature and its results
  FeaturePtr aNewFeature = theSketch->addFeature(theFeature->getKind());
  theFeature->data()->copyTo(aNewFeature->data());
  int aResultIndex = 0;
  for (std::list<ResultPtr>::const_iterator aRIt = theFeature->results().begin();
       aRIt != theFeature->results().end(); ++aRIt) {
    ResultConstructionPtr aResult =
        theSketch->document()->createConstruction(aNewFeature->data(), aResultIndex);

    GeomAlgoAPI_Copy aCopyAlgo((*aRIt)->shape());

    aResult->setShape(aCopyAlgo.shape());
    aResult->setIsInHistory((*aRIt)->isInHistory());
    aNewFeature->setResult(aResult, aResultIndex++);
  }
  theMapOldNew.bind(theFeature, aNewFeature);

  // update referred features and attributes
  bool aWasBlocked = aNewFeature->data()->blockSendAttributeUpdated(true, false);
  std::list<AttributePtr> anAttrs =
      aNewFeature->data()->attributes(std::string());
  for (std::list<AttributePtr>::iterator anIt = anAttrs.begin(); anIt != anAttrs.end(); ++anIt) {
    AttributePtr anOldAttr = theFeature->attribute((*anIt)->id());
    copyAttribute(anOldAttr, *anIt, theMapOldNew);
  }
  aNewFeature->data()->blockSendAttributeUpdated(aWasBlocked, false);

  // rename feature according to parent
  renameByParent(theFeature, aNewFeature);
}

static int index(const std::wstring& theName, const std::wstring& thePrefix)
{
  int anIndex = -1;
  if (theName.find(thePrefix) == 0) {
    anIndex = 0;
    if (theName[thePrefix.size()] == '_') {
      std::wstring anIndexStr = theName.substr(thePrefix.size() + 1);
      anIndex = std::stoi(anIndexStr);
    }
  }
  return anIndex;
}


void SketchPlugin_SketchCopy::execute()
{
  FeaturePtr aBaseSketchFeature = ModelAPI_Feature::feature(reference(BASE_ID())->value());
  CompositeFeaturePtr aBaseSketch =
      std::dynamic_pointer_cast<ModelAPI_CompositeFeature>(aBaseSketchFeature);
  if (!aBaseSketch) {
    setError("Error: a base feature is not a sketch.");
    return; // invalid case
  }

  FeaturePtr aNewSketchFeature = document()->addFeature(aBaseSketch->getKind());
  std::shared_ptr<SketchPlugin_Sketch> aNewSketch =
      std::dynamic_pointer_cast<SketchPlugin_Sketch>(aNewSketchFeature);
  // copy all attributes of sketch, then clear sub-features to be able to copy then one by one
  aBaseSketch->data()->copyTo(aNewSketch->data());
  aNewSketch->reflist(SketchPlugin_Sketch::FEATURES_ID())->clear();

  MapEntities aMapOldNew;

  std::list<ObjectPtr> aBaseFeatures =
      aBaseSketch->reflist(SketchPlugin_Sketch::FEATURES_ID())->list();
  for (std::list<ObjectPtr>::const_iterator aFIt = aBaseFeatures.begin();
       aFIt != aBaseFeatures.end(); ++aFIt) {
    FeaturePtr aCurFeature = ModelAPI_Feature::feature(*aFIt);
    copyFeature(aCurFeature, aNewSketch, aMapOldNew);
  }

  aNewSketch->execute();

  // check number of copies of the selected sketch before name the new sketch
  static const std::wstring SKETCH_NAME_SUFFIX(L"_Copy");
  std::wstring aSketchName = aBaseSketch->name() + SKETCH_NAME_SUFFIX;
  int aNewSketchIndex = 0;
  std::list<FeaturePtr> aFeatures = document()->allFeatures();
  for (std::list<FeaturePtr>::iterator aFIt = aFeatures.begin(); aFIt != aFeatures.end(); ++aFIt) {
    if ((*aFIt)->getKind() != SketchPlugin_Sketch::ID())
      continue;
    int anIndex = index((*aFIt)->name(), aSketchName);
    if (anIndex >= aNewSketchIndex)
      aNewSketchIndex = anIndex + 1;
    anIndex = index((*aFIt)->lastResult()->data()->name(), aSketchName);
    if (anIndex >= aNewSketchIndex)
      aNewSketchIndex = anIndex + 1;
  }
  std::wostringstream aNameStream;
  aNameStream << aSketchName;
  if (aNewSketchIndex > 0)
    aNameStream << '_' << aNewSketchIndex;
  aNewSketch->data()->setName(aNameStream.str());
  aNewSketch->lastResult()->data()->setName(aNameStream.str());
}
