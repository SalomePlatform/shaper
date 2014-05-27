// File:        SketchPlugin_Arc.cpp
// Created:     26 Apr 2014
// Author:      Artem ZHIDKOV

#include "SketchPlugin_Arc.h"
#include "SketchPlugin_Sketch.h"
#include <ModelAPI_Data.h>
#include <GeomDataAPI_Point2D.h>

SketchPlugin_Arc::SketchPlugin_Arc()
  : SketchPlugin_Feature()
{
}

void SketchPlugin_Arc::initAttributes()
{
  data()->addAttribute(ARC_ATTR_CENTER, GeomDataAPI_Point2D::type());
  data()->addAttribute(ARC_ATTR_START,  GeomDataAPI_Point2D::type());
  data()->addAttribute(ARC_ATTR_END,    GeomDataAPI_Point2D::type());
}

void SketchPlugin_Arc::execute() 
{
}

const boost::shared_ptr<GeomAPI_Shape>& SketchPlugin_Arc::preview()
{
  /// \todo Implement preview for arc of circle
  return getPreview();
}
