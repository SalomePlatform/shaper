// File:    SketchPlugin_ConstraintCoincidence.cpp
// Created: 08 May 2014
// Author:  Artem ZHIDKOV

#include "SketchPlugin_ConstraintCoincidence.h"

#include <ModelAPI_AttributeDouble.h>
#include <ModelAPI_Data.h>
#include <SketchPlugin_Point.h>

SketchPlugin_ConstraintCoincidence::SketchPlugin_ConstraintCoincidence()
{
}

void SketchPlugin_ConstraintCoincidence::initAttributes()
{
  data()->addAttribute(CONSTRAINT_ATTR_ENTITY_A, ModelAPI_AttributeRefAttr::type());
  data()->addAttribute(CONSTRAINT_ATTR_ENTITY_B, ModelAPI_AttributeRefAttr::type());
}

void SketchPlugin_ConstraintCoincidence::execute()
{
}

const boost::shared_ptr<GeomAPI_Shape>&  SketchPlugin_ConstraintCoincidence::preview()
{
  /// \todo Preview for point coincidence
  return getPreview();
}

