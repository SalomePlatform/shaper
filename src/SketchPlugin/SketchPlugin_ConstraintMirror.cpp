// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:    SketchPlugin_ConstraintMirror.cpp
// Created: 17 Mar 2015
// Author:  Artem ZHIDKOV

#include "SketchPlugin_ConstraintMirror.h"

#include <ModelAPI_AttributeDouble.h>
#include <ModelAPI_Data.h>
#include <ModelAPI_ResultConstruction.h>
#include <ModelAPI_AttributeRefList.h>
#include <ModelAPI_AttributeSelectionList.h>
#include <ModelAPI_Events.h>
#include <ModelAPI_Session.h>
#include <ModelAPI_Validator.h>

#include <SketchPlugin_Line.h>
#include <SketchPlugin_Sketch.h>

#include <SketcherPrs_Factory.h>

#include <Config_PropManager.h>

SketchPlugin_ConstraintMirror::SketchPlugin_ConstraintMirror()
{
}

void SketchPlugin_ConstraintMirror::initAttributes()
{
  data()->addAttribute(SketchPlugin_Constraint::ENTITY_A(), ModelAPI_AttributeRefAttr::typeId());
  data()->addAttribute(SketchPlugin_Constraint::ENTITY_B(), ModelAPI_AttributeRefList::typeId());
  data()->addAttribute(SketchPlugin_Constraint::ENTITY_C(), ModelAPI_AttributeRefList::typeId());
  AttributeSelectionListPtr aSelection = 
    std::dynamic_pointer_cast<ModelAPI_AttributeSelectionList>(data()->addAttribute(
    SketchPlugin_ConstraintMirror::MIRROR_LIST_ID(), ModelAPI_AttributeSelectionList::typeId()));
  aSelection->setSelectionType("EDGE");
  ModelAPI_Session::get()->validators()->registerNotObligatory(getKind(), SketchPlugin_Constraint::ENTITY_B());
  ModelAPI_Session::get()->validators()->registerNotObligatory(getKind(), SketchPlugin_Constraint::ENTITY_C());
}

void SketchPlugin_ConstraintMirror::execute()
{
  AttributeSelectionListPtr aMirrorObjectRefs =
      selectionList(SketchPlugin_ConstraintMirror::MIRROR_LIST_ID());

  // Wait all objects being created, then send update events
  static Events_ID anUpdateEvent = Events_Loop::eventByName(EVENT_OBJECT_UPDATED);
  bool isUpdateFlushed = Events_Loop::loop()->isFlushed(anUpdateEvent);
  if (isUpdateFlushed)
    Events_Loop::loop()->setFlushed(anUpdateEvent, false);

  std::shared_ptr<ModelAPI_Data> aData = data();
  AttributeRefListPtr aRefListOfShapes = std::dynamic_pointer_cast<ModelAPI_AttributeRefList>(
      aData->attribute(SketchPlugin_Constraint::ENTITY_B()));
  AttributeRefListPtr aRefListOfMirrored = std::dynamic_pointer_cast<ModelAPI_AttributeRefList>(
      aData->attribute(SketchPlugin_Constraint::ENTITY_C()));
  std::list<ObjectPtr> anInitialList =  aRefListOfShapes->list();
  std::list<ObjectPtr> aMirroredList =  aRefListOfMirrored->list();
  std::vector<bool> isUsed(anInitialList.size(), false);
  // add new items to the list
  for(int anInd = 0; anInd < aMirrorObjectRefs->size(); anInd++) {
    std::shared_ptr<ModelAPI_AttributeSelection> aSelect = aMirrorObjectRefs->value(anInd);
    std::list<ObjectPtr>::const_iterator anIt = anInitialList.begin();
    std::vector<bool>::iterator aUsedIt = isUsed.begin();
    for (; anIt != anInitialList.end(); anIt++, aUsedIt++)
      if (*anIt == aSelect->context()) {
        *aUsedIt = true;
        break;
      }
    if (anIt == anInitialList.end())
      aRefListOfShapes->append(aSelect->context());
  }
  // remove unused items
  std::list<ObjectPtr>::iterator anInitIter = anInitialList.begin();
  std::list<ObjectPtr>::iterator aMirrorIter = aMirroredList.begin();
  std::vector<bool>::iterator aUsedIter = isUsed.begin();
  for (; aUsedIter != isUsed.end(); aUsedIter++) {
    if (!(*aUsedIter)) {
      aRefListOfShapes->remove(*anInitIter);
      if (aMirrorIter != aMirroredList.end()) {
        aRefListOfMirrored->remove(*aMirrorIter);
        // remove the corresponding feature from the sketch
        ResultConstructionPtr aRC =
            std::dynamic_pointer_cast<ModelAPI_ResultConstruction>(*aMirrorIter);
        DocumentPtr aDoc = aRC ? aRC->document() : DocumentPtr();
        FeaturePtr aFeature =  aDoc ? aDoc->feature(aRC) : FeaturePtr();
        if (aFeature)
          aDoc->removeFeature(aFeature);
      }
    }
    if (anInitIter != anInitialList.end())
      anInitIter++;
    if (aMirrorIter != aMirroredList.end())
      aMirrorIter++;
  }

  static Events_ID aRedisplayEvent = Events_Loop::eventByName(EVENT_OBJECT_TO_REDISPLAY);

  // Check consistency of initial list and mirrored list
  anInitialList =  aRefListOfShapes->list();
  anInitIter = anInitialList.begin();
  aMirrorIter = aMirroredList.begin();
  int indFirstWrong = 0; // index of element starts difference in the lists
  std::set<int> anInvalidInd; // list of indices of removed features
  std::shared_ptr<SketchPlugin_Feature> aFeatureIn, aFeatureOut;
  for ( ; anInitIter != anInitialList.end(); anInitIter++, indFirstWrong++) {
    // Add features and store indices of objects to remove
    aFeatureIn = std::dynamic_pointer_cast<SketchPlugin_Feature>(*anInitIter);
    ResultConstructionPtr aRCIn;
    if (!aFeatureIn) {
      aRCIn = std::dynamic_pointer_cast<ModelAPI_ResultConstruction>(*anInitIter);
      if (aRCIn)
        aFeatureIn = std::dynamic_pointer_cast<SketchPlugin_Feature>(
            aRCIn->document()->feature(aRCIn));
    }
    if (aMirrorIter == aMirroredList.end())
      aFeatureOut = std::shared_ptr<SketchPlugin_Feature>();
    else {
      aFeatureOut = std::dynamic_pointer_cast<SketchPlugin_Feature>(*aMirrorIter);
      if (!aFeatureOut) {
        ResultConstructionPtr aRC =
            std::dynamic_pointer_cast<ModelAPI_ResultConstruction>(*anInitIter);
        if (aRC)
          aFeatureOut = std::dynamic_pointer_cast<SketchPlugin_Feature>(
              aRC->document()->feature(aRC));
      }
    }
    if (!aFeatureIn) {
      if (aFeatureOut)
        break; // the lists are inconsistent
      continue;
    }
    if (!aFeatureOut) {
      if (aMirrorIter != aMirroredList.end())
        break; // the lists are inconsistent
      // There is no mirrored object yet, create it
      FeaturePtr aNewFeature = SketchPlugin_Sketch::addUniqueNamedCopiedFeature(aFeatureIn, sketch());
      aNewFeature->execute();
      ModelAPI_EventCreator::get()->sendUpdated(aNewFeature, aRedisplayEvent);

      std::shared_ptr<GeomAPI_Shape> aShapeIn = aRCIn->shape();
      const std::list<ResultPtr>& aResults = aNewFeature->results();
      std::list<ResultPtr>::const_iterator anIt = aResults.begin();
      for (; anIt != aResults.end(); anIt++) {
        ResultConstructionPtr aRC = std::dynamic_pointer_cast<ModelAPI_ResultConstruction>(*anIt);
        if (!aRC) continue;
        std::shared_ptr<GeomAPI_Shape> aShapeOut = aRC->shape();
        if ((aShapeIn->isVertex() && aShapeOut->isVertex()) ||
            (aShapeIn->isEdge() && aShapeOut->isEdge()) ||
            (aShapeIn->isFace() && aShapeOut->isFace()))
          aRefListOfMirrored->append(aRC);
      }
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

  // send events to update the sub-features by the solver
  if (isUpdateFlushed)
    Events_Loop::loop()->setFlushed(anUpdateEvent, true);
}

AISObjectPtr SketchPlugin_ConstraintMirror::getAISObject(AISObjectPtr thePrevious)
{
  if (!sketch())
    return thePrevious;

  AISObjectPtr anAIS = thePrevious;
  if (!anAIS) {
    anAIS = SketcherPrs_Factory::mirrorConstraint(this, sketch()->coordinatePlane());
  }
  return anAIS;
}


void SketchPlugin_ConstraintMirror::attributeChanged(const std::string& theID)
{
  if (theID == MIRROR_LIST_ID()) {
    AttributeSelectionListPtr aMirrorObjectRefs = selectionList(MIRROR_LIST_ID());
    if (aMirrorObjectRefs->size() == 0) {
      // Wait all objects being created, then send update events
      static Events_ID anUpdateEvent = Events_Loop::eventByName(EVENT_OBJECT_UPDATED);
      bool isUpdateFlushed = Events_Loop::loop()->isFlushed(anUpdateEvent);
      if (isUpdateFlushed)
        Events_Loop::loop()->setFlushed(anUpdateEvent, false);

      // Clear list of objects
      AttributeRefListPtr aRefListOfMirrored = std::dynamic_pointer_cast<ModelAPI_AttributeRefList>(
          data()->attribute(SketchPlugin_Constraint::ENTITY_C()));
      std::list<ObjectPtr> aTargetList = aRefListOfMirrored->list();
      std::list<ObjectPtr>::iterator aTargetIter = aTargetList.begin();
      for (; aTargetIter != aTargetList.end(); aTargetIter++) {
        aRefListOfMirrored->remove(*aTargetIter);
        // remove the corresponding feature from the sketch
        ResultConstructionPtr aRC =
            std::dynamic_pointer_cast<ModelAPI_ResultConstruction>(*aTargetIter);
        DocumentPtr aDoc = aRC ? aRC->document() : DocumentPtr();
        FeaturePtr aFeature =  aDoc ? aDoc->feature(aRC) : FeaturePtr();
        if (aFeature)
          aDoc->removeFeature(aFeature);
      }

      // send events to update the sub-features by the solver
      if (isUpdateFlushed)
        Events_Loop::loop()->setFlushed(anUpdateEvent, true);
    }
  }
}

