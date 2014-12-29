// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:        SketchPlugin_Arc.cpp
// Created:     26 Apr 2014
// Author:      Artem ZHIDKOV

#include "SketchPlugin_Arc.h"
#include "SketchPlugin_Sketch.h"
#include <ModelAPI_Data.h>
#include <ModelAPI_ResultConstruction.h>
#include <ModelAPI_AttributeSelection.h>
#include <ModelAPI_Validator.h>
#include <ModelAPI_Session.h>

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
  myStartUpdate = false;
  myEndUpdate = false;
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
  // result for the arc is set only when all obligatory attributes are initialized,
  // otherwise AIS object is used to visualize the arc's preview
  if (aSketch && isFeatureValid()) {
    // compute a circle point in 3D view
    std::shared_ptr<GeomDataAPI_Point2D> aCenterAttr = std::dynamic_pointer_cast<
        GeomDataAPI_Point2D>(data()->attribute(CENTER_ID()));
    // compute the arc start point
    std::shared_ptr<GeomDataAPI_Point2D> aStartAttr = std::dynamic_pointer_cast<
        GeomDataAPI_Point2D>(data()->attribute(START_ID()));

    std::shared_ptr<GeomAPI_Pnt> aCenter(aSketch->to3D(aCenterAttr->x(), aCenterAttr->y()));
    // make a visible point
    std::shared_ptr<GeomAPI_Shape> aCenterPointShape = GeomAlgoAPI_PointBuilder::point(aCenter);
    std::shared_ptr<ModelAPI_ResultConstruction> aConstr1 = document()->createConstruction(
        data(), 0);
    aConstr1->setShape(aCenterPointShape);
    aConstr1->setIsInHistory(false);
    setResult(aConstr1, 0);

    // make a visible circle
    std::shared_ptr<GeomDataAPI_Dir> aNDir = std::dynamic_pointer_cast<GeomDataAPI_Dir>(
        aSketch->data()->attribute(SketchPlugin_Sketch::NORM_ID()));
    std::shared_ptr<GeomAPI_Dir> aNormal = aNDir->dir();
    std::shared_ptr<GeomAPI_Pnt> aStartPoint(aSketch->to3D(aStartAttr->x(), aStartAttr->y()));

    // compute and change the arc end point
    std::shared_ptr<GeomDataAPI_Point2D> anEndAttr = std::dynamic_pointer_cast<
        GeomDataAPI_Point2D>(data()->attribute(END_ID()));
    /* must be automatically done in attributeChanged
    std::shared_ptr<GeomAPI_Circ2d> aCircleForArc(
        new GeomAPI_Circ2d(aCenterAttr->pnt(), aStartAttr->pnt()));
    std::shared_ptr<GeomAPI_Pnt2d> aProjection = aCircleForArc->project(anEndAttr->pnt());
    if (aProjection && anEndAttr->pnt()->distance(aProjection) > tolerance)
      anEndAttr->setValue(aProjection);
    */
    std::shared_ptr<GeomAPI_Pnt> aEndPoint(aSketch->to3D(anEndAttr->x(), anEndAttr->y()));

    std::shared_ptr<GeomAPI_Shape> aCircleShape = GeomAlgoAPI_EdgeBuilder::lineCircleArc(
        aCenter, aStartPoint, aEndPoint, aNormal);
    if (aCircleShape) {
      std::shared_ptr<ModelAPI_ResultConstruction> aConstr2 = document()->createConstruction(
          data(), 1);
      aConstr2->setShape(aCircleShape);
      aConstr2->setIsInHistory(false);
      setResult(aConstr2, 1);
    }
  }
}

AISObjectPtr SketchPlugin_Arc::getAISObject(AISObjectPtr thePrevious)
{
  SketchPlugin_Sketch* aSketch = sketch();
  if (aSketch) {
    // if the feature is valid, the execute() method should be performed, AIS object is empty
    if (!isFeatureValid()) {
      // compute a circle point in 3D view
      std::shared_ptr<GeomDataAPI_Point2D> aCenterAttr = std::dynamic_pointer_cast<
          GeomDataAPI_Point2D>(data()->attribute(CENTER_ID()));
      if (aCenterAttr->isInitialized()) {
        std::shared_ptr<GeomAPI_Pnt> aCenter(aSketch->to3D(aCenterAttr->x(), aCenterAttr->y()));

        std::shared_ptr<GeomDataAPI_Point2D> aStartAttr = std::dynamic_pointer_cast<
            GeomDataAPI_Point2D>(data()->attribute(SketchPlugin_Arc::START_ID()));
        if (aStartAttr->isInitialized()) {
          // make a visible circle
          std::shared_ptr<GeomDataAPI_Dir> aNDir = std::dynamic_pointer_cast<GeomDataAPI_Dir>(
              aSketch->data()->attribute(SketchPlugin_Sketch::NORM_ID()));
          bool aHasPlane = aNDir && !(aNDir->x() == 0 && aNDir->y() == 0 && aNDir->z() == 0);
          if (aHasPlane) {
            std::shared_ptr<GeomAPI_Dir> aNormal = aNDir->dir();
            std::shared_ptr<GeomAPI_Pnt> aStartPoint(aSketch->to3D(aStartAttr->x(), aStartAttr->y()));
            std::shared_ptr<GeomAPI_Shape> aCircleShape = GeomAlgoAPI_EdgeBuilder::lineCircleArc(
                                                            aCenter, aStartPoint, aStartPoint, aNormal);
            if (aCircleShape) {
              std::list<std::shared_ptr<GeomAPI_Shape> > aShapes;
              // make a visible point
              std::shared_ptr<GeomAPI_Shape> aCenterPointShape = GeomAlgoAPI_PointBuilder::point(aCenter);
              aShapes.push_back(aCenterPointShape);

              aShapes.push_back(aCircleShape);
              if (!aShapes.empty())
              {
                std::shared_ptr<GeomAPI_Shape> aCompound = GeomAlgoAPI_CompoundBuilder::compound(aShapes);
                AISObjectPtr anAIS = thePrevious;
                if (!anAIS)
                  anAIS = AISObjectPtr(new GeomAPI_AISObject);
                anAIS->createShape(aCompound);
                anAIS->setWidth(3);
                return anAIS;
              }
            }
          }
        }
      }
    }
  }
  return AISObjectPtr();
}

void SketchPlugin_Arc::move(double theDeltaX, double theDeltaY)
{
  std::shared_ptr<ModelAPI_Data> aData = data();
  if (!aData->isValid())
    return;

  std::shared_ptr<GeomDataAPI_Point2D> aPoint1 = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
      aData->attribute(SketchPlugin_Arc::CENTER_ID()));
  aPoint1->move(theDeltaX, theDeltaY);

  myStartUpdate = true;
  myEndUpdate = true;
  std::shared_ptr<GeomDataAPI_Point2D> aPoint2 = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
      aData->attribute(SketchPlugin_Arc::START_ID()));
  aPoint2->move(theDeltaX, theDeltaY);

  std::shared_ptr<GeomDataAPI_Point2D> aPoint3 = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
      aData->attribute(SketchPlugin_Arc::END_ID()));
  aPoint3->move(theDeltaX, theDeltaY);
  myStartUpdate = false;
  myEndUpdate = false;
}

double SketchPlugin_Arc::distanceToPoint(const std::shared_ptr<GeomAPI_Pnt2d>& thePoint)
{
  double aDelta = 0;
  std::shared_ptr<ModelAPI_Data> aData = data();

  std::shared_ptr<GeomDataAPI_Point2D> aPoint1 = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
      aData->attribute(SketchPlugin_Arc::CENTER_ID()));
  aDelta = aPoint1->pnt()->distance(thePoint);

  std::shared_ptr<GeomDataAPI_Point2D> aPoint2 = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
      aData->attribute(SketchPlugin_Arc::START_ID()));
  double aDistance = aPoint2->pnt()->distance(thePoint);
  if (aDelta < aDistance)
    aDelta = aDistance;

  std::shared_ptr<GeomDataAPI_Point2D> aPoint3 = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
      aData->attribute(SketchPlugin_Arc::END_ID()));
  aDistance = aPoint3->pnt()->distance(thePoint);
  if (aDelta < aDistance)
    aDelta = aDistance;

  return aDelta;
}

bool SketchPlugin_Arc::isFixed() {
  return data()->selection(EXTERNAL_ID())->context();
}

bool SketchPlugin_Arc::isFeatureValid()
{
  std::shared_ptr<GeomDataAPI_Point2D> aCenterAttr = std::dynamic_pointer_cast<
      GeomDataAPI_Point2D>(data()->attribute(SketchPlugin_Arc::CENTER_ID()));
  std::shared_ptr<GeomDataAPI_Point2D> aStartAttr = std::dynamic_pointer_cast<
      GeomDataAPI_Point2D>(data()->attribute(SketchPlugin_Arc::START_ID()));
  std::shared_ptr<GeomDataAPI_Point2D> anEndAttr = std::dynamic_pointer_cast<
      GeomDataAPI_Point2D>(data()->attribute(SketchPlugin_Arc::END_ID()));

  return aCenterAttr->isInitialized() && aStartAttr->isInitialized() && anEndAttr->isInitialized();
}

void SketchPlugin_Arc::attributeChanged(const std::string& theID)
{
  std::shared_ptr<GeomDataAPI_Point2D> aCenterAttr = std::dynamic_pointer_cast<
      GeomDataAPI_Point2D>(data()->attribute(CENTER_ID()));
  if (!aCenterAttr->isInitialized())
    return;
  std::shared_ptr<GeomDataAPI_Point2D> aStartAttr = std::dynamic_pointer_cast<
      GeomDataAPI_Point2D>(data()->attribute(START_ID()));
  if (!aStartAttr->isInitialized())
    return;
  std::shared_ptr<GeomDataAPI_Point2D> anEndAttr = std::dynamic_pointer_cast<
      GeomDataAPI_Point2D>(data()->attribute(END_ID()));
  if (!anEndAttr->isInitialized())
    return;

  // update the points in accordance to the changed point changes
  if (theID == END_ID() && !myEndUpdate) {
    myEndUpdate = true;
    // compute and change the arc end point
    std::shared_ptr<GeomAPI_Circ2d> aCircleForArc(
        new GeomAPI_Circ2d(aCenterAttr->pnt(), aStartAttr->pnt()));
    std::shared_ptr<GeomAPI_Pnt2d> aProjection = aCircleForArc->project(anEndAttr->pnt());
    if (aProjection && anEndAttr->pnt()->distance(aProjection) > tolerance)
      anEndAttr->setValue(aProjection);
    myEndUpdate = false;
  } else if (theID == START_ID() && !myStartUpdate) {
    myStartUpdate = true;
    // compute and change the arc end point
    std::shared_ptr<GeomAPI_Circ2d> aCircleForArc(
        new GeomAPI_Circ2d(aCenterAttr->pnt(), anEndAttr->pnt()));
    std::shared_ptr<GeomAPI_Pnt2d> aProjection = aCircleForArc->project(aStartAttr->pnt());
    if (aProjection && aStartAttr->pnt()->distance(aProjection) > tolerance)
      aStartAttr->setValue(aProjection);
    myStartUpdate = false;
  } else if (theID == CENTER_ID() && !myEndUpdate) {
    myEndUpdate = true;
    // compute and change the arc end point
    std::shared_ptr<GeomAPI_Circ2d> aCircleForArc(
        new GeomAPI_Circ2d(aCenterAttr->pnt(), aStartAttr->pnt()));
    std::shared_ptr<GeomAPI_Pnt2d> aProjection = aCircleForArc->project(anEndAttr->pnt());
    if (aProjection && anEndAttr->pnt()->distance(aProjection) > tolerance)
      anEndAttr->setValue(aProjection);
    myEndUpdate = false;
  }
}
