// File:    SketchPlugin_ConstraintRigid.cpp
// Created: 13 Oct 2014
// Author:  Artem ZHIDKOV

#include "SketchPlugin_ConstraintRigid.h"

#include <ModelAPI_ResultConstruction.h>
#include <Config_PropManager.h>

#include <GeomDataAPI_Point2D.h>
#include <GeomAlgoAPI_PointBuilder.h>

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
  if (!anAttr /*|| !anAttr->isObject()*/)
    return thePrevious;

  boost::shared_ptr<GeomAPI_Shape> aShape;

  if (anAttr->isObject()) {
    boost::shared_ptr<ModelAPI_ResultConstruction> aConst;
    aConst = boost::dynamic_pointer_cast<ModelAPI_ResultConstruction>(anAttr->object());

    if (!aConst) 
      return thePrevious;
    aShape = aConst->shape();
  }
  else {
    boost::shared_ptr<GeomDataAPI_Point2D> aPointAttr =
                             boost::dynamic_pointer_cast<GeomDataAPI_Point2D>(anAttr->attr());
    if (!aPointAttr)
      return thePrevious;
    boost::shared_ptr<GeomAPI_Pnt> aPoint(sketch()->to3D(aPointAttr->x(), aPointAttr->y()));
    aShape = GeomAlgoAPI_PointBuilder::point(aPoint);
  }

  AISObjectPtr anAIS = thePrevious;
  if (!anAIS)
    anAIS = AISObjectPtr(new GeomAPI_AISObject);

  boost::shared_ptr<GeomAPI_Pln> aPlane = sketch()->plane();
  anAIS->createFixed(aShape, aPlane);

  // Set color from preferences
  std::vector<int> aRGB = Config_PropManager::color("Visualization", "fixing_color",
                                                    FIXING_COLOR);
  anAIS->setColor(aRGB[0], aRGB[1], aRGB[2]);

  return anAIS;
}