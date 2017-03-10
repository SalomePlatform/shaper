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
#include "SketchPlugin_ConstraintTangent.h"
#include "SketchPlugin_ConstraintRadius.h"
#include "SketchPlugin_Tools.h"

#include <ModelAPI_AttributeRefAttr.h>
#include <ModelAPI_Data.h>
#include <ModelAPI_Events.h>
#include <ModelAPI_Session.h>
#include <ModelAPI_Tools.h>
#include <ModelAPI_Validator.h>

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

  // Getting points located at 1/3 of edge length from fillet point.
  std::shared_ptr<GeomAPI_Pnt2d> aFilletPnt2d = aFilletPoint2D->pnt();
  std::shared_ptr<GeomAPI_Pnt2d> aPnt1, aPnt2;
  getPointOnEdge(myBaseFeatures[0], aFilletPnt2d, aPnt1);
  getPointOnEdge(myBaseFeatures[1], aFilletPnt2d, aPnt2);

  /// Getting distances.
  double aDistance1 = getProjectionDistance(myBaseFeatures[1], aPnt1);
  double aDistance2 = getProjectionDistance(myBaseFeatures[0], aPnt2);

  // Calculate radius value for fillet.
  double aRadius = aDistance1 < aDistance2 ? aDistance1 / 2.0 : aDistance2 / 2.0;

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

  calculateFilletCenter(myBaseFeatures[0], myBaseFeatures[1], aRadius,
      myIsNotInversed, myCenterXY, myTangentXY1, myTangentXY2);

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

      double x1 = theCenterA->x() +
        (aMedDist * aCenterDir->x() + aCenterDir->y() * aHeight) / aCenterDist;
      double y1 = theCenterA->y() +
        (aMedDist * aCenterDir->y() - aCenterDir->x() * aHeight) / aCenterDist;

      double x2 = theCenterA->x() +
        (aMedDist * aCenterDir->x() - aCenterDir->y() * aHeight) / aCenterDist;
      double y2 = theCenterA->y() +
        (aMedDist * aCenterDir->y() + aCenterDir->x() * aHeight) / aCenterDist;

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
    possibleFilletCenterLineLine(aStart[0], aDir[0], aStart[1], aDir[1],
                                 theRadius, aSuspectCenters);
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
    possibleFilletCenterLineArc(aStart[aLineInd], aDirLine, aCenter[1-aLineInd],
                                anArcRadius, theRadius, aSuspectCenters);
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
    possibleFilletCenterArcArc(aCenter[0], anArcRadius[0], aCenter[1],
                               anArcRadius[1], theRadius, aSuspectCenters);
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
    thePoint.reset(
      new GeomAPI_Pnt2d(aPntStart->xy()->added(
      aPntEnd->xy()->decreased( aPntStart->xy() )->multiplied(1.0 / 3.0) ) ) );
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
        || aFeature->getKind() == SketchPlugin_ConstraintEqual::ID()) {
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
