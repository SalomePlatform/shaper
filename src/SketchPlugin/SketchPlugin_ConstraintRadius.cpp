// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:    SketchPlugin_ConstraintRadius.cpp
// Created: 26 May 2014
// Author:  Artem ZHIDKOV

#include "SketchPlugin_ConstraintRadius.h"

#include <SketchPlugin_Arc.h>
#include <SketchPlugin_Circle.h>
#include <SketchPlugin_Point.h>

#include <SketcherPrs_Factory.h>

#include <ModelAPI_AttributeDouble.h>
#include <ModelAPI_Data.h>

#include <GeomAPI_Pnt2d.h>
#include <GeomAPI_Circ.h>
#include <GeomAPI_Circ2d.h>
#include <GeomAPI_Dir.h>
#include <GeomAPI_XY.h>
#include <GeomAPI_XYZ.h>
#include <GeomDataAPI_Point2D.h>
#include <GeomDataAPI_Dir.h>

#include <Config_PropManager.h>

const double tolerance = 1.e-7;

SketchPlugin_ConstraintRadius::SketchPlugin_ConstraintRadius()
{
  myFlyoutUpdate = false;
}

void SketchPlugin_ConstraintRadius::initAttributes()
{
  data()->addAttribute(SketchPlugin_Constraint::VALUE(), ModelAPI_AttributeDouble::typeId());
  data()->addAttribute(SketchPlugin_Constraint::ENTITY_A(), ModelAPI_AttributeRefAttr::typeId());
  data()->addAttribute(SketchPlugin_Constraint::FLYOUT_VALUE_PNT(), GeomDataAPI_Point2D::typeId());
}

void SketchPlugin_ConstraintRadius::colorConfigInfo(std::string& theSection, std::string& theName,
                                                    std::string& theDefault)
{
  theSection = "Visualization";
  theName = "sketch_dimension_color";
  theDefault = SKETCH_DIMENSION_COLOR;
}

void SketchPlugin_ConstraintRadius::execute()
{
  std::shared_ptr<ModelAPI_AttributeRefAttr> aRef = std::dynamic_pointer_cast<
      ModelAPI_AttributeRefAttr>(data()->attribute(SketchPlugin_Constraint::ENTITY_A()));
  FeaturePtr aFeature = ModelAPI_Feature::feature(aRef->object());
  if (aFeature) {
    double aRadius = 0;
    std::shared_ptr<ModelAPI_Data> aData = aFeature->data();
    if (aFeature->getKind() == SketchPlugin_Circle::ID()) {
      AttributeDoublePtr anAttribute = std::dynamic_pointer_cast<ModelAPI_AttributeDouble>(
          aData->attribute(SketchPlugin_Circle::RADIUS_ID()));
      if (anAttribute)
        aRadius = anAttribute->value();
    } else if (aFeature->getKind() == SketchPlugin_Arc::ID()) {
      std::shared_ptr<GeomDataAPI_Point2D> aCenterAttr = std::dynamic_pointer_cast<
          GeomDataAPI_Point2D>(aData->attribute(SketchPlugin_Arc::CENTER_ID()));
      std::shared_ptr<GeomDataAPI_Point2D> aStartAttr = std::dynamic_pointer_cast<
          GeomDataAPI_Point2D>(aData->attribute(SketchPlugin_Arc::START_ID()));
      if (aCenterAttr && aStartAttr)
        aRadius = aCenterAttr->pnt()->distance(aStartAttr->pnt());
    }
    //std::shared_ptr<ModelAPI_AttributeDouble> aValueAttr = std::dynamic_pointer_cast<
    //    ModelAPI_AttributeDouble>(data()->attribute(SketchPlugin_Constraint::VALUE()));
    //if(!aValueAttr->isInitialized()) {
    //  aValueAttr->setValue(aRadius);
    //}

    // the value should to be computed here, not in the getAISObject in order to change the model value
    // inside the object transaction. This is important for creating a constraint by preselection.
    // The display of the presentation in this case happens after the transaction commit
    std::shared_ptr<GeomDataAPI_Point2D> aFlyoutAttr = std::dynamic_pointer_cast<
        GeomDataAPI_Point2D>(data()->attribute(SketchPlugin_Constraint::FLYOUT_VALUE_PNT()));
    if (!aFlyoutAttr->isInitialized())
      compute(SketchPlugin_Constraint::FLYOUT_VALUE_PNT());
  }
}

bool SketchPlugin_ConstraintRadius::compute(const std::string& theAttributeId)
{
  if (theAttributeId != SketchPlugin_Constraint::FLYOUT_VALUE_PNT())
    return false;

  std::shared_ptr<ModelAPI_Feature> aCyrcFeature;
  double aRadius = circleRadius(aCyrcFeature);
  if (aRadius < 0)
    return false;

  // Flyout point
  std::shared_ptr<GeomDataAPI_Point2D> aFlyoutAttr = std::dynamic_pointer_cast<
      GeomDataAPI_Point2D>(data()->attribute(theAttributeId));
  // Prepare a circle
  if (aCyrcFeature->getKind() == SketchPlugin_Circle::ID()) { // circle
    std::shared_ptr<GeomDataAPI_Point2D> aCenterAttr = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
        aCyrcFeature->data()->attribute(SketchPlugin_Circle::CENTER_ID()));
    double aShift = aRadius * 1.1;
    std::shared_ptr<GeomAPI_Pnt2d> aPnt = aCenterAttr->pnt();
    std::shared_ptr<GeomAPI_Pnt2d> aFPnt = 
      std::shared_ptr<GeomAPI_Pnt2d>(new GeomAPI_Pnt2d(aPnt->x() + aShift, aPnt->y() + aShift));
    aFlyoutAttr->setValue(aFPnt);
  } else { // arc
    std::shared_ptr<GeomDataAPI_Point2D> aStartAttr = std::dynamic_pointer_cast<
      GeomDataAPI_Point2D>(aCyrcFeature->data()->attribute(SketchPlugin_Arc::START_ID()));      
    aFlyoutAttr->setValue(aStartAttr->pnt());
  }
  return true;
}

double SketchPlugin_ConstraintRadius::circleRadius(std::shared_ptr<ModelAPI_Feature>& theCirc)
{
  static const double kErrorResult = -1.;
  if (!sketch())
    return kErrorResult;

  std::shared_ptr<ModelAPI_Data> aData = data();
  std::shared_ptr<ModelAPI_AttributeRefAttr> anAttr = std::dynamic_pointer_cast<
      ModelAPI_AttributeRefAttr>(aData->attribute(SketchPlugin_Constraint::ENTITY_A()));
  if (!anAttr)
    return kErrorResult;
  theCirc = ModelAPI_Feature::feature(anAttr->object());
  std::string aKind = theCirc ? theCirc->getKind() : "";
  if (aKind != SketchPlugin_Circle::ID() && aKind != SketchPlugin_Arc::ID())
    return kErrorResult;

  DataPtr aCircData = theCirc->data();
  std::shared_ptr<GeomDataAPI_Point2D> aCenterAttr;
  if (aKind == SketchPlugin_Circle::ID()) {
    AttributeDoublePtr aCircRadius = std::dynamic_pointer_cast<ModelAPI_AttributeDouble>(
        aCircData->attribute(SketchPlugin_Circle::RADIUS_ID()));
    return aCircRadius->value();
  } else {
    aCenterAttr = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
        aCircData->attribute(SketchPlugin_Arc::CENTER_ID()));
    std::shared_ptr<GeomDataAPI_Point2D> aStartAttr = std::dynamic_pointer_cast<
        GeomDataAPI_Point2D>(aCircData->attribute(SketchPlugin_Arc::START_ID()));
    return aCenterAttr->pnt()->distance(aStartAttr->pnt());
  }
  return kErrorResult;
}

AISObjectPtr SketchPlugin_ConstraintRadius::getAISObject(AISObjectPtr thePrevious)
{
  if (!sketch())
    return thePrevious;

  AISObjectPtr anAIS = thePrevious;
  if (!anAIS) {
    anAIS = SketcherPrs_Factory::radiusConstraint(this, sketch()->coordinatePlane());
  }
  return anAIS;
}

void SketchPlugin_ConstraintRadius::move(double theDeltaX, double theDeltaY)
{
  std::shared_ptr<ModelAPI_Data> aData = data();
  if (!aData->isValid())
    return;

  myFlyoutUpdate = true;
  std::shared_ptr<GeomDataAPI_Point2D> aFlyoutAttr = std::dynamic_pointer_cast<
      GeomDataAPI_Point2D>(aData->attribute(SketchPlugin_Constraint::FLYOUT_VALUE_PNT()));
  aFlyoutAttr->setValue(aFlyoutAttr->x() + theDeltaX, aFlyoutAttr->y() + theDeltaY);
  myFlyoutUpdate = false;
}

void SketchPlugin_ConstraintRadius::attributeChanged(const std::string& theID) {
  if (theID == SketchPlugin_Constraint::ENTITY_A()) {
    std::shared_ptr<ModelAPI_AttributeDouble> aValueAttr = std::dynamic_pointer_cast<
        ModelAPI_AttributeDouble>(data()->attribute(SketchPlugin_Constraint::VALUE()));
    if (!aValueAttr->isInitialized()) { // only if it is not initialized, try to compute the current value
      std::shared_ptr<ModelAPI_Feature> aCyrcFeature;
      double aRadius = circleRadius(aCyrcFeature);
      if (aRadius > 0) { // set as value the radius of updated reference to another circle
        aValueAttr->setValue(aRadius);
      }
    }
  } else if (theID == SketchPlugin_Constraint::FLYOUT_VALUE_PNT() && !myFlyoutUpdate) {
    // Recalculate flyout point in local coordinates of the circle (or arc):
    // coordinates are calculated according to center of the shape
    std::shared_ptr<GeomDataAPI_Point2D> aFlyoutAttr =
        std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
        attribute(SketchPlugin_Constraint::FLYOUT_VALUE_PNT()));

    AttributeRefAttrPtr aRefAttr = std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(
        attribute(SketchPlugin_Constraint::ENTITY_A()));
    if (!aRefAttr || !aRefAttr->isObject())
      return;
    FeaturePtr aFeature = ModelAPI_Feature::feature(aRefAttr->object());
    if (!aFeature || (aFeature->getKind() != SketchPlugin_Arc::ID() &&
        aFeature->getKind() != SketchPlugin_Circle::ID()))
      return;

    std::string aCenterAttrName;
    if (aFeature->getKind() == SketchPlugin_Circle::ID())
      aCenterAttrName = SketchPlugin_Circle::CENTER_ID();
    else if (aFeature->getKind() == SketchPlugin_Arc::ID())
      aCenterAttrName = SketchPlugin_Arc::CENTER_ID();
    std::shared_ptr<GeomDataAPI_Point2D> aCenterAttr = std::dynamic_pointer_cast<
        GeomDataAPI_Point2D>(aFeature->data()->attribute(aCenterAttrName));
    std::shared_ptr<GeomAPI_Pnt2d> aCenter = aCenterAttr->pnt();
    std::shared_ptr<ModelAPI_AttributeDouble> aRadius = std::dynamic_pointer_cast<
        ModelAPI_AttributeDouble>(attribute(SketchPlugin_Constraint::VALUE()));

    std::shared_ptr<GeomAPI_Pnt2d> aFlyoutPnt = aFlyoutAttr->pnt();
    double aDist = aFlyoutPnt->distance(aCenter);
    if (aDist < tolerance)
      return;

    myFlyoutUpdate = true;
    std::shared_ptr<GeomAPI_XY> aFlyoutDir = aFlyoutPnt->xy()->decreased(aCenter->xy());
    aFlyoutAttr->setValue(aFlyoutDir->x(), aFlyoutDir->y());
    myFlyoutUpdate = false;
  }
}
