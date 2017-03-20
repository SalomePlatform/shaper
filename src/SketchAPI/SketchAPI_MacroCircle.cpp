// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:        SketchAPI_MacroCircle.cpp
// Created:     09 June 2016
// Author:      Dmitry Bobylev

#include "SketchAPI_MacroCircle.h"

#include <GeomAPI_Pnt2d.h>

#include <ModelHighAPI_Double.h>
#include <ModelHighAPI_Dumper.h>
#include <ModelHighAPI_Selection.h>
#include <ModelHighAPI_Tools.h>

//==================================================================================================
SketchAPI_MacroCircle::SketchAPI_MacroCircle(const std::shared_ptr<ModelAPI_Feature>& theFeature)
: SketchAPI_SketchEntity(theFeature)
{
  initialize();
}

//==================================================================================================
SketchAPI_MacroCircle::SketchAPI_MacroCircle(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                             double theCenterX,
                                             double theCenterY,
                                             double thePassedX,
                                             double thePassedY)
: SketchAPI_SketchEntity(theFeature)
{
  if(initialize()) {
    setByCenterAndPassedPoints(theCenterX, theCenterY, thePassedX, thePassedY);
  }
}

//==================================================================================================
SketchAPI_MacroCircle::SketchAPI_MacroCircle(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                             const std::shared_ptr<GeomAPI_Pnt2d>& theCenterPoint,
                                             const std::shared_ptr<GeomAPI_Pnt2d>& thePassedPoint)
: SketchAPI_SketchEntity(theFeature)
{
  if(initialize()) {
    setByCenterAndPassedPoints(theCenterPoint, thePassedPoint);
  }
}

//==================================================================================================
SketchAPI_MacroCircle::SketchAPI_MacroCircle(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                             double theX1, double theY1,
                                             double theX2, double theY2,
                                             double theX3, double theY3)
: SketchAPI_SketchEntity(theFeature)
{
  if(initialize()) {
    setByThreePoints(theX1, theY1, theX2, theY2, theX3, theY3);
  }
}

//==================================================================================================
SketchAPI_MacroCircle::SketchAPI_MacroCircle(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                             const std::shared_ptr<GeomAPI_Pnt2d>& thePoint1,
                                             const std::shared_ptr<GeomAPI_Pnt2d>& thePoint2,
                                             const std::shared_ptr<GeomAPI_Pnt2d>& thePoint3)
: SketchAPI_SketchEntity(theFeature)
{
  if(initialize()) {
    setByThreePoints(thePoint1, thePoint2, thePoint3);
  }
}

//==================================================================================================
SketchAPI_MacroCircle::~SketchAPI_MacroCircle()
{
}

//==================================================================================================
void SketchAPI_MacroCircle::setByCenterAndPassedPoints(double theCenterX,
                                                       double theCenterY,
                                                       double thePassedX,
                                                       double thePassedY)
{
  fillAttribute(SketchPlugin_MacroCircle::CIRCLE_TYPE_BY_CENTER_AND_PASSED_POINTS(), mycircleType);
  fillAttribute(centerPoint(), theCenterX, theCenterY);
  fillAttribute(passedPoint(), thePassedX, thePassedY);

  execute();
}

//==================================================================================================
void SketchAPI_MacroCircle::setByCenterAndPassedPoints(
    const std::shared_ptr<GeomAPI_Pnt2d>& theCenterPoint,
    const std::shared_ptr<GeomAPI_Pnt2d>& thePassedPoint)
{
  fillAttribute(SketchPlugin_MacroCircle::CIRCLE_TYPE_BY_CENTER_AND_PASSED_POINTS(), mycircleType);
  fillAttribute(theCenterPoint, mycenterPoint);
  fillAttribute(thePassedPoint, mypassedPoint);

  execute();
}

//==================================================================================================
void SketchAPI_MacroCircle::setByThreePoints(double theX1, double theY1,
                                             double theX2, double theY2,
                                             double theX3, double theY3)
{
  fillAttribute(SketchPlugin_MacroCircle::CIRCLE_TYPE_BY_THREE_POINTS(), mycircleType);
  fillAttribute(firstPoint(), theX1, theY1);
  fillAttribute(secondPoint(), theX2, theY2);
  fillAttribute(thirdPoint(), theX3, theY3);

  execute();
}

//==================================================================================================
void SketchAPI_MacroCircle::setByThreePoints(const std::shared_ptr<GeomAPI_Pnt2d>& thePoint1,
                                             const std::shared_ptr<GeomAPI_Pnt2d>& thePoint2,
                                             const std::shared_ptr<GeomAPI_Pnt2d>& thePoint3)
{
  fillAttribute(SketchPlugin_MacroCircle::CIRCLE_TYPE_BY_THREE_POINTS(), mycircleType);
  fillAttribute(thePoint1, myfirstPoint);
  fillAttribute(thePoint2, mysecondPoint);
  fillAttribute(thePoint3, mythirdPoint);

  execute();
}

//==================================================================================================
void SketchAPI_MacroCircle::setCenterPoint(double theX, double theY)
{
  fillAttribute(SketchPlugin_MacroCircle::CIRCLE_TYPE_BY_CENTER_AND_PASSED_POINTS(), mycircleType);
  fillAttribute(centerPoint(), theX, theY);

  execute();
}

//==================================================================================================
void SketchAPI_MacroCircle::setCenterPoint(const std::shared_ptr<GeomAPI_Pnt2d>& theCenterPoint)
{
  fillAttribute(SketchPlugin_MacroCircle::CIRCLE_TYPE_BY_CENTER_AND_PASSED_POINTS(), mycircleType);
  fillAttribute(theCenterPoint, mycenterPoint);

  execute();
}

//==================================================================================================
void SketchAPI_MacroCircle::setPassedPoint(double theX, double theY)
{
  fillAttribute(SketchPlugin_MacroCircle::CIRCLE_TYPE_BY_CENTER_AND_PASSED_POINTS(), mycircleType);
  fillAttribute(passedPoint(), theX, theY);

  execute();
}

//==================================================================================================
void SketchAPI_MacroCircle::setPassedPoint(const std::shared_ptr<GeomAPI_Pnt2d>& thePassedPoint)
{
  fillAttribute(SketchPlugin_MacroCircle::CIRCLE_TYPE_BY_CENTER_AND_PASSED_POINTS(), mycircleType);
  fillAttribute(thePassedPoint, mypassedPoint);

  execute();
}

//==================================================================================================
void SketchAPI_MacroCircle::setFirstPoint(double theX, double theY)
{
  fillAttribute(SketchPlugin_MacroCircle::CIRCLE_TYPE_BY_THREE_POINTS(), mycircleType);
  fillAttribute(firstPoint(), theX, theY);

  execute();
}

//==================================================================================================
void SketchAPI_MacroCircle::setFirstPoint(const std::shared_ptr<GeomAPI_Pnt2d>& thePoint)
{
  fillAttribute(SketchPlugin_MacroCircle::CIRCLE_TYPE_BY_THREE_POINTS(), mycircleType);
  fillAttribute(thePoint, myfirstPoint);

  execute();
}

//==================================================================================================
void SketchAPI_MacroCircle::setSecondPoint(double theX, double theY)
{
  fillAttribute(SketchPlugin_MacroCircle::CIRCLE_TYPE_BY_THREE_POINTS(), mycircleType);
  fillAttribute(secondPoint(), theX, theY);

  execute();
}

//==================================================================================================
void SketchAPI_MacroCircle::setSecondPoint(const std::shared_ptr<GeomAPI_Pnt2d>& thePoint)
{
  fillAttribute(SketchPlugin_MacroCircle::CIRCLE_TYPE_BY_THREE_POINTS(), mycircleType);
  fillAttribute(thePoint, mysecondPoint);

  execute();
}

//==================================================================================================
void SketchAPI_MacroCircle::setThirdPoint(double theX, double theY)
{
  fillAttribute(SketchPlugin_MacroCircle::CIRCLE_TYPE_BY_THREE_POINTS(), mycircleType);
  fillAttribute(thirdPoint(), theX, theY);

  execute();
}

//==================================================================================================
void SketchAPI_MacroCircle::setThirdPoint(const std::shared_ptr<GeomAPI_Pnt2d>& thePoint)
{
  fillAttribute(SketchPlugin_MacroCircle::CIRCLE_TYPE_BY_THREE_POINTS(), mycircleType);
  fillAttribute(thePoint, mythirdPoint);

  execute();
}
