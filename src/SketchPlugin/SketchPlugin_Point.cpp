// File:    SketchPlugin_Point.cpp
// Created: 07 May 2014
// Author:  Artem ZHIDKOV

#include "SketchPlugin_Point.h"
#include "SketchPlugin_Sketch.h"
#include <ModelAPI_Data.h>
#include <GeomDataAPI_Point2D.h>
#include <GeomAlgoAPI_PointBuilder.h>

using namespace std;

SketchPlugin_Point::SketchPlugin_Point()
{
}

void SketchPlugin_Point::initAttributes()
{
  data()->addAttribute(POINT_ATTR_COORD, GeomDataAPI_Point2D::type());
}

void SketchPlugin_Point::execute() 
{
}

const boost::shared_ptr<GeomAPI_Shape>& SketchPlugin_Point::preview()
{
  SketchPlugin_Sketch* aSketch = sketch();
  if (aSketch) {
    // compute a point in 3D view
    boost::shared_ptr<GeomDataAPI_Point2D> aPoint = 
      boost::dynamic_pointer_cast<GeomDataAPI_Point2D>(data()->attribute(POINT_ATTR_COORD));
    boost::shared_ptr<GeomAPI_Pnt> aPoint3D(aSketch->to3D(aPoint->x(), aPoint->y()));
    // make a visible point
    boost::shared_ptr<GeomAPI_Shape> aPointShape = GeomAlgoAPI_PointBuilder::point(aPoint3D);
    setPreview(aPointShape);
  }
  return getPreview();
}
