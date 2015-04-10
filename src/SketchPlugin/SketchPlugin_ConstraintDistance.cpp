// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:    SketchPlugin_ConstraintDistance.cpp
// Created: 23 May 2014
// Author:  Artem ZHIDKOV

#include "SketchPlugin_ConstraintDistance.h"
#include <SketchPlugin_Point.h>
#include <SketchPlugin_Circle.h>
#include <SketchPlugin_Line.h>

#include <SketcherPrs_Tools.h>
#include <SketcherPrs_Factory.h>

#include <GeomAPI_Lin2d.h>
#include <GeomAPI_Pnt2d.h>
#include <GeomDataAPI_Point2D.h>

#include <ModelAPI_AttributeDouble.h>
#include <ModelAPI_Data.h>

#include <Config_PropManager.h>

const double tolerance = 1e-7;


SketchPlugin_ConstraintDistance::SketchPlugin_ConstraintDistance()
{
}

//*************************************************************************************
void SketchPlugin_ConstraintDistance::initAttributes()
{
  data()->addAttribute(SketchPlugin_Constraint::VALUE(), ModelAPI_AttributeDouble::typeId());
  data()->addAttribute(SketchPlugin_Constraint::FLYOUT_VALUE_PNT(), GeomDataAPI_Point2D::typeId());
  data()->addAttribute(SketchPlugin_Constraint::ENTITY_A(), ModelAPI_AttributeRefAttr::typeId());
  data()->addAttribute(SketchPlugin_Constraint::ENTITY_B(), ModelAPI_AttributeRefAttr::typeId());
}

//*************************************************************************************
void SketchPlugin_ConstraintDistance::execute()
{
  std::shared_ptr<ModelAPI_Data> aData = data();
  AttributeDoublePtr anAttrValue = std::dynamic_pointer_cast<ModelAPI_AttributeDouble>(
      aData->attribute(SketchPlugin_Constraint::VALUE()));

  if(anAttrValue->isInitialized())
    return;

  double aDistance = calculateCurrentDistance();
  if(aDistance >= 0) {
    anAttrValue->setValue(aDistance);
  }

  // the value should to be computed here, not in the getAISObject in order to change the model value
  // inside the object transaction. This is important for creating a constraint by preselection.
  // The display of the presentation in this case happens after the transaction commit
  std::shared_ptr<GeomDataAPI_Point2D> aFlyOutAttr = std::dynamic_pointer_cast<
      GeomDataAPI_Point2D>(aData->attribute(SketchPlugin_Constraint::FLYOUT_VALUE_PNT()));
  if(!aFlyOutAttr->isInitialized())
    compute(SketchPlugin_Constraint::FLYOUT_VALUE_PNT());
}

bool SketchPlugin_ConstraintDistance::compute(const std::string& theAttributeId)
{
  if (theAttributeId != SketchPlugin_Constraint::FLYOUT_VALUE_PNT())
    return false;

  if (!sketch())
    return false;

  DataPtr aData = data();
  std::shared_ptr<GeomDataAPI_Point2D> aPoint_A = SketcherPrs_Tools::getFeaturePoint(
      aData, SketchPlugin_Constraint::ENTITY_A());
  std::shared_ptr<GeomDataAPI_Point2D> aPoint_B = SketcherPrs_Tools::getFeaturePoint(
      aData, SketchPlugin_Constraint::ENTITY_B());

  std::shared_ptr<GeomAPI_Pnt2d> aPnt_A;
  std::shared_ptr<GeomAPI_Pnt2d> aPnt_B;

  if (aPoint_A && aPoint_B) {
    aPnt_A = aPoint_A->pnt();
    aPnt_B = aPoint_B->pnt();
  } else if (!aPoint_A && aPoint_B) {
    //std::shared_ptr<SketchPlugin_Line> aLine = SketcherPrs_Tools::getFeatureLine(
    //    aData, SketchPlugin_Constraint::ENTITY_A());
    //if (aLine) {
    //  aPnt_B = aPoint_B->pnt();
    //  aPnt_A = SketcherPrs_Tools::getProjectionPoint(aLine, aPnt_B);
    //}
  } else if (aPoint_A && !aPoint_B) {
    //std::shared_ptr<SketchPlugin_Line> aLine = SketcherPrs_Tools::getFeatureLine(
    //    aData, SketchPlugin_Constraint::ENTITY_B());
    //if (aLine) {
    //  aPnt_A = aPoint_A->pnt();
    //  aPnt_B = SketcherPrs_Tools::getProjectionPoint(aLine, aPnt_A);
    //}
  }
  if (!aPnt_A || !aPnt_B)
    return false;

  std::shared_ptr<GeomDataAPI_Point2D> aFlyOutAttr = std::dynamic_pointer_cast<
                           GeomDataAPI_Point2D>(aData->attribute(theAttributeId));

  std::shared_ptr<GeomAPI_Pnt> aPoint1 = sketch()->to3D(aPnt_A->x(), aPnt_A->y());
  std::shared_ptr<GeomAPI_Pnt> aPoint2 = sketch()->to3D(aPnt_B->x(), aPnt_B->y());
  // it is not possible to create lin2d on the points with equal position
  if (aPoint1->distance(aPoint2) < tolerance)
    return false;

  std::shared_ptr<GeomAPI_Lin2d> aLine = std::shared_ptr<GeomAPI_Lin2d>(new GeomAPI_Lin2d(aPnt_A, aPnt_B));
  double aDist = aPoint1->distance(aPoint2)/5.;
  std::shared_ptr<GeomAPI_Pnt2d> aFPnt = aLine->shiftedLocation(aDist);
  aFlyOutAttr->setValue(aFPnt);

  return true;
}

//*************************************************************************************
AISObjectPtr SketchPlugin_ConstraintDistance::getAISObject(AISObjectPtr thePrevious)
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

//*************************************************************************************
void SketchPlugin_ConstraintDistance::move(double theDeltaX, double theDeltaY)
{
  std::shared_ptr<ModelAPI_Data> aData = data();
  if (!aData->isValid())
    return;

  std::shared_ptr<GeomDataAPI_Point2D> aPoint = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
      aData->attribute(SketchPlugin_Constraint::FLYOUT_VALUE_PNT()));
  aPoint->move(theDeltaX, theDeltaY);
}

double SketchPlugin_ConstraintDistance::calculateCurrentDistance() const
{
  double aDistance = -1.;

  std::shared_ptr<ModelAPI_Data> aData = data();
  std::shared_ptr<GeomDataAPI_Point2D> aPointA =
    SketcherPrs_Tools::getFeaturePoint(aData, SketchPlugin_Constraint::ENTITY_A());
  std::shared_ptr<GeomDataAPI_Point2D> aPointB =
      SketcherPrs_Tools::getFeaturePoint(aData, SketchPlugin_Constraint::ENTITY_B());

  if (aPointA && aPointB) {  // both points
    aDistance = aPointA->pnt()->distance(aPointB->pnt());
  } else {
//    if (!aPointA && aPointB) {  //Line and point
//      std::shared_ptr<SketchPlugin_Line> aLine =
//          SketcherPrs_Tools::getFeatureLine(aData, SketchPlugin_Constraint::ENTITY_A());
//      if (aLine) {
//        aDistance = aLine->distanceToPoint(aPointB->pnt());
//      }
//    } else if (aPointA && !aPointB) {  // Point and line
//      std::shared_ptr<SketchPlugin_Line> aLine =
//          SketcherPrs_Tools::getFeatureLine(aData, SketchPlugin_Constraint::ENTITY_B());
//      if (aLine) {
//        aDistance = aLine->distanceToPoint(aPointA->pnt());
//      }
//    }
  }
  return aDistance;
}

void SketchPlugin_ConstraintDistance::attributeChanged(const std::string& theID) {
  if (theID == SketchPlugin_Constraint::ENTITY_A() || 
      theID == SketchPlugin_Constraint::ENTITY_B()) 
  {
    std::shared_ptr<ModelAPI_AttributeDouble> aValueAttr = std::dynamic_pointer_cast<
        ModelAPI_AttributeDouble>(data()->attribute(SketchPlugin_Constraint::VALUE()));
    if (!aValueAttr->isInitialized()) { // only if it is not initialized, try to compute the current value
      double aDistance = calculateCurrentDistance();
      if (aDistance > 0) { // set as value the length of updated references
        aValueAttr->setValue(aDistance);
      }
    }
  }
}

