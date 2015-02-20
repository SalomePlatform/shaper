// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:    SketchPlugin_ConstraintDistance.cpp
// Created: 23 May 2014
// Author:  Artem ZHIDKOV

#include "SketchPlugin_ConstraintDistance.h"
#include <SketchPlugin_Point.h>
#include <SketchPlugin_Circle.h>
#include <SketchPlugin_Line.h>

#include <GeomAPI_Lin2d.h>
#include <GeomAPI_Pnt2d.h>
#include <GeomDataAPI_Point2D.h>

#include <ModelAPI_AttributeDouble.h>
#include <ModelAPI_Data.h>

#include <Config_PropManager.h>

SketchPlugin_ConstraintDistance::SketchPlugin_ConstraintDistance()
{
}

//*************************************************************************************
void SketchPlugin_ConstraintDistance::initAttributes()
{
  data()->addAttribute(SketchPlugin_Constraint::VALUE(), ModelAPI_AttributeDouble::type());
  data()->addAttribute(SketchPlugin_Constraint::FLYOUT_VALUE_PNT(), GeomDataAPI_Point2D::type());
  data()->addAttribute(SketchPlugin_Constraint::ENTITY_A(), ModelAPI_AttributeRefAttr::type());
  data()->addAttribute(SketchPlugin_Constraint::ENTITY_B(), ModelAPI_AttributeRefAttr::type());
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
  std::shared_ptr<GeomDataAPI_Point2D> aPoint_A = getFeaturePoint(
      aData, SketchPlugin_Constraint::ENTITY_A());
  std::shared_ptr<GeomDataAPI_Point2D> aPoint_B = getFeaturePoint(
      aData, SketchPlugin_Constraint::ENTITY_B());

  std::shared_ptr<GeomAPI_Pnt2d> aPnt_A;
  std::shared_ptr<GeomAPI_Pnt2d> aPnt_B;

  if (aPoint_A && aPoint_B) {
    aPnt_A = aPoint_A->pnt();
    aPnt_B = aPoint_B->pnt();
  } else if (!aPoint_A && aPoint_B) {
    std::shared_ptr<SketchPlugin_Line> aLine = getFeatureLine(
        aData, SketchPlugin_Constraint::ENTITY_A());
    if (aLine) {
      aPnt_B = aPoint_B->pnt();
      aPnt_A = getProjectionPoint(aLine, aPnt_B);
    }
  } else if (aPoint_A && !aPoint_B) {
    std::shared_ptr<SketchPlugin_Line> aLine = getFeatureLine(
        aData, SketchPlugin_Constraint::ENTITY_B());
    if (aLine) {
      aPnt_A = aPoint_A->pnt();
      aPnt_B = getProjectionPoint(aLine, aPnt_A);
    }
  }
  if (!aPnt_A || !aPnt_B)
    return false;

  std::shared_ptr<GeomDataAPI_Point2D> aFlyOutAttr = std::dynamic_pointer_cast<
                           GeomDataAPI_Point2D>(aData->attribute(theAttributeId));

  std::shared_ptr<GeomAPI_Pnt> aPoint1 = sketch()->to3D(aPnt_A->x(), aPnt_A->y());
  std::shared_ptr<GeomAPI_Pnt> aPoint2 = sketch()->to3D(aPnt_B->x(), aPnt_B->y());
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

  std::shared_ptr<GeomAPI_Pln> aPlane = sketch()->plane();

  DataPtr aData = data();
  std::shared_ptr<GeomDataAPI_Point2D> aPoint_A = getFeaturePoint(
      aData, SketchPlugin_Constraint::ENTITY_A());
  std::shared_ptr<GeomDataAPI_Point2D> aPoint_B = getFeaturePoint(
      aData, SketchPlugin_Constraint::ENTITY_B());

  std::shared_ptr<GeomAPI_Pnt2d> aPnt_A;
  std::shared_ptr<GeomAPI_Pnt2d> aPnt_B;

  if (aPoint_A && aPoint_B) {
    aPnt_A = aPoint_A->pnt();
    aPnt_B = aPoint_B->pnt();
  } else if (!aPoint_A && aPoint_B) {
    std::shared_ptr<SketchPlugin_Line> aLine = getFeatureLine(
        aData, SketchPlugin_Constraint::ENTITY_A());
    if (aLine) {
      aPnt_B = aPoint_B->pnt();
      aPnt_A = getProjectionPoint(aLine, aPnt_B);
    }
  } else if (aPoint_A && !aPoint_B) {
    std::shared_ptr<SketchPlugin_Line> aLine = getFeatureLine(
        aData, SketchPlugin_Constraint::ENTITY_B());
    if (aLine) {
      aPnt_A = aPoint_A->pnt();
      aPnt_B = getProjectionPoint(aLine, aPnt_A);
    }
  }
  if (!aPnt_A || !aPnt_B)
    return AISObjectPtr();

  std::shared_ptr<GeomDataAPI_Point2D> aFlyOutAttr = std::dynamic_pointer_cast<
      GeomDataAPI_Point2D>(aData->attribute(SketchPlugin_Constraint::FLYOUT_VALUE_PNT()));

  std::shared_ptr<GeomAPI_Pnt> aPoint1 = sketch()->to3D(aPnt_A->x(), aPnt_A->y());
  std::shared_ptr<GeomAPI_Pnt> aPoint2 = sketch()->to3D(aPnt_B->x(), aPnt_B->y());
  if(!aFlyOutAttr->isInitialized())
    return AISObjectPtr();
  std::shared_ptr<GeomAPI_Pnt> aFlyoutPnt = sketch()->to3D(aFlyOutAttr->x(), aFlyOutAttr->y()/*aFPnt->x(), aFPnt->y()*/);

  // value calculation
  std::shared_ptr<ModelAPI_AttributeDouble> aValueAttr = std::dynamic_pointer_cast<
      ModelAPI_AttributeDouble>(aData->attribute(SketchPlugin_Constraint::VALUE()));
  double aValue = 0;
  // Issue #196: checking the positivity of the distance constraint
  // there is a validator for a distance constraint, that the value should be positive
  // in case if an invalid value is set, the current distance value is shown
  if (aValueAttr->isInitialized())
    aValue = aValueAttr->value();

  AISObjectPtr anAIS = thePrevious;
  if (!anAIS)
    anAIS = AISObjectPtr(new GeomAPI_AISObject);
  anAIS->createDistance(aPoint1, aPoint2, aFlyoutPnt, aPlane, aValue);

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
    getFeaturePoint(aData, SketchPlugin_Constraint::ENTITY_A());
  std::shared_ptr<GeomDataAPI_Point2D> aPointB =
      getFeaturePoint(aData, SketchPlugin_Constraint::ENTITY_B());

  if (aPointA && aPointB) {  // both points
    aDistance = aPointA->pnt()->distance(aPointB->pnt());
  } else {
    if (!aPointA && aPointB) {  //Line and point
      std::shared_ptr<SketchPlugin_Line> aLine =
          getFeatureLine(aData, SketchPlugin_Constraint::ENTITY_A());
      if (aLine) {
        aDistance = aLine->distanceToPoint(aPointB->pnt());
      }
    } else if (aPointA && !aPointB) {  // Point and line
      std::shared_ptr<SketchPlugin_Line> aLine =
          getFeatureLine(aData, SketchPlugin_Constraint::ENTITY_B());
      if (aLine) {
        aDistance = aLine->distanceToPoint(aPointA->pnt());
      }
    }
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

//*************************************************************************************
std::shared_ptr<GeomDataAPI_Point2D> getFeaturePoint(DataPtr theData,
                                                       const std::string& theAttribute)
{
  std::shared_ptr<GeomDataAPI_Point2D> aPointAttr;

  if (!theData)
    return aPointAttr;

  FeaturePtr aFeature;
  std::shared_ptr<ModelAPI_AttributeRefAttr> anAttr = std::dynamic_pointer_cast<
      ModelAPI_AttributeRefAttr>(theData->attribute(theAttribute));
  if (anAttr)
    aFeature = ModelAPI_Feature::feature(anAttr->object());

  if (aFeature && aFeature->getKind() == SketchPlugin_Point::ID())
    aPointAttr = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
        aFeature->data()->attribute(SketchPlugin_Point::COORD_ID()));
  else if (aFeature && aFeature->getKind() == SketchPlugin_Circle::ID())
    aPointAttr = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
        aFeature->data()->attribute(SketchPlugin_Circle::CENTER_ID()));
  else if (anAttr->attr()) {
    aPointAttr = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(anAttr->attr());
  }
  return aPointAttr;
}

//*************************************************************************************
std::shared_ptr<SketchPlugin_Line> getFeatureLine(DataPtr theData,
                                                    const std::string& theAttribute)
{
  std::shared_ptr<SketchPlugin_Line> aLine;
  if (!theData)
    return aLine;

  std::shared_ptr<ModelAPI_AttributeRefAttr> anAttr = std::dynamic_pointer_cast<
      ModelAPI_AttributeRefAttr>(theData->attribute(theAttribute));
  if (anAttr) {
    FeaturePtr aFeature = ModelAPI_Feature::feature(anAttr->object());
    if (aFeature && aFeature->getKind() == SketchPlugin_Line::ID()) {
      aLine = std::dynamic_pointer_cast<SketchPlugin_Line>(aFeature);
    }
  }
  return aLine;
}

//*************************************************************************************
std::shared_ptr<GeomAPI_Pnt2d> getProjectionPoint(
    const std::shared_ptr<SketchPlugin_Line>& theLine,
    const std::shared_ptr<GeomAPI_Pnt2d>& thePoint)
{
  std::shared_ptr<ModelAPI_Data> aData = theLine->data();
  std::shared_ptr<GeomDataAPI_Point2D> aPoint1 = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
      aData->attribute(SketchPlugin_Line::START_ID()));
  std::shared_ptr<GeomDataAPI_Point2D> aPoint2 = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
      aData->attribute(SketchPlugin_Line::END_ID()));

  GeomAPI_Lin2d aLin2d(aPoint1->x(), aPoint1->y(), aPoint2->x(), aPoint2->y());
  return aLin2d.project(thePoint);
}
