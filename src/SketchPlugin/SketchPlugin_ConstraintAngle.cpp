// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:    SketchPlugin_ConstraintAngle.cpp
// Created: 19 August 2015
// Author:  Artem ZHIDKOV

#include "SketchPlugin_ConstraintAngle.h"
#include <SketchPlugin_Line.h>

#include <ModelAPI_AttributeDouble.h>
#include <ModelAPI_AttributeInteger.h>

#include <GeomDataAPI_Point2D.h>

#include <GeomAPI_Angle2d.h>
#include <GeomAPI_Dir2d.h>
#include <GeomAPI_Lin2d.h>
#include <GeomAPI_Pnt2d.h>
#include <GeomAPI_XY.h>

#include <SketcherPrs_Factory.h>
#include <SketcherPrs_Tools.h>

#include <math.h>

const double tolerance = 1.e-7;
#define PI 3.1415926535897932

/// \brief Calculate intersection point of two lines
static std::shared_ptr<GeomAPI_Pnt2d> intersect(FeaturePtr theLine1, FeaturePtr theLine2);


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

  data()->addAttribute(SketchPlugin_ConstraintAngle::ANGLE_VALUE_ID(), ModelAPI_AttributeDouble::typeId());
  data()->addAttribute(SketchPlugin_ConstraintAngle::TYPE_ID(), ModelAPI_AttributeInteger::typeId());

  data()->addAttribute(SketchPlugin_ConstraintAngle::ANGLE_REVERSED_FIRST_LINE_ID(), ModelAPI_AttributeBoolean::typeId());
  data()->addAttribute(SketchPlugin_ConstraintAngle::ANGLE_REVERSED_SECOND_LINE_ID(), ModelAPI_AttributeBoolean::typeId());
}

void SketchPlugin_ConstraintAngle::colorConfigInfo(std::string& theSection, std::string& theName,
                                                   std::string& theDefault)
{
  theSection = "Visualization";
  theName = "sketch_dimension_color";
  theDefault = SKETCH_DIMENSION_COLOR;
}

void SketchPlugin_ConstraintAngle::execute()
{
  std::shared_ptr<ModelAPI_Data> aData = data();

  std::shared_ptr<ModelAPI_AttributeRefAttr> anAttrA = aData->refattr(SketchPlugin_Constraint::ENTITY_A());
  std::shared_ptr<ModelAPI_AttributeRefAttr> anAttrB = aData->refattr(SketchPlugin_Constraint::ENTITY_B());
  if (!anAttrA->isInitialized() || !anAttrB->isInitialized())
    return;

  AttributeDoublePtr anAttrValue = std::dynamic_pointer_cast<ModelAPI_AttributeDouble>(
      aData->attribute(SketchPlugin_ConstraintAngle::ANGLE_VALUE_ID()));

  if (!anAttrValue->isInitialized()) {
    double anAngle = calculateAngle();
    anAttrValue->setValue(anAngle);
    updateConstraintValueByAngleValue();
  }
  // the value should to be computed here, not in the getAISObject in order to change the model value
  // inside the object transaction. This is important for creating a constraint by preselection.
  // The display of the presentation in this case happens after the transaction commit
  std::shared_ptr<GeomDataAPI_Point2D> aFlyOutAttr = std::dynamic_pointer_cast<
      GeomDataAPI_Point2D>(aData->attribute(SketchPlugin_Constraint::FLYOUT_VALUE_PNT()));
  if(!aFlyOutAttr->isInitialized())
    compute(SketchPlugin_Constraint::FLYOUT_VALUE_PNT());
}

AISObjectPtr SketchPlugin_ConstraintAngle::getAISObject(AISObjectPtr thePrevious)
{
  if (!sketch())
    return thePrevious;

  AISObjectPtr anAIS = SketcherPrs_Factory::angleConstraint(this, sketch()->coordinatePlane(),
                                                            thePrevious);
  return anAIS;
}

void SketchPlugin_ConstraintAngle::attributeChanged(const std::string& theID)
{
  std::shared_ptr<ModelAPI_Data> aData = data();
  if (!aData)
    return;
  FeaturePtr aLineA = SketcherPrs_Tools::getFeatureLine(aData, SketchPlugin_Constraint::ENTITY_A());
  FeaturePtr aLineB = SketcherPrs_Tools::getFeatureLine(aData, SketchPlugin_Constraint::ENTITY_B());
  if (!aLineA || !aLineB)
    return;

  if (theID == SketchPlugin_Constraint::ENTITY_A() || 
      theID == SketchPlugin_Constraint::ENTITY_B()) {
    std::shared_ptr<ModelAPI_AttributeDouble> aValueAttr = std::dynamic_pointer_cast<
        ModelAPI_AttributeDouble>(data()->attribute(SketchPlugin_ConstraintAngle::ANGLE_VALUE_ID()));
    if (!aValueAttr->isInitialized()) { // only if it is not initialized, try to compute the current value
      double anAngle = calculateAngle();
      aValueAttr->setValue(anAngle);
      updateConstraintValueByAngleValue();
    }
  } else if (theID == SketchPlugin_Constraint::FLYOUT_VALUE_PNT() && !myFlyoutUpdate) {
    // Recalculate flyout point in local coordinates
    // coordinates are calculated according to the center of shapes intersection
    std::shared_ptr<GeomDataAPI_Point2D> aFlyoutAttr =
        std::dynamic_pointer_cast<GeomDataAPI_Point2D>(attribute(SketchPlugin_Constraint::FLYOUT_VALUE_PNT()));

    std::shared_ptr<ModelAPI_Data> aData = data();
    std::shared_ptr<GeomAPI_Ax3> aPlane = SketchPlugin_Sketch::plane(sketch());
    FeaturePtr aLineA = SketcherPrs_Tools::getFeatureLine(aData, SketchPlugin_Constraint::ENTITY_A());
    FeaturePtr aLineB = SketcherPrs_Tools::getFeatureLine(aData, SketchPlugin_Constraint::ENTITY_B());

    // Intersection of lines
    std::shared_ptr<GeomAPI_Pnt2d> anInter = intersect(aLineA, aLineB);
    if (!anInter)
      return;

    myFlyoutUpdate = true;
    std::shared_ptr<GeomAPI_XY> aFlyoutDir = aFlyoutAttr->pnt()->xy()->decreased(anInter->xy());
    if (aFlyoutDir->dot(aFlyoutDir) < tolerance * tolerance)
      aFlyoutAttr->setValue(aFlyoutAttr->x() + tolerance, aFlyoutAttr->y());
    myFlyoutUpdate = false;
  }
  else if (theID == SketchPlugin_ConstraintAngle::TYPE_ID()) {
    std::shared_ptr<ModelAPI_AttributeDouble> aValueAttr = std::dynamic_pointer_cast<
      ModelAPI_AttributeDouble>(data()->attribute(SketchPlugin_ConstraintAngle::ANGLE_VALUE_ID()));
    double anAngle = calculateAngle();
    aValueAttr->setValue(anAngle);
  }
  else if (theID == SketchPlugin_ConstraintAngle::ANGLE_VALUE_ID()) {
    updateConstraintValueByAngleValue();
  }
}

double SketchPlugin_ConstraintAngle::calculateAngle()
{
  std::shared_ptr<ModelAPI_Data> aData = data();
  std::shared_ptr<GeomAPI_Ax3> aPlane = SketchPlugin_Sketch::plane(sketch());
  FeaturePtr aLineA = SketcherPrs_Tools::getFeatureLine(aData, SketchPlugin_Constraint::ENTITY_A());
  FeaturePtr aLineB = SketcherPrs_Tools::getFeatureLine(aData, SketchPlugin_Constraint::ENTITY_B());

  std::shared_ptr<GeomDataAPI_Point2D> aStartA = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
      aLineA->attribute(SketchPlugin_Line::START_ID()));
  std::shared_ptr<GeomDataAPI_Point2D> aEndA = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
      aLineA->attribute(SketchPlugin_Line::END_ID()));
  std::shared_ptr<GeomDataAPI_Point2D> aStartB = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
      aLineB->attribute(SketchPlugin_Line::START_ID()));
  std::shared_ptr<GeomDataAPI_Point2D> aEndB = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
      aLineB->attribute(SketchPlugin_Line::END_ID()));

  std::shared_ptr<GeomAPI_Angle2d> anAng;
  if (!attribute(ANGLE_REVERSED_FIRST_LINE_ID())->isInitialized() ||
      !attribute(ANGLE_REVERSED_SECOND_LINE_ID())->isInitialized())
    anAng = std::shared_ptr<GeomAPI_Angle2d>(new GeomAPI_Angle2d(
        aStartA->pnt(), aEndA->pnt(), aStartB->pnt(), aEndB->pnt()));
  else {
    std::shared_ptr<GeomAPI_Lin2d> aLine1(new GeomAPI_Lin2d(aStartA->pnt(), aEndA->pnt()));
    bool isReversed1 = boolean(ANGLE_REVERSED_FIRST_LINE_ID())->value();
    std::shared_ptr<GeomAPI_Lin2d> aLine2(new GeomAPI_Lin2d(aStartB->pnt(), aEndB->pnt()));
    bool isReversed2 = boolean(ANGLE_REVERSED_SECOND_LINE_ID())->value();
    anAng = std::shared_ptr<GeomAPI_Angle2d>(new GeomAPI_Angle2d(aLine1, isReversed1, aLine2, isReversed2));
  }
  double anAngle = anAng->angleDegree();
  std::shared_ptr<ModelAPI_AttributeDouble> aValueAttr = std::dynamic_pointer_cast<
      ModelAPI_AttributeDouble>(data()->attribute(SketchPlugin_ConstraintAngle::VALUE()));
  if (!aValueAttr->isInitialized())
    aValueAttr->setValue(anAngle);
  /// an angle value should be corrected by the current angle type
  anAngle = getAngleForType(anAngle);
  boolean(ANGLE_REVERSED_FIRST_LINE_ID())->setValue(anAng->isReversed(0));
  boolean(ANGLE_REVERSED_SECOND_LINE_ID())->setValue(anAng->isReversed(1));
  return anAngle;
}

double SketchPlugin_ConstraintAngle::getAngleForType(double theAngle, bool isPreviousValueObtuse)
{
  double anAngle = theAngle;

  std::shared_ptr<ModelAPI_Data> aData = data();
  std::shared_ptr<ModelAPI_AttributeInteger> aTypeAttr = std::dynamic_pointer_cast<
      ModelAPI_AttributeInteger>(aData->attribute(SketchPlugin_ConstraintAngle::TYPE_ID()));
  SketcherPrs_Tools::AngleType anAngleType = (SketcherPrs_Tools::AngleType)(aTypeAttr->value());
  switch (anAngleType) {
    case SketcherPrs_Tools::ANGLE_DIRECT:
      anAngle = theAngle;
    break;
    case SketcherPrs_Tools::ANGLE_COMPLEMENTARY: {
      if (theAngle > 180 || isPreviousValueObtuse)
        anAngle = theAngle - 180.0;
      else
        anAngle = 180.0 - theAngle;
      
      if (anAngle < 0.0)
        anAngle += 360.0;
    }
    break;
    case SketcherPrs_Tools::ANGLE_BACKWARD:
      anAngle = 360.0 - theAngle;
    break;
    default:
      break;
  }
  return anAngle;
}

void SketchPlugin_ConstraintAngle::updateConstraintValueByAngleValue()
{
  std::shared_ptr<ModelAPI_AttributeDouble> aValueAttr = std::dynamic_pointer_cast<
    ModelAPI_AttributeDouble>(data()->attribute(SketchPlugin_ConstraintAngle::ANGLE_VALUE_ID()));
  double anAngle = aValueAttr->value();

  /// an angle value should be corrected by the current angle type
  aValueAttr = std::dynamic_pointer_cast<
                  ModelAPI_AttributeDouble>(data()->attribute(SketchPlugin_Constraint::VALUE()));
  anAngle = getAngleForType(anAngle, aValueAttr->value() > 180.0);
  aValueAttr->setValue(anAngle);
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


bool SketchPlugin_ConstraintAngle::compute(const std::string& theAttributeId)
{
  if (theAttributeId != SketchPlugin_Constraint::FLYOUT_VALUE_PNT())
    return false;
  if (!sketch())
    return false;

  std::shared_ptr<GeomDataAPI_Point2D> aFlyOutAttr = std::dynamic_pointer_cast<
                           GeomDataAPI_Point2D>(attribute(theAttributeId));
  if (fabs(aFlyOutAttr->x()) >= tolerance || fabs(aFlyOutAttr->y()) >= tolerance)
    return false;

  DataPtr aData = data();
  std::shared_ptr<GeomAPI_Ax3> aPlane = SketchPlugin_Sketch::plane(sketch());
  FeaturePtr aLineA = SketcherPrs_Tools::getFeatureLine(aData, SketchPlugin_Constraint::ENTITY_A());
  FeaturePtr aLineB = SketcherPrs_Tools::getFeatureLine(aData, SketchPlugin_Constraint::ENTITY_B());

  if ((aLineA.get() == NULL) || (aLineB.get() == NULL))
    return false;

  // Start and end points of lines
  std::shared_ptr<GeomDataAPI_Point2D> aPointA1 = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
      aLineA->attribute(SketchPlugin_Line::START_ID()));
  std::shared_ptr<GeomDataAPI_Point2D> aPointA2 = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
      aLineA->attribute(SketchPlugin_Line::END_ID()));

  std::shared_ptr<GeomAPI_Pnt2d> aStartA = aPointA1->pnt();
  std::shared_ptr<GeomAPI_Pnt2d> aEndA   = aPointA2->pnt();
  if (aStartA->distance(aEndA) < tolerance)
    return false;

  myFlyoutUpdate = true;
  double aX = (aStartA->x() + aEndA->x()) / 2.;
  double aY = (aStartA->y() + aEndA->y()) / 2.;

  aFlyOutAttr->setValue(aX, aY);
  myFlyoutUpdate = false;

  return true;
}


// ===============   Auxiliary functions   ==================================
std::shared_ptr<GeomAPI_Pnt2d> intersect(FeaturePtr theLine1, FeaturePtr theLine2)
{
  // Start and end points of lines
  std::shared_ptr<GeomDataAPI_Point2D> aPointA1 = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
      theLine1->attribute(SketchPlugin_Line::START_ID()));
  std::shared_ptr<GeomDataAPI_Point2D> aPointA2 = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
      theLine1->attribute(SketchPlugin_Line::END_ID()));

  std::shared_ptr<GeomDataAPI_Point2D> aPointB1 = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
      theLine2->attribute(SketchPlugin_Line::START_ID()));
  std::shared_ptr<GeomDataAPI_Point2D> aPointB2 = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
      theLine2->attribute(SketchPlugin_Line::END_ID()));

  std::shared_ptr<GeomAPI_Pnt2d> aStartA = aPointA1->pnt();
  std::shared_ptr<GeomAPI_Pnt2d> aEndA   = aPointA2->pnt();
  std::shared_ptr<GeomAPI_Pnt2d> aStartB = aPointB1->pnt();
  std::shared_ptr<GeomAPI_Pnt2d> aEndB   = aPointB2->pnt();
  if (aStartA->distance(aEndA) < tolerance || aStartB->distance(aEndB) < tolerance)
    std::shared_ptr<GeomAPI_Pnt2d>();

  // Lines and their intersection point
  std::shared_ptr<GeomAPI_Lin2d> aLA(new GeomAPI_Lin2d(aStartA, aEndA));
  std::shared_ptr<GeomAPI_Lin2d> aLB(new GeomAPI_Lin2d(aStartB, aEndB));
  return aLA->intersect(aLB);
}
