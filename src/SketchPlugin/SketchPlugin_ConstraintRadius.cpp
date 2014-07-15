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

SketchPlugin_ConstraintRadius::SketchPlugin_ConstraintRadius()
{
}

void SketchPlugin_ConstraintRadius::initAttributes()
{
  data()->addAttribute(SketchPlugin_Constraint::VALUE(),    ModelAPI_AttributeDouble::type());
  data()->addAttribute(SketchPlugin_Constraint::ENTITY_A(), ModelAPI_AttributeRefAttr::type());
  data()->addAttribute(SketchPlugin_Constraint::FLYOUT_VALUE_PNT(), GeomDataAPI_Point2D::type());
}

void SketchPlugin_ConstraintRadius::execute()
{
  if (data()->attribute(SketchPlugin_Constraint::ENTITY_A())->isInitialized() &&
      !data()->attribute(SketchPlugin_Constraint::VALUE())->isInitialized()) {

    boost::shared_ptr<ModelAPI_AttributeRefAttr> aRef =
      boost::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(data()->attribute(SketchPlugin_Constraint::ENTITY_A()));
    FeaturePtr aFeature = boost::dynamic_pointer_cast<ModelAPI_Feature>(aRef->object());
    if (aFeature) {
      double aRadius = 0;
      boost::shared_ptr<ModelAPI_Data> aData = aFeature->data();
      if (aFeature->getKind() == SketchPlugin_Circle::ID()) {
        AttributeDoublePtr anAttribute = boost::dynamic_pointer_cast<ModelAPI_AttributeDouble>
                                            (aData->attribute(SketchPlugin_Circle::RADIUS_ID()));
        if (anAttribute)
          aRadius = anAttribute->value();
      }
      else if (aFeature->getKind() == SketchPlugin_Arc::ID()) {
        boost::shared_ptr<GeomDataAPI_Point2D> aCenterAttr = 
          boost::dynamic_pointer_cast<GeomDataAPI_Point2D>(aData->attribute(SketchPlugin_Arc::CENTER_ID()));
        boost::shared_ptr<GeomDataAPI_Point2D> aStartAttr = 
          boost::dynamic_pointer_cast<GeomDataAPI_Point2D>(aData->attribute(SketchPlugin_Arc::START_ID()));
        if (aCenterAttr && aStartAttr)
          aRadius = aCenterAttr->pnt()->distance(aStartAttr->pnt());
      }
      boost::shared_ptr<ModelAPI_AttributeDouble> aValueAttr = 
        boost::dynamic_pointer_cast<ModelAPI_AttributeDouble>(data()->attribute(SketchPlugin_Constraint::VALUE()));
      aValueAttr->setValue(aRadius);
    }
  }
}

boost::shared_ptr<GeomAPI_AISObject> SketchPlugin_ConstraintRadius::getAISObject(
                    boost::shared_ptr<GeomAPI_AISObject> thePrevious)
{
  if (!sketch())
    return thePrevious;

  boost::shared_ptr<ModelAPI_Data> aData = data();
  boost::shared_ptr<ModelAPI_AttributeRefAttr> anAttr = 
    boost::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(aData->attribute(SketchPlugin_Constraint::ENTITY_A()));
  if (!anAttr)
    return thePrevious;
  FeaturePtr aFeature = boost::dynamic_pointer_cast<ModelAPI_Feature>(anAttr->object());
  std::string aKind = aFeature ? aFeature->getKind() : "";
  if (aKind != SketchPlugin_Circle::ID() && aKind != SketchPlugin_Arc::ID())
    return thePrevious;

  // Flyout point
  boost::shared_ptr<GeomDataAPI_Point2D> aFlyoutAttr = 
    boost::dynamic_pointer_cast<GeomDataAPI_Point2D>(aData->attribute(SketchPlugin_Constraint::FLYOUT_VALUE_PNT()));
  if (!aFlyoutAttr->isInitialized())
    return thePrevious;
  boost::shared_ptr<GeomAPI_Pnt> aFlyoutPnt = sketch()->to3D(aFlyoutAttr->x(), aFlyoutAttr->y());

  // Prepare a circle
  aData = aFeature->data();
  boost::shared_ptr<GeomDataAPI_Point2D> aCenterAttr;
  double aRadius;
  if (aKind == SketchPlugin_Circle::ID()) {
    aCenterAttr = boost::dynamic_pointer_cast<GeomDataAPI_Point2D>(aData->attribute(SketchPlugin_Circle::CENTER_ID()));
    AttributeDoublePtr aCircRadius = 
      boost::dynamic_pointer_cast<ModelAPI_AttributeDouble>(aData->attribute(SketchPlugin_Circle::RADIUS_ID()));
    aRadius = aCircRadius->value();
  }
  else if (aKind == SketchPlugin_Arc::ID()) {
    aCenterAttr = boost::dynamic_pointer_cast<GeomDataAPI_Point2D>(aData->attribute(SketchPlugin_Arc::CENTER_ID()));
    boost::shared_ptr<GeomDataAPI_Point2D> aStartAttr = 
      boost::dynamic_pointer_cast<GeomDataAPI_Point2D>(aData->attribute(SketchPlugin_Arc::START_ID()));
    aRadius = aCenterAttr->pnt()->distance(aStartAttr->pnt());
  }

  boost::shared_ptr<GeomAPI_Pnt> aCenter = sketch()->to3D(aCenterAttr->x(), aCenterAttr->y());
  boost::shared_ptr<GeomDataAPI_Dir> aNDir = 
    boost::dynamic_pointer_cast<GeomDataAPI_Dir>(sketch()->data()->attribute(SketchPlugin_Sketch::NORM_ID()));
  boost::shared_ptr<GeomAPI_Dir> aNormal = aNDir->dir();
  boost::shared_ptr<GeomAPI_Circ> aCircle(new GeomAPI_Circ(aCenter, aNormal, aRadius));

  // Value
  boost::shared_ptr<ModelAPI_AttributeDouble> aValueAttr = 
    boost::dynamic_pointer_cast<ModelAPI_AttributeDouble>(aData->attribute(SketchPlugin_Constraint::VALUE()));
  double aValue = aRadius;
  if (aValueAttr && aValueAttr->isInitialized())
   aValue = aValueAttr->value();

  boost::shared_ptr<GeomAPI_AISObject> anAIS = thePrevious;
  if (!anAIS)
    anAIS = boost::shared_ptr<GeomAPI_AISObject>(new GeomAPI_AISObject);
  anAIS->createRadius(aCircle, aFlyoutPnt, aValue);
  return anAIS;
}

void SketchPlugin_ConstraintRadius::move(double theDeltaX, double theDeltaY)
{
  boost::shared_ptr<ModelAPI_Data> aData = data();
  if (!aData->isValid())
    return;

  boost::shared_ptr<ModelAPI_AttributeRefAttr> aRef =
    boost::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(data()->attribute(SketchPlugin_Constraint::ENTITY_A()));
  FeaturePtr aFeature = boost::dynamic_pointer_cast<ModelAPI_Feature>(aRef->object());
  if (!aFeature)
    return;
  std::string aCenterAttrName;
  if (aFeature->getKind() == SketchPlugin_Circle::ID())
    aCenterAttrName = SketchPlugin_Circle::CENTER_ID();
  else if (aFeature->getKind() == SketchPlugin_Arc::ID())
    aCenterAttrName = SketchPlugin_Arc::CENTER_ID();
  boost::shared_ptr<GeomDataAPI_Point2D> aCenterAttr = 
    boost::dynamic_pointer_cast<GeomDataAPI_Point2D>(aFeature->data()->attribute(aCenterAttrName));
  boost::shared_ptr<GeomAPI_Pnt2d> aCenter = aCenterAttr->pnt();

  // The specified delta applied on the circle curve, 
  // so it will be scaled due to distance between flyout and center points
  boost::shared_ptr<GeomDataAPI_Point2D> aFlyoutAttr =
    boost::dynamic_pointer_cast<GeomDataAPI_Point2D>(aData->attribute(SketchPlugin_Constraint::FLYOUT_VALUE_PNT()));
  boost::shared_ptr<GeomAPI_Pnt2d> aFlyout = aFlyoutAttr->pnt();

  boost::shared_ptr<ModelAPI_AttributeDouble> aRadius =
    boost::dynamic_pointer_cast<ModelAPI_AttributeDouble>(aData->attribute(SketchPlugin_Constraint::VALUE()));
  double aScale = aFlyout->distance(aCenter) / aRadius->value();

  boost::shared_ptr<GeomAPI_Circ2d> aCircle(new GeomAPI_Circ2d(aCenter, aFlyout));
  aFlyout->setX(aFlyout->x() + aScale * theDeltaX);
  aFlyout->setY(aFlyout->y() + aScale * theDeltaY);
  aFlyout = aCircle->project(aFlyout);

  aFlyoutAttr->setValue(aFlyout->x(), aFlyout->y());
}
