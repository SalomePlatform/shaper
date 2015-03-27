// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:    SketchPlugin_ConstraintLength.cpp
// Created: 30 May 2014
// Author:  Artem ZHIDKOV

#include "SketchPlugin_ConstraintLength.h"
#include <SketchPlugin_Line.h>

#include <GeomDataAPI_Point2D.h>

#include <SketcherPrs_Factory.h>

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
  data()->addAttribute(SketchPlugin_Constraint::VALUE(), ModelAPI_AttributeDouble::typeId());
  data()->addAttribute(SketchPlugin_Constraint::FLYOUT_VALUE_PNT(), GeomDataAPI_Point2D::typeId());
  data()->addAttribute(SketchPlugin_Constraint::ENTITY_A(), ModelAPI_AttributeRefAttr::typeId());
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

    //std::shared_ptr<ModelAPI_AttributeDouble> aValueAttr = std::dynamic_pointer_cast<
    //    ModelAPI_AttributeDouble>(data()->attribute(SketchPlugin_Constraint::VALUE()));
    //if(!aValueAttr->isInitialized()) {
    //  aValueAttr->setValue(aLenght);
    //}

    // the value should to be computed here, not in the getAISObject in order to change the model value
    // inside the object transaction. This is important for creating a constraint by preselection.
    // The display of the presentation in this case happens after the transaction commit
    AttributePtr aFlyOutAttribute = data()->attribute(SketchPlugin_Constraint::FLYOUT_VALUE_PNT());
    if (!aFlyOutAttribute->isInitialized()) {
      compute(SketchPlugin_Constraint::FLYOUT_VALUE_PNT());
    }
  }
}

bool SketchPlugin_ConstraintLength::compute(const std::string& theAttributeId)
{
  if (theAttributeId != SketchPlugin_Constraint::FLYOUT_VALUE_PNT())
    return false;

  std::shared_ptr<GeomAPI_Pnt> aPoint1, aPoint2;
  std::shared_ptr<GeomDataAPI_Point2D> aStartPoint, anEndPoint;
  if (!getPoints(aPoint1, aPoint2, aStartPoint, anEndPoint))
    return false;

  std::shared_ptr<GeomDataAPI_Point2D> aFlyOutAttr = std::dynamic_pointer_cast<
      GeomDataAPI_Point2D>(data()->attribute(theAttributeId));

  std::shared_ptr<GeomAPI_Lin2d> aLine = 
    std::shared_ptr<GeomAPI_Lin2d>(new GeomAPI_Lin2d(aStartPoint->pnt(), anEndPoint->pnt()));
  double aDist = aPoint1->distance(aPoint2)/5.;
  std::shared_ptr<GeomAPI_Pnt2d> aFPnt = aLine->shiftedLocation(aDist);
  aFlyOutAttr->setValue(aFPnt);

  return true;
}

bool SketchPlugin_ConstraintLength::getPoints(
  std::shared_ptr<GeomAPI_Pnt>& thePoint1, std::shared_ptr<GeomAPI_Pnt>& thePoint2,
  std::shared_ptr<GeomDataAPI_Point2D>& theStartPoint,
  std::shared_ptr<GeomDataAPI_Point2D>& theEndPoint)
{
  if (!sketch())
    return false;
  std::shared_ptr<GeomAPI_Pln> aPlane = sketch()->plane();
  std::shared_ptr<ModelAPI_AttributeRefAttr> anAttr = std::dynamic_pointer_cast<
      ModelAPI_AttributeRefAttr>(data()->attribute(SketchPlugin_Constraint::ENTITY_A()));
  if (!anAttr)
    return false;
  FeaturePtr aFeature = ModelAPI_Feature::feature(anAttr->object());
  if (!aFeature || aFeature->getKind() != SketchPlugin_Line::ID())
    return false;
  DataPtr aData = aFeature->data();
  theStartPoint = std::dynamic_pointer_cast<
      GeomDataAPI_Point2D>(aData->attribute(SketchPlugin_Line::START_ID()));
  theEndPoint = std::dynamic_pointer_cast<
      GeomDataAPI_Point2D>(aData->attribute(SketchPlugin_Line::END_ID()));
  thePoint1 = sketch()->to3D(theStartPoint->x(), theStartPoint->y());
  thePoint2 = sketch()->to3D(theEndPoint->x(), theEndPoint->y());
  return true;
}

AISObjectPtr SketchPlugin_ConstraintLength::getAISObject(AISObjectPtr thePrevious)
{
  if (!sketch())
    return thePrevious;

  AISObjectPtr anAIS = thePrevious;
  if (!anAIS) {
    anAIS = SketcherPrs_Factory::lengthDimensionConstraint(this, sketch()->coordinatePlane());
  }

  // Set color from preferences
  std::vector<int> aRGB = Config_PropManager::color("Visualization", "sketch_dimension_color",
                                                    SKETCH_DIMENSION_COLOR);
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

void SketchPlugin_ConstraintLength::attributeChanged(const std::string& theID) {
  if (theID == SketchPlugin_Constraint::ENTITY_A()) 
  {
    std::shared_ptr<ModelAPI_AttributeDouble> aValueAttr = std::dynamic_pointer_cast<
      ModelAPI_AttributeDouble>(data()->attribute(SketchPlugin_Constraint::VALUE()));
    if (!aValueAttr->isInitialized()) { // only if it is not initialized, try to compute the current value
      std::shared_ptr<GeomAPI_Pnt> aPoint1, aPoint2;
      std::shared_ptr<GeomDataAPI_Point2D> aStartPoint, anEndPoint;
      if (getPoints(aPoint1, aPoint2, aStartPoint, anEndPoint)) {
        double aLength = aPoint1->distance(aPoint2);
        aValueAttr->setValue(aLength);
      }
    }
  }
}
