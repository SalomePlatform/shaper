// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:        SketchAPI_Arc.cpp
// Created:     09 June 2016
// Author:      Dmitry Bobylev

#include "SketchAPI_Arc.h"

#include <GeomAPI_Pnt2d.h>

#include <ModelHighAPI_Double.h>
#include <ModelHighAPI_Selection.h>
#include <ModelHighAPI_Tools.h>

//==================================================================================================
SketchAPI_Arc::SketchAPI_Arc(const std::shared_ptr<ModelAPI_Feature> & theFeature)
: SketchAPI_SketchEntity(theFeature)
{
  initialize();
}

//==================================================================================================
SketchAPI_Arc::SketchAPI_Arc(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                             double theCenterX, double theCenterY,
                             double theStartX, double theStartY,
                             double theEndX, double theEndY,
                             bool theInversed)
: SketchAPI_SketchEntity(theFeature)
{
  if(initialize()) {
    setByCenterStartEnd(theCenterX, theCenterY, theStartX, theStartY, theEndX, theEndY, theInversed);
  }
}

//==================================================================================================
SketchAPI_Arc::SketchAPI_Arc(const std::shared_ptr<ModelAPI_Feature>& theFeature,
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

//==================================================================================================
SketchAPI_Arc::SketchAPI_Arc(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                             double theStartX, double theStartY,
                             double theEndX, double theEndY,
                             double thePassedX, double thePassedY)
: SketchAPI_SketchEntity(theFeature)
{
  if (initialize()) {
    setByStartEndPassed(theStartX, theStartY, theEndX, theEndY, thePassedX, thePassedY);
  }
}

//==================================================================================================
SketchAPI_Arc::SketchAPI_Arc(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                             const std::shared_ptr<GeomAPI_Pnt2d>& theStart,
                             const std::shared_ptr<GeomAPI_Pnt2d>& theEnd,
                             const std::shared_ptr<GeomAPI_Pnt2d>& thePassed)
: SketchAPI_SketchEntity(theFeature)
{
  if (initialize()) {
    setByStartEndPassed(theStart, theEnd, thePassed);
  }
}

//==================================================================================================
SketchAPI_Arc::SketchAPI_Arc(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                             const ModelHighAPI_RefAttr& theTangentPoint,
                             double theEndX, double theEndY,
                             bool theInversed)
: SketchAPI_SketchEntity(theFeature)
{
  if (initialize()) {
    setByTangent(theTangentPoint, theEndX, theEndY, theInversed);
  }
}

//==================================================================================================
SketchAPI_Arc::SketchAPI_Arc(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                             const ModelHighAPI_RefAttr& theTangentPoint,
                             const std::shared_ptr<GeomAPI_Pnt2d>& theEnd,
                             bool theInversed)
: SketchAPI_SketchEntity(theFeature)
{
  if (initialize()) {
    setByTangent(theTangentPoint, theEnd, theInversed);
  }
}

//==================================================================================================
SketchAPI_Arc::SketchAPI_Arc(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                             const ModelHighAPI_Selection& theExternal)
: SketchAPI_SketchEntity(theFeature)
{
  if (initialize()) {
    setByExternal(theExternal);
  }
}

//==================================================================================================
SketchAPI_Arc::SketchAPI_Arc(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                             const std::string& theExternalName)
: SketchAPI_SketchEntity(theFeature)
{
  if (initialize()) {
    setByExternalName(theExternalName);
  }
}

//==================================================================================================
SketchAPI_Arc::~SketchAPI_Arc()
{

}

//==================================================================================================
void SketchAPI_Arc::setByCenterStartEnd(double theCenterX, double theCenterY,
                                        double theStartX, double theStartY,
                                        double theEndX, double theEndY,
                                        bool theInversed)
{
  fillAttribute(SketchPlugin_Arc::ARC_TYPE_CENTER_START_END(), myarcType);
  fillAttribute(center(), theCenterX, theCenterY);
  fillAttribute(startPoint(), theStartX, theStartY);
  fillAttribute(endPoint(), theEndX, theEndY);
  fillAttribute(theInversed, myinversed);

  execute();
}

//==================================================================================================
void SketchAPI_Arc::setByCenterStartEnd(const std::shared_ptr<GeomAPI_Pnt2d>& theCenter,
                                        const std::shared_ptr<GeomAPI_Pnt2d>& theStart,
                                        const std::shared_ptr<GeomAPI_Pnt2d>& theEnd,
                                        bool theInversed)
{
  fillAttribute(SketchPlugin_Arc::ARC_TYPE_CENTER_START_END(), myarcType);
  fillAttribute(theCenter, mycenter);
  fillAttribute(theStart, mystartPoint);
  fillAttribute(theEnd, myendPoint);
  fillAttribute(theInversed, myinversed);

  execute();
}

//==================================================================================================
void SketchAPI_Arc::setByStartEndPassed(double theStartX, double theStartY,
                                        double theEndX, double theEndY,
                                        double thePassedX, double thePassedY)
{
  fillAttribute(SketchPlugin_Arc::ARC_TYPE_THREE_POINTS(), myarcType);
  fillAttribute(startPoint(), theStartX, theStartY);
  fillAttribute(endPoint(), theEndX, theEndY);
  fillAttribute(passedPoint(), thePassedX, thePassedY);

  execute();
}

//==================================================================================================
void SketchAPI_Arc::setByStartEndPassed(const std::shared_ptr<GeomAPI_Pnt2d>& theStart,
                                        const std::shared_ptr<GeomAPI_Pnt2d>& theEnd,
                                        const std::shared_ptr<GeomAPI_Pnt2d>& thePassed)
{
  fillAttribute(SketchPlugin_Arc::ARC_TYPE_THREE_POINTS(), myarcType);
  fillAttribute(theStart, mystartPoint);
  fillAttribute(theEnd, myendPoint);
  fillAttribute(thePassed, mypassedPoint);

  execute();
}

//==================================================================================================
void SketchAPI_Arc::setByTangent(const ModelHighAPI_RefAttr& theTangentPoint,
                                 double theEndX, double theEndY,
                                 bool theInversed)
{
  fillAttribute(SketchPlugin_Arc::ARC_TYPE_TANGENT(), myarcType);
  fillAttribute(theTangentPoint, mytangentPoint);
  fillAttribute(endPoint(), theEndX, theEndY);
  fillAttribute(theInversed, myinversed);

  execute();
}

//==================================================================================================
void SketchAPI_Arc::setByTangent(const ModelHighAPI_RefAttr& theTangentPoint,
                                 const std::shared_ptr<GeomAPI_Pnt2d>& theEnd,
                                 bool theInversed)
{
  fillAttribute(SketchPlugin_Arc::ARC_TYPE_TANGENT(), myarcType);
  fillAttribute(theTangentPoint, mytangentPoint);
  fillAttribute(theEnd, myendPoint);
  fillAttribute(theInversed, myinversed);

  execute();
}

//==================================================================================================
void SketchAPI_Arc::setByExternal(const ModelHighAPI_Selection & theExternal)
{
  fillAttribute(theExternal, external());

  execute();
}

//==================================================================================================
void SketchAPI_Arc::setByExternalName(const std::string & theExternalName)
{
  fillAttribute(ModelHighAPI_Selection("EDGE", theExternalName), external());

  execute();
}

//==================================================================================================
void SketchAPI_Arc::setRadius(double theRadius)
{
  fillAttribute(ModelHighAPI_Double(theRadius), myradius);

  execute();
}

//==================================================================================================
void SketchAPI_Arc::setAngle(double theAngle)
{
  fillAttribute(ModelHighAPI_Double(theAngle), myangle);

  execute();
}
