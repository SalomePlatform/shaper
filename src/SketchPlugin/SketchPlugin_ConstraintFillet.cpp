// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:    SketchPlugin_ConstraintFillet.cpp
// Created: 19 Mar 2015
// Author:  Artem ZHIDKOV

#include "SketchPlugin_ConstraintFillet.h"

#include <GeomAPI_Dir2d.h>
#include <GeomAPI_Pnt2d.h>
#include <GeomAPI_XY.h>
#include <GeomDataAPI_Point2D.h>
#include <ModelAPI_AttributeDouble.h>
#include <ModelAPI_AttributeRefAttr.h>
#include <ModelAPI_AttributeRefList.h>
#include <ModelAPI_Data.h>
#include <ModelAPI_Events.h>
#include <ModelAPI_ResultConstruction.h>
#include <ModelAPI_Session.h>
#include <ModelAPI_Validator.h>

#include <SketchPlugin_Arc.h>
#include <SketchPlugin_Line.h>
#include <SketchPlugin_Sketch.h>
#include <SketchPlugin_ConstraintCoincidence.h>
#include <SketchPlugin_ConstraintTangent.h>
#include <SketchPlugin_ConstraintRadius.h>
#include <SketchPlugin_Tools.h>

#include <SketcherPrs_Factory.h>
#include <SketcherPrs_Tools.h>

#include <Config_PropManager.h>
#include <Events_Loop.h>

#define _USE_MATH_DEFINES
#include <math.h>

static const std::string PREVIOUS_VALUE("FilletPreviousRadius");

/// \brief Attract specified point on theNewArc to the attribute of theFeature
static void recalculateAttributes(FeaturePtr theNewArc, const std::string& theNewArcAttribute,
  FeaturePtr theFeature, const std::string& theFeatureAttribute);

/// \brief Calculates center of fillet arc and coordinates of tangency points
static void calculateFilletCenter(FeaturePtr theFeatureA, FeaturePtr theFeatureB,
                                  double theRadius, bool theNotInversed[2],
                                  std::shared_ptr<GeomAPI_XY>& theCenter,
                                  std::shared_ptr<GeomAPI_XY>& theTangentA,
                                  std::shared_ptr<GeomAPI_XY>& theTangentB);

SketchPlugin_ConstraintFillet::SketchPlugin_ConstraintFillet()
{
}

void SketchPlugin_ConstraintFillet::initAttributes()
{
  data()->addAttribute(SketchPlugin_Constraint::VALUE(), ModelAPI_AttributeDouble::typeId());
  data()->addAttribute(SketchPlugin_Constraint::ENTITY_A(), ModelAPI_AttributeRefAttr::typeId());
  data()->addAttribute(SketchPlugin_Constraint::ENTITY_B(), ModelAPI_AttributeRefList::typeId());
  // This attribute used to store base edges
  data()->addAttribute(SketchPlugin_Constraint::ENTITY_C(), ModelAPI_AttributeRefList::typeId());
  data()->addAttribute(PREVIOUS_VALUE, ModelAPI_AttributeDouble::typeId());
  // initialize attribute not applicable for user
  ModelAPI_Session::get()->validators()->registerNotObligatory(getKind(), SketchPlugin_Constraint::ENTITY_B());
  ModelAPI_Session::get()->validators()->registerNotObligatory(getKind(), SketchPlugin_Constraint::ENTITY_C());
  data()->attribute(PREVIOUS_VALUE)->setInitialized();
  std::dynamic_pointer_cast<ModelAPI_AttributeDouble>(data()->attribute(PREVIOUS_VALUE))->setValue(0.0);
}

void SketchPlugin_ConstraintFillet::execute()
{
  // the viewer update should be blocked in order to avoid the temporaty fillet sub-features visualization
  // before they are processed by the solver
  //std::shared_ptr<Events_Message> aMsg = std::shared_ptr<Events_Message>(
  //    new Events_Message(Events_Loop::eventByName(EVENT_UPDATE_VIEWER_BLOCKED)));
  //Events_Loop::loop()->send(aMsg);

  std::shared_ptr<ModelAPI_Data> aData = data();
  ResultConstructionPtr aRC;
  // Check the base objects are initialized
  double aFilletRadius = std::dynamic_pointer_cast<ModelAPI_AttributeDouble>(
      aData->attribute(SketchPlugin_Constraint::VALUE()))->value();
  AttributeRefAttrPtr aBaseA = std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(
      aData->attribute(SketchPlugin_Constraint::ENTITY_A()));
  if (!aBaseA->isInitialized() || aBaseA->isObject()) {
    setError("Bad vertex selected");
    return;
  }

  // Check the fillet shapes is not initialized yet
  AttributeRefListPtr aRefListOfFillet = std::dynamic_pointer_cast<ModelAPI_AttributeRefList>(
      aData->attribute(SketchPlugin_Constraint::ENTITY_B()));
  bool needNewObjects = aRefListOfFillet->size() == 0;

  AttributeRefListPtr aRefListOfBaseLines = std::dynamic_pointer_cast<ModelAPI_AttributeRefList>(
      aData->attribute(SketchPlugin_Constraint::ENTITY_C()));

  // Obtain base features
  FeaturePtr anOldFeatureA, anOldFeatureB;
  std::list<ObjectPtr> aNewFeatList = aRefListOfBaseLines->list();
  std::list<ObjectPtr>::iterator aFeatIt = aNewFeatList.begin();
  anOldFeatureA = ModelAPI_Feature::feature(*aFeatIt++);
  anOldFeatureB = ModelAPI_Feature::feature(*aFeatIt);


  if(!anOldFeatureA.get() || !anOldFeatureB.get()) {
    setError("One of the edges is empty");
    return;
  }

  FeaturePtr aNewFeatureA, aNewFeatureB, aNewArc;
  if (needNewObjects) {
    // Create list of objects composing a fillet
    // copy aFeatureA
    aNewFeatureA = SketchPlugin_Sketch::addUniqueNamedCopiedFeature(anOldFeatureA, sketch());
    // copy aFeatureB
    aNewFeatureB = SketchPlugin_Sketch::addUniqueNamedCopiedFeature(anOldFeatureB, sketch());
    // create filleting arc (it will be attached to the list later)
    aNewArc = sketch()->addFeature(SketchPlugin_Arc::ID());
  } else {
    // Obtain features from the list
    std::list<ObjectPtr> aNewFeatList = aRefListOfFillet->list();
    std::list<ObjectPtr>::iterator aFeatIt = aNewFeatList.begin();
    aNewFeatureA = ModelAPI_Feature::feature(*aFeatIt++);
    aNewFeatureB = ModelAPI_Feature::feature(*aFeatIt++);
    aNewArc = ModelAPI_Feature::feature(*aFeatIt);
  }

  // Calculate arc attributes
  static const int aNbFeatures = 2;
  FeaturePtr aFeature[aNbFeatures] = {anOldFeatureA, anOldFeatureB};
  FeaturePtr aNewFeature[aNbFeatures] = {aNewFeatureA, aNewFeatureB};
  std::shared_ptr<GeomAPI_Dir2d> aTangentDir[aNbFeatures]; // tangent directions of the features in coincident point
  bool isStart[aNbFeatures]; // indicates which point the features share
  std::shared_ptr<GeomAPI_Pnt2d> aStartEndPnt[aNbFeatures * 2]; // first pair of points relate to first feature, second pair -  to second
  std::string aFeatAttributes[aNbFeatures * 2]; // attributes of features
  for (int i = 0; i < aNbFeatures; i++) {
    std::string aStartAttr, aEndAttr;
    if (aNewFeature[i]->getKind() == SketchPlugin_Line::ID()) {
      aStartAttr = SketchPlugin_Line::START_ID();
      aEndAttr = SketchPlugin_Line::END_ID();
    } else if (aNewFeature[i]->getKind() == SketchPlugin_Arc::ID()) {
      aStartAttr = SketchPlugin_Arc::START_ID();
      aEndAttr = SketchPlugin_Arc::END_ID();
    } else { // wrong argument
      aRefListOfFillet->remove(aNewFeatureA);
      aRefListOfFillet->remove(aNewFeatureB);
      aRefListOfFillet->remove(aNewArc);
      aRefListOfBaseLines->clear();
      return;
    }
    aFeatAttributes[2*i] = aStartAttr;
    aStartEndPnt[2*i] = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
        aFeature[i]->attribute(aStartAttr))->pnt();
    aFeatAttributes[2*i+1] = aEndAttr;
    aStartEndPnt[2*i+1] = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
        aFeature[i]->attribute(aEndAttr))->pnt();
  }
  for (int i = 0; i < aNbFeatures; i++) {
    int j = aNbFeatures;
    for (; j < 2 * aNbFeatures; j++)
      if (aStartEndPnt[i]->distance(aStartEndPnt[j]) < 1.e-10) {
        isStart[0] = i==0;
        isStart[1] = j==aNbFeatures;
        break;
      }
    if (j < 2 * aNbFeatures)
      break;
  }
  // tangent directions of the features
  for (int i = 0; i < aNbFeatures; i++) {
    std::shared_ptr<GeomAPI_XY> aDir;
    if (aNewFeature[i]->getKind() == SketchPlugin_Line::ID()) {
      aDir = aStartEndPnt[2*i+1]->xy()->decreased(aStartEndPnt[2*i]->xy());
      if (!isStart[i])
        aDir = aDir->multiplied(-1.0);
    } else if (aNewFeature[i]->getKind() == SketchPlugin_Arc::ID()) {
      std::shared_ptr<GeomAPI_Pnt2d> aCenterPoint =
          std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
          aNewFeature[i]->attribute(SketchPlugin_Arc::CENTER_ID()))->pnt();
      aDir = isStart[i] ? aStartEndPnt[2*i]->xy() : aStartEndPnt[2*i+1]->xy();
      aDir = aDir->decreased(aCenterPoint->xy());

      double x = aDir->x();
      double y = aDir->y();
      aDir->setX(-y);
      aDir->setY(x);
      if (!isStart[i])
        aDir = aDir->multiplied(-1.0);
    }
    aTangentDir[i] = std::shared_ptr<GeomAPI_Dir2d>(new GeomAPI_Dir2d(aDir));
  }

  // Wait all constraints being created, then send update events
  static Events_ID anUpdateEvent = Events_Loop::eventByName(EVENT_OBJECT_UPDATED);
  bool isUpdateFlushed = Events_Loop::loop()->isFlushed(anUpdateEvent);
  if (isUpdateFlushed)
    Events_Loop::loop()->setFlushed(anUpdateEvent, false);

  // By default, the start point of fillet arc is connected to FeatureA,
  // and the end point - to FeatureB. But when the angle between TangentDirA and
  // TangentDirB greater 180 degree, the sequaence of features need to be reversed.
  double cosBA = aTangentDir[0]->cross(aTangentDir[1]); // cos(B-A), where A and B - angles between corresponding tanget direction and the X axis
  bool isReversed = cosBA > 0.0;

  // Calculate fillet arc parameters
  std::shared_ptr<GeomAPI_XY> aCenter, aTangentPntA, aTangentPntB;
  calculateFilletCenter(anOldFeatureA, anOldFeatureB, aFilletRadius, isStart, aCenter, aTangentPntA, aTangentPntB);
  // update features
  std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
      aNewFeatureA->attribute(aFeatAttributes[isStart[0] ? 0 : 1]))->setValue(
      aTangentPntA->x(), aTangentPntA->y());
  aNewFeatureA->execute();
  std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
      aNewFeatureB->attribute(aFeatAttributes[2 + (isStart[1] ? 0 : 1)]))->setValue(
      aTangentPntB->x(), aTangentPntB->y());
  aNewFeatureB->execute();
  // update fillet arc
  std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
      aNewArc->attribute(SketchPlugin_Arc::CENTER_ID()))->setValue(
      aCenter->x(), aCenter->y());
  if (isReversed) {
    std::shared_ptr<GeomAPI_XY> aTmp = aTangentPntA;
    aTangentPntA = aTangentPntB;
    aTangentPntB = aTmp;
  }
  std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
      aNewArc->attribute(SketchPlugin_Arc::START_ID()))->setValue(aTangentPntA->x(), aTangentPntA->y());
  std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
      aNewArc->attribute(SketchPlugin_Arc::END_ID()))->setValue(aTangentPntB->x(), aTangentPntB->y());
  aNewArc->execute();

  if (needNewObjects) {
    // attach new arc to the list
    aRefListOfFillet->append(aNewFeatureA->lastResult());
    aRefListOfFillet->append(aNewFeatureB->lastResult());
    aRefListOfFillet->append(aNewArc->lastResult());

    // attach base lines to the list
    aRefListOfBaseLines->append(anOldFeatureA);
    aRefListOfBaseLines->append(anOldFeatureB);

    myProducedFeatures.push_back(aNewFeatureA);
    myProducedFeatures.push_back(aNewFeatureB);
    myProducedFeatures.push_back(aNewArc);

    // Create list of additional constraints:
    // 1. Coincidence of boundary points of features (copied lines/arcs) and fillet arc
    // 1.1. coincidence
    FeaturePtr aConstraint = sketch()->addFeature(SketchPlugin_ConstraintCoincidence::ID());
    AttributeRefAttrPtr aRefAttr = std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(
        aConstraint->attribute(SketchPlugin_Constraint::ENTITY_A()));
    aRefAttr->setAttr(aNewArc->attribute(SketchPlugin_Arc::START_ID()));
    aRefAttr = std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(
        aConstraint->attribute(SketchPlugin_Constraint::ENTITY_B()));
    int aFeatInd = isReversed ? 1 : 0;
    int anAttrInd = (isReversed ? 2 : 0) + (isStart[isReversed ? 1 : 0] ? 0 : 1);
    aRefAttr->setAttr(aNewFeature[aFeatInd]->attribute(aFeatAttributes[anAttrInd]));
    recalculateAttributes(aNewArc, SketchPlugin_Arc::START_ID(), aNewFeature[aFeatInd], aFeatAttributes[anAttrInd]);
    aConstraint->execute();
    myProducedFeatures.push_back(aConstraint);
    ModelAPI_EventCreator::get()->sendUpdated(aConstraint, anUpdateEvent);
    // 1.2. coincidence
    aConstraint = sketch()->addFeature(SketchPlugin_ConstraintCoincidence::ID());
    aRefAttr = std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(
        aConstraint->attribute(SketchPlugin_Constraint::ENTITY_A()));
    aRefAttr->setAttr(aNewArc->attribute(SketchPlugin_Arc::END_ID()));
    aRefAttr = std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(
        aConstraint->attribute(SketchPlugin_Constraint::ENTITY_B()));
    aFeatInd = isReversed ? 0 : 1;
    anAttrInd = (isReversed ? 0 : 2) + (isStart[isReversed ? 0 : 1] ? 0 : 1);
    aRefAttr->setAttr(aNewFeature[aFeatInd]->attribute(aFeatAttributes[anAttrInd]));
    recalculateAttributes(aNewArc, SketchPlugin_Arc::END_ID(), aNewFeature[aFeatInd], aFeatAttributes[anAttrInd]);
    aConstraint->execute();
    myProducedFeatures.push_back(aConstraint);
    ModelAPI_EventCreator::get()->sendUpdated(aConstraint, anUpdateEvent);
    // 2. Fillet arc radius
    //aConstraint = sketch()->addFeature(SketchPlugin_ConstraintRadius::ID());
    //aRefAttr = std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(
    //    aConstraint->attribute(SketchPlugin_Constraint::ENTITY_A()));
    //aRefAttr->setObject(aNewArc->lastResult());
    //std::dynamic_pointer_cast<ModelAPI_AttributeDouble>(
    //    aConstraint->attribute(SketchPlugin_Constraint::VALUE()))->setValue(aFilletRadius);
    //std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
    //    aConstraint->attribute(SketchPlugin_Constraint::FLYOUT_VALUE_PNT()))->setValue(
    //    isStart[0] ? aStartEndPnt[0] : aStartEndPnt[1]);
    //aConstraint->execute();
    //myProducedFeatures.push_back(aConstraint);
    //ModelAPI_EventCreator::get()->sendUpdated(aConstraint, anUpdateEvent);
    // 3. Tangency of fillet arc and features
    for (int i = 0; i < aNbFeatures; i++) {
      aConstraint = sketch()->addFeature(SketchPlugin_ConstraintTangent::ID());
      aRefAttr = std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(
          aConstraint->attribute(SketchPlugin_Constraint::ENTITY_A()));
      aRefAttr->setObject(aNewArc->lastResult());
      aRefAttr = std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(
          aConstraint->attribute(SketchPlugin_Constraint::ENTITY_B()));
      bool isArc = aNewFeature[i]->getKind() == SketchPlugin_Arc::ID();
      aRefAttr->setObject(isArc ? aNewFeature[i]->lastResult() : aNewFeature[i]->firstResult());
      aConstraint->execute();
      myProducedFeatures.push_back(aConstraint);
      ModelAPI_EventCreator::get()->sendUpdated(aConstraint, anUpdateEvent);
    }
    // 4. Coincidence of free boundaries of base and copied features
    for (int i = 0; i < aNbFeatures; i++) {
      anAttrInd = 2*i + (isStart[i] ? 1 : 0);
      aConstraint = sketch()->addFeature(SketchPlugin_ConstraintCoincidence::ID());
      aRefAttr = std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(
          aConstraint->attribute(SketchPlugin_Constraint::ENTITY_A()));
      aRefAttr->setAttr(aFeature[i]->attribute(aFeatAttributes[anAttrInd]));
      aRefAttr = std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(
          aConstraint->attribute(SketchPlugin_Constraint::ENTITY_B()));
      aRefAttr->setAttr(aNewFeature[i]->attribute(aFeatAttributes[anAttrInd]));
      myProducedFeatures.push_back(aConstraint);
    }
    // 5. Tangent points should be placed on the base features
    for (int i = 0; i < aNbFeatures; i++) {
      anAttrInd = 2*i + (isStart[i] ? 0 : 1);
      aConstraint = sketch()->addFeature(SketchPlugin_ConstraintCoincidence::ID());
      aRefAttr = std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(
          aConstraint->attribute(SketchPlugin_Constraint::ENTITY_A()));
      aRefAttr->setAttr(aNewFeature[i]->attribute(aFeatAttributes[anAttrInd]));
      aRefAttr = std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(
          aConstraint->attribute(SketchPlugin_Constraint::ENTITY_B()));
      aRefAttr->setObject(aFeature[i]->lastResult());
      myProducedFeatures.push_back(aConstraint);
    }
    // make base features auxiliary
    anOldFeatureA->boolean(SketchPlugin_SketchEntity::AUXILIARY_ID())->setValue(true);
    anOldFeatureB->boolean(SketchPlugin_SketchEntity::AUXILIARY_ID())->setValue(true);
    myBaseObjects.clear();
    myBaseObjects.push_back(anOldFeatureA);
    myBaseObjects.push_back(anOldFeatureB);
    // exchange the naming IDs of newly created and old line that become auxiliary
    sketch()->exchangeIDs(anOldFeatureA, aNewFeatureA);
    sketch()->exchangeIDs(anOldFeatureB, aNewFeatureB);
  } else {
    // Update radius value
    int aNbSubs = sketch()->numberOfSubs();
    FeaturePtr aSubFeature;
    for (int aSub = 0; aSub < aNbSubs; aSub++) {
      aSubFeature = sketch()->subFeature(aSub);
      if (aSubFeature->getKind() != SketchPlugin_ConstraintRadius::ID())
        continue;
      AttributeRefAttrPtr aRefAttr = std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(
          aSubFeature->attribute(SketchPlugin_Constraint::ENTITY_A()));
      if (!aRefAttr || !aRefAttr->isObject())
        continue;
      FeaturePtr aFeature = ModelAPI_Feature::feature(aRefAttr->object());
      if (aFeature == aNewArc) {
        AttributeDoublePtr aRadius = std::dynamic_pointer_cast<ModelAPI_AttributeDouble>(
          aSubFeature->attribute(SketchPlugin_Constraint::VALUE()));
        aRadius->setValue(aFilletRadius);
        break;
      }
    }
  }

  // send events to update the sub-features by the solver
  if (isUpdateFlushed)
    Events_Loop::loop()->setFlushed(anUpdateEvent, true);

  // the viewer update should be unblocked in order after the fillet features
  // are processed by the solver
  //aMsg = std::shared_ptr<Events_Message>(
  //              new Events_Message(Events_Loop::eventByName(EVENT_UPDATE_VIEWER_UNBLOCKED)));
  //Events_Loop::loop()->send(aMsg);
}

void SketchPlugin_ConstraintFillet::attributeChanged(const std::string& theID)
{
  if (theID == SketchPlugin_Constraint::ENTITY_A()) {
    // clear the list of fillet entities
    AttributeRefListPtr aRefListOfFillet = std::dynamic_pointer_cast<ModelAPI_AttributeRefList>(
        data()->attribute(SketchPlugin_Constraint::ENTITY_B()));
    aRefListOfFillet->clear();

    // clear the list of base features
    AttributeRefListPtr aRefListOfBaseLines = std::dynamic_pointer_cast<ModelAPI_AttributeRefList>(
        data()->attribute(SketchPlugin_Constraint::ENTITY_C()));
      aRefListOfBaseLines->clear();

    // remove all produced objects and constraints
    DocumentPtr aDoc = sketch()->document();
    std::list<FeaturePtr>::iterator aCIt = myProducedFeatures.begin();
    for (; aCIt != myProducedFeatures.end(); ++aCIt)
      aDoc->removeFeature(*aCIt);
    myProducedFeatures.clear();

    // clear auxiliary flag on initial objects
    for (aCIt = myBaseObjects.begin(); aCIt != myBaseObjects.end(); ++aCIt)
      (*aCIt)->boolean(SketchPlugin_SketchEntity::AUXILIARY_ID())->setValue(false);
    myBaseObjects.clear();

    AttributeRefAttrPtr aBaseA = std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(
        data()->attribute(SketchPlugin_Constraint::ENTITY_A()));
    if(!aBaseA->isInitialized() || aBaseA->isObject()) {
      return;
    }

    AttributePtr anAttrBase = aBaseA->attr();
    const std::set<AttributePtr>& aRefsList = anAttrBase->owner()->data()->refsToMe();
    std::set<AttributePtr>::const_iterator aIt;
    FeaturePtr aCoincident;
    for (aIt = aRefsList.cbegin(); aIt != aRefsList.cend(); ++aIt) {
      std::shared_ptr<ModelAPI_Attribute> aAttr = (*aIt);
      FeaturePtr aConstrFeature = std::dynamic_pointer_cast<ModelAPI_Feature>(aAttr->owner());
      if (aConstrFeature->getKind() == SketchPlugin_ConstraintCoincidence::ID()) {
        AttributeRefAttrPtr anAttrRefA = std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(
          aConstrFeature->attribute(SketchPlugin_ConstraintCoincidence::ENTITY_A()));
        AttributeRefAttrPtr anAttrRefB = std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(
          aConstrFeature->attribute(SketchPlugin_ConstraintCoincidence::ENTITY_B()));
        if(anAttrRefA.get() && !anAttrRefA->isObject()) {
          AttributePtr anAttrA = anAttrRefA->attr();
          if(anAttrBase == anAttrA) {
            aCoincident = aConstrFeature;
            break;
          }
        }
        if(anAttrRefA.get() && !anAttrRefB->isObject()) {
          AttributePtr anAttrB = anAttrRefB->attr();
          if(anAttrBase == anAttrB) {
            aCoincident = aConstrFeature;
            break;
          }
        }
      }
    }

    if(!aCoincident.get()) {
      setError("No coincident edges at selected vertex");
      return;
    }

    std::set<FeaturePtr> aCoinsideLines;
    SketchPlugin_Tools::findCoincidences(aCoincident,
                                         SketchPlugin_ConstraintCoincidence::ENTITY_A(),
                                         aCoinsideLines);
    SketchPlugin_Tools::findCoincidences(aCoincident,
                                         SketchPlugin_ConstraintCoincidence::ENTITY_B(),
                                         aCoinsideLines);

    // Remove auxilary lines
    if(aCoinsideLines.size() > 2) {
      std::set<FeaturePtr> aNewLines;
      for(std::set<FeaturePtr>::iterator anIt = aCoinsideLines.begin(); anIt != aCoinsideLines.end(); ++anIt) {
        if(!(*anIt)->boolean(SketchPlugin_SketchEntity::AUXILIARY_ID())->value()) {
          aNewLines.insert(*anIt);
        }
      }
      aCoinsideLines = aNewLines;
    }


    if(aCoinsideLines.size() != 2) {
      setError("At selected vertex should be two coincident lines");
      return;
    }

    FeaturePtr anOldFeatureA, anOldFeatureB;
    std::set<FeaturePtr>::iterator aLinesIt = aCoinsideLines.begin();
    anOldFeatureA = *aLinesIt++;
    anOldFeatureB = *aLinesIt;
    aRefListOfBaseLines->append(anOldFeatureA);
    aRefListOfBaseLines->append(anOldFeatureB);


    // Set default value equal to 1/3 of the smallest line sharing the point.
    static const int aNbFeatures = 2;
    FeaturePtr aFeature[aNbFeatures] = {anOldFeatureA, anOldFeatureB};
    double aLength = 0;

    double aLengths[aNbFeatures];
    for (int i = 0; i < aNbFeatures; i++) {
      std::shared_ptr<GeomAPI_Pnt2d> aStartPnt = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(aFeature[i]->attribute(
        aFeature[i]->getKind() == SketchPlugin_Line::ID() ? SketchPlugin_Line::START_ID() : SketchPlugin_Arc::START_ID()))->pnt();
      std::shared_ptr<GeomAPI_Pnt2d> anEndPnt = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(aFeature[i]->attribute(
        aFeature[i]->getKind() == SketchPlugin_Line::ID() ? SketchPlugin_Line::END_ID() : SketchPlugin_Arc::END_ID()))->pnt();
      if(aFeature[i]->getKind() == SketchPlugin_Line::ID()) {
        aLengths[i] = aStartPnt->distance(anEndPnt);
      } else {
        std::shared_ptr<GeomAPI_Pnt2d> anArcCenter = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(aFeature[i]->attribute(
          SketchPlugin_Arc::CENTER_ID()))->pnt();
        std::shared_ptr<GeomAPI_Dir2d> aStartDir(new GeomAPI_Dir2d(aStartPnt->xy()->decreased(anArcCenter->xy())));
        std::shared_ptr<GeomAPI_Dir2d> anEndDir(new GeomAPI_Dir2d(anEndPnt->xy()->decreased(anArcCenter->xy())));
        double aRadius = aStartPnt->distance(anArcCenter);
        double anAngle = aStartDir->angle(anEndDir);
        aLengths[i] = aRadius * abs(anAngle);
      }
    }
    aLength = aLengths[0];
    for(int i = 1; i < aNbFeatures; i++) {
      if(aLengths[i] < aLength) aLength = aLengths[i];
    }
    std::dynamic_pointer_cast<ModelAPI_AttributeDouble>(data()->attribute(SketchPlugin_Constraint::VALUE()))->setValue(aLength / 3.0);
  }
}

AISObjectPtr SketchPlugin_ConstraintFillet::getAISObject(AISObjectPtr thePrevious)
{
  if (!sketch())
    return thePrevious;

  AISObjectPtr anAIS = thePrevious;
  /// TODO: Equal constraint presentation should be put here
  return anAIS;
}

bool SketchPlugin_ConstraintFillet::isMacro() const
{
  return true;
}


// =========   Auxiliary functions   =================
void recalculateAttributes(FeaturePtr theNewArc,  const std::string& theNewArcAttribute,
                           FeaturePtr theFeature, const std::string& theFeatureAttribute)
{
  std::shared_ptr<GeomAPI_Pnt2d> anArcPoint = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
      theNewArc->attribute(theNewArcAttribute))->pnt();
  std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
      theFeature->attribute(theFeatureAttribute))->setValue(anArcPoint->x(), anArcPoint->y());
}

/// \brief Find intersections of lines shifted along normal direction
void possibleFilletCenterLineLine(
    std::shared_ptr<GeomAPI_XY> thePointA, std::shared_ptr<GeomAPI_Dir2d> theDirA,
    std::shared_ptr<GeomAPI_XY> thePointB, std::shared_ptr<GeomAPI_Dir2d> theDirB,
    double theRadius, std::list< std::shared_ptr<GeomAPI_XY> >& theCenters)
{
  std::shared_ptr<GeomAPI_Dir2d> aDirAT(new GeomAPI_Dir2d(-theDirA->y(), theDirA->x()));
  std::shared_ptr<GeomAPI_Dir2d> aDirBT(new GeomAPI_Dir2d(-theDirB->y(), theDirB->x()));
  std::shared_ptr<GeomAPI_XY> aPntA, aPntB;
  double aDet = theDirA->cross(theDirB);
  for (double aStepA = -1.0; aStepA <= 1.0; aStepA += 2.0) {
    aPntA = thePointA->added(aDirAT->xy()->multiplied(aStepA * theRadius));
    for (double aStepB = -1.0; aStepB <= 1.0; aStepB += 2.0) {
      aPntB = thePointB->added(aDirBT->xy()->multiplied(aStepB * theRadius));
      double aVX = aDirAT->xy()->dot(aPntA);
      double aVY = aDirBT->xy()->dot(aPntB);
      std::shared_ptr<GeomAPI_XY> aPoint(new GeomAPI_XY(
          (theDirB->x() * aVX - theDirA->x() * aVY) / aDet,
          (theDirB->y() * aVX - theDirA->y() * aVY) / aDet));
      theCenters.push_back(aPoint);
    }
  }
}

/// \brief Find intersections of line shifted along normal direction in both sides
///        and a circle with extended radius
void possibleFilletCenterLineArc(
    std::shared_ptr<GeomAPI_XY> theStartLine, std::shared_ptr<GeomAPI_Dir2d> theDirLine,
    std::shared_ptr<GeomAPI_XY> theCenterArc, double theRadiusArc,
    double theRadius, std::list< std::shared_ptr<GeomAPI_XY> >& theCenters)
{
  std::shared_ptr<GeomAPI_Dir2d> aDirT(new GeomAPI_Dir2d(-theDirLine->y(), theDirLine->x()));
  std::shared_ptr<GeomAPI_XY> aPnt;
  double aDirNorm2 = theDirLine->dot(theDirLine);
  double aRad = 0.0;
  double aDirX = theDirLine->x();
  double aDirX2 = theDirLine->x() * theDirLine->x();
  double aDirY2 = theDirLine->y() * theDirLine->y();
  double aDirXY = theDirLine->x() * theDirLine->y();
  for (double aStepA = -1.0; aStepA <= 1.0; aStepA += 2.0) {
    aPnt = theStartLine->added(aDirT->xy()->multiplied(aStepA * theRadius));
    double aCoeff = aDirT->xy()->dot(aPnt->decreased(theCenterArc));
    double aCoeff2 = aCoeff * aCoeff;
    for (double aStepB = -1.0; aStepB <= 1.0; aStepB += 2.0) {
      aRad = theRadiusArc + aStepB * theRadius;
      double aD = aRad * aRad * aDirNorm2 - aCoeff2;
      if (aD < 0.0)
        continue;
      double aDs = sqrt(aD);
      double x1 = theCenterArc->x() + (aCoeff * aDirT->x() - aDirT->y() * aDs) / aDirNorm2;
      double x2 = theCenterArc->x() + (aCoeff * aDirT->x() + aDirT->y() * aDs) / aDirNorm2;
      double y1 = (aDirX2 * aPnt->y() + aDirY2 * theCenterArc->y() -
          aDirXY * (aPnt->x() - theCenterArc->x()) - theDirLine->y() * aDs) / aDirNorm2;
      double y2 = (aDirX2 * aPnt->y() + aDirY2 * theCenterArc->y() -
          aDirXY * (aPnt->x() - theCenterArc->x()) + theDirLine->y() * aDs) / aDirNorm2;

      std::shared_ptr<GeomAPI_XY> aPoint1(new GeomAPI_XY(x1, y1));
      theCenters.push_back(aPoint1);
      std::shared_ptr<GeomAPI_XY> aPoint2(new GeomAPI_XY(x2, y2));
      theCenters.push_back(aPoint2);
    }
  }
}

/// \brief Find intersections of two circles with extended radii
void possibleFilletCenterArcArc(
    std::shared_ptr<GeomAPI_XY> theCenterA, double theRadiusA,
    std::shared_ptr<GeomAPI_XY> theCenterB, double theRadiusB,
    double theRadius, std::list< std::shared_ptr<GeomAPI_XY> >& theCenters)
{
  std::shared_ptr<GeomAPI_XY> aCenterDir = theCenterB->decreased(theCenterA);
  double aCenterDist2 = aCenterDir->dot(aCenterDir);
  double aCenterDist = sqrt(aCenterDist2);

  double aRadA, aRadB;
  for (double aStepA = -1.0; aStepA <= 1.0; aStepA += 2.0) {
    aRadA = theRadiusA + aStepA * theRadius;
    for (double aStepB = -1.0; aStepB <= 1.0; aStepB += 2.0) {
      aRadB = theRadiusB + aStepB * theRadius;
      if (aRadA + aRadB < aCenterDist || fabs(aRadA - aRadB) > aCenterDist)
        continue; // there is no intersections

      double aMedDist = (aRadA * aRadA - aRadB * aRadB + aCenterDist2) / (2.0 * aCenterDist);
      double aHeight = sqrt(aRadA * aRadA - aMedDist * aMedDist);

      double x1 = theCenterA->x() + (aMedDist * aCenterDir->x() + aCenterDir->y() * aHeight) / aCenterDist;
      double y1 = theCenterA->y() + (aMedDist * aCenterDir->y() - aCenterDir->x() * aHeight) / aCenterDist;

      double x2 = theCenterA->x() + (aMedDist * aCenterDir->x() - aCenterDir->y() * aHeight) / aCenterDist;
      double y2 = theCenterA->y() + (aMedDist * aCenterDir->y() + aCenterDir->x() * aHeight) / aCenterDist;

      std::shared_ptr<GeomAPI_XY> aPoint1(new GeomAPI_XY(x1, y1));
      theCenters.push_back(aPoint1);
      std::shared_ptr<GeomAPI_XY> aPoint2(new GeomAPI_XY(x2, y2));
      theCenters.push_back(aPoint2);
    }
  }
}

void calculateFilletCenter(FeaturePtr theFeatureA, FeaturePtr theFeatureB,
                           double theRadius, bool theNotInversed[2],
                           std::shared_ptr<GeomAPI_XY>& theCenter,
                           std::shared_ptr<GeomAPI_XY>& theTangentA,
                           std::shared_ptr<GeomAPI_XY>& theTangentB)
{
  static const int aNbFeatures = 2;
  FeaturePtr aFeature[aNbFeatures] = {theFeatureA, theFeatureB};
  std::shared_ptr<GeomAPI_XY> aStart[aNbFeatures], aEnd[aNbFeatures], aCenter[aNbFeatures];
  std::shared_ptr<GeomDataAPI_Point2D> aStartPoint, aEndPoint;

  for (int i = 0; i < aNbFeatures; i++) {
    if (aFeature[i]->getKind() == SketchPlugin_Line::ID()) {
      aStartPoint = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
          aFeature[i]->attribute(SketchPlugin_Line::START_ID()));
      aEndPoint = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
          aFeature[i]->attribute(SketchPlugin_Line::END_ID()));
    } else if (aFeature[i]->getKind() == SketchPlugin_Arc::ID()) {
      aStartPoint = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
          aFeature[i]->attribute(SketchPlugin_Arc::START_ID()));
      aEndPoint = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
          aFeature[i]->attribute(SketchPlugin_Arc::END_ID()));
      aCenter[i] = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
          aFeature[i]->attribute(SketchPlugin_Arc::CENTER_ID()))->pnt()->xy();
    } else
      return;
    aStart[i] = std::shared_ptr<GeomAPI_XY>(theNotInversed[i] ?
        new GeomAPI_XY(aStartPoint->x(), aStartPoint->y()) :
        new GeomAPI_XY(aEndPoint->x(), aEndPoint->y()));
    aEnd[i] = std::shared_ptr<GeomAPI_XY>(theNotInversed[i] ?
        new GeomAPI_XY(aEndPoint->x(), aEndPoint->y()) :
        new GeomAPI_XY(aStartPoint->x(), aStartPoint->y()));
  }

  if (theFeatureA->getKind() == SketchPlugin_Line::ID() &&
      theFeatureB->getKind() == SketchPlugin_Line::ID()) {
    std::shared_ptr<GeomAPI_Dir2d> aDir[2];
    std::shared_ptr<GeomAPI_Dir2d> aDirT[2];
    for (int i = 0; i < aNbFeatures; i++) {
      aDir[i] = std::shared_ptr<GeomAPI_Dir2d>(new GeomAPI_Dir2d(aEnd[i]->decreased(aStart[i])));
      aDirT[i] = std::shared_ptr<GeomAPI_Dir2d>(new GeomAPI_Dir2d(-aDir[i]->y(), aDir[i]->x()));
    }

    // get and filter possible centers
    std::list< std::shared_ptr<GeomAPI_XY> > aSuspectCenters;
    possibleFilletCenterLineLine(aStart[0], aDir[0], aStart[1], aDir[1], theRadius, aSuspectCenters);
    double aDot = 0.0;
    std::list< std::shared_ptr<GeomAPI_XY> >::iterator anIt = aSuspectCenters.begin();
    for (; anIt != aSuspectCenters.end(); anIt++) {
      aDot = aDirT[0]->xy()->dot(aStart[0]->decreased(*anIt));
      theTangentA = (*anIt)->added(aDirT[0]->xy()->multiplied(aDot));
      if (theTangentA->decreased(aStart[0])->dot(aDir[0]->xy()) < 0.0)
        continue; // incorrect position
      aDot = aDirT[1]->xy()->dot(aStart[1]->decreased(*anIt));
      theTangentB = (*anIt)->added(aDirT[1]->xy()->multiplied(aDot));
      if (theTangentB->decreased(aStart[1])->dot(aDir[1]->xy()) < 0.0)
        continue; // incorrect position
      // the center is found, stop searching
      theCenter = *anIt;
      return;
    }
  } else if ((theFeatureA->getKind() == SketchPlugin_Arc::ID() &&
      theFeatureB->getKind() == SketchPlugin_Line::ID()) || 
      (theFeatureA->getKind() == SketchPlugin_Line::ID() &&
      theFeatureB->getKind() == SketchPlugin_Arc::ID())) {
    int aLineInd = theFeatureA->getKind() == SketchPlugin_Line::ID() ? 0 : 1;
    double anArcRadius = aStart[1-aLineInd]->distance(aCenter[1-aLineInd]);
    std::shared_ptr<GeomAPI_Dir2d> aDirLine = std::shared_ptr<GeomAPI_Dir2d>(
        new GeomAPI_Dir2d(aEnd[aLineInd]->decreased(aStart[aLineInd])));
    std::shared_ptr<GeomAPI_Dir2d> aDirT = std::shared_ptr<GeomAPI_Dir2d>(
        new GeomAPI_Dir2d(-aDirLine->y(), aDirLine->x()));

    std::shared_ptr<GeomAPI_Dir2d> aStartArcDir = std::shared_ptr<GeomAPI_Dir2d>(
        new GeomAPI_Dir2d(aStart[1-aLineInd]->decreased(aCenter[1-aLineInd])));
    std::shared_ptr<GeomAPI_Dir2d> aEndArcDir = std::shared_ptr<GeomAPI_Dir2d>(
        new GeomAPI_Dir2d(aEnd[1-aLineInd]->decreased(aCenter[1-aLineInd])));
    double anArcAngle = aEndArcDir->angle(aStartArcDir);

    // get and filter possible centers
    std::list< std::shared_ptr<GeomAPI_XY> > aSuspectCenters;
    possibleFilletCenterLineArc(aStart[aLineInd], aDirLine, aCenter[1-aLineInd], anArcRadius, theRadius, aSuspectCenters);
    double aDot = 0.0;
    std::shared_ptr<GeomAPI_XY> aLineTgPoint, anArcTgPoint;
    std::list< std::shared_ptr<GeomAPI_XY> >::iterator anIt = aSuspectCenters.begin();
    for (; anIt != aSuspectCenters.end(); anIt++) {
      aDot = aDirT->xy()->dot(aStart[aLineInd]->decreased(*anIt));
      aLineTgPoint = (*anIt)->added(aDirT->xy()->multiplied(aDot));
      if (aLineTgPoint->decreased(aStart[aLineInd])->dot(aDirLine->xy()) < 0.0)
        continue; // incorrect position
      std::shared_ptr<GeomAPI_Dir2d> aCurDir = std::shared_ptr<GeomAPI_Dir2d>(
          new GeomAPI_Dir2d((*anIt)->decreased(aCenter[1-aLineInd])));
      double aCurAngle = aCurDir->angle(aStartArcDir);
      if (anArcAngle < 0.0) aCurAngle *= -1.0;
      if (aCurAngle < 0.0 || aCurAngle > fabs(anArcAngle))
        continue; // incorrect position
      // the center is found, stop searching
      theCenter = *anIt;
      anArcTgPoint = aCenter[1-aLineInd]->added(aCurDir->xy()->multiplied(anArcRadius));
      if (theFeatureA->getKind() == SketchPlugin_Line::ID()) {
        theTangentA = aLineTgPoint;
        theTangentB = anArcTgPoint;
      } else {
        theTangentA = anArcTgPoint;
        theTangentB = aLineTgPoint;
      }
      return;
    }
  } else if (theFeatureA->getKind() == SketchPlugin_Arc::ID() &&
      theFeatureB->getKind() == SketchPlugin_Arc::ID()) {
    double anArcRadius[aNbFeatures];
    double anArcAngle[aNbFeatures];
    std::shared_ptr<GeomAPI_Dir2d> aStartArcDir[aNbFeatures];
    for (int i = 0; i < aNbFeatures; i++) {
      anArcRadius[i] = aStart[i]->distance(aCenter[i]);
      aStartArcDir[i] = std::shared_ptr<GeomAPI_Dir2d>(
          new GeomAPI_Dir2d(aStart[i]->decreased(aCenter[i])));
      std::shared_ptr<GeomAPI_Dir2d> aEndArcDir = std::shared_ptr<GeomAPI_Dir2d>(
          new GeomAPI_Dir2d(aEnd[i]->decreased(aCenter[i])));
      anArcAngle[i] = aEndArcDir->angle(aStartArcDir[i]);
    }

    // get and filter possible centers
    std::list< std::shared_ptr<GeomAPI_XY> > aSuspectCenters;
    possibleFilletCenterArcArc(aCenter[0], anArcRadius[0], aCenter[1], anArcRadius[1], theRadius, aSuspectCenters);
    double aDot = 0.0;
    std::shared_ptr<GeomAPI_XY> aLineTgPoint, anArcTgPoint;
    std::list< std::shared_ptr<GeomAPI_XY> >::iterator anIt = aSuspectCenters.begin();
    for (; anIt != aSuspectCenters.end(); anIt++) {
      std::shared_ptr<GeomAPI_Dir2d> aCurDir = std::shared_ptr<GeomAPI_Dir2d>(
          new GeomAPI_Dir2d((*anIt)->decreased(aCenter[0])));
      double aCurAngle = aCurDir->angle(aStartArcDir[0]);
      if (anArcAngle[0] < 0.0) aCurAngle *= -1.0;
      if (aCurAngle < 0.0 || aCurAngle > fabs(anArcAngle[0]))
        continue; // incorrect position
      theTangentA = aCenter[0]->added(aCurDir->xy()->multiplied(anArcRadius[0]));

      aCurDir = std::shared_ptr<GeomAPI_Dir2d>(new GeomAPI_Dir2d((*anIt)->decreased(aCenter[1])));
      aCurAngle = aCurDir->angle(aStartArcDir[1]);
      if (anArcAngle[1] < 0.0) aCurAngle *= -1.0;
      if (aCurAngle < 0.0 || aCurAngle > fabs(anArcAngle[1]))
        continue; // incorrect position
      theTangentB = aCenter[1]->added(aCurDir->xy()->multiplied(anArcRadius[1]));

      // the center is found, stop searching
      theCenter = *anIt;
      return;
    }
  }
}
