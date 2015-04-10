// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:    SketchSolver_FeatureStorage.cpp
// Created: 23 Mar 2015
// Author:  Artem ZHIDKOV

#include <SketchSolver_FeatureStorage.h>

#include <ModelAPI_AttributeRefAttr.h>
#include <ModelAPI_AttributeRefList.h>
#include <ModelAPI_ResultConstruction.h>

void SketchSolver_FeatureStorage::changeConstraint(ConstraintPtr theConstraint)
{
  std::list<AttributePtr> anAttributes = theConstraint->data()->attributes(std::string());
  std::list<AttributePtr>::iterator anIter = anAttributes.begin();
  for (; anIter != anAttributes.end(); anIter++) {
    AttributeRefAttrPtr aRefAttr = std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(*anIter);
    if (aRefAttr) {
      if (!aRefAttr->isObject()) {
        changeAttribute(aRefAttr->attr(), theConstraint);
        continue;
      }
      ResultConstructionPtr aRC = std::dynamic_pointer_cast<ModelAPI_ResultConstruction>(
          aRefAttr->object());
      FeaturePtr aFeature = aRC ? aRC->document()->feature(aRC) :
          std::dynamic_pointer_cast<ModelAPI_Feature>(aRefAttr->object());
      if (aFeature)
        changeFeature(aFeature, theConstraint);
      continue;
    }
    AttributeRefListPtr aRefList = std::dynamic_pointer_cast<ModelAPI_AttributeRefList>(*anIter);
    if (aRefList) {
      std::list<ObjectPtr> aList = aRefList->list();
      std::list<ObjectPtr>::iterator aListIter = aList.begin();
      for (; aListIter != aList.end(); aListIter++) {
        ResultConstructionPtr aRC = std::dynamic_pointer_cast<ModelAPI_ResultConstruction>(
            *aListIter);
        FeaturePtr aFeature = aRC ? aRC->document()->feature(aRC) :
            std::dynamic_pointer_cast<ModelAPI_Feature>(*aListIter);
        if (aFeature)
          changeFeature(aFeature, theConstraint);
      }
      continue;
    }
    changeAttribute(*anIter, theConstraint);
  }
  myConstraints.insert(theConstraint);
}

void SketchSolver_FeatureStorage::removeConstraint(ConstraintPtr theConstraint)
{
  DataPtr aData = theConstraint->data();
  if (aData) { // Constraint has data. Iterate through its attributes and remove them
    std::list<AttributePtr> anAttributes = aData->attributes(std::string());
    std::list<AttributePtr>::iterator anIter = anAttributes.begin();
    for (; anIter != anAttributes.end(); anIter++) {
      AttributeRefAttrPtr aRefAttr = std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(*anIter);
      if (aRefAttr) {
        if (!aRefAttr->isObject()) {
          removeAttribute(aRefAttr->attr(), theConstraint);
          continue;
        }
        ResultConstructionPtr aRC = std::dynamic_pointer_cast<ModelAPI_ResultConstruction>(
            aRefAttr->object());
        FeaturePtr aFeature = aRC ? aRC->document()->feature(aRC) :
            std::dynamic_pointer_cast<ModelAPI_Feature>(aRefAttr->object());
        if (aFeature)
          removeFeature(aFeature, theConstraint);
        continue;
      }
      AttributeRefListPtr aRefList = std::dynamic_pointer_cast<ModelAPI_AttributeRefList>(*anIter);
      if (aRefList) {
        std::list<ObjectPtr> aList = aRefList->list();
        std::list<ObjectPtr>::iterator aListIter = aList.begin();
        for (; aListIter != aList.end(); aListIter++) {
          ResultConstructionPtr aRC = std::dynamic_pointer_cast<ModelAPI_ResultConstruction>(
              *aListIter);
          FeaturePtr aFeature = aRC ? aRC->document()->feature(aRC) :
              std::dynamic_pointer_cast<ModelAPI_Feature>(*aListIter);
          if (aFeature)
            removeFeature(aFeature, theConstraint);
        }
        continue;
      }
      removeAttribute(*anIter, theConstraint);
    }
  } else { // Constraint has no data. Search the links on it in the lists of back references for features and attributes
    std::set<ConstraintPtr>::iterator aCIter;
    MapFeatureConstraint::iterator aFeatIter = myFeatures.begin();
    while (aFeatIter != myFeatures.end()) {
      aCIter = aFeatIter->second.find(theConstraint);
      if (aCIter != aFeatIter->second.end()) {
        FeaturePtr aFeature = aFeatIter->first;
        aFeatIter++;
        removeFeature(aFeature, theConstraint);
        continue;
      }
      aFeatIter++;
    }
    std::set<FeaturePtr>::iterator aFIter;
    MapAttributeFeature::iterator anAttrIter = myAttributes.begin();
    while (anAttrIter != myAttributes.end()) {
      aFIter = anAttrIter->second.find(theConstraint);
      if (aFIter != anAttrIter->second.end()) {
        anAttrIter->second.erase(aFIter);
        if (anAttrIter->second.empty()) {
          MapAttributeFeature::iterator aTmpIter = anAttrIter; // stores iterator for the next element, while the current is deleting
          aTmpIter++;
          myAttributes.erase(anAttrIter);
          anAttrIter = aTmpIter;
          continue;
        }
      }
      anAttrIter++;
    }
  }
  myConstraints.erase(theConstraint);
}

bool SketchSolver_FeatureStorage::isInteract(ConstraintPtr theConstraint) const
{
  if (myConstraints.empty() || myConstraints.find(theConstraint) != myConstraints.end())
    return true;

  DataPtr aData = theConstraint->data();
  if (!aData)
    return false;

  std::list<AttributePtr> anAttributes = aData->attributes(std::string());
  std::list<AttributePtr>::iterator anIter = anAttributes.begin();
  for (; anIter != anAttributes.end(); anIter++) {
    AttributeRefAttrPtr aRefAttr = std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(*anIter);
    if (aRefAttr) {
      if (!aRefAttr->isObject()) {
        if (isInteract(aRefAttr->attr()))
          return true;
        continue;
      }
      ResultConstructionPtr aRC = std::dynamic_pointer_cast<ModelAPI_ResultConstruction>(
          aRefAttr->object());
      FeaturePtr aFeature = aRC ? aRC->document()->feature(aRC) :
          std::dynamic_pointer_cast<ModelAPI_Feature>(aRefAttr->object());
      if (aFeature)
        if (isInteract(aFeature))
          return true;
      continue;
    }
    AttributeRefListPtr aRefList = std::dynamic_pointer_cast<ModelAPI_AttributeRefList>(*anIter);
    if (aRefList) {
      std::list<ObjectPtr> aList = aRefList->list();
      std::list<ObjectPtr>::iterator aListIter = aList.begin();
      for (; aListIter != aList.end(); aListIter++) {
        ResultConstructionPtr aRC = std::dynamic_pointer_cast<ModelAPI_ResultConstruction>(
            *aListIter);
        FeaturePtr aFeature = aRC ? aRC->document()->feature(aRC) :
            std::dynamic_pointer_cast<ModelAPI_Feature>(*aListIter);
        if (aFeature)
          if (isInteract(aFeature))
            return true;
      }
      continue;
    }
    if (isInteract(*anIter))
      return true;
  }
  return false;
}


void SketchSolver_FeatureStorage::changeFeature(FeaturePtr theFeature)
{
  std::list<AttributePtr> anAttributes = theFeature->data()->attributes(std::string());
  std::list<AttributePtr>::iterator anIter = anAttributes.begin();
  for (; anIter != anAttributes.end(); anIter++) {
    AttributeRefAttrPtr aRefAttr = std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(*anIter);
    if (aRefAttr) {
      if (!aRefAttr->isObject())
        changeAttribute(aRefAttr->attr(), theFeature);
      continue;
    }
    changeAttribute(*anIter, theFeature);
  }
  if (myFeatures.find(theFeature) == myFeatures.end())
    myFeatures[theFeature] = std::set<ConstraintPtr>();
}

void SketchSolver_FeatureStorage::changeFeature(FeaturePtr theFeature, ConstraintPtr theConstraint)
{
  // Change all attributes of the feature
  changeFeature(theFeature);
  // Add back reference feature to constraint
  myFeatures[theFeature].insert(theConstraint);
}

void SketchSolver_FeatureStorage::removeFeature(FeaturePtr theFeature)
{
  MapFeatureConstraint::iterator aFeatIter = myFeatures.find(theFeature);
  if (aFeatIter == myFeatures.end())
    return; // no such feature

  std::set<ConstraintPtr> aConstraints = aFeatIter->second;
  std::set<ConstraintPtr>::iterator aCIter = aConstraints.begin();
  for (; aCIter != aConstraints.end(); aCIter++)
    removeFeature(theFeature, *aCIter);
}

void SketchSolver_FeatureStorage::removeFeature(FeaturePtr theFeature, ConstraintPtr theConstraint)
{
  MapFeatureConstraint::iterator aFeatIter = myFeatures.find(theFeature);
  if (aFeatIter == myFeatures.end())
    return; // no such feature

  if (theFeature->data()) {
    std::list<AttributePtr> anAttributes = theFeature->data()->attributes(std::string());
    std::list<AttributePtr>::iterator anIter = anAttributes.begin();
    for (; anIter != anAttributes.end(); anIter++) {
      AttributeRefAttrPtr aRefAttr = std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(*anIter);
      if (aRefAttr) {
        if (!aRefAttr->isObject())
          removeAttribute(aRefAttr->attr(), theFeature);
        continue;
      }
      removeAttribute(*anIter, theFeature);
    }
  } else {
    // iterate on attributes to find items refered to theFeature
    MapAttributeFeature::iterator anIter = myAttributes.begin();
    while (anIter != myAttributes.end()) {
      if (anIter->second.find(theFeature) != anIter->second.end()) {
        anIter->second.erase(theFeature);
        if (anIter->second.empty()) {
          MapAttributeFeature::iterator aDeadIter = anIter++;
          myAttributes.erase(aDeadIter);
          continue;
        }
      }
      anIter++;
    }
  }

  aFeatIter->second.erase(theConstraint);
  if (aFeatIter->second.empty())
    myFeatures.erase(aFeatIter);
}

bool SketchSolver_FeatureStorage::isInteract(FeaturePtr theFeature) const
{
  if (myFeatures.find(theFeature) != myFeatures.end())
    return true;
  if (!theFeature->data() || !theFeature->data()->isValid())
    return false;

  std::list<AttributePtr> anAttributes = theFeature->data()->attributes(std::string());
  std::list<AttributePtr>::iterator anIter = anAttributes.begin();
  for (; anIter != anAttributes.end(); anIter++) {
    AttributeRefAttrPtr aRefAttr = std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(*anIter);
    if (aRefAttr) {
      if (!aRefAttr->isObject())
        if (isInteract(aRefAttr->attr()))
          return true;
      continue;
    }
    if (isInteract(*anIter))
      return true;
  }
  return false;
}


void SketchSolver_FeatureStorage::changeAttribute(AttributePtr theAttribute)
{
  if (myAttributes.find(theAttribute) == myAttributes.end())
    myAttributes[theAttribute] = std::set<FeaturePtr>();
}

void SketchSolver_FeatureStorage::changeAttribute(AttributePtr theAttribute, FeaturePtr theFeature)
{
  MapAttributeFeature::iterator anAttrIter = myAttributes.find(theAttribute);
  if (anAttrIter == myAttributes.end()) {
    std::set<FeaturePtr> aFeatures;
    aFeatures.insert(theFeature);
    myAttributes[theAttribute] = aFeatures;
    return;
  }
  anAttrIter->second.insert(theFeature);
}

void SketchSolver_FeatureStorage::removeAttribute(AttributePtr theAttribute)
{
  MapAttributeFeature::iterator anAttrIter = myAttributes.find(theAttribute);
  if (anAttrIter == myAttributes.end())
    return;

  std::set<FeaturePtr> aFeatures = anAttrIter->second;
  std::set<FeaturePtr>::iterator aFeatIter = aFeatures.begin();
  for (; aFeatIter != aFeatures.end(); aFeatIter++)
    removeAttribute(theAttribute, *aFeatIter);
}

void SketchSolver_FeatureStorage::removeAttribute(AttributePtr theAttribute, FeaturePtr theFeature)
{
  MapAttributeFeature::iterator anAttrIter = myAttributes.find(theAttribute);
  if (anAttrIter == myAttributes.end())
    return; // no such attribute

  anAttrIter->second.erase(theFeature);
  if (anAttrIter->second.empty())
    myAttributes.erase(anAttrIter);
}

bool SketchSolver_FeatureStorage::isInteract(AttributePtr theAttribute) const
{
  return myAttributes.find(theAttribute) != myAttributes.end();
}


bool SketchSolver_FeatureStorage::isConsistent() const
{
  // Check the constraints are valid
  std::set<ConstraintPtr>::const_iterator aCIter = myConstraints.begin();
  for (; aCIter != myConstraints.end(); aCIter++)
    if (!(*aCIter)->data() || !(*aCIter)->data()->isValid())
      return false;
  // Check the features are valid
  MapFeatureConstraint::const_iterator aFIter = myFeatures.begin();
  for (; aFIter != myFeatures.end(); aFIter++)
    if (!aFIter->first->data() || !aFIter->first->data()->isValid())
      return false;
  return true;
}

std::set<ConstraintPtr> SketchSolver_FeatureStorage::getConstraints(FeaturePtr theFeature) const
{
  std::set<ConstraintPtr> aResult;
  MapFeatureConstraint::const_iterator aFeatIter = myFeatures.find(theFeature);
  if (aFeatIter != myFeatures.end())
    aResult.insert(aFeatIter->second.begin(), aFeatIter->second.end());

  std::list<AttributePtr> anAttributes = theFeature->data()->attributes(std::string());
  std::list<AttributePtr>::const_iterator anAttrIter = anAttributes.begin();
  for (; anAttrIter != anAttributes.end(); anAttrIter++) {
    MapAttributeFeature::const_iterator anIt = myAttributes.find(*anAttrIter);
    if (anIt == myAttributes.end())
      continue;
    std::set<FeaturePtr>::const_iterator aFIter = anIt->second.begin();
    for (; aFIter != anIt->second.end(); aFIter++) {
      aFeatIter = myFeatures.find(*aFIter);
      if (aFeatIter != myFeatures.end())
        aResult.insert(aFeatIter->second.begin(), aFeatIter->second.end());
      else {
        ConstraintPtr aConstraint = std::dynamic_pointer_cast<SketchPlugin_Constraint>(*aFIter);
        if (aConstraint)
          aResult.insert(aConstraint);
      }
    }
  }
  return aResult;
}

std::set<ConstraintPtr> SketchSolver_FeatureStorage::getConstraints(AttributePtr theAttribute) const
{
  std::set<ConstraintPtr> aResult;
  MapAttributeFeature::const_iterator anIt = myAttributes.find(theAttribute);
  if (anIt == myAttributes.end())
    return aResult;
  std::set<FeaturePtr>::const_iterator aFIter = anIt->second.begin();
  MapFeatureConstraint::const_iterator aFeatIter;
  for (; aFIter != anIt->second.end(); aFIter++) {
    aFeatIter = myFeatures.find(*aFIter);
    if (aFeatIter != myFeatures.end())
      aResult.insert(aFeatIter->second.begin(), aFeatIter->second.end());
    else {
      ConstraintPtr aConstraint = std::dynamic_pointer_cast<SketchPlugin_Constraint>(*aFIter);
      if (aConstraint)
        aResult.insert(aConstraint);
    }
  }
  return aResult;
}

void SketchSolver_FeatureStorage::blockEvents(bool isBlocked) const
{
  std::set<ConstraintPtr>::iterator aCIter = myConstraints.begin();
  for (; aCIter != myConstraints.end(); aCIter++)
    if ((*aCIter)->data() && (*aCIter)->data()->isValid())
      (*aCIter)->data()->blockSendAttributeUpdated(isBlocked);

  MapFeatureConstraint::const_iterator aFIter = myFeatures.begin();
  for (; aFIter != myFeatures.end(); aFIter++)
    if (aFIter->first->data() && aFIter->first->data()->isValid())
      aFIter->first->data()->blockSendAttributeUpdated(isBlocked);

  MapAttributeFeature::const_iterator anAtIter = myAttributes.begin();
  for (; anAtIter != myAttributes.end(); anAtIter++)
    if (anAtIter->first->owner() && anAtIter->first->owner()->data() &&
        anAtIter->first->owner()->data()->isValid())
      anAtIter->first->owner()->data()->blockSendAttributeUpdated(isBlocked);
}
