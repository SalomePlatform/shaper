// Copyright (C) 2014-2019  CEA/DEN, EDF R&D
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

#include "SketchPlugin_Fillet.h"

#include "SketchPlugin_Arc.h"
#include "SketchPlugin_Line.h"
#include "SketchPlugin_Point.h"
#include "SketchPlugin_Sketch.h"
#include "SketchPlugin_ConstraintDistance.h"
#include "SketchPlugin_ConstraintEqual.h"
#include "SketchPlugin_ConstraintCoincidence.h"
#include "SketchPlugin_ConstraintLength.h"
#include "SketchPlugin_ConstraintMiddle.h"
#include "SketchPlugin_ConstraintTangent.h"
#include "SketchPlugin_ConstraintRadius.h"
#include "SketchPlugin_Tools.h"

#include <ModelAPI_AttributeDouble.h>
#include <ModelAPI_AttributeInteger.h>
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

static std::set<FeaturePtr> findFeaturesToRemove(const FeaturePtr theFeature,
                                                 const AttributePtr theAttribute);

SketchPlugin_Fillet::SketchPlugin_Fillet()
: myFilletCreated(false), myIsReversed(false)
{
  myIsNotInversed[0] = myIsNotInversed[1] = true;
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

  // set flag here to avoid building Fillet presentation if "Redisplay" event appears
  myFilletCreated = true;

  // create feature for fillet arc
  FeaturePtr aFilletArc = createFilletArc();
  if (!aFilletArc) {
    setError("Error: unable to create a fillet arc.");
    return;
  }

  // collect features referred to the edges participating in fillet
  AttributePoint2DPtr aFilletPoints[2];
  int aFeatInd[2];
  int anAttrInd[2];
  std::set<FeaturePtr> aFeaturesToBeRemoved;
  for (int i = 0; i < 2; ++i) {
    bool isFirstIndex = (i == 0);
    aFeatInd[i] = myIsReversed == isFirstIndex ? 1 : 0;
    anAttrInd[i] = (myIsReversed == isFirstIndex ? 2 : 0) + (myIsNotInversed[aFeatInd[i]] ? 0 : 1);
    aFilletPoints[i] = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
        myBaseFeatures[aFeatInd[i]]->attribute(myFeatAttributes[anAttrInd[i]]));
    std::set<FeaturePtr> aRemove =
        findFeaturesToRemove(myBaseFeatures[aFeatInd[i]], aFilletPoints[i]);
    aFeaturesToBeRemoved.insert(aRemove.begin(), aRemove.end());
  }

  // keep "distance" constraints and remove all other references
  removeReferencesButKeepDistances(aFeaturesToBeRemoved, aFilletPoints);

  // Update fillet edges.
  recalculateAttributes(aFilletArc, SketchPlugin_Arc::START_ID(),
                        myBaseFeatures[aFeatInd[0]], myFeatAttributes[anAttrInd[0]]);
  recalculateAttributes(aFilletArc, SketchPlugin_Arc::END_ID(),
                        myBaseFeatures[aFeatInd[1]], myFeatAttributes[anAttrInd[1]]);

  FeaturePtr aConstraint;

  // Create coincidence features.
  aConstraint = SketchPlugin_Tools::createConstraintAttrAttr(sketch(),
                    SketchPlugin_ConstraintCoincidence::ID(),
                    aFilletArc->attribute(SketchPlugin_Arc::START_ID()),
                    myBaseFeatures[aFeatInd[0]]->attribute(myFeatAttributes[anAttrInd[0]]));
  ModelAPI_EventCreator::get()->sendUpdated(aConstraint, anUpdateEvent);
  aConstraint = SketchPlugin_Tools::createConstraintAttrAttr(sketch(),
                    SketchPlugin_ConstraintCoincidence::ID(),
                    aFilletArc->attribute(SketchPlugin_Arc::END_ID()),
                    myBaseFeatures[aFeatInd[1]]->attribute(myFeatAttributes[anAttrInd[1]]));
  ModelAPI_EventCreator::get()->sendUpdated(aConstraint, anUpdateEvent);

  // Create tangent features.
  for (int i = 0; i < 2; i++) {
    aConstraint = SketchPlugin_Tools::createConstraintObjectObject(sketch(),
                      SketchPlugin_ConstraintTangent::ID(),
                      aFilletArc->lastResult(),
                      myBaseFeatures[i]->lastResult());
    aConstraint->execute();
    ModelAPI_EventCreator::get()->sendUpdated(aConstraint, anUpdateEvent);
  }

  // Send events to update the sub-features by the solver.
  if (isUpdateFlushed)
    Events_Loop::loop()->setFlushed(anUpdateEvent, true);
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
  bool isAxiliary = false;
  AttributeBooleanPtr aAttr = boolean(AUXILIARY_ID());
  if (aAttr.get())
    isAxiliary = aAttr->value();
  SketchPlugin_Tools::customizeFeaturePrs(anAISObject, isAxiliary);
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

  std::set<AttributePoint2DPtr> aCoincidentPoints =
      SketchPlugin_Tools::findPointsCoincidentToPoint(aFilletPoint2D);
  std::set<FeaturePtr> aFilletFeatures;
  for (std::set<AttributePoint2DPtr>::iterator aCPIt = aCoincidentPoints.begin();
       aCPIt != aCoincidentPoints.end(); ++aCPIt) {
    std::shared_ptr<SketchPlugin_Feature> anOwner =
        std::dynamic_pointer_cast<SketchPlugin_Feature>(
        ModelAPI_Feature::feature((*aCPIt)->owner()));
    if (anOwner && !anOwner->isExternal())
      aFilletFeatures.insert(anOwner);
  }
  if (aFilletFeatures.size() != 2) {
    setError("Error: Selected point does not have two suitable edges for fillet.");
    return false;
  }

  std::set<FeaturePtr>::iterator aFIt = aFilletFeatures.begin();
  myBaseFeatures[0] = *aFIt;
  myBaseFeatures[1] = *(++aFIt);

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

FeaturePtr SketchPlugin_Fillet::createFilletArc()
{
  // Calculate Fillet parameters if does not yet
  if (!myBaseFeatures[0] || !myBaseFeatures[1])
    calculateFilletParameters();

  // fix for issue #2810 (sometimes, myCenterXY is NULL, fillet should report an error)
  if (!myCenterXY)
    return FeaturePtr();

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

  return aFilletArc;
}

FeaturePtr SketchPlugin_Fillet::createFilletApex(const GeomPnt2dPtr& theCoordinates)
{
  FeaturePtr anApex = sketch()->addFeature(SketchPlugin_Point::ID());
  AttributePoint2DPtr aCoord = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
      anApex->attribute(SketchPlugin_Point::COORD_ID()));
  aCoord->setValue(theCoordinates);
  anApex->boolean(SketchPlugin_Point::AUXILIARY_ID())->setValue(true);

  // additional coincidence constraints
  static Events_ID anUpdateEvent = Events_Loop::eventByName(EVENT_OBJECT_UPDATED);
  FeaturePtr aConstraint;
  for (int i = 0; i < 2; i++) {
    aConstraint = SketchPlugin_Tools::createConstraintAttrObject(sketch(),
                      SketchPlugin_ConstraintCoincidence::ID(),
                      aCoord,
                      myBaseFeatures[i]->lastResult());
    aConstraint->execute();
    ModelAPI_EventCreator::get()->sendUpdated(aConstraint, anUpdateEvent);
  }

  return anApex;
}

struct Length {
  AttributePtr myPoints[2];
  std::string myValueText;
  double myValueDouble;
  GeomPnt2dPtr myFlyoutPoint;
  int myLocationType;
};

void SketchPlugin_Fillet::removeReferencesButKeepDistances(
    std::set<FeaturePtr>& theFeaturesToRemove,
    const AttributePoint2DPtr theFilletPoints[2])
{
  FeaturePtr aFilletApex;
  std::list<Length> aLengthToDistance;

  std::set<FeaturePtr>::iterator aFeat = theFeaturesToRemove.begin();
  while (aFeat != theFeaturesToRemove.end()) {
    std::shared_ptr<SketchPlugin_ConstraintDistance> aDistance =
        std::dynamic_pointer_cast<SketchPlugin_ConstraintDistance>(*aFeat);
    if (aDistance) {
      if (!aFilletApex)
        aFilletApex = createFilletApex(theFilletPoints[0]->pnt());
      // update attributes of distance constraints
      bool isUpdated = false;
      for (int attrInd = 0; attrInd < CONSTRAINT_ATTR_SIZE && !isUpdated; ++attrInd) {
        AttributeRefAttrPtr aRefAttr =
            aDistance->refattr(SketchPlugin_Constraint::ATTRIBUTE(attrInd));
        if (aRefAttr && !aRefAttr->isObject() &&
           (aRefAttr->attr() == theFilletPoints[0] || aRefAttr->attr() == theFilletPoints[1])) {
          aRefAttr->setAttr(aFilletApex->attribute(SketchPlugin_Point::COORD_ID()));
          isUpdated = true;
        }
      }
      // avoid distance from removing if it is updated
      std::set<FeaturePtr>::iterator aKeepIt = aFeat++;
      if (isUpdated)
        theFeaturesToRemove.erase(aKeepIt);

    } else {
      std::shared_ptr<SketchPlugin_ConstraintLength> aLength =
          std::dynamic_pointer_cast<SketchPlugin_ConstraintLength>(*aFeat);
      if (aLength) {
        if (!aFilletApex)
          aFilletApex = createFilletApex(theFilletPoints[0]->pnt());
        // remove Length, but create new distance constraint
        AttributeRefAttrPtr aRefAttr =
          aLength->refattr(SketchPlugin_Constraint::ENTITY_A());
        FeaturePtr aLine = ModelAPI_Feature::feature(aRefAttr->object());
        if (aLine) {
          aLengthToDistance.push_back(Length());
          Length& aNewLength = aLengthToDistance.back();
          // main attrbutes
          for (int i = 0; i < 2; ++i) {
            AttributePtr anAttr = aLine->attribute(
                i == 0 ? SketchPlugin_Line::START_ID() : SketchPlugin_Line::END_ID());
            if (anAttr == theFilletPoints[0] || anAttr == theFilletPoints[1])
              aNewLength.myPoints[i] = aFilletApex->attribute(SketchPlugin_Point::COORD_ID());
            else
              aNewLength.myPoints[i] = anAttr;
          }
          // value
          AttributeDoublePtr aValue = aLength->real(SketchPlugin_Constraint::VALUE());
          aNewLength.myValueDouble = aValue->value();
          aNewLength.myValueText = aValue->text();
          // auxiliary attributes
          AttributePoint2DPtr aFlyoutAttr = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
              aLength->attribute(SketchPlugin_ConstraintLength::FLYOUT_VALUE_PNT()));
          if (aFlyoutAttr && aFlyoutAttr->isInitialized())
            aNewLength.myFlyoutPoint = SketchPlugin_Tools::flyoutPointCoordinates(aLength);
          AttributeIntegerPtr aLocationAttr =
              aLength->integer(SketchPlugin_ConstraintLength::LOCATION_TYPE_ID());
          if (aLocationAttr && aLocationAttr->isInitialized())
            aNewLength.myLocationType = aLocationAttr->value();
          else
            aNewLength.myLocationType = -1;
        }
      }

      ++aFeat;
    }
  }

  // remove references
  ModelAPI_Tools::removeFeaturesAndReferences(theFeaturesToRemove);
  Events_Loop::loop()->flush(Events_Loop::eventByName(EVENT_OBJECT_DELETED));

  // restore Length constraints as point-point distances
  FeaturePtr aConstraint;
  std::list<Length>::iterator anIt = aLengthToDistance.begin();
  for (; anIt != aLengthToDistance.end(); ++anIt) {
    aConstraint = SketchPlugin_Tools::createConstraintAttrAttr(sketch(),
        SketchPlugin_ConstraintDistance::ID(), anIt->myPoints[0], anIt->myPoints[1]);
    // set value
    AttributeDoublePtr aValue = aConstraint->real(SketchPlugin_Constraint::VALUE());
    if (anIt->myValueText.empty())
      aValue->setValue(anIt->myValueDouble);
    else
      aValue->setText(anIt->myValueText);
    // set flyout point if exists
    if (anIt->myFlyoutPoint) {
      AttributePoint2DPtr aFlyoutAttr = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
          aConstraint->attribute(SketchPlugin_ConstraintDistance::FLYOUT_VALUE_PNT()));
      aFlyoutAttr->setValue(anIt->myFlyoutPoint);
    }
    // set location type if initialized
    if (anIt->myLocationType >= 0) {
      AttributeIntegerPtr aLocationType =
          aConstraint->integer(SketchPlugin_ConstraintDistance::LOCATION_TYPE_ID());
      aLocationType->setValue(anIt->myLocationType);
    }
    aConstraint->execute();
    ModelAPI_EventCreator::get()->sendUpdated(aConstraint,
        Events_Loop::eventByName(EVENT_OBJECT_UPDATED));
  }
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
  return (aLengths[0] < aLengths[1] ? aLengths[0] : aLengths[1]) / 6.0;
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
