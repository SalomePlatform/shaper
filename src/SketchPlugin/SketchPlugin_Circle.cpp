// File:        SketchPlugin_Circle.cpp
// Created:     26 May 2014
// Author:      Artem ZHIDKOV

#include "SketchPlugin_Circle.h"
#include "SketchPlugin_Sketch.h"
#include <ModelAPI_Data.h>
#include <ModelAPI_ResultConstruction.h>
#include <ModelAPI_AttributeSelection.h>
#include <ModelAPI_Validator.h>
#include <ModelAPI_AttributeDouble.h>
#include <ModelAPI_Session.h>

#include <GeomAPI_Pnt2d.h>
#include <GeomAPI_Circ.h>
#include <GeomDataAPI_Point2D.h>
#include <GeomDataAPI_Dir.h>
#include <GeomAlgoAPI_PointBuilder.h>
#include <GeomAlgoAPI_EdgeBuilder.h>
#include <GeomAlgoAPI_CompoundBuilder.h>

SketchPlugin_Circle::SketchPlugin_Circle()
    : SketchPlugin_Feature()
{
}

void SketchPlugin_Circle::initAttributes()
{
  data()->addAttribute(CENTER_ID(), GeomDataAPI_Point2D::type());
  data()->addAttribute(RADIUS_ID(), ModelAPI_AttributeDouble::type());
  data()->addAttribute(EXTERNAL_ID(), ModelAPI_AttributeSelection::type());
  ModelAPI_Session::get()->validators()->registerNotObligatory(getKind(), EXTERNAL_ID());
}

void SketchPlugin_Circle::execute()
{
  SketchPlugin_Sketch* aSketch = sketch();
  if (aSketch) {
    std::list<boost::shared_ptr<GeomAPI_Shape> > aShapes;

    // compute a circle point in 3D view
    boost::shared_ptr<GeomDataAPI_Point2D> aCenterAttr = boost::dynamic_pointer_cast<
        GeomDataAPI_Point2D>(data()->attribute(CENTER_ID()));
    AttributeDoublePtr aRadiusAttr = 
      boost::dynamic_pointer_cast<ModelAPI_AttributeDouble>(data()->attribute(RADIUS_ID()));
    if (aCenterAttr->isInitialized() && aRadiusAttr->isInitialized()) {
      boost::shared_ptr<GeomAPI_Pnt> aCenter(aSketch->to3D(aCenterAttr->x(), aCenterAttr->y()));
      // make a visible point
      boost::shared_ptr<GeomAPI_Shape> aCenterPointShape = GeomAlgoAPI_PointBuilder::point(aCenter);
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
        boost::shared_ptr<GeomAPI_Dir> aNormal(new GeomAPI_Dir(aNDir->x(), aNDir->y(), aNDir->z()));
        // compute the circle radius
        double aRadius = aRadiusAttr->value();

        boost::shared_ptr<GeomAPI_Shape> aCircleShape = GeomAlgoAPI_EdgeBuilder::lineCircle(
            aCenter, aNormal, aRadius);
        aShapes.push_back(aCircleShape);
        boost::shared_ptr<ModelAPI_ResultConstruction> aConstr2 = document()->createConstruction(
            data(), 1);
        aConstr2->setShape(aCircleShape);
        aConstr2->setIsInHistory(false);
        setResult(aConstr2, 1);
      }
    }
  }
}

void SketchPlugin_Circle::move(double theDeltaX, double theDeltaY)
{
  boost::shared_ptr<ModelAPI_Data> aData = data();
  if (!aData->isValid())
    return;

  boost::shared_ptr<GeomDataAPI_Point2D> aPoint1 = boost::dynamic_pointer_cast<GeomDataAPI_Point2D>(
      aData->attribute(CENTER_ID()));
  aPoint1->move(theDeltaX, theDeltaY);
}

double SketchPlugin_Circle::distanceToPoint(const boost::shared_ptr<GeomAPI_Pnt2d>& thePoint)
{
  boost::shared_ptr<ModelAPI_Data> aData = data();
  boost::shared_ptr<GeomDataAPI_Point2D> aPoint = 
    boost::dynamic_pointer_cast<GeomDataAPI_Point2D>(aData->attribute(CENTER_ID()));

  return aPoint->pnt()->distance(thePoint);
}

bool SketchPlugin_Circle::isFixed() {
  return data()->selection(EXTERNAL_ID())->context();
}

void SketchPlugin_Circle::attributeChanged() {
  static bool myIsUpdated = false; // to avoid infinitive cycle on attrubtes change
  boost::shared_ptr<GeomAPI_Shape> aSelection = data()->selection(EXTERNAL_ID())->value();
  // update arguments due to the selection value
  if (aSelection && !aSelection->isNull() && aSelection->isEdge() && !myIsUpdated) {
    myIsUpdated = true;
    boost::shared_ptr<GeomAPI_Edge> anEdge( new GeomAPI_Edge(aSelection));
    boost::shared_ptr<GeomAPI_Circ> aCirc = anEdge->circle();
    boost::shared_ptr<GeomDataAPI_Point2D> aCenterAttr = 
      boost::dynamic_pointer_cast<GeomDataAPI_Point2D>(attribute(CENTER_ID()));
    aCenterAttr->setValue(sketch()->to2D(aCirc->center()));
    real(RADIUS_ID())->setValue(aCirc->radius());
    myIsUpdated = false;
  }
}
