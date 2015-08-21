// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:    SketchPlugin_ConstraintAngle.cpp
// Created: 19 August 2015
// Author:  Artem ZHIDKOV

#include "SketchPlugin_ConstraintAngle.h"
#include <SketchPlugin_Line.h>

#include <ModelAPI_AttributeDouble.h>
#include <GeomDataAPI_Point2D.h>

#include <GeomAPI_Dir2d.h>
#include <GeomAPI_Lin2d.h>
#include <GeomAPI_Pnt2d.h>
#include <GeomAPI_XY.h>

#include <SketcherPrs_Factory.h>
#include <SketcherPrs_Tools.h>

const double tolerance = 1.e-7;
#define PI 3.1415926535897932


SketchPlugin_ConstraintAngle::SketchPlugin_ConstraintAngle()
{
  myFlyoutUpdate = false;
}

void SketchPlugin_ConstraintAngle::initAttributes()
{
  data()->addAttribute(SketchPlugin_Constraint::VALUE(), ModelAPI_AttributeDouble::typeId());
  data()->addAttribute(SketchPlugin_Constraint::ENTITY_A(), ModelAPI_AttributeRefAttr::typeId());
  data()->addAttribute(SketchPlugin_Constraint::ENTITY_B(), ModelAPI_AttributeRefAttr::typeId());
  data()->addAttribute(SketchPlugin_Constraint::FLYOUT_VALUE_PNT(), GeomDataAPI_Point2D::typeId());
}

void SketchPlugin_ConstraintAngle::execute()
{
  std::shared_ptr<ModelAPI_Data> aData = data();
  AttributeDoublePtr anAttrValue = std::dynamic_pointer_cast<ModelAPI_AttributeDouble>(
      aData->attribute(SketchPlugin_Constraint::VALUE()));

  if(anAttrValue->isInitialized())
    return;

  double anAngle = calculateAngle();
  anAttrValue->setValue(anAngle);
}

AISObjectPtr SketchPlugin_ConstraintAngle::getAISObject(AISObjectPtr thePrevious)
{
  if (!sketch())
    return thePrevious;

  AISObjectPtr anAIS = thePrevious;
  if (!anAIS) {
    anAIS = SketcherPrs_Factory::angleConstraint(this, sketch()->coordinatePlane());
  }

  // Set color from preferences
  std::vector<int> aRGB = Config_PropManager::color("Visualization", "sketch_dimension_color",
                                                    SKETCH_DIMENSION_COLOR);
  anAIS->setColor(aRGB[0], aRGB[1], aRGB[2]);
  return anAIS;
}

void SketchPlugin_ConstraintAngle::attributeChanged(const std::string& theID)
{
  if (theID == SketchPlugin_Constraint::ENTITY_A() || 
      theID == SketchPlugin_Constraint::ENTITY_B()) {
    std::shared_ptr<ModelAPI_Data> aData = data();
    if (!aData)
      return;
  FeaturePtr aLineA = SketcherPrs_Tools::getFeatureLine(aData, SketchPlugin_Constraint::ENTITY_A());
  FeaturePtr aLineB = SketcherPrs_Tools::getFeatureLine(aData, SketchPlugin_Constraint::ENTITY_B());
    if (!aLineA || !aLineB)
      return;

    std::shared_ptr<ModelAPI_AttributeDouble> aValueAttr = std::dynamic_pointer_cast<
        ModelAPI_AttributeDouble>(data()->attribute(SketchPlugin_Constraint::VALUE()));
    if (!aValueAttr->isInitialized()) { // only if it is not initialized, try to compute the current value
      double anAngle = calculateAngle();
      aValueAttr->setValue(anAngle);
    }
  } else if (theID == SketchPlugin_Constraint::FLYOUT_VALUE_PNT() && !myFlyoutUpdate) {
    // Recalculate flyout point in local coordinates
    std::shared_ptr<GeomDataAPI_Point2D> aFlyoutAttr =
        std::dynamic_pointer_cast<GeomDataAPI_Point2D>(attribute(SketchPlugin_Constraint::FLYOUT_VALUE_PNT()));
  }
}

double SketchPlugin_ConstraintAngle::calculateAngle()
{
  double anAngle = 0.0;

  std::shared_ptr<ModelAPI_Data> aData = data();
  std::shared_ptr<GeomAPI_Ax3> aPlane = SketchPlugin_Sketch::plane(sketch());
  FeaturePtr aLineA = SketcherPrs_Tools::getFeatureLine(aData, SketchPlugin_Constraint::ENTITY_A());
  FeaturePtr aLineB = SketcherPrs_Tools::getFeatureLine(aData, SketchPlugin_Constraint::ENTITY_B());

  // Start and end points of lines
  std::shared_ptr<GeomDataAPI_Point2D> aPointA1 = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
      aLineA->attribute(SketchPlugin_Line::START_ID()));
  std::shared_ptr<GeomDataAPI_Point2D> aPointA2 = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
      aLineA->attribute(SketchPlugin_Line::END_ID()));

  std::shared_ptr<GeomDataAPI_Point2D> aPointB1 = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
      aLineB->attribute(SketchPlugin_Line::START_ID()));
  std::shared_ptr<GeomDataAPI_Point2D> aPointB2 = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
      aLineB->attribute(SketchPlugin_Line::END_ID()));

  std::shared_ptr<GeomAPI_Pnt2d> aStartA = aPointA1->pnt();
  std::shared_ptr<GeomAPI_Pnt2d> aEndA   = aPointA2->pnt();
  std::shared_ptr<GeomAPI_Pnt2d> aStartB = aPointB1->pnt();
  std::shared_ptr<GeomAPI_Pnt2d> aEndB   = aPointB2->pnt();
  if (aStartA->distance(aEndA) < tolerance || aStartB->distance(aEndB) < tolerance)
    return anAngle;

  // Lines and their intersection point
  std::shared_ptr<GeomAPI_Lin2d> aLA(new GeomAPI_Lin2d(aStartA, aEndA));
  std::shared_ptr<GeomAPI_Lin2d> aLB(new GeomAPI_Lin2d(aStartB, aEndB));
  std::shared_ptr<GeomAPI_Pnt2d> anInter = aLA->intersect(aLB);
  if (!anInter)
    return anAngle;

  // Directions of lines
  if (anInter->distance(aEndA) < tolerance)
    aEndA = aStartA;
  if (anInter->distance(aEndB) < tolerance)
    aEndB = aStartB;
  std::shared_ptr<GeomAPI_Dir2d> aDirA(new GeomAPI_Dir2d(aEndA->xy()->decreased(anInter->xy())));
  std::shared_ptr<GeomAPI_Dir2d> aDirB(new GeomAPI_Dir2d(aEndB->xy()->decreased(anInter->xy())));

  anAngle = aDirA->angle(aDirB) * 180.0 / PI;
  return anAngle;
}

void SketchPlugin_ConstraintAngle::move(double theDeltaX, double theDeltaY)
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
