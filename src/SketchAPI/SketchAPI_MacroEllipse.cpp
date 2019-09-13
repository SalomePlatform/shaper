// Copyright (C) 2014-2019  CEA/DEN, EDF R&D
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

#include "SketchAPI_MacroEllipse.h"

#include <GeomAPI_Pnt2d.h>

#include <ModelHighAPI_RefAttr.h>
#include <ModelHighAPI_Tools.h>

SketchAPI_MacroEllipse::SketchAPI_MacroEllipse(const std::shared_ptr<ModelAPI_Feature>& theFeature)
: SketchAPI_SketchEntity(theFeature)
{
  initialize();
}

SketchAPI_MacroEllipse::SketchAPI_MacroEllipse(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                               double theX1, double theY1,
                                               double theX2, double theY2,
                                               double theX3, double theY3,
                                               bool byCenter)
  : SketchAPI_SketchEntity(theFeature)
{
  if (initialize()) {
    if (byCenter)
      setByCenterAndPassedPoints();
    else
      setByMajorAxisAndPassedPoint();

    initializePoints(theX1, theY1, theX2, theY2, theX3, theY3);
  }
}

SketchAPI_MacroEllipse::SketchAPI_MacroEllipse(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                               const std::shared_ptr<GeomAPI_Pnt2d>& thePoint1,
                                               const std::shared_ptr<GeomAPI_Pnt2d>& thePoint2,
                                               const std::shared_ptr<GeomAPI_Pnt2d>& thePoint3,
                                               bool byCenter)
  : SketchAPI_SketchEntity(theFeature)
{
  if (initialize()) {
    if (byCenter)
      setByCenterAndPassedPoints();
    else
      setByMajorAxisAndPassedPoint();

    initializePoints(thePoint1, thePoint2, thePoint3);
  }
}

SketchAPI_MacroEllipse::SketchAPI_MacroEllipse(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                               const std::shared_ptr<GeomAPI_Pnt2d>& thePoint1,
                                               const ModelHighAPI_RefAttr&           thePoint1Ref,
                                               const std::shared_ptr<GeomAPI_Pnt2d>& thePoint2,
                                               const ModelHighAPI_RefAttr&           thePoint2Ref,
                                               const std::shared_ptr<GeomAPI_Pnt2d>& thePoint3,
                                               const ModelHighAPI_RefAttr&           thePoint3Ref,
                                               bool byCenter)
  : SketchAPI_SketchEntity(theFeature)
{
  if (initialize()) {
    if (byCenter)
      setByCenterAndPassedPoints();
    else
      setByMajorAxisAndPassedPoint();

    initializePoints(thePoint1, thePoint1Ref, thePoint2, thePoint2Ref, thePoint3, thePoint3Ref);
  }
}

SketchAPI_MacroEllipse::~SketchAPI_MacroEllipse()
{
}

void SketchAPI_MacroEllipse::setByCenterAndPassedPoints()
{
  fillAttribute(SketchPlugin_MacroEllipse::ELLIPSE_TYPE_BY_CENTER_AXIS_POINT(), myellipseType);
}

void SketchAPI_MacroEllipse::setByMajorAxisAndPassedPoint()
{
  fillAttribute(SketchPlugin_MacroEllipse::ELLIPSE_TYPE_BY_AXIS_AND_POINT(), myellipseType);
}

void SketchAPI_MacroEllipse::initializePoints(double theX1, double theY1,
                                              double theX2, double theY2,
                                              double theX3, double theY3)
{
  fillAttribute(majorAxisNegativePoint(), theX1, theY1);
  fillAttribute(majorAxisPositivePoint(), theX2, theY2);
  fillAttribute(passedPoint(), theX3, theY3);

  execute();
}

void SketchAPI_MacroEllipse::initializePoints(const std::shared_ptr<GeomAPI_Pnt2d>& thePoint1,
                                              const std::shared_ptr<GeomAPI_Pnt2d>& thePoint2,
                                              const std::shared_ptr<GeomAPI_Pnt2d>& thePoint3)
{
  fillAttribute(thePoint1, mymajorAxisNegativePoint);
  fillAttribute(thePoint2, mymajorAxisPositivePoint);
  fillAttribute(thePoint3, mypassedPoint);

  execute();
}

static void fillAttribute(const std::shared_ptr<GeomAPI_Pnt2d>& thePoint,
                          const ModelHighAPI_RefAttr& thePointRef,
                          std::shared_ptr<GeomDataAPI_Point2D> thePointAttr,
                          AttributeRefAttrPtr thePointRefAttr)
{
  GeomPnt2dPtr aPoint = thePoint;
  if (!thePointRef.isEmpty()) {
    fillAttribute(thePointRef, thePointRefAttr);
    std::shared_ptr<GeomDataAPI_Point2D> anAttrPnt =
        std::dynamic_pointer_cast<GeomDataAPI_Point2D>(thePointRefAttr->attr());
    if (anAttrPnt)
      aPoint = anAttrPnt->pnt();
  }
  fillAttribute(aPoint, thePointAttr);
}

void SketchAPI_MacroEllipse::initializePoints(
    const std::shared_ptr<GeomAPI_Pnt2d>& theMajorAxisPoint1,
    const ModelHighAPI_RefAttr&           theMajorAxisPoint1Ref,
    const std::shared_ptr<GeomAPI_Pnt2d>& theMajorAxisPoint2,
    const ModelHighAPI_RefAttr&           theMajorAxisPoint2Ref,
    const std::shared_ptr<GeomAPI_Pnt2d>& thePassedPoint,
    const ModelHighAPI_RefAttr&           thePassedPointRef)
{
  fillAttribute(theMajorAxisPoint1, theMajorAxisPoint1Ref,
                mymajorAxisNegativePoint, mymajorAxisNegativePointRef);
  fillAttribute(theMajorAxisPoint2, theMajorAxisPoint2Ref,
                mymajorAxisPositivePoint, mymajorAxisPositivePointRef);
  fillAttribute(thePassedPoint, thePassedPointRef, mypassedPoint, mypassedPointRef);

  execute();
}
