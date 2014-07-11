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
  data()->addAttribute(CONSTRAINT_ATTR_VALUE,    ModelAPI_AttributeDouble::type());
  data()->addAttribute(CONSTRAINT_ATTR_ENTITY_A, ModelAPI_AttributeRefAttr::type());
  data()->addAttribute(CONSTRAINT_ATTR_FLYOUT_VALUE_PNT, GeomDataAPI_Point2D::type());
}

void SketchPlugin_ConstraintRadius::execute(boost::shared_ptr<ModelAPI_Result>& theResult)
{
  if (data()->attribute(CONSTRAINT_ATTR_ENTITY_A)->isInitialized() &&
      !data()->attribute(CONSTRAINT_ATTR_VALUE)->isInitialized()) {

    boost::shared_ptr<ModelAPI_AttributeRefAttr> aRef =
      boost::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(data()->attribute(CONSTRAINT_ATTR_ENTITY_A));
    FeaturePtr aFeature = aRef->feature();
    if (aFeature) {
      double aRadius = 0;
      boost::shared_ptr<ModelAPI_Data> aData = aFeature->data();
      if (aFeature->getKind() == SKETCH_CIRCLE_KIND) {
        AttributeDoublePtr anAttribute = boost::dynamic_pointer_cast<ModelAPI_AttributeDouble>
                                            (aData->attribute(CIRCLE_ATTR_RADIUS));
        if (anAttribute)
          aRadius = anAttribute->value();
      }
      else if (aFeature->getKind() == SKETCH_ARC_KIND) {
        boost::shared_ptr<GeomDataAPI_Point2D> aCenterAttr = 
          boost::dynamic_pointer_cast<GeomDataAPI_Point2D>(aData->attribute(ARC_ATTR_CENTER));
        boost::shared_ptr<GeomDataAPI_Point2D> aStartAttr = 
          boost::dynamic_pointer_cast<GeomDataAPI_Point2D>(aData->attribute(ARC_ATTR_START));
        if (aCenterAttr && aStartAttr)
          aRadius = aCenterAttr->pnt()->distance(aStartAttr->pnt());
      }
      boost::shared_ptr<ModelAPI_AttributeDouble> aValueAttr = 
        boost::dynamic_pointer_cast<ModelAPI_AttributeDouble>(data()->attribute(CONSTRAINT_ATTR_VALUE));
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
    boost::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(aData->attribute(CONSTRAINT_ATTR_ENTITY_A));
  if (!anAttr)
    return thePrevious;
  FeaturePtr aFeature = anAttr->feature();
  std::string aKind = aFeature ? aFeature->getKind() : "";
  if (aKind != SKETCH_CIRCLE_KIND && aKind != SKETCH_ARC_KIND)
    return thePrevious;

  // Flyout point
  boost::shared_ptr<GeomDataAPI_Point2D> aFlyoutAttr = 
    boost::dynamic_pointer_cast<GeomDataAPI_Point2D>(aData->attribute(CONSTRAINT_ATTR_FLYOUT_VALUE_PNT));
  if (!aFlyoutAttr->isInitialized())
    return thePrevious;
  boost::shared_ptr<GeomAPI_Pnt> aFlyoutPnt = sketch()->to3D(aFlyoutAttr->x(), aFlyoutAttr->y());

  // Prepare a circle
  aData = aFeature->data();
  boost::shared_ptr<GeomDataAPI_Point2D> aCenterAttr;
  double aRadius;
  if (aKind == SKETCH_CIRCLE_KIND) {
    aCenterAttr = boost::dynamic_pointer_cast<GeomDataAPI_Point2D>(aData->attribute(CIRCLE_ATTR_CENTER));
    AttributeDoublePtr aCircRadius = 
      boost::dynamic_pointer_cast<ModelAPI_AttributeDouble>(aData->attribute(CIRCLE_ATTR_RADIUS));
    aRadius = aCircRadius->value();
  }
  else if (aKind == SKETCH_ARC_KIND) {
    aCenterAttr = boost::dynamic_pointer_cast<GeomDataAPI_Point2D>(aData->attribute(ARC_ATTR_CENTER));
    boost::shared_ptr<GeomDataAPI_Point2D> aStartAttr = 
      boost::dynamic_pointer_cast<GeomDataAPI_Point2D>(aData->attribute(ARC_ATTR_START));
    aRadius = aCenterAttr->pnt()->distance(aStartAttr->pnt());
  }

  boost::shared_ptr<GeomAPI_Pnt> aCenter = sketch()->to3D(aCenterAttr->x(), aCenterAttr->y());
  boost::shared_ptr<GeomDataAPI_Dir> aNDir = 
    boost::dynamic_pointer_cast<GeomDataAPI_Dir>(sketch()->data()->attribute(SKETCH_ATTR_NORM));
  boost::shared_ptr<GeomAPI_Dir> aNormal = aNDir->dir();
  boost::shared_ptr<GeomAPI_Circ> aCircle(new GeomAPI_Circ(aCenter, aNormal, aRadius));

  // Value
  boost::shared_ptr<ModelAPI_AttributeDouble> aValueAttr = 
    boost::dynamic_pointer_cast<ModelAPI_AttributeDouble>(aData->attribute(CONSTRAINT_ATTR_VALUE));
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
    boost::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(data()->attribute(CONSTRAINT_ATTR_ENTITY_A));
  FeaturePtr aFeature = aRef->feature();
  if (!aFeature)
    return;
  std::string aCenterAttrName;
  if (aFeature->getKind() == SKETCH_CIRCLE_KIND)
    aCenterAttrName = CIRCLE_ATTR_CENTER;
  else if (aFeature->getKind() == SKETCH_ARC_KIND)
    aCenterAttrName = ARC_ATTR_CENTER;
  boost::shared_ptr<GeomDataAPI_Point2D> aCenterAttr = 
    boost::dynamic_pointer_cast<GeomDataAPI_Point2D>(aFeature->data()->attribute(aCenterAttrName));
  boost::shared_ptr<GeomAPI_Pnt2d> aCenter = aCenterAttr->pnt();

  // The specified delta applied on the circle curve, 
  // so it will be scaled due to distance between flyout and center points
  boost::shared_ptr<GeomDataAPI_Point2D> aFlyoutAttr =
    boost::dynamic_pointer_cast<GeomDataAPI_Point2D>(aData->attribute(CONSTRAINT_ATTR_FLYOUT_VALUE_PNT));
  boost::shared_ptr<GeomAPI_Pnt2d> aFlyout = aFlyoutAttr->pnt();

  boost::shared_ptr<ModelAPI_AttributeDouble> aRadius =
    boost::dynamic_pointer_cast<ModelAPI_AttributeDouble>(aData->attribute(CONSTRAINT_ATTR_VALUE));
  double aScale = aFlyout->distance(aCenter) / aRadius->value();

  boost::shared_ptr<GeomAPI_Circ2d> aCircle(new GeomAPI_Circ2d(aCenter, aFlyout));
  aFlyout->setX(aFlyout->x() + aScale * theDeltaX);
  aFlyout->setY(aFlyout->y() + aScale * theDeltaY);
  aFlyout = aCircle->project(aFlyout);

  aFlyoutAttr->setValue(aFlyout->x(), aFlyout->y());
}
