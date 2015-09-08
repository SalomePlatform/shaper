// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:    SketchPlugin_ConstraintCoincidence.cpp
// Created: 08 May 2014
// Author:  Artem ZHIDKOV

#include "SketchPlugin_ConstraintCoincidence.h"

#include <SketchPlugin_Arc.h>
#include <SketchPlugin_Circle.h>
#include <SketchPlugin_Line.h>
#include <SketchPlugin_Point.h>

#include <SketcherPrs_Factory.h>

#include <ModelAPI_AttributeDouble.h>
#include <ModelAPI_Data.h>
#include <GeomDataAPI_Dir.h>
#include <GeomDataAPI_Point.h>
#include <GeomDataAPI_Point2D.h>

#include <GeomAPI_Circ2d.h>
#include <GeomAPI_Dir2d.h>
#include <GeomAPI_Lin2d.h>

/// \brief Update coordinates of the point to be correctly placed on the feature
static void adjustPointOnEntity(std::shared_ptr<GeomDataAPI_Point2D> thePoint, FeaturePtr theFeature);

SketchPlugin_ConstraintCoincidence::SketchPlugin_ConstraintCoincidence()
{
}

void SketchPlugin_ConstraintCoincidence::initAttributes()
{
  data()->addAttribute(SketchPlugin_Constraint::ENTITY_A(), ModelAPI_AttributeRefAttr::typeId());
  data()->addAttribute(SketchPlugin_Constraint::ENTITY_B(), ModelAPI_AttributeRefAttr::typeId());
}

void SketchPlugin_ConstraintCoincidence::execute()
{
}

AISObjectPtr SketchPlugin_ConstraintCoincidence::getAISObject(AISObjectPtr thePrevious)
{
  if (!sketch())
    return thePrevious;

  AISObjectPtr anAIS = thePrevious;
  if (!anAIS) {
    anAIS = SketcherPrs_Factory::coincidentConstraint(this, sketch()->coordinatePlane());
  }
  return anAIS;
}

void SketchPlugin_ConstraintCoincidence::attributeChanged(const std::string& theID)
{
  if (theID == ENTITY_A() || theID == ENTITY_B()) {
    AttributeRefAttrPtr anAttrA = 
        std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(attribute(ENTITY_A()));
    AttributeRefAttrPtr anAttrB = 
        std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(attribute(ENTITY_B()));

    if (!anAttrA || !anAttrB || (!anAttrA->isObject() && !anAttrB->isObject()))
      return;

    if (anAttrA->isObject()) {
      AttributeRefAttrPtr aTransient = anAttrA;
      anAttrA = anAttrB;
      anAttrB = aTransient;
    }

    std::shared_ptr<GeomDataAPI_Point2D> aPoint =
        std::dynamic_pointer_cast<GeomDataAPI_Point2D>(anAttrA->attr());
    FeaturePtr aFeature;
    if (anAttrB->object())
      aFeature = ModelAPI_Feature::feature(anAttrB->object());

    // Adjust position of a point lying on a line or arc
    if (aPoint && aFeature)
      adjustPointOnEntity(aPoint, aFeature);
  }
}




// =======   Auxiliary functions   ================================================
void adjustPointOnEntity(std::shared_ptr<GeomDataAPI_Point2D> thePoint, FeaturePtr theFeature)
{
  if (theFeature->getKind() == SketchPlugin_Line::ID()) {
    // project point on line
    std::shared_ptr<GeomDataAPI_Point2D> aStart = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
        theFeature->attribute(SketchPlugin_Line::START_ID()));
    std::shared_ptr<GeomDataAPI_Point2D> aEnd = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
        theFeature->attribute(SketchPlugin_Line::END_ID()));

    std::shared_ptr<GeomAPI_Lin2d> aLine(new GeomAPI_Lin2d(aStart->pnt(), aEnd->pnt()));
    std::shared_ptr<GeomAPI_Pnt2d> aProjected = aLine->project(thePoint->pnt());
    thePoint->setValue(aProjected);
  }
  else if (theFeature->getKind() == SketchPlugin_Circle::ID()) {
    // project point on circle
    std::shared_ptr<GeomDataAPI_Point2D> aCenter = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
        theFeature->attribute(SketchPlugin_Circle::CENTER_ID()));
    double aRadius = theFeature->real(SketchPlugin_Circle::RADIUS_ID())->value();

    std::shared_ptr<GeomAPI_Dir2d> aDir(new GeomAPI_Dir2d(1.0, 0.0));
    std::shared_ptr<GeomAPI_Circ2d> aCircle(new GeomAPI_Circ2d(aCenter->pnt(), aDir, aRadius));

    std::shared_ptr<GeomAPI_Pnt2d> aProjected = aCircle->project(thePoint->pnt());
    thePoint->setValue(aProjected);
  }
  else if (theFeature->getKind() == SketchPlugin_Arc::ID()) {
    // project point on arc
    std::shared_ptr<GeomDataAPI_Point2D> aCenter = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
        theFeature->attribute(SketchPlugin_Arc::CENTER_ID()));
    std::shared_ptr<GeomDataAPI_Point2D> aStart = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
        theFeature->attribute(SketchPlugin_Arc::START_ID()));

    std::shared_ptr<GeomAPI_Circ2d> aCircle(new GeomAPI_Circ2d(aCenter->pnt(), aStart->pnt()));
    std::shared_ptr<GeomAPI_Pnt2d> aProjected = aCircle->project(thePoint->pnt());
    thePoint->setValue(aProjected);
  }
}
