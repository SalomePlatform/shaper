// File:        SketchPlugin_Arc.cpp
// Created:     26 Apr 2014
// Author:      Artem ZHIDKOV

#include "SketchPlugin_Arc.h"
#include "SketchPlugin_Sketch.h"
#include <ModelAPI_Data.h>

#include <GeomDataAPI_Point2D.h>
#include <GeomDataAPI_Dir.h>

#include <GeomAlgoAPI_PointBuilder.h>
#include <GeomAlgoAPI_EdgeBuilder.h>
#include <GeomAlgoAPI_CompoundBuilder.h>

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
  SketchPlugin_Sketch* aSketch = sketch();
  if (aSketch) {
    std::list<boost::shared_ptr<GeomAPI_Shape> > aShapes;

    // compute a circle point in 3D view
    boost::shared_ptr<GeomDataAPI_Point2D> aCenterAttr = 
      boost::dynamic_pointer_cast<GeomDataAPI_Point2D>(data()->attribute(ARC_ATTR_CENTER));
    boost::shared_ptr<GeomAPI_Pnt> aCenter(aSketch->to3D(aCenterAttr->x(), aCenterAttr->y()));
    // make a visible point
    boost::shared_ptr<GeomAPI_Shape> aCenterPointShape = GeomAlgoAPI_PointBuilder::point(aCenter);
    aShapes.push_back(aCenterPointShape);

    // make a visible circle
    boost::shared_ptr<GeomDataAPI_Dir> aNDir = 
      boost::dynamic_pointer_cast<GeomDataAPI_Dir>(aSketch->data()->attribute(SKETCH_ATTR_NORM));
    bool aHasPlane = aNDir && !(aNDir->x() == 0 && aNDir->y() == 0 && aNDir->z() == 0);
    if (aHasPlane) {
      boost::shared_ptr<GeomAPI_Dir> aNormal(new GeomAPI_Dir(aNDir->x(), aNDir->y(), aNDir->z()));
      // compute the arc start point
      boost::shared_ptr<GeomDataAPI_Point2D> aStartAttr = 
        boost::dynamic_pointer_cast<GeomDataAPI_Point2D>(data()->attribute(ARC_ATTR_START));
      boost::shared_ptr<GeomAPI_Pnt> aStartPoint(aSketch->to3D(aStartAttr->x(), aStartAttr->y()));

      // compute the arc end point
      boost::shared_ptr<GeomDataAPI_Point2D> anEndAttr = 
        boost::dynamic_pointer_cast<GeomDataAPI_Point2D>(data()->attribute(ARC_ATTR_END));
      boost::shared_ptr<GeomAPI_Pnt> aEndPoint(aSketch->to3D(anEndAttr->x(), anEndAttr->y()));

      boost::shared_ptr<GeomAPI_Shape> aCircleShape = 
                 GeomAlgoAPI_EdgeBuilder::lineCircleArc(aCenter, aStartPoint, aEndPoint, aNormal);
      aShapes.push_back(aCircleShape);
    }
    boost::shared_ptr<GeomAPI_Shape> aCompound = GeomAlgoAPI_CompoundBuilder::compound(aShapes);
    setPreview(aCompound);
  }
  return getPreview();
}
