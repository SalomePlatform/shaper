// File:        SketchPlugin_Arc.cpp
// Created:     26 Apr 2014
// Author:      Artem ZHIDKOV

#include "SketchPlugin_Arc.h"
#include "SketchPlugin_Sketch.h"
#include <ModelAPI_Data.h>
#include <ModelAPI_ResultConstruction.h>

#include <GeomAPI_Circ2d.h>
#include <GeomAPI_Pnt2d.h>
#include <GeomDataAPI_Point2D.h>
#include <GeomDataAPI_Dir.h>
#include <GeomAlgoAPI_PointBuilder.h>
#include <GeomAlgoAPI_EdgeBuilder.h>
#include <GeomAlgoAPI_CompoundBuilder.h>

const double tolerance = 1e-7;

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
  SketchPlugin_Sketch* aSketch = sketch();
  if (aSketch) {
    std::list<boost::shared_ptr<GeomAPI_Shape> > aShapes;

    // compute a circle point in 3D view
    boost::shared_ptr<GeomDataAPI_Point2D> aCenterAttr = 
      boost::dynamic_pointer_cast<GeomDataAPI_Point2D>(data()->attribute(ARC_ATTR_CENTER));
      // compute the arc start point
      boost::shared_ptr<GeomDataAPI_Point2D> aStartAttr = 
        boost::dynamic_pointer_cast<GeomDataAPI_Point2D>(data()->attribute(ARC_ATTR_START));
      if (aCenterAttr->isInitialized() && aStartAttr->isInitialized()) {
      boost::shared_ptr<GeomAPI_Pnt> aCenter(aSketch->to3D(aCenterAttr->x(), aCenterAttr->y()));
      // make a visible point
      boost::shared_ptr<GeomAPI_Shape> aCenterPointShape = GeomAlgoAPI_PointBuilder::point(aCenter);
      aShapes.push_back(aCenterPointShape);

      // make a visible circle
      boost::shared_ptr<GeomDataAPI_Dir> aNDir = 
        boost::dynamic_pointer_cast<GeomDataAPI_Dir>(aSketch->data()->attribute(SKETCH_ATTR_NORM));
      bool aHasPlane = aNDir && !(aNDir->x() == 0 && aNDir->y() == 0 && aNDir->z() == 0);
      if (aHasPlane) {
        boost::shared_ptr<GeomAPI_Dir> aNormal = aNDir->dir();
        boost::shared_ptr<GeomAPI_Pnt> aStartPoint(aSketch->to3D(aStartAttr->x(), aStartAttr->y()));

        // compute and change the arc end point
        boost::shared_ptr<GeomDataAPI_Point2D> anEndAttr = 
          boost::dynamic_pointer_cast<GeomDataAPI_Point2D>(data()->attribute(ARC_ATTR_END));
        if (anEndAttr->isInitialized())
        {
          boost::shared_ptr<GeomAPI_Circ2d> aCircleForArc(
            new GeomAPI_Circ2d(aCenterAttr->pnt(), aStartAttr->pnt()));
          boost::shared_ptr<GeomAPI_Pnt2d> aProjection = aCircleForArc->project(anEndAttr->pnt());
          if (aProjection && anEndAttr->pnt()->distance(aProjection) > tolerance)
            anEndAttr->setValue(aProjection);
        }
        boost::shared_ptr<GeomAPI_Pnt> aEndPoint(aSketch->to3D(anEndAttr->x(), anEndAttr->y()));

        boost::shared_ptr<GeomAPI_Shape> aCircleShape = 
                   GeomAlgoAPI_EdgeBuilder::lineCircleArc(aCenter, aStartPoint, aEndPoint, aNormal);
        if (aCircleShape)
          aShapes.push_back(aCircleShape);
      }
      boost::shared_ptr<GeomAPI_Shape> aCompound = GeomAlgoAPI_CompoundBuilder::compound(aShapes);
      // store the result
      boost::shared_ptr<ModelAPI_ResultConstruction> aConstr = 
        document()->createConstruction(data());
      aConstr->setShape(aCompound);
      results().push_back(aConstr);
    }
  }
}

void SketchPlugin_Arc::move(double theDeltaX, double theDeltaY)
{
  boost::shared_ptr<ModelAPI_Data> aData = data();
  if (!aData->isValid())
    return;

  boost::shared_ptr<GeomDataAPI_Point2D> aPoint1 =
        boost::dynamic_pointer_cast<GeomDataAPI_Point2D>(aData->attribute(ARC_ATTR_CENTER));
  aPoint1->setValue(aPoint1->x() + theDeltaX, aPoint1->y() + theDeltaY);

  boost::shared_ptr<GeomDataAPI_Point2D> aPoint2 =
        boost::dynamic_pointer_cast<GeomDataAPI_Point2D>(aData->attribute(ARC_ATTR_START));
  aPoint2->setValue(aPoint2->x() + theDeltaX, aPoint2->y() + theDeltaY);

  boost::shared_ptr<GeomDataAPI_Point2D> aPoint3 =
        boost::dynamic_pointer_cast<GeomDataAPI_Point2D>(aData->attribute(ARC_ATTR_END));
  aPoint3->setValue(aPoint3->x() + theDeltaX, aPoint3->y() + theDeltaY);
}

double SketchPlugin_Arc::distanceToPoint(const boost::shared_ptr<GeomAPI_Pnt2d>& thePoint)
{
  double aDelta = 0;
  boost::shared_ptr<ModelAPI_Data> aData = data();

  boost::shared_ptr<GeomDataAPI_Point2D> aPoint1 =
        boost::dynamic_pointer_cast<GeomDataAPI_Point2D>(aData->attribute(ARC_ATTR_CENTER));
  aDelta = aPoint1->pnt()->distance(thePoint);

  boost::shared_ptr<GeomDataAPI_Point2D> aPoint2 =
        boost::dynamic_pointer_cast<GeomDataAPI_Point2D>(aData->attribute(ARC_ATTR_START));
  double aDistance = aPoint2->pnt()->distance(thePoint);
  if (aDelta < aDistance)
    aDelta = aDistance;

  boost::shared_ptr<GeomDataAPI_Point2D> aPoint3 =
        boost::dynamic_pointer_cast<GeomDataAPI_Point2D>(aData->attribute(ARC_ATTR_END));
  aDistance = aPoint3->pnt()->distance(thePoint);
  if (aDelta < aDistance)
    aDelta = aDistance;

  return aDelta;
}
