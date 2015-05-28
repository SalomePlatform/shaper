// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:    SketchPlugin_MultiTranslation.cpp
// Created: 21 Apr 2015
// Author:  Artem ZHIDKOV

#include "SketchPlugin_MultiTranslation.h"

#include <GeomAPI_XY.h>
#include <GeomDataAPI_Point2D.h>
#include <ModelAPI_AttributeDouble.h>
#include <ModelAPI_AttributeInteger.h>
#include <ModelAPI_Data.h>
#include <ModelAPI_ResultConstruction.h>
#include <ModelAPI_AttributeRefList.h>
#include <ModelAPI_AttributeSelectionList.h>
#include <ModelAPI_Events.h>
#include <ModelAPI_Session.h>
#include <ModelAPI_Validator.h>

#include <SketcherPrs_Factory.h>

SketchPlugin_MultiTranslation::SketchPlugin_MultiTranslation()
{
}

void SketchPlugin_MultiTranslation::initAttributes()
{
  data()->addAttribute(START_POINT_ID(), GeomDataAPI_Point2D::typeId());
  data()->addAttribute(END_POINT_ID(), GeomDataAPI_Point2D::typeId());
  data()->addAttribute(NUMBER_OF_COPIES_ID(), ModelAPI_AttributeInteger::typeId());
  data()->addAttribute(SketchPlugin_Constraint::ENTITY_A(), ModelAPI_AttributeRefList::typeId());
  data()->addAttribute(SketchPlugin_Constraint::ENTITY_B(), ModelAPI_AttributeRefList::typeId());
  AttributeSelectionListPtr aSelection = 
    std::dynamic_pointer_cast<ModelAPI_AttributeSelectionList>(data()->addAttribute(
    TRANSLATION_LIST_ID(), ModelAPI_AttributeSelectionList::typeId()));
  aSelection->setSelectionType("EDGE");
  ModelAPI_Session::get()->validators()->registerNotObligatory(getKind(), SketchPlugin_Constraint::ENTITY_A());
  ModelAPI_Session::get()->validators()->registerNotObligatory(getKind(), SketchPlugin_Constraint::ENTITY_B());
}

void SketchPlugin_MultiTranslation::execute()
{
  if (!sketch()) {
    // it is possible, that 
    return;
  }

  AttributeSelectionListPtr aTranslationObjectRefs = selectionList(TRANSLATION_LIST_ID());
  int aNbCopies = integer(NUMBER_OF_COPIES_ID())->value();

  // Calculate shift vector
  std::shared_ptr<GeomDataAPI_Point2D> aStart = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
      attribute(START_POINT_ID()));
  std::shared_ptr<GeomDataAPI_Point2D> aEnd = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
      attribute(END_POINT_ID()));
  if (!aStart || !aEnd || !aStart->isInitialized() || !aEnd->isInitialized())
    return;

  // make a visible points
  SketchPlugin_Sketch::createPoint2DResult(this, sketch(), START_POINT_ID(), 0);
  SketchPlugin_Sketch::createPoint2DResult(this, sketch(), END_POINT_ID(), 1);

  std::shared_ptr<GeomAPI_XY> aShiftVec(new GeomAPI_XY(aEnd->x() - aStart->x(), aEnd->y() - aStart->y()));

  // Wait all objects being created, then send update events
  static Events_ID anUpdateEvent = Events_Loop::eventByName(EVENT_OBJECT_UPDATED);
  bool isUpdateFlushed = Events_Loop::loop()->isFlushed(anUpdateEvent);
  if (isUpdateFlushed)
    Events_Loop::loop()->setFlushed(anUpdateEvent, false);

  std::shared_ptr<ModelAPI_Data> aData = data();
  AttributeRefListPtr aRefListOfShapes = std::dynamic_pointer_cast<ModelAPI_AttributeRefList>(
      aData->attribute(SketchPlugin_Constraint::ENTITY_A()));
  AttributeRefListPtr aRefListOfTranslated = std::dynamic_pointer_cast<ModelAPI_AttributeRefList>(
      aData->attribute(SketchPlugin_Constraint::ENTITY_B()));
  int aCurrentNbCopies = aRefListOfShapes->size() ?
      aRefListOfTranslated->size() / aRefListOfShapes->size() - 1 : 0;
  std::list<ObjectPtr> anInitialList = aRefListOfShapes->list();
  std::list<ObjectPtr> aTargetList = aRefListOfTranslated->list();
  std::list<ResultPtr> anAddition;
  std::vector<bool> isUsed(anInitialList.size(), false);
  // collect new items and check the items to remove
  for(int anInd = 0; anInd < aTranslationObjectRefs->size(); anInd++) {
    std::shared_ptr<ModelAPI_AttributeSelection> aSelect = aTranslationObjectRefs->value(anInd);
    std::list<ObjectPtr>::const_iterator anIt = anInitialList.begin();
    std::vector<bool>::iterator aUsedIt = isUsed.begin();
    for (; anIt != anInitialList.end(); anIt++, aUsedIt++)
      if (*anIt == aSelect->context()) {
        *aUsedIt = true;
        break;
      }
    if (anIt == anInitialList.end())
      anAddition.push_back(aSelect->context());
  }
  // remove unused items
  std::list<ObjectPtr>::iterator anInitIter = anInitialList.begin();
  std::list<ObjectPtr>::iterator aTargetIter = aTargetList.begin();
  std::vector<bool>::iterator aUsedIter = isUsed.begin();
  for (; aUsedIter != isUsed.end(); aUsedIter++) {
    if (!(*aUsedIter)) {
      aRefListOfShapes->remove(*anInitIter);
      for (int i = 0; i < aCurrentNbCopies && aTargetIter != aTargetList.end(); i++, aTargetIter++) {
        aRefListOfTranslated->remove(*aTargetIter);
        // remove the corresponding feature from the sketch
        ResultConstructionPtr aRC =
            std::dynamic_pointer_cast<ModelAPI_ResultConstruction>(*aTargetIter);
        DocumentPtr aDoc = aRC ? aRC->document() : DocumentPtr();
        FeaturePtr aFeature =  aDoc ? aDoc->feature(aRC) : FeaturePtr();
        if (aFeature)
          aDoc->removeFeature(aFeature);
      }
    } else {
      for (int i = 0; i <= aNbCopies && aTargetIter != aTargetList.end(); i++)
        aTargetIter++;
    }
    if (anInitIter != anInitialList.end())
      anInitIter++;
  }
  // change number of copies
  if (aCurrentNbCopies != 0 && aNbCopies != aCurrentNbCopies) {
    bool isAdd = aNbCopies > aCurrentNbCopies;
    int aMinCopies = isAdd ? aCurrentNbCopies : aNbCopies;
    int aMaxCopies = isAdd ? aNbCopies : aCurrentNbCopies;
    int ind = 0;

    aTargetList = aRefListOfTranslated->list();
    aTargetIter = aTargetList.begin();
    ObjectPtr anObjToCopy = *aTargetIter;
    while (aTargetIter != aTargetList.end()) {
      aRefListOfTranslated->remove(*aTargetIter);
      aTargetIter++;
      ind++;
      if (ind > aMinCopies && ind <=aMaxCopies) {
        while (ind <= aMaxCopies) {
          if (isAdd) {
            // Add new shifted item
            ObjectPtr anObject = copyFeature(anObjToCopy);
            aTargetList.insert(aTargetIter, anObject);
          } else {
            // remove object
            std::list<ObjectPtr>::iterator aRemoveIt = aTargetIter++;
            ObjectPtr anObject = *aRemoveIt;
            aTargetList.erase(aRemoveIt);
            aRefListOfTranslated->remove(anObject);
            // remove the corresponding feature from the sketch
            ResultConstructionPtr aRC =
                std::dynamic_pointer_cast<ModelAPI_ResultConstruction>(anObject);
            DocumentPtr aDoc = aRC ? aRC->document() : DocumentPtr();
            FeaturePtr aFeature =  aDoc ? aDoc->feature(aRC) : FeaturePtr();
            if (aFeature)
              aDoc->removeFeature(aFeature);
          }
          ind++;
        }
        ind = 0;
        if (aTargetIter != aTargetList.end())
          anObjToCopy = *aTargetIter;
      }
    }

    for (aTargetIter = aTargetList.begin(); aTargetIter != aTargetList.end(); aTargetIter++)
      aRefListOfTranslated->append(*aTargetIter);
  }
  // add new items
  std::list<ResultPtr>::iterator anAddIter = anAddition.begin();
  for (; anAddIter != anAddition.end(); anAddIter++) {
    aRefListOfShapes->append(*anAddIter);
    aRefListOfTranslated->append(*anAddIter);
    for (int i = 0; i < aNbCopies; i++) {
      ObjectPtr anObject = copyFeature(*anAddIter);
      aRefListOfTranslated->append(anObject);
    }
  }

  // send events to update the sub-features by the solver
  if (isUpdateFlushed)
    Events_Loop::loop()->setFlushed(anUpdateEvent, true);
}

AISObjectPtr SketchPlugin_MultiTranslation::getAISObject(AISObjectPtr thePrevious)
{
  if (!sketch())
    return thePrevious;

  AISObjectPtr anAIS = thePrevious;
  if (!anAIS) {
    anAIS = SketcherPrs_Factory::translateConstraint(this, sketch()->coordinatePlane());
  }
  return anAIS;
}

ObjectPtr SketchPlugin_MultiTranslation::copyFeature(ObjectPtr theObject)
{
  ResultPtr aResult = std::dynamic_pointer_cast<ModelAPI_ResultConstruction>(theObject);
  FeaturePtr aFeature = ModelAPI_Feature::feature(theObject);
  if (!aFeature || !aResult)
    return ObjectPtr();

  FeaturePtr aNewFeature = SketchPlugin_Sketch::addUniqueNamedCopiedFeature(aFeature, sketch());

  aNewFeature->execute();
  static Events_ID aRedisplayEvent = Events_Loop::eventByName(EVENT_OBJECT_TO_REDISPLAY);
  ModelAPI_EventCreator::get()->sendUpdated(aNewFeature, aRedisplayEvent);

  std::shared_ptr<GeomAPI_Shape> aShapeIn = aResult->shape();
  const std::list<ResultPtr>& aResults = aNewFeature->results();
  std::list<ResultPtr>::const_iterator anIt = aResults.begin();
  for (; anIt != aResults.end(); anIt++) {
    ResultConstructionPtr aRC = std::dynamic_pointer_cast<ModelAPI_ResultConstruction>(*anIt);
    if (!aRC) continue;
    std::shared_ptr<GeomAPI_Shape> aShapeOut = aRC->shape();
    if ((aShapeIn->isVertex() && aShapeOut->isVertex()) ||
        (aShapeIn->isEdge() && aShapeOut->isEdge()) ||
        (aShapeIn->isFace() && aShapeOut->isFace()))
      return aRC;
  }
  return ObjectPtr();
}

