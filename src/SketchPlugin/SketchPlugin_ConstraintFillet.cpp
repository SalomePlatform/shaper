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

#include <SketcherPrs_Factory.h>

#include <Config_PropManager.h>
#include <Events_Loop.h>

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
  data()->addAttribute(SketchPlugin_Constraint::ENTITY_B(), ModelAPI_AttributeRefAttr::typeId());
  data()->addAttribute(SketchPlugin_Constraint::ENTITY_C(), ModelAPI_AttributeRefList::typeId());
  data()->addAttribute(PREVIOUS_VALUE, ModelAPI_AttributeDouble::typeId());
  // initialize attribute not applicable for user
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
  AttributeRefAttrPtr aBaseB = std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(
      aData->attribute(SketchPlugin_Constraint::ENTITY_B()));
  if (!aBaseA->isInitialized() || !aBaseB->isInitialized() ||
      !aBaseA->isObject() || !aBaseB->isObject())
    return;
  // Check the fillet shapes is not initialized yet
  AttributeRefListPtr aRefListOfFillet = std::dynamic_pointer_cast<ModelAPI_AttributeRefList>(
      aData->attribute(SketchPlugin_Constraint::ENTITY_C()));
  bool needNewObjects = aRefListOfFillet->size() == 0;

  // Obtain features for the base objects
  FeaturePtr aFeatureA, aFeatureB;
  aRC = std::dynamic_pointer_cast<ModelAPI_ResultConstruction>(aBaseA->object());
  if (aRC) aFeatureA = aRC->document()->feature(aRC);
  aRC = std::dynamic_pointer_cast<ModelAPI_ResultConstruction>(aBaseB->object());
  if (aRC) aFeatureB = aRC->document()->feature(aRC);
  if (!aFeatureA || !aFeatureB)
    return;

  FeaturePtr aNewFeatureA, aNewFeatureB, aNewArc;
  if (needNewObjects) {
    // Create list of objects composing a fillet
    // copy aFeatureA
    aNewFeatureA = sketch()->addFeature(aFeatureA->getKind());
    aFeatureA->data()->copyTo(aNewFeatureA->data());
    // copy aFeatureB
    aNewFeatureB = sketch()->addFeature(aFeatureB->getKind());
    aFeatureB->data()->copyTo(aNewFeatureB->data());
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
  FeaturePtr aFeature[aNbFeatures] = {aFeatureA, aFeatureB};
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
  calculateFilletCenter(aFeatureA, aFeatureB, aFilletRadius, isStart, aCenter, aTangentPntA, aTangentPntB);
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
    ModelAPI_EventCreator::get()->sendUpdated(aConstraint, anUpdateEvent);
    // 2. Fillet arc radius
    aConstraint = sketch()->addFeature(SketchPlugin_ConstraintRadius::ID());
    aRefAttr = std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(
        aConstraint->attribute(SketchPlugin_Constraint::ENTITY_A()));
    aRefAttr->setObject(aNewArc->lastResult());
    std::dynamic_pointer_cast<ModelAPI_AttributeDouble>(
        aConstraint->attribute(SketchPlugin_Constraint::VALUE()))->setValue(aFilletRadius);
    std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
        aConstraint->attribute(SketchPlugin_Constraint::FLYOUT_VALUE_PNT()))->setValue(
        isStart[0] ? aStartEndPnt[0] : aStartEndPnt[1]);
    aConstraint->execute();
    ModelAPI_EventCreator::get()->sendUpdated(aConstraint, anUpdateEvent);
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
    }
    // make base features auxiliary
    aFeatureA->boolean(SketchPlugin_SketchEntity::AUXILIARY_ID())->setValue(true);
    aFeatureB->boolean(SketchPlugin_SketchEntity::AUXILIARY_ID())->setValue(true);
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

void calculateFilletCenter(FeaturePtr theFeatureA, FeaturePtr theFeatureB,
                           double theRadius, bool theNotInversed[2],
                           std::shared_ptr<GeomAPI_XY>& theCenter,
                           std::shared_ptr<GeomAPI_XY>& theTangentA,
                           std::shared_ptr<GeomAPI_XY>& theTangentB)
{
  if (theFeatureA->getKind() == SketchPlugin_Line::ID() &&
      theFeatureB->getKind() == SketchPlugin_Line::ID()) {
    std::shared_ptr<GeomAPI_XY> aStartA, aEndA, aStartB, aEndB;
    std::shared_ptr<GeomDataAPI_Point2D> aStartPoint, aEndPoint;

    aStartPoint = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
        theFeatureA->attribute(SketchPlugin_Line::START_ID()));
    aEndPoint = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
        theFeatureA->attribute(SketchPlugin_Line::END_ID()));
    aStartA = std::shared_ptr<GeomAPI_XY>(theNotInversed[0] ?
        new GeomAPI_XY(aStartPoint->x(), aStartPoint->y()) :
        new GeomAPI_XY(aEndPoint->x(), aEndPoint->y()));
    aEndA = std::shared_ptr<GeomAPI_XY>(theNotInversed[0] ?
        new GeomAPI_XY(aEndPoint->x(), aEndPoint->y()) :
        new GeomAPI_XY(aStartPoint->x(), aStartPoint->y()));

    aStartPoint = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
        theFeatureB->attribute(SketchPlugin_Line::START_ID()));
    aEndPoint = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
        theFeatureB->attribute(SketchPlugin_Line::END_ID()));
    aStartB = std::shared_ptr<GeomAPI_XY>(theNotInversed[1] ?
        new GeomAPI_XY(aStartPoint->x(), aStartPoint->y()) :
        new GeomAPI_XY(aEndPoint->x(), aEndPoint->y()));
    aEndB = std::shared_ptr<GeomAPI_XY>(theNotInversed[1] ?
        new GeomAPI_XY(aEndPoint->x(), aEndPoint->y()) :
        new GeomAPI_XY(aStartPoint->x(), aStartPoint->y()));

    std::shared_ptr<GeomAPI_Dir2d> aDirA(new GeomAPI_Dir2d(aEndA->decreased(aStartA)));
    std::shared_ptr<GeomAPI_Dir2d> aDirB(new GeomAPI_Dir2d(aEndB->decreased(aStartB)));
    std::shared_ptr<GeomAPI_Dir2d> aDirAT(new GeomAPI_Dir2d(-aDirA->y(), aDirA->x()));
    std::shared_ptr<GeomAPI_Dir2d> aDirBT(new GeomAPI_Dir2d(-aDirB->y(), aDirB->x()));

    // get and filter possible centers
    std::list< std::shared_ptr<GeomAPI_XY> > aSuspectCenters;
    possibleFilletCenterLineLine(aStartA, aDirA, aStartB, aDirB, theRadius, aSuspectCenters);
    double aDot = 0.0;
    std::list< std::shared_ptr<GeomAPI_XY> >::iterator anIt = aSuspectCenters.begin();
    for (; anIt != aSuspectCenters.end(); anIt++) {
      aDot = aDirAT->xy()->dot(aStartA->decreased(*anIt));
      theTangentA = (*anIt)->added(aDirAT->xy()->multiplied(aDot));
      if (theTangentA->decreased(aStartA)->dot(aDirA->xy()) < 0.0)
        continue; // incorrect position
      aDot = aDirBT->xy()->dot(aStartB->decreased(*anIt));
      theTangentB = (*anIt)->added(aDirBT->xy()->multiplied(aDot));
      if (theTangentB->decreased(aStartB)->dot(aDirB->xy()) < 0.0)
        continue; // incorrect position
      // the center is found, stop searching
      theCenter = *anIt;
      return;
    }
  }
}
