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
  boost::shared_ptr<ModelAPI_AttributeRefAttr> aRef = boost::dynamic_pointer_cast<
      ModelAPI_AttributeRefAttr>(data()->attribute(SketchPlugin_Constraint::ENTITY_A()));
  FeaturePtr aFeature = ModelAPI_Feature::feature(aRef->object());
  if (aFeature) {
    double aRadius = 0;
    boost::shared_ptr<ModelAPI_Data> aData = aFeature->data();
    if (aFeature->getKind() == SketchPlugin_Circle::ID()) {
      AttributeDoublePtr anAttribute = boost::dynamic_pointer_cast<ModelAPI_AttributeDouble>(
          aData->attribute(SketchPlugin_Circle::RADIUS_ID()));
      if (anAttribute)
        aRadius = anAttribute->value();
    } else if (aFeature->getKind() == SketchPlugin_Arc::ID()) {
      boost::shared_ptr<GeomDataAPI_Point2D> aCenterAttr = boost::dynamic_pointer_cast<
          GeomDataAPI_Point2D>(aData->attribute(SketchPlugin_Arc::CENTER_ID()));
      boost::shared_ptr<GeomDataAPI_Point2D> aStartAttr = boost::dynamic_pointer_cast<
          GeomDataAPI_Point2D>(aData->attribute(SketchPlugin_Arc::START_ID()));
      if (aCenterAttr && aStartAttr)
        aRadius = aCenterAttr->pnt()->distance(aStartAttr->pnt());
    }
    boost::shared_ptr<ModelAPI_AttributeDouble> aValueAttr = boost::dynamic_pointer_cast<
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

  boost::shared_ptr<ModelAPI_Data> aData = data();
  boost::shared_ptr<ModelAPI_AttributeRefAttr> anAttr = boost::dynamic_pointer_cast<
      ModelAPI_AttributeRefAttr>(aData->attribute(SketchPlugin_Constraint::ENTITY_A()));
  if (!anAttr)
    return thePrevious;
  FeaturePtr aFeature = ModelAPI_Feature::feature(anAttr->object());
  std::string aKind = aFeature ? aFeature->getKind() : "";
  if (aKind != SketchPlugin_Circle::ID() && aKind != SketchPlugin_Arc::ID())
    return thePrevious;

  // Flyout point
  boost::shared_ptr<GeomDataAPI_Point2D> aFlyoutAttr = boost::dynamic_pointer_cast<
      GeomDataAPI_Point2D>(aData->attribute(SketchPlugin_Constraint::FLYOUT_VALUE_PNT()));
  boost::shared_ptr<GeomAPI_Pnt> aFlyoutPnt;
  if (aFlyoutAttr->isInitialized()) {
    aFlyoutPnt = sketch()->to3D(aFlyoutAttr->x(), aFlyoutAttr->y());
  } 

  // Prepare a circle
  aData = aFeature->data();
  boost::shared_ptr<GeomDataAPI_Point2D> aCenterAttr;
  double aRadius;
  if (aKind == SketchPlugin_Circle::ID()) {
    aCenterAttr = boost::dynamic_pointer_cast<GeomDataAPI_Point2D>(
        aData->attribute(SketchPlugin_Circle::CENTER_ID()));
    AttributeDoublePtr aCircRadius = boost::dynamic_pointer_cast<ModelAPI_AttributeDouble>(
        aData->attribute(SketchPlugin_Circle::RADIUS_ID()));
    aRadius = aCircRadius->value();
    if (!aFlyoutPnt) {
      double aShift = aRadius * 1.1;
      boost::shared_ptr<GeomAPI_Pnt2d> aPnt = aCenterAttr->pnt();
      boost::shared_ptr<GeomAPI_Pnt2d> aFPnt = 
        boost::shared_ptr<GeomAPI_Pnt2d>(new GeomAPI_Pnt2d(aPnt->x() + aShift, aPnt->y() + aShift));
      aFlyoutAttr->setValue(aFPnt);
      aFlyoutPnt = sketch()->to3D(aFPnt->x(), aFPnt->y());
    }
  } else if (aKind == SketchPlugin_Arc::ID()) {
    aCenterAttr = boost::dynamic_pointer_cast<GeomDataAPI_Point2D>(
        aData->attribute(SketchPlugin_Arc::CENTER_ID()));
    boost::shared_ptr<GeomDataAPI_Point2D> aStartAttr = boost::dynamic_pointer_cast<
        GeomDataAPI_Point2D>(aData->attribute(SketchPlugin_Arc::START_ID()));
    aRadius = aCenterAttr->pnt()->distance(aStartAttr->pnt());
    if (!aFlyoutPnt) {
      boost::shared_ptr<GeomDataAPI_Point2D> aStartAttr = boost::dynamic_pointer_cast<
        GeomDataAPI_Point2D>(aData->attribute(SketchPlugin_Arc::START_ID()));      
      aFlyoutAttr->setValue(aStartAttr->pnt());
      aFlyoutPnt = sketch()->to3D(aStartAttr->pnt()->x(), aStartAttr->pnt()->y());
    }
  }

  boost::shared_ptr<GeomAPI_Pnt> aCenter = sketch()->to3D(aCenterAttr->x(), aCenterAttr->y());
  boost::shared_ptr<GeomDataAPI_Dir> aNDir = boost::dynamic_pointer_cast<GeomDataAPI_Dir>(
      sketch()->data()->attribute(SketchPlugin_Sketch::NORM_ID()));
  boost::shared_ptr<GeomAPI_Dir> aNormal = aNDir->dir();
  boost::shared_ptr<GeomAPI_Circ> aCircle(new GeomAPI_Circ(aCenter, aNormal, aRadius));

  // Value
  boost::shared_ptr<ModelAPI_AttributeDouble> aValueAttr = boost::dynamic_pointer_cast<
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
  boost::shared_ptr<ModelAPI_Data> aData = data();
  if (!aData->isValid())
    return;

  boost::shared_ptr<ModelAPI_AttributeRefAttr> aRef = boost::dynamic_pointer_cast<
      ModelAPI_AttributeRefAttr>(data()->attribute(SketchPlugin_Constraint::ENTITY_A()));
  FeaturePtr aFeature = ModelAPI_Feature::feature(aRef->object());
  if (!aFeature)
    return;
  std::string aCenterAttrName;
  if (aFeature->getKind() == SketchPlugin_Circle::ID())
    aCenterAttrName = SketchPlugin_Circle::CENTER_ID();
  else if (aFeature->getKind() == SketchPlugin_Arc::ID())
    aCenterAttrName = SketchPlugin_Arc::CENTER_ID();
  boost::shared_ptr<GeomDataAPI_Point2D> aCenterAttr = boost::dynamic_pointer_cast<
      GeomDataAPI_Point2D>(aFeature->data()->attribute(aCenterAttrName));
  boost::shared_ptr<GeomAPI_Pnt2d> aCenter = aCenterAttr->pnt();

  // The specified delta applied on the circle curve, 
  // so it will be scaled due to distance between flyout and center points
  boost::shared_ptr<GeomDataAPI_Point2D> aFlyoutAttr = boost::dynamic_pointer_cast<
      GeomDataAPI_Point2D>(aData->attribute(SketchPlugin_Constraint::FLYOUT_VALUE_PNT()));
  boost::shared_ptr<GeomAPI_Pnt2d> aFlyout = aFlyoutAttr->pnt();

  boost::shared_ptr<ModelAPI_AttributeDouble> aRadius = boost::dynamic_pointer_cast<
      ModelAPI_AttributeDouble>(aData->attribute(SketchPlugin_Constraint::VALUE()));
  double aScale = aFlyout->distance(aCenter) / aRadius->value();

  boost::shared_ptr<GeomAPI_Circ2d> aCircle(new GeomAPI_Circ2d(aCenter, aFlyout));
  aFlyout->setX(aFlyout->x() + aScale * theDeltaX);
  aFlyout->setY(aFlyout->y() + aScale * theDeltaY);
  aFlyout = aCircle->project(aFlyout);

  aFlyoutAttr->setValue(aFlyout->x(), aFlyout->y());
}
