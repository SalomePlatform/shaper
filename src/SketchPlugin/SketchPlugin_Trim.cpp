// Copyright (C) 2014-2020  CEA/DEN, EDF R&D
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

#include "SketchPlugin_Trim.h"

#include <GeomAPI_Dir2d.h>
#include <GeomAPI_Edge.h>
#include <GeomAPI_Pnt2d.h>
#include <GeomAPI_XY.h>
#include <GeomDataAPI_Point2D.h>
#include <GeomAlgoAPI_ShapeTools.h>

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
#include <SketchPlugin_ConstraintTangent.h>
#include <SketchPlugin_ConstraintLength.h>
#include <SketchPlugin_ConstraintMirror.h>
#include <SketchPlugin_ConstraintCollinear.h>
#include <SketchPlugin_Ellipse.h>
#include <SketchPlugin_EllipticArc.h>
#include <SketchPlugin_Line.h>
#include <SketchPlugin_MultiRotation.h>
#include <SketchPlugin_MultiTranslation.h>
#include <SketchPlugin_Point.h>

#include <ModelAPI_EventReentrantMessage.h>

#include <ModelAPI_Events.h>
#include <SketchPlugin_Line.h>
#include <SketchPlugin_Arc.h>
#include <SketchPlugin_Circle.h>

#include <ModelGeomAlgo_Point2D.h>
#include <Events_Loop.h>

#include <cmath>

#ifdef DEBUG_TRIM
#include <iostream>
#endif

#ifdef DEBUG_TRIM_METHODS
#include <iostream>
#endif

static const double PI = 3.141592653589793238463;

SketchPlugin_Trim::SketchPlugin_Trim()
{
}

void SketchPlugin_Trim::initAttributes()
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

  if (myCashedShapes.find(aBaseObject) == myCashedShapes.end()) {
    SketchPlugin_SegmentationTools::fillObjectShapes(
        this, aBaseObject, myCashedShapes, myObjectToPoints);
  }

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
  if (!thePoint.get())
    return aPoint;

  AttributeReferencePtr aBaseObjectAttr = std::dynamic_pointer_cast<ModelAPI_AttributeReference>(
                                        data()->attribute(SketchPlugin_Trim::SELECTED_OBJECT()));
  ObjectPtr aBaseObject = aBaseObjectAttr->value();
  if (myObjectToPoints.find(aBaseObject) == myObjectToPoints.end()) {
    SketchPlugin_SegmentationTools::fillObjectShapes(
        this, aBaseObject, myCashedShapes, myObjectToPoints);
  }

  bool aFound = false;
  const GeomAlgoAPI_ShapeTools::PointToRefsMap& aRefsMap = myObjectToPoints.at(aBaseObject);
  for (GeomAlgoAPI_ShapeTools::PointToRefsMap::const_iterator aPointIt = aRefsMap.begin();
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
#ifdef DEBUG_TRIM_METHODS
  std::cout << "SketchPlugin_Trim::execute: " << data()->name() << std::endl;
#endif

  SketchPlugin_Sketch* aSketch = sketch();
  if (!aSketch) {
    setError("Error: Sketch object is empty.");
    return;
  }

  // Check the base objects are initialized.
  AttributeReferencePtr aBaseObjectAttr = std::dynamic_pointer_cast<ModelAPI_AttributeReference>(
                                        data()->attribute(SketchPlugin_Trim::SELECTED_OBJECT()));
  if(!aBaseObjectAttr->isInitialized()) {
    setError("Error: Base object is not initialized.");
    return;
  }
  ObjectPtr aBaseObject = aBaseObjectAttr->value();
  if (!aBaseObject.get()) {
    setError("Error: Base object is not initialized.");
    return;
  }
  FeaturePtr aBaseFeature = ModelAPI_Feature::feature(aBaseObjectAttr->value());

  /// Remove reference of this feature to feature used in preview, it is not necessary anymore
  /// as trim will be removed after execute
  AttributeReferencePtr aPreviewObjectAttr =
                     std::dynamic_pointer_cast<ModelAPI_AttributeReference>(
                     data()->attribute(SketchPlugin_Trim::PREVIEW_OBJECT()));

  ObjectPtr aPreviewObject = aPreviewObjectAttr->value();
  AttributePoint2DPtr aPoint = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
                                           data()->attribute(PREVIEW_POINT()));
  std::shared_ptr<GeomAPI_Pnt2d> aPreviewPnt2d = aPoint->pnt();
  // nullify pointer of preview attribute
  aPreviewObjectAttr->setValue(ResultPtr());

  bool anIsEqualPreviewAndSelected = aPreviewObject == aBaseObject;

  /// points of trim
  std::shared_ptr<GeomAPI_Pnt> aStartShapePoint, aLastShapePoint;
#ifdef DEBUG_TRIM
  std::cout << " Base Feature: " << aBaseFeature->data()->name() << std::endl;
#endif
  findShapePoints(SELECTED_OBJECT(), SELECTED_POINT(), aStartShapePoint, aLastShapePoint);
  if (!aStartShapePoint || !aLastShapePoint) {
    setError("Error: Selected point is not placed on any edge");
    return;
  }

  std::shared_ptr<GeomAPI_Pnt2d> aStartShapePoint2d = convertPoint(aStartShapePoint);
  std::shared_ptr<GeomAPI_Pnt2d> aLastShapePoint2d = convertPoint(aLastShapePoint);
  /// find features that should be deleted (e.g. Middle Point) or updated (e.g. Length)
  std::set<FeaturePtr> aFeaturesToDelete, aFeaturesToUpdate;
  getConstraints(aFeaturesToDelete, aFeaturesToUpdate);
  // find references(attributes and features) to the base feature
  std::map<AttributePtr, std::list<AttributePtr> > aBaseRefAttributes;
  std::list<AttributePtr> aRefsToFeature;
  SketchPlugin_SegmentationTools::getRefAttributes(
      aBaseFeature, aBaseRefAttributes, aRefsToFeature);
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

  keepCurrentFeature();

  std::set<AttributePoint2DPtr> aFurtherCoincidences;
  std::set<std::pair<AttributePtr, AttributePtr>> aModifiedAttributes;
  const std::string& aKind = aBaseFeature->getKind();
  FeaturePtr aReplacingFeature, aNewFeature;
  if (aKind == SketchPlugin_Circle::ID() ||
      aKind == SketchPlugin_Ellipse::ID()) {
    aReplacingFeature = trimClosed(aStartShapePoint2d, aLastShapePoint2d,
               aFurtherCoincidences, aModifiedAttributes);

    aFeaturesToDelete.insert(aBaseFeature);
    // as circle is removed, erase it from dependencies(arguments) of this feature
    // otherwise Trim feature will be removed with the circle before
    // this operation is finished
    aBaseObjectAttr->setObject(ResultPtr());
  }
  else if (aKind == SketchPlugin_Line::ID()) {
    aNewFeature = trimLine(aStartShapePoint2d, aLastShapePoint2d, aBaseRefAttributes,
                           aFurtherCoincidences, aModifiedAttributes);
  }
  else if (aKind == SketchPlugin_Arc::ID()) {
    aNewFeature = trimArc(aStartShapePoint2d, aLastShapePoint2d, aBaseRefAttributes,
                          aFurtherCoincidences, aModifiedAttributes);
  }
  else if (aKind == SketchPlugin_EllipticArc::ID()) {
    aNewFeature = trimEllipticArc(aStartShapePoint2d, aLastShapePoint2d, aBaseRefAttributes,
                          aFurtherCoincidences, aModifiedAttributes);
  }

  restoreCurrentFeature();

  // constraints to end points of trim feature
  if (myObjectToPoints.find(aBaseObject) == myObjectToPoints.end()) {
    SketchPlugin_SegmentationTools::fillObjectShapes(
        this, aBaseObject, myCashedShapes, myObjectToPoints);
  }

  // create coincidence to objects, intersected the base object
  const GeomAlgoAPI_ShapeTools::PointToRefsMap& aRefsMap = myObjectToPoints.at(aBaseObject);
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
    for (GeomAlgoAPI_ShapeTools::PointToRefsMap::const_iterator aRefIt = aRefsMap.begin();
         aRefIt != aRefsMap.end(); aRefIt++)
    {
      if (aRefIt->first->isEqual(aPoint)) {
        anInfo = aRefIt->second;
        // prefer a segment instead of a point, because further coincidence with a segment
        // decreases only 1 DoF (instead of 2 for point) and prevents an overconstraint situation.
        bool isEdge = false;
        for (std::list<ObjectPtr>::const_iterator anIt = anInfo.second.begin();
             anIt != anInfo.second.end() && !isEdge; ++anIt) {
          ResultPtr aResult = std::dynamic_pointer_cast<ModelAPI_Result>(*anIt);
          if (aResult) {
            GeomShapePtr aShape = aResult->shape();
            isEdge = aShape && aShape->isEdge();
          }
        }
        if (isEdge)
          break;
      }
    }
    const std::list<ObjectPtr>& anObjects = anInfo.second;
    for (std::list<ObjectPtr>::const_iterator anObjectIt = anObjects.begin();
      anObjectIt != anObjects.end(); anObjectIt++) {
      SketchPlugin_Tools::createConstraintAttrObject(sketch(),
            SketchPlugin_ConstraintCoincidence::ID(),
            aPointAttribute, *anObjectIt);
    }
  }

  // move constraints from base feature to replacing feature: ignore coincidences to feature
  // if attributes of coincidence participated in split
  ResultPtr aReplacingResult;
  if (aReplacingFeature.get()) {
    aReplacingFeature->execute(); // need it to obtain result
    aReplacingResult = aReplacingFeature->lastResult();
  }
  for(std::list<AttributePtr>::const_iterator anIt = aRefsToFeature.begin(),
                                          aLast = aRefsToFeature.end();
      anIt != aLast; anIt++) {
    AttributePtr anAttribute = *anIt;

    if (setCoincidenceToAttribute(anAttribute, aFurtherCoincidences, aFeaturesToDelete))
      continue;

    // move tangency constraint to the nearest feature if possible
    if (aNewFeature.get() && moveTangency(anAttribute, aNewFeature))
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

  SketchPlugin_SegmentationTools::updateRefAttConstraints(aBaseRefAttributes, aModifiedAttributes);

  // Wait all constraints being created, then send update events
  static Events_ID anUpdateEvent = Events_Loop::eventByName(EVENT_OBJECT_UPDATED);
  bool isUpdateFlushed = Events_Loop::loop()->isFlushed(anUpdateEvent);
  if (isUpdateFlushed)
    Events_Loop::loop()->setFlushed(anUpdateEvent, false);

  // delete constraints
#ifdef DEBUG_TRIM
  if (aFeaturesToDelete.size() > 0) {
    std::cout << "after SPlit: removeFeaturesAndReferences: " << std::endl;
    std::string aValue;
    for (std::set<FeaturePtr>::const_iterator anIt = aFeaturesToDelete.begin();
         anIt != aFeaturesToDelete.end(); anIt++) {
      FeaturePtr aFeature = *anIt;
      std::cout << aFeature->data()->name() << std::endl;
    }
  }
#endif
  ModelAPI_Tools::removeFeaturesAndReferences(aFeaturesToDelete);
  Events_Loop::loop()->flush(Events_Loop::eventByName(EVENT_OBJECT_DELETED));

  SketchPlugin_SegmentationTools::updateFeaturesAfterOperation(aFeaturesToUpdate);

  // Send events to update the sub-features by the solver.
  if(isUpdateFlushed) {
    Events_Loop::loop()->setFlushed(anUpdateEvent, true);
  }

  if (anIsEqualPreviewAndSelected) {
    // equal preview and selected objects
    // nothing to do if the preview and selected objects are different
    if (aReplacingResult.get()) { // base object was removed
      aPreviewObject = aReplacingResult;
      //aMessage->setSelectedObject(aReplacingResult);
#ifdef DEBUG_TRIM_METHODS
      if (!aSelectedShape.get())
        std::cout << "Set empty selected object" << std::endl;
      else
        std::cout << "Set shape with ShapeType: " << aSelectedShape->shapeTypeStr() << std::endl;
#endif
    }
    else {
      aPreviewObject = ObjectPtr();

      aBaseFeature->execute(); // should recompute shapes of result to do not check obsolete one
      aBaseObject = aBaseFeature->lastResult();
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
        ResultPtr aNewFeatureResult = aNewFeature->lastResult();
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
#ifdef DEBUG_TRIM
  std::cout << "SketchPlugin_Trim::done" << std::endl;
#endif
}

// LCOV_EXCL_START
std::string SketchPlugin_Trim::processEvent(const std::shared_ptr<Events_Message>& theMessage)
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
      if (myCashedShapes.find(anObject) == myCashedShapes.end()) {
        SketchPlugin_SegmentationTools::fillObjectShapes(
            this, anObject, myCashedShapes, myObjectToPoints);
      }
      const std::set<GeomShapePtr>& aShapes = myCashedShapes[anObject];
      if (aShapes.size() > 1) {
        std::shared_ptr<ModelAPI_AttributeReference> aRefSelectedAttr =
                              std::dynamic_pointer_cast<ModelAPI_AttributeReference>(
                              data()->attribute(SketchPlugin_Trim::SELECTED_OBJECT()));
        std::shared_ptr<ModelAPI_AttributeReference> aRefPreviewAttr =
                              std::dynamic_pointer_cast<ModelAPI_AttributeReference>(
                              data()->attribute(SketchPlugin_Trim::PREVIEW_OBJECT()));
        aRefSelectedAttr->setValue(anObject);
        aRefPreviewAttr->setValue(anObject);

        std::shared_ptr<GeomDataAPI_Point2D> aPointSelectedAttr =
                              std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
                              data()->attribute(SketchPlugin_Trim::SELECTED_POINT()));
        std::shared_ptr<GeomDataAPI_Point2D> aPointPreviewAttr =
                              std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
                              data()->attribute(SketchPlugin_Trim::PREVIEW_POINT()));
        aPointSelectedAttr->setValue(aPoint);
        aPointPreviewAttr->setValue(aPoint);

        Events_Loop::loop()->flush(Events_Loop::eventByName(EVENT_OBJECT_UPDATED));

        GeomShapePtr aSelectedShape = SketchPlugin_SegmentationTools::getSubShape(this,
            SELECTED_OBJECT(), SELECTED_POINT(), myCashedShapes, myObjectToPoints);
  #ifdef DEBUG_TRIM_METHODS
        if (!aSelectedShape.get())
          std::cout << "Set empty selected object" << std::endl;
        else
          std::cout << "Set shape with ShapeType: " << aSelectedShape->shapeTypeStr() << std::endl;
  #endif
        aFilledAttributeName = SketchPlugin_Trim::SELECTED_OBJECT();
      }
    }
  }
  return aFilledAttributeName;
}
// LCOV_EXCL_STOP

bool SketchPlugin_Trim::setCoincidenceToAttribute(const AttributePtr& theAttribute,
                                const std::set<AttributePoint2DPtr>& theFurtherCoincidences,
                                std::set<std::shared_ptr<ModelAPI_Feature>>& theFeaturesToDelete)
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
      // create new coincidence and then remove the old one
      SketchPlugin_Tools::createConstraintAttrAttr(sketch(),
          SketchPlugin_ConstraintCoincidence::ID(),
          aRefPointAttr, aPointAttribute);
      theFeaturesToDelete.insert(aFeature);
    }
  }
  return aFoundPoint;
}

bool SketchPlugin_Trim::moveTangency(const AttributePtr& theAttribute,
                                     const FeaturePtr& theFeature)
{
  FeaturePtr aFeature = ModelAPI_Feature::feature(theAttribute->owner());
  if (aFeature->getKind() != SketchPlugin_ConstraintTangent::ID())
    return false;

  AttributeRefAttrPtr aRefAttr = std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(
                                                                           theAttribute);
  if (!aRefAttr.get())
    return false;

  // get shape of tangent object to the current
  std::string aTangentAttr = SketchPlugin_Constraint::ENTITY_A();
  if (aRefAttr->id() == SketchPlugin_Constraint::ENTITY_A())
    aTangentAttr = SketchPlugin_Constraint::ENTITY_B();
  AttributeRefAttrPtr aTangentRefAttr = std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(
                                                     aFeature->attribute(aTangentAttr));
  FeaturePtr aTangentFeature = ModelAPI_Feature::feature(aTangentRefAttr->object());

  // get shape of the feature of the attribute
  FeaturePtr anAttributeFeature = ModelAPI_Feature::feature(aRefAttr->object());
  anAttributeFeature->execute(); // the modified value should be applyed to recompute shape
  GeomAlgoAPI_ShapeTools::PointToRefsMap aPointToAttributeOrObject;
  std::list<FeaturePtr> aFeatures;
  aFeatures.push_back(anAttributeFeature);
  ModelGeomAlgo_Point2D::getPointsIntersectedShape(aTangentFeature, aFeatures,
                                                   aPointToAttributeOrObject);
  if (!aPointToAttributeOrObject.empty())
    return true; // the attribute feature has a point of intersection, so we do not replace it

  // get shape of the feature
  aPointToAttributeOrObject.clear();
  aFeatures.clear();
  aFeatures.push_back(theFeature);
  ModelGeomAlgo_Point2D::getPointsIntersectedShape(aTangentFeature, aFeatures,
                                                   aPointToAttributeOrObject);
  if (!aPointToAttributeOrObject.empty()) {
    std::set<ResultPtr> anEdgeShapes;
    ModelGeomAlgo_Shape::shapesOfType(theFeature, GeomAPI_Shape::EDGE, anEdgeShapes);
    if (!anEdgeShapes.empty()) {
      ResultPtr aResult = *anEdgeShapes.begin();
      if (aResult.get()) {
        aRefAttr->setObject(aResult);
        return true; // the attribute feature has a point of intersection, so we do not replace it
      }
    }
  }
  return false;
}

AISObjectPtr SketchPlugin_Trim::getAISObject(AISObjectPtr thePrevious)
{
  return SketchPlugin_SegmentationTools::getAISObject(thePrevious,
      this, PREVIEW_OBJECT(), PREVIEW_POINT(), SELECTED_OBJECT(), SELECTED_POINT());
}

void SketchPlugin_Trim::getConstraints(std::set<FeaturePtr>& theFeaturesToDelete,
                                       std::set<FeaturePtr>& theFeaturesToUpdate)
{
  std::shared_ptr<ModelAPI_Data> aData = data();

  // Check the base objects are initialized.
  AttributeReferencePtr aBaseObjectAttr = std::dynamic_pointer_cast<ModelAPI_AttributeReference>(
                                         aData->attribute(SketchPlugin_Trim::SELECTED_OBJECT()));
  FeaturePtr aBaseFeature = ModelAPI_Feature::feature(aBaseObjectAttr->value());
  ResultPtr aBaseFeatureResult = aBaseFeature->lastResult();

  std::set<AttributePtr> aRefsList = aBaseFeatureResult->data()->refsToMe();
  std::set<AttributePtr> aFRefsList = aBaseFeature->data()->refsToMe();
  aRefsList.insert(aFRefsList.begin(), aFRefsList.end());

  std::set<AttributePtr>::const_iterator aIt;
  for (aIt = aRefsList.cbegin(); aIt != aRefsList.cend(); ++aIt) {
    std::shared_ptr<ModelAPI_Attribute> anAttr = (*aIt);
    FeaturePtr aRefFeature = std::dynamic_pointer_cast<ModelAPI_Feature>(anAttr->owner());
    std::string aRefFeatureKind = aRefFeature->getKind();
    std::string anAttributeId = anAttr->id();
    if ((aRefFeatureKind == SketchPlugin_ConstraintMirror::ID() &&
         anAttributeId == SketchPlugin_ConstraintMirror::MIRROR_LIST_ID()) ||
        (aRefFeatureKind == SketchPlugin_MultiRotation::ID() &&
         anAttributeId == SketchPlugin_MultiRotation::ROTATION_LIST_ID()) ||
        (aRefFeatureKind == SketchPlugin_MultiTranslation::ID() &&
         anAttributeId == SketchPlugin_MultiTranslation::TRANSLATION_LIST_ID()) ||
        aRefFeatureKind == SketchPlugin_ConstraintMiddle::ID())
      theFeaturesToDelete.insert(aRefFeature);
    else if (aRefFeatureKind == SketchPlugin_ConstraintLength::ID())
      theFeaturesToUpdate.insert(aRefFeature);
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

#ifdef DEBUG_TRIM
  // delete constraints
  if (aFeaturesToDelete.size() > 0) {
    std::cout << "removeReferencesToAttribute: " << std::endl;
    std::string aValue;
    for (std::set<FeaturePtr>::const_iterator anIt = aFeaturesToDelete.begin();
         anIt != aFeaturesToDelete.end(); anIt++) {
      FeaturePtr aFeature = *anIt;
      std::cout << aFeature->data()->name() << std::endl;
    }
  }
#endif
  ModelAPI_Tools::removeFeaturesAndReferences(aFeaturesToDelete);
  Events_Loop::loop()->flush(Events_Loop::eventByName(EVENT_OBJECT_DELETED));
}

FeaturePtr SketchPlugin_Trim::trimLine(const std::shared_ptr<GeomAPI_Pnt2d>& theStartShapePoint,
                  const std::shared_ptr<GeomAPI_Pnt2d>& theLastShapePoint,
                  std::map<AttributePtr, std::list<AttributePtr> >& theBaseRefAttributes,
                  std::set<AttributePoint2DPtr>& thePoints,
                  std::set<std::pair<AttributePtr, AttributePtr>>& theModifiedAttributes)
{
  FeaturePtr anNewFeature;

  // Check the base objects are initialized.
  AttributeReferencePtr aBaseObjectAttr = std::dynamic_pointer_cast<ModelAPI_AttributeReference>(
                                        data()->attribute(SketchPlugin_Trim::SELECTED_OBJECT()));
  ObjectPtr aBaseObject = aBaseObjectAttr->value();
  FeaturePtr aBaseFeature = ModelAPI_Feature::feature(aBaseObjectAttr->value());

  /// points of trim
  AttributePoint2DPtr aStartPointAttrOfBase, anEndPointAttrOfBase;
  SketchPlugin_SegmentationTools::getFeaturePoints(
      aBaseFeature, aStartPointAttrOfBase, anEndPointAttrOfBase);

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
    anNewFeature = SketchPlugin_SegmentationTools::createLineFeature(
        aBaseFeature, aLastShapePoint, aLastFeaturePoint);
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
    SketchPlugin_Tools::createConstraintObjectObject(sketch(),
                                         SketchPlugin_ConstraintCollinear::ID(),
                                         aBaseFeature->lastResult(),
                                         anNewFeature->lastResult());
  }
  return anNewFeature;
}

FeaturePtr SketchPlugin_Trim::trimArc(const std::shared_ptr<GeomAPI_Pnt2d>& theStartShapePoint,
                 const std::shared_ptr<GeomAPI_Pnt2d>& theLastShapePoint,
                 std::map<AttributePtr, std::list<AttributePtr> >& theBaseRefAttributes,
                 std::set<AttributePoint2DPtr>& thePoints,
                 std::set<std::pair<AttributePtr, AttributePtr>>& theModifiedAttributes)
{
  FeaturePtr anNewFeature;
  // Check the base objects are initialized.
  AttributeReferencePtr aBaseObjectAttr = std::dynamic_pointer_cast<ModelAPI_AttributeReference>(
                                        data()->attribute(SketchPlugin_Trim::SELECTED_OBJECT()));
  ObjectPtr aBaseObject = aBaseObjectAttr->value();
  FeaturePtr aBaseFeature = ModelAPI_Feature::feature(aBaseObjectAttr->value());

  /// points of trim
  AttributePoint2DPtr aStartPointAttrOfBase, anEndPointAttrOfBase;
  SketchPlugin_SegmentationTools::getFeaturePoints(
      aBaseFeature, aStartPointAttrOfBase, anEndPointAttrOfBase);

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
    anNewFeature = SketchPlugin_SegmentationTools::createArcFeature(
        aBaseFeature, aLastShapePoint, aLastArcPoint);
    thePoints.insert(std::dynamic_pointer_cast<GeomDataAPI_Point2D>
                               (anNewFeature->attribute(SketchPlugin_Arc::START_ID())));

    std::string aModifiedAttribute = SketchPlugin_Arc::END_ID();
    theModifiedAttributes.insert(
      std::make_pair(aBaseFeature->attribute(aModifiedAttribute),
                                   anNewFeature->attribute(SketchPlugin_Arc::END_ID())));

    // modify base arc
    fillPointAttribute(aBaseFeature->attribute(aModifiedAttribute), aStartShapePoint);

    thePoints.insert(std::dynamic_pointer_cast<GeomDataAPI_Point2D>
                               (aBaseFeature->attribute(aModifiedAttribute)));

    // equal Radius constraint for arcs
    SketchPlugin_Tools::createConstraintObjectObject(sketch(),
                                         SketchPlugin_ConstraintEqual::ID(),
                                         aBaseFeature->lastResult(),
                                         anNewFeature->lastResult());
    // coincident centers constraint
    SketchPlugin_Tools::createConstraintAttrAttr(sketch(),
                                         SketchPlugin_ConstraintCoincidence::ID(),
                                         aBaseFeature->attribute(SketchPlugin_Arc::CENTER_ID()),
                                         anNewFeature->attribute(SketchPlugin_Arc::CENTER_ID()));

#ifdef DEBUG_TRIM
    std::cout << "Created arc on points:" << std::endl;
    std::cout << "Start shape point: [" << aStartShapePoint->x() << ", " <<
                                           aStartShapePoint->y() << "]" << std::endl;
#endif
  }
  return anNewFeature;
}

FeaturePtr SketchPlugin_Trim::trimEllipticArc(
                 const std::shared_ptr<GeomAPI_Pnt2d>& theStartShapePoint,
                 const std::shared_ptr<GeomAPI_Pnt2d>& theLastShapePoint,
                 std::map<AttributePtr, std::list<AttributePtr> >& theBaseRefAttributes,
                 std::set<AttributePoint2DPtr>& thePoints,
                 std::set<std::pair<AttributePtr, AttributePtr>>& theModifiedAttributes)
{
  FeaturePtr anNewFeature;
  // Check the base objects are initialized.
  AttributeReferencePtr aBaseObjectAttr = reference(SELECTED_OBJECT());
  ObjectPtr aBaseObject = aBaseObjectAttr->value();
  FeaturePtr aBaseFeature = ModelAPI_Feature::feature(aBaseObjectAttr->value());

  // points of trim
  AttributePoint2DPtr aStartPointAttrOfBase, anEndPointAttrOfBase;
  SketchPlugin_SegmentationTools::getFeaturePoints(
      aBaseFeature, aStartPointAttrOfBase, anEndPointAttrOfBase);

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
    std::string aModifiedAttribute = isStartPoint ? SketchPlugin_EllipticArc::START_POINT_ID()
                                                  : SketchPlugin_EllipticArc::END_POINT_ID();
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
    anNewFeature = SketchPlugin_SegmentationTools::createArcFeature(
        aBaseFeature, aLastShapePoint, aLastArcPoint);
    thePoints.insert(std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
                     anNewFeature->attribute(SketchPlugin_EllipticArc::START_POINT_ID())));

    std::string aModifiedAttribute = SketchPlugin_EllipticArc::END_POINT_ID();
    theModifiedAttributes.insert(
      std::make_pair(aBaseFeature->attribute(aModifiedAttribute),
                     anNewFeature->attribute(SketchPlugin_EllipticArc::END_POINT_ID())));

    // modify base arc
    fillPointAttribute(aBaseFeature->attribute(aModifiedAttribute), aStartShapePoint);

    thePoints.insert(std::dynamic_pointer_cast<GeomDataAPI_Point2D>
                               (aBaseFeature->attribute(aModifiedAttribute)));

    // make elliptic arcs equal
    SketchPlugin_Tools::createConstraintObjectObject(sketch(),
                                         SketchPlugin_ConstraintEqual::ID(),
                                         aBaseFeature->lastResult(),
                                         anNewFeature->lastResult());
    // coincident centers constraint
    SketchPlugin_Tools::createConstraintAttrAttr(sketch(),
        SketchPlugin_ConstraintCoincidence::ID(),
        aBaseFeature->attribute(SketchPlugin_EllipticArc::CENTER_ID()),
        anNewFeature->attribute(SketchPlugin_EllipticArc::CENTER_ID()));

#ifdef DEBUG_TRIM
    std::cout << "Created arc on points:" << std::endl;
    std::cout << "Start shape point: [" << aStartShapePoint->x() << ", " <<
                                           aStartShapePoint->y() << "]" << std::endl;
#endif
  }
  return anNewFeature;
}

FeaturePtr SketchPlugin_Trim::trimClosed(const std::shared_ptr<GeomAPI_Pnt2d>& theStartShapePoint,
                                         const std::shared_ptr<GeomAPI_Pnt2d>& theLastShapePoint,
                                         std::set<AttributePoint2DPtr>& thePoints,
                           std::set<std::pair<AttributePtr, AttributePtr>>& theModifiedAttributes)
{
  // Check the base objects are initialized.
  AttributeReferencePtr aBaseObjectAttr = reference(SELECTED_OBJECT());
  ObjectPtr aBaseObject = aBaseObjectAttr->value();
  FeaturePtr aBaseFeature = ModelAPI_Feature::feature(aBaseObjectAttr->value());

  // trim feature
  FeaturePtr anNewFeature = SketchPlugin_SegmentationTools::createArcFeature(
      aBaseFeature, theStartShapePoint, theLastShapePoint);
  // arc created by trim of circle is always correct, that means that it is not inversed
  const std::string& aReversedAttrName = anNewFeature->getKind() == SketchPlugin_Arc::ID() ?
      SketchPlugin_Arc::REVERSED_ID() : SketchPlugin_EllipticArc::REVERSED_ID();
  anNewFeature->boolean(aReversedAttrName)->setValue(false);

  if (aBaseFeature->getKind() == SketchPlugin_Circle::ID()) {
    theModifiedAttributes.insert(
      std::make_pair(aBaseFeature->attribute(SketchPlugin_Circle::CENTER_ID()),
                     anNewFeature->attribute(SketchPlugin_Arc::CENTER_ID())));
  }
  else if (aBaseFeature->getKind() == SketchPlugin_Ellipse::ID()) {
    theModifiedAttributes.insert(std::make_pair(
        aBaseFeature->attribute(SketchPlugin_Ellipse::CENTER_ID()),
        anNewFeature->attribute(SketchPlugin_EllipticArc::CENTER_ID())));
    theModifiedAttributes.insert(std::make_pair(
        aBaseFeature->attribute(SketchPlugin_Ellipse::FIRST_FOCUS_ID()),
        anNewFeature->attribute(SketchPlugin_EllipticArc::FIRST_FOCUS_ID())));
    theModifiedAttributes.insert(std::make_pair(
        aBaseFeature->attribute(SketchPlugin_Ellipse::SECOND_FOCUS_ID()),
        anNewFeature->attribute(SketchPlugin_EllipticArc::SECOND_FOCUS_ID())));
    theModifiedAttributes.insert(std::make_pair(
        aBaseFeature->attribute(SketchPlugin_Ellipse::MAJOR_AXIS_START_ID()),
        anNewFeature->attribute(SketchPlugin_EllipticArc::MAJOR_AXIS_START_ID())));
    theModifiedAttributes.insert(std::make_pair(
        aBaseFeature->attribute(SketchPlugin_Ellipse::MAJOR_AXIS_END_ID()),
        anNewFeature->attribute(SketchPlugin_EllipticArc::MAJOR_AXIS_END_ID())));
    theModifiedAttributes.insert(std::make_pair(
        aBaseFeature->attribute(SketchPlugin_Ellipse::MINOR_AXIS_START_ID()),
        anNewFeature->attribute(SketchPlugin_EllipticArc::MINOR_AXIS_START_ID())));
    theModifiedAttributes.insert(std::make_pair(
        aBaseFeature->attribute(SketchPlugin_Ellipse::MINOR_AXIS_END_ID()),
        anNewFeature->attribute(SketchPlugin_EllipticArc::MINOR_AXIS_END_ID())));

    // update the PARENT_ID reference for all the features created by the ellipse
    const std::set<AttributePtr>& aRefs = aBaseFeature->data()->refsToMe();
    std::list<AttributePtr> aRefsToParent;
    for (std::set<AttributePtr>::const_iterator aRef = aRefs.begin(); aRef != aRefs.end(); ++aRef) {
      if ((*aRef)->id() == SketchPlugin_SketchEntity::PARENT_ID())
        aRefsToParent.push_back(*aRef);
    }
    for (std::list<AttributePtr>::iterator aRef = aRefsToParent.begin();
         aRef != aRefsToParent.end(); ++aRef) {
      std::dynamic_pointer_cast<ModelAPI_AttributeReference>(*aRef)->setValue(anNewFeature);

      FeaturePtr anOwner = ModelAPI_Feature::feature((*aRef)->owner());
      SketchPlugin_Tools::replaceInName(anOwner, aBaseFeature->name(), anNewFeature->name());
      SketchPlugin_Tools::replaceInName(anOwner->lastResult(),
          aBaseFeature->name(), anNewFeature->name());
    }
  }

  const std::string& aStartAttrName = anNewFeature->getKind() == SketchPlugin_Arc::ID() ?
      SketchPlugin_Arc::START_ID() : SketchPlugin_EllipticArc::START_POINT_ID();
  const std::string& aEndAttrName = anNewFeature->getKind() == SketchPlugin_Arc::ID() ?
      SketchPlugin_Arc::END_ID() : SketchPlugin_EllipticArc::END_POINT_ID();

  thePoints.insert(std::dynamic_pointer_cast<GeomDataAPI_Point2D>
                             (anNewFeature->attribute(aStartAttrName)));
  thePoints.insert(std::dynamic_pointer_cast<GeomDataAPI_Point2D>
                             (anNewFeature->attribute(aEndAttrName)));

  return anNewFeature;
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

  const std::string& aCenterAttrName = theArc->getKind() == SketchPlugin_Arc::ID() ?
      SketchPlugin_Arc::CENTER_ID() : SketchPlugin_EllipticArc::CENTER_ID();
  const std::string& aReversedAttrName = theArc->getKind() == SketchPlugin_Arc::ID() ?
      SketchPlugin_Arc::REVERSED_ID() : SketchPlugin_EllipticArc::REVERSED_ID();

  std::shared_ptr<GeomAPI_Pnt2d> aCenter = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
      theArc->attribute(aCenterAttrName))->pnt();
  bool isReversed = theArc->boolean(aReversedAttrName)->value();

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
}
