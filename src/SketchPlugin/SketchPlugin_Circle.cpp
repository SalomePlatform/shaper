// File:        SketchPlugin_Circle.cpp
// Created:     26 May 2014
// Author:      Artem ZHIDKOV

#include "SketchPlugin_Circle.h"
#include "SketchPlugin_Sketch.h"
#include <ModelAPI_Data.h>
#include <GeomDataAPI_Point2D.h>
#include <ModelAPI_AttributeDouble.h>

SketchPlugin_Circle::SketchPlugin_Circle()
  : SketchPlugin_Feature()
{
}

void SketchPlugin_Circle::initAttributes()
{
  data()->addAttribute(CIRCLE_ATTR_CENTER, GeomDataAPI_Point2D::type());
  data()->addAttribute(CIRCLE_ATTR_RADIUS, ModelAPI_AttributeDouble::type());
}

void SketchPlugin_Circle::execute()
{
}

const boost::shared_ptr<GeomAPI_Shape>& SketchPlugin_Circle::preview()
{
  /// \todo Implement preview for the circle
  return getPreview();
}
