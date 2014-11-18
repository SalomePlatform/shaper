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
  std::shared_ptr<ModelAPI_AttributeRefAttr> aRef = std::dynamic_pointer_cast<
      ModelAPI_AttributeRefAttr>(data()->attribute(SketchPlugin_Constraint::ENTITY_A()));
  FeaturePtr aFeature = ModelAPI_Feature::feature(aRef->object());
  if (aFeature) {
    // set length value
    std::shared_ptr<GeomDataAPI_Point2D> aPoint1 = std::dynamic_pointer_cast<
        GeomDataAPI_Point2D>(aFeature->data()->attribute(SketchPlugin_Line::START_ID()));
    std::shared_ptr<GeomDataAPI_Point2D> aPoint2 = std::dynamic_pointer_cast<
        GeomDataAPI_Point2D>(aFeature->data()->attribute(SketchPlugin_Line::END_ID()));

    double aLenght = aPoint1->pnt()->distance(aPoint2->pnt());

    std::shared_ptr<ModelAPI_AttributeDouble> aValueAttr = std::dynamic_pointer_cast<
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

  std::shared_ptr<GeomAPI_Pln> aPlane = sketch()->plane();

  std::shared_ptr<ModelAPI_AttributeRefAttr> anAttr = std::dynamic_pointer_cast<
      ModelAPI_AttributeRefAttr>(data()->attribute(SketchPlugin_Constraint::ENTITY_A()));
  if (!anAttr)
    return thePrevious;
  FeaturePtr aFeature = ModelAPI_Feature::feature(anAttr->object());
  if (!aFeature || aFeature->getKind() != SketchPlugin_Line::ID())
    return thePrevious;

  std::shared_ptr<GeomDataAPI_Point2D> aFlyOutAttr = std::dynamic_pointer_cast<
      GeomDataAPI_Point2D>(data()->attribute(SketchPlugin_Constraint::FLYOUT_VALUE_PNT()));

  DataPtr aData = aFeature->data();
  std::shared_ptr<GeomDataAPI_Point2D> aStartPoint = std::dynamic_pointer_cast<
      GeomDataAPI_Point2D>(aData->attribute(SketchPlugin_Line::START_ID()));
  std::shared_ptr<GeomDataAPI_Point2D> anEndPoint = std::dynamic_pointer_cast<
      GeomDataAPI_Point2D>(aData->attribute(SketchPlugin_Line::END_ID()));

  std::shared_ptr<GeomAPI_Pnt> aPoint1 = sketch()->to3D(aStartPoint->x(), aStartPoint->y());
  std::shared_ptr<GeomAPI_Pnt> aPoint2 = sketch()->to3D(anEndPoint->x(), anEndPoint->y());
  std::shared_ptr<GeomAPI_Pnt> aFlyoutPnt = std::shared_ptr<GeomAPI_Pnt>();
  if (aFlyOutAttr->isInitialized()) {
    aFlyoutPnt = sketch()->to3D(aFlyOutAttr->x(), aFlyOutAttr->y());
  } else {
    std::shared_ptr<GeomAPI_Lin2d> aLine = 
      std::shared_ptr<GeomAPI_Lin2d>(new GeomAPI_Lin2d(aStartPoint->pnt(), anEndPoint->pnt()));
    double aDist = aPoint1->distance(aPoint2)/5.;
    std::shared_ptr<GeomAPI_Pnt2d> aFPnt = aLine->shiftedLocation(aDist);
    aFlyOutAttr->setValue(aFPnt);
    aFlyoutPnt = sketch()->to3D(aFPnt->x(), aFPnt->y());
  }
  // value calculation
  std::shared_ptr<ModelAPI_AttributeDouble> aValueAttr = std::dynamic_pointer_cast<
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
  std::shared_ptr<ModelAPI_Data> aData = data();
  if (!aData->isValid())
    return;

  std::shared_ptr<GeomDataAPI_Point2D> aPoint = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
      aData->attribute(SketchPlugin_Constraint::FLYOUT_VALUE_PNT()));
  aPoint->move(theDeltaX, theDeltaY);
}

