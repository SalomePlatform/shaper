// File:        SketchPlugin_Arc.cpp
// Created:     26 Apr 2014
// Author:      Artem ZHIDKOV

#include "SketchPlugin_Arc.h"
#include "SketchPlugin_Sketch.h"
#include <ModelAPI_Data.h>
#include <ModelAPI_ResultConstruction.h>
#include <ModelAPI_AttributeSelection.h>
#include <ModelAPI_Validator.h>

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
  data()->addAttribute(CENTER_ID(), GeomDataAPI_Point2D::type());
  data()->addAttribute(START_ID(), GeomDataAPI_Point2D::type());
  data()->addAttribute(END_ID(), GeomDataAPI_Point2D::type());
  data()->addAttribute(EXTERNAL_ID(), ModelAPI_AttributeSelection::type());
  ModelAPI_Session::get()->validators()->registerNotObligatory(getKind(), EXTERNAL_ID());
}

void SketchPlugin_Arc::execute()
{
  SketchPlugin_Sketch* aSketch = sketch();
  if (aSketch) {
    std::list<boost::shared_ptr<GeomAPI_Shape> > aShapes;

    // compute a circle point in 3D view
    boost::shared_ptr<GeomDataAPI_Point2D> aCenterAttr = boost::dynamic_pointer_cast<
        GeomDataAPI_Point2D>(data()->attribute(SketchPlugin_Arc::CENTER_ID()));
    // compute the arc start point
    boost::shared_ptr<GeomDataAPI_Point2D> aStartAttr = boost::dynamic_pointer_cast<
        GeomDataAPI_Point2D>(data()->attribute(SketchPlugin_Arc::START_ID()));
    if (aCenterAttr->isInitialized() && aStartAttr->isInitialized()) {
      boost::shared_ptr<GeomAPI_Pnt> aCenter(aSketch->to3D(aCenterAttr->x(), aCenterAttr->y()));
      // make a visible point
      boost::shared_ptr<GeomAPI_Shape> aCenterPointShape = GeomAlgoAPI_PointBuilder::point(aCenter);
      //aShapes.push_back(aCenterPointShape);
      boost::shared_ptr<ModelAPI_ResultConstruction> aConstr1 = document()->createConstruction(
          data(), 0);
      aConstr1->setShape(aCenterPointShape);
      aConstr1->setIsInHistory(false);
      setResult(aConstr1, 0);

      // make a visible circle
      boost::shared_ptr<GeomDataAPI_Dir> aNDir = boost::dynamic_pointer_cast<GeomDataAPI_Dir>(
          aSketch->data()->attribute(SketchPlugin_Sketch::NORM_ID()));
      bool aHasPlane = aNDir && !(aNDir->x() == 0 && aNDir->y() == 0 && aNDir->z() == 0);
      if (aHasPlane) {
        boost::shared_ptr<GeomAPI_Dir> aNormal = aNDir->dir();
        boost::shared_ptr<GeomAPI_Pnt> aStartPoint(aSketch->to3D(aStartAttr->x(), aStartAttr->y()));

        // compute and change the arc end point
        boost::shared_ptr<GeomDataAPI_Point2D> anEndAttr = boost::dynamic_pointer_cast<
            GeomDataAPI_Point2D>(data()->attribute(SketchPlugin_Arc::END_ID()));
        if (anEndAttr->isInitialized()) {
          boost::shared_ptr<GeomAPI_Circ2d> aCircleForArc(
              new GeomAPI_Circ2d(aCenterAttr->pnt(), aStartAttr->pnt()));
          boost::shared_ptr<GeomAPI_Pnt2d> aProjection = aCircleForArc->project(anEndAttr->pnt());
          if (aProjection && anEndAttr->pnt()->distance(aProjection) > tolerance)
            anEndAttr->setValue(aProjection);
        }
        boost::shared_ptr<GeomAPI_Pnt> aEndPoint(aSketch->to3D(anEndAttr->x(), anEndAttr->y()));

        boost::shared_ptr<GeomAPI_Shape> aCircleShape = GeomAlgoAPI_EdgeBuilder::lineCircleArc(
            aCenter, aStartPoint, aEndPoint, aNormal);
        if (aCircleShape) {
          boost::shared_ptr<ModelAPI_ResultConstruction> aConstr2 = document()->createConstruction(
              data(), 1);
          aConstr2->setShape(aCircleShape);
          aConstr2->setIsInHistory(false);
          setResult(aConstr2, 1);
          //aShapes.push_back(aCircleShape);
        }
      }
      /*
       boost::shared_ptr<GeomAPI_Shape> aCompound = GeomAlgoAPI_CompoundBuilder::compound(aShapes);
       // store the result
       boost::shared_ptr<ModelAPI_ResultConstruction> aConstr = 
       document()->createConstruction(data());
       aConstr->setShape(aCompound);
       aConstr->setIsInHistory(false);
       setResult(aConstr);
       */
    }
  }
}

void SketchPlugin_Arc::move(double theDeltaX, double theDeltaY)
{
  boost::shared_ptr<ModelAPI_Data> aData = data();
  if (!aData->isValid())
    return;

  boost::shared_ptr<GeomDataAPI_Point2D> aPoint1 = boost::dynamic_pointer_cast<GeomDataAPI_Point2D>(
      aData->attribute(SketchPlugin_Arc::CENTER_ID()));
  aPoint1->setValue(aPoint1->x() + theDeltaX, aPoint1->y() + theDeltaY);

  boost::shared_ptr<GeomDataAPI_Point2D> aPoint2 = boost::dynamic_pointer_cast<GeomDataAPI_Point2D>(
      aData->attribute(SketchPlugin_Arc::START_ID()));
  aPoint2->setValue(aPoint2->x() + theDeltaX, aPoint2->y() + theDeltaY);

  boost::shared_ptr<GeomDataAPI_Point2D> aPoint3 = boost::dynamic_pointer_cast<GeomDataAPI_Point2D>(
      aData->attribute(SketchPlugin_Arc::END_ID()));
  aPoint3->setValue(aPoint3->x() + theDeltaX, aPoint3->y() + theDeltaY);
}

double SketchPlugin_Arc::distanceToPoint(const boost::shared_ptr<GeomAPI_Pnt2d>& thePoint)
{
  double aDelta = 0;
  boost::shared_ptr<ModelAPI_Data> aData = data();

  boost::shared_ptr<GeomDataAPI_Point2D> aPoint1 = boost::dynamic_pointer_cast<GeomDataAPI_Point2D>(
      aData->attribute(SketchPlugin_Arc::CENTER_ID()));
  aDelta = aPoint1->pnt()->distance(thePoint);

  boost::shared_ptr<GeomDataAPI_Point2D> aPoint2 = boost::dynamic_pointer_cast<GeomDataAPI_Point2D>(
      aData->attribute(SketchPlugin_Arc::START_ID()));
  double aDistance = aPoint2->pnt()->distance(thePoint);
  if (aDelta < aDistance)
    aDelta = aDistance;

  boost::shared_ptr<GeomDataAPI_Point2D> aPoint3 = boost::dynamic_pointer_cast<GeomDataAPI_Point2D>(
      aData->attribute(SketchPlugin_Arc::END_ID()));
  aDistance = aPoint3->pnt()->distance(thePoint);
  if (aDelta < aDistance)
    aDelta = aDistance;

  return aDelta;
}

bool SketchPlugin_Arc::isFixed() {
  return data()->selection(EXTERNAL_ID())->context();
}
