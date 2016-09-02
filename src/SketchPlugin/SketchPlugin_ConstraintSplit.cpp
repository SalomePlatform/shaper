// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:    SketchPlugin_ConstraintSplit.cpp
// Created: 25 Aug 2016
// Author:  Natalia ERMOLAEVA

#include "SketchPlugin_ConstraintSplit.h"

#include <GeomAPI_Dir2d.h>
#include <GeomAPI_Pnt2d.h>
#include <GeomAPI_XY.h>
#include <GeomDataAPI_Point2D.h>
#include <ModelAPI_AttributeReference.h>
#include <ModelAPI_AttributeString.h>
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

#include <ModelAPI_Events.h>
#include <SketchPlugin_Line.h>
#include <SketchPlugin_Arc.h>
#include <SketchPlugin_Circle.h>

#include <ModelGeomAlgo_Point2D.h>
#include <Events_Loop.h>

#include <cmath>

#define DEBUG_SPLIT
#ifdef DEBUG_SPLIT
#include <iostream>
#endif

static const double PI = 3.141592653589793238463;

SketchPlugin_ConstraintSplit::SketchPlugin_ConstraintSplit()
{
}

void SketchPlugin_ConstraintSplit::initAttributes()
{
  data()->addAttribute(SketchPlugin_Constraint::VALUE(), ModelAPI_AttributeReference::typeId());
  data()->addAttribute(SketchPlugin_Constraint::ENTITY_A(), ModelAPI_AttributeRefAttr::typeId());
  data()->addAttribute(SketchPlugin_Constraint::ENTITY_B(), ModelAPI_AttributeRefAttr::typeId());
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
  AttributePoint2DPtr aFirstPointAttrOfSplit = getPointOfRefAttr(aData->attribute(SketchPlugin_Constraint::ENTITY_A()));
  AttributePoint2DPtr aSecondPointAttrOfSplit = getPointOfRefAttr(aData->attribute(SketchPlugin_Constraint::ENTITY_B()));
  if (!aFirstPointAttrOfSplit.get() || !aFirstPointAttrOfSplit->isInitialized() ||
      !aSecondPointAttrOfSplit.get() || !aSecondPointAttrOfSplit->isInitialized()) {
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
  //std::map<FeaturePtr, IdToPointPair> aCoincidenceToPoint;
  getConstraints(aFeaturesToDelete, aTangentFeatures, aCoincidenceToFeature);//, aCoincidenceToPoint);

  std::map<AttributePtr, std::list<AttributePtr> > aBaseRefAttributes;
  getRefAttributes(aBaseFeature, aBaseRefAttributes);

  std::map<AttributePtr, AttributePtr> aBasePointModifiedAttributes;

#ifdef DEBUG_SPLIT
  std::cout << std::endl;
  std::cout << "SketchPlugin_ConstraintSplit::execute()" << std::endl;
  std::cout << std::endl;

  SketchPlugin_Sketch* aSketch = sketch();
  std::cout << "SKETCH FEATURES (before split) [" << aSketch->numberOfSubs() << "]:" << std::endl;
  for (int i = 0, aNbSubs = aSketch->numberOfSubs(); i < aNbSubs; i++) {
    std::cout << getFeatureInfo(aSketch->subFeature(i), false) << std::endl;
  }

  std::cout << std::endl;
  std::cout << "---- IN PARAMETERS ----" << std::endl;
  std::cout << "Base feature:" << getFeatureInfo(aBaseFeature) << std::endl;
  std::cout << std::endl;

  if (!aCoincidenceToFeature.empty()) {
    std::cout << "Coincidences to base feature[" << aCoincidenceToFeature.size() << "]: " << std::endl;
    std::map<FeaturePtr, IdToPointPair>::const_iterator anIt = aCoincidenceToFeature.begin(),
                                                        aLast = aCoincidenceToFeature.end();
    for (int i = 1; anIt != aLast; anIt++, i++) {
      FeaturePtr aFeature = (*anIt).first;
      std::string anAttributeId = (*anIt).second.first;
      std::shared_ptr<GeomDataAPI_Point2D> aPointAttr = (*anIt).second.second;

      std::cout << i << "-" << getFeatureInfo(aFeature) << std::endl;
      std::cout <<     " -Attribute to correct:" << anAttributeId << std::endl;
      std::cout <<     " -Point attribute:" << ModelGeomAlgo_Point2D::getPointAttributeInfo(aPointAttr) << std::endl;
    }
  }

  if (!aTangentFeatures.empty()) {
    std::cout << std::endl;
    std::cout << "Tangencies to base feature[" << aTangentFeatures.size() << "]: " << std::endl;
    std::map<FeaturePtr, IdToPointPair>::const_iterator anIt = aTangentFeatures.begin(),
                                                        aLast = aTangentFeatures.end();
    for (int i = 1; anIt != aLast; anIt++, i++) {
      FeaturePtr aFeature = (*anIt).first;
      std::string anAttributeId = (*anIt).second.first;
      std::shared_ptr<GeomDataAPI_Point2D> aPointAttr = (*anIt).second.second;

      std::cout << i << "-" << getFeatureInfo(aFeature) << std::endl;
      std::cout <<     " -Attribute to correct:" << anAttributeId << std::endl;
      std::cout <<     " -Point attribute:" << ModelGeomAlgo_Point2D::getPointAttributeInfo(aPointAttr) << std::endl;
    }
  }

  /*if (!aCoincidenceToPoint.empty()) {
    std::cout << std::endl;
    std::cout << "Coincidences to points on base feature[" << aCoincidenceToPoint.size() << "]: " << std::endl;
    std::map<FeaturePtr, IdToPointPair>::const_iterator anIt = aCoincidenceToPoint.begin(),
                                                        aLast = aCoincidenceToPoint.end();
    for (int i = 1; anIt != aLast; anIt++, i++) {
      FeaturePtr aFeature = (*anIt).first;
      std::string anAttributeId = (*anIt).second.first;
      std::shared_ptr<GeomDataAPI_Point2D> aPointAttr = (*anIt).second.second;

      std::cout << i << "-" << getFeatureInfo(aFeature) << std::endl;
      std::cout <<     " -Attribute to correct:" << anAttributeId << std::endl;
      std::cout <<     " -Point attribute:" << ModelGeomAlgo_Point2D::getPointAttributeInfo(aPointAttr) << std::endl;
    }
  }*/
  std::map<AttributePtr, std::list<AttributePtr> >::const_iterator aRefIt = aBaseRefAttributes.begin(),
                                                                   aRefLast = aBaseRefAttributes.end();
  std::cout << std::endl << "References to attributes of base feature [" << aBaseRefAttributes.size() << "]" << std::endl;
  for (; aRefIt != aRefLast; aRefIt++) {
    AttributePtr aBaseAttr = aRefIt->first;
    std::list<AttributePtr> aRefAttributes = aRefIt->second;
    std::string aRefsInfo;
    std::list<AttributePtr>::const_iterator aRefAttrIt = aRefAttributes.begin(),
                                            aRefAttrLast = aRefAttributes.end();
    for (; aRefAttrIt != aRefAttrLast; aRefAttrIt++) {
      if (!aRefsInfo.empty())
        aRefsInfo.append(",");
      AttributePtr aRAttr = *aRefAttrIt;
      aRefsInfo.append(aRAttr->id());
      FeaturePtr aRFeature = ModelAPI_Feature::feature(aRAttr->owner());
      aRefsInfo.append("(" + aRFeature->name() + ") ");
    }
    std::shared_ptr<GeomDataAPI_Point2D> aPointAttr = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(aBaseAttr);
    std::cout << aPointAttr->id().c_str() << ": " << "[" << aRefAttributes.size() << "] " << aRefsInfo << std::endl;
  }
  std::cout << std::endl;
  std::cout << "---- SPLIT ----" << std::endl;
  std::cout << std::endl;
#endif

  std::string aFeatureKind = aBaseFeature->getKind();
  FeaturePtr aSplitFeature, anAfterFeature;
  std::set<AttributePoint2DPtr> aFurtherCoincidences;
  std::set<FeaturePtr> aCreatedFeatures;
  std::set<std::pair<AttributePtr, AttributePtr>> aModifiedAttributes;
  if (aFeatureKind == SketchPlugin_Line::ID())
    splitLine(aSplitFeature, aBaseFeature, anAfterFeature, aFurtherCoincidences, aCreatedFeatures,
              aModifiedAttributes);
  else if (aFeatureKind == SketchPlugin_Arc::ID())
    splitArc(aSplitFeature, aBaseFeature, anAfterFeature, aFurtherCoincidences, aCreatedFeatures,
             aModifiedAttributes);
  if (aFeatureKind == SketchPlugin_Circle::ID()) {
    FeaturePtr aCircleFeature = aBaseFeature;
    splitCircle(aSplitFeature, aBaseFeature, anAfterFeature, aFurtherCoincidences, aCreatedFeatures,
                aModifiedAttributes);
    aFeaturesToDelete.insert(aCircleFeature);
    aBaseObjectAttr->setObject(ResultPtr()); // as circle is removed, temporary fill this attribute
  }

#ifdef DEBUG_SPLIT
  std::cout << "---- OUT PARAMETERS ----" << std::endl;
  std::cout << "Base modified feature:" << getFeatureInfo(aBaseFeature) << std::endl;
  std::cout << "Split feature:" << getFeatureInfo(aSplitFeature) << std::endl;
  std::cout << "After feature:" << getFeatureInfo(anAfterFeature) << std::endl;
  std::cout << std::endl;

  std::cout << "Created features by split:[" << aCreatedFeatures.size() << "]" << std::endl;
  std::set<FeaturePtr>::const_iterator aFIt = aCreatedFeatures.begin(),
                                       aFLast = aCreatedFeatures.end();
  for (; aFIt != aFLast; aFIt++) {
    std::cout << getFeatureInfo(*aFIt) << std::endl;
  }
  std::cout << std::endl;

  std::cout << "Attributes for further Coincidences:" << std::endl;
  std::set<AttributePoint2DPtr>::const_iterator anIt = aFurtherCoincidences.begin(),
                                                aLast = aFurtherCoincidences.end();
  for (; anIt != aLast; anIt++) {
    AttributePtr anAttribute = *anIt;
    FeaturePtr aFeature = ModelAPI_Feature::feature(anAttribute->owner());
    std::cout << ModelGeomAlgo_Point2D::getPointAttributeInfo(anAttribute)
              << " [" << getFeatureInfo(aFeature, false) << "]" << std::endl;
  }

  std::cout << "Modifed attributes (constraints to attributes are moved here):" << std::endl;
  std::set<std::pair<AttributePtr, AttributePtr> >::const_iterator aPIt = aModifiedAttributes.begin(),
                                                                   aPLast = aModifiedAttributes.end();
  std::string aResInfo;
  for (; aPIt != aPLast; aPIt++) {
    if (!aResInfo.empty())
      aResInfo += "\n";

    std::pair<AttributePtr, AttributePtr> aPair = *aPIt;

    AttributePtr anAttr = aPair.first;
    aResInfo.append(anAttr->id());
    FeaturePtr aFeature = ModelAPI_Feature::feature(anAttr->owner());
    aResInfo.append("(" + aFeature->name() + ") ");

    aResInfo.append("  - is modified to -  ");

    anAttr = aPair.second;
    aResInfo.append(anAttr->id());
    aFeature = ModelAPI_Feature::feature(anAttr->owner());
    aResInfo.append("(" + aFeature->name() + ") ");
  }
  std::cout << aResInfo << std::endl;
#endif

  std::set<ResultPtr> aFeatureResults;
  aFeatureResults.insert(getFeatureResult(aBaseFeature));
  if (anAfterFeature.get() && anAfterFeature != aBaseFeature)
    aFeatureResults.insert(getFeatureResult(anAfterFeature));

  // coincidence to feature
  updateCoincidenceConstraintsToFeature(aCoincidenceToFeature, aFurtherCoincidences,
                                        aFeatureResults);
  // coincidence to points
  //updateCoincidenceConstraintsToFeature(aCoincidenceToPoint, aFurtherCoincidences,
  //                                      std::set<ResultPtr>());
  // tangency
  updateTangentConstraintsToFeature(aTangentFeatures, aFurtherCoincidences);

  updateRefAttConstraints(aBaseRefAttributes, aModifiedAttributes);

  // delete constraints
#ifdef DEBUG_SPLIT
  std::cout << "remove features and references:" << std::endl;
  std::set<FeaturePtr>::const_iterator aDIt = aFeaturesToDelete.begin(),
                                       aDLast = aFeaturesToDelete.end();
  for (; aDIt != aDLast; aDIt++) {
    std::cout << getFeatureInfo(*aDIt, false) << std::endl;
    std::cout << std::endl;
  }
#endif
  ModelAPI_Tools::removeFeaturesAndReferences(aFeaturesToDelete);

  // Send events to update the sub-features by the solver.
  if(isUpdateFlushed) {
    Events_Loop::loop()->setFlushed(anUpdateEvent, true);
  }

#ifdef DEBUG_SPLIT
  std::cout << "SKETCH FEATURES (after split) [" << aSketch->numberOfSubs() << "]:" << std::endl;
  for (int i = 0, aNbSubs = aSketch->numberOfSubs(); i < aNbSubs; i++) {
    std::cout << getFeatureInfo(aSketch->subFeature(i), false) << std::endl;
  }
#endif
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
                                      std::map<FeaturePtr, IdToPointPair>& theCoincidenceToFeature/*,
                                      std::map<FeaturePtr, IdToPointPair>& theCoincidenceToPoint*/)
{
  std::shared_ptr<ModelAPI_Data> aData = data();

  // Check the base objects are initialized.
  AttributeReferencePtr aBaseObjectAttr = std::dynamic_pointer_cast<ModelAPI_AttributeReference>(
                                            aData->attribute(SketchPlugin_Constraint::VALUE()));
  FeaturePtr aBaseFeature = ModelAPI_Feature::feature(aBaseObjectAttr->value());
  ResultPtr aBaseFeatureResult = getFeatureResult(aBaseFeature);

  std::set<AttributePtr> aRefsList = aBaseFeatureResult->data()->refsToMe();
  std::set<AttributePtr> aFRefsList = aBaseFeature->data()->refsToMe();
  aRefsList.insert(aFRefsList.begin(), aFRefsList.end());

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
                       ? SketchPlugin_Constraint::ENTITY_A() : SketchPlugin_Constraint::ENTITY_B();
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
        anAttributeToBeModified = anAttrA->id();
        if (!isToFeature) {
          aFeature = anAttrB->isObject() ? ModelAPI_Feature::feature(anAttrB->object())
                                         : FeaturePtr();
          isToFeature = aFeature.get() && aFeature == aBaseFeature;
          anAttributeToBeModified = anAttrB->id();
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
              anAttribute = anAttrB->attr();
              anAttributeToBeModified = anAttrA->id();
            }
          }
        }
        if (!anAttribute.get() && !anAttrB->isObject()) {
          AttributePtr aCurAttribute = anAttrB->attr();
          if (aCurAttribute.get()) {
            FeaturePtr aCurFeature = ModelAPI_Feature::feature(aCurAttribute->owner());
            if (aCurFeature.get() && aCurFeature == aBaseFeature) {
              anAttribute = anAttrA->attr();
              anAttributeToBeModified = anAttrB->id();
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
        //else
          //theCoincidenceToPoint[aRefFeature] = std::make_pair(anAttributeToBeModified,
          //                                                    aCoincidentPoint);
      }
      else
        theFeaturesToDelete.insert(aRefFeature); /// this case should not happen
    }
  }
}

void SketchPlugin_ConstraintSplit::getRefAttributes(const FeaturePtr& theFeature,
                                                    std::map<AttributePtr, std::list<AttributePtr> >& theRefs)
{
  theRefs.clear();

  std::list<AttributePtr> aPointAttributes = theFeature->data()->attributes(GeomDataAPI_Point2D::typeId());
  std::set<AttributePtr> aPointAttributesSet;

  std::list<AttributePtr>::const_iterator aPIt = aPointAttributes.begin(), aPLast = aPointAttributes.end();
  for (; aPIt != aPLast; aPIt++)
    aPointAttributesSet.insert(*aPIt);

  const std::set<AttributePtr>& aRefsAttributes = theFeature->data()->refsToMe();
  std::set<AttributePtr>::const_iterator aIt;
  for (aIt = aRefsAttributes.cbegin(); aIt != aRefsAttributes.cend(); ++aIt) {
    AttributePtr anAttr = (*aIt);
    FeaturePtr anAttrFeature = ModelAPI_Feature::feature(anAttr->owner());
    if (anAttrFeature.get() != this &&
        anAttr.get() && anAttr->attributeType() == ModelAPI_AttributeRefAttr::typeId()) {
      AttributeRefAttrPtr aRefAttr = std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(anAttr);
      if (!aRefAttr->isObject()) {
        AttributePtr anAttrInRef = aRefAttr->attr();
        if (anAttrInRef.get() && aPointAttributesSet.find(anAttrInRef) != aPointAttributesSet.end()) {
          if (theRefs.find(anAttrInRef) != theRefs.end())
            theRefs[anAttrInRef].push_back(aRefAttr);
          else {
            std::list<AttributePtr> anAttrList;
            anAttrList.push_back(aRefAttr);
            theRefs[anAttrInRef] = anAttrList;
          }
        }
      }
    }
  }
}

void SketchPlugin_ConstraintSplit::updateCoincidenceConstraintsToFeature(
      const std::map<std::shared_ptr<ModelAPI_Feature>, IdToPointPair>& theCoincidenceToFeature,
      const std::set<std::shared_ptr<GeomDataAPI_Point2D> >& theFurtherCoincidences,
      const std::set<ResultPtr>& theFeatureResults)
{
  if (theCoincidenceToFeature.empty())
    return;

  std::map<FeaturePtr, IdToPointPair>::const_iterator aCIt = theCoincidenceToFeature.begin(),
                                                            aCLast = theCoincidenceToFeature.end();
#ifdef DEBUG_SPLIT
  std::cout << std::endl;
  std::cout << "Coincidences to feature(modified):"<< std::endl;
#endif
  for (; aCIt != aCLast; aCIt++) {
    FeaturePtr aCoincFeature = aCIt->first;
    std::string anAttributeId = aCIt->second.first;
    AttributePoint2DPtr aCoincPoint = aCIt->second.second;
    std::set<AttributePoint2DPtr>::const_iterator aFCIt = theFurtherCoincidences.begin(),
                                                  aFCLast = theFurtherCoincidences.end();
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
      ResultPtr aResultForCoincidence = *(theFeatureResults.begin());

      if (theFeatureResults.size() > 1) { // try to find point on additional feature
        ResultPtr anAddtionalResult = *(theFeatureResults.begin()++);
        GeomShapePtr aShape = anAddtionalResult->shape();

        std::shared_ptr<GeomAPI_Pnt2d> aPnt2d = aCoincPoint->pnt();
        std::shared_ptr<GeomAPI_Pnt> aPoint = sketch()->to3D(aPnt2d->x(), aPnt2d->y());

        std::shared_ptr<GeomAPI_Pnt> aProjectedPoint;
        if (ModelGeomAlgo_Point2D::isPointOnEdge(aShape, aPoint, aProjectedPoint))
          aResultForCoincidence = anAddtionalResult;
      }
      aCoincFeature->refattr(anAttributeId)->setObject(aResultForCoincidence);
    }
#ifdef DEBUG_SPLIT
  std::cout << " -" << getFeatureInfo(aCoincFeature) << std::endl;
#endif
  }
}

void SketchPlugin_ConstraintSplit::updateTangentConstraintsToFeature(
      const std::map<std::shared_ptr<ModelAPI_Feature>, IdToPointPair>& theTangentFeatures,
      const std::set<std::shared_ptr<GeomDataAPI_Point2D> >& theFurtherCoincidences)
{
  if (theTangentFeatures.empty())
    return;

  std::map<FeaturePtr, IdToPointPair>::const_iterator aTIt = theTangentFeatures.begin(),
                                                      aTLast = theTangentFeatures.end();
#ifdef DEBUG_SPLIT
  std::cout << std::endl;
  std::cout << "Tangencies to feature(modified):"<< std::endl;
#endif
  for (; aTIt != aTLast; aTIt++) {
    FeaturePtr aTangentFeature = aTIt->first;
    std::string anAttributeId = aTIt->second.first;
    AttributePoint2DPtr aTangentPoint = aTIt->second.second;
    std::set<AttributePoint2DPtr>::const_iterator aFCIt = theFurtherCoincidences.begin(),
                                                  aFCLast = theFurtherCoincidences.end();
    std::shared_ptr<GeomAPI_Pnt2d> aCoincPnt = aTangentPoint->pnt();
    AttributePoint2DPtr aFeaturePointAttribute;
    /// here we rely on created coincidence between further coincidence point and tangent result
    for (; aFCIt != aFCLast && !aFeaturePointAttribute.get(); aFCIt++) {
      AttributePoint2DPtr aFCAttribute = *aFCIt;
      if (aCoincPnt->isEqual(aFCAttribute->pnt()))
        aFeaturePointAttribute = aFCAttribute;
    }
    if (aFeaturePointAttribute.get()) {
      FeaturePtr aFeature = std::dynamic_pointer_cast<ModelAPI_Feature>(aFeaturePointAttribute->owner());
      aTangentFeature->refattr(anAttributeId)->setObject(getFeatureResult(aFeature));
    }
#ifdef DEBUG_SPLIT
  std::cout << " -" << getFeatureInfo(aTangentFeature) << std::endl;
#endif
  }
}

void SketchPlugin_ConstraintSplit::updateRefAttConstraints(
                      const std::map<AttributePtr, std::list<AttributePtr> >& theBaseRefAttributes,
                      const std::set<std::pair<AttributePtr, AttributePtr> >& theModifiedAttributes)
{
#ifdef DEBUG_SPLIT
  std::cout << "SketchPlugin_ConstraintSplit::updateRefAttConstraints" << std::endl;
#endif

  std::set<std::pair<AttributePtr, AttributePtr> >::const_iterator anIt = theModifiedAttributes.begin(),
                                                                   aLast = theModifiedAttributes.end();
  for (; anIt != aLast; anIt++) {
    AttributePtr anAttribute = anIt->first;

    if (theBaseRefAttributes.find(anAttribute) == theBaseRefAttributes.end()) /// not found in references
      continue;
    std::list<AttributePtr> aRefAttributes = theBaseRefAttributes.at(anAttribute);
    std::list<AttributePtr>::const_iterator aRefIt = aRefAttributes.begin(),
                                            aRLast = aRefAttributes.end();

    AttributePtr aNewAttribute = anIt->second;
    for (; aRefIt != aRLast; aRefIt++) {
      AttributeRefAttrPtr aRefAttr = std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(*aRefIt);
      if (aRefAttr.get()) {
        aRefAttr->setAttr(aNewAttribute);
#ifdef DEBUG_SPLIT
        FeaturePtr aFeature = ModelAPI_Feature::feature(aRefAttr->owner());
        std::cout << " -" << getFeatureInfo(aFeature) << std::endl;
#endif
      }
    }
  }
}

void SketchPlugin_ConstraintSplit::splitLine(FeaturePtr& theSplitFeature,
                                             FeaturePtr& theBaseFeatureModified,
                                             FeaturePtr& theAfterFeature,
                                             std::set<AttributePoint2DPtr>& thePoints,
                                             std::set<FeaturePtr>& theCreatedFeatures,
                 std::set<std::pair<AttributePtr, AttributePtr>>& theModifiedAttributes)
{
  std::set<FeaturePtr> aCreatedFeatures;
  FeaturePtr aConstraintFeature;
  theBaseFeatureModified = FeaturePtr(); // it will contain modified base feature

  SketchPlugin_Sketch* aSketch = sketch();
  if (!aSketch)
    return;

  AttributeReferencePtr aBaseObjectAttr = std::dynamic_pointer_cast<ModelAPI_AttributeReference>(
                                           data()->attribute(SketchPlugin_Constraint::VALUE()));
  FeaturePtr aBaseFeature = ModelAPI_Feature::feature(aBaseObjectAttr->value());
  std::string aFeatureKind = aBaseFeature->getKind();
  if (aFeatureKind != SketchPlugin_Line::ID())
    return;

  AttributePoint2DPtr aFirstPointAttrOfSplit = getPointOfRefAttr(data()->attribute(SketchPlugin_Constraint::ENTITY_A()));
  AttributePoint2DPtr aSecondPointAttrOfSplit = getPointOfRefAttr(data()->attribute(SketchPlugin_Constraint::ENTITY_B()));
  AttributePoint2DPtr aStartPointAttrOfBase, anEndPointAttrOfBase;
  getFeaturePoints(aStartPointAttrOfBase, anEndPointAttrOfBase);
  if (!aStartPointAttrOfBase.get() && !anEndPointAttrOfBase.get()) {
    setError("Error: Feature has no start and end points.");
    return;
  }

  arrangePointsOnLine(aStartPointAttrOfBase, anEndPointAttrOfBase, aFirstPointAttrOfSplit, aSecondPointAttrOfSplit);

#ifdef DEBUG_SPLIT
  std::cout << "Arranged points (to build split between 1st and 2nd points:" << std::endl;
  std::cout << "Start point: " << ModelGeomAlgo_Point2D::getPointAttributeInfo(aStartPointAttrOfBase) << std::endl;
  std::cout << "1st point:   " << ModelGeomAlgo_Point2D::getPointAttributeInfo(aFirstPointAttrOfSplit) << std::endl;
  std::cout << "2nd point:   " << ModelGeomAlgo_Point2D::getPointAttributeInfo(aSecondPointAttrOfSplit) << std::endl;
  std::cout << "End point:   " << ModelGeomAlgo_Point2D::getPointAttributeInfo(anEndPointAttrOfBase) << std::endl;
#endif

  /// create a split feature
  theSplitFeature = createLineFeature(aBaseFeature, aFirstPointAttrOfSplit, aSecondPointAttrOfSplit);
  theCreatedFeatures.insert(theSplitFeature);

  // before split feature
  if (aStartPointAttrOfBase->pnt()->isEqual(aFirstPointAttrOfSplit->pnt())) {
    theModifiedAttributes.insert(std::make_pair(aStartPointAttrOfBase,
                                                theSplitFeature->attribute(SketchPlugin_Line::START_ID())));
  }
  else {
    theBaseFeatureModified = aBaseFeature; ///< use base feature to store all constraints here
    /// move end arc point to start of split
  }

  // after split feature
  if (!aSecondPointAttrOfSplit->pnt()->isEqual(anEndPointAttrOfBase->pnt())) {
    FeaturePtr aFeature;
    if (!theBaseFeatureModified.get()) {
      aFeature = aBaseFeature; ///< use base feature to store all constraints here
      fillAttribute(aFeature->attribute(SketchPlugin_Line::START_ID()), aSecondPointAttrOfSplit);
      aFeature->execute(); // to update result
    }
    else {
      aFeature = createLineFeature(aBaseFeature, aSecondPointAttrOfSplit, anEndPointAttrOfBase);
      theCreatedFeatures.insert(aFeature);
      theModifiedAttributes.insert(std::make_pair(anEndPointAttrOfBase,
                                                  aFeature->attribute(SketchPlugin_Line::END_ID())));
    }
    aConstraintFeature = createConstraint(SketchPlugin_ConstraintCoincidence::ID(),
                     theSplitFeature->attribute(SketchPlugin_Line::END_ID()),
                     aFeature->attribute(SketchPlugin_Line::START_ID()));
    theCreatedFeatures.insert(aConstraintFeature);

    thePoints.insert(std::dynamic_pointer_cast<GeomDataAPI_Point2D>
                                (aFeature->attribute(SketchPlugin_Line::START_ID())));
    thePoints.insert(std::dynamic_pointer_cast<GeomDataAPI_Point2D>
                                (aFeature->attribute(SketchPlugin_Line::END_ID())));

    if (!theBaseFeatureModified.get())
      theBaseFeatureModified = aFeature;
    else
      theAfterFeature = aFeature;
  }
  else {
    thePoints.insert(std::dynamic_pointer_cast<GeomDataAPI_Point2D>
                                  (theSplitFeature->attribute(SketchPlugin_Line::END_ID())));
    theModifiedAttributes.insert(std::make_pair(anEndPointAttrOfBase,
                                                theSplitFeature->attribute(SketchPlugin_Line::END_ID())));
  }
  // base split, that is defined before split feature should be changed at end
  // (after the after feature creation). Otherwise modified value will be used in after feature
  // before split feature
  if (!aStartPointAttrOfBase->pnt()->isEqual(aFirstPointAttrOfSplit->pnt())) {
    /// move end arc point to start of split
    fillAttribute(theBaseFeatureModified->attribute(SketchPlugin_Line::END_ID()), aFirstPointAttrOfSplit);
    theBaseFeatureModified->execute(); // to update result
    aConstraintFeature = createConstraint(SketchPlugin_ConstraintCoincidence::ID(),
                     theBaseFeatureModified->attribute(SketchPlugin_Line::END_ID()),
                     theSplitFeature->attribute(SketchPlugin_Line::START_ID()));
    theCreatedFeatures.insert(aConstraintFeature);

    thePoints.insert(std::dynamic_pointer_cast<GeomDataAPI_Point2D>
                             (theBaseFeatureModified->attribute(SketchPlugin_Line::START_ID())));
    thePoints.insert(std::dynamic_pointer_cast<GeomDataAPI_Point2D>
                               (theBaseFeatureModified->attribute(SketchPlugin_Line::END_ID())));
  }
  else
    thePoints.insert(std::dynamic_pointer_cast<GeomDataAPI_Point2D>
                                       (theSplitFeature->attribute(SketchPlugin_Line::START_ID())));

  // additional constraints between split and base features
  aConstraintFeature = createConstraintForObjects(SketchPlugin_ConstraintParallel::ID(),
                                                       getFeatureResult(aBaseFeature),
                                                       getFeatureResult(theSplitFeature));
  theCreatedFeatures.insert(aConstraintFeature);
  if (theAfterFeature.get()) {
    aConstraintFeature = createConstraintForObjects(SketchPlugin_ConstraintParallel::ID(),
                                                    getFeatureResult(aBaseFeature),
                                                    getFeatureResult(theAfterFeature));
    theCreatedFeatures.insert(aConstraintFeature);
  }
}

void SketchPlugin_ConstraintSplit::splitArc(FeaturePtr& theSplitFeature,
                                            FeaturePtr& theBaseFeatureModified,
                                            FeaturePtr& theAfterFeature,
                                            std::set<AttributePoint2DPtr>& thePoints,
                                            std::set<FeaturePtr>& theCreatedFeatures,
                 std::set<std::pair<AttributePtr, AttributePtr>>& theModifiedAttributes)
{
  std::set<FeaturePtr> aCreatedFeatures;
  FeaturePtr aConstraintFeature;
  theBaseFeatureModified = FeaturePtr(); // it will contain modified base feature

  SketchPlugin_Sketch* aSketch = sketch();
  if (!aSketch)
    return;

  AttributeReferencePtr aBaseObjectAttr = std::dynamic_pointer_cast<ModelAPI_AttributeReference>(
                                           data()->attribute(SketchPlugin_Constraint::VALUE()));
  FeaturePtr aBaseFeature = ModelAPI_Feature::feature(aBaseObjectAttr->value());
  std::string aFeatureKind = aBaseFeature->getKind();
  if (aFeatureKind != SketchPlugin_Arc::ID())
    return;

  AttributePoint2DPtr aFirstPointAttrOfSplit = getPointOfRefAttr(data()->attribute(SketchPlugin_Constraint::ENTITY_A()));
  AttributePoint2DPtr aSecondPointAttrOfSplit = getPointOfRefAttr(data()->attribute(SketchPlugin_Constraint::ENTITY_B()));
  AttributePoint2DPtr aStartPointAttrOfBase, anEndPointAttrOfBase;
  getFeaturePoints(aStartPointAttrOfBase, anEndPointAttrOfBase);
  if (!aStartPointAttrOfBase.get() && !anEndPointAttrOfBase.get()) {
    setError("Error: Feature has no start and end points.");
    return;
  }

  arrangePointsOnArc(aBaseFeature, aStartPointAttrOfBase, anEndPointAttrOfBase,
                     aFirstPointAttrOfSplit, aSecondPointAttrOfSplit);
#ifdef DEBUG_SPLIT
  std::cout << "Arranged points (to build split between 1st and 2nd points:" << std::endl;
  std::cout << "Start point: " << ModelGeomAlgo_Point2D::getPointAttributeInfo(aStartPointAttrOfBase) << std::endl;
  std::cout << "1st point:   " << ModelGeomAlgo_Point2D::getPointAttributeInfo(aFirstPointAttrOfSplit) << std::endl;
  std::cout << "2nd point:   " << ModelGeomAlgo_Point2D::getPointAttributeInfo(aSecondPointAttrOfSplit) << std::endl;
  std::cout << "End point:   " << ModelGeomAlgo_Point2D::getPointAttributeInfo(anEndPointAttrOfBase) << std::endl;
#endif

  /// split feature
  theSplitFeature = createArcFeature(aBaseFeature, aFirstPointAttrOfSplit, aSecondPointAttrOfSplit);
  theCreatedFeatures.insert(theSplitFeature);

  // before split feature
  if (aStartPointAttrOfBase->pnt()->isEqual(aFirstPointAttrOfSplit->pnt())) {
    theModifiedAttributes.insert(std::make_pair(aStartPointAttrOfBase,
                                                theSplitFeature->attribute(SketchPlugin_Arc::START_ID())));
  }
  else {
    theBaseFeatureModified = aBaseFeature; ///< use base feature to store all constraints here
    /// move end arc point to start of split
  }

  // after split feature
  if (!aSecondPointAttrOfSplit->pnt()->isEqual(anEndPointAttrOfBase->pnt())) {
    FeaturePtr aFeature;
    if (!theBaseFeatureModified.get()) {
      aFeature = aBaseFeature; ///< use base feature to store all constraints here
      fillAttribute(aFeature->attribute(SketchPlugin_Arc::START_ID()), aSecondPointAttrOfSplit);
      aFeature->execute(); // to update result
    }
    else {
      aFeature = createArcFeature(aBaseFeature, aSecondPointAttrOfSplit, anEndPointAttrOfBase);
      theCreatedFeatures.insert(aFeature);
      theModifiedAttributes.insert(std::make_pair(anEndPointAttrOfBase,
                                                  aFeature->attribute(SketchPlugin_Arc::END_ID())));
    }
    aConstraintFeature = createConstraint(SketchPlugin_ConstraintCoincidence::ID(),
                     theSplitFeature->attribute(SketchPlugin_Arc::END_ID()),
                     aFeature->attribute(SketchPlugin_Arc::START_ID()));
    theCreatedFeatures.insert(aConstraintFeature);

    thePoints.insert(std::dynamic_pointer_cast<GeomDataAPI_Point2D>
                                (aFeature->attribute(SketchPlugin_Arc::START_ID())));
    thePoints.insert(std::dynamic_pointer_cast<GeomDataAPI_Point2D>
                                (aFeature->attribute(SketchPlugin_Arc::END_ID())));

    if (!theBaseFeatureModified.get())
      theBaseFeatureModified = aFeature;
    else
      theAfterFeature = aFeature;
  }
  else {
    thePoints.insert(std::dynamic_pointer_cast<GeomDataAPI_Point2D>
                                  (theSplitFeature->attribute(SketchPlugin_Arc::END_ID())));
    theModifiedAttributes.insert(std::make_pair(anEndPointAttrOfBase,
                                   theSplitFeature->attribute(SketchPlugin_Arc::END_ID())));
  }
  // base split, that is defined before split feature should be changed at end
  // (after the after feature creation). Otherwise modified value will be used in after feature
  // before split feature
  if (!aStartPointAttrOfBase->pnt()->isEqual(aFirstPointAttrOfSplit->pnt())) {
    /// move end arc point to start of split
    fillAttribute(theBaseFeatureModified->attribute(SketchPlugin_Arc::END_ID()), aFirstPointAttrOfSplit);
    theBaseFeatureModified->execute(); // to update result
    aConstraintFeature = createConstraint(SketchPlugin_ConstraintCoincidence::ID(),
                     theBaseFeatureModified->attribute(SketchPlugin_Arc::END_ID()),
                     theSplitFeature->attribute(SketchPlugin_Arc::START_ID()));
    theCreatedFeatures.insert(aConstraintFeature);

    thePoints.insert(std::dynamic_pointer_cast<GeomDataAPI_Point2D>
                             (theBaseFeatureModified->attribute(SketchPlugin_Arc::START_ID())));
    thePoints.insert(std::dynamic_pointer_cast<GeomDataAPI_Point2D>
                               (theBaseFeatureModified->attribute(SketchPlugin_Arc::END_ID())));
  }
  else
    thePoints.insert(std::dynamic_pointer_cast<GeomDataAPI_Point2D>
                                       (theSplitFeature->attribute(SketchPlugin_Arc::START_ID())));

  // additional constraints between split and base features
  aConstraintFeature = createConstraintForObjects(SketchPlugin_ConstraintEqual::ID(),
                                                       getFeatureResult(aBaseFeature),
                                                       getFeatureResult(theSplitFeature));
  theCreatedFeatures.insert(aConstraintFeature);
  aConstraintFeature = createConstraintForObjects(SketchPlugin_ConstraintTangent::ID(),
                                                       getFeatureResult(theSplitFeature),
                                                       getFeatureResult(aBaseFeature));
  theCreatedFeatures.insert(aConstraintFeature);
  if (theAfterFeature.get()) {
    aConstraintFeature = createConstraintForObjects(SketchPlugin_ConstraintEqual::ID(),
                                                    getFeatureResult(aBaseFeature),
                                                    getFeatureResult(theAfterFeature));
    theCreatedFeatures.insert(aConstraintFeature);
    aConstraintFeature = createConstraintForObjects(SketchPlugin_ConstraintTangent::ID(),
                                                    getFeatureResult(theSplitFeature),
                                                    getFeatureResult(theAfterFeature));
    theCreatedFeatures.insert(aConstraintFeature);
  }
}

void SketchPlugin_ConstraintSplit::splitCircle(FeaturePtr& theSplitFeature,
                                               FeaturePtr& theBaseFeatureModified,
                                               FeaturePtr& theAfterFeature,
                                               std::set<AttributePoint2DPtr>& thePoints,
                                               std::set<FeaturePtr>& theCreatedFeatures,
                 std::set<std::pair<AttributePtr, AttributePtr>>& theModifiedAttributes)
{
  std::set<FeaturePtr> aCreatedFeatures;
  FeaturePtr aConstraintFeature;
  theBaseFeatureModified = FeaturePtr(); // it will contain modified base feature

  SketchPlugin_Sketch* aSketch = sketch();
  if (!aSketch)
    return;

  AttributeReferencePtr aBaseObjectAttr = std::dynamic_pointer_cast<ModelAPI_AttributeReference>(
                                           data()->attribute(SketchPlugin_Constraint::VALUE()));
  FeaturePtr aBaseFeature = ModelAPI_Feature::feature(aBaseObjectAttr->value());
  std::string aFeatureKind = aBaseFeature->getKind();
  if (aFeatureKind != SketchPlugin_Circle::ID())
    return;

  AttributePoint2DPtr aFirstPointAttrOfSplit = getPointOfRefAttr(data()->attribute(SketchPlugin_Constraint::ENTITY_A()));
  AttributePoint2DPtr aSecondPointAttrOfSplit = getPointOfRefAttr(data()->attribute(SketchPlugin_Constraint::ENTITY_B()));

  /// split feature
  theSplitFeature = createArcFeature(aBaseFeature, aFirstPointAttrOfSplit, aSecondPointAttrOfSplit);
  bool aSplitReversed = std::dynamic_pointer_cast<SketchPlugin_Arc>(theSplitFeature)->isReversed();
  theCreatedFeatures.insert(theSplitFeature);

  /// base feature is a left part of the circle
  theBaseFeatureModified = createArcFeature(aBaseFeature, aFirstPointAttrOfSplit, aSecondPointAttrOfSplit);
  std::dynamic_pointer_cast<SketchPlugin_Arc>(theBaseFeatureModified)->setReversed(!aSplitReversed);
  theBaseFeatureModified->execute();

  theModifiedAttributes.insert(std::make_pair(aBaseFeature->attribute(SketchPlugin_Circle::CENTER_ID()),
                                              theBaseFeatureModified->attribute(SketchPlugin_Arc::CENTER_ID())));

  theCreatedFeatures.insert(theBaseFeatureModified);

  thePoints.insert(std::dynamic_pointer_cast<GeomDataAPI_Point2D>
                             (theBaseFeatureModified->attribute(SketchPlugin_Arc::START_ID())));
  thePoints.insert(std::dynamic_pointer_cast<GeomDataAPI_Point2D>
                             (theBaseFeatureModified->attribute(SketchPlugin_Arc::END_ID())));

  // additional constraints between split and base features
  aConstraintFeature = createConstraint(SketchPlugin_ConstraintCoincidence::ID(),
                     theBaseFeatureModified->attribute(SketchPlugin_Arc::END_ID()),
                     theSplitFeature->attribute(SketchPlugin_Arc::END_ID()));
  theCreatedFeatures.insert(aConstraintFeature);
  aConstraintFeature = createConstraint(SketchPlugin_ConstraintCoincidence::ID(),
                     theBaseFeatureModified->attribute(SketchPlugin_Arc::START_ID()),
                     theSplitFeature->attribute(SketchPlugin_Arc::START_ID()));
  theCreatedFeatures.insert(aConstraintFeature);

  aConstraintFeature = createConstraintForObjects(SketchPlugin_ConstraintTangent::ID(),
                                                       getFeatureResult(theSplitFeature),
                                                       getFeatureResult(theBaseFeatureModified));
  theCreatedFeatures.insert(aConstraintFeature);
}

void SketchPlugin_ConstraintSplit::arrangePointsOnLine(
    const AttributePoint2DPtr& theStartPointAttr,
    const AttributePoint2DPtr& theEndPointAttr,
    AttributePoint2DPtr& theFirstPointAttr,
    AttributePoint2DPtr& theLastPointAttr) const
{
  // if first point is closer to last point, swap first and last values
  if (theStartPointAttr->pnt()->distance(theFirstPointAttr->pnt()) >
      theStartPointAttr->pnt()->distance(theLastPointAttr->pnt())) {
    AttributePoint2DPtr aTmpPoint = theFirstPointAttr;
    theFirstPointAttr = theLastPointAttr;
    theLastPointAttr = aTmpPoint;
  }
}

void SketchPlugin_ConstraintSplit::arrangePointsOnArc(
    const FeaturePtr& theArc,
    const std::shared_ptr<GeomDataAPI_Point2D>& theStartPointAttr,
    const std::shared_ptr<GeomDataAPI_Point2D>& theEndPointAttr,
    std::shared_ptr<GeomDataAPI_Point2D>& theFirstPointAttr,
    std::shared_ptr<GeomDataAPI_Point2D>& theSecondPointAttr) const
{
  std::shared_ptr<GeomAPI_Pnt2d> aCenter = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
      theArc->attribute(SketchPlugin_Arc::CENTER_ID()))->pnt();
  bool isReversed = theArc->boolean(SketchPlugin_Arc::INVERSED_ID())->value();

  // collect directions to each point
  std::shared_ptr<GeomAPI_Dir2d> aStartDir(
      new GeomAPI_Dir2d(theStartPointAttr->pnt()->xy()->decreased(aCenter->xy())));
  std::shared_ptr<GeomAPI_Dir2d> aFirstPtDir(
      new GeomAPI_Dir2d(theFirstPointAttr->pnt()->xy()->decreased(aCenter->xy())));
  std::shared_ptr<GeomAPI_Dir2d> aSecondPtDir(
      new GeomAPI_Dir2d(theSecondPointAttr->pnt()->xy()->decreased(aCenter->xy())));

  // sort points by their angular values
  double aFirstPtAngle = aStartDir->angle(aFirstPtDir);
  double aSecondPtAngle = aStartDir->angle(aSecondPtDir);
  double aPeriod = isReversed ? -2.0 * PI : 2.0 * PI;
  if (isReversed == (aFirstPtAngle > 0.))
    aFirstPtAngle += aPeriod;
  if (isReversed == (aSecondPtAngle > 0.))
    aSecondPtAngle += aPeriod;

  if (fabs(aFirstPtAngle) > fabs(aSecondPtAngle)) {
    AttributePoint2DPtr aTmpPoint = theFirstPointAttr;
    theFirstPointAttr = theSecondPointAttr;
    theSecondPointAttr = aTmpPoint;
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

FeaturePtr SketchPlugin_ConstraintSplit::createLineFeature(const FeaturePtr& theBaseFeature,
                                                           const AttributePtr& theFirstPointAttr,
                                                           const AttributePtr& theSecondPointAttr)
{
  FeaturePtr aFeature;
  SketchPlugin_Sketch* aSketch = sketch();
  if (!aSketch || !theBaseFeature.get())
    return aFeature;

  aFeature = aSketch->addFeature(SketchPlugin_Line::ID());

  fillAttribute(aFeature->attribute(SketchPlugin_Line::START_ID()), theFirstPointAttr);
  fillAttribute(aFeature->attribute(SketchPlugin_Line::END_ID()), theSecondPointAttr);
  aFeature->execute(); // to obtain result

  return aFeature;
}

FeaturePtr SketchPlugin_ConstraintSplit::createArcFeature(const FeaturePtr& theBaseFeature,
                                                          const AttributePtr& theFirstPointAttr,
                                                          const AttributePtr& theSecondPointAttr)
{
  FeaturePtr aFeature;
  SketchPlugin_Sketch* aSketch = sketch();
  if (!aSketch || !theBaseFeature.get())
    return aFeature;

  std::string aCenterAttributeId;
  if (theBaseFeature->getKind() == SketchPlugin_Arc::ID())
    aCenterAttributeId = SketchPlugin_Arc::CENTER_ID();
  else if (theBaseFeature->getKind() == SketchPlugin_Circle::ID())
    aCenterAttributeId = SketchPlugin_Circle::CENTER_ID();

  if (aCenterAttributeId.empty())
    return aFeature;

  aFeature = aSketch->addFeature(SketchPlugin_Arc::ID());
  // update fillet arc: make the arc correct for sure, so, it is not needed to process the "attribute updated"
  // by arc; moreover, it may cause cyclicity in hte mechanism of updater
  aFeature->data()->blockSendAttributeUpdated(true);

  aFeature->string(SketchPlugin_Arc::ARC_TYPE())->setValue(
                SketchPlugin_Arc::ARC_TYPE_CENTER_START_END());

  fillAttribute(aFeature->attribute(SketchPlugin_Arc::CENTER_ID()),
                theBaseFeature->attribute(aCenterAttributeId));
  fillAttribute(aFeature->attribute(SketchPlugin_Arc::START_ID()), theFirstPointAttr);
  fillAttribute(aFeature->attribute(SketchPlugin_Arc::END_ID()), theSecondPointAttr);

  /// fill referersed state of created arc as it is on the base arc
  if (theBaseFeature->getKind() == SketchPlugin_Arc::ID()) {
    bool aReversed = theBaseFeature->boolean(SketchPlugin_Arc::INVERSED_ID())->value();
    aFeature->boolean(SketchPlugin_Arc::INVERSED_ID())->setValue(aReversed);
  }
  aFeature->data()->blockSendAttributeUpdated(false);
  aFeature->execute(); // to obtain result

  return aFeature;
}

FeaturePtr SketchPlugin_ConstraintSplit::createConstraint(const std::string& theConstraintId,
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

  return aConstraint;
}

FeaturePtr SketchPlugin_ConstraintSplit::createConstraintForObjects(const std::string& theConstraintId,
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

  return aConstraint;
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

std::set<std::shared_ptr<ModelAPI_Attribute> > SketchPlugin_ConstraintSplit::getEdgeAttributes(
                                           const std::shared_ptr<ModelAPI_Feature>& theFeature)
{
  std::set<std::shared_ptr<ModelAPI_Attribute> > anAttributes;

  std::string aFeatureKind = theFeature->getKind();
  if (aFeatureKind == SketchPlugin_Line::ID()) {
    anAttributes.insert(theFeature->attribute(SketchPlugin_Line::START_ID()));
    anAttributes.insert(theFeature->attribute(SketchPlugin_Line::END_ID()));
  }
  else if (aFeatureKind == SketchPlugin_Arc::ID()) {
    anAttributes.insert(theFeature->attribute(SketchPlugin_Arc::START_ID()));
    anAttributes.insert(theFeature->attribute(SketchPlugin_Arc::END_ID()));
  }
  else if (aFeatureKind == SketchPlugin_Circle::ID()) {
  }

  return anAttributes;
}


std::string SketchPlugin_ConstraintSplit::getFeatureInfo(
                                               const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                               const bool isUseAttributesInfo)
{
  std::string anInfo;
  if (!theFeature.get()) {
    return "none";
  }

  if (theFeature->data()->isValid())
    anInfo.append(theFeature->data()->name().c_str());

  if (isUseAttributesInfo) {
    std::string aPointsInfo = ModelGeomAlgo_Point2D::getPontAttributesInfo(theFeature,
                                                             getEdgeAttributes(theFeature));
    if (!aPointsInfo.empty()) { /// processing of feature with point 2d attributes, like line, arc, circle
      anInfo += ": ";
      anInfo += "\n";
      anInfo += aPointsInfo;
    }
    else { /// process constraint coincidence, find points in ref attr attributes
      std::list<AttributePtr> anAttrs = theFeature->data()->attributes(
                                                                ModelAPI_AttributeRefAttr::typeId());
      std::list<AttributePtr>::const_iterator anIt = anAttrs.begin(), aLast = anAttrs.end();
      std::string anAttributesInfo;
      for(; anIt != aLast; anIt++) {
        if (!anAttributesInfo.empty()) {
          anAttributesInfo.append(", ");
          anAttributesInfo += "\n";
        }
        AttributePtr anAttr = *anIt;
        std::string aValue = "not defined";
        std::string aType = anAttr->attributeType();
        if (aType == ModelAPI_AttributeRefAttr::typeId()) {
          std::shared_ptr<ModelAPI_AttributeRefAttr> aRefAttr =
                                        std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(anAttr);
          if (aRefAttr.get()) {
            if (aRefAttr->isObject()) {
              FeaturePtr aFeature = ModelAPI_Feature::feature(aRefAttr->object());
              aValue = "<object:>" + getFeatureInfo(aFeature, false);
            }
            else {
              AttributePtr anAttribute = aRefAttr->attr();
              if (anAttribute.get()) {
                FeaturePtr aFeature = ModelAPI_Feature::feature(anAttribute->owner());
                aValue = "<attr:>" + ModelGeomAlgo_Point2D::getPointAttributeInfo(anAttribute) +
                         " [" + getFeatureInfo(aFeature, false) + "]";
              }
            }
          }
        }
        anAttributesInfo.append("    " + anAttr->id() + ": " + aValue);
      }
      if (!anAttributesInfo.empty())
        anInfo = anInfo + "\n" + anAttributesInfo;
    }
  }
  return anInfo;
}

