// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:    SketchPlugin_ConstraintRadius.cpp
// Created: 26 May 2014
// Author:  Artem ZHIDKOV

#include "SketchPlugin_ConstraintRadius.h"

#include <SketchPlugin_Arc.h>
#include <SketchPlugin_Circle.h>
#include <SketchPlugin_Point.h>

#include <ModelAPI_AttributeDouble.h>
#include <ModelAPI_Data.h>

#include <GeomAPI_Pnt2d.h>
#include <GeomAPI_Circ.h>
#include <GeomAPI_Circ2d.h>
#include <GeomAPI_Dir.h>
#include <GeomAPI_XYZ.h>
#include <GeomDataAPI_Point2D.h>
#include <GeomDataAPI_Dir.h>

#include <Config_PropManager.h>

SketchPlugin_ConstraintRadius::SketchPlugin_ConstraintRadius()
{
}

void SketchPlugin_ConstraintRadius::initAttributes()
{
  data()->addAttribute(SketchPlugin_Constraint::VALUE(), ModelAPI_AttributeDouble::type());
  data()->addAttribute(SketchPlugin_Constraint::ENTITY_A(), ModelAPI_AttributeRefAttr::type());
  data()->addAttribute(SketchPlugin_Constraint::FLYOUT_VALUE_PNT(), GeomDataAPI_Point2D::type());
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
  std::shared_ptr<ModelAPI_Feature> aCyrcFeature;
  double aRadius = circleRadius(aCyrcFeature);
  if (aRadius < 0)
    return thePrevious; // can not create a good presentation

  // Flyout point
  std::shared_ptr<GeomDataAPI_Point2D> aFlyoutAttr = std::dynamic_pointer_cast<
      GeomDataAPI_Point2D>(data()->attribute(SketchPlugin_Constraint::FLYOUT_VALUE_PNT()));
  if (!aFlyoutAttr->isInitialized())
    return thePrevious; // can not create a good presentation
  std::shared_ptr<GeomAPI_Pnt> aFlyoutPnt = sketch()->to3D(aFlyoutAttr->x(), aFlyoutAttr->y());

  // Prepare a circle
  std::shared_ptr<GeomDataAPI_Point2D> aCenterAttr;
  if (aCyrcFeature->getKind() == SketchPlugin_Circle::ID()) { // circle
    aCenterAttr = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
        aCyrcFeature->data()->attribute(SketchPlugin_Circle::CENTER_ID()));
  } else { // arc
    aCenterAttr = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
        aCyrcFeature->data()->attribute(SketchPlugin_Arc::CENTER_ID()));
  }
  std::shared_ptr<GeomAPI_Pnt> aCenter = sketch()->to3D(aCenterAttr->x(), aCenterAttr->y());
  std::shared_ptr<GeomDataAPI_Dir> aNDir = std::dynamic_pointer_cast<GeomDataAPI_Dir>(
      sketch()->data()->attribute(SketchPlugin_Sketch::NORM_ID()));
  std::shared_ptr<GeomAPI_Dir> aNormal = aNDir->dir();
  std::shared_ptr<GeomAPI_Circ> aCircle(new GeomAPI_Circ(aCenter, aNormal, aRadius));

  // Value
  std::shared_ptr<ModelAPI_AttributeDouble> aValueAttr = std::dynamic_pointer_cast<
      ModelAPI_AttributeDouble>(data()->attribute(SketchPlugin_Constraint::VALUE()));
  if (aValueAttr->isInitialized())
    aRadius = aValueAttr->value();
  AISObjectPtr anAIS = thePrevious;
  if (!anAIS)
    anAIS = AISObjectPtr(new GeomAPI_AISObject);
  anAIS->createRadius(aCircle, aFlyoutPnt, aRadius);

  // Set color from preferences
  std::vector<int> aRGB = Config_PropManager::color("Visualization", "radius_color", RADIUS_COLOR);
  anAIS->setColor(aRGB[0], aRGB[1], aRGB[2]);
  return anAIS;
}

void SketchPlugin_ConstraintRadius::move(double theDeltaX, double theDeltaY)
{
  std::shared_ptr<ModelAPI_Data> aData = data();
  if (!aData->isValid())
    return;

  std::shared_ptr<ModelAPI_AttributeRefAttr> aRef = std::dynamic_pointer_cast<
      ModelAPI_AttributeRefAttr>(data()->attribute(SketchPlugin_Constraint::ENTITY_A()));
  FeaturePtr aFeature = ModelAPI_Feature::feature(aRef->object());
  if (!aFeature)
    return;
  std::string aCenterAttrName;
  if (aFeature->getKind() == SketchPlugin_Circle::ID())
    aCenterAttrName = SketchPlugin_Circle::CENTER_ID();
  else if (aFeature->getKind() == SketchPlugin_Arc::ID())
    aCenterAttrName = SketchPlugin_Arc::CENTER_ID();
  std::shared_ptr<GeomDataAPI_Point2D> aCenterAttr = std::dynamic_pointer_cast<
      GeomDataAPI_Point2D>(aFeature->data()->attribute(aCenterAttrName));
  std::shared_ptr<GeomAPI_Pnt2d> aCenter = aCenterAttr->pnt();

  // The specified delta applied on the circle curve, 
  // so it will be scaled due to distance between flyout and center points
  std::shared_ptr<GeomDataAPI_Point2D> aFlyoutAttr = std::dynamic_pointer_cast<
      GeomDataAPI_Point2D>(aData->attribute(SketchPlugin_Constraint::FLYOUT_VALUE_PNT()));
  std::shared_ptr<GeomAPI_Pnt2d> aFlyout = aFlyoutAttr->pnt();

  std::shared_ptr<ModelAPI_AttributeDouble> aRadius = std::dynamic_pointer_cast<
      ModelAPI_AttributeDouble>(aData->attribute(SketchPlugin_Constraint::VALUE()));
  double aScale = aFlyout->distance(aCenter) / aRadius->value();

  std::shared_ptr<GeomAPI_Circ2d> aCircle(new GeomAPI_Circ2d(aCenter, aFlyout));
  aFlyout->setX(aFlyout->x() + aScale * theDeltaX);
  aFlyout->setY(aFlyout->y() + aScale * theDeltaY);
  aFlyout = aCircle->project(aFlyout);

  aFlyoutAttr->setValue(aFlyout->x(), aFlyout->y());
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
  }
}
