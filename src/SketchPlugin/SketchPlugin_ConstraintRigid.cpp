// File:    SketchPlugin_ConstraintRigid.cpp
// Created: 13 Oct 2014
// Author:  Artem ZHIDKOV

#include "SketchPlugin_ConstraintRigid.h"

#include <ModelAPI_ResultConstruction.h>

SketchPlugin_ConstraintRigid::SketchPlugin_ConstraintRigid()
{
}

void SketchPlugin_ConstraintRigid::initAttributes()
{
  data()->addAttribute(SketchPlugin_Constraint::ENTITY_A(), ModelAPI_AttributeRefAttr::type());
}

void SketchPlugin_ConstraintRigid::execute()
{
}

AISObjectPtr SketchPlugin_ConstraintRigid::getAISObject(AISObjectPtr thePrevious)
{
  if (!sketch())
    return thePrevious;

  boost::shared_ptr<ModelAPI_Data> aData = data();
  boost::shared_ptr<ModelAPI_AttributeRefAttr> anAttr = boost::dynamic_pointer_cast<
      ModelAPI_AttributeRefAttr>(aData->attribute(SketchPlugin_Constraint::ENTITY_A()));
  if (!anAttr || !anAttr->isObject())
    return thePrevious;

  boost::shared_ptr<ModelAPI_ResultConstruction> aConst = 
    boost::dynamic_pointer_cast<ModelAPI_ResultConstruction>(anAttr->object());
  if (!aConst) 
    return thePrevious;

  boost::shared_ptr<GeomAPI_Shape> aShape;
  aShape = aConst->shape();

  AISObjectPtr anAIS = thePrevious;
  if (!anAIS)
    anAIS = AISObjectPtr(new GeomAPI_AISObject);

  boost::shared_ptr<GeomAPI_Pln> aPlane = sketch()->plane();
  anAIS->createFixed(aShape, aPlane);

  // Set color from preferences
  //std::vector<int> aRGB = Config_PropManager::color("Visualization", "perpendicular_color",
  //                                                  PERPENDICULAR_COLOR);
  //anAIS->setColor(aRGB[0], aRGB[1], aRGB[2]);

  return anAIS;
}