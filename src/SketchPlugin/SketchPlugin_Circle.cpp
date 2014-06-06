// File:        SketchPlugin_Circle.cpp
// Created:     26 May 2014
// Author:      Artem ZHIDKOV

#include "SketchPlugin_Circle.h"
#include "SketchPlugin_Sketch.h"
#include <ModelAPI_Data.h>
#include <GeomDataAPI_Point2D.h>
#include <GeomAlgoAPI_PointBuilder.h>
#include <GeomAlgoAPI_EdgeBuilder.h>
#include <GeomAlgoAPI_CompoundBuilder.h>
#include <GeomDataAPI_Dir.h>

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
  SketchPlugin_Sketch* aSketch = sketch();
  if (aSketch) {
    // compute a circle point in 3D view
    boost::shared_ptr<GeomDataAPI_Point2D> aCenterAttr = 
      boost::dynamic_pointer_cast<GeomDataAPI_Point2D>(data()->attribute(CIRCLE_ATTR_CENTER));
    boost::shared_ptr<GeomAPI_Pnt> aCenter(aSketch->to3D(aCenterAttr->x(), aCenterAttr->y()));

    // compute the circle radius
    boost::shared_ptr<ModelAPI_AttributeDouble> aRadiusAttr = 
      boost::dynamic_pointer_cast<ModelAPI_AttributeDouble>(data()->attribute(CIRCLE_ATTR_RADIUS));
    double aRadius = aRadiusAttr->value();

    std::list<boost::shared_ptr<GeomAPI_Shape> > aShapes;
    // make a visible point
    boost::shared_ptr<GeomAPI_Shape> aCenterPointShape = GeomAlgoAPI_PointBuilder::point(aCenter);
    aShapes.push_back(aCenterPointShape);

    // make a visible circle
    boost::shared_ptr<GeomDataAPI_Dir> aNDir = 
      boost::dynamic_pointer_cast<GeomDataAPI_Dir>(aSketch->data()->attribute(SKETCH_ATTR_NORM));
    bool aHasPlane = aNDir && !(aNDir->x() == 0 && aNDir->y() == 0 && aNDir->z() == 0);
    if (aHasPlane) {
      boost::shared_ptr<GeomAPI_Dir> aNormal(new GeomAPI_Dir(aNDir->x(), aNDir->y(), aNDir->z()));

      boost::shared_ptr<GeomAPI_Shape> aCircleShape = 
                              GeomAlgoAPI_EdgeBuilder::lineCircle(aCenter, aNormal, aRadius);
      aShapes.push_back(aCircleShape);

      boost::shared_ptr<GeomAPI_Shape> aCompound = GeomAlgoAPI_CompoundBuilder::compound(aShapes);
      setPreview(aCompound);
    }
  }
  /// \todo Implement preview for the circle
  return getPreview();
}
