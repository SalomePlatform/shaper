// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:        SketchPlugin_MacroArc.cpp
// Created:     26 Apr 2014
// Author:      Artem ZHIDKOV

#include "SketchPlugin_MacroArc.h"

#include "SketchPlugin_Sketch.h"
#include "SketchPlugin_Tools.h"

#include <ModelAPI_AttributeDouble.h>
#include <ModelAPI_AttributeRefAttr.h>
#include <ModelAPI_AttributeString.h>
#include <ModelAPI_Session.h>
#include <ModelAPI_Validator.h>

#include <GeomAPI_Circ.h>
#include <GeomAPI_Circ2d.h>
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
    std::string aType = string(ARC_TYPE())->value();
    if(aType == ARC_TYPE_BY_CENTER_AND_POINTS()) {
      SketchPlugin_Tools::resetAttribute(this, CENTER_POINT_ID());
      SketchPlugin_Tools::resetAttribute(this, CENTER_POINT_REF_ID());
      SketchPlugin_Tools::resetAttribute(this, START_POINT_1_ID());
      SketchPlugin_Tools::resetAttribute(this, START_POINT_REF_ID());
      SketchPlugin_Tools::resetAttribute(this, END_POINT_1_ID());
      SketchPlugin_Tools::resetAttribute(this, END_POINT_REF_ID());
    } else if(aType == ARC_TYPE_BY_THREE_POINTS()) {
      SketchPlugin_Tools::resetAttribute(this, START_POINT_2_ID());
      SketchPlugin_Tools::resetAttribute(this, START_POINT_REF_ID());
      SketchPlugin_Tools::resetAttribute(this, END_POINT_2_ID());
      SketchPlugin_Tools::resetAttribute(this, END_POINT_REF_ID());
      SketchPlugin_Tools::resetAttribute(this, PASSED_POINT_ID());
      SketchPlugin_Tools::resetAttribute(this, PASSED_POINT_REF_ID());
    } else if(aType == ARC_TYPE_BY_TANGENT_EDGE()) {
      SketchPlugin_Tools::resetAttribute(this, TANGENT_POINT_ID());
      SketchPlugin_Tools::resetAttribute(this, END_POINT_3_ID());
      SketchPlugin_Tools::resetAttribute(this, END_POINT_REF_ID());
    }

    myCenter.reset();
    myStart.reset();
    myEnd.reset();
    boolean(REVERSED_ID())->setValue(false);
    myParamBefore = 0.0;
  } else if(anArcType == ARC_TYPE_BY_CENTER_AND_POINTS()) {
    std::shared_ptr<GeomDataAPI_Point2D> aCenterPointAttr =
        std::dynamic_pointer_cast<GeomDataAPI_Point2D>(attribute(CENTER_POINT_ID()));
    if(!aCenterPointAttr->isInitialized()) {
      return;
    }
    std::shared_ptr<GeomDataAPI_Point2D> aStartPointAttr =
        std::dynamic_pointer_cast<GeomDataAPI_Point2D>(attribute(START_POINT_1_ID()));
    if(!aStartPointAttr->isInitialized()) {
      return;
    }

    myCenter = aCenterPointAttr->pnt();
    myStart = aStartPointAttr->pnt();
    myEnd = myStart;

    std::shared_ptr<GeomDataAPI_Point2D> anEndPointAttr =
        std::dynamic_pointer_cast<GeomDataAPI_Point2D>(attribute(END_POINT_1_ID()));
    if(anEndPointAttr->isInitialized()) {
      // End point should be a projection on circle.
      GeomAPI_Circ2d aCircleForArc(myCenter, myStart);
      std::shared_ptr<GeomAPI_Pnt2d> aProjection = aCircleForArc.project(anEndPointAttr->pnt());
      if(aProjection.get()) {
        bool aWasBlocked = data()->blockSendAttributeUpdated(true);
        anEndPointAttr->setValue(aProjection);
        data()->blockSendAttributeUpdated(aWasBlocked, false);
      }
      myEnd = anEndPointAttr->pnt();

      double aParameterNew = 0.0;
      if(aCircleForArc.parameter(myEnd, paramTolerance, aParameterNew)) {
        if(myParamBefore <= PI / 2.0 && aParameterNew >= PI * 1.5) {
          boolean(REVERSED_ID())->setValue(true);
        } else if(myParamBefore >= PI * 1.5 && aParameterNew <= PI / 2.0) {
          boolean(REVERSED_ID())->setValue(false);
        }
      }
      myParamBefore = aParameterNew;
    }
  } else if(anArcType == ARC_TYPE_BY_THREE_POINTS()) {
    std::shared_ptr<GeomDataAPI_Point2D> aStartPointAttr =
        std::dynamic_pointer_cast<GeomDataAPI_Point2D>(attribute(START_POINT_2_ID()));
    if(!aStartPointAttr->isInitialized()) {
      return;
    }
    std::shared_ptr<GeomDataAPI_Point2D> anEndPointAttr =
        std::dynamic_pointer_cast<GeomDataAPI_Point2D>(attribute(END_POINT_2_ID()));
    if(!anEndPointAttr->isInitialized()) {
      return;
    }

    myStart = aStartPointAttr->pnt();
    myEnd = anEndPointAttr->pnt();

    std::shared_ptr<GeomDataAPI_Point2D> aPassedPointAttr =
        std::dynamic_pointer_cast<GeomDataAPI_Point2D>(attribute(PASSED_POINT_ID()));
    if(aPassedPointAttr->isInitialized()) {
      std::shared_ptr<GeomAPI_Pnt2d> aPassed = aPassedPointAttr->pnt();
      GeomAPI_Circ2d aCircle(myStart, myEnd, aPassed);
      myCenter = aCircle.center();
      aCircle = GeomAPI_Circ2d(myCenter, myStart);
      double anEndParam, aPassedParam;
      aCircle.parameter(myEnd, paramTolerance, anEndParam);
      aCircle.parameter(aPassed, paramTolerance, aPassedParam);
      if(aPassedParam > anEndParam) {
        boolean(REVERSED_ID())->setValue(true);
      } else {
        boolean(REVERSED_ID())->setValue(false);
      }
    } else {
      std::shared_ptr<GeomAPI_XY> aDir = myEnd->xy()->decreased(myStart->xy())->multiplied(0.5);
      double x = aDir->x();
      double y = aDir->y();
      aDir->setX(x - y);
      aDir->setY(y + x);
      myCenter.reset(new GeomAPI_Pnt2d(myStart->xy()->added(aDir)));
    }
  } else if(anArcType == ARC_TYPE_BY_TANGENT_EDGE()) {
    AttributeRefAttrPtr aTangentAttr = refattr(TANGENT_POINT_ID());
    if(!aTangentAttr->isInitialized()) {
      return;
    }
    std::shared_ptr<GeomDataAPI_Point2D> aTangentPointAttr =
        std::dynamic_pointer_cast<GeomDataAPI_Point2D>(aTangentAttr->attr());
    if(!aTangentPointAttr->isInitialized()) {
      return;
    }
    std::shared_ptr<GeomDataAPI_Point2D> anEndPointAttr =
        std::dynamic_pointer_cast<GeomDataAPI_Point2D>(attribute(END_POINT_3_ID()));
    if(!anEndPointAttr->isInitialized()) {
      return;
    }

    myStart = aTangentPointAttr->pnt();
    myEnd = anEndPointAttr->pnt();

    if(myStart->isEqual(myEnd)) {
      return;
    }

    SketchPlugin_Sketch* aSketch = sketch();
    if(!aSketch) {
      return;
    }

    std::shared_ptr<GeomAPI_Dir2d> anOrthoDir;
    FeaturePtr aTangFeature = ModelAPI_Feature::feature(aTangentPointAttr->owner());
    std::shared_ptr<GeomAPI_Edge> aTangEdge =
      std::dynamic_pointer_cast<GeomAPI_Edge>(aTangFeature->lastResult()->shape());
    if(aTangEdge->isLine()) {
      std::shared_ptr<GeomAPI_Dir> aDir = aTangEdge->line()->direction();
      std::shared_ptr<GeomAPI_Pnt> aPnt(new GeomAPI_Pnt(aDir->x(), aDir->y(), aDir->z()));
      std::shared_ptr<GeomAPI_Pnt2d> aPnt2d = aSketch->to2D(aPnt);
      anOrthoDir = std::shared_ptr<GeomAPI_Dir2d>(new GeomAPI_Dir2d(-aPnt2d->y(), aPnt2d->x()));
    }
    else if (aTangEdge->isArc()) {
      std::shared_ptr<GeomAPI_Pnt> aCenter = aTangEdge->circle()->center();
      std::shared_ptr<GeomAPI_Pnt2d> aCenter2d = aSketch->to2D(aCenter);
      anOrthoDir = std::shared_ptr<GeomAPI_Dir2d>(
          new GeomAPI_Dir2d(myStart->xy()->decreased(aCenter2d->xy())));
    }

    // compute parameters of the middle perpendicular
    std::shared_ptr<GeomAPI_XY> aEndPntCoord = myEnd->xy();
    std::shared_ptr<GeomAPI_XY> aTempDir = aEndPntCoord->decreased(myStart->xy());
    std::shared_ptr<GeomAPI_Dir2d> aMidDir(new GeomAPI_Dir2d(-aTempDir->y(), aTempDir->x()));
    std::shared_ptr<GeomAPI_Pnt2d> aMidPnt(
        new GeomAPI_Pnt2d(aEndPntCoord->added(myStart->xy())->multiplied(0.5)));

    // compute center of arc by calculating intersection of
    // orthogonal line and middle perpendicular
    std::shared_ptr<GeomAPI_Lin2d> anOrthoLine(new GeomAPI_Lin2d(myStart, anOrthoDir));
    std::shared_ptr<GeomAPI_Lin2d> aMiddleLine(new GeomAPI_Lin2d(aMidPnt, aMidDir));
    std::shared_ptr<GeomAPI_Pnt2d> aCenter = anOrthoLine->intersect(aMiddleLine);
    if(aCenter) {
      myCenter = aCenter;
    }

    GeomAPI_Circ2d aCircleForArc(myCenter, myStart);
    double aParameterNew = 0.0;
    if(aCircleForArc.parameter(myEnd, paramTolerance, aParameterNew)) {
      if(myParamBefore <= PI / 2.0 && aParameterNew >= PI * 1.5) {
        if(!boolean(REVERSED_ID())->value()) {
          boolean(REVERSED_ID())->setValue(true);
        }
      } else if(myParamBefore >= PI * 1.5 && aParameterNew <= PI / 2.0) {
        if(boolean(REVERSED_ID())->value()) {
          boolean(REVERSED_ID())->setValue(false);
        }
      }
    }
    myParamBefore = aParameterNew;
  }

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

AISObjectPtr SketchPlugin_MacroArc::getAISObject(AISObjectPtr thePrevious)
{
  if(!myStart.get() || !myEnd.get() || !myCenter.get()) {
    return AISObjectPtr();
  }

  SketchPlugin_Sketch* aSketch = sketch();
  if(!aSketch) {
    return AISObjectPtr();
  }

  std::shared_ptr<GeomAPI_Pnt> aStart = aSketch->to3D(myStart->x(), myStart->y());
  std::shared_ptr<GeomAPI_Pnt> anEnd = aSketch->to3D(myEnd->x(), myEnd->y());
  std::shared_ptr<GeomAPI_Pnt> aCenter = aSketch->to3D(myCenter->x(), myCenter->y());;

  std::shared_ptr<GeomDataAPI_Dir> aNDir =
      std::dynamic_pointer_cast<GeomDataAPI_Dir>(
          aSketch->data()->attribute(SketchPlugin_Sketch::NORM_ID()));
  std::shared_ptr<GeomAPI_Dir> aNormal = aNDir->dir();
  GeomShapePtr anArcShape = boolean(REVERSED_ID())->value() ?
      GeomAlgoAPI_EdgeBuilder::lineCircleArc(aCenter, anEnd, aStart, aNormal)
    : GeomAlgoAPI_EdgeBuilder::lineCircleArc(aCenter, aStart, anEnd, aNormal);
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
}
