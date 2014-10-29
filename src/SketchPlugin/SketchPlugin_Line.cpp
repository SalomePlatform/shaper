// File:        SketchPlugin_Line.cxx
// Created:     27 Mar 2014
// Author:      Mikhail PONIKAROV

#include "SketchPlugin_Line.h"
#include "SketchPlugin_Sketch.h"
#include <ModelAPI_Data.h>
#include <ModelAPI_ResultConstruction.h>
#include <ModelAPI_AttributeSelection.h>
#include <ModelAPI_Validator.h>
#include <ModelAPI_Session.h>

#include <GeomAPI_Pnt.h>
#include <GeomAPI_Lin2d.h>
#include <GeomAPI_Pnt2d.h>

#include <GeomAlgoAPI_EdgeBuilder.h>
#include <GeomDataAPI_Point2D.h>

using namespace std;

SketchPlugin_Line::SketchPlugin_Line()
    : SketchPlugin_Feature()
{
}

void SketchPlugin_Line::initAttributes()
{
  data()->addAttribute(SketchPlugin_Line::START_ID(), GeomDataAPI_Point2D::type());
  data()->addAttribute(SketchPlugin_Line::END_ID(), GeomDataAPI_Point2D::type());
  data()->addAttribute(EXTERNAL_ID(), ModelAPI_AttributeSelection::type());
  ModelAPI_Session::get()->validators()->registerNotObligatory(getKind(), EXTERNAL_ID());
}

void SketchPlugin_Line::execute()
{
  SketchPlugin_Sketch* aSketch = sketch();
  if (aSketch) {
    // compute a start point in 3D view
    boost::shared_ptr<GeomDataAPI_Point2D> aStartAttr = boost::dynamic_pointer_cast<
        GeomDataAPI_Point2D>(data()->attribute(SketchPlugin_Line::START_ID()));
    // compute an end point in 3D view
    boost::shared_ptr<GeomDataAPI_Point2D> anEndAttr = boost::dynamic_pointer_cast<
        GeomDataAPI_Point2D>(data()->attribute(SketchPlugin_Line::END_ID()));
    if (aStartAttr->isInitialized() && anEndAttr->isInitialized()) {
      boost::shared_ptr<GeomAPI_Pnt> aStart(aSketch->to3D(aStartAttr->x(), aStartAttr->y()));
      boost::shared_ptr<GeomAPI_Pnt> anEnd(aSketch->to3D(anEndAttr->x(), anEndAttr->y()));
      // make linear edge
      boost::shared_ptr<GeomAPI_Shape> anEdge = GeomAlgoAPI_EdgeBuilder::line(aStart, anEnd);
      // store the result
      boost::shared_ptr<ModelAPI_ResultConstruction> aConstr = document()->createConstruction(
          data());
      aConstr->setShape(anEdge);
      aConstr->setIsInHistory(false);
      setResult(aConstr);
    }
  }
}

void SketchPlugin_Line::move(double theDeltaX, double theDeltaY)
{
  boost::shared_ptr<ModelAPI_Data> aData = data();
  if (!aData->isValid())
    return;

  boost::shared_ptr<GeomDataAPI_Point2D> aPoint1 = boost::dynamic_pointer_cast<GeomDataAPI_Point2D>(
      aData->attribute(SketchPlugin_Line::START_ID()));
  aPoint1->move(theDeltaX, theDeltaY);

  boost::shared_ptr<GeomDataAPI_Point2D> aPoint2 = boost::dynamic_pointer_cast<GeomDataAPI_Point2D>(
      aData->attribute(SketchPlugin_Line::END_ID()));
  aPoint2->move(theDeltaX, theDeltaY);
}

double SketchPlugin_Line::distanceToPoint(const boost::shared_ptr<GeomAPI_Pnt2d>& thePoint)
{
  double aDelta = 0;

  boost::shared_ptr<ModelAPI_Data> aData = data();
  boost::shared_ptr<GeomDataAPI_Point2D> aPoint1 = boost::dynamic_pointer_cast<GeomDataAPI_Point2D>(
      aData->attribute(SketchPlugin_Line::START_ID()));
  boost::shared_ptr<GeomDataAPI_Point2D> aPoint2 = boost::dynamic_pointer_cast<GeomDataAPI_Point2D>(
      aData->attribute(SketchPlugin_Line::END_ID()));

  GeomAPI_Lin2d aLin2d(aPoint1->x(), aPoint1->y(), aPoint2->x(), aPoint2->y());

  if (false/*projection*/) {  // TODO: if it has not been necessary, remove this block
    boost::shared_ptr<GeomAPI_Pnt2d> aResult = aLin2d.project(thePoint);
    aDelta = aResult->distance(thePoint);
  } else {  // distance
    aDelta = aLin2d.distance(thePoint);
  }

  return aDelta;
}

bool SketchPlugin_Line::isFixed() {
  return data()->selection(EXTERNAL_ID())->context();
}
