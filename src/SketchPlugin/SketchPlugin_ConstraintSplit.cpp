// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:    SketchPlugin_ConstraintSplit.cpp
// Created: 17 Jul 2016
// Author:  Natalia ERMOLAEVA

#include "SketchPlugin_ConstraintSplit.h"

//#include <GeomAPI_Circ2d.h>
//#include <GeomAPI_Dir2d.h>
//#include <GeomAPI_Lin2d.h>
#include <GeomAPI_Pnt2d.h>
//#include <GeomAPI_XY.h>
#include <GeomDataAPI_Point2D.h>
//#include <ModelAPI_AttributeDouble.h>
#include <ModelAPI_AttributeReference.h>
#include <ModelAPI_AttributeString.h>
//#include <ModelAPI_AttributeRefList.h>
#include <ModelAPI_AttributeRefAttr.h>

#include <ModelAPI_Validator.h>
#include <ModelAPI_Session.h>

#include <SketchPlugin_Line.h>
#include <SketchPlugin_Arc.h>
#include <SketchPlugin_Circle.h>
#include <SketchPlugin_ConstraintCoincidence.h>
#include <SketchPlugin_ConstraintEqual.h>
#include <SketchPlugin_ConstraintParallel.h>
#include <SketchPlugin_ConstraintTangent.h>

//#include <ModelAPI_Data.h>
#include <ModelAPI_Events.h>
//#include <ModelAPI_Session.h>
//#include <ModelAPI_Validator.h>
//
//#include <SketchPlugin_Arc.h>
//#include <SketchPlugin_Line.h>
//#include <SketchPlugin_Point.h>
//#include <SketchPlugin_Sketch.h>
//#include <SketchPlugin_ConstraintCoincidence.h>
//#include <SketchPlugin_ConstraintTangent.h>
//#include <SketchPlugin_ConstraintRadius.h>
//#include <SketchPlugin_Tools.h>
//
#include <Events_Loop.h>
//
//#include <math.h>
//
//const double tolerance = 1.e-7;
//const double paramTolerance = 1.e-4;

///// \brief Attract specified point on theNewArc to the attribute of theFeature
//static void recalculateAttributes(FeaturePtr theNewArc, const std::string& theNewArcAttribute,
//  FeaturePtr theFeature, const std::string& theFeatureAttribute);
//
///// \brief Calculates center of fillet arc and coordinates of tangency points
//static void calculateFilletCenter(FeaturePtr theFeatureA, FeaturePtr theFeatureB,
//                                  double theRadius, bool theNotInversed[2],
//                                  std::shared_ptr<GeomAPI_XY>& theCenter,
//                                  std::shared_ptr<GeomAPI_XY>& theTangentA,
//                                  std::shared_ptr<GeomAPI_XY>& theTangentB);
//
///// Get point on 1/3 length of edge from fillet point
//static void getPointOnEdge(const FeaturePtr theFeature,
//                           const std::shared_ptr<GeomAPI_Pnt2d> theFilletPoint,
//                           std::shared_ptr<GeomAPI_Pnt2d>& thePoint);
//
///// Get distance from point to feature
//static double getProjectionDistance(const FeaturePtr theFeature,
//                             const std::shared_ptr<GeomAPI_Pnt2d> thePoint);
//
///// Get coincide edges for fillet
//static std::set<FeaturePtr> getCoincides(const FeaturePtr& theConstraintCoincidence);

SketchPlugin_ConstraintSplit::SketchPlugin_ConstraintSplit()
//: myListOfPointsChangedInCode(false),
//  myRadiusChangedByUser(false),
//  myRadiusChangedInCode(false),
//  myRadiusInitialized(false)
{
}

void SketchPlugin_ConstraintSplit::initAttributes()
{
  data()->addAttribute(SketchPlugin_Constraint::VALUE(), ModelAPI_AttributeReference::typeId());
  data()->addAttribute(SketchPlugin_Constraint::ENTITY_A(), ModelAPI_AttributeRefAttr::typeId());
  data()->addAttribute(SketchPlugin_Constraint::ENTITY_B(), ModelAPI_AttributeRefAttr::typeId());

  //data()->addAttribute(SketchPlugin_Constraint::VALUE(), ModelAPI_AttributeDouble::typeId());
  //data()->addAttribute(SketchPlugin_Constraint::ENTITY_A(), ModelAPI_AttributeRefAttrList::typeId());
}

void SketchPlugin_ConstraintSplit::execute()
{
  std::shared_ptr<ModelAPI_Data> aData = data();

  // Check the base objects are initialized.
  AttributeReferencePtr aBaseObjectAttr = std::dynamic_pointer_cast<ModelAPI_AttributeReference>(
                                            aData->attribute(SketchPlugin_Constraint::VALUE()));
  if(!aBaseObjectAttr->isInitialized()) {
    setError("Error: Base object is not initialized.");
    return;
  }
  AttributePoint2DPtr aFirstPointAttr = getPointOfRefAttr(aData->attribute(SketchPlugin_Constraint::ENTITY_A()));
  AttributePoint2DPtr aSecondPointAttr = getPointOfRefAttr(aData->attribute(SketchPlugin_Constraint::ENTITY_B()));
  if (!aFirstPointAttr.get() || !aFirstPointAttr->isInitialized() ||
      !aSecondPointAttr.get() || !aSecondPointAttr->isInitialized()) {
    setError("Error: Sub-shape is not initialized.");
    return;
  }

  // Wait all constraints being created, then send update events
  static Events_ID anUpdateEvent = Events_Loop::eventByName(EVENT_OBJECT_UPDATED);
  bool isUpdateFlushed = Events_Loop::loop()->isFlushed(anUpdateEvent);
  if (isUpdateFlushed)
    Events_Loop::loop()->setFlushed(anUpdateEvent, false);


  FeaturePtr aBaseFeature = ModelAPI_Feature::feature(aBaseObjectAttr->value());
  std::string aFeatureKind = aBaseFeature->getKind();
  FeaturePtr aSplitFeature, aBeforeFeature, anAfterFeature;
  /*if (aFeatureKind == SketchPlugin_Line::ID())
    splitLine(aSplitFeature, anOtherFeatures);
  else*/ if (aFeatureKind == SketchPlugin_Arc::ID())
    splitArc(aSplitFeature, aBeforeFeature, anAfterFeature);
  /*if (aFeatureKind == SketchPlugin_Circle::ID())
    splitCircle(aSplitFeature, anOtherFeatures);
  FeaturePtr aSplitFeature;
  std::set<FeaturePtr> anOtherFeatures;*/

  // Send events to update the sub-features by the solver.
  if(isUpdateFlushed) {
    Events_Loop::loop()->setFlushed(anUpdateEvent, true);
  }
}

void SketchPlugin_ConstraintSplit::attributeChanged(const std::string& theID)
{
/*  if(theID == SketchPlugin_Constraint::ENTITY_A()) {
    if(myListOfPointsChangedInCode) {
      return;
    }

    // Clear results.
    clearResults();

    // Clear list of new points.
    myNewPoints.clear();

    // Get list of points for fillets and current radius.
    AttributeRefAttrListPtr aRefListOfFilletPoints = std::dynamic_pointer_cast<ModelAPI_AttributeRefAttrList>(
      data()->attribute(SketchPlugin_Constraint::ENTITY_A()));
    AttributeDoublePtr aRadiusAttribute = real(VALUE());
    int aListSize = aRefListOfFilletPoints->size();
    if(aListSize == 0 && !myRadiusChangedByUser) {
      // If list is empty reset radius to zero (if it was not changed by user).
      myRadiusChangedInCode = true;
      aRadiusAttribute->setValue(0);
      myRadiusChangedInCode = false;
      return;
    }

    // Iterate over points to get base lines an calculate radius for fillets.
    double aMinimumRadius = 0;
    std::list<std::pair<ObjectPtr, AttributePtr>> aSelectedPointsList = aRefListOfFilletPoints->list();
    std::list<std::pair<ObjectPtr, AttributePtr>>::iterator anIter = aSelectedPointsList.begin();
    std::set<AttributePtr> aPointsToSkeep;
    for(int anIndex = 0; anIndex < aListSize; anIndex++, anIter++) {
      AttributePtr aFilletPointAttr = (*anIter).second;
      std::shared_ptr<GeomDataAPI_Point2D> aFilletPoint2D =
        std::dynamic_pointer_cast<GeomDataAPI_Point2D>(aFilletPointAttr);
      if(!aFilletPoint2D.get()) {
        myNewPoints.clear();
        setError("Error: One of the selected points is invalid.");
        return;
      }

      // If point or coincident point is already in list remove it from attribute.
      if(aPointsToSkeep.find(aFilletPointAttr) != aPointsToSkeep.end()) {
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
        myNewPoints.clear();
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
            aPointsToSkeep.insert(anAttr);
          }
        } else if(aFeatureKind == SketchPlugin_Line::ID()) {
          AttributePtr anAttrStart = (*anIt)->attribute(SketchPlugin_Line::START_ID());
          std::shared_ptr<GeomDataAPI_Point2D> aPointStart2D =
            std::dynamic_pointer_cast<GeomDataAPI_Point2D>(anAttrStart);
          if(aPointStart2D.get() && aFilletPnt2d->isEqual(aPointStart2D->pnt())) {
            aPointsToSkeep.insert(anAttrStart);
          }
          AttributePtr anAttrEnd = (*anIt)->attribute(SketchPlugin_Line::END_ID());
          std::shared_ptr<GeomDataAPI_Point2D> aPointEnd2D =
            std::dynamic_pointer_cast<GeomDataAPI_Point2D>(anAttrEnd);
          if(aPointEnd2D.get() && aFilletPnt2d->isEqual(aPointEnd2D->pnt())) {
            aPointsToSkeep.insert(anAttrEnd);
          }
          aNewSetOfCoincides.insert(*anIt);
        } else if(aFeatureKind == SketchPlugin_Arc::ID() ) {
          AttributePtr anAttrCenter = (*anIt)->attribute(SketchPlugin_Arc::CENTER_ID());
          std::shared_ptr<GeomDataAPI_Point2D> aPointCenter2D =
            std::dynamic_pointer_cast<GeomDataAPI_Point2D>(anAttrCenter);
          if(aPointCenter2D.get() && aFilletPnt2d->isEqual(aPointCenter2D->pnt())) {
            aPointsToSkeep.insert(anAttrCenter);
            continue;
          }
          AttributePtr anAttrStart = (*anIt)->attribute(SketchPlugin_Arc::START_ID());
          std::shared_ptr<GeomDataAPI_Point2D> aPointStart2D =
            std::dynamic_pointer_cast<GeomDataAPI_Point2D>(anAttrStart);
          if(aPointStart2D.get() && aFilletPnt2d->isEqual(aPointStart2D->pnt())) {
            aPointsToSkeep.insert(anAttrStart);
          }
          AttributePtr anAttrEnd = (*anIt)->attribute(SketchPlugin_Arc::END_ID());
          std::shared_ptr<GeomDataAPI_Point2D> aPointEnd2D =
            std::dynamic_pointer_cast<GeomDataAPI_Point2D>(anAttrEnd);
          if(aPointEnd2D.get() && aFilletPnt2d->isEqual(aPointEnd2D->pnt())) {
            aPointsToSkeep.insert(anAttrEnd);
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
        myNewPoints.clear();
        setError("Error: One of the selected points does not have two suitable edges for fillet.");
        return;
      }

      // Store base point for fillet.
      aPointsToSkeep.insert(aFilletPointAttr);
      myNewPoints.insert(aFilletPointAttr);

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
    if(myRadiusInitialized && !myRadiusChangedInCode) {
      myRadiusChangedByUser = true;
    }
    if(!myRadiusInitialized) {
      myRadiusInitialized = true;
    }
  }
*/
}

//AISObjectPtr SketchPlugin_ConstraintSplit::getAISObject(AISObjectPtr thePrevious)
//{
//  if (!sketch())
//    return thePrevious;
//
//  AISObjectPtr anAIS = thePrevious;
//  /// TODO: Equal constraint presentation should be put here
//  return anAIS;
//}

bool SketchPlugin_ConstraintSplit::isMacro() const
{
  return true;
}

//void SketchPlugin_ConstraintSplit::clearResults()
//{
///*  // Clear auxiliary flag on initial objects.
//  for(std::map<AttributePtr, FilletFeatures>::iterator aPointsIter = myPointFeaturesMap.begin();
//      aPointsIter != myPointFeaturesMap.end();) {
//    const FilletFeatures& aFilletFeatures = aPointsIter->second;
//    std::list<std::pair<FeaturePtr, bool>>::const_iterator aFeatureIt;
//    for(aFeatureIt = aFilletFeatures.baseEdgesState.cbegin();
//        aFeatureIt != aFilletFeatures.baseEdgesState.cend();
//        ++aFeatureIt) {
//      aFeatureIt->first->boolean(SketchPlugin_SketchEntity::AUXILIARY_ID())->setValue(aFeatureIt->second);
//    }
//    ++aPointsIter;
//  }
//
//  // And remove all produced features.
//  DocumentPtr aDoc = sketch()->document();
//  for(std::map<AttributePtr, FilletFeatures>::iterator aPointsIter = myPointFeaturesMap.begin();
//      aPointsIter != myPointFeaturesMap.end();) {
//    // Remove all produced constraints.
//    const FilletFeatures& aFilletFeatures = aPointsIter->second;
//    std::list<FeaturePtr>::const_iterator aFeatureIt;
//    for(aFeatureIt = aFilletFeatures.resultConstraints.cbegin();
//        aFeatureIt != aFilletFeatures.resultConstraints.cend();
//        ++aFeatureIt) {
//      aDoc->removeFeature(*aFeatureIt);
//    }
//
//    // Remove all result edges.
//    for(aFeatureIt = aFilletFeatures.resultEdges.cbegin();
//        aFeatureIt != aFilletFeatures.resultEdges.cend();
//        ++aFeatureIt) {
//      aDoc->removeFeature(*aFeatureIt);
//    }
//
//    // Remove point from map.
//    myPointFeaturesMap.erase(aPointsIter++);
//  }
//*/
//};
//
//
//// =========   Auxiliary functions   =================
//void recalculateAttributes(FeaturePtr theNewArc,  const std::string& theNewArcAttribute,
//                           FeaturePtr theFeature, const std::string& theFeatureAttribute)
//{
//  std::shared_ptr<GeomAPI_Pnt2d> anArcPoint = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
//      theNewArc->attribute(theNewArcAttribute))->pnt();
//  std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
//      theFeature->attribute(theFeatureAttribute))->setValue(anArcPoint->x(), anArcPoint->y());
//}

/*/// \brief Find intersections of lines shifted along normal direction
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

  std::shared_ptr<GeomAPI_Pnt2d> aFilletPnt = SketchPlugin_Tools::getCoincidencePoint(theConstraintCoincidence);

  SketchPlugin_Tools::findCoincidences(theConstraintCoincidence,
                                       SketchPlugin_ConstraintCoincidence::ENTITY_A(),
                                       aCoincides);
  SketchPlugin_Tools::findCoincidences(theConstraintCoincidence,
                                       SketchPlugin_ConstraintCoincidence::ENTITY_B(),
                                       aCoincides);

  // Remove points from set of coincides.
  std::set<FeaturePtr> aNewSetOfCoincides;
  for(std::set<FeaturePtr>::iterator anIt = aCoincides.begin(); anIt != aCoincides.end(); ++anIt) {
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
    for(std::set<FeaturePtr>::iterator anIt = aCoincides.begin(); anIt != aCoincides.end(); ++anIt) {
      if(!(*anIt)->boolean(SketchPlugin_SketchEntity::AUXILIARY_ID())->value()) {
        aNewSetOfCoincides.insert(*anIt);
      }
    }
    aCoincides = aNewSetOfCoincides;
  }

  return aCoincides;
}
*/

std::shared_ptr<GeomDataAPI_Point2D> SketchPlugin_ConstraintSplit::getPointOfRefAttr(
                                                                  const AttributePtr& theAttribute)
{
  AttributePoint2DPtr aPointAttribute;

  if (theAttribute->attributeType() == ModelAPI_AttributeRefAttr::typeId()) {
    AttributeRefAttrPtr aRefAttr = std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(theAttribute);
    if (aRefAttr.get() && aRefAttr->isInitialized()) {
      AttributePtr anAttribute = aRefAttr->attr();
      if (anAttribute.get() && anAttribute->attributeType() == GeomDataAPI_Point2D::typeId())
        aPointAttribute = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(anAttribute);
    }
  }
  return aPointAttribute;
}

void SketchPlugin_ConstraintSplit::getFeaturePoints(AttributePoint2DPtr& theStartPointAttr,
                                                    AttributePoint2DPtr& theEndPointAttr)
{
  AttributePoint2DPtr aPointAttribute;

  AttributeReferencePtr aBaseObjectAttr = std::dynamic_pointer_cast<ModelAPI_AttributeReference>(
                                           data()->attribute(SketchPlugin_Constraint::VALUE()));
  FeaturePtr aBaseFeature = ModelAPI_Feature::feature(aBaseObjectAttr->value());

  std::string aFeatureKind = aBaseFeature->getKind();
  std::string aStartAttributeName, anEndAttributeName;
  if (aFeatureKind == SketchPlugin_Line::ID()) {
    aStartAttributeName = SketchPlugin_Line::START_ID();
    anEndAttributeName = SketchPlugin_Line::END_ID();
  }
  else if (aFeatureKind == SketchPlugin_Arc::ID()) {
    aStartAttributeName = SketchPlugin_Arc::START_ID();
    anEndAttributeName = SketchPlugin_Arc::END_ID();
  }
  if (!aStartAttributeName.empty() && !anEndAttributeName.empty()) {
    theStartPointAttr = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
                                                         aBaseFeature->attribute(aStartAttributeName));
    theEndPointAttr = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
                                                         aBaseFeature->attribute(anEndAttributeName));
  }
}

void SketchPlugin_ConstraintSplit::splitArc(FeaturePtr& theSplitFeature,
                                            FeaturePtr& theBeforeFeature,
                                            FeaturePtr& theAfterFeature)
{
  SketchPlugin_Sketch* aSketch = sketch();
  if (!aSketch)
    return;

  AttributeReferencePtr aBaseObjectAttr = std::dynamic_pointer_cast<ModelAPI_AttributeReference>(
                                           data()->attribute(SketchPlugin_Constraint::VALUE()));
  FeaturePtr aBaseFeature = ModelAPI_Feature::feature(aBaseObjectAttr->value());
  std::string aFeatureKind = aBaseFeature->getKind();
  if (aFeatureKind != SketchPlugin_Arc::ID())
    return;

  AttributePoint2DPtr aFirstPointAttr = getPointOfRefAttr(data()->attribute(SketchPlugin_Constraint::ENTITY_A()));
  AttributePoint2DPtr aSecondPointAttr = getPointOfRefAttr(data()->attribute(SketchPlugin_Constraint::ENTITY_B()));
  AttributePoint2DPtr aStartPointAttr, anEndPointAttr;
  getFeaturePoints(aStartPointAttr, anEndPointAttr);
  if (!aStartPointAttr.get() && !anEndPointAttr.get()) {
    setError("Error: Feature has no start and end points.");
    return;
  }

  arrangePoints(aStartPointAttr, anEndPointAttr, aFirstPointAttr, aSecondPointAttr);

  /// split feature
  theSplitFeature = createArcFeature(aBaseFeature, aFirstPointAttr, aSecondPointAttr);
  static ModelAPI_ValidatorsFactory* aFactory = ModelAPI_Session::get()->validators();
  aFactory->validate(theSplitFeature); // need to be validated to update the "Apply" state if not previewed
  std::string anError = theSplitFeature->error();

  if (!aStartPointAttr->pnt()->isEqual(aFirstPointAttr->pnt())) {
    theBeforeFeature = aBaseFeature; ///< use base feature to store all constraints here
    /// move end arc point to start of split
    fillAttribute(theBeforeFeature->attribute(SketchPlugin_Arc::END_ID()), aFirstPointAttr);
    createConstraint(SketchPlugin_ConstraintCoincidence::ID(),
                     theBeforeFeature->attribute(SketchPlugin_Arc::END_ID()),
                     theSplitFeature->attribute(SketchPlugin_Arc::START_ID()));
  }

  if (!aSecondPointAttr->pnt()->isEqual(anEndPointAttr->pnt())) {
    if (!theBeforeFeature) {
      theAfterFeature = aBaseFeature; ///< use base feature to store all constraints here
      fillAttribute(theAfterFeature->attribute(SketchPlugin_Arc::START_ID()), aSecondPointAttr);
    }
    else
      theAfterFeature = createArcFeature(aBaseFeature, aSecondPointAttr, anEndPointAttr);

    createConstraint(SketchPlugin_ConstraintCoincidence::ID(),
                     theSplitFeature->attribute(SketchPlugin_Arc::END_ID()),
                     theAfterFeature->attribute(SketchPlugin_Arc::START_ID()));
  }

  // additional constraints between split and base features
  createConstraint(SketchPlugin_ConstraintEqual::ID(), getFeatureResult(aBaseFeature),
                                                       getFeatureResult(theSplitFeature));
  createConstraint(SketchPlugin_ConstraintTangent::ID(), getFeatureResult(theSplitFeature),
                                                         getFeatureResult(aBaseFeature));
  if (theAfterFeature.get()) {
    createConstraint(SketchPlugin_ConstraintEqual::ID(), getFeatureResult(aBaseFeature),
                                                         getFeatureResult(theAfterFeature));
    createConstraint(SketchPlugin_ConstraintTangent::ID(), getFeatureResult(theSplitFeature),
                                                         getFeatureResult(theAfterFeature));
  }
}

void SketchPlugin_ConstraintSplit::arrangePoints(const AttributePoint2DPtr& theStartPointAttr,
                                                 const AttributePoint2DPtr& theEndPointAttr,
                                                 AttributePoint2DPtr& theFirstPointAttr,
                                                 AttributePoint2DPtr& theLastPointAttr)
{
  /// if first point is closer to last point, wrap first and last values
  if (theStartPointAttr->pnt()->distance(theFirstPointAttr->pnt()) >
      theEndPointAttr->pnt()->distance(theLastPointAttr->pnt())) {
    AttributePoint2DPtr aTmpPoint = theFirstPointAttr;
    theFirstPointAttr = theLastPointAttr;
    theLastPointAttr = aTmpPoint;
  }
}

void SketchPlugin_ConstraintSplit::fillAttribute(const AttributePtr& theModifiedAttribute,
                                                 const AttributePtr& theSourceAttribute)
{
  AttributePoint2DPtr aModifiedAttribute = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
                                            theModifiedAttribute);
  AttributePoint2DPtr aSourceAttribute = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
                                            theSourceAttribute);

  if (aModifiedAttribute.get() && aSourceAttribute.get())
    aSourceAttribute->setValue(aModifiedAttribute->pnt());
}

FeaturePtr SketchPlugin_ConstraintSplit::createArcFeature(const FeaturePtr& theBaseFeature,
                                                          const AttributePtr& theFirstPointAttr,
                                                          const AttributePtr& theSecondPointAttr)
{
  FeaturePtr aFeature;
  SketchPlugin_Sketch* aSketch = sketch();
  if (!aSketch || !theBaseFeature.get())
    return aFeature;

  aFeature = aSketch->addFeature(theBaseFeature->getKind());
  // update fillet arc: make the arc correct for sure, so, it is not needed to process the "attribute updated"
  // by arc; moreover, it may cause cyclicity in hte mechanism of updater
  aFeature->data()->blockSendAttributeUpdated(true);

  aFeature->string(SketchPlugin_Arc::ARC_TYPE())->setValue(
                SketchPlugin_Arc::ARC_TYPE_CENTER_START_END());
  fillAttribute(aFeature->attribute(SketchPlugin_Arc::CENTER_ID()),
                theBaseFeature->attribute(SketchPlugin_Arc::CENTER_ID()));
  fillAttribute(aFeature->attribute(SketchPlugin_Arc::START_ID()), theFirstPointAttr);
  fillAttribute(aFeature->attribute(SketchPlugin_Arc::END_ID()), theSecondPointAttr);
  aFeature->data()->blockSendAttributeUpdated(false);
  aFeature->execute();

  return aFeature;
}

void SketchPlugin_ConstraintSplit::createConstraint(const std::string& theConstraintId,
                                                    const AttributePtr& theFirstAttribute,
                                                    const AttributePtr& theSecondAttribute)
{
  FeaturePtr aConstraint = sketch()->addFeature(theConstraintId);
  AttributeRefAttrPtr aRefAttr = std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(
                                 aConstraint->attribute(SketchPlugin_Constraint::ENTITY_A()));
  aRefAttr->setAttr(theFirstAttribute);

  aRefAttr = std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(
                                 aConstraint->attribute(SketchPlugin_Constraint::ENTITY_B()));
  aRefAttr->setAttr(theSecondAttribute);
}

void SketchPlugin_ConstraintSplit::createConstraint(const std::string& theConstraintId,
                                                    const ObjectPtr& theFirstObject,
                                                    const ObjectPtr& theSecondObject)
{
  FeaturePtr aConstraint = sketch()->addFeature(theConstraintId);
  AttributeRefAttrPtr aRefAttr = std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(
                                 aConstraint->attribute(SketchPlugin_Constraint::ENTITY_A()));
  aRefAttr->setObject(theFirstObject);

  aRefAttr = std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(
                                 aConstraint->attribute(SketchPlugin_Constraint::ENTITY_B()));
  aRefAttr->setObject(theSecondObject);
}

std::shared_ptr<ModelAPI_Object> SketchPlugin_ConstraintSplit::getFeatureResult(
                                    const std::shared_ptr<ModelAPI_Feature>& theFeature)
{
  std::shared_ptr<ModelAPI_Object> aResult;

  std::string aFeatureKind = theFeature->getKind();
  if (aFeatureKind == SketchPlugin_Line::ID())
    aResult = theFeature->firstResult();
  else if (aFeatureKind == SketchPlugin_Arc::ID())
    aResult = theFeature->lastResult();
  else if (aFeatureKind == SketchPlugin_Circle::ID())
    aResult = theFeature->lastResult();

  return aResult;
}

