// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:    SketchPlugin_ConstraintRigid.cpp
// Created: 13 Oct 2014
// Author:  Artem ZHIDKOV

#include "SketchPlugin_ConstraintRigid.h"
#include "SketchPlugin_ConstraintParallel.h"

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

  std::shared_ptr<ModelAPI_Data> aData = data();
  std::shared_ptr<ModelAPI_AttributeRefAttr> anAttr = std::dynamic_pointer_cast<
      ModelAPI_AttributeRefAttr>(aData->attribute(SketchPlugin_Constraint::ENTITY_A()));
  if (!anAttr /*|| !anAttr->isObject()*/)
    return thePrevious;

  std::shared_ptr<GeomAPI_Shape> aShape;

  if (anAttr->isObject()) {
    std::shared_ptr<ModelAPI_ResultConstruction> aConst;
    aConst = std::dynamic_pointer_cast<ModelAPI_ResultConstruction>(anAttr->object());

    if (!aConst) 
      return thePrevious;

    std::shared_ptr<SketchPlugin_Feature> aSketchFea = 
      std::dynamic_pointer_cast<SketchPlugin_Feature>(ModelAPI_Feature::feature(aConst));
    if (aSketchFea.get() != NULL) {
      if (aSketchFea->isExternal())
        return thePrevious;
    }
    aShape = aConst->shape();
  }
  else {
    std::shared_ptr<GeomDataAPI_Point2D> aPointAttr =
                             std::dynamic_pointer_cast<GeomDataAPI_Point2D>(anAttr->attr());
    if (!aPointAttr)
      return thePrevious;
    std::shared_ptr<GeomAPI_Pnt> aPoint(sketch()->to3D(aPointAttr->x(), aPointAttr->y()));
    aShape = GeomAlgoAPI_PointBuilder::point(aPoint);
  }

  AISObjectPtr anAIS = thePrevious;
  if (!anAIS)
    anAIS = AISObjectPtr(new GeomAPI_AISObject);

  std::shared_ptr<GeomAPI_Pln> aPlane = sketch()->plane();
  anAIS->createFixed(aShape, aPlane);

  // Set color from preferences
  std::vector<int> aRGB = Config_PropManager::color("Visualization", "sketch_constraint_color",
                                                    SKETCH_CONSTRAINT_COLOR);

  anAIS->setColor(aRGB[0], aRGB[1], aRGB[2]);

  return anAIS;
}