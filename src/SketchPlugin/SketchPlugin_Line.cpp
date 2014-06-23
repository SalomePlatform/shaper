// File:        SketchPlugin_Line.cxx
// Created:     27 Mar 2014
// Author:      Mikhail PONIKAROV

#include "SketchPlugin_Line.h"
#include "SketchPlugin_Sketch.h"
#include <ModelAPI_Data.h>

#include <GeomAPI_Pnt.h>
#include <GeomAPI_Lin2d.h>
#include <GeomAPI_Pnt2d.h>

#include <GeomAlgoAPI_EdgeBuilder.h>
#include <GeomDataAPI_Point2D.h>

using namespace std;

// face of the square-face displayed for selection of general plane
const double PLANE_SIZE = 200;

SketchPlugin_Line::SketchPlugin_Line()
  : SketchPlugin_Feature()
{
}

void SketchPlugin_Line::initAttributes()
{
  data()->addAttribute(LINE_ATTR_START, GeomDataAPI_Point2D::type());
  data()->addAttribute(LINE_ATTR_END, GeomDataAPI_Point2D::type());
}

void SketchPlugin_Line::execute() 
{
}

const boost::shared_ptr<GeomAPI_Shape>& SketchPlugin_Line::preview()
{
  SketchPlugin_Sketch* aSketch = sketch();
  if (aSketch) {
    // compute a start point in 3D view
    boost::shared_ptr<GeomDataAPI_Point2D> aStartAttr = 
      boost::dynamic_pointer_cast<GeomDataAPI_Point2D>(data()->attribute(LINE_ATTR_START));
    boost::shared_ptr<GeomAPI_Pnt> aStart(aSketch->to3D(aStartAttr->x(), aStartAttr->y()));
    // compute an end point in 3D view
    boost::shared_ptr<GeomDataAPI_Point2D> anEndAttr = 
      boost::dynamic_pointer_cast<GeomDataAPI_Point2D>(data()->attribute(LINE_ATTR_END));
    boost::shared_ptr<GeomAPI_Pnt> anEnd(aSketch->to3D(anEndAttr->x(), anEndAttr->y()));
    // make linear edge
    boost::shared_ptr<GeomAPI_Shape> anEdge = GeomAlgoAPI_EdgeBuilder::line(aStart, anEnd);
    setPreview(anEdge);
  }
  return getPreview();
}

void SketchPlugin_Line::move(double theDeltaX, double theDeltaY)
{
  boost::shared_ptr<ModelAPI_Data> aData = data();
  if (!aData->isValid())
    return;

  boost::shared_ptr<GeomDataAPI_Point2D> aPoint1 =
        boost::dynamic_pointer_cast<GeomDataAPI_Point2D>(aData->attribute(LINE_ATTR_START));
  aPoint1->setValue(aPoint1->x() + theDeltaX, aPoint1->y() + theDeltaY);

  boost::shared_ptr<GeomDataAPI_Point2D> aPoint2 =
        boost::dynamic_pointer_cast<GeomDataAPI_Point2D>(aData->attribute(LINE_ATTR_END));
  aPoint2->setValue(aPoint2->x() + theDeltaX, aPoint2->y() + theDeltaY);
}

double SketchPlugin_Line::distanceToPoint(const boost::shared_ptr<GeomAPI_Pnt2d>& thePoint)
{
  double aDelta = 0;

  boost::shared_ptr<ModelAPI_Data> aData = data();
  boost::shared_ptr<GeomDataAPI_Point2D> aPoint1 =
        boost::dynamic_pointer_cast<GeomDataAPI_Point2D>(aData->attribute(LINE_ATTR_START));
  boost::shared_ptr<GeomDataAPI_Point2D> aPoint2 =
        boost::dynamic_pointer_cast<GeomDataAPI_Point2D>(aData->attribute(LINE_ATTR_END));

  GeomAPI_Lin2d aLin2d(aPoint1->x(), aPoint1->y(), aPoint2->x(), aPoint2->y());

  if (false/*projection*/) { // TODO: if it has not been necessary, remove this block
    boost::shared_ptr<GeomAPI_Pnt2d> aResult = aLin2d.project(thePoint);
    aDelta = aResult->distance(thePoint);
  }
  else { // distance
    aDelta = aLin2d.distance(thePoint);
  }

  return aDelta;
}
