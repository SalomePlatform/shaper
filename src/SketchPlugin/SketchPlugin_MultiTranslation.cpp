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

#include "SketchPlugin_MultiTranslation.h"
#include "SketchPlugin_Tools.h"

#include <GeomAPI_XY.h>
#include <GeomDataAPI_Point2D.h>
#include <ModelAPI_AttributeDouble.h>
#include <ModelAPI_AttributeInteger.h>
#include <ModelAPI_Data.h>
#include <ModelAPI_ResultConstruction.h>
#include <ModelAPI_AttributeRefList.h>
#include <ModelAPI_AttributeRefAttr.h>
#include <ModelAPI_AttributeString.h>
#include <ModelAPI_Events.h>
#include <ModelAPI_Session.h>
#include <ModelAPI_Validator.h>
#include <ModelAPI_Tools.h>

#include <SketchPlugin_SketchEntity.h>
#include <SketcherPrs_Factory.h>

SketchPlugin_MultiTranslation::SketchPlugin_MultiTranslation()
{
}

void SketchPlugin_MultiTranslation::initAttributes()
{
  data()->addAttribute(VALUE_TYPE(),   ModelAPI_AttributeString::typeId());

  data()->addAttribute(START_POINT_ID(), ModelAPI_AttributeRefAttr::typeId());
  data()->addAttribute(END_POINT_ID(), ModelAPI_AttributeRefAttr::typeId());

  data()->addAttribute(NUMBER_OF_OBJECTS_ID(), ModelAPI_AttributeInteger::typeId());
  data()->addAttribute(SketchPlugin_Constraint::ENTITY_A(), ModelAPI_AttributeRefList::typeId());
  data()->addAttribute(SketchPlugin_Constraint::ENTITY_B(), ModelAPI_AttributeRefList::typeId());
  data()->addAttribute(TRANSLATION_LIST_ID(), ModelAPI_AttributeRefList::typeId());
  ModelAPI_Session::get()->validators()->
    registerNotObligatory(getKind(), SketchPlugin_Constraint::ENTITY_A());
  ModelAPI_Session::get()->validators()->
    registerNotObligatory(getKind(), SketchPlugin_Constraint::ENTITY_B());
}

void SketchPlugin_MultiTranslation::execute()
{
  if (!sketch()) {
    // it is possible, that this method is called before this feature has back reference to sketch
    // in this case, the execute is performed after this is done
    return;
  }

  AttributeRefListPtr aTranslationObjectRefs = reflist(TRANSLATION_LIST_ID());
  int aNbCopies = integer(NUMBER_OF_OBJECTS_ID())->value()-1;
  if (aNbCopies <= 0)
    return;

  // Calculate shift vector
  AttributeRefAttrPtr aStartAttr = data()->refattr(START_POINT_ID());
  AttributeRefAttrPtr anEndAttr = data()->refattr(END_POINT_ID());

  if (!aStartAttr || !anEndAttr || !aStartAttr->isInitialized() || !anEndAttr->isInitialized())
    return;

  DataPtr aData = data();
  AttributePoint2DPtr aStart = GeomDataAPI_Point2D::getPoint2D(aData, START_POINT_ID());
  AttributePoint2DPtr aEnd = GeomDataAPI_Point2D::getPoint2D(aData, END_POINT_ID());
  if (!aStart || !aEnd)
    return;

  std::shared_ptr<GeomAPI_XY>
    aShiftVec(new GeomAPI_XY(aEnd->x() - aStart->x(), aEnd->y() - aStart->y()));

  // Wait all objects being created, then send update events
  static Events_ID anUpdateEvent = Events_Loop::eventByName(EVENT_OBJECT_UPDATED);
  bool isUpdateFlushed = Events_Loop::loop()->isFlushed(anUpdateEvent);
  if (isUpdateFlushed)
    Events_Loop::loop()->setFlushed(anUpdateEvent, false);

  // Save the current feature of the document, because new features may appear while executing.
  // In this case, they will become current. But if the number of copies is updated from outside
  // of sketch (e.g. by parameter change), the history line should not hold in sketch.
  keepCurrentFeature();

  AttributeRefListPtr aRefListOfShapes = std::dynamic_pointer_cast<ModelAPI_AttributeRefList>(
      aData->attribute(SketchPlugin_Constraint::ENTITY_A()));
  AttributeRefListPtr aRefListOfTranslated = std::dynamic_pointer_cast<ModelAPI_AttributeRefList>(
      aData->attribute(SketchPlugin_Constraint::ENTITY_B()));
  int aCurrentNbCopies = aRefListOfShapes->size() ?
      aRefListOfTranslated->size() / aRefListOfShapes->size() - 1 : 0;
  std::list<ObjectPtr> anInitialList = aRefListOfShapes->list();
  std::list<ObjectPtr> aTargetList = aRefListOfTranslated->list();
  std::list<ObjectPtr> anAddition;
  std::vector<bool> isUsed(anInitialList.size(), false);
  // collect new items and check the items to remove
  for(int anInd = 0; anInd < aTranslationObjectRefs->size(); anInd++) {
    //std::shared_ptr<ModelAPI_AttributeSelection> aSelect = aTranslationObjectRefs->value(anInd);
    ObjectPtr anObject = aTranslationObjectRefs->object(anInd);
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
      aRefListOfTranslated->remove(*aTargetIter++);
      for (int i = 0; i < aCurrentNbCopies && aTargetIter != aTargetList.end();
           i++, aTargetIter++) {
        aRefListOfTranslated->remove(*aTargetIter);
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
  std::list<ObjectPtr>::iterator anAddIter = anAddition.begin();
  for (; anAddIter != anAddition.end(); anAddIter++) {
    aRefListOfShapes->append(*anAddIter);
    aRefListOfTranslated->append(*anAddIter);
    for (int i = 0; i < aNbCopies; i++) {
      ObjectPtr anObject = copyFeature(*anAddIter);
      aRefListOfTranslated->append(anObject);
    }
  }

  restoreCurrentFeature();

  // send events to update the sub-features by the solver
  if (isUpdateFlushed)
    Events_Loop::loop()->setFlushed(anUpdateEvent, true);
}

AISObjectPtr SketchPlugin_MultiTranslation::getAISObject(AISObjectPtr thePrevious)
{
  if (!sketch())
    return thePrevious;

  AISObjectPtr anAIS = SketcherPrs_Factory::translateConstraint(this, sketch(),
                                                                thePrevious);
  return anAIS;
}

void SketchPlugin_MultiTranslation::erase()
{
  static Events_Loop* aLoop = Events_Loop::loop();
  static Events_ID aRedispEvent = aLoop->eventByName(EVENT_OBJECT_TO_REDISPLAY);

  // Set copy attribute to false on all copied features.
  AttributeRefListPtr aRefListOfShapes = std::dynamic_pointer_cast<ModelAPI_AttributeRefList>(
      data()->attribute(SketchPlugin_Constraint::ENTITY_A()));
  AttributeRefListPtr aRefListOfTranslated = std::dynamic_pointer_cast<ModelAPI_AttributeRefList>(
      data()->attribute(SketchPlugin_Constraint::ENTITY_B()));

  if(aRefListOfShapes.get() && aRefListOfTranslated.get()) {
    for(int anIndex = 0; anIndex < aRefListOfTranslated->size(); anIndex++) {
      ObjectPtr anObject = aRefListOfTranslated->object(anIndex);
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

ObjectPtr SketchPlugin_MultiTranslation::copyFeature(ObjectPtr theObject)
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

void SketchPlugin_MultiTranslation::attributeChanged(const std::string& theID)
{
  if (theID == TRANSLATION_LIST_ID()) {
    AttributeRefListPtr aTranslationObjectRefs = reflist(TRANSLATION_LIST_ID());
    if (aTranslationObjectRefs->size() == 0) {
      int aNbCopies = integer(NUMBER_OF_OBJECTS_ID())->value()-1;
      if (aNbCopies <= 0)
        return;

      DocumentPtr aDoc = document();
      // Clear list of objects
      AttributeRefListPtr aRefListOfTranslated = reflist(SketchPlugin_Constraint::ENTITY_B());
      std::list<ObjectPtr> aTargetList = aRefListOfTranslated->list();
      std::list<ObjectPtr>::iterator aTargetIter = aTargetList.begin();
      while (aTargetIter != aTargetList.end()) {
        aTargetIter++;
        for (int i = 0; i < aNbCopies && aTargetIter != aTargetList.end(); i++, aTargetIter++) {
          aRefListOfTranslated->remove(*aTargetIter);
          FeaturePtr aFeature = ModelAPI_Feature::feature(*aTargetIter);
          if (aFeature)
            aDoc->removeFeature(aFeature);
        }
      }
      aRefListOfTranslated->clear();
      reflist(SketchPlugin_Constraint::ENTITY_A())->clear();
    }
  }
}
