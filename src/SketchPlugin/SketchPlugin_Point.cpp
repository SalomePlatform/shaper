// File:    SketchPlugin_Point.cpp
// Created: 07 May 2014
// Author:  Artem ZHIDKOV

#include "SketchPlugin_Point.h"
#include "SketchPlugin_Sketch.h"
#include <ModelAPI_Data.h>
////#include <GeomAPI_Pnt.h>
////#include <GeomAlgoAPI_EdgeBuilder.h>
#include <GeomDataAPI_Point2D.h>

using namespace std;

////// face of the square-face displayed for selection of general plane
////const double PLANE_SIZE = 200;

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
  // compute a point in 3D view
  boost::shared_ptr<GeomDataAPI_Point2D> aPoint = 
    boost::dynamic_pointer_cast<GeomDataAPI_Point2D>(data()->attribute(POINT_ATTR_COORD));
  boost::shared_ptr<GeomAPI_Pnt> aPoint3D(aSketch->to3D(aPoint->x(), aPoint->y()));
  // make a visible point
  //boost::shared_ptr<GeomAPI_Shape> anEdge = GeomAlgoAPI_EdgeBuilder::line(aStart, anEnd);
  //setPreview(anEdge);

  return getPreview();
}
