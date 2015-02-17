// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

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
  data()->addAttribute(SketchPlugin_Constraint::ENTITY_A(), ModelAPI_AttributeRefAttr::typeId());
  data()->addAttribute(SketchPlugin_Constraint::ENTITY_B(), ModelAPI_AttributeRefAttr::typeId());
}

void SketchPlugin_ConstraintPerpendicular::execute()
{
}

AISObjectPtr SketchPlugin_ConstraintPerpendicular::getAISObject(AISObjectPtr thePrevious)
{
  if (!sketch())
    return thePrevious;

  std::shared_ptr<ModelAPI_Data> aData = data();
  std::shared_ptr<ModelAPI_AttributeRefAttr> anAttr1 = std::dynamic_pointer_cast<
      ModelAPI_AttributeRefAttr>(aData->attribute(SketchPlugin_Constraint::ENTITY_A()));
  std::shared_ptr<ModelAPI_AttributeRefAttr> anAttr2 = std::dynamic_pointer_cast<
      ModelAPI_AttributeRefAttr>(aData->attribute(SketchPlugin_Constraint::ENTITY_B()));
  if (!anAttr1 || !anAttr1->isObject() || !anAttr2 || !anAttr2->isObject())
    return thePrevious;

  FeaturePtr aFeature = ModelAPI_Feature::feature(anAttr1->object());
  if (!aFeature)
    return thePrevious;
  std::shared_ptr<SketchPlugin_Line> aLine1Feature =
      std::dynamic_pointer_cast<SketchPlugin_Line>(aFeature);

  aFeature = ModelAPI_Feature::feature(anAttr2->object());
  if (!aFeature)
    return thePrevious;
  std::shared_ptr<SketchPlugin_Line> aLine2Feature =
      std::dynamic_pointer_cast<SketchPlugin_Line>(aFeature);

  if (!aLine1Feature || !aLine2Feature)
    return thePrevious;

  std::shared_ptr<GeomAPI_Pln> aPlane = sketch()->plane();
  std::shared_ptr<GeomAPI_Shape> aLine1, aLine2;
  std::shared_ptr<ModelAPI_ResultConstruction> aConst1 = std::dynamic_pointer_cast<
      ModelAPI_ResultConstruction>(anAttr1->object());
  if (aConst1)
    aLine1 = aConst1->shape();
  std::shared_ptr<ModelAPI_ResultConstruction> aConst2 = std::dynamic_pointer_cast<
      ModelAPI_ResultConstruction>(anAttr2->object());
  if (aConst2)
    aLine2 = aConst2->shape();

  AISObjectPtr anAIS = thePrevious;
  if (!anAIS)
    anAIS = AISObjectPtr(new GeomAPI_AISObject);
  anAIS->createPerpendicular(aLine1, aLine2, aPlane);

  // Set color from preferences
  std::vector<int> aRGB = Config_PropManager::color("Visualization", "perpendicular_color",
                                                    PERPENDICULAR_COLOR);
  anAIS->setColor(aRGB[0], aRGB[1], aRGB[2]);
  return anAIS;
}

void SketchPlugin_ConstraintPerpendicular::move(double theDeltaX, double theDeltaY)
{
  //Flyout point of the constraint follows it's features
  return;
}

