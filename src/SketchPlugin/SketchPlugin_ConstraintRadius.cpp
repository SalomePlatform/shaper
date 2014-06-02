// File:    SketchPlugin_ConstraintRadius.cpp
// Created: 26 May 2014
// Author:  Artem ZHIDKOV

#include "SketchPlugin_ConstraintRadius.h"

#include <ModelAPI_AttributeDouble.h>
#include <ModelAPI_Data.h>
#include <SketchPlugin_Point.h>

SketchPlugin_ConstraintRadius::SketchPlugin_ConstraintRadius()
{
}

void SketchPlugin_ConstraintRadius::initAttributes()
{
  data()->addAttribute(CONSTRAINT_ATTR_VALUE,    ModelAPI_AttributeDouble::type());
  data()->addAttribute(CONSTRAINT_ATTR_ENTITY_A, ModelAPI_AttributeRefAttr::type());
}

void SketchPlugin_ConstraintRadius::execute()
{
}

const boost::shared_ptr<GeomAPI_Shape>&  SketchPlugin_ConstraintRadius::preview()
{
  /// \todo Preview for diameter constraint
  return getPreview();
}

