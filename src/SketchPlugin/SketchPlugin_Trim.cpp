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

#include <ModelAPI_AttributeReference.h>
#include <ModelAPI_AttributeString.h>
#include <ModelAPI_AttributeRefAttr.h>
#include <ModelAPI_Tools.h>
#include <ModelAPI_AttributeBoolean.h>

#include <ModelAPI_Validator.h>
#include <ModelAPI_Session.h>
#include <ModelAPI_AttributeDouble.h>

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

SketchPlugin_Trim::SketchPlugin_Trim()
{
}

void SketchPlugin_Trim::initAttributes()
{
  data()->addAttribute(SketchPlugin_Trim::BASE_OBJECT(), ModelAPI_AttributeReference::typeId());
  data()->addAttribute(ENTITY_POINT(), GeomDataAPI_Point2D::typeId());
}

void SketchPlugin_Trim::findShapePoints(std::shared_ptr<GeomAPI_Pnt>& aStartPoint,
                                        std::shared_ptr<GeomAPI_Pnt>& aLastPoint)
{
  AttributeReferencePtr aBaseObjectAttr = std::dynamic_pointer_cast<ModelAPI_AttributeReference>(
                                            data()->attribute(SketchPlugin_Trim::BASE_OBJECT()));
  ObjectPtr aBaseObject = aBaseObjectAttr->value();

  AttributePoint2DPtr aPoint = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
                                                            data()->attribute(ENTITY_POINT()));
  std::shared_ptr<GeomAPI_Pnt2d> anAttributePnt2d = aPoint->pnt();
  std::shared_ptr<GeomAPI_Pnt> anAttributePnt = sketch()->to3D(anAttributePnt2d->x(),
                                                               anAttributePnt2d->y());

  if (myCashedShapes.find(aBaseObject) == myCashedShapes.end())
    fillObjectShapes(aBaseObject);

  const std::set<GeomShapePtr>& aShapes = myCashedShapes[aBaseObject];
  if (!aShapes.empty()) {
    std::set<GeomShapePtr>::const_iterator anIt = aShapes.begin(), aLast = aShapes.end();
    for (; anIt != aLast; anIt++) {
      GeomShapePtr aBaseShape = *anIt;
      std::shared_ptr<GeomAPI_Pnt> aProjectedPoint;
      if (ModelGeomAlgo_Point2D::isPointOnEdge(aBaseShape, anAttributePnt, aProjectedPoint)) {

        if (aBaseShape->shapeType() == GeomAPI_Shape::EDGE) {
          std::shared_ptr<GeomAPI_Edge> anEdge(new GeomAPI_Edge(aBaseShape));
          aStartPoint = anEdge->lastPoint();
          aLastPoint = anEdge->firstPoint();
        }
      }
    }
  }
#ifdef DEBUG_TRIM
  std::cout << "<findShapePoints> => "
            << "Start Point: ["
            << aStartPoint->x() << ", " << aStartPoint->y() << ", " << aStartPoint->z() << "]"
            << "Last Point: ["
            << aLastPoint->x() << ", " << aLastPoint->y() << ", " << aLastPoint->z() << "]"
            << std::endl;
#endif
}

std::shared_ptr<GeomAPI_Pnt2d> SketchPlugin_Trim::convertPoint(
                                                   const std::shared_ptr<GeomAPI_Pnt>& thePoint)
{
  std::shared_ptr<GeomAPI_Pnt2d> aPoint;

  AttributeReferencePtr aBaseObjectAttr = std::dynamic_pointer_cast<ModelAPI_AttributeReference>(
                                            data()->attribute(SketchPlugin_Trim::BASE_OBJECT()));
  ObjectPtr aBaseObject = aBaseObjectAttr->value();
  if (myObjectToPoints.find(aBaseObject) == myObjectToPoints.end())
    return aPoint;

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
        std::shared_ptr<GeomAPI_Pln> aPlane = sketch()->plane();
        aPoint = thePoint->to2D(aPlane);
        aFound = true;
      }
    }
  }
  if (!aFound) {
    // returns an end of the shape to define direction of split if feature's attribute participates
    std::shared_ptr<GeomAPI_Pln> aPlane = sketch()->plane();
    aPoint = thePoint->to2D(aPlane);
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
                                            data()->attribute(SketchPlugin_Trim::BASE_OBJECT()));
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
  findShapePoints(aStartShapePoint, aLastShapePoint);
  std::shared_ptr<GeomAPI_Pnt2d> aStartShapePoint2d = convertPoint(aStartShapePoint);

  std::shared_ptr<GeomAPI_Pnt2d> aLastShapePoint2d = convertPoint(aLastShapePoint);

  std::set<FeaturePtr> aFeaturesToDelete;
  getConstraints(aFeaturesToDelete);

  std::map<AttributePtr, std::list<AttributePtr> > aBaseRefAttributes;
  std::list<AttributePtr> aRefsToFeature;
  getRefAttributes(aBaseFeature, aBaseRefAttributes, aRefsToFeature);


  // coincidence to result points
  // find coincidences to the base object, it should be used when attribute is found
  // in myObjectToPoints
  std::map<AttributePtr, FeaturePtr> aCoincidencesToBaseFeature;
  getCoincidencesToObject(aBaseObject, aCoincidencesToBaseFeature);

  std::set<AttributePoint2DPtr> aFurtherCoincidences;
  std::set<std::pair<AttributePtr, AttributePtr>> aModifiedAttributes;
  const std::string& aKind = aBaseFeature->getKind();
  FeaturePtr aReplacingFeature;
  if (aKind == SketchPlugin_Circle::ID()) {
    aReplacingFeature = trimCircle(aStartShapePoint2d, aLastShapePoint2d,
               aFurtherCoincidences, aModifiedAttributes);

    aFeaturesToDelete.insert(aBaseFeature);
    // as circle is removed, temporary fill this attribute
    aBaseObjectAttr->setObject(ResultPtr());
  }
  else if (aKind == SketchPlugin_Line::ID()) {
    trimLine(aStartShapePoint2d, aLastShapePoint2d,
             aFurtherCoincidences, aModifiedAttributes);
  }
  else if (aKind == SketchPlugin_Arc::ID()) {
    trimArc(aStartShapePoint2d, aLastShapePoint2d,
            aFurtherCoincidences, aModifiedAttributes);
  }

  //
  const PointToRefsMap& aRefsMap = myObjectToPoints.at(aBaseObject);
  std::set<AttributePoint2DPtr>::const_iterator anIt = aFurtherCoincidences.begin(),
                                                aLast = aFurtherCoincidences.end();
  for (; anIt != aLast; anIt++) {
    AttributePoint2DPtr aPointAttribute = (*anIt);
    std::shared_ptr<GeomAPI_Pnt2d> aPoint2d = aPointAttribute->pnt();

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
    const std::list<AttributePoint2DPtr >& anAttributes = anInfo.first;
    for (std::list<AttributePoint2DPtr>::const_iterator anAttrIt = anAttributes.begin();
          anAttrIt != anAttributes.end(); anAttrIt++) {
      AttributePtr anAttribute = *anAttrIt;
      if (aCoincidencesToBaseFeature.find(anAttribute) != aCoincidencesToBaseFeature.end())
      {
        FeaturePtr anAttrFeature = aCoincidencesToBaseFeature.at(anAttribute);
        AttributePtr anOtherAttribute;
        if (std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>
           (anAttrFeature->attribute(SketchPlugin_Constraint::ENTITY_A()))->attr() == anAttribute)
          anOtherAttribute = anAttrFeature->attribute(SketchPlugin_Constraint::ENTITY_B());
        else if (std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>
          (anAttrFeature->attribute(SketchPlugin_Constraint::ENTITY_B()))->attr() == anAttribute)
          anOtherAttribute = anAttrFeature->attribute(SketchPlugin_Constraint::ENTITY_A());
        else
          continue;
        AttributeRefAttrPtr aRefAttr = std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>
                                       (anOtherAttribute);
        if (aRefAttr.get())
          aRefAttr->setAttr(aPointAttribute);
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
  if (aReplacingFeature.get()) {
    ResultPtr aReplacingResult = getFeatureResult(aReplacingFeature);
    std::list<AttributePtr>::const_iterator anIt = aRefsToFeature.begin(),
                                            aLast = aRefsToFeature.end();
    for (; anIt != aLast; anIt++) {
      AttributeRefAttrPtr aRefAttr = std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(*anIt);
      if (!aRefAttr.get())
        continue;
      FeaturePtr anAttrFeature = ModelAPI_Feature::feature(aRefAttr->owner());
      if (anAttrFeature.get() &&
          anAttrFeature->getKind() == SketchPlugin_ConstraintCoincidence::ID())
      {
        if (anAttrFeature->attribute(SketchPlugin_Constraint::ENTITY_A()) == aRefAttr ||
            anAttrFeature->attribute(SketchPlugin_Constraint::ENTITY_B()) == aRefAttr)
          continue;
      }
      aRefAttr->setObject(aReplacingResult);
    }
  }

  // Wait all constraints being created, then send update events
  static Events_ID anUpdateEvent = Events_Loop::eventByName(EVENT_OBJECT_UPDATED);
  bool isUpdateFlushed = Events_Loop::loop()->isFlushed(anUpdateEvent);
  if (isUpdateFlushed)
    Events_Loop::loop()->setFlushed(anUpdateEvent, false);

  updateRefAttConstraints(aBaseRefAttributes, aModifiedAttributes, aFeaturesToDelete);

  // delete constraints
#ifdef DEBUG_TRIM
  std::cout << "remove features and references:" << std::endl;
  std::set<FeaturePtr>::const_iterator aDIt = aFeaturesToDelete.begin(),
                                       aDLast = aFeaturesToDelete.end();
  for (; aDIt != aDLast; aDIt++) {
    //std::cout << getFeatureInfo(*aDIt, false) << std::endl;
    //std::cout << std::endl;
  }
#endif
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

bool SketchPlugin_Trim::isMacro() const
{
  return true;
}

AISObjectPtr SketchPlugin_Trim::getAISObject(AISObjectPtr thePrevious)
{
  AISObjectPtr anAIS = thePrevious;
  // feature for trim
  AttributeReferencePtr aBaseObjectAttr = std::dynamic_pointer_cast<ModelAPI_AttributeReference>(
                                           data()->attribute(SketchPlugin_Trim::BASE_OBJECT()));
  ObjectPtr aBaseObject = aBaseObjectAttr->value();
  if (!aBaseObject.get())
    return anAIS;
  FeaturePtr aBaseFeature = ModelAPI_Feature::feature(aBaseObjectAttr->value());

  // point on feature
  AttributePoint2DPtr aPoint = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
                                           data()->attribute(ENTITY_POINT()));
  std::shared_ptr<GeomAPI_Pnt2d> anAttributePnt2d = aPoint->pnt();
  std::shared_ptr<GeomAPI_Pnt> anAttributePnt = sketch()->to3D(anAttributePnt2d->x(),
                                                               anAttributePnt2d->y());

  if (myCashedShapes.find(aBaseObject) == myCashedShapes.end())
    fillObjectShapes(aBaseObject);

  const std::set<GeomShapePtr>& aShapes = myCashedShapes[aBaseObject];
  if (!aShapes.empty()) {
    std::set<GeomShapePtr>::const_iterator anIt = aShapes.begin(), aLast = aShapes.end();
    for (; anIt != aLast; anIt++) {
      GeomShapePtr aBaseShape = *anIt;
      std::shared_ptr<GeomAPI_Pnt> aProjectedPoint;
      if (ModelGeomAlgo_Point2D::isPointOnEdge(aBaseShape, anAttributePnt, aProjectedPoint)) {
        if (aBaseShape) {
          if (!anAIS)
            anAIS = AISObjectPtr(new GeomAPI_AISObject);
          anAIS->createShape(aBaseShape);

          std::shared_ptr<ModelAPI_AttributeBoolean> anAuxiliaryAttr =
                 aBaseFeature->data()->boolean(SketchPlugin_SketchEntity::AUXILIARY_ID());

          bool isConstruction = anAuxiliaryAttr.get() != NULL && anAuxiliaryAttr->value();

          std::vector<int> aColor;
          aColor = Config_PropManager::color("Visualization", "operation_highlight_color",
                                             OPERATION_HIGHLIGHT_COLOR());
          double aWidth = SketchPlugin_SketchEntity::SKETCH_LINE_WIDTH();
          int aLineStyle = SketchPlugin_SketchEntity::SKETCH_LINE_STYLE();
          if (isConstruction) {
            aWidth = SketchPlugin_SketchEntity::SKETCH_LINE_WIDTH_AUXILIARY();
            aLineStyle = SketchPlugin_SketchEntity::SKETCH_LINE_STYLE_AUXILIARY();
          }
          anAIS->setColor(aColor[0], aColor[1], aColor[2]);
           // modification of width should be replaced to value 1 after highlight problem is fixed
          anAIS->setWidth(aWidth + 2);//1);
          anAIS->setLineStyle(aLineStyle);
          break;
        }
      }
    }
  }

  return anAIS;
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
                                            aData->attribute(SketchPlugin_Trim::BASE_OBJECT()));
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

void SketchPlugin_Trim::getCoincidencesToObject(const ObjectPtr& theObject,
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
      aRefAttr = std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>
                                    (aRefFeature->attribute(SketchPlugin_Constraint::ENTITY_B()));
      if (aRefAttr->isObject() && aRefAttr->object() == theObject)
      {
        anAttribute = aRefFeature->attribute(SketchPlugin_Constraint::ENTITY_A());
      }
      if (anAttribute.get())
      {
        aRefAttr = std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>
                                    (anAttribute);
        anAttribute = aRefAttr->attr();
        if (anAttribute.get())
          theCoincidencesToBaseFeature[anAttribute] = aRefFeature;
      }
    }
  }
}

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
    for (; aRefIt != aRLast; aRefIt++) {
      AttributeRefAttrPtr aRefAttr = std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(*aRefIt);
      if (aRefAttr.get()) {
        if (aNewAttribute.get())
          aRefAttr->setAttr(aNewAttribute);
        else
          theFeaturesToDelete.insert(ModelAPI_Feature::feature(aRefAttr->owner()));
#ifdef DEBUG_TRIM
        //FeaturePtr aFeature = ModelAPI_Feature::feature(aRefAttr->owner());
        //std::cout << " -" << getFeatureInfo(aFeature) << std::endl;
#endif
      }
    }
  }
}

void SketchPlugin_Trim::trimLine(const std::shared_ptr<GeomAPI_Pnt2d>& theStartShapePoint,
                                 const std::shared_ptr<GeomAPI_Pnt2d>& theLastShapePoint,
                                 std::set<AttributePoint2DPtr>& thePoints,
                  std::set<std::pair<AttributePtr, AttributePtr>>& theModifiedAttributes)
{
  // Check the base objects are initialized.
  AttributeReferencePtr aBaseObjectAttr = std::dynamic_pointer_cast<ModelAPI_AttributeReference>(
                                            data()->attribute(SketchPlugin_Trim::BASE_OBJECT()));
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

    fillPointAttribute(aBaseFeature->attribute(aModifiedAttribute), aPoint);
    theModifiedAttributes.insert(
      std::make_pair(aBaseFeature->attribute(aModifiedAttribute), AttributePtr()));

    thePoints.insert(std::dynamic_pointer_cast<GeomDataAPI_Point2D>
                               (aBaseFeature->attribute(aModifiedAttribute)));
  }
  else {
    // result is two lines: start line point - start shape point, last shape point - last line point
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
                                std::set<AttributePoint2DPtr>& thePoints,
                 std::set<std::pair<AttributePtr, AttributePtr>>& theModifiedAttributes)
{
  // Check the base objects are initialized.
  AttributeReferencePtr aBaseObjectAttr = std::dynamic_pointer_cast<ModelAPI_AttributeReference>(
                                            data()->attribute(SketchPlugin_Trim::BASE_OBJECT()));
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
    std::cout << "Start point: [" << aStartShapePoint->x() << ", " <<
                                       aStartShapePoint->y() << "]" << std::endl;
  std::cout << "1st point:   [" << aStartArcPoint->x() << ", " <<
                                   aStartArcPoint->y() << "]" << std::endl;
  if (aLastShapePoint.get())
    std::cout << "2st point:   [" << aLastShapePoint->x() << ", " <<
                                     aLastShapePoint->y() << "]" << std::endl;
  std::cout << "End point:   [" << aLastArcPoint->x() << ", " <<
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

    fillPointAttribute(aBaseFeature->attribute(aModifiedAttribute), aPoint);
    theModifiedAttributes.insert(
      std::make_pair(aBaseFeature->attribute(aModifiedAttribute), AttributePtr()));

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
    createConstraintForObjects(SketchPlugin_ConstraintEqual::ID(),
                               getFeatureResult(aBaseFeature),
                               getFeatureResult(anArcFeature));
    // coincident centers constraint
    createConstraint(SketchPlugin_ConstraintCoincidence::ID(),
                     aBaseFeature->attribute(SketchPlugin_Arc::CENTER_ID()),
                     anArcFeature->attribute(SketchPlugin_Arc::CENTER_ID()));
  }
}

FeaturePtr SketchPlugin_Trim::trimCircle(const std::shared_ptr<GeomAPI_Pnt2d>& theStartShapePoint,
                                   const std::shared_ptr<GeomAPI_Pnt2d>& theLastShapePoint,
                                   std::set<AttributePoint2DPtr>& thePoints,
                 std::set<std::pair<AttributePtr, AttributePtr>>& theModifiedAttributes)
{
  // Check the base objects are initialized.
  AttributeReferencePtr aBaseObjectAttr = std::dynamic_pointer_cast<ModelAPI_AttributeReference>(
                                            data()->attribute(SketchPlugin_Trim::BASE_OBJECT()));
  ObjectPtr aBaseObject = aBaseObjectAttr->value();
  FeaturePtr aBaseFeature = ModelAPI_Feature::feature(aBaseObjectAttr->value());

  /// points of trim
  //AttributePoint2DPtr aStartPointAttrOfBase, anEndPointAttrOfBase;
  //getFeaturePoints(aBaseFeature, aStartPointAttrOfBase, anEndPointAttrOfBase);

  /// trim feature
  FeaturePtr anArcFeature = createArcFeature(aBaseFeature, theStartShapePoint, theLastShapePoint);

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
  bool isReversed = theArc->boolean(SketchPlugin_Arc::INVERSED_ID())->value();

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
    std::cout << "<fillPointAttribute> => Pnt2d - [" << thePoint->x() << ", "
              << thePoint->y() << "]" << std::endl;
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

  aFeature = aSketch->addFeature(SketchPlugin_Arc::ID());
  // update fillet arc: make the arc correct for sure, so, it is not needed to process
  // the "attribute updated"
  // by arc; moreover, it may cause cyclicity in hte mechanism of updater
  bool aWasBlocked = aFeature->data()->blockSendAttributeUpdated(true);

  aFeature->string(SketchPlugin_Arc::ARC_TYPE())->setValue(
                SketchPlugin_Arc::ARC_TYPE_CENTER_START_END());

  fillAttribute(aFeature->attribute(SketchPlugin_Arc::CENTER_ID()),
                theBaseFeature->attribute(aCenterAttributeId));
  fillPointAttribute(aFeature->attribute(SketchPlugin_Arc::START_ID()), theFirstPoint);
  fillPointAttribute(aFeature->attribute(SketchPlugin_Arc::END_ID()), theSecondPoint);

  fillAttribute(aFeature->attribute(SketchPlugin_SketchEntity::AUXILIARY_ID()),
                theBaseFeature->attribute(SketchPlugin_SketchEntity::AUXILIARY_ID()));

  /// fill referersed state of created arc as it is on the base arc
  if (theBaseFeature->getKind() == SketchPlugin_Arc::ID()) {
    bool aReversed = theBaseFeature->boolean(SketchPlugin_Arc::INVERSED_ID())->value();
    aFeature->boolean(SketchPlugin_Arc::INVERSED_ID())->setValue(aReversed);
  }
  aFeature->execute(); // to obtain result
  aFeature->data()->blockSendAttributeUpdated(aWasBlocked);

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
bool SketchPlugin_Trim::useGraphicIntersection() const
{
  return true;
}

//********************************************************************
void SketchPlugin_Trim::fillObjectShapes(const ObjectPtr& theObject)
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
  ModelAPI_Tools::shapesOfType(aFeature, GeomAPI_Shape::EDGE, anEdgeShapes);
  if (!anEdgeShapes.empty()) {
    GeomShapePtr aFeatureShape = (*anEdgeShapes.begin())->shape();

    // coincidences to the feature
    ModelGeomAlgo_Point2D::getPointsOfReference(aFeature, SketchPlugin_ConstraintCoincidence::ID(),
                         aRefAttributes, SketchPlugin_Point::ID(), SketchPlugin_Point::COORD_ID());
    // layed on feature coincidences to divide it on several shapes
    SketchPlugin_Sketch* aSketch = sketch();
    std::shared_ptr<ModelAPI_Data> aData = aSketch->data();
    std::shared_ptr<GeomDataAPI_Point> aC = std::dynamic_pointer_cast<GeomDataAPI_Point>(
        aData->attribute(SketchPlugin_Sketch::ORIGIN_ID()));
    std::shared_ptr<GeomDataAPI_Dir> aX = std::dynamic_pointer_cast<GeomDataAPI_Dir>(
        aData->attribute(SketchPlugin_Sketch::DIRX_ID()));
    std::shared_ptr<GeomDataAPI_Dir> aNorm = std::dynamic_pointer_cast<GeomDataAPI_Dir>(
        aData->attribute(SketchPlugin_Sketch::NORM_ID()));
    std::shared_ptr<GeomAPI_Dir> aY(new GeomAPI_Dir(aNorm->dir()->cross(aX->dir())));

    ModelGeomAlgo_Point2D::getPointsInsideShape(aFeatureShape, aRefAttributes, aC->pnt(),
                                                aX->dir(), aY, aPointsInfo);

    // intersection points
    if (useGraphicIntersection()) {
      std::list<FeaturePtr> aFeatures;
      for (int i = 0; i < aSketch->numberOfSubs(); i++) {
        FeaturePtr aFeature = aSketch->subFeature(i);
        if (aFeature.get())
          aFeatures.push_back(aFeature);
      }
      ModelGeomAlgo_Point2D::getPointsIntersectedShape(aFeature, aFeatures, aPointsInfo);
    }
    GeomAlgoAPI_ShapeTools::splitShape(aFeatureShape, aPointsInfo, aShapes);
  }
  myObjectToPoints[theObject] = aPointsInfo;
  myCashedShapes[theObject] = aShapes;
}

//********************************************************************
void SketchPlugin_Trim::attributeChanged(const std::string& theID)
{
  //data()->addAttribute(SketchPlugin_Trim::BASE_OBJECT(), ModelAPI_AttributeReference::typeId());
  if (theID == SketchPlugin_Trim::BASE_OBJECT()) {
    bool isValidAttribute = false;
    // feature for trim
    AttributeReferencePtr aBaseObjectAttr = std::dynamic_pointer_cast<ModelAPI_AttributeReference>(
                                             data()->attribute(SketchPlugin_Trim::BASE_OBJECT()));
    ObjectPtr aBaseObject = aBaseObjectAttr->value();
    if (aBaseObject.get()) {
      FeaturePtr aBaseFeature = ModelAPI_Feature::feature(aBaseObjectAttr->value());
      // point on feature
      AttributePoint2DPtr aPoint = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
                                               data()->attribute(ENTITY_POINT()));
      std::shared_ptr<GeomAPI_Pnt2d> anAttributePnt2d = aPoint->pnt();
      std::shared_ptr<GeomAPI_Pnt> anAttributePnt = sketch()->to3D(anAttributePnt2d->x(),
                                                                   anAttributePnt2d->y());

      if (myCashedShapes.find(aBaseObject) == myCashedShapes.end())
        fillObjectShapes(aBaseObject);

      const std::set<GeomShapePtr>& aShapes = myCashedShapes[aBaseObject];
      isValidAttribute = !aShapes.empty();

      if (!isValidAttribute) {
        bool aWasBlocked = data()->blockSendAttributeUpdated(true);
        aBaseObjectAttr->setValue(ObjectPtr());
        data()->blockSendAttributeUpdated(aWasBlocked);
      }
    }
  }
}
