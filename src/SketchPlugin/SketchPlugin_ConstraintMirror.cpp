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

#include "SketchPlugin_ConstraintMirror.h"

#include <ModelAPI_AttributeDouble.h>
#include <ModelAPI_Data.h>
#include <ModelAPI_ResultConstruction.h>
#include <ModelAPI_AttributeRefList.h>
#include <ModelAPI_Events.h>
#include <ModelAPI_Session.h>
#include <ModelAPI_Validator.h>
#include <ModelAPI_Tools.h>

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
  data()->addAttribute(SketchPlugin_ConstraintMirror::MIRROR_LIST_ID(),
                       ModelAPI_AttributeRefList::typeId());
  ModelAPI_Session::get()->validators()->
    registerNotObligatory(getKind(), SketchPlugin_Constraint::ENTITY_B());
  ModelAPI_Session::get()->validators()->
    registerNotObligatory(getKind(), SketchPlugin_Constraint::ENTITY_C());
}

void SketchPlugin_ConstraintMirror::execute()
{
  AttributeRefListPtr aMirrorObjectRefs = reflist(SketchPlugin_ConstraintMirror::MIRROR_LIST_ID());

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
      aData->attribute(SketchPlugin_Constraint::ENTITY_B()));
  AttributeRefListPtr aRefListOfMirrored = std::dynamic_pointer_cast<ModelAPI_AttributeRefList>(
      aData->attribute(SketchPlugin_Constraint::ENTITY_C()));
  std::list<ObjectPtr> anInitialList =  aRefListOfShapes->list();
  std::list<ObjectPtr> aMirroredList =  aRefListOfMirrored->list();
  std::vector<bool> isUsed(anInitialList.size(), false);
  // add new items to the list
  for(int anInd = 0; anInd < aMirrorObjectRefs->size(); anInd++) {
    ObjectPtr anObject = aMirrorObjectRefs->object(anInd);
    std::list<ObjectPtr>::const_iterator anIt = anInitialList.begin();
    std::vector<bool>::iterator aUsedIt = isUsed.begin();
    for (; anIt != anInitialList.end(); anIt++, aUsedIt++)
      if (*anIt == anObject) {
        *aUsedIt = true;
        break;
      }
    if (anIt == anInitialList.end())
      aRefListOfShapes->append(anObject);
  }
  // remove unused items
  std::list<ObjectPtr>::iterator anInitIter = anInitialList.begin();
  std::list<ObjectPtr>::iterator aMirrorIter = aMirroredList.begin();
  std::vector<bool>::iterator aUsedIter = isUsed.begin();
  std::set<FeaturePtr> aFeaturesToBeRemoved;
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
          aFeaturesToBeRemoved.insert(aFeature);
      }
    }
    if (anInitIter != anInitialList.end())
      anInitIter++;
    if (aMirrorIter != aMirroredList.end())
      aMirrorIter++;
  }
  ModelAPI_Tools::removeFeaturesAndReferences(aFeaturesToBeRemoved);

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
      FeaturePtr aNewFeature =
        SketchPlugin_Sketch::addUniqueNamedCopiedFeature(aFeatureIn, sketch(), true);
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

  restoreCurrentFeature();

  // send events to update the sub-features by the solver
  if (isUpdateFlushed)
    Events_Loop::loop()->setFlushed(anUpdateEvent, true);
}

AISObjectPtr SketchPlugin_ConstraintMirror::getAISObject(AISObjectPtr thePrevious)
{
  if (!sketch())
    return thePrevious;

  AISObjectPtr anAIS = SketcherPrs_Factory::mirrorConstraint(this, sketch(),
                                                             thePrevious);
  return anAIS;
}

void SketchPlugin_ConstraintMirror::erase()
{
  // Set copy attribute to false on all copied features.
  AttributeRefListPtr aRefListOfMirrored = std::dynamic_pointer_cast<ModelAPI_AttributeRefList>(
      data()->attribute(SketchPlugin_Constraint::ENTITY_C()));

  if(aRefListOfMirrored.get()) {
    static Events_Loop* aLoop = Events_Loop::loop();
    static Events_ID aRedispEvent = aLoop->eventByName(EVENT_OBJECT_TO_REDISPLAY);

    std::list<ObjectPtr> aTargetList = aRefListOfMirrored->list();
    for(std::list<ObjectPtr>::const_iterator aTargetIt =
        aTargetList.cbegin(); aTargetIt != aTargetList.cend(); aTargetIt++) {
      if((*aTargetIt).get()) {
        ResultPtr aRes = std::dynamic_pointer_cast<ModelAPI_Result>(*aTargetIt);
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

void SketchPlugin_ConstraintMirror::attributeChanged(const std::string& theID)
{
  if (theID == MIRROR_LIST_ID()) {
    AttributeRefListPtr aMirrorObjectRefs = reflist(MIRROR_LIST_ID());
    if (aMirrorObjectRefs->size() == 0) {
      DocumentPtr aDoc = document();
      // Clear list of objects
      AttributeRefListPtr aRefListOfMirrored = std::dynamic_pointer_cast<ModelAPI_AttributeRefList>(
          data()->attribute(SketchPlugin_Constraint::ENTITY_C()));
      std::list<ObjectPtr> aTargetList = aRefListOfMirrored->list();
      std::list<ObjectPtr>::iterator aTargetIter = aTargetList.begin();
      for (; aTargetIter != aTargetList.end(); aTargetIter++) {
        FeaturePtr aFeature = ModelAPI_Feature::feature(*aTargetIter);
        if (aFeature)
          aDoc->removeFeature(aFeature);
      }
      aRefListOfMirrored->clear();
      std::dynamic_pointer_cast<ModelAPI_AttributeRefList>(
        data()->attribute(SketchPlugin_Constraint::ENTITY_B()))->clear();
    }
  }
}

