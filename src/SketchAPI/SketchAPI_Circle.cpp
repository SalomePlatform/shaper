// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:        SketchAPI_Circle.cpp
// Created:     09 June 2016
// Author:      Dmitry Bobylev

#include "SketchAPI_Circle.h"

#include <GeomAPI_Pnt2d.h>

#include <ModelHighAPI_Double.h>
#include <ModelHighAPI_Dumper.h>
#include <ModelHighAPI_Selection.h>
#include <ModelHighAPI_Tools.h>

//==================================================================================================
SketchAPI_Circle::SketchAPI_Circle(const std::shared_ptr<ModelAPI_Feature> & theFeature)
: SketchAPI_SketchEntity(theFeature)
{
  initialize();
}

//==================================================================================================
SketchAPI_Circle::SketchAPI_Circle(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                   double theCenterX,
                                   double theCenterY,
                                   double theRadius)
: SketchAPI_SketchEntity(theFeature)
{
  if(initialize()) {
    setByCenterAndRadius(theCenterX, theCenterY, theRadius);
  }
}

//==================================================================================================
SketchAPI_Circle::SketchAPI_Circle(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                   const std::shared_ptr<GeomAPI_Pnt2d>& theCenter,
                                   double theRadius)
: SketchAPI_SketchEntity(theFeature)
{
  if(initialize()) {
    setByCenterAndRadius(theCenter, theRadius);
  }
}

//==================================================================================================
SketchAPI_Circle::SketchAPI_Circle(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                   double theX1, double theY1,
                                   double theX2, double theY2,
                                   double theX3, double theY3)
: SketchAPI_SketchEntity(theFeature)
{
  if (initialize()) {
    setByThreePoints(theX1, theY1, theX2, theY2, theX3, theY3);
  }
}

//==================================================================================================
SketchAPI_Circle::SketchAPI_Circle(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                   const std::shared_ptr<GeomAPI_Pnt2d>& thePoint1,
                                   const std::shared_ptr<GeomAPI_Pnt2d>& thePoint2,
                                   const std::shared_ptr<GeomAPI_Pnt2d>& thePoint3)
: SketchAPI_SketchEntity(theFeature)
{
  if (initialize()) {
    setByThreePoints(thePoint1, thePoint2, thePoint3);
  }
}

//==================================================================================================
SketchAPI_Circle::SketchAPI_Circle(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                   const ModelHighAPI_Selection& theExternal)
: SketchAPI_SketchEntity(theFeature)
{
  if (initialize()) {
    setByExternal(theExternal);
  }
}

//==================================================================================================
SketchAPI_Circle::SketchAPI_Circle(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                   const std::string& theExternalName)
: SketchAPI_SketchEntity(theFeature)
{
  if (initialize()) {
    setByExternalName(theExternalName);
  }
}

//==================================================================================================
SketchAPI_Circle::~SketchAPI_Circle()
{

}

//==================================================================================================
void SketchAPI_Circle::setByCenterAndRadius(double theCenterX, double theCenterY, double theRadius)
{
  fillAttribute(SketchPlugin_Circle::CIRCLE_TYPE_CENTER_AND_RADIUS(), mycircleType);
  fillAttribute(center(), theCenterX, theCenterY);
  fillAttribute(theRadius, myradius);

  execute();
}

//==================================================================================================
void SketchAPI_Circle::setByCenterAndRadius(const std::shared_ptr<GeomAPI_Pnt2d>& theCenter,
                                            double theRadius)
{
  fillAttribute(SketchPlugin_Circle::CIRCLE_TYPE_CENTER_AND_RADIUS(), mycircleType);
  fillAttribute(theCenter, mycenter);
  fillAttribute(theRadius, myradius);

  execute();
}

//==================================================================================================
void SketchAPI_Circle::setByThreePoints(double theX1, double theY1,
                                        double theX2, double theY2,
                                        double theX3, double theY3)
{
  fillAttribute(SketchPlugin_Circle::CIRCLE_TYPE_THREE_POINTS(), mycircleType);
  fillAttribute(firstPoint(), theX1, theY1);
  fillAttribute(secondPoint(), theX2, theY2);
  fillAttribute(thirdPoint(), theX3, theY3);

  execute();
}

//==================================================================================================
void SketchAPI_Circle::setByThreePoints(const std::shared_ptr<GeomAPI_Pnt2d>& thePoint1,
                                        const std::shared_ptr<GeomAPI_Pnt2d>& thePoint2,
                                        const std::shared_ptr<GeomAPI_Pnt2d>& thePoint3)
{
  fillAttribute(SketchPlugin_Circle::CIRCLE_TYPE_THREE_POINTS(), mycircleType);
  fillAttribute(thePoint1, myfirstPoint);
  fillAttribute(thePoint2, mysecondPoint);
  fillAttribute(thePoint3, mythirdPoint);

  execute();
}

//==================================================================================================
void SketchAPI_Circle::setByExternal(const ModelHighAPI_Selection & theExternal)
{
  fillAttribute(theExternal, external());

  execute();
}

//==================================================================================================
void SketchAPI_Circle::setByExternalName(const std::string & theExternalName)
{
  fillAttribute(ModelHighAPI_Selection("EDGE", theExternalName), external());

  execute();
}

//==================================================================================================
void SketchAPI_Circle::setCenter(double theX, double theY)
{
  fillAttribute(SketchPlugin_Circle::CIRCLE_TYPE_CENTER_AND_RADIUS(), mycircleType);
  fillAttribute(center(), theX, theY);

  execute();
}

//==================================================================================================
void SketchAPI_Circle::setCenter(const std::shared_ptr<GeomAPI_Pnt2d> & theCenter)
{
  fillAttribute(SketchPlugin_Circle::CIRCLE_TYPE_CENTER_AND_RADIUS(), mycircleType);
  fillAttribute(theCenter, mycenter);

  execute();
}

//==================================================================================================
void SketchAPI_Circle::setRadius(double theRadius)
{
  fillAttribute(SketchPlugin_Circle::CIRCLE_TYPE_CENTER_AND_RADIUS(), mycircleType);
  fillAttribute(ModelHighAPI_Double(theRadius), myradius);

  execute();
}

//==================================================================================================
void SketchAPI_Circle::setFirstPoint(double theX, double theY)
{
  fillAttribute(SketchPlugin_Circle::CIRCLE_TYPE_THREE_POINTS(), mycircleType);
  fillAttribute(firstPoint(), theX, theY);

  execute();
}

//==================================================================================================
void SketchAPI_Circle::setFirstPoint(const std::shared_ptr<GeomAPI_Pnt2d>& thePoint)
{
  fillAttribute(SketchPlugin_Circle::CIRCLE_TYPE_THREE_POINTS(), mycircleType);
  fillAttribute(thePoint, myfirstPoint);

  execute();
}

//==================================================================================================
void SketchAPI_Circle::setSecondPoint(double theX, double theY)
{
  fillAttribute(SketchPlugin_Circle::CIRCLE_TYPE_THREE_POINTS(), mycircleType);
  fillAttribute(secondPoint(), theX, theY);

  execute();
}

//==================================================================================================
void SketchAPI_Circle::setSecondPoint(const std::shared_ptr<GeomAPI_Pnt2d>& thePoint)
{
  fillAttribute(SketchPlugin_Circle::CIRCLE_TYPE_THREE_POINTS(), mycircleType);
  fillAttribute(thePoint, mysecondPoint);

  execute();
}

//==================================================================================================
void SketchAPI_Circle::setThirdPoint(double theX, double theY)
{
  fillAttribute(SketchPlugin_Circle::CIRCLE_TYPE_THREE_POINTS(), mycircleType);
  fillAttribute(thirdPoint(), theX, theY);

  execute();
}

//==================================================================================================
void SketchAPI_Circle::setThirdPoint(const std::shared_ptr<GeomAPI_Pnt2d>& thePoint)
{
  fillAttribute(SketchPlugin_Circle::CIRCLE_TYPE_THREE_POINTS(), mycircleType);
  fillAttribute(thePoint, mythirdPoint);

  execute();
}

//==================================================================================================
void SketchAPI_Circle::dump(ModelHighAPI_Dumper& theDumper) const
{
  if (isCopy())
    return; // no need to dump copied feature

  FeaturePtr aBase = feature();
  const std::string& aSketchName = theDumper.parentName(aBase);

  AttributeSelectionPtr anExternal = aBase->selection(SketchPlugin_SketchEntity::EXTERNAL_ID());
  if (anExternal->context()) {
    // circle is external
    theDumper << aBase << " = " << aSketchName << ".addCircle(" << anExternal << ")" << std::endl;
  } else {
    AttributeStringPtr aType = circleType();
    if (aType->value() == SketchPlugin_Circle::CIRCLE_TYPE_CENTER_AND_RADIUS()) {
      // circle given by center and radius
      theDumper << aBase << " = " << aSketchName << ".addCircle("
                << center() << ", " << radius() << ")" << std::endl;
    } else {
      // circle given by three points
      theDumper << aBase << " = " << aSketchName << ".addCircle(" << firstPoint() << ", "
                << secondPoint() << ", " << thirdPoint() << ")" << std::endl;
    }
  }
  // dump "auxiliary" flag if necessary
  SketchAPI_SketchEntity::dump(theDumper);
}
