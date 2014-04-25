// File:        SketchPlugin_Line.cxx
// Created:     27 Mar 2014
// Author:      Mikhail PONIKAROV

#include "SketchPlugin_Line.h"
#include "SketchPlugin_Sketch.h"
#include <ModelAPI_Data.h>
#include <GeomAPI_Pnt.h>
#include <GeomAlgoAPI_EdgeBuilder.h>
#include <GeomDataAPI_Point2D.h>

using namespace std;

// face of the square-face displayed for selection of general plane
const double PLANE_SIZE = 200;

SketchPlugin_Line::SketchPlugin_Line()
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

  return getPreview();
}
