// File:    SketchPlugin_Point.cpp
// Created: 07 May 2014
// Author:  Artem ZHIDKOV

#include "SketchPlugin_Point.h"
#include "SketchPlugin_Sketch.h"

#include <ModelAPI_Data.h>
#include <ModelAPI_ResultConstruction.h>

#include <GeomAPI_Pnt2d.h>

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

void SketchPlugin_Point::execute(boost::shared_ptr<ModelAPI_Result>& theResult) 
{
  SketchPlugin_Sketch* aSketch = sketch();
  if (aSketch) {
    // compute a point in 3D view
    boost::shared_ptr<GeomDataAPI_Point2D> aPoint = 
      boost::dynamic_pointer_cast<GeomDataAPI_Point2D>(data()->attribute(POINT_ATTR_COORD));
    boost::shared_ptr<GeomAPI_Pnt> aPoint3D(aSketch->to3D(aPoint->x(), aPoint->y()));
    // make a visible point
    boost::shared_ptr<GeomAPI_Shape> aPointShape = GeomAlgoAPI_PointBuilder::point(aPoint3D);
    boost::dynamic_pointer_cast<ModelAPI_ResultConstruction>(theResult)->setShape(aPointShape);
  }
}

void SketchPlugin_Point::move(double theDeltaX, double theDeltaY)
{
  boost::shared_ptr<ModelAPI_Data> aData = data();
  if (!aData->isValid())
    return;

  boost::shared_ptr<GeomDataAPI_Point2D> aPoint1 =
        boost::dynamic_pointer_cast<GeomDataAPI_Point2D>(aData->attribute(POINT_ATTR_COORD));
  aPoint1->setValue(aPoint1->x() + theDeltaX, aPoint1->y() + theDeltaY);
}

double SketchPlugin_Point::distanceToPoint(const boost::shared_ptr<GeomAPI_Pnt2d>& thePoint)
{
  boost::shared_ptr<ModelAPI_Data> aData = data();
  boost::shared_ptr<GeomDataAPI_Point2D> aPoint =
        boost::dynamic_pointer_cast<GeomDataAPI_Point2D>(aData->attribute(POINT_ATTR_COORD));

  return aPoint->pnt()->distance(thePoint);
}
