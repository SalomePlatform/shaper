// File:    SketchPlugin_ConstraintPerpendicular.cpp
// Created: 26 May 2014
// Author:  Artem ZHIDKOV

#include "SketchPlugin_ConstraintPerpendicular.h"

#include <ModelAPI_AttributeDouble.h>
#include <ModelAPI_Data.h>
#include <SketchPlugin_Point.h>

SketchPlugin_ConstraintPerpendicular::SketchPlugin_ConstraintPerpendicular()
{
}

void SketchPlugin_ConstraintPerpendicular::initAttributes()
{
  data()->addAttribute(CONSTRAINT_ATTR_ENTITY_A, ModelAPI_AttributeRefAttr::type());
  data()->addAttribute(CONSTRAINT_ATTR_ENTITY_B, ModelAPI_AttributeRefAttr::type());
}

void SketchPlugin_ConstraintPerpendicular::execute()
{
}

const boost::shared_ptr<GeomAPI_Shape>&  SketchPlugin_ConstraintPerpendicular::preview()
{
  /// \todo Preview for perpendicular constraint
  return getPreview();
}

