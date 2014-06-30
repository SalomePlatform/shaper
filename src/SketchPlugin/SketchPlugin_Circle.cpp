// File:        SketchPlugin_Circle.cpp
// Created:     26 May 2014
// Author:      Artem ZHIDKOV

#include "SketchPlugin_Circle.h"
#include "SketchPlugin_Sketch.h"
#include <ModelAPI_Data.h>

#include <GeomAPI_Pnt2d.h>

#include <GeomDataAPI_Point2D.h>
#include <GeomDataAPI_Dir.h>

#include <GeomAlgoAPI_PointBuilder.h>
#include <GeomAlgoAPI_EdgeBuilder.h>
#include <GeomAlgoAPI_CompoundBuilder.h>

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
    std::list<boost::shared_ptr<GeomAPI_Shape> > aShapes;

    // compute a circle point in 3D view
    boost::shared_ptr<GeomDataAPI_Point2D> aCenterAttr = 
      boost::dynamic_pointer_cast<GeomDataAPI_Point2D>(data()->attribute(CIRCLE_ATTR_CENTER));
    AttributeDoublePtr aRadiusAttr = 
      boost::dynamic_pointer_cast<ModelAPI_AttributeDouble>(data()->attribute(CIRCLE_ATTR_RADIUS));
    if (aCenterAttr->isInitialized() && aRadiusAttr->isInitialized()) {
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
        // compute the circle radius
        double aRadius = aRadiusAttr->value();

        boost::shared_ptr<GeomAPI_Shape> aCircleShape = 
                                GeomAlgoAPI_EdgeBuilder::lineCircle(aCenter, aNormal, aRadius);
        aShapes.push_back(aCircleShape);
      }
    }
    boost::shared_ptr<GeomAPI_Shape> aCompound = GeomAlgoAPI_CompoundBuilder::compound(aShapes);
    setPreview(aCompound);
  }
  return getPreview();
}

void SketchPlugin_Circle::move(double theDeltaX, double theDeltaY)
{
  boost::shared_ptr<ModelAPI_Data> aData = data();
  if (!aData->isValid())
    return;

  boost::shared_ptr<GeomDataAPI_Point2D> aPoint1 =
        boost::dynamic_pointer_cast<GeomDataAPI_Point2D>(aData->attribute(CIRCLE_ATTR_CENTER));
  aPoint1->setValue(aPoint1->x() + theDeltaX, aPoint1->y() + theDeltaY);
}

double SketchPlugin_Circle::distanceToPoint(const boost::shared_ptr<GeomAPI_Pnt2d>& thePoint)
{
  boost::shared_ptr<ModelAPI_Data> aData = data();
  boost::shared_ptr<GeomDataAPI_Point2D> aPoint =
        boost::dynamic_pointer_cast<GeomDataAPI_Point2D>(aData->attribute(CIRCLE_ATTR_CENTER));

  return aPoint->pnt()->distance(thePoint);
}
