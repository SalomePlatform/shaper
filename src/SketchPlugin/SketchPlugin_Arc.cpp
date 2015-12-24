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

#include <GeomAPI_Ax2.h>
#include <GeomAPI_Circ2d.h>
#include <GeomAPI_Circ.h>
#include <GeomAPI_Pnt2d.h>
#include <GeomDataAPI_Point2D.h>
#include <GeomDataAPI_Dir.h>
#include <GeomAlgoAPI_PointBuilder.h>
#include <GeomAlgoAPI_EdgeBuilder.h>
#include <GeomAlgoAPI_CompoundBuilder.h>
// for sqrt on Linux
#include <math.h>

const double tolerance = 1e-7;
const double paramTolerance = 1.e-4;
const double PI =3.141592653589793238463;


SketchPlugin_Arc::SketchPlugin_Arc()
    : SketchPlugin_SketchEntity()
{
  myStartUpdate = false;
  myEndUpdate = false;
  // default values
  myXEndBefore = 0;
  myYEndBefore = 0;

  myParamBefore = 0;
}

void SketchPlugin_Arc::initDerivedClassAttributes()
{
  data()->addAttribute(CENTER_ID(), GeomDataAPI_Point2D::typeId());
  data()->addAttribute(START_ID(), GeomDataAPI_Point2D::typeId());
  std::shared_ptr<GeomDataAPI_Point2D> anEndAttr = std::dynamic_pointer_cast<
    GeomDataAPI_Point2D>(data()->addAttribute(END_ID(), GeomDataAPI_Point2D::typeId()));
  data()->addAttribute(EXTERNAL_ID(), ModelAPI_AttributeSelection::typeId());
  ModelAPI_Session::get()->validators()->registerNotObligatory(getKind(), EXTERNAL_ID());

  data()->addAttribute(INVERSED_ID(), ModelAPI_AttributeBoolean::typeId());
  AttributeBooleanPtr isInversed =
      std::dynamic_pointer_cast<ModelAPI_AttributeBoolean>(attribute(INVERSED_ID()));
  if (!isInversed->isInitialized())
    isInversed->setValue(false);

  // get the initial values
  if (anEndAttr->isInitialized()) {
    myXEndBefore = anEndAttr->x();
    myYEndBefore = anEndAttr->y();
  }
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
    AttributeBooleanPtr isInversed =
        std::dynamic_pointer_cast<ModelAPI_AttributeBoolean>(attribute(INVERSED_ID()));

    std::shared_ptr<GeomAPI_Dir> anXDir(new GeomAPI_Dir(aStartPoint->xyz()->decreased(aCenter->xyz())));
    std::shared_ptr<GeomAPI_Ax2> anAx2(new GeomAPI_Ax2(aCenter, aNormal, anXDir));
    std::shared_ptr<GeomAPI_Circ> aCirc(new GeomAPI_Circ(anAx2, aCenter->distance(aStartPoint)));
    double aParameterNew = 0.0;
    if(aCirc->parameter(aEndPoint, paramTolerance, aParameterNew)) {
      if(0 <= myParamBefore && myParamBefore <= PI / 2.0
        && PI * 1.5 <= aParameterNew && aParameterNew <= PI * 2.0) {
          isInversed->setValue(true);
      } else if(PI * 1.5 <= myParamBefore && myParamBefore <= PI * 2.0
        && 0 <= aParameterNew && aParameterNew <= PI / 2.0) {
          isInversed->setValue(false);
      }
    }
    myParamBefore = aParameterNew;

    std::shared_ptr<GeomAPI_Shape> aCircleShape;
    if(!isInversed->value()) {
      aCircleShape = GeomAlgoAPI_EdgeBuilder::lineCircleArc(aCenter, aStartPoint, aEndPoint, aNormal);
    } else {
      aCircleShape = GeomAlgoAPI_EdgeBuilder::lineCircleArc(aCenter, aEndPoint, aStartPoint, aNormal);
    }

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

  std::shared_ptr<GeomDataAPI_Point2D> aPoint1 = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
      aData->attribute(SketchPlugin_Arc::CENTER_ID()));
  aPoint1->move(theDeltaX, theDeltaY);
}

bool SketchPlugin_Arc::isFixed() {
  return data()->selection(EXTERNAL_ID())->context().get() != NULL;
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
  std::shared_ptr<GeomDataAPI_Point2D> aStartAttr = std::dynamic_pointer_cast<
      GeomDataAPI_Point2D>(data()->attribute(START_ID()));
  std::shared_ptr<GeomDataAPI_Point2D> anEndAttr = std::dynamic_pointer_cast<
      GeomDataAPI_Point2D>(data()->attribute(END_ID()));
  // the second condition for unability to move external segments anywhere
  if (theID == EXTERNAL_ID() || isFixed()) {
    std::shared_ptr<GeomAPI_Shape> aSelection = data()->selection(EXTERNAL_ID())->value();
    // update arguments due to the selection value
    if (aSelection && !aSelection->isNull() && aSelection->isEdge()) {
      std::shared_ptr<GeomAPI_Edge> anEdge( new GeomAPI_Edge(aSelection));
      std::shared_ptr<GeomAPI_Circ> aCirc = anEdge->circle();
      if (aCirc.get()) {
        aStartAttr->setValue(sketch()->to2D(anEdge->firstPoint()));
        anEndAttr->setValue(sketch()->to2D(anEdge->lastPoint()));
        aCenterAttr->setValue(sketch()->to2D(aCirc->center()));
      }
    }
    return;
  }
  if (!aCenterAttr->isInitialized())
    return;
  if (!aStartAttr->isInitialized())
    return;
  if (!anEndAttr->isInitialized())
    return;

  // update the points in accordance to the changed point changes
  if (theID == END_ID() && !myEndUpdate) {
    myEndUpdate = true;
    // compute and change the arc end point
    std::shared_ptr<GeomAPI_Circ2d> aCircleForArc(
        new GeomAPI_Circ2d(aCenterAttr->pnt(), aStartAttr->pnt()));
    std::shared_ptr<GeomAPI_Pnt2d> aProjection = aCircleForArc->project(anEndAttr->pnt());
    if (aProjection && anEndAttr->pnt()->distance(aProjection) > tolerance) {
      if (!isStable()) { // issue #855: trying to update only not-updated coordinate if it is possible
        if (fabs(myXEndBefore - anEndAttr->x()) < 1.e-10) { // keep Y unchanged
          double aVy = aCenterAttr->y() - anEndAttr->y();
          double aVy2 = aVy * aVy;
          double aR2 = aCircleForArc->radius() * aCircleForArc->radius();
          if (aVy2 <= aR2) {
            double aDX = sqrt(aR2 - aVy * aVy);
            if (anEndAttr->x() > aCenterAttr->x())
              aProjection->setX(aCenterAttr->x() + aDX);
            else 
              aProjection->setX(aCenterAttr->x() - aDX);
            aProjection->setY(anEndAttr->y());
          }
        } else if (fabs(myYEndBefore - anEndAttr->y()) < 1.e-10) { // keep X unchanged
          double aVx = aCenterAttr->x() - anEndAttr->x();
          double aVx2 = aVx * aVx;
          double aR2 = aCircleForArc->radius() * aCircleForArc->radius();
          if (aVx2 <= aR2) {
            double aDY = sqrt(aR2 - aVx * aVx);
            if (anEndAttr->y() > aCenterAttr->y())
              aProjection->setY(aCenterAttr->y() + aDY);
            else 
              aProjection->setY(aCenterAttr->y() - aDY);
            aProjection->setX(anEndAttr->x());
          }
        }
      }

      anEndAttr->setValue(aProjection);
    }
    myXEndBefore = anEndAttr->x();
    myYEndBefore = anEndAttr->y();
    myEndUpdate = false;
  } else if (theID == START_ID() && !myStartUpdate) {
    myStartUpdate = true;
    // compute and change the arc start point
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

void SketchPlugin_Arc::setReversed(bool isReversed)
{
  std::dynamic_pointer_cast<ModelAPI_AttributeBoolean>(attribute(INVERSED_ID()))->setValue(isReversed);
  myParamBefore = 0.0;
}

bool SketchPlugin_Arc::isReversed()
{
  return std::dynamic_pointer_cast<ModelAPI_AttributeBoolean>(attribute(INVERSED_ID()))->value();
}
