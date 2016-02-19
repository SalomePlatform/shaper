// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:    SketchPlugin_ConstraintFillet.cpp
// Created: 19 Mar 2015
// Author:  Artem ZHIDKOV

#include "SketchPlugin_ConstraintFillet.h"

#include <GeomAPI_Circ2d.h>
#include <GeomAPI_Dir2d.h>
#include <GeomAPI_Lin2d.h>
#include <GeomAPI_Pnt2d.h>
#include <GeomAPI_XY.h>
#include <GeomDataAPI_Point2D.h>
#include <ModelAPI_AttributeDouble.h>
#include <ModelAPI_AttributeRefList.h>
#include <ModelAPI_AttributeRefAttrList.h>
#include <ModelAPI_Data.h>
#include <ModelAPI_Events.h>
#include <ModelAPI_Session.h>
#include <ModelAPI_Validator.h>

#include <SketchPlugin_Arc.h>
#include <SketchPlugin_Line.h>
#include <SketchPlugin_Point.h>
#include <SketchPlugin_Sketch.h>
#include <SketchPlugin_ConstraintCoincidence.h>
#include <SketchPlugin_ConstraintTangent.h>
#include <SketchPlugin_ConstraintRadius.h>
#include <SketchPlugin_Tools.h>

#include <Events_Loop.h>

#include <math.h>

const double tolerance = 1.e-7;
const double paramTolerance = 1.e-4;

/// \brief Attract specified point on theNewArc to the attribute of theFeature
static void recalculateAttributes(FeaturePtr theNewArc, const std::string& theNewArcAttribute,
  FeaturePtr theFeature, const std::string& theFeatureAttribute);

/// \brief Calculates center of fillet arc and coordinates of tangency points
static void calculateFilletCenter(FeaturePtr theFeatureA, FeaturePtr theFeatureB,
                                  double theRadius, bool theNotInversed[2],
                                  std::shared_ptr<GeomAPI_XY>& theCenter,
                                  std::shared_ptr<GeomAPI_XY>& theTangentA,
                                  std::shared_ptr<GeomAPI_XY>& theTangentB);

/// Get point on 1/3 length of edge from fillet point
static void getPointOnEdge(const FeaturePtr theFeature,
                           const std::shared_ptr<GeomAPI_Pnt2d> theFilletPoint,
                           std::shared_ptr<GeomAPI_Pnt2d>& thePoint);

/// Get distance from point to feature
static double getProjectionDistance(const FeaturePtr theFeature,
                             const std::shared_ptr<GeomAPI_Pnt2d> thePoint);

/// Get coincide edges for fillet
static std::set<FeaturePtr> getCoincides(const FeaturePtr& theConstraintCoincidence);

SketchPlugin_ConstraintFillet::SketchPlugin_ConstraintFillet()
: myListOfPointsChangedInCode(false),
  myRadiusChangedByUser(false),
  myRadiusChangedInCode(true)
{
}

void SketchPlugin_ConstraintFillet::initAttributes()
{
  data()->addAttribute(SketchPlugin_Constraint::VALUE(), ModelAPI_AttributeDouble::typeId());
  data()->addAttribute(SketchPlugin_Constraint::ENTITY_A(), ModelAPI_AttributeRefAttrList::typeId());
}

void SketchPlugin_ConstraintFillet::execute()
{
  std::shared_ptr<ModelAPI_Data> aData = data();

  // Check the base objects are initialized.
  AttributeRefAttrListPtr aPointsRefList = std::dynamic_pointer_cast<ModelAPI_AttributeRefAttrList>(
    aData->attribute(SketchPlugin_Constraint::ENTITY_A()));
  if(!aPointsRefList->isInitialized()) {
    setError("Error: List of points is not initialized.");
    return;
  }

  // Get fillet radius.
  double aFilletRadius = std::dynamic_pointer_cast<ModelAPI_AttributeDouble>(
    aData->attribute(SketchPlugin_Constraint::VALUE()))->value();

  // Check the fillet result edges is not initialized yet.
  bool anIsNeedNewObjects = myResultEdges.size() == 0;

  // Wait all constraints being created, then send update events
  static Events_ID anUpdateEvent = Events_Loop::eventByName(EVENT_OBJECT_UPDATED);
  bool isUpdateFlushed = Events_Loop::loop()->isFlushed(anUpdateEvent);
  if (isUpdateFlushed)
    Events_Loop::loop()->setFlushed(anUpdateEvent, false);

  std::list<FeaturePtr>::iterator aResultEdgesIt = myResultEdges.begin();
  for(int anIndex = 0; anIndex < aPointsRefList->size(); ++anIndex) {
    std::shared_ptr<GeomDataAPI_Point2D> aFilletPoint2d =
      std::dynamic_pointer_cast<GeomDataAPI_Point2D>(aPointsRefList->attribute(anIndex));
    if(!aFilletPoint2d.get()) {
      setError("Error: One of the selected points is empty.");
      return;
    }
    std::shared_ptr<GeomAPI_Pnt2d> aFilletPnt2d = aFilletPoint2d->pnt();

    // Obtain base lines for fillet.
    FeaturePtr aBaseEdgeA, aBaseEdgeB;
    if(myBaseEdges.size() > (unsigned int)(anIndex * 2)) {
      std::list<FeaturePtr>::iterator anIter = myBaseEdges.begin();
      std::advance(anIter, anIndex * 2);
      aBaseEdgeA = *anIter++;
      aBaseEdgeB = *anIter;
    } else {
      // Obtain constraint coincidence for the fillet point.
      FeaturePtr aConstraintCoincidence;
      const std::set<AttributePtr>& aRefsList = aFilletPoint2d->owner()->data()->refsToMe();
      for(std::set<AttributePtr>::const_iterator anIt = aRefsList.cbegin(); anIt != aRefsList.cend(); ++anIt) {
        std::shared_ptr<ModelAPI_Attribute> anAttr = (*anIt);
        FeaturePtr aConstrFeature = std::dynamic_pointer_cast<ModelAPI_Feature>(anAttr->owner());
        if(aConstrFeature->getKind() == SketchPlugin_ConstraintCoincidence::ID()) {
          AttributeRefAttrPtr anAttrRefA = std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(
            aConstrFeature->attribute(SketchPlugin_ConstraintCoincidence::ENTITY_A()));
          AttributeRefAttrPtr anAttrRefB = std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(
            aConstrFeature->attribute(SketchPlugin_ConstraintCoincidence::ENTITY_B()));
          if(anAttrRefA.get() && !anAttrRefA->isObject()) {
            AttributePtr anAttrA = anAttrRefA->attr();
            if(aFilletPoint2d == anAttrA) {
              aConstraintCoincidence = aConstrFeature;
              break;
            }
          }
          if(anAttrRefB.get() && !anAttrRefB->isObject()) {
            AttributePtr anAttrB = anAttrRefB->attr();
            if(aFilletPoint2d == anAttrB) {
              aConstraintCoincidence = aConstrFeature;
              break;
            }
          }
        }
      }

      if(!aConstraintCoincidence.get()) {
        setError("Error: No coincident edges at one of the selected points.");
        return;
      }

      // Get coincide edges.
      std::set<FeaturePtr> aCoincides = getCoincides(aConstraintCoincidence);
      if(aCoincides.size() != 2) {
        setError("Error: One of the selected points does not have two suitable edges for fillet.");
        return;
      }

      std::set<FeaturePtr>::iterator aLinesIt = aCoincides.begin();
      aBaseEdgeA = *aLinesIt++;
      aBaseEdgeB = *aLinesIt;
    }

    if(!aBaseEdgeA.get() || !aBaseEdgeB.get()) {
      setError("Error: One of the base edges is empty.");
      return;
    }

    // Create new edges and arc if needed.
    FeaturePtr aResultEdgeA, aResultEdgeB, aResultArc;
    if(anIsNeedNewObjects) {
      // Copy edges and create arc.
      aResultEdgeA = SketchPlugin_Sketch::addUniqueNamedCopiedFeature(aBaseEdgeA, sketch());
      aResultEdgeB = SketchPlugin_Sketch::addUniqueNamedCopiedFeature(aBaseEdgeB, sketch());
      aResultArc = sketch()->addFeature(SketchPlugin_Arc::ID());
    } else {
      // Obtain features from the list.
      aResultEdgeA = *aResultEdgesIt++;
      aResultEdgeB = *aResultEdgesIt++;
      aResultArc = *aResultEdgesIt++;
    }

    // Calculate arc attributes
    static const int aNbFeatures = 2;
    FeaturePtr aBaseFeatures[aNbFeatures] = {aBaseEdgeA, aBaseEdgeB};
    FeaturePtr aResultFeatures[aNbFeatures] = {aResultEdgeA, aResultEdgeB};
    std::shared_ptr<GeomAPI_Dir2d> aTangentDir[aNbFeatures]; // tangent directions of the features in coincident point
    bool isStart[aNbFeatures]; // indicates which point the features share
    std::shared_ptr<GeomAPI_Pnt2d> aStartEndPnt[aNbFeatures * 2]; // first pair of points relate to first feature, second pair -  to second
    std::string aFeatAttributes[aNbFeatures * 2]; // attributes of features
    for (int i = 0; i < aNbFeatures; i++) {
      std::string aStartAttr, aEndAttr;
      if (aResultFeatures[i]->getKind() == SketchPlugin_Line::ID()) {
        aStartAttr = SketchPlugin_Line::START_ID();
        aEndAttr = SketchPlugin_Line::END_ID();
      } else if (aResultFeatures[i]->getKind() == SketchPlugin_Arc::ID()) {
        aStartAttr = SketchPlugin_Arc::START_ID();
        aEndAttr = SketchPlugin_Arc::END_ID();
      } else { // wrong argument
        myResultEdges.clear();
        setError("Error: One of the points has wrong coincide feature");
        return;
      }
      aFeatAttributes[2*i] = aStartAttr;
      aStartEndPnt[2*i] = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
        aBaseFeatures[i]->attribute(aStartAttr))->pnt();
      aFeatAttributes[2*i+1] = aEndAttr;
      aStartEndPnt[2*i+1] = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
        aBaseFeatures[i]->attribute(aEndAttr))->pnt();
    }
    for (int aFeatInd = 0; aFeatInd < aNbFeatures; aFeatInd++) {
      for (int j = 0; j < 2; j++) // loop on start-end of each feature
        if (aStartEndPnt[aFeatInd * aNbFeatures + j]->distance(aFilletPnt2d) < 1.e-10) {
          isStart[aFeatInd] = (j==0);
          break;
        }
    }
    // tangent directions of the features
    for (int i = 0; i < aNbFeatures; i++) {
      std::shared_ptr<GeomAPI_XY> aDir;
      if (aResultFeatures[i]->getKind() == SketchPlugin_Line::ID()) {
        aDir = aStartEndPnt[2*i+1]->xy()->decreased(aStartEndPnt[2*i]->xy());
        if (!isStart[i])
          aDir = aDir->multiplied(-1.0);
      } else if (aResultFeatures[i]->getKind() == SketchPlugin_Arc::ID()) {
        std::shared_ptr<GeomAPI_Pnt2d> aCenterPoint = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
          aResultFeatures[i]->attribute(SketchPlugin_Arc::CENTER_ID()))->pnt();
        aDir = isStart[i] ? aStartEndPnt[2*i]->xy() : aStartEndPnt[2*i+1]->xy();
        aDir = aDir->decreased(aCenterPoint->xy());

        double x = aDir->x();
        double y = aDir->y();
        aDir->setX(-y);
        aDir->setY(x);
        if (isStart[i] == std::dynamic_pointer_cast<SketchPlugin_Arc>(aBaseFeatures[i])->isReversed())
          aDir = aDir->multiplied(-1.0);
      }
      aTangentDir[i] = std::shared_ptr<GeomAPI_Dir2d>(new GeomAPI_Dir2d(aDir));
    }

    // By default, the start point of fillet arc is connected to FeatureA,
    // and the end point - to FeatureB. But when the angle between TangentDirA and
    // TangentDirB greater 180 degree, the sequaence of features need to be reversed.
    double cosBA = aTangentDir[0]->cross(aTangentDir[1]); // cos(B-A), where A and B - angles between corresponding tanget direction and the X axis
    bool isReversed = cosBA > 0.0;

    // Calculate fillet arc parameters
    std::shared_ptr<GeomAPI_XY> aCenter, aTangentPntA, aTangentPntB;
    calculateFilletCenter(aBaseEdgeA, aBaseEdgeB, aFilletRadius, isStart, aCenter, aTangentPntA, aTangentPntB);
    if(!aCenter.get() || !aTangentPntA.get() || !aTangentPntB.get()) {
      setError("Can not create fillet with the specified parameters.");
      return;
    }
    // update features
    std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
      aResultEdgeA->attribute(aFeatAttributes[isStart[0] ? 0 : 1]))->setValue(aTangentPntA->x(), aTangentPntA->y());
    aResultEdgeA->execute();
    std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
      aResultEdgeB->attribute(aFeatAttributes[2 + (isStart[1] ? 0 : 1)]))->setValue(aTangentPntB->x(), aTangentPntB->y());
    aResultEdgeB->execute();
    // update fillet arc: make the arc correct for sure, so, it is not needed to process the "attribute updated"
    // by arc; moreover, it may cause cyclicity in hte mechanism of updater
    aResultArc->data()->blockSendAttributeUpdated(true);
    std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
      aResultArc->attribute(SketchPlugin_Arc::CENTER_ID()))->setValue(aCenter->x(), aCenter->y());
    if(isReversed) {
      std::shared_ptr<GeomAPI_XY> aTmp = aTangentPntA;
      aTangentPntA = aTangentPntB;
      aTangentPntB = aTmp;
    }
    std::shared_ptr<GeomDataAPI_Point2D> aStartPoint = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
      aResultArc->attribute(SketchPlugin_Arc::START_ID()));
    std::shared_ptr<GeomDataAPI_Point2D> aEndPoint = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
      aResultArc->attribute(SketchPlugin_Arc::END_ID()));
    if(aStartPoint->isInitialized() && aEndPoint->isInitialized() &&
      (aStartPoint->pnt()->xy()->distance(aTangentPntA) > tolerance ||
       aEndPoint->pnt()->xy()->distance(aTangentPntB) > tolerance)) {
      std::dynamic_pointer_cast<SketchPlugin_Arc>(aResultArc)->setReversed(false);
    }
    aStartPoint->setValue(aTangentPntA->x(), aTangentPntA->y());
    aEndPoint->setValue(aTangentPntB->x(), aTangentPntB->y());
    aResultArc->data()->blockSendAttributeUpdated(false);
    aResultArc->execute();

    if(anIsNeedNewObjects) {
      // attach new arc to the list
      myResultEdges.push_back(aResultEdgeA);
      myResultEdges.push_back(aResultEdgeB);
      myResultEdges.push_back(aResultArc);

      myProducedFeatures.push_back(aResultEdgeA);
      myProducedFeatures.push_back(aResultEdgeB);
      myProducedFeatures.push_back(aResultArc);

      // Create list of additional constraints:
      // 1. Coincidence of boundary points of features (copied lines/arcs) and fillet arc
      // 1.1. coincidence
      FeaturePtr aConstraint = sketch()->addFeature(SketchPlugin_ConstraintCoincidence::ID());
      AttributeRefAttrPtr aRefAttr = std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(
          aConstraint->attribute(SketchPlugin_Constraint::ENTITY_A()));
      aRefAttr->setAttr(aResultArc->attribute(SketchPlugin_Arc::START_ID()));
      aRefAttr = std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(
          aConstraint->attribute(SketchPlugin_Constraint::ENTITY_B()));
      int aFeatInd = isReversed ? 1 : 0;
      int anAttrInd = (isReversed ? 2 : 0) + (isStart[isReversed ? 1 : 0] ? 0 : 1);
      aRefAttr->setAttr(aResultFeatures[aFeatInd]->attribute(aFeatAttributes[anAttrInd]));
      recalculateAttributes(aResultArc, SketchPlugin_Arc::START_ID(), aResultFeatures[aFeatInd], aFeatAttributes[anAttrInd]);
      aConstraint->execute();
      myProducedFeatures.push_back(aConstraint);
      ModelAPI_EventCreator::get()->sendUpdated(aConstraint, anUpdateEvent);
      // 1.2. coincidence
      aConstraint = sketch()->addFeature(SketchPlugin_ConstraintCoincidence::ID());
      aRefAttr = std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(
          aConstraint->attribute(SketchPlugin_Constraint::ENTITY_A()));
      aRefAttr->setAttr(aResultArc->attribute(SketchPlugin_Arc::END_ID()));
      aRefAttr = std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(
          aConstraint->attribute(SketchPlugin_Constraint::ENTITY_B()));
      aFeatInd = isReversed ? 0 : 1;
      anAttrInd = (isReversed ? 0 : 2) + (isStart[isReversed ? 0 : 1] ? 0 : 1);
      aRefAttr->setAttr(aResultFeatures[aFeatInd]->attribute(aFeatAttributes[anAttrInd]));
      recalculateAttributes(aResultArc, SketchPlugin_Arc::END_ID(), aResultFeatures[aFeatInd], aFeatAttributes[anAttrInd]);
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
        aRefAttr->setObject(aResultArc->lastResult());
        aRefAttr = std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(
            aConstraint->attribute(SketchPlugin_Constraint::ENTITY_B()));
        bool isArc = aResultFeatures[i]->getKind() == SketchPlugin_Arc::ID();
        aRefAttr->setObject(isArc ? aResultFeatures[i]->lastResult() : aResultFeatures[i]->firstResult());
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
        aRefAttr->setAttr(aBaseFeatures[i]->attribute(aFeatAttributes[anAttrInd]));
        aRefAttr = std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(
            aConstraint->attribute(SketchPlugin_Constraint::ENTITY_B()));
        aRefAttr->setAttr(aResultFeatures[i]->attribute(aFeatAttributes[anAttrInd]));
        myProducedFeatures.push_back(aConstraint);
      }
      // 4.1. Additional tangency constraints when the fillet is based on arcs.
      //      It is used to verify the created arc will be placed on a source.
      for (int i = 0; i < aNbFeatures; ++i) {
        if (aResultFeatures[i]->getKind() != SketchPlugin_Arc::ID())
          continue;
        aConstraint = sketch()->addFeature(SketchPlugin_ConstraintTangent::ID());
        aRefAttr = std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(
            aConstraint->attribute(SketchPlugin_Constraint::ENTITY_A()));
        aRefAttr->setObject(aBaseFeatures[i]->lastResult());
        aRefAttr = std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(
            aConstraint->attribute(SketchPlugin_Constraint::ENTITY_B()));
        aRefAttr->setObject(aResultFeatures[i]->lastResult());
        aConstraint->execute();
        myProducedFeatures.push_back(aConstraint);
        ModelAPI_EventCreator::get()->sendUpdated(aConstraint, anUpdateEvent);
      }
      // 5. Tangent points should be placed on the base features
      for (int i = 0; i < aNbFeatures; i++) {
        anAttrInd = 2*i + (isStart[i] ? 0 : 1);
        aConstraint = sketch()->addFeature(SketchPlugin_ConstraintCoincidence::ID());
        aRefAttr = std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(
            aConstraint->attribute(SketchPlugin_Constraint::ENTITY_A()));
        aRefAttr->setAttr(aResultFeatures[i]->attribute(aFeatAttributes[anAttrInd]));
        aRefAttr = std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(
            aConstraint->attribute(SketchPlugin_Constraint::ENTITY_B()));
        aRefAttr->setObject(aBaseFeatures[i]->lastResult());
        myProducedFeatures.push_back(aConstraint);
      }
      // make base features auxiliary
      aBaseEdgeA->boolean(SketchPlugin_SketchEntity::AUXILIARY_ID())->setValue(true);
      aBaseEdgeB->boolean(SketchPlugin_SketchEntity::AUXILIARY_ID())->setValue(true);

      myBaseEdges.push_back(aBaseEdgeA);
      myBaseEdges.push_back(aBaseEdgeB);
      // exchange the naming IDs of newly created and old line that become auxiliary
      sketch()->exchangeIDs(aBaseEdgeA, aResultEdgeA);
      sketch()->exchangeIDs(aBaseEdgeB, aResultEdgeB);
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
        if (aFeature == aResultArc) {
          AttributeDoublePtr aRadius = std::dynamic_pointer_cast<ModelAPI_AttributeDouble>(
            aSubFeature->attribute(SketchPlugin_Constraint::VALUE()));
          aRadius->setValue(aFilletRadius);
          break;
        }
      }
    }
  }

  // Send events to update the sub-features by the solver.
  if(isUpdateFlushed) {
    Events_Loop::loop()->setFlushed(anUpdateEvent, true);
  }
}

void SketchPlugin_ConstraintFillet::attributeChanged(const std::string& theID)
{
  if(theID == SketchPlugin_Constraint::ENTITY_A()) {
    if(myListOfPointsChangedInCode) {
      return;
    }

    // Clear the list of fillet entities.
    myResultEdges.clear();

    // Clear the list of base points.
    myBasePoints.clear();

    // Clear auxiliary flag on initial objects.
    std::list<FeaturePtr>::const_iterator aFeatureIt;
    for(aFeatureIt = myBaseEdges.cbegin(); aFeatureIt != myBaseEdges.cend(); ++aFeatureIt) {
      (*aFeatureIt)->boolean(SketchPlugin_SketchEntity::AUXILIARY_ID())->setValue(false);
    }
    myBaseEdges.clear();

    // Remove all produced objects and constraints.
    DocumentPtr aDoc = sketch()->document();
    for(aFeatureIt = myProducedFeatures.cbegin(); aFeatureIt != myProducedFeatures.cend(); ++aFeatureIt) {
      aDoc->removeFeature(*aFeatureIt);
    }
    myProducedFeatures.clear();

    // Get list of points for fillets and current radius.
    AttributeRefAttrListPtr aRefListOfFilletPoints = std::dynamic_pointer_cast<ModelAPI_AttributeRefAttrList>(
      data()->attribute(SketchPlugin_Constraint::ENTITY_A()));
    AttributeDoublePtr aRadiusAttribute = real(SketchPlugin_Constraint::VALUE());
    int aListSize = aRefListOfFilletPoints->size();
    if(aListSize == 0 && !myRadiusChangedByUser) {
      // If list is empty just reset radius to zero (if it was not changed by user).
      myRadiusChangedInCode = true;
      aRadiusAttribute->setValue(0);
      myRadiusChangedInCode = false;
      return;
    }

    // Iterate over points to get base lines an calculate radius for fillets.
    double aMinimumRadius = 0;
    std::list<std::pair<ObjectPtr, AttributePtr>> aSelectedPointsList = aRefListOfFilletPoints->list();
    std::list<std::pair<ObjectPtr, AttributePtr>>::iterator anIter = aSelectedPointsList.begin();
    std::set<AttributePtr> aBasePoints;
    for(int anIndex = 0; anIndex < aListSize; anIndex++, anIter++) {
      AttributePtr aFilletPointAttr = (*anIter).second;
      std::shared_ptr<GeomDataAPI_Point2D> aFilletPoint2D =
        std::dynamic_pointer_cast<GeomDataAPI_Point2D>(aFilletPointAttr);
      if(!aFilletPoint2D.get()) {
        setError("Error: One of the selected points is invalid.");
        return;
      }

      // If point or coincident point is already in list remove it from attribute.
      if(aBasePoints.find(aFilletPointAttr) != aBasePoints.end()) {
        myListOfPointsChangedInCode = true;
        aRefListOfFilletPoints->remove(aFilletPointAttr);
        myListOfPointsChangedInCode = false;
        continue;
      }

      // Obtain constraint coincidence for the fillet point.
      FeaturePtr aConstraintCoincidence;
      const std::set<AttributePtr>& aRefsList = aFilletPointAttr->owner()->data()->refsToMe();
      for(std::set<AttributePtr>::const_iterator anIt = aRefsList.cbegin(); anIt != aRefsList.cend(); ++anIt) {
        std::shared_ptr<ModelAPI_Attribute> anAttr = (*anIt);
        FeaturePtr aConstrFeature = std::dynamic_pointer_cast<ModelAPI_Feature>(anAttr->owner());
        if(aConstrFeature->getKind() == SketchPlugin_ConstraintCoincidence::ID()) {
          AttributeRefAttrPtr anAttrRefA = std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(
            aConstrFeature->attribute(SketchPlugin_ConstraintCoincidence::ENTITY_A()));
          AttributeRefAttrPtr anAttrRefB = std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(
            aConstrFeature->attribute(SketchPlugin_ConstraintCoincidence::ENTITY_B()));
          if(anAttrRefA.get()) {
            AttributePtr anAttrA = anAttrRefA->attr();
            if(aFilletPointAttr == anAttrA) {
              aConstraintCoincidence = aConstrFeature;
              break;
            }
          }
          if(anAttrRefB.get()) {
            AttributePtr anAttrB = anAttrRefB->attr();
            if(aFilletPointAttr == anAttrB) {
              aConstraintCoincidence = aConstrFeature;
              break;
            }
          }
        }
      }

      if(!aConstraintCoincidence.get()) {
        setError("Error: No coincident edges at one of the selected points.");
        return;
      }

      // Get coincides from constraint.
      std::set<FeaturePtr> aCoincides;


      SketchPlugin_Tools::findCoincidences(aConstraintCoincidence,
                                           SketchPlugin_ConstraintCoincidence::ENTITY_A(),
                                           aCoincides);
      SketchPlugin_Tools::findCoincidences(aConstraintCoincidence,
                                           SketchPlugin_ConstraintCoincidence::ENTITY_B(),
                                           aCoincides);

      // Remove points from set of coincides. Also get all attributes which is equal to this point to exclude it.
      std::shared_ptr<GeomAPI_Pnt2d> aFilletPnt2d = aFilletPoint2D->pnt();
      std::set<FeaturePtr> aNewSetOfCoincides;
      for(std::set<FeaturePtr>::iterator anIt = aCoincides.begin(); anIt != aCoincides.end(); ++anIt) {
        std::string aFeatureKind = (*anIt)->getKind();
        if(aFeatureKind == SketchPlugin_Point::ID()) {
          AttributePtr anAttr = (*anIt)->attribute(SketchPlugin_Point::COORD_ID());
          std::shared_ptr<GeomDataAPI_Point2D> aPoint2D =
            std::dynamic_pointer_cast<GeomDataAPI_Point2D>(anAttr);
          if(aPoint2D.get() && aFilletPnt2d->isEqual(aPoint2D->pnt())) {
            aBasePoints.insert(anAttr);
          }
        } else if(aFeatureKind == SketchPlugin_Line::ID()) {
          AttributePtr anAttrStart = (*anIt)->attribute(SketchPlugin_Line::START_ID());
          std::shared_ptr<GeomDataAPI_Point2D> aPointStart2D =
            std::dynamic_pointer_cast<GeomDataAPI_Point2D>(anAttrStart);
          if(aPointStart2D.get() && aFilletPnt2d->isEqual(aPointStart2D->pnt())) {
            aBasePoints.insert(anAttrStart);
          }
          AttributePtr anAttrEnd = (*anIt)->attribute(SketchPlugin_Line::END_ID());
          std::shared_ptr<GeomDataAPI_Point2D> aPointEnd2D =
            std::dynamic_pointer_cast<GeomDataAPI_Point2D>(anAttrEnd);
          if(aPointEnd2D.get() && aFilletPnt2d->isEqual(aPointEnd2D->pnt())) {
            aBasePoints.insert(anAttrEnd);
          }
          aNewSetOfCoincides.insert(*anIt);
        } else if(aFeatureKind == SketchPlugin_Arc::ID() ) {
          AttributePtr anAttrStart = (*anIt)->attribute(SketchPlugin_Arc::START_ID());
          std::shared_ptr<GeomDataAPI_Point2D> aPointStart2D =
            std::dynamic_pointer_cast<GeomDataAPI_Point2D>(anAttrStart);
          if(aPointStart2D.get() && aFilletPnt2d->isEqual(aPointStart2D->pnt())) {
            aBasePoints.insert(anAttrStart);
          }
          AttributePtr anAttrEnd = (*anIt)->attribute(SketchPlugin_Arc::END_ID());
          std::shared_ptr<GeomDataAPI_Point2D> aPointEnd2D =
            std::dynamic_pointer_cast<GeomDataAPI_Point2D>(anAttrEnd);
          if(aPointEnd2D.get() && aFilletPnt2d->isEqual(aPointEnd2D->pnt())) {
            aBasePoints.insert(anAttrEnd);
          }
          aNewSetOfCoincides.insert(*anIt);
        }
      }
      aCoincides = aNewSetOfCoincides;

      // If we still have more than two coincides remove auxilary entities from set of coincides.
      if(aCoincides.size() > 2) {
        aNewSetOfCoincides.clear();
        for(std::set<FeaturePtr>::iterator anIt = aCoincides.begin(); anIt != aCoincides.end(); ++anIt) {
          if(!(*anIt)->boolean(SketchPlugin_SketchEntity::AUXILIARY_ID())->value()) {
            aNewSetOfCoincides.insert(*anIt);
          }
        }
        aCoincides = aNewSetOfCoincides;
      }

      if(aCoincides.size() != 2) {
        setError("Error: One of the selected points does not have two suitable edges for fillet.");
        return;
      }

      // Store base point for fillet.
      aBasePoints.insert(aFilletPointAttr);
      myBasePoints.push_back(aFilletPointAttr);

      // Get base lines for fillet.
      FeaturePtr anOldFeatureA, anOldFeatureB;
      std::set<FeaturePtr>::iterator aLinesIt = aCoincides.begin();
      anOldFeatureA = *aLinesIt++;
      anOldFeatureB = *aLinesIt;

      // Getting radius value if it was not changed by user.
      if(!myRadiusChangedByUser) {
        // Getting points located at 1/3 of edge length from fillet point.
        std::shared_ptr<GeomAPI_Pnt2d> aFilletPnt2d = aFilletPoint2D->pnt();
        std::shared_ptr<GeomAPI_Pnt2d> aPntA, aPntB;
        getPointOnEdge(anOldFeatureA, aFilletPnt2d, aPntA);
        getPointOnEdge(anOldFeatureB, aFilletPnt2d, aPntB);

        /// Getting distances.
        double aDistanceA = getProjectionDistance(anOldFeatureB, aPntA);
        double aDistanceB = getProjectionDistance(anOldFeatureA, aPntB);
        double aRadius = aDistanceA < aDistanceB ? aDistanceA / 2.0 : aDistanceB / 2.0;
        aMinimumRadius = aMinimumRadius == 0 ? aRadius : aRadius < aMinimumRadius ? aRadius : aMinimumRadius;
      }
    }

    // Set new default radius if it was not changed by user.
    if(!myRadiusChangedByUser) {
      myRadiusChangedInCode = true;
      aRadiusAttribute->setValue(aMinimumRadius);
      myRadiusChangedInCode = false;
    }

  } else if(theID == SketchPlugin_Constraint::VALUE()) {
    if(!myRadiusChangedInCode) {
      myRadiusChangedByUser = true;
    }
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

    // get possible centers and filter them
    std::list< std::shared_ptr<GeomAPI_XY> > aSuspectCenters;
    possibleFilletCenterLineArc(aStart[aLineInd], aDirLine, aCenter[1-aLineInd], anArcRadius, theRadius, aSuspectCenters);
    double aDot = 0.0;
    // the line is forward into the arc
    double innerArc = aCenter[1-aLineInd]->decreased(aStart[aLineInd])->dot(aDirLine->xy());
    std::shared_ptr<GeomAPI_XY> aLineTgPoint, anArcTgPoint;
    // The possible centers are ranged by their positions.
    // If the point is not satisfy one of criteria, the weight is decreased with penalty.
    int aBestWeight = 0;
    std::list< std::shared_ptr<GeomAPI_XY> >::iterator anIt = aSuspectCenters.begin();
    for (; anIt != aSuspectCenters.end(); anIt++) {
      int aWeight = 2;
      aDot = aDirT->xy()->dot(aStart[aLineInd]->decreased(*anIt));
      aLineTgPoint = (*anIt)->added(aDirT->xy()->multiplied(aDot));
      // Check the point is placed on the correct arc (penalty if false)
      if (aCenter[1-aLineInd]->distance(*anIt) * innerArc > anArcRadius * innerArc)
        aWeight -= 1;
      std::shared_ptr<GeomAPI_Dir2d> aCurDir = std::shared_ptr<GeomAPI_Dir2d>(
          new GeomAPI_Dir2d((*anIt)->decreased(aCenter[1-aLineInd])));
      double aCurAngle = aCurDir->angle(aStartArcDir);
      if (anArcAngle < 0.0) aCurAngle *= -1.0;
      if (aCurAngle < 0.0 || aCurAngle > fabs(anArcAngle))
        continue;
      if (aWeight > aBestWeight)
        aBestWeight = aWeight;
      else if (aWeight < aBestWeight ||
               aStart[aLineInd]->distance(*anIt) >
               aStart[aLineInd]->distance(theCenter)) // <-- take closer point
        continue;
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
      //return;
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

void getPointOnEdge(const FeaturePtr theFeature,
                    const std::shared_ptr<GeomAPI_Pnt2d> theFilletPoint,
                    std::shared_ptr<GeomAPI_Pnt2d>& thePoint) {
  if(theFeature->getKind() == SketchPlugin_Line::ID()) {
    std::shared_ptr<GeomAPI_Pnt2d> aPntStart = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
      theFeature->attribute(SketchPlugin_Line::START_ID()))->pnt();
    std::shared_ptr<GeomAPI_Pnt2d> aPntEnd = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
      theFeature->attribute(SketchPlugin_Line::END_ID()))->pnt();
    if(aPntStart->distance(theFilletPoint) > 1.e-7) {
      aPntStart = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
        theFeature->attribute(SketchPlugin_Line::END_ID()))->pnt();
      aPntEnd = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
        theFeature->attribute(SketchPlugin_Line::START_ID()))->pnt();
    }
    thePoint.reset( new GeomAPI_Pnt2d(aPntStart->xy()->added( aPntEnd->xy()->decreased( aPntStart->xy() )->multiplied(1.0 / 3.0) ) ) );
  } else {
    std::shared_ptr<GeomAPI_Pnt2d> aPntTemp;
    std::shared_ptr<GeomAPI_Pnt2d> aPntStart = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
      theFeature->attribute(SketchPlugin_Arc::START_ID()))->pnt();
    std::shared_ptr<GeomAPI_Pnt2d> aPntEnd = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
      theFeature->attribute(SketchPlugin_Arc::END_ID()))->pnt();
    if(theFeature->attribute(SketchPlugin_Arc::INVERSED_ID())) {
      aPntTemp = aPntStart;
      aPntStart = aPntEnd;
      aPntEnd = aPntTemp;
    }
    std::shared_ptr<GeomAPI_Pnt2d> aCenterPnt = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
      theFeature->attribute(SketchPlugin_Arc::CENTER_ID()))->pnt();
    std::shared_ptr<GeomAPI_Circ2d> aCirc(new GeomAPI_Circ2d(aCenterPnt, aPntStart));
    double aStartParameter(0), anEndParameter(0);
    aCirc->parameter(aPntStart, paramTolerance, aStartParameter);
    aCirc->parameter(aPntEnd, paramTolerance, anEndParameter);
    if(aPntStart->distance(theFilletPoint) > tolerance) {
      double aTmpParameter = aStartParameter;
      aStartParameter = anEndParameter;
      anEndParameter = aTmpParameter;
    }
    double aPntParameter = aStartParameter + (anEndParameter - aStartParameter) / 3.0;
    aCirc->D0(aPntParameter, thePoint);
  }
}

double getProjectionDistance(const FeaturePtr theFeature,
                             const std::shared_ptr<GeomAPI_Pnt2d> thePoint)
{
  std::shared_ptr<GeomAPI_Pnt2d> aProjectPnt;
  if(theFeature->getKind() == SketchPlugin_Line::ID()) {
    std::shared_ptr<GeomAPI_Pnt2d> aPntStart = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
      theFeature->attribute(SketchPlugin_Line::START_ID()))->pnt();
    std::shared_ptr<GeomAPI_Pnt2d> aPntEnd = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
      theFeature->attribute(SketchPlugin_Line::END_ID()))->pnt();
    std::shared_ptr<GeomAPI_Lin2d> aLin(new GeomAPI_Lin2d(aPntStart, aPntEnd));
    aProjectPnt = aLin->project(thePoint);
  } else {
    std::shared_ptr<GeomAPI_Pnt2d> aPntStart = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
      theFeature->attribute(SketchPlugin_Arc::START_ID()))->pnt();
    std::shared_ptr<GeomAPI_Pnt2d> aPntEnd = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
      theFeature->attribute(SketchPlugin_Arc::END_ID()))->pnt();
    std::shared_ptr<GeomAPI_Pnt2d> aCenterPnt = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
      theFeature->attribute(SketchPlugin_Arc::CENTER_ID()))->pnt();
    std::shared_ptr<GeomAPI_Circ2d> aCirc(new GeomAPI_Circ2d(aCenterPnt, aPntStart));
    aProjectPnt = aCirc->project(thePoint);
  }
  if(aProjectPnt.get()) {
    return aProjectPnt->distance(thePoint);
  }
  return -1;
}

std::set<FeaturePtr> getCoincides(const FeaturePtr& theConstraintCoincidence)
{
  std::set<FeaturePtr> aCoincides;

  SketchPlugin_Tools::findCoincidences(theConstraintCoincidence,
                                       SketchPlugin_ConstraintCoincidence::ENTITY_A(),
                                       aCoincides);
  SketchPlugin_Tools::findCoincidences(theConstraintCoincidence,
                                       SketchPlugin_ConstraintCoincidence::ENTITY_B(),
                                       aCoincides);

  // Remove points from set of coincides.
  std::set<FeaturePtr> aNewSetOfCoincides;
  for(std::set<FeaturePtr>::iterator anIt = aCoincides.begin(); anIt != aCoincides.end(); ++anIt) {
    if((*anIt)->getKind() == SketchPlugin_Line::ID() ||
        (*anIt)->getKind() == SketchPlugin_Arc::ID() ) {
      aNewSetOfCoincides.insert(*anIt);
    }
  }
  aCoincides = aNewSetOfCoincides;

  // If we still have more than two coincides remove auxilary entities from set of coincides.
  if(aCoincides.size() > 2) {
    aNewSetOfCoincides.clear();
    for(std::set<FeaturePtr>::iterator anIt = aCoincides.begin(); anIt != aCoincides.end(); ++anIt) {
      if(!(*anIt)->boolean(SketchPlugin_SketchEntity::AUXILIARY_ID())->value()) {
        aNewSetOfCoincides.insert(*anIt);
      }
    }
    aCoincides = aNewSetOfCoincides;
  }

  return aCoincides;
}
