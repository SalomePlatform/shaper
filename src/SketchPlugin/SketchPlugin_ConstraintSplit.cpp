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
#include <ModelAPI_Tools.h>

#include <ModelAPI_Validator.h>
#include <ModelAPI_Session.h>

#include <SketchPlugin_Line.h>
#include <SketchPlugin_Arc.h>
#include <SketchPlugin_Circle.h>
#include <SketchPlugin_ConstraintCoincidence.h>
#include <SketchPlugin_ConstraintEqual.h>
#include <SketchPlugin_ConstraintParallel.h>
#include <SketchPlugin_ConstraintTangent.h>
#include <SketchPlugin_ConstraintMirror.h>
#include <SketchPlugin_MultiRotation.h>
#include <SketchPlugin_MultiTranslation.h>

//#include <ModelAPI_Data.h>
#include <ModelAPI_Events.h>
//#include <ModelAPI_Session.h>
//#include <ModelAPI_Validator.h>
//
#include <SketchPlugin_Line.h>
#include <SketchPlugin_Arc.h>
#include <SketchPlugin_Circle.h>
//#include <SketchPlugin_Point.h>
//#include <SketchPlugin_Sketch.h>
//#include <SketchPlugin_Tools.h>
//

#include <ModelGeomAlgo_Point2D.h>
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


  // Find feature constraints
  FeaturePtr aBaseFeature = ModelAPI_Feature::feature(aBaseObjectAttr->value());
  ResultPtr aBaseFeatureResult = getFeatureResult(aBaseFeature);

  std::set<FeaturePtr> aFeaturesToDelete;
  std::map<FeaturePtr, IdToPointPair> aTangentFeatures;
  std::map<FeaturePtr, IdToPointPair> aCoincidenceToFeature;
  std::map<FeaturePtr, IdToPointPair> aCoincidenceToPoint;
  getConstraints(aFeaturesToDelete, aTangentFeatures, aCoincidenceToFeature, aCoincidenceToPoint);


  std::string aFeatureKind = aBaseFeature->getKind();
  FeaturePtr aSplitFeature, anAfterFeature;
  std::set<AttributePoint2DPtr> aFurtherCoincidences;
  /*if (aFeatureKind == SketchPlugin_Line::ID())
    splitLine(aSplitFeature, anOtherFeatures);
  else*/ if (aFeatureKind == SketchPlugin_Arc::ID())
    splitArc(aSplitFeature, aBaseFeature, anAfterFeature, aFurtherCoincidences);
  /*if (aFeatureKind == SketchPlugin_Circle::ID())
    splitCircle(aSplitFeature, anOtherFeatures);
  FeaturePtr aSplitFeature;
  std::set<FeaturePtr> anOtherFeatures;*/

  //setConstraints(aSplitFeature, aBaseFeature, anAfterFeature, aFeaturesToDelete, aTangentFeatures,
  //               aCoincidenceToFeature, aCoincidenceToPoint);
  if (true) {
  std::set<ResultPtr> aFeatureResults;
  aFeatureResults.insert(getFeatureResult(aBaseFeature));
  if (anAfterFeature.get() && anAfterFeature != aBaseFeature)
    aFeatureResults.insert(getFeatureResult(anAfterFeature));

  // coincidence to feature
  std::map<FeaturePtr, IdToPointPair>::const_iterator aCIt = aCoincidenceToFeature.begin(),
                                                            aCLast = aCoincidenceToFeature.end();
  for (; aCIt != aCLast; aCIt++) {
    FeaturePtr aCoincFeature = aCIt->first;
    std::string anAttributeId = aCIt->second.first;
    AttributePoint2DPtr aCoincPoint = aCIt->second.second;
    std::set<AttributePoint2DPtr>::const_iterator aFCIt = aFurtherCoincidences.begin(),
                                                  aFCLast = aFurtherCoincidences.end();
    std::shared_ptr<GeomAPI_Pnt2d> aCoincPnt = aCoincPoint->pnt();
    AttributePoint2DPtr aFeaturePointAttribute;
    for (; aFCIt != aFCLast && !aFeaturePointAttribute.get(); aFCIt++) {
      AttributePoint2DPtr aFCAttribute = *aFCIt;
      if (aCoincPnt->isEqual(aFCAttribute->pnt()))
        aFeaturePointAttribute = aFCAttribute;
    }
    if (aFeaturePointAttribute.get()) {
      aCoincFeature->refattr(anAttributeId)->setAttr(aFeaturePointAttribute);
    }
    else {
      /// find feature by shape intersected the point
      ResultPtr aResultForCoincidence = *(aFeatureResults.begin());

      if (aFeatureResults.size() > 1) { // try to find point on additional feature
        ResultPtr anAddtionalResult = *(aFeatureResults.begin()++);
        GeomShapePtr aShape = anAddtionalResult->shape();

        std::shared_ptr<GeomAPI_Pnt2d> aPnt2d = aCoincPoint->pnt();
        std::shared_ptr<GeomAPI_Pnt> aPoint = sketch()->to3D(aPnt2d->x(), aPnt2d->y());

        std::shared_ptr<GeomAPI_Pnt> aProjectedPoint;
        if (ModelGeomAlgo_Point2D::isPointOnEdge(aShape, aPoint, aProjectedPoint))
          aResultForCoincidence = anAddtionalResult;
      }
      aCoincFeature->refattr(anAttributeId)->setObject(aResultForCoincidence);
    }
  }
  // coincidence to points
  // TODO
  // tangency
  // TODO
  }
  // delete constraints
  ModelAPI_Tools::removeFeaturesAndReferences(aFeaturesToDelete);

  // Send events to update the sub-features by the solver.
  if(isUpdateFlushed) {
    Events_Loop::loop()->setFlushed(anUpdateEvent, true);
  }
}

bool SketchPlugin_ConstraintSplit::isMacro() const
{
  return true;
}

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

void SketchPlugin_ConstraintSplit::getConstraints(std::set<FeaturePtr>& theFeaturesToDelete,
                                      std::map<FeaturePtr, IdToPointPair>& theTangentFeatures,
                                      std::map<FeaturePtr, IdToPointPair>& theCoincidenceToFeature,
                                      std::map<FeaturePtr, IdToPointPair>& theCoincidenceToPoint)
{
  std::shared_ptr<ModelAPI_Data> aData = data();

  // Check the base objects are initialized.
  AttributeReferencePtr aBaseObjectAttr = std::dynamic_pointer_cast<ModelAPI_AttributeReference>(
                                            aData->attribute(SketchPlugin_Constraint::VALUE()));
  FeaturePtr aBaseFeature = ModelAPI_Feature::feature(aBaseObjectAttr->value());
  ResultPtr aBaseFeatureResult = getFeatureResult(aBaseFeature);

  const std::set<AttributePtr>& aRefsList = aBaseFeatureResult->data()->refsToMe();
  std::set<AttributePtr>::const_iterator aIt;
  for (aIt = aRefsList.cbegin(); aIt != aRefsList.cend(); ++aIt) {
    std::shared_ptr<ModelAPI_Attribute> aAttr = (*aIt);
    FeaturePtr aRefFeature = std::dynamic_pointer_cast<ModelAPI_Feature>(aAttr->owner());
    std::string aRefFeatureKind = aRefFeature->getKind();
    if (aRefFeatureKind == SketchPlugin_ConstraintMirror::ID() ||
        aRefFeatureKind == SketchPlugin_MultiRotation::ID() ||
        aRefFeatureKind == SketchPlugin_MultiTranslation::ID())
      theFeaturesToDelete.insert(aRefFeature);
    else if (aRefFeatureKind == SketchPlugin_ConstraintTangent::ID()) {
      if (aBaseFeature->getKind() == SketchPlugin_Circle::ID()) /// TEMPORARY limitaion
        theFeaturesToDelete.insert(aRefFeature); /// until tangency between arc and line is implemented
      else {
        std::string anAttributeToBeModified;
        AttributePoint2DPtr aTangentPoint;
        ObjectPtr aResult1 = aRefFeature->refattr(SketchPlugin_Constraint::ENTITY_A())->object();
        ObjectPtr aResult2 = aRefFeature->refattr(SketchPlugin_Constraint::ENTITY_B())->object();
        if (aResult1.get() && aResult2.get()) {
          FeaturePtr aCoincidenceFeature = SketchPlugin_ConstraintCoincidence::findCoincidenceFeature
                                                                    (ModelAPI_Feature::feature(aResult1),
                                                                     ModelAPI_Feature::feature(aResult2));
          aTangentPoint = SketchPlugin_ConstraintCoincidence::getPoint(aCoincidenceFeature);
        }
        if (aTangentPoint.get()) {
          FeaturePtr aFeature1 = ModelAPI_Feature::feature(aResult1);
          std::string anAttributeToBeModified = aFeature1 == aBaseFeature
                       ? SketchPlugin_Constraint::ENTITY_B() : SketchPlugin_Constraint::ENTITY_A();
          theTangentFeatures[aRefFeature] = std::make_pair(anAttributeToBeModified, aTangentPoint);
        }
        else
          theFeaturesToDelete.insert(aRefFeature); /// there is not coincident point between tangent constraint
      }
    }
    else if (aRefFeatureKind == SketchPlugin_ConstraintCoincidence::ID()) {
      std::string anAttributeToBeModified;
      AttributePoint2DPtr aCoincidentPoint;
      AttributeRefAttrPtr anAttrA = aRefFeature->refattr(SketchPlugin_Constraint::ENTITY_A());
      AttributeRefAttrPtr anAttrB = aRefFeature->refattr(SketchPlugin_Constraint::ENTITY_B());
      bool isToFeature = false;
      if (anAttrA->isObject() || anAttrB->isObject()) { /// coincidence to base feature
        FeaturePtr aFeature = anAttrA->isObject() ? ModelAPI_Feature::feature(anAttrA->object())
                                                  : FeaturePtr();
        isToFeature = aFeature.get() && aFeature == aBaseFeature;
        anAttributeToBeModified = SketchPlugin_Constraint::ENTITY_B();
        if (!isToFeature) {
          aFeature = anAttrB->isObject() ? ModelAPI_Feature::feature(anAttrB->object())
                                         : FeaturePtr();
          isToFeature = aFeature.get() && aFeature == aBaseFeature;
          anAttributeToBeModified = SketchPlugin_Constraint::ENTITY_A();
        }
        if (isToFeature)
          aCoincidentPoint = SketchPlugin_ConstraintCoincidence::getPoint(aRefFeature);
      }
      if (!isToFeature) { /// coincidence to point on base feature
        AttributePtr anAttribute;
        if (!anAttrA->isObject()) {
          AttributePtr aCurAttribute = anAttrA->attr();
          if (aCurAttribute.get()) {
            FeaturePtr aCurFeature = ModelAPI_Feature::feature(aCurAttribute->owner());
            if (aCurFeature.get() && aCurFeature == aBaseFeature) {
              anAttribute = anAttrA->attr();
              anAttributeToBeModified = SketchPlugin_Constraint::ENTITY_A();
            }
          }
        }
        if (!anAttribute.get() && !anAttrB->isObject()) {
          AttributePtr aCurAttribute = anAttrB->attr();
          if (aCurAttribute.get()) {
            FeaturePtr aCurFeature = ModelAPI_Feature::feature(aCurAttribute->owner());
            if (aCurFeature.get() && aCurFeature == aBaseFeature) {
              anAttribute = anAttrB->attr();
              anAttributeToBeModified = SketchPlugin_Constraint::ENTITY_B();
            }
          }
        }
        if (anAttribute.get())
          aCoincidentPoint = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(anAttribute);
      }
      if (aCoincidentPoint.get()) {
        if (isToFeature)
          theCoincidenceToFeature[aRefFeature] = std::make_pair(anAttributeToBeModified,
                                                                aCoincidentPoint);
        else
          theCoincidenceToPoint[aRefFeature] = std::make_pair(anAttributeToBeModified,
                                                              aCoincidentPoint);
      }
      else
        theFeaturesToDelete.insert(aRefFeature); /// this case should not happen
    }
  }
}

/*void SketchPlugin_ConstraintSplit::setConstraints(const FeaturePtr& theSplitFeature,
                                                  const FeaturePtr& theBaseFeature,
                                                  const FeaturePtr& theAfterFeature,
    const std::set<std::shared_ptr<ModelAPI_Feature>>& theFeaturesToDelete,
    const std::map<std::shared_ptr<ModelAPI_Feature>, std::shared_ptr<GeomDataAPI_Point2D> >& theTangentFeatures,
    const std::map<std::shared_ptr<ModelAPI_Feature>, std::shared_ptr<GeomDataAPI_Point2D> >& theCoincidenceToFeature,
    const std::map<std::shared_ptr<ModelAPI_Feature>, std::shared_ptr<GeomDataAPI_Point2D> >& theCoincidenceToPoint)
{
  // coincidence to feature


  // coincidence to points

  // tangency

  // delete constraints

}*/

void SketchPlugin_ConstraintSplit::splitArc(FeaturePtr& theSplitFeature,
                                            FeaturePtr& theBaseFeature,
                                            FeaturePtr& theAfterFeature,
                                            std::set<AttributePoint2DPtr>& thePoints)
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

  // before split feature
  if (!aStartPointAttr->pnt()->isEqual(aFirstPointAttr->pnt())) {
    theBaseFeature = aBaseFeature; ///< use base feature to store all constraints here
    /// move end arc point to start of split
  }

  // after split feature
  if (!aSecondPointAttr->pnt()->isEqual(anEndPointAttr->pnt())) {
    FeaturePtr aFeature;
    if (!theBaseFeature.get()) {
      aFeature = aBaseFeature; ///< use base feature to store all constraints here
      fillAttribute(aFeature->attribute(SketchPlugin_Arc::START_ID()), aSecondPointAttr);
      aFeature->execute(); // to update result
    }
    else
      aFeature = createArcFeature(aBaseFeature, aSecondPointAttr, anEndPointAttr);

    createConstraint(SketchPlugin_ConstraintCoincidence::ID(),
                     theSplitFeature->attribute(SketchPlugin_Arc::END_ID()),
                     aFeature->attribute(SketchPlugin_Arc::START_ID()));
    thePoints.insert(std::dynamic_pointer_cast<GeomDataAPI_Point2D>
                                (theBaseFeature->attribute(SketchPlugin_Arc::START_ID())));
    thePoints.insert(std::dynamic_pointer_cast<GeomDataAPI_Point2D>
                                (theBaseFeature->attribute(SketchPlugin_Arc::END_ID())));

    if (!theBaseFeature.get())
      theBaseFeature = aFeature;
    else
      theAfterFeature = aFeature;
  }
  else
    thePoints.insert(std::dynamic_pointer_cast<GeomDataAPI_Point2D>
                                  (theSplitFeature->attribute(SketchPlugin_Arc::END_ID())));

  // base split, that is defined before split feature should be changed at end
  // (after the after feature creation). Otherwise modified value will be used in after feature
  // before split feature
  if (!aStartPointAttr->pnt()->isEqual(aFirstPointAttr->pnt())) {
    /// move end arc point to start of split
    fillAttribute(theBaseFeature->attribute(SketchPlugin_Arc::END_ID()), aFirstPointAttr);
    theBaseFeature->execute(); // to update result
    createConstraint(SketchPlugin_ConstraintCoincidence::ID(),
                     theBaseFeature->attribute(SketchPlugin_Arc::END_ID()),
                     theSplitFeature->attribute(SketchPlugin_Arc::START_ID()));
    thePoints.insert(std::dynamic_pointer_cast<GeomDataAPI_Point2D>
                                        (theBaseFeature->attribute(SketchPlugin_Arc::START_ID())));
    thePoints.insert(std::dynamic_pointer_cast<GeomDataAPI_Point2D>
                                        (theBaseFeature->attribute(SketchPlugin_Arc::END_ID())));
  }
  else
    thePoints.insert(std::dynamic_pointer_cast<GeomDataAPI_Point2D>
                                       (theSplitFeature->attribute(SketchPlugin_Arc::START_ID())));

  // additional constraints between split and base features
  createConstraintForObjects(SketchPlugin_ConstraintEqual::ID(), getFeatureResult(aBaseFeature),
                                                       getFeatureResult(theSplitFeature));
  createConstraintForObjects(SketchPlugin_ConstraintTangent::ID(), getFeatureResult(theSplitFeature),
                                                         getFeatureResult(aBaseFeature));
  if (theAfterFeature.get()) {
    createConstraintForObjects(SketchPlugin_ConstraintEqual::ID(), getFeatureResult(aBaseFeature),
                                                         getFeatureResult(theAfterFeature));
    createConstraintForObjects(SketchPlugin_ConstraintTangent::ID(), getFeatureResult(theSplitFeature),
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
      theStartPointAttr->pnt()->distance(theLastPointAttr->pnt())) {
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
    aModifiedAttribute->setValue(aSourceAttribute->pnt());
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
  aFeature->execute(); // to obtain result

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

void SketchPlugin_ConstraintSplit::createConstraintForObjects(const std::string& theConstraintId,
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

std::shared_ptr<ModelAPI_Result> SketchPlugin_ConstraintSplit::getFeatureResult(
                                    const std::shared_ptr<ModelAPI_Feature>& theFeature)
{
  std::shared_ptr<ModelAPI_Result> aResult;

  std::string aFeatureKind = theFeature->getKind();
  if (aFeatureKind == SketchPlugin_Line::ID())
    aResult = theFeature->firstResult();
  else if (aFeatureKind == SketchPlugin_Arc::ID())
    aResult = theFeature->lastResult();
  else if (aFeatureKind == SketchPlugin_Circle::ID())
    aResult = theFeature->lastResult();

  return aResult;
}

