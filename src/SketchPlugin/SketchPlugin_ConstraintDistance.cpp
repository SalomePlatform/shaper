// File:    SketchPlugin_ConstraintDistance.cpp
// Created: 23 May 2014
// Author:  Artem ZHIDKOV

#include "SketchPlugin_ConstraintDistance.h"

#include "GeomDataAPI_Point2D.h"

#include <ModelAPI_AttributeDouble.h>
#include <ModelAPI_Data.h>
#include <SketchPlugin_Point.h>

SketchPlugin_ConstraintDistance::SketchPlugin_ConstraintDistance()
{
}

void SketchPlugin_ConstraintDistance::initAttributes()
{
  data()->addAttribute(CONSTRAINT_ATTR_VALUE,    ModelAPI_AttributeDouble::type());
  data()->addAttribute(CONSTRAINT_ATTR_FLYOUT_VALUE_PNT, GeomDataAPI_Point2D::type());
  data()->addAttribute(CONSTRAINT_ATTR_ENTITY_A, ModelAPI_AttributeRefAttr::type());
  data()->addAttribute(CONSTRAINT_ATTR_ENTITY_B, ModelAPI_AttributeRefAttr::type());
}

void SketchPlugin_ConstraintDistance::execute()
{
}

const boost::shared_ptr<GeomAPI_Shape>&  SketchPlugin_ConstraintDistance::preview()
{
  /// \todo Preview for distance constraint
  return getPreview();
}

