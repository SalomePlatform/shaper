// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:    SketchPlugin_Split.cpp
// Created: 04 May 2017
// Author:  Natalia ERMOLAEVA

#include "SketchPlugin_Split.h"

#include <Events_Message.h>

#include <GeomAPI_Dir2d.h>
#include <GeomAPI_Edge.h>
#include <GeomAPI_Pnt2d.h>
#include <GeomAPI_XY.h>
#include <GeomDataAPI_Point2D.h>
#include <GeomAlgoAPI_ShapeTools.h>
#include <GeomAlgoAPI_CompoundBuilder.h>

#include <ModelAPI_AttributeBoolean.h>
#include <ModelAPI_AttributeDouble.h>
#include <ModelAPI_AttributeRefAttr.h>
#include <ModelAPI_AttributeReference.h>
#include <ModelAPI_AttributeString.h>
#include <ModelAPI_Events.h>
#include <ModelAPI_Validator.h>
#include <ModelAPI_Session.h>
#include <ModelAPI_Tools.h>

#include <ModelGeomAlgo_Shape.h>

#include <SketchPlugin_Arc.h>
#include <SketchPlugin_Circle.h>
#include <SketchPlugin_ConstraintCoincidence.h>
#include <SketchPlugin_ConstraintEqual.h>
#include <SketchPlugin_ConstraintLength.h>
#include <SketchPlugin_ConstraintMiddle.h>
#include <SketchPlugin_ConstraintMirror.h>
#include <SketchPlugin_ConstraintParallel.h>
#include <SketchPlugin_ConstraintTangent.h>
#include <SketchPlugin_Line.h>
#include <SketchPlugin_MultiRotation.h>
#include <SketchPlugin_MultiTranslation.h>
#include <SketchPlugin_Point.h>

#include <ModelGeomAlgo_Point2D.h>
#include <ModelAPI_EventReentrantMessage.h>
#include <Events_Loop.h>

#include <cmath>

//#define CREATE_CONSTRAINTS

//#define DEBUG_SPLIT
#ifdef DEBUG_SPLIT
#include <iostream>
#endif

static const double PI = 3.141592653589793238463;

SketchPlugin_Split::SketchPlugin_Split()
{
}

void SketchPlugin_Split::initAttributes()
{
  data()->addAttribute(SELECTED_OBJECT(), ModelAPI_AttributeReference::typeId());
  data()->addAttribute(SELECTED_POINT(), GeomDataAPI_Point2D::typeId());

  data()->addAttribute(PREVIEW_POINT(), GeomDataAPI_Point2D::typeId());
  data()->addAttribute(PREVIEW_OBJECT(), ModelAPI_AttributeReference::typeId());

  data()->attribute(PREVIEW_POINT())->setIsArgument(false);
  data()->attribute(SELECTED_POINT())->setIsArgument(false);
  data()->attribute(PREVIEW_OBJECT())->setIsArgument(false);

  ModelAPI_Session::get()->validators()->registerNotObligatory(getKind(), PREVIEW_POINT());
  ModelAPI_Session::get()->validators()->registerNotObligatory(getKind(), PREVIEW_OBJECT());

  // TODO: remove
  //data()->addAttribute(SketchPlugin_Constraint::VALUE(), ModelAPI_AttributeReference::typeId());
  //data()->addAttribute(SketchPlugin_Constraint::ENTITY_A(), ModelAPI_AttributeRefAttr::typeId());
  //data()->addAttribute(SketchPlugin_Constraint::ENTITY_B(), ModelAPI_AttributeRefAttr::typeId());
}

void SketchPlugin_Split::execute()
{
  std::shared_ptr<ModelAPI_Data> aData = data();

  // Check the base objects are initialized.
  AttributeReferencePtr aBaseObjectAttr = std::dynamic_pointer_cast<ModelAPI_AttributeReference>(
                                                           data()->attribute(SELECTED_OBJECT()));
  //ObjectPtr aBaseObject = anObjectAttr->value();
  //AttributeReferencePtr aBaseObjectAttr = std::dynamic_pointer_cast<ModelAPI_AttributeReference>(
  //                                          aData->attribute(SketchPlugin_Constraint::VALUE()));
  if(!aBaseObjectAttr->isInitialized()) {
    setError("Error: Base object is not initialized.");
    return;
  }
  ObjectPtr aBaseObject = aBaseObjectAttr->value();
  AttributePoint2DPtr aFirstPointAttrOfSplit = getPointAttribute(true);
  //  getPointOfRefAttr(aData->attribute(SketchPlugin_Constraint::ENTITY_A()));
  AttributePoint2DPtr aSecondPointAttrOfSplit = getPointAttribute(false);
  //  getPointOfRefAttr(aData->attribute(SketchPlugin_Constraint::ENTITY_B()));
  if (!aFirstPointAttrOfSplit.get() || !aFirstPointAttrOfSplit->isInitialized() ||
      !aSecondPointAttrOfSplit.get() || !aSecondPointAttrOfSplit->isInitialized()) {
    setError("Error: Sub-shape is not initialized.");
    return;
  }

  /// Remove reference of this feature to feature used in preview, it is not necessary anymore
  /// as trim will be removed after execute
  AttributeReferencePtr aPreviewObjectAttr =
                     std::dynamic_pointer_cast<ModelAPI_AttributeReference>(
                     data()->attribute(PREVIEW_OBJECT()));

  ObjectPtr aPreviewObject = aPreviewObjectAttr->value();
  AttributePoint2DPtr aPoint = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
                                           data()->attribute(PREVIEW_POINT()));
  std::shared_ptr<GeomAPI_Pnt2d> aPreviewPnt2d = aPoint->pnt();
  // nullify pointer of preview attribute
  aPreviewObjectAttr->setValue(ResultPtr());
  bool anIsEqualPreviewAndSelected = aPreviewObject == aBaseObject;

  // Wait all constraints being created, then send update events
  static Events_ID anUpdateEvent = Events_Loop::eventByName(EVENT_OBJECT_UPDATED);
  bool isUpdateFlushed = Events_Loop::loop()->isFlushed(anUpdateEvent);
  if (isUpdateFlushed)
    Events_Loop::loop()->setFlushed(anUpdateEvent, false);

  // Find feature constraints
  FeaturePtr aBaseFeature = ModelAPI_Feature::feature(aBaseObject);
  ResultPtr aBaseFeatureResult = getFeatureResult(aBaseFeature);
  std::set<FeaturePtr> aFeaturesToDelete, aFeaturesToUpdate;

  //std::map<FeaturePtr, IdToPointPair> aTangentFeatures;
  std::map<FeaturePtr, IdToPointPair> aCoincidenceToFeature;
  getConstraints(aFeaturesToDelete, aFeaturesToUpdate, /*aTangentFeatures, */
                 aCoincidenceToFeature);

  std::map<AttributePtr, std::list<AttributePtr> > aBaseRefAttributes;
  std::list<AttributePtr> aRefsToFeature;
  getRefAttributes(aBaseFeature, aBaseRefAttributes, aRefsToFeature);

  std::map<AttributePtr, AttributePtr> aBasePointModifiedAttributes;

#ifdef DEBUG_SPLIT
  std::cout << std::endl;
  std::cout << "SketchPlugin_Split::execute()" << std::endl;
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
    std::cout << "Coincidences to base feature[" <<
      aCoincidenceToFeature.size() << "]: " << std::endl;
    std::map<FeaturePtr, IdToPointPair>::const_iterator anIt = aCoincidenceToFeature.begin(),
                                                        aLast = aCoincidenceToFeature.end();
    for (int i = 1; anIt != aLast; anIt++, i++) {
      FeaturePtr aFeature = (*anIt).first;
      std::string anAttributeId = (*anIt).second.first;
      std::shared_ptr<GeomDataAPI_Point2D> aPointAttr = (*anIt).second.second;

      std::cout << i << "-" << getFeatureInfo(aFeature) << std::endl;
      std::cout <<     " -Attribute to correct:" << anAttributeId << std::endl;
      std::cout <<     " -Point attribute:" <<
        ModelGeomAlgo_Point2D::getPointAttributeInfo(aPointAttr) << std::endl;
    }
  }

  std::map<AttributePtr, std::list<AttributePtr> >::const_iterator
    aRefIt = aBaseRefAttributes.begin(), aRefLast = aBaseRefAttributes.end();
  std::cout << std::endl << "References to attributes of base feature [" <<
    aBaseRefAttributes.size() << "]" << std::endl;
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
    std::shared_ptr<GeomDataAPI_Point2D> aPointAttr =
      std::dynamic_pointer_cast<GeomDataAPI_Point2D>(aBaseAttr);
    std::cout << aPointAttr->id().c_str() <<
      ": " << "[" << aRefAttributes.size() << "] " << aRefsInfo << std::endl;
  }
  std::cout << std::endl;
  std::cout << std::endl << "References to base feature [" <<
    aRefsToFeature.size() << "]" << std::endl;
  std::list<AttributePtr>::const_iterator aRefAttrIt = aRefsToFeature.begin(),
                                          aRefAttrLast = aRefsToFeature.end();
  std::string aRefsInfo;
  for (; aRefAttrIt != aRefAttrLast; aRefAttrIt++) {
    if (!aRefsInfo.empty())
      aRefsInfo.append(",");
    AttributePtr aRAttr = *aRefAttrIt;
    aRefsInfo.append(aRAttr->id());
    FeaturePtr aRFeature = ModelAPI_Feature::feature(aRAttr->owner());
    aRefsInfo.append("(" + aRFeature->name() + ") ");
  }
  std::cout << "[" << aRefsToFeature.size() << "] " << aRefsInfo << std::endl;


  std::cout << std::endl;
  std::cout << "---- SPLIT ----" << std::endl;
  std::cout << std::endl;
#endif

  std::string aFeatureKind = aBaseFeature->getKind();
  FeaturePtr aSplitFeature, anAfterFeature;
  std::set<AttributePoint2DPtr> aFurtherCoincidences;
  std::set<FeaturePtr> aCreatedFeatures;
  std::set<std::pair<AttributePtr, AttributePtr>> aModifiedAttributes;
  FeaturePtr aReplacingFeature, aNewFeature;
  if (aFeatureKind == SketchPlugin_Line::ID())
    aNewFeature = splitLine(aSplitFeature, aBaseFeature, anAfterFeature,
                            aFurtherCoincidences, aCreatedFeatures, aModifiedAttributes);
  else if (aFeatureKind == SketchPlugin_Arc::ID())
    aNewFeature = splitArc(aSplitFeature, aBaseFeature, anAfterFeature, aFurtherCoincidences,
                           aCreatedFeatures, aModifiedAttributes);
  if (aFeatureKind == SketchPlugin_Circle::ID()) {
    FeaturePtr aCircleFeature = aBaseFeature;
    aReplacingFeature = splitCircle(aSplitFeature, aBaseFeature, anAfterFeature,
                                    aFurtherCoincidences, aCreatedFeatures, aModifiedAttributes);

    updateRefFeatureConstraints(getFeatureResult(aBaseFeature), aRefsToFeature);

    AttributePtr aCenterAttr = aCircleFeature->attribute(SketchPlugin_Circle::CENTER_ID());
    aFeaturesToDelete.insert(aCircleFeature);
    // as circle is removed, temporary fill this attribute*/
    aBaseObjectAttr->setObject(ResultPtr());
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
  std::set<std::pair<AttributePtr, AttributePtr> >::const_iterator
    aPIt = aModifiedAttributes.begin(), aPLast = aModifiedAttributes.end();
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
                                        aFeatureResults, aSplitFeature);

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
  Events_Loop::loop()->flush(Events_Loop::eventByName(EVENT_OBJECT_DELETED));

#ifdef DEBUG_SPLIT
  std::cout << "update features after split:" << std::endl;
  std::set<FeaturePtr>::const_iterator anUIt = aFeaturesToUpdate.begin(),
                                       anULast = aFeaturesToUpdate.end();
  for (; anUIt != anULast; anUIt++) {
    std::cout << getFeatureInfo(*anUIt, false) << std::endl;
    std::cout << std::endl;
  }
#endif
  updateFeaturesAfterSplit(aFeaturesToUpdate);

  // Send events to update the sub-features by the solver.
  if(isUpdateFlushed) {
    Events_Loop::loop()->setFlushed(anUpdateEvent, true);
  }

    if (anIsEqualPreviewAndSelected) {
    // equal preview and selected objects
    // nothing to do if the preview and selected objects are different
    ResultPtr aReplacingResult;
    if (aReplacingFeature.get()) {
      aReplacingFeature->execute(); // need it to obtain result
      aReplacingResult = getFeatureResult(aReplacingFeature);
    }
    if (aReplacingResult.get()) { // base object was removed
      aPreviewObject = aReplacingResult;
      //aMessage->setSelectedObject(aReplacingResult);

      //GeomShapePtr aSelectedShape = aReplacingResult->shape();
      //std::shared_ptr<GeomAPI_Pnt> aPreviewPnt = sketch()->to3D(aPreviewPnt2d->x(),
      //                                                          aPreviewPnt2d->y());
      //std::shared_ptr<GeomAPI_Pnt> aProjectedPoint;
      //if (ModelGeomAlgo_Point2D::isPointOnEdge(aSelectedShape, aPreviewPnt, aProjectedPoint)) {
        //bool aValue = true;
      //}
      //aBaseShape = aShape;

#ifdef DEBUG_TRIM_METHODS
      if (!aSelectedShape.get())
        std::cout << "Set empty selected object" << std::endl;
      else
        std::cout << "Set shape with ShapeType: " << aSelectedShape->shapeTypeStr() << std::endl;
#endif
      //bool aValue = true;
    }
    else {
      aPreviewObject = ObjectPtr();

      aBaseFeature->execute(); // should recompute shapes of result to do not check obsolete one
      aBaseObject = getFeatureResult(aBaseFeature);
      std::shared_ptr<GeomAPI_Pnt> aPreviewPnt = sketch()->to3D(aPreviewPnt2d->x(),
                                                                aPreviewPnt2d->y());
      ResultPtr aBaseResult = std::dynamic_pointer_cast<ModelAPI_Result>(aBaseObject);
      if (aBaseResult) {
        GeomShapePtr aShape = aBaseResult->shape();
        std::shared_ptr<GeomAPI_Pnt> aProjectedPoint;
        if (ModelGeomAlgo_Point2D::isPointOnEdge(aShape, aPreviewPnt, aProjectedPoint))
          aPreviewObject = aBaseResult;
      }
      if (!aPreviewObject.get() && aNewFeature.get()) {
        ResultPtr aNewFeatureResult = getFeatureResult(aNewFeature);
        if (aNewFeatureResult.get()) {
          GeomShapePtr aShape = aNewFeatureResult->shape();
          std::shared_ptr<GeomAPI_Pnt> aProjectedPoint;
          if (ModelGeomAlgo_Point2D::isPointOnEdge(aShape, aPreviewPnt, aProjectedPoint))
            aPreviewObject = aNewFeatureResult;
        }
      }
    }
  }
  if (aPreviewObject.get()) {
    std::shared_ptr<ModelAPI_EventReentrantMessage> aMessage = std::shared_ptr
      <ModelAPI_EventReentrantMessage>(new ModelAPI_EventReentrantMessage(
                                           ModelAPI_EventReentrantMessage::eventId(), this));
    aMessage->setSelectedObject(aPreviewObject);
    Events_Loop::loop()->send(aMessage);
  }


#ifdef DEBUG_SPLIT
  std::cout << "SKETCH FEATURES (after split) [" << aSketch->numberOfSubs() << "]:" << std::endl;
  for (int i = 0, aNbSubs = aSketch->numberOfSubs(); i < aNbSubs; i++) {
    std::cout << getFeatureInfo(aSketch->subFeature(i), false) << std::endl;
  }
#endif
}

std::string SketchPlugin_Split::processEvent(const std::shared_ptr<Events_Message>& theMessage)
{
#ifdef DEBUG_TRIM_METHODS
  std::cout << "SketchPlugin_Trim::processEvent:" << data()->name() << std::endl;
#endif
  std::string aFilledAttributeName;

  std::shared_ptr<ModelAPI_EventReentrantMessage> aMessage =
        std::dynamic_pointer_cast<ModelAPI_EventReentrantMessage>(theMessage);
  if (aMessage.get()) {
    ObjectPtr anObject = aMessage->selectedObject();
    std::shared_ptr<GeomAPI_Pnt2d> aPoint = aMessage->clickedPoint();

    if (anObject.get() && aPoint.get()) {
      //if (myCashedShapes.find(anObject) == myCashedShapes.end())
      //  fillObjectShapes(anObject, sketch()->data()->owner(), myCashedShapes, myObjectToPoints);
      if (myCashedShapes.find(anObject) == myCashedShapes.end())
        fillObjectShapes(anObject, sketch()->data()->owner());
      const std::set<GeomShapePtr>& aShapes = myCashedShapes[anObject];
      if (aShapes.size() > 1) {
        std::shared_ptr<ModelAPI_AttributeReference> aRefSelectedAttr =
                              std::dynamic_pointer_cast<ModelAPI_AttributeReference>(
                              data()->attribute(SELECTED_OBJECT()));
        std::shared_ptr<ModelAPI_AttributeReference> aRefPreviewAttr =
                              std::dynamic_pointer_cast<ModelAPI_AttributeReference>(
                              data()->attribute(PREVIEW_OBJECT()));
        aRefSelectedAttr->setValue(anObject);
        aRefPreviewAttr->setValue(anObject);

        std::shared_ptr<GeomDataAPI_Point2D> aPointSelectedAttr =
                              std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
                              data()->attribute(SELECTED_POINT()));
        std::shared_ptr<GeomDataAPI_Point2D> aPointPreviewAttr =
                              std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
                              data()->attribute(PREVIEW_POINT()));
        aPointSelectedAttr->setValue(aPoint);
        aPointPreviewAttr->setValue(aPoint);

        Events_Loop::loop()->flush(Events_Loop::eventByName(EVENT_OBJECT_UPDATED));

        GeomShapePtr aSelectedShape = getSubShape(SELECTED_OBJECT(), SELECTED_POINT());
  #ifdef DEBUG_TRIM_METHODS
        if (!aSelectedShape.get())
          std::cout << "Set empty selected object" << std::endl;
        else
          std::cout << "Set shape with ShapeType: " << aSelectedShape->shapeTypeStr() << std::endl;
  #endif
        aFilledAttributeName = SELECTED_OBJECT();
      }
    }
  }
  return aFilledAttributeName;
}

AISObjectPtr SketchPlugin_Split::getAISObject(AISObjectPtr thePrevious)
{
  /*AttributeReferencePtr aBaseObjectAttr = std::dynamic_pointer_cast<ModelAPI_AttributeReference>(
                                           data()->attribute(SketchPlugin_Constraint::VALUE()));
  FeaturePtr aBaseFeature = ModelAPI_Feature::feature(aBaseObjectAttr->value());

  AttributePoint2DPtr aFirstPointAttrOfSplit = getPointOfRefAttr(
                                        data()->attribute(SketchPlugin_Constraint::ENTITY_A()));
  AttributePoint2DPtr aSecondPointAttrOfSplit = getPointOfRefAttr(
                                        data()->attribute(SketchPlugin_Constraint::ENTITY_B()));

  if (aBaseObjectAttr->isInitialized() && aBaseFeature.get() &&
      aFirstPointAttrOfSplit->isInitialized() &&
      aSecondPointAttrOfSplit->isInitialized()) {

    ResultPtr aResult = getFeatureResult(aBaseFeature);
    GeomShapePtr aBaseShape = aResult->shape();
    std::list<std::shared_ptr<GeomAPI_Pnt> > aPoints;

    std::shared_ptr<GeomAPI_Pnt2d> aStartPnt2d = aFirstPointAttrOfSplit->pnt();
    std::shared_ptr<GeomAPI_Pnt> aStartPoint = sketch()->to3D(aStartPnt2d->x(), aStartPnt2d->y());
    aPoints.push_back(aStartPoint);

    std::shared_ptr<GeomAPI_Pnt2d> aSecondPnt2d = aSecondPointAttrOfSplit->pnt();
    std::shared_ptr<GeomAPI_Pnt> aSecondPoint =
      sketch()->to3D(aSecondPnt2d->x(), aSecondPnt2d->y());
    aPoints.push_back(aSecondPoint);

    std::set<std::shared_ptr<GeomAPI_Shape> > aSplitShapes;

    GeomAlgoAPI_ShapeTools::splitShape_p(aBaseShape, aPoints, aSplitShapes);
    std::shared_ptr<GeomAPI_Shape> aShape =
      GeomAlgoAPI_ShapeTools::findShape(aPoints, aSplitShapes);

    AISObjectPtr anAIS = thePrevious;
    if (aShape) {
      if (!anAIS)
        anAIS = AISObjectPtr(new GeomAPI_AISObject);
      anAIS->createShape(aShape);
      std::shared_ptr<ModelAPI_AttributeBoolean> anAuxiliaryAttr =
             aBaseFeature->data()->boolean(SketchPlugin_SketchEntity::AUXILIARY_ID());

      bool isConstruction = anAuxiliaryAttr.get() != NULL && anAuxiliaryAttr->value();

      std::vector<int> aColor;
      double aWidth = SketchPlugin_SketchEntity::SKETCH_LINE_WIDTH();
      int aLineStyle = SketchPlugin_SketchEntity::SKETCH_LINE_STYLE();
      if (isConstruction) {
        aColor = Config_PropManager::color("Visualization", "sketch_auxiliary_color");
        aWidth = SketchPlugin_SketchEntity::SKETCH_LINE_WIDTH_AUXILIARY();
        aLineStyle = SketchPlugin_SketchEntity::SKETCH_LINE_STYLE_AUXILIARY();
      }
      else {
        aColor = Config_PropManager::color("Visualization", "sketch_entity_color");
      }
      anAIS->setColor(aColor[0], aColor[1], aColor[2]);
      anAIS->setWidth(aWidth + 1);
      anAIS->setLineStyle(aLineStyle);
    }
    return anAIS;
  }
  return AISObjectPtr();*/
#ifdef DEBUG_TRIM_METHODS
  std::cout << "SketchPlugin_Trim::getAISObject: " << data()->name() << std::endl;
#endif

  AISObjectPtr anAIS = thePrevious;

  std::list<std::shared_ptr<GeomAPI_Shape> > aShapes;
  GeomShapePtr aPreviewShape = getSubShape(PREVIEW_OBJECT(), PREVIEW_POINT());
  if (aPreviewShape.get())
    aShapes.push_back(aPreviewShape);
  GeomShapePtr aSelectedShape = getSubShape(SELECTED_OBJECT(), SELECTED_POINT());
  if (aSelectedShape.get())
    aShapes.push_back(aSelectedShape);

  if (aShapes.empty())
    return AISObjectPtr();

  GeomShapePtr aBaseShape = GeomAlgoAPI_CompoundBuilder::compound(aShapes);
  if (!aBaseShape.get())
    return AISObjectPtr();

  if (aBaseShape.get()) {
    if (!anAIS)
      anAIS = AISObjectPtr(new GeomAPI_AISObject);
    anAIS->createShape(aBaseShape);

    std::vector<int> aColor;
    aColor = Config_PropManager::color("Visualization", "operation_remove_feature_color");
    double aWidth = SketchPlugin_SketchEntity::SKETCH_LINE_WIDTH();
    int aLineStyle = SketchPlugin_SketchEntity::SKETCH_LINE_STYLE();
    anAIS->setColor(aColor[0], aColor[1], aColor[2]);
    // width when there is not base object should be extened in several points
    // in order to see this preview over highlight
    anAIS->setWidth(aWidth+4);
    anAIS->setLineStyle(aLineStyle);
  }
  else
    anAIS = AISObjectPtr();
  return anAIS;
}

//********************************************************************
void SketchPlugin_Split::fillObjectShapes(const ObjectPtr& theObject,
                                          const ObjectPtr& theSketch)
{
  std::set<std::shared_ptr<GeomAPI_Shape> > aShapes;
  std::map<std::shared_ptr<GeomDataAPI_Point2D>, std::shared_ptr<GeomAPI_Pnt> > aPointToAttributes;
  std::set<std::shared_ptr<GeomDataAPI_Point2D> > aRefAttributes;
  // current feature
  FeaturePtr aFeature = ModelAPI_Feature::feature(theObject);
  // edges on feature
  std::set<ResultPtr> anEdgeResults;
  ModelGeomAlgo_Shape::shapesOfType(aFeature, GeomAPI_Shape::EDGE, anEdgeResults);
  if (!anEdgeResults.empty()) {
    GeomShapePtr aFeatureShape = (*anEdgeResults.begin())->shape();

    // coincidences to the feature
    ModelGeomAlgo_Point2D::getPointsOfReference(aFeature, SketchPlugin_ConstraintCoincidence::ID(),
                         aRefAttributes, SketchPlugin_Point::ID(), SketchPlugin_Point::COORD_ID());
    // layed on feature coincidences to divide it on several shapes
    std::shared_ptr<ModelAPI_Data> aData = theSketch->data();
    std::shared_ptr<GeomDataAPI_Point> aC = std::dynamic_pointer_cast<GeomDataAPI_Point>(
        aData->attribute(SketchPlugin_Sketch::ORIGIN_ID()));
    std::shared_ptr<GeomDataAPI_Dir> aX = std::dynamic_pointer_cast<GeomDataAPI_Dir>(
        aData->attribute(SketchPlugin_Sketch::DIRX_ID()));
    std::shared_ptr<GeomDataAPI_Dir> aNorm = std::dynamic_pointer_cast<GeomDataAPI_Dir>(
        aData->attribute(SketchPlugin_Sketch::NORM_ID()));
    std::shared_ptr<GeomAPI_Dir> aY(new GeomAPI_Dir(aNorm->dir()->cross(aX->dir())));
    std::list<std::shared_ptr<GeomAPI_Pnt> > aPoints;
    ModelGeomAlgo_Point2D::getPointsInsideShape_p(aFeatureShape, aRefAttributes, aC->pnt(),
                                                aX->dir(), aY, aPoints, aPointToAttributes);

    GeomAlgoAPI_ShapeTools::splitShape_p(aFeatureShape, aPoints, aShapes);
  }
  myCashedShapes[theObject] = aShapes;
  myCashedReferences[theObject] = aPointToAttributes;
}

GeomShapePtr SketchPlugin_Split::getSubShape(const std::string& theObjectAttributeId,
                                             const std::string& thePointAttributeId)
{
  GeomShapePtr aBaseShape;

  AttributeReferencePtr anObjectAttr = std::dynamic_pointer_cast<ModelAPI_AttributeReference>(
                                                       data()->attribute(theObjectAttributeId));
  ObjectPtr aBaseObject = anObjectAttr->value();
  if (!aBaseObject.get())
    return aBaseShape;

  // point on feature
  AttributePoint2DPtr aPointAttr = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
                                           data()->attribute(thePointAttributeId));
  std::shared_ptr<GeomAPI_Pnt2d> anAttributePnt2d = aPointAttr->pnt();
  std::shared_ptr<GeomAPI_Pnt> anAttributePnt = sketch()->to3D(anAttributePnt2d->x(),
                                                               anAttributePnt2d->y());

#ifdef TRIM_SHAPE
  if (myCashedShapes.find(aBaseObject) == myCashedShapes.end())
    fillObjectShapes(aBaseObject, sketch()->data()->owner(), myCashedShapes, myObjectToPoints);

  const std::set<GeomShapePtr>& aShapes = myCashedShapes[aBaseObject];
  if (!aShapes.empty()) {
    std::set<GeomShapePtr>::const_iterator anIt = aShapes.begin(), aLast = aShapes.end();
    for (; anIt != aLast; anIt++) {
      GeomShapePtr aShape = *anIt;
      std::shared_ptr<GeomAPI_Pnt> aProjectedPoint;
      if (ModelGeomAlgo_Point2D::isPointOnEdge(aShape, anAttributePnt, aProjectedPoint))
        aBaseShape = aShape;
    }
  }
#else
  if (myCashedShapes.find(aBaseObject) == myCashedShapes.end())
    fillObjectShapes(aBaseObject, sketch()->data()->owner());

  std::shared_ptr<GeomAPI_Pnt> aStartPoint;
  std::shared_ptr<GeomAPI_Pnt> aSecondPoint;
  const std::set<GeomShapePtr>& aShapes = myCashedShapes[aBaseObject];
  std::set<GeomShapePtr>::const_iterator anIt = aShapes.begin(), aLast = aShapes.end();
  for (; anIt != aLast; anIt++) {
    GeomShapePtr aCurrentShape = *anIt;
    std::shared_ptr<GeomAPI_Pnt> aProjectedPoint;
    if (ModelGeomAlgo_Point2D::isPointOnEdge(aCurrentShape, anAttributePnt, aProjectedPoint)) {
      if (aCurrentShape->shapeType() == GeomAPI_Shape::EDGE) {
        std::shared_ptr<GeomAPI_Edge> anEdge(new GeomAPI_Edge(aCurrentShape));
        aStartPoint = anEdge->firstPoint();
        aSecondPoint = anEdge->lastPoint();
      }
      break;
    }
  }

  if (!aStartPoint.get() || !aSecondPoint.get())
    return aBaseShape;

  //AttributeReferencePtr aBaseObjectAttr = std::dynamic_pointer_cast<ModelAPI_AttributeReference>(
  //                                         data()->attribute(SketchPlugin_Constraint::VALUE()));
  FeaturePtr aBaseFeature = ModelAPI_Feature::feature(aBaseObject/*aBaseObjectAttr->value()*/);

  //AttributePoint2DPtr aFirstPointAttrOfSplit = getPointOfRefAttr(
  //                                      data()->attribute(SketchPlugin_Constraint::ENTITY_A()));
  //AttributePoint2DPtr aSecondPointAttrOfSplit = getPointOfRefAttr(
  //                                      data()->attribute(SketchPlugin_Constraint::ENTITY_B()));
  if (anObjectAttr->isInitialized() && aBaseFeature.get() && aPointAttr->isInitialized()) {
      //aFirstPointAttrOfSplit->isInitialized() &&
      //aSecondPointAttrOfSplit->isInitialized()) {
    ResultPtr aResult = getFeatureResult(aBaseFeature);
    GeomShapePtr aResultShape = aResult->shape();
    std::list<std::shared_ptr<GeomAPI_Pnt> > aPoints;

    //std::shared_ptr<GeomAPI_Pnt2d> aStartPnt2d = aFirstPointAttrOfSplit->pnt();
    //std::shared_ptr<GeomAPI_Pnt> aStartPoint = sketch()->to3D(aStartPnt2d->x(), aStartPnt2d->y());
    aPoints.push_back(aStartPoint);

    //std::shared_ptr<GeomAPI_Pnt2d> aSecondPnt2d = aSecondPointAttrOfSplit->pnt();
    //std::shared_ptr<GeomAPI_Pnt> aSecondPoint =
    //  sketch()->to3D(aSecondPnt2d->x(), aSecondPnt2d->y());
    aPoints.push_back(aSecondPoint);

    std::set<std::shared_ptr<GeomAPI_Shape> > aSplitShapes;
    GeomAlgoAPI_ShapeTools::splitShape_p(aResultShape, aPoints, aSplitShapes);
    aBaseShape = GeomAlgoAPI_ShapeTools::findShape(aPoints, aSplitShapes);
#endif
  }
  return aBaseShape;
}

std::shared_ptr<GeomDataAPI_Point2D> SketchPlugin_Split::getPointOfRefAttr(
                                                      const AttributePtr& theAttribute)
{
  AttributePoint2DPtr aPointAttribute;

  if (theAttribute->attributeType() == ModelAPI_AttributeRefAttr::typeId()) {
    AttributeRefAttrPtr aRefAttr =
      std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(theAttribute);
    if (aRefAttr.get() && aRefAttr->isInitialized()) {
      AttributePtr anAttribute = aRefAttr->attr();
      if (anAttribute.get() && anAttribute->attributeType() == GeomDataAPI_Point2D::typeId())
        aPointAttribute = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(anAttribute);
    }
  }
  return aPointAttribute;
}

void SketchPlugin_Split::getFeaturePoints(const FeaturePtr& theFeature,
                                                    AttributePoint2DPtr& theStartPointAttr,
                                                    AttributePoint2DPtr& theEndPointAttr)
{
  std::string aFeatureKind = theFeature->getKind();
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
                                         theFeature->attribute(aStartAttributeName));
    theEndPointAttr = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
                                         theFeature->attribute(anEndAttributeName));
  }
}

void SketchPlugin_Split::getConstraints(std::set<FeaturePtr>& theFeaturesToDelete,
                                        std::set<FeaturePtr>& theFeaturesToUpdate,
                                        std::map<FeaturePtr, IdToPointPair>& theCoincidenceToFeature)
{
  std::shared_ptr<ModelAPI_Data> aData = data();

  // Check the base objects are initialized.
  AttributeReferencePtr aBaseObjectAttr = std::dynamic_pointer_cast<ModelAPI_AttributeReference>(
                                                           data()->attribute(SELECTED_OBJECT()));
  //AttributeReferencePtr aBaseObjectAttr = std::dynamic_pointer_cast<ModelAPI_AttributeReference>(
  //                                          aData->attribute(SketchPlugin_Constraint::VALUE()));
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
        aRefFeatureKind == SketchPlugin_MultiTranslation::ID() ||
        aRefFeatureKind == SketchPlugin_ConstraintMiddle::ID())
      theFeaturesToDelete.insert(aRefFeature);
    else if (aRefFeatureKind == SketchPlugin_ConstraintLength::ID())
      theFeaturesToUpdate.insert(aRefFeature);
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
      if (aCoincidentPoint.get() && isToFeature)
        theCoincidenceToFeature[aRefFeature] = std::make_pair(anAttributeToBeModified,
                                                              aCoincidentPoint);
    }
  }
}

void SketchPlugin_Split::getRefAttributes(const FeaturePtr& theFeature,
                                    std::map<AttributePtr, std::list<AttributePtr> >& theRefs,
                                    std::list<AttributePtr>& theRefsToFeature)
{
  theRefs.clear();

  std::list<AttributePtr> aPointAttributes =
    theFeature->data()->attributes(GeomDataAPI_Point2D::typeId());
  std::set<AttributePtr> aPointAttributesSet;

  std::list<AttributePtr>::const_iterator aPIt =
    aPointAttributes.begin(), aPLast = aPointAttributes.end();
  for (; aPIt != aPLast; aPIt++)
    aPointAttributesSet.insert(*aPIt);

  std::set<AttributePtr> aRefsAttributes = getFeatureResult(theFeature)->data()->refsToMe();
  std::set<AttributePtr> aFRefsList = theFeature->data()->refsToMe();
  aRefsAttributes.insert(aFRefsList.begin(), aFRefsList.end());

  std::set<AttributePtr>::const_iterator aIt;
  for (aIt = aRefsAttributes.cbegin(); aIt != aRefsAttributes.cend(); ++aIt) {
    AttributePtr anAttr = (*aIt);
    FeaturePtr anAttrFeature = ModelAPI_Feature::feature(anAttr->owner());
    if (anAttrFeature.get() != this &&
        anAttr.get() && anAttr->attributeType() == ModelAPI_AttributeRefAttr::typeId()) {
      AttributeRefAttrPtr aRefAttr = std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(anAttr);
      if (!aRefAttr->isObject()) { /// find attributes referenced to feature point attributes
        AttributePtr anAttrInRef = aRefAttr->attr();
        if (anAttrInRef.get() &&
            aPointAttributesSet.find(anAttrInRef) != aPointAttributesSet.end()) {
          if (theRefs.find(anAttrInRef) != theRefs.end())
            theRefs[anAttrInRef].push_back(aRefAttr);
          else {
            std::list<AttributePtr> anAttrList;
            anAttrList.push_back(aRefAttr);
            theRefs[anAttrInRef] = anAttrList;
          }
        }
      }
      else { /// find attributes referenced to feature itself
        theRefsToFeature.push_back(anAttr);
      }
    }
  }
}

void SketchPlugin_Split::updateCoincidenceConstraintsToFeature(
      const std::map<std::shared_ptr<ModelAPI_Feature>, IdToPointPair>& theCoincidenceToFeature,
      const std::set<std::shared_ptr<GeomDataAPI_Point2D> >& theFurtherCoincidences,
      const std::set<ResultPtr>& theFeatureResults,
      const FeaturePtr& theSplitFeature)
{
  if (theCoincidenceToFeature.empty())
    return;

  // we should build coincidence constraints to end of the split feature
  std::set<std::shared_ptr<GeomDataAPI_Point2D> > aNewCoincidencesToSplitFeature;
  AttributePoint2DPtr aStartPointAttr, anEndPointAttr;
  getFeaturePoints(theSplitFeature, aStartPointAttr, anEndPointAttr);
  if (theFurtherCoincidences.find(aStartPointAttr) == theFurtherCoincidences.end())
    aNewCoincidencesToSplitFeature.insert(aStartPointAttr);
  if (theFurtherCoincidences.find(anEndPointAttr) == theFurtherCoincidences.end())
    aNewCoincidencesToSplitFeature.insert(anEndPointAttr);

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
      aCoincFeature->refattr(anAttributeId)->setObject(ResultPtr());
      aCoincFeature->refattr(anAttributeId)->setAttr(aFeaturePointAttribute);
      // create new coincidences to split feature points
      std::set<AttributePoint2DPtr>::const_iterator aSFIt = aNewCoincidencesToSplitFeature.begin(),
                                                    aSFLast = aNewCoincidencesToSplitFeature.end();
      for (; aSFIt != aSFLast; aSFIt++) {
        AttributePoint2DPtr aSFAttribute = *aSFIt;
        if (aCoincPnt->isEqual(aSFAttribute->pnt())) {
          std::string aSecondAttribute = SketchPlugin_Constraint::ENTITY_A();
          if (anAttributeId == SketchPlugin_Constraint::ENTITY_A())
            aSecondAttribute = SketchPlugin_Constraint::ENTITY_B();
          createConstraint(SketchPlugin_ConstraintCoincidence::ID(),
                           aSFAttribute, aCoincFeature->refattr(aSecondAttribute)->attr());
        }
      }
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

void SketchPlugin_Split::updateRefFeatureConstraints(
                                                  const ResultPtr& theFeatureBaseResult,
                                                  const std::list<AttributePtr>& theRefsToFeature)
{
  std::list<AttributePtr>::const_iterator anIt = theRefsToFeature.begin(),
                                          aLast = theRefsToFeature.end();
  for (; anIt != aLast; anIt++) {
    AttributeRefAttrPtr aRefAttr = std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(*anIt);
    if (aRefAttr.get())
      aRefAttr->setObject(theFeatureBaseResult);
  }
}

void SketchPlugin_Split::updateRefAttConstraints(
                    const std::map<AttributePtr, std::list<AttributePtr> >& theBaseRefAttributes,
                    const std::set<std::pair<AttributePtr, AttributePtr> >& theModifiedAttributes)
{
#ifdef DEBUG_SPLIT
  std::cout << "SketchPlugin_Split::updateRefAttConstraints" << std::endl;
#endif

  std::set<std::pair<AttributePtr, AttributePtr> >::const_iterator
    anIt = theModifiedAttributes.begin(),  aLast = theModifiedAttributes.end();
  for (; anIt != aLast; anIt++) {
    AttributePtr anAttribute = anIt->first;

    /// not found in references
    if (theBaseRefAttributes.find(anAttribute) == theBaseRefAttributes.end())
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

FeaturePtr SketchPlugin_Split::splitLine(FeaturePtr& theSplitFeature,
                                             FeaturePtr& theBaseFeatureModified,
                                             FeaturePtr& theAfterFeature,
                                             std::set<AttributePoint2DPtr>& thePoints,
                                             std::set<FeaturePtr>& theCreatedFeatures,
                 std::set<std::pair<AttributePtr, AttributePtr>>& theModifiedAttributes)
{
  FeaturePtr anNewFeature;

  std::set<FeaturePtr> aCreatedFeatures;
  FeaturePtr aConstraintFeature;
  theBaseFeatureModified = FeaturePtr(); // it will contain modified base feature

  SketchPlugin_Sketch* aSketch = sketch();
  if (!aSketch)
    return anNewFeature;

  AttributeReferencePtr aBaseObjectAttr = std::dynamic_pointer_cast<ModelAPI_AttributeReference>(
                                                           data()->attribute(SELECTED_OBJECT()));
  //AttributeReferencePtr aBaseObjectAttr = std::dynamic_pointer_cast<ModelAPI_AttributeReference>(
  //                                         data()->attribute(SketchPlugin_Constraint::VALUE()));
  FeaturePtr aBaseFeature = ModelAPI_Feature::feature(aBaseObjectAttr->value());
  std::string aFeatureKind = aBaseFeature->getKind();
  if (aFeatureKind != SketchPlugin_Line::ID())
    return anNewFeature;

  AttributePoint2DPtr aFirstPointAttrOfSplit = getPointAttribute(true);
    //getPointOfRefAttr(data()->attribute(SketchPlugin_Constraint::ENTITY_A()));
  AttributePoint2DPtr aSecondPointAttrOfSplit = getPointAttribute(false);
    //getPointOfRefAttr(data()->attribute(SketchPlugin_Constraint::ENTITY_B()));
  AttributePoint2DPtr aStartPointAttrOfBase, anEndPointAttrOfBase;

  getFeaturePoints(aBaseFeature, aStartPointAttrOfBase, anEndPointAttrOfBase);
  if (!aStartPointAttrOfBase.get() && !anEndPointAttrOfBase.get()) {
    setError("Error: Feature has no start and end points.");
    return anNewFeature;
  }

  arrangePointsOnLine(aStartPointAttrOfBase, anEndPointAttrOfBase,
                      aFirstPointAttrOfSplit, aSecondPointAttrOfSplit);

#ifdef DEBUG_SPLIT
  std::cout << "Arranged points (to build split between 1st and 2nd points:" << std::endl;
  std::cout << "Start point: " <<
    ModelGeomAlgo_Point2D::getPointAttributeInfo(aStartPointAttrOfBase) << std::endl;
  std::cout << "1st point:   " <<
    ModelGeomAlgo_Point2D::getPointAttributeInfo(aFirstPointAttrOfSplit) << std::endl;
  std::cout << "2nd point:   " <<
    ModelGeomAlgo_Point2D::getPointAttributeInfo(aSecondPointAttrOfSplit) << std::endl;
  std::cout << "End point:   " <<
    ModelGeomAlgo_Point2D::getPointAttributeInfo(anEndPointAttrOfBase) << std::endl;
#endif

  /// create a split feature
  theSplitFeature =
    createLineFeature(aBaseFeature, aFirstPointAttrOfSplit, aSecondPointAttrOfSplit);
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
      anNewFeature = aFeature;
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
    fillAttribute(theBaseFeatureModified->attribute(SketchPlugin_Line::END_ID()),
                                                    aFirstPointAttrOfSplit);
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

#ifdef CREATE_CONSTRAINTS
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
#endif
  return anNewFeature;
}

FeaturePtr SketchPlugin_Split::splitArc(FeaturePtr& theSplitFeature,
                                            FeaturePtr& theBaseFeatureModified,
                                            FeaturePtr& theAfterFeature,
                                            std::set<AttributePoint2DPtr>& thePoints,
                                            std::set<FeaturePtr>& theCreatedFeatures,
                 std::set<std::pair<AttributePtr, AttributePtr>>& theModifiedAttributes)
{
  FeaturePtr anNewFeature;

  std::set<FeaturePtr> aCreatedFeatures;
  FeaturePtr aConstraintFeature;
  theBaseFeatureModified = FeaturePtr(); // it will contain modified base feature

  SketchPlugin_Sketch* aSketch = sketch();
  if (!aSketch)
    return anNewFeature;

  AttributeReferencePtr aBaseObjectAttr = std::dynamic_pointer_cast<ModelAPI_AttributeReference>(
                                                           data()->attribute(SELECTED_OBJECT()));
  //AttributeReferencePtr aBaseObjectAttr = std::dynamic_pointer_cast<ModelAPI_AttributeReference>(
  //                                         data()->attribute(SketchPlugin_Constraint::VALUE()));
  FeaturePtr aBaseFeature = ModelAPI_Feature::feature(aBaseObjectAttr->value());
  std::string aFeatureKind = aBaseFeature->getKind();
  if (aFeatureKind != SketchPlugin_Arc::ID())
    return anNewFeature;

  AttributePoint2DPtr aFirstPointAttrOfSplit = getPointAttribute(true);
    //getPointOfRefAttr(data()->attribute(SketchPlugin_Constraint::ENTITY_A()));
  AttributePoint2DPtr aSecondPointAttrOfSplit = getPointAttribute(false);
    //getPointOfRefAttr(data()->attribute(SketchPlugin_Constraint::ENTITY_B()));
  AttributePoint2DPtr aStartPointAttrOfBase, anEndPointAttrOfBase;
  getFeaturePoints(aBaseFeature, aStartPointAttrOfBase, anEndPointAttrOfBase);
  if (!aStartPointAttrOfBase.get() && !anEndPointAttrOfBase.get()) {
    setError("Error: Feature has no start and end points.");
    return anNewFeature;
  }

  arrangePointsOnArc(aBaseFeature, aStartPointAttrOfBase, anEndPointAttrOfBase,
                     aFirstPointAttrOfSplit, aSecondPointAttrOfSplit);
#ifdef DEBUG_SPLIT
  std::cout << "Arranged points (to build split between 1st and 2nd points:" << std::endl;
  std::cout << "Start point: " <<
    ModelGeomAlgo_Point2D::getPointAttributeInfo(aStartPointAttrOfBase) << std::endl;
  std::cout << "1st point:   " <<
    ModelGeomAlgo_Point2D::getPointAttributeInfo(aFirstPointAttrOfSplit) << std::endl;
  std::cout << "2nd point:   " <<
    ModelGeomAlgo_Point2D::getPointAttributeInfo(aSecondPointAttrOfSplit) << std::endl;
  std::cout << "End point:   " <<
    ModelGeomAlgo_Point2D::getPointAttributeInfo(anEndPointAttrOfBase) << std::endl;
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
      anNewFeature = aFeature;
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
    fillAttribute(theBaseFeatureModified->attribute(SketchPlugin_Arc::END_ID()),
                                                    aFirstPointAttrOfSplit);
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
#ifdef CREATE_CONSTRAINTS
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
#endif
  return anNewFeature;
}

FeaturePtr SketchPlugin_Split::splitCircle(FeaturePtr& theSplitFeature,
                                               FeaturePtr& theBaseFeatureModified,
                                               FeaturePtr& theAfterFeature,
                                               std::set<AttributePoint2DPtr>& thePoints,
                                               std::set<FeaturePtr>& theCreatedFeatures,
                 std::set<std::pair<AttributePtr, AttributePtr>>& theModifiedAttributes)
{
  FeaturePtr anNewFeature;

  std::set<FeaturePtr> aCreatedFeatures;
  FeaturePtr aConstraintFeature;
  theBaseFeatureModified = FeaturePtr(); // it will contain modified base feature

  SketchPlugin_Sketch* aSketch = sketch();
  if (!aSketch)
    return anNewFeature;

  AttributeReferencePtr aBaseObjectAttr = std::dynamic_pointer_cast<ModelAPI_AttributeReference>(
                                                           data()->attribute(SELECTED_OBJECT()));
  //AttributeReferencePtr aBaseObjectAttr = std::dynamic_pointer_cast<ModelAPI_AttributeReference>(
  //                                         data()->attribute(SketchPlugin_Constraint::VALUE()));
  FeaturePtr aBaseFeature = ModelAPI_Feature::feature(aBaseObjectAttr->value());
  std::string aFeatureKind = aBaseFeature->getKind();
  if (aFeatureKind != SketchPlugin_Circle::ID())
    return anNewFeature;

  AttributePoint2DPtr aFirstPointAttrOfSplit = getPointAttribute(true);
    //getPointOfRefAttr(data()->attribute(SketchPlugin_Constraint::ENTITY_A()));
  AttributePoint2DPtr aSecondPointAttrOfSplit = getPointAttribute(false);
    //getPointOfRefAttr(data()->attribute(SketchPlugin_Constraint::ENTITY_B()));

  /// split feature
  theSplitFeature =
    createArcFeature(aBaseFeature, aFirstPointAttrOfSplit, aSecondPointAttrOfSplit);
  bool aSplitReversed = std::dynamic_pointer_cast<SketchPlugin_Arc>(theSplitFeature)->isReversed();
  theCreatedFeatures.insert(theSplitFeature);

  /// base feature is a left part of the circle
  theBaseFeatureModified = createArcFeature(aBaseFeature,
    aFirstPointAttrOfSplit, aSecondPointAttrOfSplit);
  anNewFeature = theBaseFeatureModified;
  std::dynamic_pointer_cast<SketchPlugin_Arc>(
    theBaseFeatureModified)->setReversed(!aSplitReversed);
  theBaseFeatureModified->execute();

  theModifiedAttributes.insert(
    std::make_pair(aBaseFeature->attribute(SketchPlugin_Circle::CENTER_ID()),
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

#ifdef CREATE_CONSTRAINTS
  aConstraintFeature = createConstraintForObjects(SketchPlugin_ConstraintTangent::ID(),
                                                       getFeatureResult(theSplitFeature),
                                                       getFeatureResult(theBaseFeatureModified));
  theCreatedFeatures.insert(aConstraintFeature);
#endif
  return anNewFeature;
}

void SketchPlugin_Split::arrangePointsOnLine(
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

void SketchPlugin_Split::arrangePointsOnArc(
    const FeaturePtr& theArc,
    const std::shared_ptr<GeomDataAPI_Point2D>& theStartPointAttr,
    const std::shared_ptr<GeomDataAPI_Point2D>& theEndPointAttr,
    std::shared_ptr<GeomDataAPI_Point2D>& theFirstPointAttr,
    std::shared_ptr<GeomDataAPI_Point2D>& theSecondPointAttr) const
{
  static const double anAngleTol = 1.e-12;

  std::shared_ptr<GeomAPI_Pnt2d> aCenter = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
      theArc->attribute(SketchPlugin_Arc::CENTER_ID()))->pnt();
  bool isReversed = theArc->boolean(SketchPlugin_Arc::REVERSED_ID())->value();

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
  if (fabs(aFirstPtAngle) > anAngleTol && isReversed == (aFirstPtAngle > 0.))
    aFirstPtAngle += aPeriod;
  if (fabs(aSecondPtAngle) > anAngleTol && isReversed == (aSecondPtAngle > 0.))
    aSecondPtAngle += aPeriod;

  if (fabs(aFirstPtAngle) > fabs(aSecondPtAngle)) {
    AttributePoint2DPtr aTmpPoint = theFirstPointAttr;
    theFirstPointAttr = theSecondPointAttr;
    theSecondPointAttr = aTmpPoint;
  }
}

void SketchPlugin_Split::fillAttribute(const AttributePtr& theModifiedAttribute,
                                                 const AttributePtr& theSourceAttribute)
{
  std::string anAttributeType = theModifiedAttribute->attributeType();
  if (anAttributeType == GeomDataAPI_Point2D::typeId()) {
    AttributePoint2DPtr aModifiedAttribute = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
                                              theModifiedAttribute);
    AttributePoint2DPtr aSourceAttribute = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
                                              theSourceAttribute);

    if (aModifiedAttribute.get() && aSourceAttribute.get())
      aModifiedAttribute->setValue(aSourceAttribute->pnt());
  }
  else if (anAttributeType == ModelAPI_AttributeBoolean::typeId()) {
    AttributeBooleanPtr aModifiedAttribute = std::dynamic_pointer_cast<ModelAPI_AttributeBoolean>(
                                              theModifiedAttribute);
    AttributeBooleanPtr aSourceAttribute = std::dynamic_pointer_cast<ModelAPI_AttributeBoolean>(
                                              theSourceAttribute);

    if (aModifiedAttribute.get() && aSourceAttribute.get())
      aModifiedAttribute->setValue(aSourceAttribute->value());
  }
}

FeaturePtr SketchPlugin_Split::createLineFeature(const FeaturePtr& theBaseFeature,
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

  fillAttribute(aFeature->attribute(SketchPlugin_SketchEntity::AUXILIARY_ID()),
                theBaseFeature->attribute(SketchPlugin_SketchEntity::AUXILIARY_ID()));

  aFeature->execute(); // to obtain result

  return aFeature;
}

FeaturePtr SketchPlugin_Split::createArcFeature(const FeaturePtr& theBaseFeature,
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
  // update fillet arc: make the arc correct for sure, so, it is not needed to process
  // the "attribute updated"
  // by arc; moreover, it may cause cyclicity in hte mechanism of updater
  bool aWasBlocked = aFeature->data()->blockSendAttributeUpdated(true);

  fillAttribute(aFeature->attribute(SketchPlugin_Arc::CENTER_ID()),
                theBaseFeature->attribute(aCenterAttributeId));
  fillAttribute(aFeature->attribute(SketchPlugin_Arc::START_ID()), theFirstPointAttr);
  fillAttribute(aFeature->attribute(SketchPlugin_Arc::END_ID()), theSecondPointAttr);

  fillAttribute(aFeature->attribute(SketchPlugin_SketchEntity::AUXILIARY_ID()),
                theBaseFeature->attribute(SketchPlugin_SketchEntity::AUXILIARY_ID()));

  /// fill referersed state of created arc as it is on the base arc
  if (theBaseFeature->getKind() == SketchPlugin_Arc::ID()) {
    bool aReversed = theBaseFeature->boolean(SketchPlugin_Arc::REVERSED_ID())->value();
    aFeature->boolean(SketchPlugin_Arc::REVERSED_ID())->setValue(aReversed);
  }
  aFeature->data()->blockSendAttributeUpdated(aWasBlocked);
  aFeature->execute(); // to obtain result

  return aFeature;
}

FeaturePtr SketchPlugin_Split::createConstraint(const std::string& theConstraintId,
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

FeaturePtr SketchPlugin_Split::createConstraintForObjects(
                                                    const std::string& theConstraintId,
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

void SketchPlugin_Split::updateFeaturesAfterSplit(
                                                   const std::set<FeaturePtr>& theFeaturesToUpdate)
{
  std::set<FeaturePtr>::const_iterator anIt = theFeaturesToUpdate.begin(),
                                       aLast = theFeaturesToUpdate.end();
  for (; anIt != aLast; anIt++) {
    FeaturePtr aRefFeature = std::dynamic_pointer_cast<ModelAPI_Feature>(*anIt);
    std::string aRefFeatureKind = aRefFeature->getKind();
    if (aRefFeatureKind == SketchPlugin_ConstraintLength::ID()) {
      std::shared_ptr<SketchPlugin_ConstraintLength> aLenghtFeature =
                              std::dynamic_pointer_cast<SketchPlugin_ConstraintLength>(*anIt);
      if (aLenghtFeature.get()) {
        std::shared_ptr<ModelAPI_AttributeDouble> aValueAttr = std::dynamic_pointer_cast<
            ModelAPI_AttributeDouble>(aLenghtFeature->attribute(SketchPlugin_Constraint::VALUE()));
        double aValue;
        if (aLenghtFeature->computeLenghtValue(aValue) && aValueAttr.get())
          aValueAttr->setValue(aValue);
      }
    }
  }
}

std::shared_ptr<ModelAPI_Result> SketchPlugin_Split::getFeatureResult(
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

std::set<std::shared_ptr<ModelAPI_Attribute> > SketchPlugin_Split::getEdgeAttributes(
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

std::shared_ptr<GeomDataAPI_Point2D> SketchPlugin_Split::getPointAttribute(const bool isFirstAttribute)
{
  std::shared_ptr<GeomDataAPI_Point2D> anAttribute;

  GeomShapePtr aSelectedShape = getSubShape(SELECTED_OBJECT(), SELECTED_POINT());
  if (!aSelectedShape.get())
    return anAttribute;

  if (aSelectedShape->shapeType() != GeomAPI_Shape::EDGE)
    return anAttribute;

  AttributeReferencePtr anObjectAttr = std::dynamic_pointer_cast<ModelAPI_AttributeReference>(
                                                       data()->attribute(SELECTED_OBJECT()));
  ObjectPtr aBaseObject = anObjectAttr->value();
  if (!aBaseObject.get())
    return anAttribute;

  std::shared_ptr<GeomAPI_Edge> anEdge(new GeomAPI_Edge(aSelectedShape));

  std::shared_ptr<GeomAPI_Pnt> aFirstPnt = anEdge->firstPoint();
  std::shared_ptr<GeomAPI_Pnt> aLastPnt = anEdge->lastPoint();

  std::shared_ptr<GeomDataAPI_Point2D> aFirstPointAttr, aLastPointAttr;
  /// find the points in feature attributes
  FeaturePtr aBaseFeature = ModelAPI_Feature::feature(aBaseObject);
  std::list<AttributePtr> a2DPointAttributes = aBaseFeature->data()->attributes(
                                                    GeomDataAPI_Point2D::typeId());
  std::list<AttributePtr>::const_iterator anIt = a2DPointAttributes.begin(),
                                          aLast = a2DPointAttributes.end();
  for (; anIt != aLast; anIt++) {
    std::shared_ptr<GeomDataAPI_Point2D> anAttributePoint =
                                  std::dynamic_pointer_cast<GeomDataAPI_Point2D>(*anIt);
    std::shared_ptr<GeomAPI_Pnt2d> aPoint2D = anAttributePoint->pnt();
    std::shared_ptr<GeomAPI_Pnt> aPoint3D = sketch()->to3D(aPoint2D->x(), aPoint2D->y());
    if (aFirstPnt->isEqual(aPoint3D))
      aFirstPointAttr = anAttributePoint;
    else if (aLastPnt->isEqual(aPoint3D))
      aLastPointAttr = anAttributePoint;
  }

  /// find the points in coincident features
  PntToAttributesMap aRefAttributes = myCashedReferences[aBaseObject];
  PntToAttributesMap::const_iterator
    aRIt = aRefAttributes.begin(), aRLast = aRefAttributes.end();
  for (; aRIt != aRLast; aRIt++) {
    std::shared_ptr<GeomDataAPI_Point2D> anAttribute = aRIt->first;
    std::shared_ptr<GeomAPI_Pnt> aPoint = aRIt->second;
    if (!aFirstPointAttr.get() && aFirstPnt->isEqual(aPoint))
      aFirstPointAttr = anAttribute;
    if (!aLastPointAttr.get() && aLastPnt->isEqual(aPoint))
      aLastPointAttr = anAttribute;
    if (aFirstPointAttr.get() && aLastPointAttr.get())
      break;
  }
  if (!aFirstPointAttr.get() || !aLastPointAttr)
    return anAttribute;

  return isFirstAttribute ? aFirstPointAttr : aLastPointAttr;
}

#ifdef _DEBUG
std::string SketchPlugin_Split::getFeatureInfo(const std::shared_ptr<ModelAPI_Feature>& theFeature,
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
    /// processing of feature with point 2d attributes, like line, arc, circle
    if (!aPointsInfo.empty()) {
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
#endif
