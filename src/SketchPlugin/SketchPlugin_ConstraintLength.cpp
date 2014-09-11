// File:    SketchPlugin_ConstraintLength.cpp
// Created: 30 May 2014
// Author:  Artem ZHIDKOV

#include "SketchPlugin_ConstraintLength.h"
#include <SketchPlugin_Line.h>

#include <GeomDataAPI_Point2D.h>

#include <ModelAPI_AttributeDouble.h>
#include <ModelAPI_Data.h>
#include <ModelAPI_Result.h>

#include <GeomDataAPI_Point2D.h>

#include <GeomAPI_Lin2d.h>
#include <GeomAPI_Pnt2d.h>

#include <Config_PropManager.h>

SketchPlugin_ConstraintLength::SketchPlugin_ConstraintLength()
{
}

void SketchPlugin_ConstraintLength::initAttributes()
{
  data()->addAttribute(SketchPlugin_Constraint::VALUE(), ModelAPI_AttributeDouble::type());
  data()->addAttribute(SketchPlugin_Constraint::FLYOUT_VALUE_PNT(), GeomDataAPI_Point2D::type());
  data()->addAttribute(SketchPlugin_Constraint::ENTITY_A(), ModelAPI_AttributeRefAttr::type());
}

void SketchPlugin_ConstraintLength::execute()
{
  boost::shared_ptr<ModelAPI_AttributeRefAttr> aRef = boost::dynamic_pointer_cast<
      ModelAPI_AttributeRefAttr>(data()->attribute(SketchPlugin_Constraint::ENTITY_A()));
  FeaturePtr aFeature = ModelAPI_Feature::feature(aRef->object());
  if (aFeature) {
    // set length value
    boost::shared_ptr<GeomDataAPI_Point2D> aPoint1 = boost::dynamic_pointer_cast<
        GeomDataAPI_Point2D>(aFeature->data()->attribute(SketchPlugin_Line::START_ID()));
    boost::shared_ptr<GeomDataAPI_Point2D> aPoint2 = boost::dynamic_pointer_cast<
        GeomDataAPI_Point2D>(aFeature->data()->attribute(SketchPlugin_Line::END_ID()));

    double aLenght = aPoint1->pnt()->distance(aPoint2->pnt());

    boost::shared_ptr<ModelAPI_AttributeDouble> aValueAttr = boost::dynamic_pointer_cast<
        ModelAPI_AttributeDouble>(data()->attribute(SketchPlugin_Constraint::VALUE()));
    if(!aValueAttr->isInitialized()) {
      aValueAttr->setValue(aLenght);
    }
  }
}

AISObjectPtr SketchPlugin_ConstraintLength::getAISObject(AISObjectPtr thePrevious)
{
  if (!sketch())
    return thePrevious;

  boost::shared_ptr<GeomAPI_Pln> aPlane = sketch()->plane();

  boost::shared_ptr<ModelAPI_AttributeRefAttr> anAttr = boost::dynamic_pointer_cast<
      ModelAPI_AttributeRefAttr>(data()->attribute(SketchPlugin_Constraint::ENTITY_A()));
  if (!anAttr)
    return thePrevious;
  FeaturePtr aFeature = ModelAPI_Feature::feature(anAttr->object());
  if (!aFeature || aFeature->getKind() != SketchPlugin_Line::ID())
    return thePrevious;

  boost::shared_ptr<GeomDataAPI_Point2D> aFlyOutAttr = boost::dynamic_pointer_cast<
      GeomDataAPI_Point2D>(data()->attribute(SketchPlugin_Constraint::FLYOUT_VALUE_PNT()));

  DataPtr aData = aFeature->data();
  boost::shared_ptr<GeomDataAPI_Point2D> aStartPoint = boost::dynamic_pointer_cast<
      GeomDataAPI_Point2D>(aData->attribute(SketchPlugin_Line::START_ID()));
  boost::shared_ptr<GeomDataAPI_Point2D> anEndPoint = boost::dynamic_pointer_cast<
      GeomDataAPI_Point2D>(aData->attribute(SketchPlugin_Line::END_ID()));

  boost::shared_ptr<GeomAPI_Pnt> aPoint1 = sketch()->to3D(aStartPoint->x(), aStartPoint->y());
  boost::shared_ptr<GeomAPI_Pnt> aPoint2 = sketch()->to3D(anEndPoint->x(), anEndPoint->y());
  boost::shared_ptr<GeomAPI_Pnt> aFlyoutPnt =
      aFlyOutAttr->isInitialized() ?
          sketch()->to3D(aFlyOutAttr->x(), aFlyOutAttr->y()) : boost::shared_ptr<GeomAPI_Pnt>();

  // value calculation
  boost::shared_ptr<ModelAPI_AttributeDouble> aValueAttr = boost::dynamic_pointer_cast<
      ModelAPI_AttributeDouble>(data()->attribute(SketchPlugin_Constraint::VALUE()));
  double aValue = aValueAttr->value();

  AISObjectPtr anAIS = thePrevious;
  if (!anAIS)
    anAIS = AISObjectPtr(new GeomAPI_AISObject);
  anAIS->createDistance(aPoint1, aPoint2, aFlyoutPnt, aPlane, aValue);

  // Set color from preferences
  std::vector<int> aRGB = Config_PropManager::color("Visualization", "length_color", LENGTH_COLOR);
  anAIS->setColor(aRGB[0], aRGB[1], aRGB[2]);
  return anAIS;
}

void SketchPlugin_ConstraintLength::move(double theDeltaX, double theDeltaY)
{
  boost::shared_ptr<ModelAPI_Data> aData = data();
  if (!aData->isValid())
    return;

  boost::shared_ptr<GeomDataAPI_Point2D> aPoint1 = boost::dynamic_pointer_cast<GeomDataAPI_Point2D>(
      aData->attribute(SketchPlugin_Constraint::FLYOUT_VALUE_PNT()));
  aPoint1->setValue(aPoint1->x() + theDeltaX, aPoint1->y() + theDeltaY);
}

