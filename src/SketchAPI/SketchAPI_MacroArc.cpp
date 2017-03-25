// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:        SketchAPI_MacroArc.cpp
// Created:     09 June 2016
// Author:      Dmitry Bobylev

#include "SketchAPI_MacroArc.h"

#include <GeomAPI_Pnt2d.h>

#include <ModelHighAPI_Double.h>
#include <ModelHighAPI_Dumper.h>
#include <ModelHighAPI_Selection.h>
#include <ModelHighAPI_Tools.h>

#include <SketchPlugin_ConstraintCoincidence.h>
#include <SketchPlugin_ConstraintTangent.h>

/// Obtain constraints prepared by tangent arc
static std::list<FeaturePtr> tangentArcConstraints(const FeaturePtr& theArc);

//================================================================================================
SketchAPI_MacroArc::SketchAPI_MacroArc(const std::shared_ptr<ModelAPI_Feature> & theFeature)
: SketchAPI_SketchEntity(theFeature)
{
  initialize();
}

//================================================================================================
SketchAPI_MacroArc::SketchAPI_MacroArc(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                       double theCenterX, double theCenterY,
                                       double theStartX, double theStartY,
                                       double theEndX, double theEndY,
                                       bool theInversed)
: SketchAPI_SketchEntity(theFeature)
{
  if(initialize()) {
    setByCenterStartEnd(theCenterX, theCenterY, theStartX,
                        theStartY, theEndX, theEndY, theInversed);
  }
}

//================================================================================================
SketchAPI_MacroArc::SketchAPI_MacroArc(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                       const std::shared_ptr<GeomAPI_Pnt2d>& theCenter,
                                       const std::shared_ptr<GeomAPI_Pnt2d>& theStart,
                                       const std::shared_ptr<GeomAPI_Pnt2d>& theEnd,
                                       bool theInversed)
: SketchAPI_SketchEntity(theFeature)
{
  if(initialize()) {
    setByCenterStartEnd(theCenter, theStart, theEnd, theInversed);
  }
}

//================================================================================================
SketchAPI_MacroArc::SketchAPI_MacroArc(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                       double theStartX, double theStartY,
                                       double theEndX, double theEndY,
                                       double thePassedX, double thePassedY)
: SketchAPI_SketchEntity(theFeature)
{
  if (initialize()) {
    setByStartEndPassed(theStartX, theStartY, theEndX, theEndY, thePassedX, thePassedY);
  }
}

//===============================================================================================
SketchAPI_MacroArc::SketchAPI_MacroArc(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                       const std::shared_ptr<GeomAPI_Pnt2d>& theStart,
                                       const std::shared_ptr<GeomAPI_Pnt2d>& theEnd,
                                       const std::shared_ptr<GeomAPI_Pnt2d>& thePassed)
: SketchAPI_SketchEntity(theFeature)
{
  if (initialize()) {
    setByStartEndPassed(theStart, theEnd, thePassed);
  }
}

//================================================================================================
SketchAPI_MacroArc::SketchAPI_MacroArc(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                       const ModelHighAPI_RefAttr& theTangentPoint,
                                       double theEndX, double theEndY,
                                       bool theInversed)
: SketchAPI_SketchEntity(theFeature)
{
  if (initialize()) {
    setByTangent(theTangentPoint, theEndX, theEndY, theInversed);
  }
}

//================================================================================================
SketchAPI_MacroArc::SketchAPI_MacroArc(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                       const ModelHighAPI_RefAttr& theTangentPoint,
                                       const std::shared_ptr<GeomAPI_Pnt2d>& theEnd,
                                       bool theInversed)
: SketchAPI_SketchEntity(theFeature)
{
  if (initialize()) {
    setByTangent(theTangentPoint, theEnd, theInversed);
  }
}

//================================================================================================
SketchAPI_MacroArc::~SketchAPI_MacroArc()
{

}

//================================================================================================
void SketchAPI_MacroArc::setByCenterStartEnd(double theCenterX, double theCenterY,
                                             double theStartX, double theStartY,
                                             double theEndX, double theEndY,
                                             bool theInversed)
{
  fillAttribute(SketchPlugin_MacroArc::ARC_TYPE_BY_CENTER_AND_POINTS(), myarcType);
  fillAttribute(center(), theCenterX, theCenterY);
  fillAttribute(startPoint(), theStartX, theStartY);
  fillAttribute(endPoint(), theEndX, theEndY);
  fillAttribute(theInversed, myreversed);

  execute();
}

//================================================================================================
void SketchAPI_MacroArc::setByCenterStartEnd(const std::shared_ptr<GeomAPI_Pnt2d>& theCenter,
                                             const std::shared_ptr<GeomAPI_Pnt2d>& theStart,
                                             const std::shared_ptr<GeomAPI_Pnt2d>& theEnd,
                                             bool theInversed)
{
  fillAttribute(SketchPlugin_MacroArc::ARC_TYPE_BY_CENTER_AND_POINTS(), myarcType);
  fillAttribute(theCenter, mycenter);
  fillAttribute(theStart, mystartPoint);
  fillAttribute(theEnd, myendPoint);
  fillAttribute(theInversed, myreversed);

  execute();
}

//================================================================================================
void SketchAPI_MacroArc::setByStartEndPassed(double theStartX, double theStartY,
                                             double theEndX, double theEndY,
                                             double thePassedX, double thePassedY)
{
  fillAttribute(SketchPlugin_MacroArc::ARC_TYPE_BY_THREE_POINTS(), myarcType);
  fillAttribute(startPoint(), theStartX, theStartY);
  fillAttribute(endPoint(), theEndX, theEndY);
  fillAttribute(passedPoint(), thePassedX, thePassedY);

  execute();
}

//================================================================================================
void SketchAPI_MacroArc::setByStartEndPassed(const std::shared_ptr<GeomAPI_Pnt2d>& theStart,
                                             const std::shared_ptr<GeomAPI_Pnt2d>& theEnd,
                                             const std::shared_ptr<GeomAPI_Pnt2d>& thePassed)
{
  fillAttribute(SketchPlugin_MacroArc::ARC_TYPE_BY_THREE_POINTS(), myarcType);
  fillAttribute(theStart, mystartPoint);
  fillAttribute(theEnd, myendPoint);
  fillAttribute(thePassed, mypassedPoint);

  execute();
}

//================================================================================================
void SketchAPI_MacroArc::setByTangent(const ModelHighAPI_RefAttr& theTangentPoint,
                                      double theEndX, double theEndY,
                                      bool theInversed)
{
  fillAttribute(SketchPlugin_MacroArc::ARC_TYPE_BY_TANGENT_EDGE(), myarcType);
  fillAttribute(theTangentPoint, mytangentPoint);
  fillAttribute(endPoint(), theEndX, theEndY);
  fillAttribute(theInversed, myreversed);

  execute();
}

//================================================================================================
void SketchAPI_MacroArc::setByTangent(const ModelHighAPI_RefAttr& theTangentPoint,
                                      const std::shared_ptr<GeomAPI_Pnt2d>& theEnd,
                                      bool theInversed)
{
  fillAttribute(SketchPlugin_MacroArc::ARC_TYPE_BY_TANGENT_EDGE(), myarcType);
  fillAttribute(theTangentPoint, mytangentPoint);
  fillAttribute(theEnd, myendPoint);
  fillAttribute(theInversed, myreversed);

  execute();
}
