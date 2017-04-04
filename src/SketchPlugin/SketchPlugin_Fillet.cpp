// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:    SketchPlugin_Fillet.cpp
// Created: 19 Mar 2015
// Author:  Artem ZHIDKOV

#include "SketchPlugin_Fillet.h"

#include "SketchPlugin_Arc.h"
#include "SketchPlugin_Line.h"
#include "SketchPlugin_Point.h"
#include "SketchPlugin_Sketch.h"
#include "SketchPlugin_ConstraintEqual.h"
#include "SketchPlugin_ConstraintCoincidence.h"
#include "SketchPlugin_ConstraintLength.h"
#include "SketchPlugin_ConstraintMiddle.h"
#include "SketchPlugin_ConstraintTangent.h"
#include "SketchPlugin_ConstraintRadius.h"
#include "SketchPlugin_Tools.h"

#include <ModelAPI_AttributeRefAttr.h>
#include <ModelAPI_Data.h>
#include <ModelAPI_Events.h>
#include <ModelAPI_Session.h>
#include <ModelAPI_Tools.h>
#include <ModelAPI_Validator.h>

#include <GeomAlgoAPI_Circ2dBuilder.h>
#include <GeomAlgoAPI_EdgeBuilder.h>

#include <GeomAPI_Circ2d.h>
#include <GeomAPI_Dir2d.h>
#include <GeomAPI_Lin2d.h>
#include <GeomAPI_Pnt2d.h>
#include <GeomAPI_XY.h>

#include <GeomDataAPI_Point2D.h>

#include <Events_Loop.h>

#include <math.h>

const double tolerance = 1.e-7;
const double paramTolerance = 1.e-4;
const double PI = 3.141592653589793238463;

/// \brief Attract specified point on theNewArc to the attribute of theFeature
static void recalculateAttributes(FeaturePtr theNewArc, const std::string& theNewArcAttribute,
  FeaturePtr theFeature, const std::string& theFeatureAttribute);


/// \brief Calculate radius of a fillet.
///        It should not be greater than 1/3 of shortest edge length.
static double calculateFilletRadius(FeaturePtr theFilletFeatures[2]);

/// \brief Calculates center of fillet arc and coordinates of tangency points
static void calculateFilletCenter(FeaturePtr theFilletFeatures[2],
                                  double theFilletRadius,
                                  const std::shared_ptr<GeomAPI_Ax3>& theSketchPlane,
                                  std::shared_ptr<GeomAPI_XY>& theCenter,
                                  std::shared_ptr<GeomAPI_XY>& theTangentA,
                                  std::shared_ptr<GeomAPI_XY>& theTangentB);

/// Get coincide edges for fillet
static std::set<FeaturePtr> getCoincides(const FeaturePtr& theConstraintCoincidence);

static std::set<FeaturePtr> findFeaturesToRemove(const FeaturePtr theFeature,
                                                 const AttributePtr theAttribute);

SketchPlugin_Fillet::SketchPlugin_Fillet()
: myFilletCreated(false)
{
}

void SketchPlugin_Fillet::initAttributes()
{
  data()->addAttribute(FILLET_POINT_ID(), ModelAPI_AttributeRefAttr::typeId());
}

void SketchPlugin_Fillet::execute()
{
  // Wait all constraints being created, then send update events
  static Events_ID anUpdateEvent = Events_Loop::eventByName(EVENT_OBJECT_UPDATED);
  bool isUpdateFlushed = Events_Loop::loop()->isFlushed(anUpdateEvent);
  if (isUpdateFlushed)
    Events_Loop::loop()->setFlushed(anUpdateEvent, false);

  // Calculate Fillet parameters if does not yet
  if (!myBaseFeatures[0] || !myBaseFeatures[1])
    calculateFilletParameters();

  // Create arc feature.
  FeaturePtr aFilletArc = sketch()->addFeature(SketchPlugin_Arc::ID());

  // Set arc attributes.
  bool aWasBlocked = aFilletArc->data()->blockSendAttributeUpdated(true);
  std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
      aFilletArc->attribute(SketchPlugin_Arc::CENTER_ID()))->setValue(myCenterXY->x(),
                                                                      myCenterXY->y());
  std::shared_ptr<GeomDataAPI_Point2D> aStartPoint =
      std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
          aFilletArc->attribute(SketchPlugin_Arc::START_ID()));
  std::shared_ptr<GeomDataAPI_Point2D> aEndPoint =
      std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
          aFilletArc->attribute(SketchPlugin_Arc::END_ID()));
  if(aStartPoint->isInitialized() && aEndPoint->isInitialized()
      && (aStartPoint->pnt()->xy()->distance(myTangentXY1) > tolerance
      || aEndPoint->pnt()->xy()->distance(myTangentXY2) > tolerance)) {
    std::dynamic_pointer_cast<SketchPlugin_Arc>(aFilletArc)->setReversed(false);
  }
  aStartPoint->setValue(myTangentXY1->x(), myTangentXY1->y());
  aEndPoint->setValue(myTangentXY2->x(), myTangentXY2->y());
  aFilletArc->data()->blockSendAttributeUpdated(aWasBlocked);
  aFilletArc->execute();

  // Delete features with refs to points of edges.
  std::shared_ptr<GeomDataAPI_Point2D> aStartPoint1;
  int aFeatInd1 = myIsReversed ? 1 : 0;
  int anAttrInd1 = (myIsReversed ? 2 : 0) + (myIsNotInversed[aFeatInd1] ? 0 : 1);
  aStartPoint1 = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
      myBaseFeatures[aFeatInd1]->attribute(myFeatAttributes[anAttrInd1]));
  std::set<FeaturePtr> aFeaturesToBeRemoved1 =
    findFeaturesToRemove(myBaseFeatures[aFeatInd1], aStartPoint1);

  std::shared_ptr<GeomDataAPI_Point2D> aStartPoint2;
  int aFeatInd2 = myIsReversed ? 0 : 1;
  int anAttrInd2 = (myIsReversed ? 0 : 2) + (myIsNotInversed[aFeatInd2] ? 0 : 1);
  aStartPoint2 = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
      myBaseFeatures[aFeatInd2]->attribute(myFeatAttributes[anAttrInd2]));
  std::set<FeaturePtr> aFeaturesToBeRemoved2 =
    findFeaturesToRemove(myBaseFeatures[aFeatInd2], aStartPoint2);

  aFeaturesToBeRemoved1.insert(aFeaturesToBeRemoved2.begin(), aFeaturesToBeRemoved2.end());
  ModelAPI_Tools::removeFeaturesAndReferences(aFeaturesToBeRemoved1);
  Events_Loop::loop()->flush(Events_Loop::eventByName(EVENT_OBJECT_DELETED));

  // Update fillet edges.
  recalculateAttributes(aFilletArc, SketchPlugin_Arc::START_ID(),
                        myBaseFeatures[aFeatInd1], myFeatAttributes[anAttrInd1]);
  recalculateAttributes(aFilletArc, SketchPlugin_Arc::END_ID(),
                        myBaseFeatures[aFeatInd2], myFeatAttributes[anAttrInd2]);

  // Create coincidence features.
  FeaturePtr aConstraint = sketch()->addFeature(SketchPlugin_ConstraintCoincidence::ID());
  AttributeRefAttrPtr aRefAttr = std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(
      aConstraint->attribute(SketchPlugin_Constraint::ENTITY_A()));
  aRefAttr->setAttr(aFilletArc->attribute(SketchPlugin_Arc::START_ID()));
  aRefAttr = std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(
      aConstraint->attribute(SketchPlugin_Constraint::ENTITY_B()));
  aRefAttr->setAttr(myBaseFeatures[aFeatInd1]->attribute(myFeatAttributes[anAttrInd1]));
  aConstraint->execute();
  ModelAPI_EventCreator::get()->sendUpdated(aConstraint, anUpdateEvent);
  aConstraint = sketch()->addFeature(SketchPlugin_ConstraintCoincidence::ID());
  aRefAttr = std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(
      aConstraint->attribute(SketchPlugin_Constraint::ENTITY_A()));
  aRefAttr->setAttr(aFilletArc->attribute(SketchPlugin_Arc::END_ID()));
  aRefAttr = std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(
      aConstraint->attribute(SketchPlugin_Constraint::ENTITY_B()));
  aRefAttr->setAttr(myBaseFeatures[aFeatInd2]->attribute(myFeatAttributes[anAttrInd2]));
  aConstraint->execute();
  ModelAPI_EventCreator::get()->sendUpdated(aConstraint, anUpdateEvent);

  // Create tangent features.
  for (int i = 0; i < 2; i++) {
    aConstraint = sketch()->addFeature(SketchPlugin_ConstraintTangent::ID());
    aRefAttr = std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(
        aConstraint->attribute(SketchPlugin_Constraint::ENTITY_A()));
    aRefAttr->setObject(aFilletArc->lastResult());
    aRefAttr = std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(
        aConstraint->attribute(SketchPlugin_Constraint::ENTITY_B()));
    bool isArc = myBaseFeatures[i]->getKind() == SketchPlugin_Arc::ID();
    aRefAttr->setObject(isArc ? myBaseFeatures[i]->lastResult() :
                                myBaseFeatures[i]->firstResult());
    aConstraint->execute();
    ModelAPI_EventCreator::get()->sendUpdated(aConstraint, anUpdateEvent);
  }

  // Send events to update the sub-features by the solver.
  if(isUpdateFlushed) {
    Events_Loop::loop()->setFlushed(anUpdateEvent, true);
  }

  myFilletCreated = true;
}

AISObjectPtr SketchPlugin_Fillet::getAISObject(AISObjectPtr thePrevious)
{
  if(myFilletCreated) {
    return AISObjectPtr();
  }

  SketchPlugin_Sketch* aSketch = sketch();
  if(!aSketch) {
    return AISObjectPtr();
  }

  if (!calculateFilletParameters())
    return AISObjectPtr();

  // Create arc for presentation.
  std::shared_ptr<GeomAPI_Pnt> aCenterPnt(aSketch->to3D(myCenterXY->x(), myCenterXY->y()));
  std::shared_ptr<GeomAPI_Pnt> aTangentPnt1(aSketch->to3D(myTangentXY1->x(),
                                                          myTangentXY1->y()));
  std::shared_ptr<GeomAPI_Pnt> aTangentPnt2(aSketch->to3D(myTangentXY2->x(),
                                                          myTangentXY2->y()));
  std::shared_ptr<GeomDataAPI_Dir> aNDir = std::dynamic_pointer_cast<GeomDataAPI_Dir>(
    aSketch->data()->attribute(SketchPlugin_Sketch::NORM_ID()));
  std::shared_ptr<GeomAPI_Shape> anArcShape =
      GeomAlgoAPI_EdgeBuilder::lineCircleArc(aCenterPnt, aTangentPnt1, aTangentPnt2, aNDir->dir());

  AISObjectPtr anAISObject = thePrevious;
  if(!anAISObject.get()) {
    anAISObject = AISObjectPtr(new GeomAPI_AISObject);
  }
  anAISObject->createShape(anArcShape);
  return anAISObject;
}

bool SketchPlugin_Fillet::calculateFilletParameters()
{
  // Get fillet point.
  AttributeRefAttrPtr aPointRefAttr = refattr(FILLET_POINT_ID());
  if (!aPointRefAttr->isInitialized() || aPointRefAttr->isObject())
    return false;
  std::shared_ptr<GeomDataAPI_Point2D> aFilletPoint2D =
    std::dynamic_pointer_cast<GeomDataAPI_Point2D>(aPointRefAttr->attr());
  if (!aFilletPoint2D.get())
    return false;

  if (!findFeaturesContainingFilletPoint(aFilletPoint2D)) {
    setError("Error: Selected point does not have two suitable edges for fillet.");
    return false;
  }

  std::shared_ptr<GeomAPI_Pnt2d> aFilletPnt2d = aFilletPoint2D->pnt();
  double aRadius = calculateFilletRadius(myBaseFeatures);

  // Calculate arc attributes.
  static const int aNbFeatures = 2;
  // First pair of points relate to first feature, second pair -  to second.
  std::shared_ptr<GeomAPI_Pnt2d> aStartEndPnt[aNbFeatures * 2];
  for (int i = 0; i < aNbFeatures; i++) {
    std::string aStartAttr, aEndAttr;
    if (myBaseFeatures[i]->getKind() == SketchPlugin_Line::ID()) {
      aStartAttr = SketchPlugin_Line::START_ID();
      aEndAttr = SketchPlugin_Line::END_ID();
    } else if (myBaseFeatures[i]->getKind() == SketchPlugin_Arc::ID()) {
      aStartAttr = SketchPlugin_Arc::START_ID();
      aEndAttr = SketchPlugin_Arc::END_ID();
    } else { // Wrong argument.
      setError("Error: One of the points has wrong coincide feature");
      return false;
    }
    myFeatAttributes[2*i] = aStartAttr;
    aStartEndPnt[2*i] = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
      myBaseFeatures[i]->attribute(aStartAttr))->pnt();
    myFeatAttributes[2*i+1] = aEndAttr;
    aStartEndPnt[2*i+1] = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
      myBaseFeatures[i]->attribute(aEndAttr))->pnt();
  }
  for (int aFeatInd = 0; aFeatInd < aNbFeatures; aFeatInd++) {
    for (int j = 0; j < 2; j++) // loop on start-end of each feature
      if (aStartEndPnt[aFeatInd * aNbFeatures + j]->distance(aFilletPnt2d) < 1.e-10) {
        myIsNotInversed[aFeatInd] = (j==0);
        break;
      }
  }

  std::shared_ptr<GeomAPI_Ax3> aSketchPlane = SketchPlugin_Sketch::plane(sketch());
  calculateFilletCenter(myBaseFeatures, aRadius, aSketchPlane,
                        myCenterXY, myTangentXY1, myTangentXY2);

  // Tangent directions of the features in coincident point.
  std::shared_ptr<GeomAPI_Dir2d> aTangentDir[aNbFeatures];
  for (int i = 0; i < aNbFeatures; i++) {
    std::shared_ptr<GeomAPI_XY> aDir;
    if (myBaseFeatures[i]->getKind() == SketchPlugin_Line::ID()) {
      aDir = aStartEndPnt[2*i+1]->xy()->decreased(aStartEndPnt[2*i]->xy());
      if (!myIsNotInversed[i])
        aDir = aDir->multiplied(-1.0);
    } else if (myBaseFeatures[i]->getKind() == SketchPlugin_Arc::ID()) {
      std::shared_ptr<GeomAPI_Pnt2d> aCenterPoint =
        std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
        myBaseFeatures[i]->attribute(SketchPlugin_Arc::CENTER_ID()))->pnt();
      aDir = myIsNotInversed[i] ? aStartEndPnt[2*i]->xy() : aStartEndPnt[2*i+1]->xy();
      aDir = aDir->decreased(aCenterPoint->xy());

      double x = aDir->x();
      double y = aDir->y();
      aDir->setX(-y);
      aDir->setY(x);
      if (myIsNotInversed[i] ==
          std::dynamic_pointer_cast<SketchPlugin_Arc>(myBaseFeatures[i])->isReversed())
        aDir = aDir->multiplied(-1.0);
    }
    aTangentDir[i] = std::shared_ptr<GeomAPI_Dir2d>(new GeomAPI_Dir2d(aDir));
  }

  // By default, the start point of fillet arc is connected to FeatureA,
  // and the end point - to FeatureB. But when the angle between TangentDirA and
  // TangentDirB greater 180 degree, the sequaence of features need to be reversed.
  double cosBA = aTangentDir[0]->cross(aTangentDir[1]); // cos(B-A),
  // where A and B - angles between corresponding tanget direction and the X axis
  myIsReversed = cosBA > 0.0;

  if(myIsReversed) {
    std::shared_ptr<GeomAPI_XY> aTmp = myTangentXY1;
    myTangentXY1 = myTangentXY2;
    myTangentXY2 = aTmp;
  }
  return true;
}

bool SketchPlugin_Fillet::findFeaturesContainingFilletPoint(
    std::shared_ptr<GeomDataAPI_Point2D> theFilletPoint)
{
  // Obtain constraint coincidence for the fillet point.
  FeaturePtr aConstraintCoincidence;
  const std::set<AttributePtr>& aRefsList = theFilletPoint->owner()->data()->refsToMe();
  for(std::set<AttributePtr>::const_iterator anIt = aRefsList.cbegin();
      anIt != aRefsList.cend();
      ++anIt) {
    std::shared_ptr<ModelAPI_Attribute> anAttr = (*anIt);
    FeaturePtr aFeature = std::dynamic_pointer_cast<ModelAPI_Feature>(anAttr->owner());
    if(aFeature->getKind() == SketchPlugin_ConstraintCoincidence::ID()) {
      AttributeRefAttrPtr anAttrRefA =
          aFeature->refattr(SketchPlugin_ConstraintCoincidence::ENTITY_A());
      AttributeRefAttrPtr anAttrRefB =
          aFeature->refattr(SketchPlugin_ConstraintCoincidence::ENTITY_B());
      if(anAttrRefA.get() && !anAttrRefA->isObject()) {
        AttributePtr anAttrA = anAttrRefA->attr();
        if(theFilletPoint == anAttrA) {
          aConstraintCoincidence = aFeature;
          break;
        }
      }
      if(anAttrRefB.get() && !anAttrRefB->isObject()) {
        AttributePtr anAttrB = anAttrRefB->attr();
        if(theFilletPoint == anAttrB) {
          aConstraintCoincidence = aFeature;
          break;
        }
      }
    }
  }

  if(!aConstraintCoincidence.get())
    return false;

  // Get coincide edges.
  std::set<FeaturePtr> anEdgeFeatures = getCoincides(aConstraintCoincidence);
  std::set<FeaturePtr>::iterator aLinesIt = anEdgeFeatures.begin();
  for (int i = 0; aLinesIt != anEdgeFeatures.end() && i < 2; ++aLinesIt, ++i)
    myBaseFeatures[i] = *aLinesIt;

  return myBaseFeatures[0] && myBaseFeatures[1];
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

static std::shared_ptr<GeomAPI_Pnt2d> toPoint(const AttributePtr& theAttribute)
{
  std::shared_ptr<GeomAPI_Pnt2d> aPoint;
  AttributePoint2DPtr aPointAttr = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(theAttribute);
  if (aPointAttr)
    aPoint = aPointAttr->pnt();
  return aPoint;
}

static std::shared_ptr<GeomAPI_Lin2d> toLine(const FeaturePtr& theFeature)
{
  std::shared_ptr<GeomAPI_Lin2d> aLine;
  if (theFeature->getKind() == SketchPlugin_Line::ID()) {
    std::shared_ptr<GeomAPI_Pnt2d> aStart =
        toPoint( theFeature->attribute(SketchPlugin_Line::START_ID()) );
    std::shared_ptr<GeomAPI_Pnt2d> aEnd =
        toPoint( theFeature->attribute(SketchPlugin_Line::END_ID()) );
    aLine = std::shared_ptr<GeomAPI_Lin2d>(new GeomAPI_Lin2d(aStart, aEnd));
  }
  return aLine;
}

static std::shared_ptr<GeomAPI_Circ2d> toCircle(const FeaturePtr& theFeature)
{
  std::shared_ptr<GeomAPI_Circ2d> aCircle;
  if (theFeature->getKind() == SketchPlugin_Arc::ID()) {
    std::shared_ptr<GeomAPI_Pnt2d> aCenter =
        toPoint( theFeature->attribute(SketchPlugin_Arc::CENTER_ID()) );
    std::shared_ptr<GeomAPI_Pnt2d> aStart =
        toPoint( theFeature->attribute(SketchPlugin_Arc::START_ID()) );
    aCircle = std::shared_ptr<GeomAPI_Circ2d>(new GeomAPI_Circ2d(aCenter, aStart));
  }
  return aCircle;
}


void calculateFilletCenter(FeaturePtr theFilletFeatures[2],
                           double theFilletRadius,
                           const std::shared_ptr<GeomAPI_Ax3>& theSketchPlane,
                           std::shared_ptr<GeomAPI_XY>& theCenter,
                           std::shared_ptr<GeomAPI_XY>& theTangentA,
                           std::shared_ptr<GeomAPI_XY>& theTangentB)
{
  GeomShapePtr aShapeA = theFilletFeatures[0]->lastResult()->shape();
  GeomShapePtr aShapeB = theFilletFeatures[1]->lastResult()->shape();

  GeomAlgoAPI_Circ2dBuilder aCircBuilder(theSketchPlane);
  aCircBuilder.addTangentCurve(aShapeA);
  aCircBuilder.addTangentCurve(aShapeB);
  aCircBuilder.setRadius(theFilletRadius);

  std::shared_ptr<GeomAPI_Circ2d> aFilletCircle = aCircBuilder.circle();
  if (!aFilletCircle)
    return;

  theCenter = aFilletCircle->center()->xy();
  // tangent points
  std::shared_ptr<GeomAPI_Pnt2d> aTgPoints[2];
  for (int i = 0; i < 2; ++i) {
    std::shared_ptr<GeomAPI_Circ2d> aCircle = toCircle(theFilletFeatures[i]);
    if (aCircle)
      aTgPoints[i] = aCircle->project(aFilletCircle->center());
    else {
      std::shared_ptr<GeomAPI_Lin2d> aLine = toLine(theFilletFeatures[i]);
      if (aLine)
        aTgPoints[i] = aLine->project(aFilletCircle->center());
    }
  }
  theTangentA = aTgPoints[0]->xy();
  theTangentB = aTgPoints[1]->xy();
}

double calculateFilletRadius(FeaturePtr theFilletFeatures[2])
{
  double aLengths[2] = { 0, 0 };
  for (int i = 0; i < 2; ++i) {
    GeomShapePtr aShape = theFilletFeatures[i]->lastResult()->shape();
    std::shared_ptr<GeomAPI_Edge> anEdge = std::dynamic_pointer_cast<GeomAPI_Edge>(aShape);
    if (anEdge)
      aLengths[i] = anEdge->length();
  }
  return std::min(aLengths[0], aLengths[1]) / 6.0;
}

std::set<FeaturePtr> getCoincides(const FeaturePtr& theConstraintCoincidence)
{
  std::set<FeaturePtr> aCoincides;

  std::shared_ptr<GeomAPI_Pnt2d> aFilletPnt =
    SketchPlugin_Tools::getCoincidencePoint(theConstraintCoincidence);

  SketchPlugin_Tools::findCoincidences(theConstraintCoincidence,
                                       SketchPlugin_ConstraintCoincidence::ENTITY_A(),
                                       aCoincides);
  SketchPlugin_Tools::findCoincidences(theConstraintCoincidence,
                                       SketchPlugin_ConstraintCoincidence::ENTITY_B(),
                                       aCoincides);

  // Remove points from set of coincides.
  std::set<FeaturePtr> aNewSetOfCoincides;
  for(std::set<FeaturePtr>::iterator anIt = aCoincides.begin(); anIt != aCoincides.end(); ++anIt) {
    std::shared_ptr<SketchPlugin_SketchEntity> aSketchEntity =
      std::dynamic_pointer_cast<SketchPlugin_SketchEntity>(*anIt);
    if(aSketchEntity.get() && aSketchEntity->isCopy()) {
      continue;
    }
    if((*anIt)->getKind() == SketchPlugin_Line::ID()) {
      aNewSetOfCoincides.insert(*anIt);
    } else if((*anIt)->getKind() == SketchPlugin_Arc::ID()) {
      AttributePtr anAttrCenter = (*anIt)->attribute(SketchPlugin_Arc::CENTER_ID());
      std::shared_ptr<GeomDataAPI_Point2D> aPointCenter2D =
        std::dynamic_pointer_cast<GeomDataAPI_Point2D>(anAttrCenter);
      if(aPointCenter2D.get() && aFilletPnt->isEqual(aPointCenter2D->pnt())) {
        continue;
      }
      aNewSetOfCoincides.insert(*anIt);
    }
  }
  aCoincides = aNewSetOfCoincides;

  // If we still have more than two coincides remove auxilary entities from set of coincides.
  if(aCoincides.size() > 2) {
    aNewSetOfCoincides.clear();
    for(std::set<FeaturePtr>::iterator
        anIt = aCoincides.begin(); anIt != aCoincides.end(); ++anIt) {
      if(!(*anIt)->boolean(SketchPlugin_SketchEntity::AUXILIARY_ID())->value()) {
        aNewSetOfCoincides.insert(*anIt);
      }
    }
    aCoincides = aNewSetOfCoincides;
  }

  return aCoincides;
}

std::set<FeaturePtr> findFeaturesToRemove(const FeaturePtr theFeature,
                                          const AttributePtr theAttribute) {
  std::set<FeaturePtr> aFeaturesToBeRemoved;
  std::set<AttributePtr> aRefs = theFeature->data()->refsToMe();
  std::list<ResultPtr> aResults = theFeature->results();
  for(std::list<ResultPtr>::const_iterator aResultsIt = aResults.cbegin();
      aResultsIt != aResults.cend();
      ++aResultsIt) {
    ResultPtr aResult = *aResultsIt;
    std::set<AttributePtr> aResultRefs = aResult->data()->refsToMe();
    aRefs.insert(aResultRefs.begin(), aResultRefs.end());
  }
  for(std::set<AttributePtr>::const_iterator anIt = aRefs.cbegin();
    anIt != aRefs.cend();
    ++anIt) {
    std::shared_ptr<ModelAPI_Attribute> anAttr = (*anIt);
    FeaturePtr aFeature = std::dynamic_pointer_cast<ModelAPI_Feature>(anAttr->owner());
    if(aFeature->getKind() == SketchPlugin_Fillet::ID()) {
      continue;
    }
    if(aFeature->getKind() == SketchPlugin_ConstraintLength::ID()
        || aFeature->getKind() == SketchPlugin_ConstraintEqual::ID()
        || aFeature->getKind() == SketchPlugin_ConstraintMiddle::ID()) {
      aFeaturesToBeRemoved.insert(aFeature);
    } else {
      std::list<AttributePtr> anAttrs =
          aFeature->data()->attributes(ModelAPI_AttributeRefAttr::typeId());
      for(std::list<AttributePtr>::const_iterator aRefAttrsIt = anAttrs.cbegin();
          aRefAttrsIt != anAttrs.cend();
          ++aRefAttrsIt) {
        AttributeRefAttrPtr anAttrRefAttr =
          std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(*aRefAttrsIt);
        if(anAttrRefAttr.get() && anAttrRefAttr->attr() == theAttribute) {
          aFeaturesToBeRemoved.insert(aFeature);
        }
      }
    }
  }
  return aFeaturesToBeRemoved;
}
