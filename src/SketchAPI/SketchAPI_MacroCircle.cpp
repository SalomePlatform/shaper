// Copyright (C) 2014-2017  CEA/DEN, EDF R&D
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
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
//
// See http://www.salome-platform.org/ or
// email : webmaster.salome@opencascade.com<mailto:webmaster.salome@opencascade.com>
//

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
