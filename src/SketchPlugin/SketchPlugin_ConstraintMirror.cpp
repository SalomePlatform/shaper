// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:    SketchPlugin_ConstraintMirror.cpp
// Created: 17 Mar 2015
// Author:  Artem ZHIDKOV

#include "SketchPlugin_ConstraintMirror.h"

#include <ModelAPI_AttributeDouble.h>
#include <ModelAPI_Data.h>
#include <ModelAPI_ResultConstruction.h>
#include <ModelAPI_AttributeRefList.h>
#include <ModelAPI_Session.h>

#include <SketchPlugin_Line.h>
#include <SketchPlugin_Sketch.h>

#include <SketcherPrs_Factory.h>

#include <Config_PropManager.h>

SketchPlugin_ConstraintMirror::SketchPlugin_ConstraintMirror()
{
}

void SketchPlugin_ConstraintMirror::initAttributes()
{
  data()->addAttribute(SketchPlugin_Constraint::ENTITY_A(), ModelAPI_AttributeRefAttr::type());
  data()->addAttribute(SketchPlugin_Constraint::ENTITY_B(), ModelAPI_AttributeRefList::type());
  data()->addAttribute(SketchPlugin_Constraint::ENTITY_C(), ModelAPI_AttributeRefList::type());
}

void SketchPlugin_ConstraintMirror::execute()
{
  // Objects to be mirrored will be created here
  std::shared_ptr<ModelAPI_Data> aData = data();
  AttributeRefListPtr aRefListOfShapes = std::dynamic_pointer_cast<ModelAPI_AttributeRefList>(
      aData->attribute(SketchPlugin_Constraint::ENTITY_B()));
  if (!aRefListOfShapes->isInitialized())
    return ;

  AttributeRefListPtr aRefListOfMirrored = std::dynamic_pointer_cast<ModelAPI_AttributeRefList>(
      aData->attribute(SketchPlugin_Constraint::ENTITY_C()));
  // Check consistency of initial list and mirrored list
  std::list<ObjectPtr> anInitialList =  aRefListOfShapes->list();
  std::list<ObjectPtr> aMirroredList =  aRefListOfMirrored->list();
  std::list<ObjectPtr>::iterator anInitIter = anInitialList.begin();
  std::list<ObjectPtr>::iterator aMirrorIter = aMirroredList.begin();
  int indFirstWrong = 0; // index of element starts difference in the lists
  std::set<int> anInvalidInd; // list of indices of removed features
  std::shared_ptr<SketchPlugin_Feature> aFeatureIn, aFeatureOut;
  for ( ; anInitIter != anInitialList.end(); anInitIter++, indFirstWrong++) {
    // Add features and store indices of objects to remove
    aFeatureIn = std::dynamic_pointer_cast<SketchPlugin_Feature>(*anInitIter);
    aFeatureOut = aMirrorIter != aMirroredList.end() ? 
        std::dynamic_pointer_cast<SketchPlugin_Feature>(*aMirrorIter) :
        std::shared_ptr<SketchPlugin_Feature>();
    if (!aFeatureIn) {
      if (aFeatureOut)
        break; // the lists are inconsistent
      continue;
    }
    if (!aFeatureOut) {
      if (aMirrorIter != aMirroredList.end())
        break; // the lists are inconsistent
      // There is no mirrored object yet, create it
      FeaturePtr aNewFeature = aFeatureIn->document()->addFeature(aFeatureIn->getKind());
      aRefListOfMirrored->append(aNewFeature);
      continue;
    }
    if (aFeatureIn->getKind() != aFeatureOut->getKind())
      break; // the lists are inconsistent
    if (!aFeatureIn->data()->isValid()) {
      // initial feature was removed, delete it from lists
      anInvalidInd.insert(indFirstWrong);
    }
    aMirrorIter++;
  }
  // Remove from the list objects already deleted before
  std::set<int>::reverse_iterator anIt = anInvalidInd.rbegin();
  for ( ; anIt != anInvalidInd.rend(); anIt++) {
    if (*anIt < indFirstWrong) indFirstWrong--;
    aRefListOfShapes->remove(aRefListOfShapes->object(*anIt));
    aRefListOfMirrored->remove(aRefListOfMirrored->object(*anIt));
  }
  // If the lists inconsistent, remove all objects from mirrored list starting from indFirstWrong
  if (anInitIter != anInitialList.end()) {
    while (aRefListOfMirrored->size() > indFirstWrong)
      aRefListOfMirrored->remove(aRefListOfMirrored->object(indFirstWrong));
    // Create mirrored features instead of removed
    anInitialList =  aRefListOfShapes->list();
    anInitIter = anInitialList.begin();
    for (int i = 0; i < indFirstWrong; i++) anInitIter++;
    for ( ; anInitIter != anInitialList.end(); anInitIter++) {
      aFeatureIn = std::dynamic_pointer_cast<SketchPlugin_Feature>(*anInitIter);
      FeaturePtr aNewFeature = aFeatureIn->document()->addFeature(aFeatureIn->getKind());
      aRefListOfMirrored->append(aNewFeature);
    }
  }
}

AISObjectPtr SketchPlugin_ConstraintMirror::getAISObject(AISObjectPtr thePrevious)
{
  if (!sketch())
    return thePrevious;

  AISObjectPtr anAIS = thePrevious;
  /// TODO: Equal constraint presentation should be put here
  return anAIS;
}


