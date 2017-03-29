// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:        SketchPlugin_MacroArc.cpp
// Created:     26 Apr 2014
// Author:      Artem ZHIDKOV

#include "SketchPlugin_MacroArc.h"

#include "SketchPlugin_Arc.h"
#include "SketchPlugin_ConstraintTangent.h"
#include "SketchPlugin_Sketch.h"
#include "SketchPlugin_Tools.h"

#include <ModelAPI_AttributeDouble.h>
#include <ModelAPI_AttributeRefAttr.h>
#include <ModelAPI_AttributeString.h>
#include <ModelAPI_Session.h>
#include <ModelAPI_Validator.h>

#include <GeomAPI_Circ.h>
#include <GeomAPI_Circ2d.h>
#include <GeomAPI_Curve.h>
#include <GeomAPI_Dir2d.h>
#include <GeomAPI_Edge.h>
#include <GeomAPI_Lin.h>
#include <GeomAPI_Lin2d.h>
#include <GeomAPI_Pnt2d.h>
#include <GeomAPI_Vertex.h>
#include <GeomAPI_XY.h>

#include <GeomDataAPI_Point2D.h>
#include <GeomDataAPI_Dir.h>
#include <GeomAlgoAPI_PointBuilder.h>
#include <GeomAlgoAPI_EdgeBuilder.h>
#include <GeomAlgoAPI_CompoundBuilder.h>

// for sqrt on Linux
#include <math.h>

const double tolerance = 1e-7;
const double paramTolerance = 1.e-4;
const double PI = 3.141592653589793238463;

static void projectPointOnCircle(AttributePoint2DPtr& thePoint, const GeomAPI_Circ2d& theCircle)
{
  std::shared_ptr<GeomAPI_Pnt2d> aProjection = theCircle.project(thePoint->pnt());
  if(aProjection.get())
    thePoint->setValue(aProjection);
}


SketchPlugin_MacroArc::SketchPlugin_MacroArc()
: SketchPlugin_SketchEntity(),
  myParamBefore(0.0)
{
}

void SketchPlugin_MacroArc::initAttributes()
{
  data()->addAttribute(ARC_TYPE(), ModelAPI_AttributeString::typeId());

  data()->addAttribute(CENTER_POINT_ID(), GeomDataAPI_Point2D::typeId());
  data()->addAttribute(START_POINT_1_ID(), GeomDataAPI_Point2D::typeId());
  data()->addAttribute(END_POINT_1_ID(), GeomDataAPI_Point2D::typeId());

  data()->addAttribute(START_POINT_2_ID(), GeomDataAPI_Point2D::typeId());
  data()->addAttribute(END_POINT_2_ID(), GeomDataAPI_Point2D::typeId());
  data()->addAttribute(PASSED_POINT_ID(), GeomDataAPI_Point2D::typeId());

  data()->addAttribute(TANGENT_POINT_ID(), ModelAPI_AttributeRefAttr::typeId());
  data()->addAttribute(END_POINT_3_ID(), GeomDataAPI_Point2D::typeId());

  data()->addAttribute(REVERSED_ID(), ModelAPI_AttributeBoolean::typeId());

  data()->addAttribute(RADIUS_ID(), ModelAPI_AttributeDouble::typeId());
  data()->addAttribute(ANGLE_ID(), ModelAPI_AttributeDouble::typeId());

  data()->addAttribute(AUXILIARY_ID(), ModelAPI_AttributeBoolean::typeId());

  data()->addAttribute(CENTER_POINT_REF_ID(), ModelAPI_AttributeRefAttr::typeId());
  data()->addAttribute(START_POINT_REF_ID(), ModelAPI_AttributeRefAttr::typeId());
  data()->addAttribute(END_POINT_REF_ID(), ModelAPI_AttributeRefAttr::typeId());
  data()->addAttribute(PASSED_POINT_REF_ID(), ModelAPI_AttributeRefAttr::typeId());

  boolean(REVERSED_ID())->setValue(false);

  ModelAPI_Session::get()->validators()->registerNotObligatory(getKind(), CENTER_POINT_REF_ID());
  ModelAPI_Session::get()->validators()->registerNotObligatory(getKind(), START_POINT_REF_ID());
  ModelAPI_Session::get()->validators()->registerNotObligatory(getKind(), END_POINT_REF_ID());
  ModelAPI_Session::get()->validators()->registerNotObligatory(getKind(), PASSED_POINT_REF_ID());
}

void SketchPlugin_MacroArc::attributeChanged(const std::string& theID)
{
  std::string anArcType = string(ARC_TYPE())->value();

  // If arc type switched reset according attributes.
  if(theID == ARC_TYPE()) {
    SketchPlugin_Tools::resetAttribute(this, CENTER_POINT_ID());
    SketchPlugin_Tools::resetAttribute(this, CENTER_POINT_REF_ID());
    SketchPlugin_Tools::resetAttribute(this, START_POINT_1_ID());
    SketchPlugin_Tools::resetAttribute(this, START_POINT_REF_ID());
    SketchPlugin_Tools::resetAttribute(this, END_POINT_1_ID());
    SketchPlugin_Tools::resetAttribute(this, END_POINT_REF_ID());
    SketchPlugin_Tools::resetAttribute(this, START_POINT_2_ID());
    SketchPlugin_Tools::resetAttribute(this, END_POINT_2_ID());
    SketchPlugin_Tools::resetAttribute(this, PASSED_POINT_ID());
    SketchPlugin_Tools::resetAttribute(this, PASSED_POINT_REF_ID());
    SketchPlugin_Tools::resetAttribute(this, TANGENT_POINT_ID());
    SketchPlugin_Tools::resetAttribute(this, END_POINT_3_ID());
    SketchPlugin_Tools::resetAttribute(this, REVERSED_ID());
    SketchPlugin_Tools::resetAttribute(this, RADIUS_ID());
    SketchPlugin_Tools::resetAttribute(this, ANGLE_ID());

    myCenter.reset();
    myStart.reset();
    myEnd.reset();
    boolean(REVERSED_ID())->setValue(false);
    myParamBefore = 0.0;
  } else if(anArcType == ARC_TYPE_BY_CENTER_AND_POINTS())
    fillByCenterAndTwoPassed();
  else if(anArcType == ARC_TYPE_BY_THREE_POINTS())
    fillByThreePassedPoints();
  else if(anArcType == ARC_TYPE_BY_TANGENT_EDGE())
    fillByTangentEdge();

  double aRadius = 0;
  double anAngle = 0;
  if(myCenter.get() && myStart.get()) {
    aRadius = myCenter->distance(myStart);
    if(myEnd.get()) {
      if(myStart->isEqual(myEnd)) {
        anAngle = 360;
      } else {
        GeomAPI_Circ2d aCircleForArc(myCenter, myStart);
        double aStartParam, anEndParam;
        aCircleForArc.parameter(myStart, paramTolerance, aStartParam);
        aCircleForArc.parameter(myEnd, paramTolerance, anEndParam);
        anAngle = (anEndParam - aStartParam) / PI * 180.0;
        if(boolean(REVERSED_ID())->value()) anAngle = 360.0 - anAngle;
      }
    }
  }

  bool aWasBlocked = data()->blockSendAttributeUpdated(true);
  real(RADIUS_ID())->setValue(aRadius);
  real(ANGLE_ID())->setValue(anAngle);
  data()->blockSendAttributeUpdated(aWasBlocked, false);
}

GeomShapePtr SketchPlugin_MacroArc::getArcShape()
{
  if(!myStart.get() || !myEnd.get() || !myCenter.get()) {
    return GeomShapePtr();
  }

  SketchPlugin_Sketch* aSketch = sketch();
  if(!aSketch) {
    return GeomShapePtr();
  }

  std::shared_ptr<GeomAPI_Pnt> aCenter(aSketch->to3D(myCenter->x(), myCenter->y()));
  std::shared_ptr<GeomAPI_Pnt> aStart(aSketch->to3D(myStart->x(), myStart->y()));
  std::shared_ptr<GeomAPI_Pnt> anEnd(aSketch->to3D(myEnd->x(), myEnd->y()));
  std::shared_ptr<GeomDataAPI_Dir> aNDir =
    std::dynamic_pointer_cast<GeomDataAPI_Dir>(aSketch->attribute(SketchPlugin_Sketch::NORM_ID()));
  std::shared_ptr<GeomAPI_Dir> aNormal(new GeomAPI_Dir(aNDir->x(), aNDir->y(), aNDir->z()));

  GeomShapePtr anArcShape = boolean(REVERSED_ID())->value() ?
      GeomAlgoAPI_EdgeBuilder::lineCircleArc(aCenter, anEnd, aStart, aNormal)
    : GeomAlgoAPI_EdgeBuilder::lineCircleArc(aCenter, aStart, anEnd, aNormal);

  return anArcShape;
}

AISObjectPtr SketchPlugin_MacroArc::getAISObject(AISObjectPtr thePrevious)
{
  if(!myStart.get() || !myEnd.get() || !myCenter.get()) {
    return AISObjectPtr();
  }

  SketchPlugin_Sketch* aSketch = sketch();
  if(!aSketch) {
    return AISObjectPtr();
  }

  GeomShapePtr anArcShape = getArcShape();
  std::shared_ptr<GeomAPI_Pnt> aCenter = aSketch->to3D(myCenter->x(), myCenter->y());;
  GeomShapePtr aCenterPointShape = GeomAlgoAPI_PointBuilder::vertex(aCenter);

  if(!anArcShape.get() || !aCenterPointShape.get()) {
    return AISObjectPtr();
  }

  std::list<std::shared_ptr<GeomAPI_Shape> > aShapes;
  aShapes.push_back(anArcShape);
  aShapes.push_back(aCenterPointShape);

  std::shared_ptr<GeomAPI_Shape> aCompound = GeomAlgoAPI_CompoundBuilder::compound(aShapes);
  AISObjectPtr anAIS = thePrevious;
  if(!anAIS.get()) {
    anAIS.reset(new GeomAPI_AISObject());
  }
  anAIS->createShape(aCompound);
  return anAIS;
}

void SketchPlugin_MacroArc::execute()
{
  FeaturePtr anArcFeature = createArcFeature();

  myCenter.reset();
  myStart.reset();
  myEnd.reset();

  // Create constraints.
  std::string anArcType = string(ARC_TYPE())->value();
  if(anArcType == ARC_TYPE_BY_CENTER_AND_POINTS()) {
    SketchPlugin_Tools::createConstraint(this,
                                         CENTER_POINT_REF_ID(),
                                         anArcFeature->attribute(SketchPlugin_Arc::CENTER_ID()),
                                         ObjectPtr(),
                                         false);
    SketchPlugin_Tools::createConstraint(this,
                                         START_POINT_REF_ID(),
                                         anArcFeature->attribute(SketchPlugin_Arc::START_ID()),
                                         ObjectPtr(),
                                         false);
    SketchPlugin_Tools::createConstraint(this,
                                         END_POINT_REF_ID(),
                                         anArcFeature->attribute(SketchPlugin_Arc::END_ID()),
                                         ObjectPtr(),
                                         false);
  } else if(anArcType == ARC_TYPE_BY_THREE_POINTS()) {
    SketchPlugin_Tools::createConstraint(this,
                                         START_POINT_REF_ID(),
                                         anArcFeature->attribute(SketchPlugin_Arc::START_ID()),
                                         ObjectPtr(),
                                         false);
    SketchPlugin_Tools::createConstraint(this,
                                         END_POINT_REF_ID(),
                                         anArcFeature->attribute(SketchPlugin_Arc::END_ID()),
                                         ObjectPtr(),
                                         false);
    SketchPlugin_Tools::createConstraint(this,
                                         PASSED_POINT_REF_ID(),
                                         AttributePtr(),
                                         anArcFeature->lastResult(),
                                         true);
  } else if(anArcType == ARC_TYPE_BY_TANGENT_EDGE()) {
    // constraints for tangent arc
    SketchPlugin_Tools::createConstraint(this,
                                         TANGENT_POINT_ID(),
                                         anArcFeature->attribute(SketchPlugin_Arc::START_ID()),
                                         ObjectPtr(),
                                         false);
    FeaturePtr aTangent = sketch()->addFeature(SketchPlugin_ConstraintTangent::ID());
    AttributeRefAttrPtr aRefAttrA = aTangent->refattr(SketchPlugin_Constraint::ENTITY_A());
    AttributeRefAttrPtr aTgPntRefAttr = refattr(TANGENT_POINT_ID());
    FeaturePtr aTgFeature = ModelAPI_Feature::feature(aTgPntRefAttr->attr()->owner());
    aRefAttrA->setObject(aTgFeature->lastResult());
    AttributeRefAttrPtr aRefAttrB = aTangent->refattr(SketchPlugin_Constraint::ENTITY_B());
    aRefAttrB->setObject(anArcFeature->lastResult());
    // constraint for end point
    SketchPlugin_Tools::createConstraint(this,
                                         END_POINT_REF_ID(),
                                         anArcFeature->attribute(SketchPlugin_Arc::END_ID()),
                                         ObjectPtr(),
                                         false);
  }
}

FeaturePtr SketchPlugin_MacroArc::createArcFeature()
{
  FeaturePtr anArcFeature = sketch()->addFeature(SketchPlugin_Arc::ID());
  std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
      anArcFeature->attribute(SketchPlugin_Arc::CENTER_ID()))->setValue(myCenter);
  std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
      anArcFeature->attribute(SketchPlugin_Arc::START_ID()))->setValue(myStart);
  std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
      anArcFeature->attribute(SketchPlugin_Arc::END_ID()))->setValue(myEnd);
  anArcFeature->boolean(SketchPlugin_Arc::REVERSED_ID())
                ->setValue(boolean(REVERSED_ID())->value());
  anArcFeature->boolean(SketchPlugin_Arc::AUXILIARY_ID())
                ->setValue(boolean(AUXILIARY_ID())->value());
  anArcFeature->execute();

  return anArcFeature;
}

void SketchPlugin_MacroArc::fillByCenterAndTwoPassed()
{
  AttributePoint2DPtr aCenterPointAttr =
      std::dynamic_pointer_cast<GeomDataAPI_Point2D>(attribute(CENTER_POINT_ID()));
  if (!aCenterPointAttr->isInitialized())
      return;

  AttributePoint2DPtr aStartPointAttr =
      std::dynamic_pointer_cast<GeomDataAPI_Point2D>(attribute(START_POINT_1_ID()));
  if (!aStartPointAttr->isInitialized())
    return;

  myCenter = aCenterPointAttr->pnt();
  myStart = aStartPointAttr->pnt();
  myEnd = myStart;

  AttributePoint2DPtr anEndPointAttr =
      std::dynamic_pointer_cast<GeomDataAPI_Point2D>(attribute(END_POINT_1_ID()));
  if (!anEndPointAttr->isInitialized())
    return;

  GeomAPI_Circ2d aCircleForArc(myCenter, myStart);

  // End point should be a projection on circle.
  bool aWasBlocked = data()->blockSendAttributeUpdated(true);
  projectPointOnCircle(anEndPointAttr, aCircleForArc);
  data()->blockSendAttributeUpdated(aWasBlocked, false);
  myEnd = anEndPointAttr->pnt();

  // update the REVERSED flag
  recalculateReversedFlagByEnd(aCircleForArc);
}

void SketchPlugin_MacroArc::recalculateReversedFlagByEnd(const GeomAPI_Circ2d& theCurrentCircular)
{
  double aParameterNew = 0.0;
  if(theCurrentCircular.parameter(myEnd, paramTolerance, aParameterNew)) {
    if(myParamBefore <= PI / 2.0 && aParameterNew >= PI * 1.5) {
      boolean(REVERSED_ID())->setValue(true);
    } else if(myParamBefore >= PI * 1.5 && aParameterNew <= PI / 2.0) {
      boolean(REVERSED_ID())->setValue(false);
    }
  }
  myParamBefore = aParameterNew;
}

void SketchPlugin_MacroArc::fillByThreePassedPoints()
{
  AttributePoint2DPtr aStartPointAttr =
      std::dynamic_pointer_cast<GeomDataAPI_Point2D>(attribute(START_POINT_2_ID()));
  if (!aStartPointAttr->isInitialized())
    return;

  AttributePoint2DPtr anEndPointAttr =
      std::dynamic_pointer_cast<GeomDataAPI_Point2D>(attribute(END_POINT_2_ID()));
  if (!anEndPointAttr->isInitialized())
    return;

  myStart = aStartPointAttr->pnt();
  myEnd = anEndPointAttr->pnt();

  AttributePoint2DPtr aPassedPointAttr =
      std::dynamic_pointer_cast<GeomDataAPI_Point2D>(attribute(PASSED_POINT_ID()));
  if (aPassedPointAttr->isInitialized()) {
    std::shared_ptr<GeomAPI_Pnt2d> aPassedPnt;
    std::shared_ptr<GeomAPI_Shape> aTangentCurve;
    SketchPlugin_Tools::convertRefAttrToPointOrTangentCurve(
        refattr(PASSED_POINT_REF_ID()), aPassedPointAttr, aTangentCurve, aPassedPnt);

    std::shared_ptr<GeomAPI_Interface> aPassed;
    if (aTangentCurve)
      aPassed = aTangentCurve;
    else
      aPassed = aPassedPnt;

    std::shared_ptr<GeomAPI_Ax3> anAxis = SketchPlugin_Sketch::plane(sketch());
    GeomAPI_Circ2d aCircle(myStart, myEnd, aPassed, anAxis);
    myCenter = aCircle.center();
    aCircle = GeomAPI_Circ2d(myCenter, myStart);

    recalculateReversedFlagByPassed(aCircle);
  } else
    myCenter.reset(new GeomAPI_Pnt2d(myStart->xy()->added(myEnd->xy())->multiplied(0.5)));
}

void SketchPlugin_MacroArc::recalculateReversedFlagByPassed(
    const GeomAPI_Circ2d& theCurrentCircular)
{
  AttributePoint2DPtr aPassedAttr =
      std::dynamic_pointer_cast<GeomDataAPI_Point2D>(attribute(PASSED_POINT_ID()));
  std::shared_ptr<GeomAPI_Pnt2d> aPassed = theCurrentCircular.project(aPassedAttr->pnt());

  double aEndParam, aPassedParam;
  theCurrentCircular.parameter(myEnd, paramTolerance, aEndParam);
  theCurrentCircular.parameter(aPassed, paramTolerance, aPassedParam);

  if(aPassedParam > aEndParam)
    boolean(REVERSED_ID())->setValue(true);
  else
    boolean(REVERSED_ID())->setValue(false);

  myParamBefore = aEndParam;
}

void SketchPlugin_MacroArc::fillByTangentEdge()
{
  AttributeRefAttrPtr aTangentAttr = refattr(TANGENT_POINT_ID());
  if (!aTangentAttr->isInitialized())
    return;

  AttributePoint2DPtr aTangentPointAttr =
      std::dynamic_pointer_cast<GeomDataAPI_Point2D>(aTangentAttr->attr());
  if (!aTangentPointAttr->isInitialized())
    return;

  AttributePoint2DPtr anEndPointAttr =
      std::dynamic_pointer_cast<GeomDataAPI_Point2D>(attribute(END_POINT_3_ID()));
  if (!anEndPointAttr->isInitialized())
    return;

  myStart = aTangentPointAttr->pnt();
  myEnd = anEndPointAttr->pnt();
  if (myStart->isEqual(myEnd))
    return;

  // obtain a shape the tangent point belongs to
  FeaturePtr aTangentFeature = ModelAPI_Feature::feature(aTangentPointAttr->owner());
  std::shared_ptr<GeomAPI_Shape> aTangentShape = aTangentFeature->lastResult()->shape();

  std::shared_ptr<GeomAPI_Ax3> anAxis = SketchPlugin_Sketch::plane(sketch());
  GeomAPI_Circ2d aCircle(myStart, myEnd, aTangentShape, anAxis);
  myCenter = aCircle.center();

  // rebuild circle to set start point equal to zero parameter
  aCircle = GeomAPI_Circ2d(myCenter, myStart);
  recalculateReversedFlagByEnd(aCircle);
}
