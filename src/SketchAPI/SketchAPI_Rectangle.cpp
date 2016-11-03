// Copyright (C) 2014-20xx CEA/DEN, EDF R&D
// Name   : SketchAPI_Rectangle.cpp
// Purpose: 
//
// History:
// 17/06/16 - Sergey POKHODENKO - Creation of the file

//--------------------------------------------------------------------------------------
#include "SketchAPI_Rectangle.h"
//--------------------------------------------------------------------------------------
#include <GeomAPI_Pnt2d.h>
//--------------------------------------------------------------------------------------
#include <ModelHighAPI_Selection.h>
#include <ModelHighAPI_Tools.h>
//--------------------------------------------------------------------------------------
SketchAPI_Rectangle::SketchAPI_Rectangle(
    const std::shared_ptr<ModelAPI_Feature> & theFeature)
: SketchAPI_SketchEntity(theFeature)
{
  initialize();
}

SketchAPI_Rectangle::SketchAPI_Rectangle(
    const std::shared_ptr<ModelAPI_Feature> & theFeature,
    double theX1, double theY1, double theX2, double theY2)
: SketchAPI_SketchEntity(theFeature)
{
  if (initialize()) {
    setByCoordinates(theX1, theY1, theX2, theY2);
  }
}

SketchAPI_Rectangle::SketchAPI_Rectangle(
    const std::shared_ptr<ModelAPI_Feature> & theFeature,
    const std::shared_ptr<GeomAPI_Pnt2d> & theStartPoint,
    const std::shared_ptr<GeomAPI_Pnt2d> & theEndPoint)
: SketchAPI_SketchEntity(theFeature)
{
  if (initialize()) {
    setByPoints(theStartPoint, theEndPoint);
  }
}

SketchAPI_Rectangle::~SketchAPI_Rectangle()
{
}

//--------------------------------------------------------------------------------------
void SketchAPI_Rectangle::setByCoordinates(
    double theX1, double theY1, double theX2, double theY2)
{
  fillAttribute(startPoint(), theX1, theY1);
  fillAttribute(endPoint(), theX2, theY2);

  execute();
}

void SketchAPI_Rectangle::setByPoints(
    const std::shared_ptr<GeomAPI_Pnt2d> & theStartPoint,
    const std::shared_ptr<GeomAPI_Pnt2d> & theEndPoint)
{
  fillAttribute(theStartPoint, startPoint());
  fillAttribute(theEndPoint, endPoint());

  execute();
}

//--------------------------------------------------------------------------------------

