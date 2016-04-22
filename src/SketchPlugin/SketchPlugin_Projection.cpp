// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:    SketchPlugin_Projection.cpp
// Created: 07 May 2014
// Author:  Artem ZHIDKOV

#include <SketchPlugin_Projection.h>

#include <SketchPlugin_Arc.h>
#include <SketchPlugin_Circle.h>
#include <SketchPlugin_Line.h>
#include <SketchPlugin_Sketch.h>

#include <ModelAPI_AttributeRefAttr.h>
#include <ModelAPI_AttributeSelection.h>
#include <ModelAPI_AttributeDouble.h>
#include <ModelAPI_ResultConstruction.h>
#include <ModelAPI_Session.h>
#include <ModelAPI_Validator.h>

#include <GeomAPI_Circ.h>
#include <GeomAPI_Edge.h>
#include <GeomAPI_Lin.h>
#include <GeomAPI_Pnt.h>
#include <GeomAPI_Pnt2d.h>
#include <GeomAlgoAPI_EdgeBuilder.h>
#include <GeomDataAPI_Point2D.h>

#include <cmath>

static const double tolerance = 1.e-7;

SketchPlugin_Projection::SketchPlugin_Projection()
    : SketchPlugin_SketchEntity(),
      myIsComputing(false)
{
}

void SketchPlugin_Projection::initDerivedClassAttributes()
{
  data()->addAttribute(EXTERNAL_FEATURE_ID(), ModelAPI_AttributeSelection::typeId());
  data()->addAttribute(PROJECTED_FEATURE_ID(), ModelAPI_AttributeRefAttr::typeId());
  data()->attribute(PROJECTED_FEATURE_ID())->setIsArgument(false);

  data()->addAttribute(EXTERNAL_ID(), ModelAPI_AttributeSelection::typeId());
  ModelAPI_Session::get()->validators()->registerNotObligatory(getKind(), EXTERNAL_ID());
}

void SketchPlugin_Projection::execute()
{
  AttributeRefAttrPtr aRefAttr = data()->refattr(PROJECTED_FEATURE_ID());
  if (!aRefAttr || !aRefAttr->isInitialized())
    return;
  FeaturePtr aProjection = ModelAPI_Feature::feature(aRefAttr->object());

  if (!lastResult()) {
    ResultConstructionPtr aConstr = document()->createConstruction(data());
    aConstr->setShape(std::shared_ptr<GeomAPI_Edge>());
    aConstr->setIsInHistory(false);
    setResult(aConstr);

    aProjection->selection(EXTERNAL_ID())->setValue(lastResult(), lastResult()->shape());
  }
}

void SketchPlugin_Projection::move(double theDeltaX, double theDeltaY)
{
  // feature cannot be moved
}

void SketchPlugin_Projection::attributeChanged(const std::string& theID)
{
  if ((theID == EXTERNAL_FEATURE_ID() || theID == EXTERNAL_ID()) && !myIsComputing) {
    myIsComputing = true;
    computeProjection(theID);
    myIsComputing = false;
  }
}

void SketchPlugin_Projection::computeProjection(const std::string& theID)
{
  AttributeSelectionPtr aExtFeature =
      std::dynamic_pointer_cast<ModelAPI_AttributeSelection>(attribute(EXTERNAL_FEATURE_ID()));

  std::shared_ptr<GeomAPI_Edge> anEdge;
  if (aExtFeature && aExtFeature->value() && aExtFeature->value()->isEdge()) {
    anEdge = std::shared_ptr<GeomAPI_Edge>(new GeomAPI_Edge(aExtFeature->value()));
  } else if (aExtFeature->context() && aExtFeature->context()->shape() && aExtFeature->context()->shape()->isEdge()) {
    anEdge = std::shared_ptr<GeomAPI_Edge>(new GeomAPI_Edge(aExtFeature->context()->shape()));
  }
  if (!anEdge.get())
    return;

  AttributeRefAttrPtr aRefAttr = data()->refattr(PROJECTED_FEATURE_ID());
  FeaturePtr aProjection;
  if (aRefAttr && aRefAttr->isInitialized())
    aProjection = ModelAPI_Feature::feature(aRefAttr->object());

  // if the type of feature differs with already selected, remove it and create once again
  if (aProjection) {
    if ((anEdge->isLine() && aProjection->getKind() != SketchPlugin_Line::ID()) ||
        (anEdge->isCircle() && aProjection->getKind() != SketchPlugin_Circle::ID()) ||
        (anEdge->isArc() && aProjection->getKind() != SketchPlugin_Arc::ID())) {
      DocumentPtr aDoc = sketch()->document();
      aDoc->removeFeature(aProjection);
      aProjection = FeaturePtr();
      aRefAttr->setObject(aProjection);
    }
  }

  std::shared_ptr<GeomAPI_Pln> aSketchPlane = sketch()->plane();

  if (anEdge->isLine()) {
    std::shared_ptr<GeomAPI_Pnt> aFirst = aSketchPlane->project(anEdge->firstPoint());
    std::shared_ptr<GeomAPI_Pnt> aLast = aSketchPlane->project(anEdge->lastPoint());

    std::shared_ptr<GeomAPI_Pnt2d> aFirstInSketch = sketch()->to2D(aFirst);
    std::shared_ptr<GeomAPI_Pnt2d> aLastInSketch = sketch()->to2D(aLast);
    if (aFirstInSketch->distance(aLastInSketch) < tolerance)
      return; // line is semi-orthogonal to the sketch plane

    if (!aProjection)
      aProjection = sketch()->addFeature(SketchPlugin_Line::ID());

    // update attributes of projection
    std::shared_ptr<GeomDataAPI_Point2D> aStartPnt = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
        aProjection->attribute(SketchPlugin_Line::START_ID()));
    std::shared_ptr<GeomDataAPI_Point2D> aEndPnt = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
        aProjection->attribute(SketchPlugin_Line::END_ID()));
    aStartPnt->setValue(aFirstInSketch);
    aEndPnt->setValue(aLastInSketch);
  }
  else if (anEdge->isCircle()) {
    std::shared_ptr<GeomAPI_Circ> aCircle = anEdge->circle();
    double aRadius = aCircle->radius();

    std::shared_ptr<GeomAPI_Pnt> aCenter = aSketchPlane->project(aCircle->center());
    std::shared_ptr<GeomAPI_Pnt2d> aCenterInSketch = sketch()->to2D(aCenter);

    if (!aProjection)
      aProjection = sketch()->addFeature(SketchPlugin_Circle::ID());

    // update attributes of projection
    std::shared_ptr<GeomDataAPI_Point2D> aCenterPnt = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
        aProjection->attribute(SketchPlugin_Circle::CENTER_ID()));
    aCenterPnt->setValue(aCenterInSketch);
    aProjection->real(SketchPlugin_Circle::RADIUS_ID())->setValue(aRadius);
  }
  else if (anEdge->isArc()) {
    std::shared_ptr<GeomAPI_Pnt> aFirst = aSketchPlane->project(anEdge->firstPoint());
    std::shared_ptr<GeomAPI_Pnt> aLast = aSketchPlane->project(anEdge->lastPoint());
    std::shared_ptr<GeomAPI_Pnt2d> aFirstInSketch = sketch()->to2D(aFirst);
    std::shared_ptr<GeomAPI_Pnt2d> aLastInSketch = sketch()->to2D(aLast);

    std::shared_ptr<GeomAPI_Circ> aCircle = anEdge->circle();
    std::shared_ptr<GeomAPI_Pnt> aCenter = aSketchPlane->project(aCircle->center());
    std::shared_ptr<GeomAPI_Pnt2d> aCenterInSketch = sketch()->to2D(aCenter);

    if (!aProjection)
      aProjection = sketch()->addFeature(SketchPlugin_Arc::ID());

    // update attributes of projection
    std::shared_ptr<GeomDataAPI_Point2D> aCenterPnt = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
        aProjection->attribute(SketchPlugin_Arc::CENTER_ID()));
    std::shared_ptr<GeomDataAPI_Point2D> aStartPnt = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
        aProjection->attribute(SketchPlugin_Arc::START_ID()));
    std::shared_ptr<GeomDataAPI_Point2D> aEndPnt = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
        aProjection->attribute(SketchPlugin_Arc::END_ID()));
    aStartPnt->setValue(aFirstInSketch);
    aEndPnt->setValue(aLastInSketch);
    aCenterPnt->setValue(aCenterInSketch);
  }

  aProjection->execute();
  aRefAttr->setObject(aProjection);

  if (theID == EXTERNAL_FEATURE_ID())
    selection(EXTERNAL_ID())->setValue(aExtFeature->context(), aExtFeature->context()->shape());
}
