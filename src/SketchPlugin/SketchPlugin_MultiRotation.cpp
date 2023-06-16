// Copyright (C) 2014-2023  CEA/DEN, EDF R&D
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

#include "SketchPlugin_MultiRotation.h"
#include "SketchPlugin_Tools.h"

#include <GeomDataAPI_Point2D.h>
#include <ModelAPI_AttributeRefAttr.h>
#include <ModelAPI_AttributeBoolean.h>
#include <ModelAPI_AttributeDouble.h>
#include <ModelAPI_AttributeString.h>
#include <ModelAPI_AttributeInteger.h>
#include <ModelAPI_Data.h>
#include <ModelAPI_ResultConstruction.h>
#include <ModelAPI_AttributeRefList.h>
#include <ModelAPI_Events.h>
#include <ModelAPI_Session.h>
#include <ModelAPI_Validator.h>
#include <ModelAPI_Tools.h>

#include <SketchPlugin_SketchEntity.h>

#include <GeomAPI_Pnt2d.h>
#include <GeomAPI_XY.h>

#include <SketcherPrs_Factory.h>

#include <cmath>

static const double PI = 3.1415926535897932;
static const double PERIOD = 360.0;
static const double ANGLETOL = 1.e-7;

SketchPlugin_MultiRotation::SketchPlugin_MultiRotation()
  : isUpdatingAngle(false)
{
}

void SketchPlugin_MultiRotation::initAttributes()
{
  data()->addAttribute(CENTER_ID(), ModelAPI_AttributeRefAttr::typeId());

  data()->addAttribute(ANGLE_TYPE(),   ModelAPI_AttributeString::typeId());
  data()->addAttribute(ANGLE_ID(), ModelAPI_AttributeDouble::typeId());
  data()->addAttribute(NUMBER_OF_OBJECTS_ID(), ModelAPI_AttributeInteger::typeId());
  data()->addAttribute(SketchPlugin_Constraint::ENTITY_A(), ModelAPI_AttributeRefList::typeId());
  data()->addAttribute(SketchPlugin_Constraint::ENTITY_B(), ModelAPI_AttributeRefList::typeId());
  data()->addAttribute(ROTATION_LIST_ID(), ModelAPI_AttributeRefList::typeId());
  data()->addAttribute(REVERSED_ID(), ModelAPI_AttributeBoolean::typeId());

  ModelAPI_Session::get()->validators()->
    registerNotObligatory(getKind(), SketchPlugin_Constraint::ENTITY_A());
  ModelAPI_Session::get()->validators()->
    registerNotObligatory(getKind(), SketchPlugin_Constraint::ENTITY_B());
}

void SketchPlugin_MultiRotation::execute()
{
  if (!sketch()) {
    // it is possible, that this method is called before this feature has back reference to sketch
    // in this case, the execute is performed after this is done
    return;
  }

  AttributeRefListPtr aRotationObjectRefs = reflist(ROTATION_LIST_ID());
  int aNbCopies = integer(NUMBER_OF_OBJECTS_ID())->value() - 1;
  if (aNbCopies <= 0)
    return;

  // Obtain center and angle of rotation
  AttributeRefAttrPtr aCenter = data()->refattr(CENTER_ID());
  if (!aCenter || !aCenter->isInitialized())
    return;

  //double anAngle = std::dynamic_pointer_cast<ModelAPI_AttributeDouble>(
  //                                                           attribute(ANGLE_ID()))->value();

  // Convert angle to radians
  //anAngle *= PI / 180.0;

  // Wait all objects being created, then send update events
  static Events_ID anUpdateEvent = Events_Loop::eventByName(EVENT_OBJECT_UPDATED);
  bool isUpdateFlushed = Events_Loop::loop()->isFlushed(anUpdateEvent);
  if (isUpdateFlushed)
    Events_Loop::loop()->setFlushed(anUpdateEvent, false);

  // Save the current feature of the document, because new features may appear while executing.
  // In this case, they will become current. But if the number of copies is updated from outside
  // of sketch (e.g. by parameter change), the history line should not hold in sketch.
  keepCurrentFeature();

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
  std::set<FeaturePtr> aFeaturesToBeRemoved;
  for (; aUsedIter != isUsed.end(); aUsedIter++) {
    if (!(*aUsedIter)) {
      aRefListOfShapes->remove(*anInitIter);
      aRefListOfRotated->remove(*aTargetIter++);
      for (int i = 0; i < aCurrentNbCopies && aTargetIter != aTargetList.end();
           i++, aTargetIter++) {
        aRefListOfRotated->remove(*aTargetIter);
        // remove the corresponding feature from the sketch
        ResultConstructionPtr aRC =
            std::dynamic_pointer_cast<ModelAPI_ResultConstruction>(*aTargetIter);
        DocumentPtr aDoc = aRC ? aRC->document() : DocumentPtr();
        FeaturePtr aFeature =  aDoc ? aDoc->feature(aRC) : FeaturePtr();
        if (aFeature)
          aFeaturesToBeRemoved.insert(aFeature);
      }
    } else {
      for (int i = 0; i <= aNbCopies && aTargetIter != aTargetList.end(); i++)
        aTargetIter++;
    }
    if (anInitIter != anInitialList.end())
      anInitIter++;
  }
  ModelAPI_Tools::removeFeaturesAndReferences(aFeaturesToBeRemoved);
  aFeaturesToBeRemoved.clear();
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
              aFeaturesToBeRemoved.insert(aFeature);
          }
          ind++;
        }
        ind = 0;
        if (aTargetIter != aTargetList.end())
          anObjToCopy = *aTargetIter;
      }
    }
    ModelAPI_Tools::removeFeaturesAndReferences(aFeaturesToBeRemoved);
    aFeaturesToBeRemoved.clear();

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

  restoreCurrentFeature();

  // send events to update the sub-features by the solver
  if (isUpdateFlushed)
    Events_Loop::loop()->setFlushed(anUpdateEvent, true);
}

AISObjectPtr SketchPlugin_MultiRotation::getAISObject(AISObjectPtr thePrevious)
{
  if (!sketch())
    return thePrevious;

  AISObjectPtr anAIS = SketcherPrs_Factory::rotateConstraint(this, sketch(),
                                                             thePrevious);
  return anAIS;
}

void SketchPlugin_MultiRotation::erase()
{
  static Events_Loop* aLoop = Events_Loop::loop();
  static Events_ID aRedispEvent = aLoop->eventByName(EVENT_OBJECT_TO_REDISPLAY);

  // Set copy attribute to false on all copied features.
  AttributeRefListPtr aRefListOfShapes = std::dynamic_pointer_cast<ModelAPI_AttributeRefList>(
      data()->attribute(SketchPlugin_Constraint::ENTITY_A()));
  AttributeRefListPtr aRefListOfRotated = std::dynamic_pointer_cast<ModelAPI_AttributeRefList>(
      data()->attribute(SketchPlugin_Constraint::ENTITY_B()));

  if(aRefListOfShapes.get() && aRefListOfRotated.get()) {
    for(int anIndex = 0; anIndex < aRefListOfRotated->size(); anIndex++) {
      ObjectPtr anObject = aRefListOfRotated->object(anIndex);
      if(aRefListOfShapes->isInList(anObject)) {
        // Don't modify attribute of original features, just skip.
        continue;
      }
      if(anObject.get()) {
        ResultPtr aRes = std::dynamic_pointer_cast<ModelAPI_Result>(anObject);
        if(aRes.get()) {
          FeaturePtr aFeature = aRes->document()->feature(aRes);
          if(aFeature.get()) {
            AttributeBooleanPtr aBooleanAttr =
              aFeature->boolean(SketchPlugin_SketchEntity::COPY_ID());
            if(aBooleanAttr.get()) {
              if (ModelAPI_Session::get()->isOperation()) // if this is not undo or redo
                aBooleanAttr->setValue(false);
              // Redisplay object as it is not copy anymore.
              ModelAPI_EventCreator::get()->sendUpdated(aRes, aRedispEvent);
            }
          }
        }
      }
    }
  }

  SketchPlugin_ConstraintBase::erase();
}

ObjectPtr SketchPlugin_MultiRotation::copyFeature(ObjectPtr theObject)
{
  ResultPtr aResult = std::dynamic_pointer_cast<ModelAPI_ResultConstruction>(theObject);
  FeaturePtr aFeature = ModelAPI_Feature::feature(theObject);
  if (!aFeature || !aResult)
    return ObjectPtr();

  FeaturePtr aNewFeature =
    SketchPlugin_Sketch::addUniqueNamedCopiedFeature(aFeature, sketch(), true);
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

/*void SketchPlugin_MultiRotation::rotateFeature(
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
}*/


void SketchPlugin_MultiRotation::attributeChanged(const std::string& theID)
{
  if (theID == ROTATION_LIST_ID()) {
    AttributeRefListPtr aRotationObjectRefs = reflist(ROTATION_LIST_ID());
    if (aRotationObjectRefs->size() == 0) {
      int aNbCopies = integer(NUMBER_OF_OBJECTS_ID())->value()-1;
      if (aNbCopies <= 0)
        return;

      // Clear list of objects
      AttributeRefListPtr aRefListOfRotated = reflist(SketchPlugin_Constraint::ENTITY_B());
      std::list<ObjectPtr> aTargetList = aRefListOfRotated->list();
      std::list<ObjectPtr>::iterator aTargetIter = aTargetList.begin();
      std::set<FeaturePtr> aFeaturesToBeRemoved;
      while (aTargetIter != aTargetList.end()) {
        aTargetIter++;
        for (int i = 0; i < aNbCopies && aTargetIter != aTargetList.end(); i++, aTargetIter++) {
          aRefListOfRotated->remove(*aTargetIter);
          // remove the corresponding feature from the sketch
          FeaturePtr aFeature = ModelAPI_Feature::feature(*aTargetIter);
          if (aFeature)
            aFeaturesToBeRemoved.insert(aFeature);
        }
      }
      ModelAPI_Tools::removeFeaturesAndReferences(aFeaturesToBeRemoved);

      aRefListOfRotated->clear();
      reflist(SketchPlugin_Constraint::ENTITY_A())->clear();
    }
  }
  else if (!isUpdatingAngle && real(ANGLE_ID())->isInitialized())
  {
    isUpdatingAngle = true;
    AttributeDoublePtr anAngle = real(ANGLE_ID());
    if (theID == ANGLE_TYPE() && integer(NUMBER_OF_OBJECTS_ID())->isInitialized()) {
      if (string(ANGLE_TYPE())->value() != "SingleAngle")
        anAngle->setValue(anAngle->value() * (integer(NUMBER_OF_OBJECTS_ID())->value() - 1));
      else
      {
        int aNbSplits = integer(NUMBER_OF_OBJECTS_ID())->value();
        if (anAngle->value() < PERIOD - ANGLETOL)
          aNbSplits -= 1;
        anAngle->setValue(anAngle->value() / aNbSplits);
      }
    }
    else if (theID == ANGLE_ID()) {
      if (anAngle->value() > PERIOD + ANGLETOL || anAngle->value() < -ANGLETOL)
        anAngle->setValue(anAngle->value() + PERIOD * ceil(-anAngle->value() / PERIOD));
      if (fabs(anAngle->value() - PERIOD) < ANGLETOL)
        anAngle->setValue(PERIOD);
      else if (fabs(anAngle->value()) < ANGLETOL)
        anAngle->setValue(0.);
    }
    isUpdatingAngle = false;
  }
}
