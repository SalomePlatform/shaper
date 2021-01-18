// Copyright (C) 2014-2020  CEA/DEN, EDF R&D
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

#include "SketchAPI_MacroRectangle.h"
#include "SketchAPI_Rectangle.h"

#include <GeomAPI_Pnt2d.h>

#include <ModelHighAPI_Double.h>
#include <ModelHighAPI_Dumper.h>
#include <ModelHighAPI_Selection.h>
#include <ModelHighAPI_Tools.h>

//==================================================================================================
SketchAPI_MacroRectangle::SketchAPI_MacroRectangle(const std::shared_ptr<ModelAPI_Feature>& theFeature)
  : SketchAPI_SketchEntity(theFeature)
{
  initialize();
}

//==================================================================================================
SketchAPI_MacroRectangle::SketchAPI_MacroRectangle(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                                   double theStartX,
                                                   double theStartY,
                                                   double theSecondX,
                                                   double theSecondY, bool isFirstPointCenter):
  SketchAPI_SketchEntity(theFeature)
{
  if(initialize()) {
    if(isFirstPointCenter)
      setByCenterAndEndPoints(theStartX, theStartY, theSecondX, theSecondY);
    else
      setByStartAndEndPoints(theStartX, theStartY, theSecondX, theSecondY);
  }
}

//==================================================================================================
SketchAPI_MacroRectangle::SketchAPI_MacroRectangle(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                                   const std::shared_ptr<GeomAPI_Pnt2d>& theStartPoint,
                                                   const std::shared_ptr<GeomAPI_Pnt2d>& theSecondPoint, bool isFirstPointCenter):
  SketchAPI_SketchEntity(theFeature)
{
  if(initialize()) {
    if(isFirstPointCenter)
      setByCenterAndEndPoints(theStartPoint, theSecondPoint);
    else
      setByStartAndEndPoints(theStartPoint, theSecondPoint);
  }
}

//==================================================================================================
SketchAPI_MacroRectangle::~SketchAPI_MacroRectangle()
{
}

//==================================================================================================
void SketchAPI_MacroRectangle::setByStartAndEndPoints(double theStartX, double theStartY,
                                                      double theEndX, double theEndY)
{
  fillAttribute(SketchPlugin_MacroRectangle::START_END_POINT_TYPE_ID(), rectangleType());
  fillAttribute(startPoint1(), theStartX, theStartY);
  fillAttribute(endPoint1(), theEndX, theEndY);
  execute();
}

//==================================================================================================
void SketchAPI_MacroRectangle::setByStartAndEndPoints(const std::shared_ptr<GeomAPI_Pnt2d>& theStartPoint,
                                                      const std::shared_ptr<GeomAPI_Pnt2d>& theEndPoint)
{
  fillAttribute(SketchPlugin_MacroRectangle::START_END_POINT_TYPE_ID(), rectangleType());
  fillAttribute(theStartPoint, startPoint1());
  fillAttribute(theEndPoint, endPoint1());

  execute();
}

//==================================================================================================
void SketchAPI_MacroRectangle::setByCenterAndEndPoints(double theCenterX, double theCenterY, double theEndX, double theEndY)
{
  fillAttribute(SketchPlugin_MacroRectangle::CENTER_END_POINT_TYPE_ID(), rectangleType());
  fillAttribute(endPoint2(), theEndX, theEndY);
  fillAttribute(centerPoint(), theCenterX, theCenterY);
  execute();
}

//==================================================================================================
void SketchAPI_MacroRectangle::setByCenterAndEndPoints(const std::shared_ptr<GeomAPI_Pnt2d>& theCenterPoint, const std::shared_ptr<GeomAPI_Pnt2d>& theEndPoint){
  fillAttribute(SketchPlugin_MacroRectangle::CENTER_END_POINT_TYPE_ID(), rectangleType());
  fillAttribute(theEndPoint, endPoint2());
  fillAttribute(theCenterPoint, centerPoint());

  execute();
}

//==================================================================================================
/*
void SketchAPI_MacroRectangle::dump(ModelHighAPI_Dumper& theDumper) const
{
  FeaturePtr aBase = feature();

  std::shared_ptr<SketchPlugin_MacroRectangle> myRectangle = std::dynamic_pointer_cast<SketchPlugin_MacroRectangle>(aBase);
  if(!myRectangle)
    return;

  if (isCopy())
    return; // no need to dump copied feature

  const std::string& aSketchName = theDumper.parentName(aBase);

  AttributeSelectionPtr anExternal = aBase->selection(SketchPlugin_SketchEntity::EXTERNAL_ID());
  if (anExternal->context()) {
    // rectangle is external
    theDumper << aBase << " = " << aSketchName << ".addRectangle(" << anExternal << ")" << std::endl;
  } else {
    theDumper << aBase << " = " << aSketchName << ".addRectangle(";

    if(myRectangle->TYPE_ID() == SketchPlugin_MacroRectangle::START_CENTER_POINT_TYPE_ID())
      // rectangle given by start and center points
      theDumper  << startPoint2() << ", " << centerPoint() << ", 1)" << std::endl;
    else
      // rectangle given by start and end points
      theDumper  << startPoint1() << ", " << endPoint1() << ", 0)" << std::endl;
  }
  // dump "auxiliary" flag if necessary
  SketchAPI_SketchEntity::dump(theDumper);
}
*/
