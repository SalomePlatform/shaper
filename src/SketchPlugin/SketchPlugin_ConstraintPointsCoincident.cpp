// File:    SketchPlugin_ConstraintPointsCoincident.cpp
// Created: 08 May 2014
// Author:  Artem ZHIDKOV

#include "SketchPlugin_ConstraintPointsCoincident.h"

#include <ModelAPI_AttributeDouble.h>
#include <ModelAPI_Data.h>
#include <SketchPlugin_Point.h>

SketchPlugin_ConstraintPointsCoincident::SketchPlugin_ConstraintPointsCoincident()
{
}

void SketchPlugin_ConstraintPointsCoincident::initAttributes()
{
  data()->addAttribute(CONSTRAINT_ATTR_POINT_A, ModelAPI_AttributeRefAttr::type());
  data()->addAttribute(CONSTRAINT_ATTR_POINT_B, ModelAPI_AttributeRefAttr::type());
}

void SketchPlugin_ConstraintPointsCoincident::execute()
{
}

const boost::shared_ptr<GeomAPI_Shape>&  SketchPlugin_ConstraintPointsCoincident::preview()
{
  /// \todo Preview for point coincidence
  return getPreview();
}

