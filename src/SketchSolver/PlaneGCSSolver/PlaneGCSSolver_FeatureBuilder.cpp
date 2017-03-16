// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:    PlaneGCSSolver_AttributeBuilder.cpp
// Created: 10 Feb 2017
// Author:  Artem ZHIDKOV

#include <PlaneGCSSolver_FeatureBuilder.h>
#include <PlaneGCSSolver_EdgeWrapper.h>
#include <PlaneGCSSolver_PointWrapper.h>
#include <PlaneGCSSolver_ScalarWrapper.h>

#include <SketchPlugin_Arc.h>
#include <SketchPlugin_Circle.h>
#include <SketchPlugin_IntersectionPoint.h>
#include <SketchPlugin_Line.h>
#include <SketchPlugin_Point.h>

#include <GeomAPI_Dir2d.h>
#include <GeomAPI_Pnt2d.h>
#include <GeomAPI_XY.h>

static bool isAttributeApplicable(const std::string& theAttrName,
                                  const std::string& theOwnerName);

static EntityWrapperPtr createLine(const AttributeEntityMap& theAttributes);
static EntityWrapperPtr createCircle(const AttributeEntityMap& theAttributes);
static EntityWrapperPtr createArc(const AttributeEntityMap&    theAttributes,
                                  PlaneGCSSolver_Storage*      theStorage,
                                  std::list<GCSConstraintPtr>& theArcConstraints);


PlaneGCSSolver_FeatureBuilder::PlaneGCSSolver_FeatureBuilder(
    PlaneGCSSolver_Storage* theStorage)
  : PlaneGCSSolver_AttributeBuilder(theStorage)
{
}

PlaneGCSSolver_FeatureBuilder::PlaneGCSSolver_FeatureBuilder(const StoragePtr& theStorage)
  : PlaneGCSSolver_AttributeBuilder(theStorage)
{
}

EntityWrapperPtr PlaneGCSSolver_FeatureBuilder::createAttribute(
    AttributePtr theAttribute)
{
  FeaturePtr anOwner = ModelAPI_Feature::feature(theAttribute->owner());
  EntityWrapperPtr anAttr;
  if (isAttributeApplicable(theAttribute->id(), anOwner->getKind()))
    anAttr = PlaneGCSSolver_AttributeBuilder::createAttribute(theAttribute);
  if (anAttr)
    myAttributes[theAttribute] = anAttr;
  return anAttr;
}

EntityWrapperPtr PlaneGCSSolver_FeatureBuilder::createFeature(FeaturePtr theFeature)
{
  EntityWrapperPtr aResult;
  if (myStorage)
    aResult = myStorage->entity(theFeature);
  if (aResult)
    return aResult;

  // Process SketchPlugin features only
  std::shared_ptr<SketchPlugin_Feature> aFeature =
      std::dynamic_pointer_cast<SketchPlugin_Feature>(theFeature);
  if (!aFeature)
    return aResult;

  // Verify the feature by its kind
  const std::string& aFeatureKind = aFeature->getKind();
  // Line
  if (aFeatureKind == SketchPlugin_Line::ID())
    aResult = createLine(myAttributes);
  // Circle
  else if (aFeatureKind == SketchPlugin_Circle::ID())
    aResult = createCircle(myAttributes);
  // Arc
  else if (aFeatureKind == SketchPlugin_Arc::ID())
    aResult = createArc(myAttributes, myStorage, myFeatureConstraints);
  // Point (it has low probability to be an attribute of constraint, so it is checked at the end)
  else if (aFeatureKind == SketchPlugin_Point::ID() ||
           aFeatureKind == SketchPlugin_IntersectionPoint::ID()) {
    AttributeEntityMap::const_iterator anIt = myAttributes.begin();
    for (; anIt != myAttributes.end(); ++anIt)
      if (anIt->first->id() == SketchPlugin_Point::COORD_ID()) {
        aResult = anIt->second;
        break;
      }
  }

  if (aResult && !myStorage)
    aResult->setExternal(true);

  myAttributes.clear();
  return aResult;
}





// ==============   Auxiliary functions   =====================================
EntityWrapperPtr createLine(const AttributeEntityMap& theAttributes)
{
  std::shared_ptr<GCS::Line> aNewLine(new GCS::Line);

  AttributeEntityMap::const_iterator anIt = theAttributes.begin();
  for (; anIt != theAttributes.end(); ++anIt) {
    std::shared_ptr<PlaneGCSSolver_PointWrapper> aPoint =
        std::dynamic_pointer_cast<PlaneGCSSolver_PointWrapper>(anIt->second);
    if (!aPoint)
      continue;

    if (anIt->first->id() == SketchPlugin_Line::START_ID())
      aNewLine->p1 = *(aPoint->point());
    else if (anIt->first->id() == SketchPlugin_Line::END_ID())
      aNewLine->p2 = *(aPoint->point());
  }

  return EntityWrapperPtr(new PlaneGCSSolver_EdgeWrapper(aNewLine));
}

EntityWrapperPtr createCircle(const AttributeEntityMap& theAttributes)
{
  std::shared_ptr<GCS::Circle> aNewCircle(new GCS::Circle);

  AttributeEntityMap::const_iterator anIt = theAttributes.begin();
  for (; anIt != theAttributes.end(); ++anIt) {
    if (anIt->first->id() == SketchPlugin_Circle::CENTER_ID()) {
      std::shared_ptr<PlaneGCSSolver_PointWrapper> aPoint =
          std::dynamic_pointer_cast<PlaneGCSSolver_PointWrapper>(anIt->second);
      aNewCircle->center = *(aPoint->point());
    }
    else if(anIt->first->id() == SketchPlugin_Circle::RADIUS_ID()) {
      ScalarWrapperPtr aScalar =
          std::dynamic_pointer_cast<PlaneGCSSolver_ScalarWrapper>(anIt->second);
      aNewCircle->rad = aScalar->scalar();
    }
  }

  return EntityWrapperPtr(new PlaneGCSSolver_EdgeWrapper(aNewCircle));
}

static double* createParameter(PlaneGCSSolver_Storage* theStorage)
{
  return theStorage ? theStorage->createParameter() : (new double(0));
}

EntityWrapperPtr createArc(const AttributeEntityMap&    theAttributes,
                           PlaneGCSSolver_Storage*      theStorage,
                           std::list<GCSConstraintPtr>& theArcConstraints)
{
  std::shared_ptr<GCS::Arc> aNewArc(new GCS::Arc);

  // Base attributes of arc (center, start and end points)
  AttributeEntityMap::const_iterator anIt = theAttributes.begin();
  for (; anIt != theAttributes.end(); ++anIt) {
    std::shared_ptr<PlaneGCSSolver_PointWrapper> aPoint =
        std::dynamic_pointer_cast<PlaneGCSSolver_PointWrapper>(anIt->second);
    if (!aPoint)
      continue;

    if (anIt->first->id() == SketchPlugin_Arc::CENTER_ID())
      aNewArc->center = *(aPoint->point());
    else if (anIt->first->id() == SketchPlugin_Arc::START_ID())
      aNewArc->start = *(aPoint->point());
    else if (anIt->first->id() == SketchPlugin_Arc::END_ID())
      aNewArc->end = *(aPoint->point());
  }

  // Additional atrtributes of arc necessary for PlaneGCS solver
  // (start and end angles, radius)
  aNewArc->startAngle = createParameter(theStorage);
  aNewArc->endAngle   = createParameter(theStorage);
  aNewArc->rad        = createParameter(theStorage);

  static std::shared_ptr<GeomAPI_Dir2d> OX(new GeomAPI_Dir2d(1.0, 0.0));
  std::shared_ptr<GeomAPI_Pnt2d> aCenter(
      new GeomAPI_Pnt2d(*aNewArc->center.x, *aNewArc->center.y));
  std::shared_ptr<GeomAPI_Pnt2d> aStart(
      new GeomAPI_Pnt2d(*aNewArc->start.x, *aNewArc->start.y));

  *aNewArc->rad = aStart->distance(aCenter);

  std::shared_ptr<GeomAPI_Dir2d> aDir(new GeomAPI_Dir2d(aStart->xy()->decreased(aCenter->xy())));
  *aNewArc->startAngle = OX->angle(aDir);

  aDir = std::shared_ptr<GeomAPI_Dir2d>(
      new GeomAPI_Dir2d((*aNewArc->end.x) - aCenter->x(), (*aNewArc->end.y) - aCenter->y()));
  *aNewArc->endAngle = OX->angle(aDir);

  if (theStorage) {
    // Additional constaints to fix arc's extra DoF (if the arc is not external):
    // 1. distances from center till start and end points are equal to radius
    theArcConstraints.push_back(GCSConstraintPtr(new GCS::ConstraintP2PDistance(
        aNewArc->center, aNewArc->start, aNewArc->rad)));
    theArcConstraints.push_back(GCSConstraintPtr(new GCS::ConstraintP2PDistance(
        aNewArc->center, aNewArc->end, aNewArc->rad)));
    // 2. angles of start and end points should be equal to the arc angles
    theArcConstraints.push_back(GCSConstraintPtr(new GCS::ConstraintP2PAngle(
        aNewArc->center, aNewArc->start, aNewArc->startAngle)));
    theArcConstraints.push_back(GCSConstraintPtr(new GCS::ConstraintP2PAngle(
        aNewArc->center, aNewArc->end, aNewArc->endAngle)));
  }

  return EntityWrapperPtr(new PlaneGCSSolver_EdgeWrapper(aNewArc));
}

bool isAttributeApplicable(const std::string& theAttrName, const std::string& theOwnerName)
{
  if (theOwnerName == SketchPlugin_Arc::ID()) {
    return theAttrName == SketchPlugin_Arc::CENTER_ID() ||
           theAttrName == SketchPlugin_Arc::START_ID() ||
           theAttrName == SketchPlugin_Arc::END_ID();
  }
  else if (theOwnerName == SketchPlugin_Circle::ID()) {
    return theAttrName == SketchPlugin_Circle::CENTER_ID() ||
           theAttrName == SketchPlugin_Circle::RADIUS_ID();
  }
  else if (theOwnerName == SketchPlugin_Line::ID()) {
    return theAttrName == SketchPlugin_Line::START_ID() ||
           theAttrName == SketchPlugin_Line::END_ID();
  }

  // suppose that all remaining features are points
  return theAttrName == SketchPlugin_Point::COORD_ID();
}
