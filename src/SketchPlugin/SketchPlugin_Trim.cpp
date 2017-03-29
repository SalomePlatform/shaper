// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:    SketchPlugin_Trim.cpp
// Created: 22 Feb 2017
// Author:  Natalia ERMOLAEVA

#include "SketchPlugin_Trim.h"

#include <GeomAPI_Dir2d.h>
#include <GeomAPI_Edge.h>
#include <GeomAPI_Pnt2d.h>
#include <GeomAPI_XY.h>
#include <GeomDataAPI_Point2D.h>
#include <GeomAlgoAPI_ShapeTools.h>
#include <GeomAlgoAPI_CompoundBuilder.h>

#include <ModelAPI_AttributeReference.h>
#include <ModelAPI_AttributeString.h>
#include <ModelAPI_AttributeRefAttr.h>
#include <ModelAPI_Tools.h>
#include <ModelAPI_AttributeBoolean.h>

#include <ModelAPI_Validator.h>
#include <ModelAPI_Session.h>
#include <ModelAPI_AttributeDouble.h>

#include <ModelGeomAlgo_Shape.h>

#include <SketchPlugin_Arc.h>
#include <SketchPlugin_ConstraintMiddle.h>
#include <SketchPlugin_Circle.h>
#include <SketchPlugin_ConstraintCoincidence.h>
#include <SketchPlugin_ConstraintEqual.h>
//#include <SketchPlugin_ConstraintParallel.h>
#include <SketchPlugin_ConstraintTangent.h>
#include <SketchPlugin_ConstraintLength.h>
#include <SketchPlugin_ConstraintMirror.h>
#include <SketchPlugin_ConstraintCollinear.h>
#include <SketchPlugin_Line.h>
#include <SketchPlugin_MultiRotation.h>
#include <SketchPlugin_MultiTranslation.h>
#include <SketchPlugin_Point.h>

#include <ModelAPI_Events.h>
#include <SketchPlugin_Line.h>
#include <SketchPlugin_Arc.h>
#include <SketchPlugin_Circle.h>

#include <ModelGeomAlgo_Point2D.h>
#include <Events_Loop.h>

#include <cmath>

#define DEBUG_TRIM
#ifdef DEBUG_TRIM
#include <iostream>
#endif

static const double PI = 3.141592653589793238463;

static const std::string OPERATION_HIGHLIGHT_COLOR() { return "128, 0, 0"; }
static const std::string OPERATION_REMOVE_FEATURE_COLOR() { return "255, 174, 201"; }

SketchPlugin_Trim::SketchPlugin_Trim()
{
}

void SketchPlugin_Trim::initAttributes()
{
  data()->addAttribute(SketchPlugin_Trim::SELECTED_OBJECT(),
                       ModelAPI_AttributeReference::typeId());
  data()->addAttribute(SELECTED_POINT(), GeomDataAPI_Point2D::typeId());

  data()->addAttribute(PREVIEW_POINT(), GeomDataAPI_Point2D::typeId());
  data()->addAttribute(PREVIEW_OBJECT(), ModelAPI_AttributeReference::typeId());

  data()->attribute(PREVIEW_POINT())->setIsArgument(false);
  data()->attribute(SELECTED_POINT())->setIsArgument(false);
  data()->attribute(PREVIEW_OBJECT())->setIsArgument(false);

  ModelAPI_Session::get()->validators()->registerNotObligatory(getKind(), PREVIEW_POINT());
  ModelAPI_Session::get()->validators()->registerNotObligatory(getKind(), PREVIEW_OBJECT());
}

void SketchPlugin_Trim::findShapePoints(const std::string& theObjectAttributeId,
                                        const std::string& thePointAttributeId,
                                        std::shared_ptr<GeomAPI_Pnt>& aStartPoint,
                                        std::shared_ptr<GeomAPI_Pnt>& aLastPoint)
{
  AttributeReferencePtr aBaseObjectAttr = std::dynamic_pointer_cast<ModelAPI_AttributeReference>(
                                            data()->attribute(theObjectAttributeId));
  ObjectPtr aBaseObject = aBaseObjectAttr->value();

  AttributePoint2DPtr aPoint = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
                                              data()->attribute(thePointAttributeId));
  std::shared_ptr<GeomAPI_Pnt2d> anAttributePnt2d = aPoint->pnt();
  std::shared_ptr<GeomAPI_Pnt> anAttributePnt = sketch()->to3D(anAttributePnt2d->x(),
                                                               anAttributePnt2d->y());

  if (myCashedShapes.find(aBaseObject) == myCashedShapes.end())
    fillObjectShapes(aBaseObject, sketch()->data()->owner(), myCashedShapes, myObjectToPoints);

  const std::set<GeomShapePtr>& aShapes = myCashedShapes[aBaseObject];
  if (!aShapes.empty()) {
    std::set<GeomShapePtr>::const_iterator anIt = aShapes.begin(), aLast = aShapes.end();
    for (; anIt != aLast; anIt++) {
      GeomShapePtr aBaseShape = *anIt;
      std::shared_ptr<GeomAPI_Pnt> aProjectedPoint;
      if (ModelGeomAlgo_Point2D::isPointOnEdge(aBaseShape, anAttributePnt, aProjectedPoint)) {

        if (aBaseShape->shapeType() == GeomAPI_Shape::EDGE) {
          std::shared_ptr<GeomAPI_Edge> anEdge(new GeomAPI_Edge(aBaseShape));
          //GeomAPI_Shape::Orientation anOrientation = anEdge->orientation();
          //if (anOrientation == GeomAPI_Shape::REVERSED) {
            aStartPoint = anEdge->lastPoint();
            aLastPoint = anEdge->firstPoint();
          //}
          //else {
            //aStartPoint = anEdge->firstPoint();
            //aLastPoint = anEdge->lastPoint();
          //}
        }
      }
    }
  }
#ifdef DEBUG_TRIM
  std::cout << "<findShapePoints> => "
    << std::endl << "Attribute point: "
    << anAttributePnt->x() << ", " << anAttributePnt->y() << ", " << anAttributePnt->z() << "]"
    << std::endl << "Start Point: ["
    << aStartPoint->x() << ", " << aStartPoint->y() << ", " << aStartPoint->z() << "]"
    << std::endl << "Last Point: ["
    << aLastPoint->x() << ", " << aLastPoint->y() << ", " << aLastPoint->z() << "]"
    << std::endl;
#endif
}

std::shared_ptr<GeomAPI_Pnt2d> SketchPlugin_Trim::convertPoint(
                                                   const std::shared_ptr<GeomAPI_Pnt>& thePoint)
{
  std::shared_ptr<GeomAPI_Pnt2d> aPoint;

  AttributeReferencePtr aBaseObjectAttr = std::dynamic_pointer_cast<ModelAPI_AttributeReference>(
                                        data()->attribute(SketchPlugin_Trim::SELECTED_OBJECT()));
  ObjectPtr aBaseObject = aBaseObjectAttr->value();
  if (myObjectToPoints.find(aBaseObject) == myObjectToPoints.end())
    fillObjectShapes(aBaseObject, sketch()->data()->owner(), myCashedShapes, myObjectToPoints);

  bool aFound = false;
  const PointToRefsMap& aRefsMap = myObjectToPoints.at(aBaseObject);
  for (PointToRefsMap::const_iterator aPointIt = aRefsMap.begin();
       aPointIt != aRefsMap.end() && !aFound; aPointIt++) {
    if (aPointIt->first->isEqual(thePoint)) {
      const std::pair<std::list<AttributePoint2DPtr >,
               std::list<ObjectPtr > >& anInfo = aPointIt->second;
      const std::list<AttributePoint2DPtr >& anAttributes = anInfo.first;
      if (!anAttributes.empty()) {
        aPoint = anAttributes.front()->pnt();
        aFound = true;
      }
      else {
        aPoint = sketch()->to2D(thePoint);
        aFound = true;
      }
    }
  }
  if (!aFound) {
    // returns an end of the shape to define direction of split if feature's attribute
    // participates
    aPoint = sketch()->to2D(thePoint);
  }
  return aPoint;
}

void SketchPlugin_Trim::execute()
{
#ifdef DEBUG_TRIM
  std::cout << "SketchPlugin_Trim::execute" << std::endl;
#endif

  SketchPlugin_Sketch* aSketch = sketch();
  if (!aSketch)
    return;

  // Check the base objects are initialized.
  AttributeReferencePtr aBaseObjectAttr = std::dynamic_pointer_cast<ModelAPI_AttributeReference>(
                                        data()->attribute(SketchPlugin_Trim::SELECTED_OBJECT()));
  if(!aBaseObjectAttr->isInitialized()) {
    setError("Error: Base object is not initialized.");
    return;
  }
  ObjectPtr aBaseObject = aBaseObjectAttr->value();
  if (!aBaseObject.get())
    return;
  FeaturePtr aBaseFeature = ModelAPI_Feature::feature(aBaseObjectAttr->value());

  /// points of trim
  std::shared_ptr<GeomAPI_Pnt> aStartShapePoint, aLastShapePoint;
#ifdef DEBUG_TRIM
  std::cout << " Base Feature: " << aBaseFeature->data()->name() << std::endl;
#endif
  findShapePoints(SELECTED_OBJECT(), SELECTED_POINT(), aStartShapePoint, aLastShapePoint);

  std::shared_ptr<GeomAPI_Pnt2d> aStartShapePoint2d = convertPoint(aStartShapePoint);

  std::shared_ptr<GeomAPI_Pnt2d> aLastShapePoint2d = convertPoint(aLastShapePoint);

  std::set<FeaturePtr> aFeaturesToDelete;
  getConstraints(aFeaturesToDelete);

  std::map<AttributePtr, std::list<AttributePtr> > aBaseRefAttributes;
  std::list<AttributePtr> aRefsToFeature;
  getRefAttributes(aBaseFeature, aBaseRefAttributes, aRefsToFeature);
#ifdef DEBUG_TRIM
  std::cout << "---- getRefAttributes ----" << std::endl;
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
  std::cout << "---- getRefAttributes:end ----" << std::endl;
#endif
  // coincidence to result points
  // find coincidences to the base object, it should be used when attribute is found
  // in myObjectToPoints
  //std::map<AttributePtr, FeaturePtr> aCoincidencesToBaseFeature;
  //getCoincidencesToObject(aBaseObject, aCoincidencesToBaseFeature);

  std::set<AttributePoint2DPtr> aFurtherCoincidences;
  std::set<std::pair<AttributePtr, AttributePtr>> aModifiedAttributes;
  const std::string& aKind = aBaseFeature->getKind();
  FeaturePtr aReplacingFeature;
  if (aKind == SketchPlugin_Circle::ID()) {
    aReplacingFeature = trimCircle(aStartShapePoint2d, aLastShapePoint2d,
               aFurtherCoincidences, aModifiedAttributes);

    aFeaturesToDelete.insert(aBaseFeature);
    // as circle is removed, erase it from dependencies(arguments) of this feature
    // otherwise Trim feature will be removed with the circle before
    // this operation is finished
    aBaseObjectAttr->setObject(ResultPtr());

    AttributeReferencePtr aPreviewObjectAttr =
                     std::dynamic_pointer_cast<ModelAPI_AttributeReference>(
                     data()->attribute(SketchPlugin_Trim::PREVIEW_OBJECT()));
    aPreviewObjectAttr->setObject(ResultPtr());

  }
  else if (aKind == SketchPlugin_Line::ID()) {
    trimLine(aStartShapePoint2d, aLastShapePoint2d, aBaseRefAttributes,
             aFurtherCoincidences, aModifiedAttributes);
  }
  else if (aKind == SketchPlugin_Arc::ID()) {
    trimArc(aStartShapePoint2d, aLastShapePoint2d, aBaseRefAttributes,
            aFurtherCoincidences, aModifiedAttributes);
  }

  // constraints to end points of trim feature
  if (myObjectToPoints.find(aBaseObject) == myObjectToPoints.end())
    fillObjectShapes(aBaseObject, sketch()->data()->owner(), myCashedShapes, myObjectToPoints);

  // create coincidence to objects, intersected the base object
  const PointToRefsMap& aRefsMap = myObjectToPoints.at(aBaseObject);
  for (std::set<AttributePoint2DPtr>::const_iterator anIt = aFurtherCoincidences.begin(),
                                                     aLast = aFurtherCoincidences.end();
       anIt != aLast; anIt++) {
    AttributePoint2DPtr aPointAttribute = (*anIt);
    std::shared_ptr<GeomAPI_Pnt2d> aPoint2d = aPointAttribute->pnt();

#ifdef DEBUG_TRIM
    std::cout << "<compare Points> => " << std::endl
            << "aPoint2d: [" << aPoint2d->x() << ", " << aPoint2d->y() << "]" << std::endl;
    if (aStartShapePoint2d.get())
      std::cout << "Start Point: [" << aStartShapePoint2d->x() << ", " << aStartShapePoint2d->y()
                << "]" << std::endl;
    if (aLastShapePoint2d.get())
      std::cout << "Last Point: [" << aLastShapePoint2d->x() << ", " << aLastShapePoint2d->y()
                << "]" << std::endl;
#endif

    std::shared_ptr<GeomAPI_Pnt> aPoint;
    if (aStartShapePoint2d.get() && aPoint2d->isEqual(aStartShapePoint2d))
      aPoint = aStartShapePoint;
    else if (aLastShapePoint2d.get() && aPoint2d->isEqual(aLastShapePoint2d))
      aPoint = aLastShapePoint;

    if (!aPoint.get())
      continue;

    std::pair<std::list<AttributePoint2DPtr >, std::list<ObjectPtr > > anInfo;
    for (PointToRefsMap::const_iterator aRefIt = aRefsMap.begin(); aRefIt != aRefsMap.end();
         aRefIt++)
    {
      if (aRefIt->first->isEqual(aPoint)) {
        anInfo = aRefIt->second;
        break;
      }
    }
    const std::list<ObjectPtr>& anObjects = anInfo.second;
    for (std::list<ObjectPtr>::const_iterator anObjectIt = anObjects.begin();
      anObjectIt != anObjects.end(); anObjectIt++) {
      createConstraintToObject(SketchPlugin_ConstraintCoincidence::ID(), aPointAttribute,
                               *anObjectIt);
    }
  }

  // move constraints from base feature to replacing feature: ignore coincidences to feature
  // if attributes of coincidence participated in split
  ResultPtr aReplacingResult;
  if (aReplacingFeature.get()) {
    aReplacingFeature->execute(); // need it to obtain result
    aReplacingResult = getFeatureResult(aReplacingFeature);
  }
  for(std::list<AttributePtr>::const_iterator anIt = aRefsToFeature.begin(),
                                          aLast = aRefsToFeature.end();
      anIt != aLast; anIt++) {
    AttributePtr anAttribute = *anIt;

    //if (replaceCoincidenceAttribute(anAttribute, aModifiedAttributes))
    //  continue;

    if (setCoincidenceToAttribute(anAttribute, aFurtherCoincidences))
      continue;

    if (aReplacingResult.get()) {
      AttributeRefAttrPtr aRefAttr =
          std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(anAttribute);
      if (aRefAttr.get())
        aRefAttr->setObject(aReplacingResult);
      else {
        AttributeReferencePtr aReferenceAttr =
                             std::dynamic_pointer_cast<ModelAPI_AttributeReference>(anAttribute);
        if (aReferenceAttr.get())
          aReferenceAttr->setObject(aReplacingResult);
      }
    }
  }

  updateRefAttConstraints(aBaseRefAttributes, aModifiedAttributes, aFeaturesToDelete);

  // Wait all constraints being created, then send update events
  static Events_ID anUpdateEvent = Events_Loop::eventByName(EVENT_OBJECT_UPDATED);
  bool isUpdateFlushed = Events_Loop::loop()->isFlushed(anUpdateEvent);
  if (isUpdateFlushed)
    Events_Loop::loop()->setFlushed(anUpdateEvent, false);

  // delete constraints
  ModelAPI_Tools::removeFeaturesAndReferences(aFeaturesToDelete);
  Events_Loop::loop()->flush(Events_Loop::eventByName(EVENT_OBJECT_DELETED));

  // Send events to update the sub-features by the solver.
  if(isUpdateFlushed) {
    Events_Loop::loop()->setFlushed(anUpdateEvent, true);
  }

#ifdef DEBUG_TRIM
  std::cout << "SketchPlugin_Trim::done" << std::endl;
#endif
}

bool SketchPlugin_Trim::setCoincidenceToAttribute(const AttributePtr& theAttribute,
                                const std::set<AttributePoint2DPtr>& theFurtherCoincidences)
{
  FeaturePtr aFeature = ModelAPI_Feature::feature(theAttribute->owner());
  if (aFeature->getKind() != SketchPlugin_ConstraintCoincidence::ID())
    return false;

  AttributePoint2DPtr aRefPointAttr = SketchPlugin_ConstraintCoincidence::getPoint(aFeature);
  if (!aRefPointAttr.get())
    return false;
  std::shared_ptr<GeomAPI_Pnt2d> aRefPnt2d = aRefPointAttr->pnt();

  std::set<AttributePoint2DPtr>::const_iterator anIt = theFurtherCoincidences.begin(),
                                                aLast = theFurtherCoincidences.end();
  bool aFoundPoint = false;
  for (; anIt != aLast && !aFoundPoint; anIt++) {
    AttributePoint2DPtr aPointAttribute = (*anIt);
    std::shared_ptr<GeomAPI_Pnt2d> aPoint2d = aPointAttribute->pnt();
    if (aPoint2d->isEqual(aRefPnt2d)) {
      AttributeRefAttrPtr aRefAttr = std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(
                                                                           theAttribute);
      if (aRefAttr.get()) {
        aRefAttr->setAttr(aPointAttribute);
        aFoundPoint = true;
      }
    }
  }
  return aFoundPoint;
}

bool SketchPlugin_Trim::replaceCoincidenceAttribute(const AttributePtr& theCoincidenceAttribute,
                   const std::set<std::pair<AttributePtr, AttributePtr>>& theModifiedAttributes)
{
  FeaturePtr aCoincidenceFeature = ModelAPI_Feature::feature(theCoincidenceAttribute->owner());
  if (aCoincidenceFeature->getKind() != SketchPlugin_ConstraintCoincidence::ID())
    return false;

  AttributeRefAttrPtr aCAttrA = std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(
                         aCoincidenceFeature->attribute(SketchPlugin_Constraint::ENTITY_A()));
  AttributeRefAttrPtr aCAttrB = std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(
                         aCoincidenceFeature->attribute(SketchPlugin_Constraint::ENTITY_B()));
  AttributePtr aCAttrRefA = aCAttrA->attr();
  AttributePtr aCAttrRefB = aCAttrB->attr();

  bool isProcessed = false;
  for (std::set<std::pair<AttributePtr, AttributePtr>>::const_iterator
       anIt = theModifiedAttributes.begin(); anIt != theModifiedAttributes.end(); anIt++) {
    AttributePtr anAttributeBefore = anIt->first;
    if (anAttributeBefore == aCAttrRefA) {
      aCAttrA->setAttr(anIt->second);
      isProcessed = true;
    }
    if (anAttributeBefore == aCAttrRefB) {
      aCAttrB->setAttr(anIt->second);
      isProcessed = true;
    }
  }
  return isProcessed;
}

bool SketchPlugin_Trim::isMacro() const
{
  return true;
}

AISObjectPtr SketchPlugin_Trim::getAISObject(AISObjectPtr thePrevious)
{
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

GeomShapePtr SketchPlugin_Trim::getSubShape(const std::string& theObjectAttributeId,
                                            const std::string& thePointAttributeId)
{
  GeomShapePtr aBaseShape;

  AttributeReferencePtr anObjectAttr = std::dynamic_pointer_cast<ModelAPI_AttributeReference>(
                                                       data()->attribute(theObjectAttributeId));
  ObjectPtr aBaseObject = anObjectAttr->value();
  if (!aBaseObject.get())
    return aBaseShape;

  // point on feature
  AttributePoint2DPtr aPoint = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
                                           data()->attribute(thePointAttributeId));
  std::shared_ptr<GeomAPI_Pnt2d> anAttributePnt2d = aPoint->pnt();
  std::shared_ptr<GeomAPI_Pnt> anAttributePnt = sketch()->to3D(anAttributePnt2d->x(),
                                                               anAttributePnt2d->y());

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
  return aBaseShape;
}

void SketchPlugin_Trim::getFeaturePoints(const FeaturePtr& theFeature,
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

void SketchPlugin_Trim::getConstraints(std::set<FeaturePtr>& theFeaturesToDelete)
{
  std::shared_ptr<ModelAPI_Data> aData = data();

  // Check the base objects are initialized.
  AttributeReferencePtr aBaseObjectAttr = std::dynamic_pointer_cast<ModelAPI_AttributeReference>(
                                         aData->attribute(SketchPlugin_Trim::SELECTED_OBJECT()));
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
  }
}

void SketchPlugin_Trim::getRefAttributes(const FeaturePtr& theFeature,
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

/*void SketchPlugin_Trim::getCoincidencesToObject(const ObjectPtr& theObject,
                   std::map<AttributePtr, FeaturePtr>& theCoincidencesToBaseFeature)
{
  const std::set<AttributePtr>& aRefsList = theObject->data()->refsToMe();
  std::set<AttributePtr>::const_iterator aIt;
  for (aIt = aRefsList.cbegin(); aIt != aRefsList.cend(); ++aIt) {
    std::shared_ptr<ModelAPI_Attribute> aAttr = (*aIt);
    FeaturePtr aRefFeature = std::dynamic_pointer_cast<ModelAPI_Feature>(aAttr->owner());
    if (aRefFeature->getKind() != SketchPlugin_ConstraintCoincidence::ID())
      continue;
    AttributePtr anAttribute;
    AttributeRefAttrPtr aRefAttr = std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>
                                  (aRefFeature->attribute(SketchPlugin_Constraint::ENTITY_A()));
    if (aRefAttr->isObject() && aRefAttr->object() == theObject)
    {
      anAttribute = aRefFeature->attribute(SketchPlugin_Constraint::ENTITY_B());
    }
    else {
      AttributeRefAttrPtr aRefAttr = std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>
                                    (aRefFeature->attribute(SketchPlugin_Constraint::ENTITY_B()));
      if (aRefAttr->isObject() && aRefAttr->object() == theObject)
        anAttribute = aRefFeature->attribute(SketchPlugin_Constraint::ENTITY_A());
    }
    if (!anAttribute.get())
      continue;

    aRefAttr = std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(anAttribute);
    if (aRefAttr->isObject())
      continue; // one of attributes of coincidence contains link to an attribute

    anAttribute = aRefAttr->attr();
    if (anAttribute.get())
    {
      theCoincidencesToBaseFeature[anAttribute] = aRefFeature;
    }
  }
}*/

void SketchPlugin_Trim::updateRefAttConstraints(
                    const std::map<AttributePtr, std::list<AttributePtr> >& theBaseRefAttributes,
                    const std::set<std::pair<AttributePtr, AttributePtr> >& theModifiedAttributes,
                    std::set<FeaturePtr>& theFeaturesToDelete)
{
#ifdef DEBUG_TRIM
  std::cout << "SketchPlugin_Trim::updateRefAttConstraints" << std::endl;
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
    if (aNewAttribute.get()) {
      for (; aRefIt != aRLast; aRefIt++) {
        AttributeRefAttrPtr aRefAttr =
                        std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(*aRefIt);
        if (aRefAttr.get()) {
            aRefAttr->setAttr(aNewAttribute);
        }
      }
    }
  }
}

void SketchPlugin_Trim::removeReferencesToAttribute(const AttributePtr& theAttribute,
                  std::map<AttributePtr, std::list<AttributePtr> >& theBaseRefAttributes)
{
  /// not found in references
  if (theBaseRefAttributes.find(theAttribute) == theBaseRefAttributes.end())
    return;

  std::list<AttributePtr> aRefAttributes = theBaseRefAttributes.at(theAttribute);
  std::list<AttributePtr>::const_iterator aRefIt = aRefAttributes.begin(),
                                          aRLast = aRefAttributes.end();

  std::set<FeaturePtr> aFeaturesToDelete;
  for (; aRefIt != aRLast; aRefIt++) {
    AttributeRefAttrPtr aRefAttr = std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(*aRefIt);
    if (aRefAttr.get()) {
      aFeaturesToDelete.insert(ModelAPI_Feature::feature(aRefAttr->owner()));
    }
  }

  // delete constraints
  ModelAPI_Tools::removeFeaturesAndReferences(aFeaturesToDelete);
  Events_Loop::loop()->flush(Events_Loop::eventByName(EVENT_OBJECT_DELETED));
}

void SketchPlugin_Trim::trimLine(const std::shared_ptr<GeomAPI_Pnt2d>& theStartShapePoint,
                  const std::shared_ptr<GeomAPI_Pnt2d>& theLastShapePoint,
                  std::map<AttributePtr, std::list<AttributePtr> >& theBaseRefAttributes,
                  std::set<AttributePoint2DPtr>& thePoints,
                  std::set<std::pair<AttributePtr, AttributePtr>>& theModifiedAttributes)
{
  // Check the base objects are initialized.
  AttributeReferencePtr aBaseObjectAttr = std::dynamic_pointer_cast<ModelAPI_AttributeReference>(
                                        data()->attribute(SketchPlugin_Trim::SELECTED_OBJECT()));
  ObjectPtr aBaseObject = aBaseObjectAttr->value();
  FeaturePtr aBaseFeature = ModelAPI_Feature::feature(aBaseObjectAttr->value());

  /// points of trim
  AttributePoint2DPtr aStartPointAttrOfBase, anEndPointAttrOfBase;
  getFeaturePoints(aBaseFeature, aStartPointAttrOfBase, anEndPointAttrOfBase);

  std::shared_ptr<GeomAPI_Pnt2d> aStartFeaturePoint = aStartPointAttrOfBase->pnt();
  std::shared_ptr<GeomAPI_Pnt2d> aLastFeaturePoint = anEndPointAttrOfBase->pnt();

  std::shared_ptr<GeomAPI_Pnt2d> aStartShapePoint = theStartShapePoint;
  std::shared_ptr<GeomAPI_Pnt2d> aLastShapePoint = theLastShapePoint;
  arrangePointsOnLine(aStartPointAttrOfBase, anEndPointAttrOfBase,
                      aStartShapePoint, aLastShapePoint);
#ifdef DEBUG_TRIM
  std::cout << "Arranged points (to build split between 1st and 2nd points:" << std::endl;
  if (aStartShapePoint.get())
    std::cout << "Start point: [" << aStartShapePoint->x() << ", " <<
                                       aStartShapePoint->y() << "]" << std::endl;
  std::cout << "1st point:   [" << aStartFeaturePoint->x() << ", " <<
                                   aStartFeaturePoint->y() << "]" << std::endl;
  if (aLastShapePoint.get())
    std::cout << "2st point:   [" << aLastShapePoint->x() << ", " <<
                                     aLastShapePoint->y() << "]" << std::endl;
  std::cout << "End point:   [" << aLastFeaturePoint->x() << ", " <<
                                   aLastFeaturePoint->y() << "]" << std::endl;
#endif

  bool isStartPoint = !aStartShapePoint.get() || aStartFeaturePoint->isEqual(aStartShapePoint);
  bool isLastPoint = !aLastShapePoint.get() || aLastFeaturePoint->isEqual(aLastShapePoint);
  if (isStartPoint || isLastPoint) {
    // result is one line: changed existing line
    std::string aModifiedAttribute = isStartPoint ? SketchPlugin_Line::START_ID()
                                                  : SketchPlugin_Line::END_ID();
    std::shared_ptr<GeomAPI_Pnt2d> aPoint;
    if (aStartShapePoint.get() && aLastShapePoint.get())
      aPoint = isStartPoint ? aLastShapePoint : aStartShapePoint;
    else
      aPoint = aStartShapePoint.get() ? aStartShapePoint : aLastShapePoint;

    // it is important to delete references before the feature modification because
    // if deletion will be after the feature modification, solver returns the feature back
    removeReferencesToAttribute(aBaseFeature->attribute(aModifiedAttribute),
                                theBaseRefAttributes);

    fillPointAttribute(aBaseFeature->attribute(aModifiedAttribute), aPoint);
    //theModifiedAttributes.insert(
    //  std::make_pair(aBaseFeature->attribute(aModifiedAttribute), AttributePtr()));

    thePoints.insert(std::dynamic_pointer_cast<GeomDataAPI_Point2D>
                               (aBaseFeature->attribute(aModifiedAttribute)));
  }
  else {
    // result is two lines: start line point - start shape point,
    // last shape point - last line point
    // create second line
    FeaturePtr anNewFeature = createLineFeature(aBaseFeature, aLastShapePoint, aLastFeaturePoint);
    thePoints.insert(std::dynamic_pointer_cast<GeomDataAPI_Point2D>
                               (anNewFeature->attribute(SketchPlugin_Line::START_ID())));

    std::string aModifiedAttribute = SketchPlugin_Line::END_ID();
    theModifiedAttributes.insert(
      std::make_pair(aBaseFeature->attribute(aModifiedAttribute),
                                   anNewFeature->attribute(SketchPlugin_Line::END_ID())));

    // modify base arc
    fillPointAttribute(aBaseFeature->attribute(aModifiedAttribute), aStartShapePoint);

    thePoints.insert(std::dynamic_pointer_cast<GeomDataAPI_Point2D>
                               (aBaseFeature->attribute(aModifiedAttribute)));

    // Collinear constraint for lines
    createConstraintForObjects(SketchPlugin_ConstraintCollinear::ID(),
                               getFeatureResult(aBaseFeature),
                               getFeatureResult(anNewFeature));

  }
}

void SketchPlugin_Trim::trimArc(const std::shared_ptr<GeomAPI_Pnt2d>& theStartShapePoint,
                 const std::shared_ptr<GeomAPI_Pnt2d>& theLastShapePoint,
                 std::map<AttributePtr, std::list<AttributePtr> >& theBaseRefAttributes,
                 std::set<AttributePoint2DPtr>& thePoints,
                 std::set<std::pair<AttributePtr, AttributePtr>>& theModifiedAttributes)
{
  // Check the base objects are initialized.
  AttributeReferencePtr aBaseObjectAttr = std::dynamic_pointer_cast<ModelAPI_AttributeReference>(
                                        data()->attribute(SketchPlugin_Trim::SELECTED_OBJECT()));
  ObjectPtr aBaseObject = aBaseObjectAttr->value();
  FeaturePtr aBaseFeature = ModelAPI_Feature::feature(aBaseObjectAttr->value());

  /// points of trim
  AttributePoint2DPtr aStartPointAttrOfBase, anEndPointAttrOfBase;
  getFeaturePoints(aBaseFeature, aStartPointAttrOfBase, anEndPointAttrOfBase);

  std::shared_ptr<GeomAPI_Pnt2d> aStartArcPoint = aStartPointAttrOfBase->pnt();
  std::shared_ptr<GeomAPI_Pnt2d> aLastArcPoint = anEndPointAttrOfBase->pnt();

  std::shared_ptr<GeomAPI_Pnt2d> aStartShapePoint = theStartShapePoint;
  std::shared_ptr<GeomAPI_Pnt2d> aLastShapePoint = theLastShapePoint;
  arrangePointsOnArc(aBaseFeature, aStartPointAttrOfBase, anEndPointAttrOfBase,
                     aStartShapePoint, aLastShapePoint);
#ifdef DEBUG_TRIM
  std::cout << "Arranged points (to build split between 1st and 2nd points:" << std::endl;
  if (aStartShapePoint.get())
    std::cout << "Start shape point: [" << aStartShapePoint->x() << ", " <<
                                       aStartShapePoint->y() << "]" << std::endl;
  std::cout << "Start arc attribute point:   [" << aStartArcPoint->x() << ", " <<
                                   aStartArcPoint->y() << "]" << std::endl;
  if (aLastShapePoint.get())
    std::cout << "Last shape point:   [" << aLastShapePoint->x() << ", " <<
                                     aLastShapePoint->y() << "]" << std::endl;
  std::cout << "Last arc attribute point:   [" << aLastArcPoint->x() << ", " <<
                                   aLastArcPoint->y() << "]" << std::endl;
#endif

  bool isStartPoint = !aStartShapePoint.get() || aStartArcPoint->isEqual(aStartShapePoint);
  bool isLastPoint = !aLastShapePoint.get() || aLastArcPoint->isEqual(aLastShapePoint);
  if (isStartPoint || isLastPoint) {
    // result is one arc: changed existing arc
    std::string aModifiedAttribute = isStartPoint ? SketchPlugin_Arc::START_ID()
                                                  : SketchPlugin_Arc::END_ID();
    std::shared_ptr<GeomAPI_Pnt2d> aPoint;
    if (aStartShapePoint.get() && aLastShapePoint.get())
      aPoint = isStartPoint ? aLastShapePoint : aStartShapePoint;
    else
      aPoint = aStartShapePoint.get() ? aStartShapePoint : aLastShapePoint;

    removeReferencesToAttribute(aBaseFeature->attribute(aModifiedAttribute),
                                theBaseRefAttributes);

    fillPointAttribute(aBaseFeature->attribute(aModifiedAttribute), aPoint);

    thePoints.insert(std::dynamic_pointer_cast<GeomDataAPI_Point2D>
                               (aBaseFeature->attribute(aModifiedAttribute)));
  }
  else {
    // result is two arcs: start arc point - start shape point, last shape point - last arc point
    // create second arc
    FeaturePtr anArcFeature = createArcFeature(aBaseFeature, aLastShapePoint, aLastArcPoint);
    thePoints.insert(std::dynamic_pointer_cast<GeomDataAPI_Point2D>
                               (anArcFeature->attribute(SketchPlugin_Arc::START_ID())));

    std::string aModifiedAttribute = SketchPlugin_Arc::END_ID();
    theModifiedAttributes.insert(
      std::make_pair(aBaseFeature->attribute(aModifiedAttribute),
                                   anArcFeature->attribute(SketchPlugin_Arc::END_ID())));

    // modify base arc
    fillPointAttribute(aBaseFeature->attribute(aModifiedAttribute), aStartShapePoint);

    thePoints.insert(std::dynamic_pointer_cast<GeomDataAPI_Point2D>
                               (aBaseFeature->attribute(aModifiedAttribute)));

    // equal Radius constraint for arcs
    anArcFeature->execute(); // we need the created arc result to set equal constraint
    createConstraintForObjects(SketchPlugin_ConstraintEqual::ID(),
                               getFeatureResult(aBaseFeature),
                               getFeatureResult(anArcFeature));
    // coincident centers constraint
    createConstraint(SketchPlugin_ConstraintCoincidence::ID(),
                     aBaseFeature->attribute(SketchPlugin_Arc::CENTER_ID()),
                     anArcFeature->attribute(SketchPlugin_Arc::CENTER_ID()));

    std::cout << "Created arc on points:" << std::endl;
    std::cout << "Start shape point: [" << aStartShapePoint->x() << ", " <<
                                           aStartShapePoint->y() << "]" << std::endl;

  }
}

FeaturePtr SketchPlugin_Trim::trimCircle(const std::shared_ptr<GeomAPI_Pnt2d>& theStartShapePoint,
                                   const std::shared_ptr<GeomAPI_Pnt2d>& theLastShapePoint,
                                   std::set<AttributePoint2DPtr>& thePoints,
                 std::set<std::pair<AttributePtr, AttributePtr>>& theModifiedAttributes)
{
  // Check the base objects are initialized.
  AttributeReferencePtr aBaseObjectAttr = std::dynamic_pointer_cast<ModelAPI_AttributeReference>(
                                        data()->attribute(SketchPlugin_Trim::SELECTED_OBJECT()));
  ObjectPtr aBaseObject = aBaseObjectAttr->value();
  FeaturePtr aBaseFeature = ModelAPI_Feature::feature(aBaseObjectAttr->value());

  /// points of trim
  //AttributePoint2DPtr aStartPointAttrOfBase, anEndPointAttrOfBase;
  //getFeaturePoints(aBaseFeature, aStartPointAttrOfBase, anEndPointAttrOfBase);

  /// trim feature
  FeaturePtr anArcFeature = createArcFeature(aBaseFeature, theStartShapePoint, theLastShapePoint);
  // arc created by trim of circle is always correct, that means that it is not inversed
  anArcFeature->boolean(SketchPlugin_Arc::REVERSED_ID())->setValue(false);

  theModifiedAttributes.insert(
    std::make_pair(aBaseFeature->attribute(SketchPlugin_Circle::CENTER_ID()),
                   anArcFeature->attribute(SketchPlugin_Arc::CENTER_ID())));

  thePoints.insert(std::dynamic_pointer_cast<GeomDataAPI_Point2D>
                             (anArcFeature->attribute(SketchPlugin_Arc::START_ID())));
  thePoints.insert(std::dynamic_pointer_cast<GeomDataAPI_Point2D>
                             (anArcFeature->attribute(SketchPlugin_Arc::END_ID())));

  return anArcFeature;
}

void SketchPlugin_Trim::arrangePointsOnLine(const AttributePoint2DPtr& theStartPointAttr,
                                            const AttributePoint2DPtr& theEndPointAttr,
                                            std::shared_ptr<GeomAPI_Pnt2d>& theFirstPoint,
                                            std::shared_ptr<GeomAPI_Pnt2d>& theLastPoint) const
{
  if (!theFirstPoint.get() || !theLastPoint.get())
    return;

  // if first point is closer to last point, swap first and last values
  if (theStartPointAttr->pnt()->distance(theFirstPoint) >
      theStartPointAttr->pnt()->distance(theLastPoint)) {
    std::shared_ptr<GeomAPI_Pnt2d> aTmpPoint = theFirstPoint;
    theFirstPoint = theLastPoint;
    theLastPoint = aTmpPoint;
  }
}

void SketchPlugin_Trim::arrangePointsOnArc(const FeaturePtr& theArc,
                                  const AttributePoint2DPtr& theStartPointAttr,
                                  const AttributePoint2DPtr& theEndPointAttr,
                                  std::shared_ptr<GeomAPI_Pnt2d>& theFirstPoint,
                                  std::shared_ptr<GeomAPI_Pnt2d>& theSecondPoint) const
{
  if (!theFirstPoint.get() || !theSecondPoint.get())
    return;

  static const double anAngleTol = 1.e-12;

  std::shared_ptr<GeomAPI_Pnt2d> aCenter = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
      theArc->attribute(SketchPlugin_Arc::CENTER_ID()))->pnt();
  bool isReversed = theArc->boolean(SketchPlugin_Arc::REVERSED_ID())->value();

  // collect directions to each point
  std::shared_ptr<GeomAPI_Dir2d> aStartDir(
      new GeomAPI_Dir2d(theStartPointAttr->pnt()->xy()->decreased(aCenter->xy())));
  std::shared_ptr<GeomAPI_Dir2d> aFirstPtDir(
      new GeomAPI_Dir2d(theFirstPoint->xy()->decreased(aCenter->xy())));
  std::shared_ptr<GeomAPI_Dir2d> aSecondPtDir(
      new GeomAPI_Dir2d(theSecondPoint->xy()->decreased(aCenter->xy())));

  // sort points by their angular values
  double aFirstPtAngle = aStartDir->angle(aFirstPtDir);
  double aSecondPtAngle = aStartDir->angle(aSecondPtDir);
  double aPeriod = isReversed ? -2.0 * PI : 2.0 * PI;
  if (fabs(aFirstPtAngle) > anAngleTol && isReversed == (aFirstPtAngle > 0.))
    aFirstPtAngle += aPeriod;
  if (fabs(aSecondPtAngle) > anAngleTol && isReversed == (aSecondPtAngle > 0.))
    aSecondPtAngle += aPeriod;

  if (fabs(aFirstPtAngle) > fabs(aSecondPtAngle)) {
    std::shared_ptr<GeomAPI_Pnt2d> aTmpPoint = theFirstPoint;
    theFirstPoint = theSecondPoint;
    theSecondPoint = aTmpPoint;
  }
}

void SketchPlugin_Trim::fillPointAttribute(const AttributePtr& theModifiedAttribute,
                                           const std::shared_ptr<GeomAPI_Pnt2d>& thePoint)
{
  std::string anAttributeType = theModifiedAttribute->attributeType();
  if (anAttributeType == GeomDataAPI_Point2D::typeId()) {
    AttributePoint2DPtr aModifiedAttribute = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
                                              theModifiedAttribute);
    aModifiedAttribute->setValue(thePoint);

#ifdef DEBUG_TRIM
    FeaturePtr aFeature = ModelAPI_Feature::feature(theModifiedAttribute->owner());
    std::cout << "<fillPointAttribute[" << aFeature->data()->name() << ": " <<
      theModifiedAttribute->id() <<
      "]> => Pnt2d - [" << thePoint->x() << ", " << thePoint->y() << "]" << std::endl;
#endif
  }
}


void SketchPlugin_Trim::fillAttribute(const AttributePtr& theModifiedAttribute,
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
  else if (anAttributeType == ModelAPI_AttributeRefAttr::typeId()) {
    AttributeRefAttrPtr aRefAttributeToFill = std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(
                                                                             theModifiedAttribute);
    AttributeRefAttrPtr aSourceRefAttr = std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(
                                         theSourceAttribute);
    if (!aSourceRefAttr.get())
      aRefAttributeToFill->setAttr(theSourceAttribute);
    else {
      if (aSourceRefAttr->isObject())
        aRefAttributeToFill->setObject(aSourceRefAttr->object());
      else
        aRefAttributeToFill->setAttr(aSourceRefAttr->attr());
    }
  }
}

FeaturePtr SketchPlugin_Trim::createLineFeature(const FeaturePtr& theBaseFeature,
                                        const std::shared_ptr<GeomAPI_Pnt2d>& theFirstPoint,
                                        const std::shared_ptr<GeomAPI_Pnt2d>& theSecondPoint)
{
#ifdef DEBUG_TRIM
  std::cout << "---- createLineFeature ---" << std::endl;
#endif

  FeaturePtr aFeature;
  SketchPlugin_Sketch* aSketch = sketch();
  if (!aSketch || !theBaseFeature.get())
    return aFeature;

  aFeature = aSketch->addFeature(SketchPlugin_Line::ID());

  fillPointAttribute(aFeature->attribute(SketchPlugin_Line::START_ID()), theFirstPoint);
  fillPointAttribute(aFeature->attribute(SketchPlugin_Line::END_ID()), theSecondPoint);

  fillAttribute(aFeature->attribute(SketchPlugin_SketchEntity::AUXILIARY_ID()),
                theBaseFeature->attribute(SketchPlugin_SketchEntity::AUXILIARY_ID()));

  aFeature->execute(); // to obtain result

#ifdef DEBUG_TRIM
  std::cout << "---- createLineFeature:end ---" << std::endl;
#endif

  return aFeature;
}

FeaturePtr SketchPlugin_Trim::createArcFeature(const FeaturePtr& theBaseFeature,
                                               const std::shared_ptr<GeomAPI_Pnt2d>& theFirstPoint,
                                               const std::shared_ptr<GeomAPI_Pnt2d>& theSecondPoint)
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

#ifdef DEBUG_TRIM
  std::cout << "---- createArcFeature ---" << std::endl;
#endif

  aFeature = aSketch->addFeature(SketchPlugin_Arc::ID());
  // update fillet arc: make the arc correct for sure, so, it is not needed to process
  // the "attribute updated"
  // by arc; moreover, it may cause cyclicity in hte mechanism of updater
  bool aWasBlocked = aFeature->data()->blockSendAttributeUpdated(true);

  fillAttribute(aFeature->attribute(SketchPlugin_Arc::CENTER_ID()),
                theBaseFeature->attribute(aCenterAttributeId));
  fillPointAttribute(aFeature->attribute(SketchPlugin_Arc::START_ID()), theFirstPoint);
  fillPointAttribute(aFeature->attribute(SketchPlugin_Arc::END_ID()), theSecondPoint);

  fillAttribute(aFeature->attribute(SketchPlugin_SketchEntity::AUXILIARY_ID()),
                theBaseFeature->attribute(SketchPlugin_SketchEntity::AUXILIARY_ID()));

  /// fill referersed state of created arc as it is on the base arc
  if (theBaseFeature->getKind() == SketchPlugin_Arc::ID()) {
    bool aReversed = theBaseFeature->boolean(SketchPlugin_Arc::REVERSED_ID())->value();
    aFeature->boolean(SketchPlugin_Arc::REVERSED_ID())->setValue(aReversed);
  }
  //aFeature->execute(); // to obtain result
  aFeature->data()->blockSendAttributeUpdated(aWasBlocked);

  #ifdef DEBUG_TRIM
  std::cout << "---- createArcFeature:end ---" << std::endl;
  #endif

  return aFeature;
}

FeaturePtr SketchPlugin_Trim::createConstraint(const std::string& theConstraintId,
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

#ifdef DEBUG_TRIM
  std::cout << "<createConstraint to attribute> :"
            << "first attribute - " << theFirstAttribute->id()
            << "second attribute - " << theSecondAttribute->id()
            << std::endl;
#endif

  return aConstraint;
}

FeaturePtr SketchPlugin_Trim::createConstraintToObject(const std::string& theConstraintId,
                                               const AttributePtr& theFirstAttribute,
                                               const ObjectPtr& theSecondObject)
{
  FeaturePtr aConstraint = sketch()->addFeature(theConstraintId);
  AttributeRefAttrPtr aRefAttr = std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(
                                 aConstraint->attribute(SketchPlugin_Constraint::ENTITY_A()));
  aRefAttr->setAttr(theFirstAttribute);

  aRefAttr = std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(
                                 aConstraint->attribute(SketchPlugin_Constraint::ENTITY_B()));
  aRefAttr->setObject(theSecondObject);

#ifdef DEBUG_TRIM
  std::cout << "<createConstraint to attribute> :"
            << "first attribute - " << theFirstAttribute->id()
            << "second object - " << ModelAPI_Feature::feature(theSecondObject)->getKind()
            << std::endl;
#endif

  return aConstraint;
}

FeaturePtr SketchPlugin_Trim::createConstraintForObjects(
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

std::shared_ptr<ModelAPI_Result> SketchPlugin_Trim::getFeatureResult(
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

//********************************************************************
void SketchPlugin_Trim::fillObjectShapes(const ObjectPtr& theObject,
                const ObjectPtr& theSketch,
                std::map<ObjectPtr, std::set<GeomShapePtr> >& theCashedShapes,
                std::map<ObjectPtr, PointToRefsMap>& theObjectToPoints)
{
  PointToRefsMap aPointsInfo;

  std::set<std::shared_ptr<GeomAPI_Shape> > aShapes;
  std::map<std::shared_ptr<GeomAPI_Pnt>,
                           std::list< AttributePoint2DPtr > > aPointToAttributes;
  std::map<std::shared_ptr<GeomAPI_Pnt>,
                           std::list< ObjectPtr > > aPointToObjects;

  std::set<AttributePoint2DPtr > aRefAttributes;
  // current feature
  FeaturePtr aFeature = ModelAPI_Feature::feature(theObject);
  std::set<ResultPtr> anEdgeShapes;
  // edges on feature
  ModelGeomAlgo_Shape::shapesOfType(aFeature, GeomAPI_Shape::EDGE, anEdgeShapes);
  if (!anEdgeShapes.empty()) {
    GeomShapePtr aFeatureShape = (*anEdgeShapes.begin())->shape();

    // coincidences to the feature
    ModelGeomAlgo_Point2D::getPointsOfReference(aFeature, SketchPlugin_ConstraintCoincidence::ID(),
                         aRefAttributes, SketchPlugin_Point::ID(), SketchPlugin_Point::COORD_ID());
    // layed on feature coincidences to divide it on several shapes
    //SketchPlugin_Sketch* aSketch = sketch();
    std::shared_ptr<ModelAPI_Data> aData = theSketch->data();
    std::shared_ptr<GeomDataAPI_Point> aC = std::dynamic_pointer_cast<GeomDataAPI_Point>(
        aData->attribute(SketchPlugin_Sketch::ORIGIN_ID()));
    std::shared_ptr<GeomDataAPI_Dir> aX = std::dynamic_pointer_cast<GeomDataAPI_Dir>(
        aData->attribute(SketchPlugin_Sketch::DIRX_ID()));
    std::shared_ptr<GeomDataAPI_Dir> aNorm = std::dynamic_pointer_cast<GeomDataAPI_Dir>(
        aData->attribute(SketchPlugin_Sketch::NORM_ID()));
    std::shared_ptr<GeomAPI_Dir> aY(new GeomAPI_Dir(aNorm->dir()->cross(aX->dir())));

    ModelGeomAlgo_Point2D::getPointsInsideShape(aFeatureShape, aRefAttributes, aC->pnt(),
                                                aX->dir(), aY, aPointsInfo);

    std::list<FeaturePtr> aFeatures;
    CompositeFeaturePtr aSketchComposite =
                         std::dynamic_pointer_cast<ModelAPI_CompositeFeature>(theSketch);
    for (int i = 0; i < aSketchComposite->numberOfSubs(); i++) {
      FeaturePtr aFeature = aSketchComposite->subFeature(i);
      if (aFeature.get())
        aFeatures.push_back(aFeature);
    }
    ModelGeomAlgo_Point2D::getPointsIntersectedShape(aFeature, aFeatures, aPointsInfo);

    GeomAlgoAPI_ShapeTools::splitShape(aFeatureShape, aPointsInfo, aShapes);
  }
  theObjectToPoints[theObject] = aPointsInfo;
  theCashedShapes[theObject] = aShapes;
}
