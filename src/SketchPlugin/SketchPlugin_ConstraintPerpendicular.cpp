// File:    SketchPlugin_ConstraintPerpendicular.cpp
// Created: 26 May 2014
// Author:  Artem ZHIDKOV

#include "SketchPlugin_ConstraintPerpendicular.h"

#include <ModelAPI_AttributeDouble.h>
#include <ModelAPI_Data.h>
#include <ModelAPI_ResultConstruction.h>

#include <SketchPlugin_Line.h>
#include <SketchPlugin_Sketch.h>

#include <GeomDataAPI_Point2D.h>
#include <GeomAPI_Pnt2d.h>
#include <GeomAPI_Pnt.h>

#include <Config_PropManager.h>

SketchPlugin_ConstraintPerpendicular::SketchPlugin_ConstraintPerpendicular()
{
}

void SketchPlugin_ConstraintPerpendicular::initAttributes()
{
  data()->addAttribute(SketchPlugin_Constraint::ENTITY_A(), ModelAPI_AttributeRefAttr::type());
  data()->addAttribute(SketchPlugin_Constraint::ENTITY_B(), ModelAPI_AttributeRefAttr::type());
  data()->addAttribute(SketchPlugin_Constraint::FLYOUT_VALUE_PNT(), GeomDataAPI_Point2D::type());
}

void SketchPlugin_ConstraintPerpendicular::execute()
{
}

AISObjectPtr SketchPlugin_ConstraintPerpendicular::getAISObject(AISObjectPtr thePrevious)
{
  if (!sketch())
    return thePrevious;

  boost::shared_ptr<ModelAPI_Data> aData = data();
  boost::shared_ptr<ModelAPI_AttributeRefAttr> anAttr1 = 
    boost::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(aData->attribute(SketchPlugin_Constraint::ENTITY_A()));
  boost::shared_ptr<ModelAPI_AttributeRefAttr> anAttr2 = 
    boost::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(aData->attribute(SketchPlugin_Constraint::ENTITY_B()));
  if (!anAttr1 || !anAttr1->isObject() || 
      !anAttr2 || !anAttr2->isObject())
    return thePrevious;

  FeaturePtr aFeature = ModelAPI_Feature::feature(anAttr1->object());
  if (!aFeature)
    return thePrevious;
  boost::shared_ptr<SketchPlugin_Line> aLine1Feature = 
    boost::dynamic_pointer_cast<SketchPlugin_Line>(aFeature);

  aFeature = ModelAPI_Feature::feature(anAttr2->object());
  if (!aFeature)
    return thePrevious;
  boost::shared_ptr<SketchPlugin_Line> aLine2Feature = 
    boost::dynamic_pointer_cast<SketchPlugin_Line>(aFeature);

  if (!aLine1Feature || !aLine2Feature)
    return thePrevious;

  boost::shared_ptr<GeomAPI_Pln> aPlane = sketch()->plane();
  boost::shared_ptr<GeomAPI_Shape> aLine1, aLine2;
  boost::shared_ptr<ModelAPI_ResultConstruction> aConst1 = 
    boost::dynamic_pointer_cast<ModelAPI_ResultConstruction>(anAttr1->object());
  if (aConst1) aLine1 = aConst1->shape();
  boost::shared_ptr<ModelAPI_ResultConstruction> aConst2 = 
    boost::dynamic_pointer_cast<ModelAPI_ResultConstruction>(anAttr2->object());
  if (aConst2) aLine2 = aConst2->shape();

  AISObjectPtr anAIS = thePrevious;
  if (!anAIS)
    anAIS = AISObjectPtr(new GeomAPI_AISObject);
  anAIS->createPerpendicular(aLine1, aLine2, aPlane);

  // Set color from preferences
  std::vector<int> aRGB = Config_PropManager::color("Visualization", "perpendicular_color", PERPENDICULAR_COLOR);
  anAIS->setColor(aRGB[0], aRGB[1], aRGB[2]);
  return anAIS;
}

void SketchPlugin_ConstraintPerpendicular::move(double theDeltaX, double theDeltaY)
{
  boost::shared_ptr<ModelAPI_Data> aData = data();
  if (!aData->isValid())
    return;

  boost::shared_ptr<GeomDataAPI_Point2D> aPoint =
        boost::dynamic_pointer_cast<GeomDataAPI_Point2D>(aData->attribute(SketchPlugin_Constraint::FLYOUT_VALUE_PNT()));
  aPoint->setValue(aPoint->x() + theDeltaX, aPoint->y() + theDeltaY);
}

