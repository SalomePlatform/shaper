// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:        SketchPlugin_MacroCircle.cpp
// Created:     26 May 2014
// Author:      Artem ZHIDKOV

#include "SketchPlugin_MacroCircle.h"

#include "SketchPlugin_Circle.h"
#include "SketchPlugin_ConstraintCoincidence.h"
#include "SketchPlugin_Tools.h"

#include <ModelAPI_Data.h>
#include <ModelAPI_Events.h>
#include <ModelAPI_ResultConstruction.h>
#include <ModelAPI_AttributeSelection.h>
#include <ModelAPI_Validator.h>
#include <ModelAPI_AttributeDouble.h>
#include <ModelAPI_AttributeRefAttr.h>
#include <ModelAPI_AttributeString.h>
#include <ModelAPI_Session.h>

#include <GeomAPI_Pnt2d.h>
#include <GeomAPI_Circ.h>
#include <GeomAPI_Circ2d.h>
#include <GeomAPI_Vertex.h>
#include <GeomAPI_XY.h>
#include <GeomDataAPI_Point2D.h>
#include <GeomDataAPI_Dir.h>
#include <GeomAlgoAPI_PointBuilder.h>
#include <GeomAlgoAPI_EdgeBuilder.h>
#include <GeomAlgoAPI_CompoundBuilder.h>


const double tolerance = 1e-7;

namespace {
  static const std::string& POINT_ID(int theIndex)
  {
    switch (theIndex) {
    case 1: return SketchPlugin_MacroCircle::FIRST_POINT_ID();
    case 2: return SketchPlugin_MacroCircle::SECOND_POINT_ID();
    case 3: return SketchPlugin_MacroCircle::THIRD_POINT_ID();
    }

    static const std::string DUMMY;
    return DUMMY;
  }
}


SketchPlugin_MacroCircle::SketchPlugin_MacroCircle()
: SketchPlugin_SketchEntity(),
myRadius(0)
{
}

void SketchPlugin_MacroCircle::initAttributes()
{
  data()->addAttribute(CIRCLE_TYPE(), ModelAPI_AttributeString::typeId());

  data()->addAttribute(CENTER_POINT_ID(), GeomDataAPI_Point2D::typeId());
  data()->addAttribute(CENTER_POINT_REF_ID(), ModelAPI_AttributeRefAttr::typeId());
  data()->addAttribute(PASSED_POINT_ID(), GeomDataAPI_Point2D::typeId());
  data()->addAttribute(PASSED_POINT_REF_ID(), ModelAPI_AttributeRefAttr::typeId());

  data()->addAttribute(FIRST_POINT_ID(), GeomDataAPI_Point2D::typeId());
  data()->addAttribute(SECOND_POINT_ID(), GeomDataAPI_Point2D::typeId());
  data()->addAttribute(THIRD_POINT_ID(), GeomDataAPI_Point2D::typeId());

  data()->addAttribute(CIRCLE_RADIUS_ID(), ModelAPI_AttributeDouble::typeId());
  data()->addAttribute(AUXILIARY_ID(), ModelAPI_AttributeBoolean::typeId());

  ModelAPI_Session::get()->validators()->registerNotObligatory(getKind(), CENTER_POINT_REF_ID());
  ModelAPI_Session::get()->validators()->registerNotObligatory(getKind(), PASSED_POINT_REF_ID());
}

void SketchPlugin_MacroCircle::execute()
{
  // Create circle feature.
  FeaturePtr aCircleFeature = sketch()->addFeature(SketchPlugin_Circle::ID());
  std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
      aCircleFeature->attribute(SketchPlugin_Circle::CENTER_ID()))->setValue(myCenter->x(),
                                                                             myCenter->y());
  aCircleFeature->real(SketchPlugin_Circle::RADIUS_ID())->setValue(myRadius);
  aCircleFeature->boolean(SketchPlugin_Circle::AUXILIARY_ID())
                ->setValue(boolean(AUXILIARY_ID())->value());
  aCircleFeature->execute();

  myCenter.reset();
  myRadius = 0;

  // Create constraints.
  SketchPlugin_Tools::createConstraint(this,
                                       CENTER_POINT_REF_ID(),
                                       aCircleFeature->attribute(SketchPlugin_Circle::CENTER_ID()),
                                       NULL,
                                       false);
  SketchPlugin_Tools::createConstraint(this,
                                       PASSED_POINT_REF_ID(),
                                       NULL,
                                       aCircleFeature->lastResult(),
                                       true);
}

AISObjectPtr SketchPlugin_MacroCircle::getAISObject(AISObjectPtr thePrevious)
{
  if(!myCenter.get() || myRadius < tolerance) {
    return AISObjectPtr();
  }

  SketchPlugin_Sketch* aSketch = sketch();
  if(!aSketch) {
    return AISObjectPtr();
  }

  // Compute a circle in 3D view.
  std::shared_ptr<GeomAPI_Pnt> aCenter(aSketch->to3D(myCenter->x(), myCenter->y()));
  std::shared_ptr<GeomDataAPI_Dir> aNDir =
      std::dynamic_pointer_cast<GeomDataAPI_Dir>(
          aSketch->data()->attribute(SketchPlugin_Sketch::NORM_ID()));
  std::shared_ptr<GeomAPI_Dir> aNormal = aNDir->dir();
  GeomShapePtr aCircleShape = GeomAlgoAPI_EdgeBuilder::lineCircle(aCenter, aNormal, myRadius);
  GeomShapePtr aCenterPointShape = GeomAlgoAPI_PointBuilder::vertex(aCenter);
  if(!aCircleShape.get() || !aCenterPointShape.get()) {
    return AISObjectPtr();
  }

  std::list<std::shared_ptr<GeomAPI_Shape> > aShapes;
  aShapes.push_back(aCircleShape);
  aShapes.push_back(aCenterPointShape);

  std::shared_ptr<GeomAPI_Shape> aCompound = GeomAlgoAPI_CompoundBuilder::compound(aShapes);
  AISObjectPtr anAIS = thePrevious;
  if(!anAIS.get()) {
    anAIS.reset(new GeomAPI_AISObject());
  }
  anAIS->createShape(aCompound);
  return anAIS;
}

void SketchPlugin_MacroCircle::attributeChanged(const std::string& theID) {
  // If circle type switched reset according attributes.
  if(theID == CIRCLE_TYPE()) {
    std::string aType = string(CIRCLE_TYPE())->value();
    if(aType == CIRCLE_TYPE_BY_CENTER_AND_PASSED_POINTS()) {
      resetAttribute(CENTER_POINT_ID());
      resetAttribute(PASSED_POINT_ID());
    } else if(aType == CIRCLE_TYPE_BY_THREE_POINTS()) {
      resetAttribute(FIRST_POINT_ID());
      resetAttribute(SECOND_POINT_ID());
      resetAttribute(THIRD_POINT_ID());
    }
    myCenter.reset();
    myRadius = 0;
  } else if(theID == CENTER_POINT_ID() || theID == PASSED_POINT_ID()) {
    std::shared_ptr<GeomDataAPI_Point2D> aCenterPointAttr =
        std::dynamic_pointer_cast<GeomDataAPI_Point2D>(attribute(CENTER_POINT_ID()));
    if(!aCenterPointAttr->isInitialized()) {
      return;
    }
    std::shared_ptr<GeomDataAPI_Point2D> aPassedPointAttr =
        std::dynamic_pointer_cast<GeomDataAPI_Point2D>(attribute(PASSED_POINT_ID()));
    if(!aPassedPointAttr->isInitialized()) {
      return;
    }

    myCenter = aCenterPointAttr->pnt();
    myRadius = myCenter->distance(aPassedPointAttr->pnt());
  } else if(theID == FIRST_POINT_ID() || theID == SECOND_POINT_ID() || theID == THIRD_POINT_ID()) {
    std::shared_ptr<GeomAPI_Pnt2d> aPoints[3];
    int aNbInitialized = 0;
    for(int i = 1; i <= 3; ++i) {
      std::shared_ptr<GeomDataAPI_Point2D> aCurPnt =
          std::dynamic_pointer_cast<GeomDataAPI_Point2D>(attribute(POINT_ID(i)));
      if(aCurPnt->isInitialized())
        aPoints[aNbInitialized++] = aCurPnt->pnt();
    }

    if(aNbInitialized == 1) {
      return;
    } else if(aNbInitialized == 2) {
      std::shared_ptr<GeomAPI_XY> aCenterXY =
          aPoints[0]->xy()->added(aPoints[1]->xy())->multiplied(0.5);
      myCenter.reset(new GeomAPI_Pnt2d(aCenterXY->x(), aCenterXY->y()));
      myRadius = aPoints[0]->distance(aPoints[1]) * 0.5;
    } else {
      std::shared_ptr<GeomAPI_Circ2d> aCircle(
          new GeomAPI_Circ2d(aPoints[0], aPoints[1], aPoints[2]));
      myCenter = aCircle->center();
      if(myCenter.get()) {
        myRadius = aCircle->radius();
      }
    }
  }

  AttributeDoublePtr aRadiusAttr = real(CIRCLE_RADIUS_ID());
  bool aWasBlocked = data()->blockSendAttributeUpdated(true);
  aRadiusAttr->setValue(myRadius);
  data()->blockSendAttributeUpdated(aWasBlocked, false);
}

void SketchPlugin_MacroCircle::resetAttribute(const std::string& theId)
{
  AttributePtr anAttr = attribute(theId);
  if(anAttr.get()) {
    anAttr->reset();
  }
}
