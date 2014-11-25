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
    std::shared_ptr<ModelAPI_AttributeDouble> aValueAttr = std::dynamic_pointer_cast<
        ModelAPI_AttributeDouble>(data()->attribute(SketchPlugin_Constraint::VALUE()));
    if(!aValueAttr->isInitialized()) {
      aValueAttr->setValue(aRadius);
    }
  }
}

AISObjectPtr SketchPlugin_ConstraintRadius::getAISObject(AISObjectPtr thePrevious)
{
  if (!sketch())
    return thePrevious;

  std::shared_ptr<ModelAPI_Data> aData = data();
  std::shared_ptr<ModelAPI_AttributeRefAttr> anAttr = std::dynamic_pointer_cast<
      ModelAPI_AttributeRefAttr>(aData->attribute(SketchPlugin_Constraint::ENTITY_A()));
  if (!anAttr)
    return thePrevious;
  FeaturePtr aFeature = ModelAPI_Feature::feature(anAttr->object());
  std::string aKind = aFeature ? aFeature->getKind() : "";
  if (aKind != SketchPlugin_Circle::ID() && aKind != SketchPlugin_Arc::ID())
    return thePrevious;

  // Flyout point
  std::shared_ptr<GeomDataAPI_Point2D> aFlyoutAttr = std::dynamic_pointer_cast<
      GeomDataAPI_Point2D>(aData->attribute(SketchPlugin_Constraint::FLYOUT_VALUE_PNT()));
  std::shared_ptr<GeomAPI_Pnt> aFlyoutPnt;
  if (aFlyoutAttr->isInitialized()) {
    aFlyoutPnt = sketch()->to3D(aFlyoutAttr->x(), aFlyoutAttr->y());
  } 

  // Prepare a circle
  aData = aFeature->data();
  std::shared_ptr<GeomDataAPI_Point2D> aCenterAttr;
  double aRadius;
  if (aKind == SketchPlugin_Circle::ID()) {
    aCenterAttr = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
        aData->attribute(SketchPlugin_Circle::CENTER_ID()));
    AttributeDoublePtr aCircRadius = std::dynamic_pointer_cast<ModelAPI_AttributeDouble>(
        aData->attribute(SketchPlugin_Circle::RADIUS_ID()));
    aRadius = aCircRadius->value();
    if (!aFlyoutPnt) {
      double aShift = aRadius * 1.1;
      std::shared_ptr<GeomAPI_Pnt2d> aPnt = aCenterAttr->pnt();
      std::shared_ptr<GeomAPI_Pnt2d> aFPnt = 
        std::shared_ptr<GeomAPI_Pnt2d>(new GeomAPI_Pnt2d(aPnt->x() + aShift, aPnt->y() + aShift));
      aFlyoutAttr->setValue(aFPnt);
      aFlyoutPnt = sketch()->to3D(aFPnt->x(), aFPnt->y());
    }
  } else if (aKind == SketchPlugin_Arc::ID()) {
    aCenterAttr = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
        aData->attribute(SketchPlugin_Arc::CENTER_ID()));
    std::shared_ptr<GeomDataAPI_Point2D> aStartAttr = std::dynamic_pointer_cast<
        GeomDataAPI_Point2D>(aData->attribute(SketchPlugin_Arc::START_ID()));
    aRadius = aCenterAttr->pnt()->distance(aStartAttr->pnt());
    if (!aFlyoutPnt) {
      std::shared_ptr<GeomDataAPI_Point2D> aStartAttr = std::dynamic_pointer_cast<
        GeomDataAPI_Point2D>(aData->attribute(SketchPlugin_Arc::START_ID()));      
      aFlyoutAttr->setValue(aStartAttr->pnt());
      aFlyoutPnt = sketch()->to3D(aStartAttr->pnt()->x(), aStartAttr->pnt()->y());
    }
  }

  std::shared_ptr<GeomAPI_Pnt> aCenter = sketch()->to3D(aCenterAttr->x(), aCenterAttr->y());
  std::shared_ptr<GeomDataAPI_Dir> aNDir = std::dynamic_pointer_cast<GeomDataAPI_Dir>(
      sketch()->data()->attribute(SketchPlugin_Sketch::NORM_ID()));
  std::shared_ptr<GeomAPI_Dir> aNormal = aNDir->dir();
  std::shared_ptr<GeomAPI_Circ> aCircle(new GeomAPI_Circ(aCenter, aNormal, aRadius));

  // Value
  std::shared_ptr<ModelAPI_AttributeDouble> aValueAttr = std::dynamic_pointer_cast<
      ModelAPI_AttributeDouble>(aData->attribute(SketchPlugin_Constraint::VALUE()));
  double aValue = aRadius;
  if (aValueAttr && aValueAttr->isInitialized())
    aValue = aValueAttr->value();

  AISObjectPtr anAIS = thePrevious;
  if (!anAIS)
    anAIS = AISObjectPtr(new GeomAPI_AISObject);
  anAIS->createRadius(aCircle, aFlyoutPnt, aValue);

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
