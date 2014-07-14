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

SketchPlugin_ConstraintPerpendicular::SketchPlugin_ConstraintPerpendicular()
{
}

void SketchPlugin_ConstraintPerpendicular::initAttributes()
{
  data()->addAttribute(CONSTRAINT_ATTR_ENTITY_A, ModelAPI_AttributeRefAttr::type());
  data()->addAttribute(CONSTRAINT_ATTR_ENTITY_B, ModelAPI_AttributeRefAttr::type());
  data()->addAttribute(CONSTRAINT_ATTR_FLYOUT_VALUE_PNT, GeomDataAPI_Point2D::type());
}

void SketchPlugin_ConstraintPerpendicular::execute()
{
}

boost::shared_ptr<GeomAPI_AISObject> SketchPlugin_ConstraintPerpendicular::getAISObject(
                    boost::shared_ptr<GeomAPI_AISObject> thePrevious)
{
  if (!sketch())
    return thePrevious;

  boost::shared_ptr<ModelAPI_Data> aData = data();
  boost::shared_ptr<ModelAPI_AttributeRefAttr> anAttr1 = 
    boost::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(aData->attribute(CONSTRAINT_ATTR_ENTITY_A));
  boost::shared_ptr<ModelAPI_AttributeRefAttr> anAttr2 = 
    boost::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(aData->attribute(CONSTRAINT_ATTR_ENTITY_B));
  if (!anAttr1 || !anAttr1->isObject() || 
      !anAttr2 || !anAttr2->isObject())
    return thePrevious;
  boost::shared_ptr<SketchPlugin_Line> aLine1Feature = 
    boost::dynamic_pointer_cast<SketchPlugin_Line>(anAttr1->object());
  boost::shared_ptr<SketchPlugin_Line> aLine2Feature = 
    boost::dynamic_pointer_cast<SketchPlugin_Line>(anAttr2->object());
  if (!aLine1Feature || !aLine2Feature)
    return thePrevious;

  boost::shared_ptr<GeomAPI_Pln> aPlane = sketch()->plane();
  boost::shared_ptr<GeomAPI_Shape> aLine1, aLine2;
  boost::shared_ptr<ModelAPI_ResultConstruction> aConst1 = 
    boost::dynamic_pointer_cast<ModelAPI_ResultConstruction>(aLine1Feature->firstResult());
  if (aConst1) aLine1 = aConst1->shape();
  boost::shared_ptr<ModelAPI_ResultConstruction> aConst2 = 
    boost::dynamic_pointer_cast<ModelAPI_ResultConstruction>(aLine1Feature->firstResult());
  if (aConst2) aLine2 = aConst2->shape();

  boost::shared_ptr<GeomAPI_AISObject> anAIS = thePrevious;
  if (!anAIS)
    anAIS = boost::shared_ptr<GeomAPI_AISObject>(new GeomAPI_AISObject);
  anAIS->createPerpendicular(aLine1, aLine2, aPlane);
  return anAIS;
}

void SketchPlugin_ConstraintPerpendicular::move(double theDeltaX, double theDeltaY)
{
  boost::shared_ptr<ModelAPI_Data> aData = data();
  if (!aData->isValid())
    return;

  boost::shared_ptr<GeomDataAPI_Point2D> aPoint =
        boost::dynamic_pointer_cast<GeomDataAPI_Point2D>(aData->attribute(CONSTRAINT_ATTR_FLYOUT_VALUE_PNT));
  aPoint->setValue(aPoint->x() + theDeltaX, aPoint->y() + theDeltaY);
}

