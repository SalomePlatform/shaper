// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:    SketchPlugin_MultiRotation.cpp
// Created: 21 Apr 2015
// Author:  Artem ZHIDKOV

#include "SketchPlugin_MultiRotation.h"

#include <GeomDataAPI_Point2D.h>
#include <ModelAPI_AttributeDouble.h>
#include <ModelAPI_AttributeInteger.h>
#include <ModelAPI_Data.h>
#include <ModelAPI_ResultConstruction.h>
#include <ModelAPI_AttributeRefList.h>
#include <ModelAPI_Events.h>
#include <ModelAPI_Session.h>
#include <ModelAPI_Validator.h>

#include <GeomAPI_Pnt2d.h>
#include <GeomAPI_XY.h>

#include <SketcherPrs_Factory.h>

#include <cmath>

#define PI 3.1415926535897932

SketchPlugin_MultiRotation::SketchPlugin_MultiRotation()
{
}

void SketchPlugin_MultiRotation::initAttributes()
{
  data()->addAttribute(CENTER_ID(), GeomDataAPI_Point2D::typeId());
  data()->addAttribute(ANGLE_ID(), ModelAPI_AttributeDouble::typeId());
  data()->addAttribute(NUMBER_OF_COPIES_ID(), ModelAPI_AttributeInteger::typeId());
  data()->addAttribute(SketchPlugin_Constraint::ENTITY_A(), ModelAPI_AttributeRefList::typeId());
  data()->addAttribute(SketchPlugin_Constraint::ENTITY_B(), ModelAPI_AttributeRefList::typeId());
  data()->addAttribute(ROTATION_LIST_ID(), ModelAPI_AttributeRefList::typeId());
  ModelAPI_Session::get()->validators()->registerNotObligatory(getKind(), SketchPlugin_Constraint::ENTITY_A());
  ModelAPI_Session::get()->validators()->registerNotObligatory(getKind(), SketchPlugin_Constraint::ENTITY_B());
}

void SketchPlugin_MultiRotation::execute()
{
  if (!sketch()) {
    // it is possible, that this method is called before this feature has back reference to sketch
    // in this case, the execute is performed after this is done
    return;
  }

  AttributeRefListPtr aRotationObjectRefs = reflist(ROTATION_LIST_ID());
  int aNbCopies = integer(NUMBER_OF_COPIES_ID())->value();

  // Obtain center and angle of rotation
  std::shared_ptr<GeomDataAPI_Point2D> aCenter = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
      attribute(CENTER_ID()));
  if (!aCenter || !aCenter->isInitialized())
    return;
  // make a visible points
  SketchPlugin_Sketch::createPoint2DResult(this, sketch(), CENTER_ID(), 0);

  double anAngle = std::dynamic_pointer_cast<ModelAPI_AttributeDouble>(
      attribute(ANGLE_ID()))->value();
  // Convert angle to radians
  anAngle *= PI / 180.0;

  // Wait all objects being created, then send update events
  static Events_ID anUpdateEvent = Events_Loop::eventByName(EVENT_OBJECT_UPDATED);
  bool isUpdateFlushed = Events_Loop::loop()->isFlushed(anUpdateEvent);
  if (isUpdateFlushed)
    Events_Loop::loop()->setFlushed(anUpdateEvent, false);

  std::shared_ptr<ModelAPI_Data> aData = data();
  AttributeRefListPtr aRefListOfShapes = std::dynamic_pointer_cast<ModelAPI_AttributeRefList>(
      aData->attribute(SketchPlugin_Constraint::ENTITY_A()));
  AttributeRefListPtr aRefListOfRotated = std::dynamic_pointer_cast<ModelAPI_AttributeRefList>(
      aData->attribute(SketchPlugin_Constraint::ENTITY_B()));
  int aCurrentNbCopies = aRefListOfShapes->size() ?
      aRefListOfRotated->size() / aRefListOfShapes->size() - 1 : 0;
  std::list<ObjectPtr> anInitialList = aRefListOfShapes->list();
  std::list<ObjectPtr> aTargetList = aRefListOfRotated->list();
  std::list<ObjectPtr> anAddition;
  std::vector<bool> isUsed(anInitialList.size(), false);
  // collect new items and check the items to remove
  for(int anInd = 0; anInd < aRotationObjectRefs->size(); anInd++) {
    ObjectPtr anObject = aRotationObjectRefs->object(anInd);
    std::list<ObjectPtr>::const_iterator anIt = anInitialList.begin();
    std::vector<bool>::iterator aUsedIt = isUsed.begin();
    for (; anIt != anInitialList.end(); anIt++, aUsedIt++)
      if (*anIt == anObject) {
        *aUsedIt = true;
        break;
      }
    if (anIt == anInitialList.end())
      anAddition.push_back(anObject);
  }
  // remove unused items
  std::list<ObjectPtr>::iterator anInitIter = anInitialList.begin();
  std::list<ObjectPtr>::iterator aTargetIter = aTargetList.begin();
  std::vector<bool>::iterator aUsedIter = isUsed.begin();
  for (; aUsedIter != isUsed.end(); aUsedIter++) {
    if (!(*aUsedIter)) {
      aRefListOfShapes->remove(*anInitIter);
      aRefListOfRotated->remove(*aTargetIter++);
      for (int i = 0; i < aCurrentNbCopies && aTargetIter != aTargetList.end(); i++, aTargetIter++) {
        aRefListOfRotated->remove(*aTargetIter);
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

    aTargetList = aRefListOfRotated->list();
    aTargetIter = aTargetList.begin();
    ObjectPtr anObjToCopy = *aTargetIter;
    while (aTargetIter != aTargetList.end()) {
      aRefListOfRotated->remove(*aTargetIter);
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
            aRefListOfRotated->remove(anObject);
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
      aRefListOfRotated->append(*aTargetIter);
  }
  // add new items
  std::list<ObjectPtr>::iterator anAddIter = anAddition.begin();
  for (; anAddIter != anAddition.end(); anAddIter++) {
    aRefListOfShapes->append(*anAddIter);
    aRefListOfRotated->append(*anAddIter);
    for (int i = 0; i < aNbCopies; i++) {
      ObjectPtr anObject = copyFeature(*anAddIter);
      aRefListOfRotated->append(anObject);
    }
  }

////  if (fabs(anAngle) > 1.e-12) {
////    // Recalculate positions of features
////    aTargetList = aRefListOfRotated->list();
////    aTargetIter = aTargetList.begin();
////    while (aTargetIter != aTargetList.end()) {
////      ObjectPtr anInitialObject = *aTargetIter++;
////      for (int i = 0; i < aNbCopies && aTargetIter != aTargetList.end(); i++, aTargetIter++)
////        rotateFeature(anInitialObject, *aTargetIter, aCenter->x(), aCenter->y(), anAngle * (i + 1));
////    }
////  }

  // send events to update the sub-features by the solver
  if (isUpdateFlushed)
    Events_Loop::loop()->setFlushed(anUpdateEvent, true);
}

AISObjectPtr SketchPlugin_MultiRotation::getAISObject(AISObjectPtr thePrevious)
{
  if (!sketch())
    return thePrevious;

  AISObjectPtr anAIS = thePrevious;
  if (!anAIS) {
    anAIS = SketcherPrs_Factory::rotateConstraint(this, sketch()->coordinatePlane());
  }
  return anAIS;
}


ObjectPtr SketchPlugin_MultiRotation::copyFeature(ObjectPtr theObject)
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

void SketchPlugin_MultiRotation::rotateFeature(
    ObjectPtr theInitial, ObjectPtr theTarget,
    double theCenterX, double theCenterY, double theAngle)
{
  std::shared_ptr<GeomAPI_Pnt2d> aCenter(new GeomAPI_Pnt2d(theCenterX, theCenterY));
  double cosA = std::cos(theAngle);
  double sinA = std::sin(theAngle);

  FeaturePtr anInitialFeature = ModelAPI_Feature::feature(theInitial);
  FeaturePtr aTargetFeature = ModelAPI_Feature::feature(theTarget);

  // block feature update
  aTargetFeature->data()->blockSendAttributeUpdated(true);

  std::list<AttributePtr> anInitAttrList =
      anInitialFeature->data()->attributes(GeomDataAPI_Point2D::typeId());
  std::list<AttributePtr> aTargetAttrList =
      aTargetFeature->data()->attributes(GeomDataAPI_Point2D::typeId());
  std::list<AttributePtr>::iterator anInitIt = anInitAttrList.begin();
  std::list<AttributePtr>::iterator aTargetIt = aTargetAttrList.begin();
  for (; anInitIt != anInitAttrList.end(); anInitIt++, aTargetIt++) {
    std::shared_ptr<GeomDataAPI_Point2D> aPointFrom =
        std::dynamic_pointer_cast<GeomDataAPI_Point2D>(*anInitIt);
    std::shared_ptr<GeomDataAPI_Point2D> aPointTo =
        std::dynamic_pointer_cast<GeomDataAPI_Point2D>(*aTargetIt);
    std::shared_ptr<GeomAPI_XY> aPnt = aPointFrom->pnt()->xy();
    if (aPnt->distance(aCenter->xy()) > 1.e-7) {
      std::shared_ptr<GeomAPI_XY> aDir = aPnt->decreased(aCenter->xy());
      double dx = cosA * aDir->x() - sinA * aDir->y();
      double dy = sinA * aDir->x() + cosA * aDir->y();
      aPnt->setX(aCenter->x() + dx);
      aPnt->setY(aCenter->y() + dy);
    }
    aPointTo->setValue(aPnt->x(), aPnt->y());
  }

  // unblock feature update
  aTargetFeature->data()->blockSendAttributeUpdated(false);
}


void SketchPlugin_MultiRotation::attributeChanged(const std::string& theID)
{
  if (theID == ROTATION_LIST_ID()) {
    AttributeRefListPtr aRotationObjectRefs = reflist(ROTATION_LIST_ID());
    if (aRotationObjectRefs->size() == 0) {
      int aNbCopies = integer(NUMBER_OF_COPIES_ID())->value();
      // Clear list of objects
      AttributeRefListPtr aRefListOfRotated = std::dynamic_pointer_cast<ModelAPI_AttributeRefList>(
          data()->attribute(SketchPlugin_Constraint::ENTITY_B()));
      std::list<ObjectPtr> aTargetList = aRefListOfRotated->list();
      std::list<ObjectPtr>::iterator aTargetIter = aTargetList.begin();
      while (aTargetIter != aTargetList.end()) {
        aTargetIter++;
        for (int i = 0; i < aNbCopies && aTargetIter != aTargetList.end(); i++, aTargetIter++) {
          aRefListOfRotated->remove(*aTargetIter);
          // remove the corresponding feature from the sketch
          ResultConstructionPtr aRC =
            std::dynamic_pointer_cast<ModelAPI_ResultConstruction>(*aTargetIter);
          DocumentPtr aDoc = aRC ? aRC->document() : DocumentPtr();
          FeaturePtr aFeature =  aDoc ? aDoc->feature(aRC) : FeaturePtr();
          if (aFeature)
            aDoc->removeFeature(aFeature);
        }
      }
      aRefListOfRotated->clear();
      std::dynamic_pointer_cast<ModelAPI_AttributeRefList>(
        data()->attribute(SketchPlugin_Constraint::ENTITY_A()))->clear();
      std::dynamic_pointer_cast<ModelAPI_AttributeRefList>(
        data()->attribute(SketchPlugin_Constraint::ENTITY_B()))->clear();
    }
  }
}
