// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:    SketchPlugin_IntersectionPoint.cpp
// Created: 07 May 2014
// Author:  Artem ZHIDKOV

#include "SketchPlugin_IntersectionPoint.h"

#include <ModelAPI_AttributeRefAttr.h>

#include <GeomAPI_Edge.h>
#include <GeomAPI_Lin.h>
#include <GeomDataAPI_Point2D.h>

SketchPlugin_IntersectionPoint::SketchPlugin_IntersectionPoint()
    : SketchPlugin_Point()
{
}

void SketchPlugin_IntersectionPoint::initDerivedClassAttributes()
{
  data()->addAttribute(EXTERNAL_LINE_ID(), ModelAPI_AttributeRefAttr::typeId());

  SketchPlugin_Point::initDerivedClassAttributes();
}

void SketchPlugin_IntersectionPoint::execute()
{
  SketchPlugin_Sketch* aSketch = sketch();
  if (aSketch) {
    computePoint();
    SketchPlugin_Point::execute();

    // set this feature as external
    data()->selection(EXTERNAL_ID())->setValue(lastResult(), lastResult()->shape());
  }
}

void SketchPlugin_IntersectionPoint::move(double theDeltaX, double theDeltaY)
{
}

void SketchPlugin_IntersectionPoint::attributeChanged(const std::string& theID)
{
  if (theID == EXTERNAL_LINE_ID()) {
    // compute intersection between line and sketch plane
    computePoint();
  }
}

void SketchPlugin_IntersectionPoint::computePoint()
{
  AttributeRefAttrPtr aLineRefAttr =
      std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(attribute(EXTERNAL_LINE_ID()));
  ResultPtr aLineResult = std::dynamic_pointer_cast<ModelAPI_Result>(aLineRefAttr->object());
  if (!aLineResult)
    return;

  std::shared_ptr<GeomAPI_Edge> aLinearEdge =
      std::dynamic_pointer_cast<GeomAPI_Edge>(aLineResult->shape());
  std::shared_ptr<GeomAPI_Lin> aLine = aLinearEdge->line();
  std::shared_ptr<GeomAPI_Pln> aSketchPlane = sketch()->plane();

  std::shared_ptr<GeomAPI_Pnt> anIntersection = aSketchPlane->intersect(aLine);
  if (!anIntersection)
    return;

  std::shared_ptr<GeomDataAPI_Point2D> aCoordAttr = 
      std::dynamic_pointer_cast<GeomDataAPI_Point2D>(attribute(COORD_ID()));
  aCoordAttr->setValue(sketch()->to2D(anIntersection));
}
