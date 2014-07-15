// File:    SketchPlugin_Point.cpp
// Created: 07 May 2014
// Author:  Artem ZHIDKOV

#include "SketchPlugin_Point.h"
#include "SketchPlugin_Sketch.h"

#include <ModelAPI_Data.h>

#include <GeomAPI_Pnt2d.h>

#include <GeomDataAPI_Point2D.h>
#include <GeomAlgoAPI_PointBuilder.h>

using namespace std;

SketchPlugin_Point::SketchPlugin_Point()
{
}

void SketchPlugin_Point::initAttributes()
{
  data()->addAttribute(SketchPlugin_Point::COORD_ID(), GeomDataAPI_Point2D::type());
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
      boost::dynamic_pointer_cast<GeomDataAPI_Point2D>(data()->attribute(SketchPlugin_Point::COORD_ID()));
    boost::shared_ptr<GeomAPI_Pnt> aPoint3D(aSketch->to3D(aPoint->x(), aPoint->y()));
    // make a visible point
    boost::shared_ptr<GeomAPI_Shape> aPointShape = GeomAlgoAPI_PointBuilder::point(aPoint3D);
    setPreview(aPointShape);
  }
  return getPreview();
}

boost::shared_ptr<GeomAPI_AISObject> SketchPlugin_Point::getAISObject(
                                boost::shared_ptr<GeomAPI_AISObject> thePrevious)
{
  return prepareAISShape(thePrevious);
}

void SketchPlugin_Point::move(double theDeltaX, double theDeltaY)
{
  boost::shared_ptr<ModelAPI_Data> aData = data();
  if (!aData->isValid())
    return;

  boost::shared_ptr<GeomDataAPI_Point2D> aPoint1 =
        boost::dynamic_pointer_cast<GeomDataAPI_Point2D>(aData->attribute(SketchPlugin_Point::COORD_ID()));
  aPoint1->setValue(aPoint1->x() + theDeltaX, aPoint1->y() + theDeltaY);
}

double SketchPlugin_Point::distanceToPoint(const boost::shared_ptr<GeomAPI_Pnt2d>& thePoint)
{
  boost::shared_ptr<ModelAPI_Data> aData = data();
  boost::shared_ptr<GeomDataAPI_Point2D> aPoint =
        boost::dynamic_pointer_cast<GeomDataAPI_Point2D>(aData->attribute(SketchPlugin_Point::COORD_ID()));

  return aPoint->pnt()->distance(thePoint);
}
